#include "stdafx.h"
#include "UpdateGameSvr.h"
#include "idxfile.h"
#include <process.h>
#include <shlwapi.h>
#include <string>
#include <psapi.h>
#include <time.h>
#include <MSTcpIP.h>

#pragma comment(lib, "psapi.lib")

#define DEFAULT_BLOCK_SIZE	0xA0000

typedef struct Param
{
	CUpdateSvrPlug* pPlug;
	SOCKET			sck;
	DWORD			ip;
}Param;

CXmlDatabase::CXmlDatabase()
{
	CoInitialize(NULL);
	
	//加载本地数据库
	TCHAR szXml[MAX_PATH] = {0};
	GetModuleFileName(NULL, szXml, _countof(szXml));
	PathRemoveFileSpec(szXml);
	PathAddBackslash(szXml);
	lstrcat(szXml, TEXT("Data\\gameinfo.xml"));
	bool bsave = false;
	m_XmlLocDb.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (VARIANT_FALSE == m_XmlLocDb->load(szXml) ||
		m_XmlLocDb->documentElement == NULL ||
		m_XmlLocDb->documentElement->nodeName != _bstr_t(TEXT("database")))
	{
		m_XmlLocDb->loadXML(TEXT("<database><gameinfo/></database>"));
		bsave = true;
	}
	if (!m_XmlLocDb->selectSingleNode(TEXT("/database/gameinfo")))
	{
		m_XmlLocDb->documentElement->appendChild(m_XmlLocDb->createElement(TEXT("gameinfo")));
		bsave = true;
	}
	if (bsave)
		SaveLocXmlDb();

	//加载内存数据库
	m_xmlMemDb.CreateInstance(__uuidof(MSXML2::DOMDocument));
	m_xmlMemDb->loadXML(TEXT("<database><updateinfo/></database>"));
}

CXmlDatabase::~CXmlDatabase()
{
	SaveLocXmlDb();
	m_XmlLocDb.Release();
	m_xmlMemDb.Release();
	CoUninitialize();
}

void CXmlDatabase::AddUpdateGame(DWORD ip, DWORD gid, DWORD speed, DWORD progress, __int64 uptbyte, __int64 leftbyte)
{
	if (ip == 0 || gid == 0)
		return ;
	CAutoLock<CLock> lock(&m_lock);
	MSXML2::IXMLDOMElementPtr updateinfo = m_xmlMemDb->selectSingleNode(TEXT("/database/updateinfo"));
	if (!updateinfo)
	{
		updateinfo = m_xmlMemDb->createElement(TEXT("updateinfo"));
		m_xmlMemDb->documentElement->appendChild(updateinfo);
	}

	TCHAR path[MAX_PATH] = {0};
	_stprintf(path, TEXT("/database/updateinfo/task[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMElementPtr task = m_xmlMemDb->selectSingleNode(path);
	if (!task)
	{
		task = m_xmlMemDb->createElement(TEXT("task"));
		updateinfo->appendChild(task);
	}
	task->setAttribute(TEXT("ip"),			(long)ip);
	task->setAttribute(TEXT("type"),		1);
	task->setAttribute(TEXT("gid"),			gid);
	task->setAttribute(TEXT("speed"),		speed);
	task->setAttribute(TEXT("progress"),	progress);
	task->setAttribute(TEXT("uptsize"),		uptbyte/1024);
	task->setAttribute(TEXT("leftsize"),	leftbyte/1024);
	task->setAttribute(TEXT("lasttime"),	time(NULL));
}

void CXmlDatabase::RemoveUpdateGame(DWORD ip)
{
	CAutoLock<CLock> lock(&m_lock);
	TCHAR path[MAX_PATH] = {0};
	_stprintf(path, TEXT("/database/updateinfo/task[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMNodePtr task = m_xmlMemDb->selectSingleNode(path);
	if (task)
		task->parentNode->removeChild(task);
}

void CXmlDatabase::AddDownloadGame(DWORD gid, DWORD ip, DWORD version, LPCWSTR dir)
{
	CAutoLock<CLock> lock(&m_lock);
	TCHAR path[MAX_PATH] = {0};
	_stprintf(path, TEXT("/database/gameinfo/game[@gid=\"%u\"]"), gid);
	MSXML2::IXMLDOMElementPtr game = m_XmlLocDb->selectSingleNode(path);
	if (!game)
	{
		MSXML2::IXMLDOMElementPtr element = m_XmlLocDb->createElement(TEXT("game"));
		m_XmlLocDb->selectSingleNode(TEXT("/database/gameinfo"))->appendChild(element);
		element->setAttribute(TEXT("gid"), gid);
		game = element;
	}

	_stprintf(path, TEXT("./item[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMElementPtr item = game->selectSingleNode(path);
	if (!item)
	{
		MSXML2::IXMLDOMElementPtr element = m_XmlLocDb->createElement(TEXT("item"));
		game->appendChild(element);
		item = element;
	}
	item->setAttribute(TEXT("ip"), (long)ip);
	item->setAttribute(TEXT("version"), version);
	item->setAttribute(TEXT("clidir"), dir);
	SaveLocXmlDb();
}

_bstr_t CXmlDatabase::GetUpdateGame()
{
	CAutoLock<CLock> lock(&m_lock);
	MSXML2::IXMLDOMElementPtr status   = m_xmlMemDb->createElement(TEXT("status"));
	MSXML2::IXMLDOMElementPtr download = m_xmlMemDb->createElement(TEXT("download"));
	status->appendChild(download);

	bool bremove = false;
	MSXML2::IXMLDOMNodeListPtr nodelist = m_xmlMemDb->selectNodes(TEXT("/database/updateinfo/task"));
	for (long idx=0; idx<nodelist->Getlength(); idx++)
	{
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);
		if (time(NULL) - (long)node->getAttribute(TEXT("lasttime")) < 5)
		{
			MSXML2::IXMLDOMElementPtr task = m_xmlMemDb->createElement(TEXT("task"));
			task->setAttribute(TEXT("ip"),		node->getAttribute(TEXT("ip")));
			task->setAttribute(TEXT("type"),	node->getAttribute(TEXT("type")));
			task->setAttribute(TEXT("gid"),		node->getAttribute(TEXT("gid")));
			task->setAttribute(TEXT("speed"),	node->getAttribute(TEXT("speed")));
			task->setAttribute(TEXT("progress"),node->getAttribute(TEXT("progress")));
			task->setAttribute(TEXT("uptsize"),	node->getAttribute(TEXT("uptsize")));
			task->setAttribute(TEXT("leftsize"),node->getAttribute(TEXT("leftsize")));
			download->appendChild(task);
		}
		else { bremove = true; }
	}
	if (bremove)
	{
		m_xmlMemDb->documentElement->removeChild(m_xmlMemDb->selectSingleNode(TEXT("/database/updateinfo")));
		_bstr_t str = m_xmlMemDb->documentElement->text;
		OutputDebugStringA("find have client need remove.");
	}
	return status->xml;
}

_bstr_t CXmlDatabase::GetIpHaveGame(DWORD ip)
{
	CAutoLock<CLock> lock(&m_lock);
	MSXML2::IXMLDOMNodePtr status = m_XmlLocDb->createElement(TEXT("status"));
	MSXML2::IXMLDOMNodePtr gameLst = m_XmlLocDb->createElement(TEXT("gameLst"));
	status->appendChild(gameLst);

	TCHAR path[MAX_PATH] = {0};
	_stprintf(path, TEXT("/database/gameinfo/game/item[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMNodeListPtr nodelist = m_XmlLocDb->selectNodes(path);
	for (long idx=0; idx<nodelist->Getlength(); idx++)
	{
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);
		MSXML2::IXMLDOMElementPtr parent = node->GetparentNode();
		MSXML2::IXMLDOMElementPtr task = m_XmlLocDb->createElement(TEXT("Task"));
		task->setAttribute(TEXT("gid"), parent->getAttribute(TEXT("gid")));
		task->setAttribute(TEXT("version"), node->getAttribute(TEXT("version")));
		gameLst->appendChild(task);
	}
	return status->xml;
}

_bstr_t CXmlDatabase::GetGameInMachine(DWORD gid)
{
	CAutoLock<CLock> lock(&m_lock);
	MSXML2::IXMLDOMNodePtr status = m_XmlLocDb->createElement(TEXT("status"));
	MSXML2::IXMLDOMNodePtr ipLst  = m_XmlLocDb->createElement(TEXT("ipLst"));
	status->appendChild(ipLst);

	TCHAR path[MAX_PATH] = {0};
	_stprintf(path, TEXT("/database/gameinfo/game[@gid=\"%u\"]/item"), gid);
	MSXML2::IXMLDOMNodeListPtr nodelist = m_XmlLocDb->selectNodes(path);
	for (long idx=0; idx<nodelist->Getlength(); idx++)
	{
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);
		MSXML2::IXMLDOMElementPtr task = m_XmlLocDb->createElement(TEXT("Task"));
		task->setAttribute(TEXT("ip"),		node->getAttribute(TEXT("ip")));
		task->setAttribute(TEXT("version"), node->getAttribute(TEXT("version")));
		ipLst->appendChild(task);
	}
	return status->xml;
}

void CXmlDatabase::SaveLocXmlDb()
{
	CAutoLock<CLock> lock(&m_lock);
	TCHAR szXml[MAX_PATH] = {0};
	GetModuleFileName(NULL, szXml, _countof(szXml));
	PathRemoveFileSpec(szXml);
	PathAddBackslash(szXml);
	lstrcat(szXml, TEXT("Data\\gameinfo.xml"));
	m_XmlLocDb->save(szXml);
}

CUpdateSvrPlug::CUpdateSvrPlug() :m_hExited(NULL), m_hThread(NULL), m_pIPlugMgr(NULL), 
	m_pXmlDb(NULL), m_pSyncServer(NULL), m_pServer(NULL), m_pILogger(NULL)
{
	WSAData data = {0};
	WSAStartup(0x0202, &data);
}

CUpdateSvrPlug::~CUpdateSvrPlug()
{
	WSACleanup();
}

bool CUpdateSvrPlug::Initialize(IPlugMgr* pPlugMgr)
{
	WriteLog("Initialize UpdateServer Plug ...");
	m_pIPlugMgr		= pPlugMgr;
	m_pXmlDb		= new CXmlDatabase;
	m_pSyncServer	= new CSyncServer(pPlugMgr);
	
	TCHAR szExe[MAX_PATH] = {0};
	GetModuleBaseName(GetCurrentProcess(), NULL, szExe, _countof(szExe));
	bool bIsMainServer = (_tcsicmp(szExe, TEXT("I8DeskSvr.exe" )) == 0);
	if (!bIsMainServer)
	{
		m_pServer = new CSocket;
		m_pServer->ConnectServer(GetMainSvrIp().c_str());
	}

	m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hExited == NULL)
		return false;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ListenThreadProc, this, 0, NULL);
	WriteLog("Initialize UpdateServer Plug Finish.");
	return (m_hThread != NULL);
}

bool CUpdateSvrPlug::UnInitialize()
{
	WriteLog("UnInitialize UpdateServer Plug ...");
	if (m_hExited != NULL)
	{
		if (m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		CloseHandle(m_hExited);
	}
	if (m_pServer != NULL)
	{
		m_pServer->CloseServer();
		Delete_Pointer(m_pServer);
	}
	m_pIPlugMgr = NULL;
	m_hExited = NULL;
	Delete_Pointer(m_pXmlDb);
	Delete_Pointer(m_pSyncServer);
	WriteLog("UnInitialize UpdateServer Plug Success.");
	Release_Interface(m_pILogger);
	return true;
}

DWORD CUpdateSvrPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	std::string result;
	switch (cmd)
	{
	case CMD_UPT_SM_GETUPDATGAME:
		result = m_pXmlDb->GetUpdateGame();
		break;
	case CMD_UPT_SM_GETSYNCGAME:
		break;
	case CMD_UPT_SM_IPHAVEGAME:
		result = m_pXmlDb->GetIpHaveGame(param1);
		break;
	case CMD_UPT_SM_GAMEINIP:
		result = m_pXmlDb->GetGameInMachine(param1);
		break;
	case CMD_UPT_SM_NOTIFYSYNC:
		{
			WriteLog("recv frame notify sync game:%d:%d", param1, param2);
			return (m_pSyncServer != NULL) ? m_pSyncServer->NotifySyncGame(param1, param2 == 1) : 0;
		}
	case CMD_UPT_SM_NOTIFYDELETE:
		{
			WriteLog("recv frame notify delete game:%d:%d", param1, param2);
			return m_pSyncServer != NULL ? m_pSyncServer->NotifyDeleteGame(param1) : 0;
		}
	default:
		return 0;
	}
	char* pData = (char*)CoTaskMemAlloc(result.size()+1);
	lstrcpyA(pData, result.c_str());
	return reinterpret_cast<DWORD>(pData);
}

bool RecvPackage(SOCKET sck, _packageheader* pheader, HANDLE hExited)
{
	char* pData = reinterpret_cast<char*>(pheader);
	int nRecvBytes = 0;
	while (1)
	{
		FD_SET fdset;
		timeval tv = {3, 0};
		FD_ZERO(&fdset);
		FD_SET(sck, &fdset);
		int ret = select(0, &fdset, NULL, NULL, &tv);
		if (ret <= 0)
		{
			if (ret < 0 || WAIT_OBJECT_0 == WaitForSingleObject(hExited, 0))
				return false;
			continue;
		}
		if (FD_ISSET(sck, &fdset))
		{
			int len = recv(sck, pData, 0x1000, 0);
			if (len <= 0)
				return false;
			nRecvBytes += len;
			pData	   += len;
			if (nRecvBytes >= sizeof(_packageheader))
			{
				if (pheader->StartFlag != START_FLAG)
					return false;

				if ((int)pheader->Length >= nRecvBytes)
					return true;
			}
		}
	}
	return true;
}

bool SendNBytes(SOCKET sck, char* pData, int length, HANDLE hEvent)
{
	int left = length;
	while (left)
	{
		int len = send(sck, &pData[length-left], left, 0);
		if (len <=0)
			return false;
		left -= len;
	}
	return true;

// 	while (length)
// 	{
// 		WSABUF buf = {length, pData};
// 		DWORD dwSendBytes = 0;
// 		DWORD dwFlags = 0;
// 		WSAOVERLAPPED ov = {0};
// 		ov.hEvent = hEvent;
// 		WSAResetEvent(hEvent);
// 
// 		if (SOCKET_ERROR == WSASend(sck, &buf, 1, &dwSendBytes, 0, &ov, NULL) && 
// 			WSA_IO_PENDING != WSAGetLastError())
// 			return false;
// 		
// 		if (WSA_WAIT_FAILED == WSAWaitForMultipleEvents(1, &hEvent, TRUE, INFINITE, TRUE))
// 			return false;
// 
// 		if (!WSAGetOverlappedResult(sck, &ov, &dwSendBytes, TRUE, &dwFlags))
// 			return false;
// 
// 		length -= dwSendBytes;
// 		pData  += dwSendBytes;
// 	}
// 	return true;
}

UINT32 __stdcall CUpdateSvrPlug::ListenThreadProc(LPVOID lpVoid)
{
	CUpdateSvrPlug* pThis = reinterpret_cast<CUpdateSvrPlug*>(lpVoid);
	SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);
	if (sck == INVALID_SOCKET)
		return 0;
	
	WSAEVENT hAccept = WSACreateEvent();
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(PORT_TCP_UPDATE);
	if (hAccept == NULL || 
		SOCKET_ERROR == bind(sck, (PSOCKADDR)&addr, sizeof(addr)) ||
		SOCKET_ERROR == listen(sck, 10) ||
		SOCKET_ERROR == WSAEventSelect(sck, hAccept, FD_ACCEPT|FD_CLOSE))
	{
		closesocket(sck);
		return 0;
	}
	HANDLE hEvents[2] = {pThis->m_hExited, hAccept };
	while (1)
	{
		DWORD ret = WSAWaitForMultipleEvents(_countof(hEvents), hEvents, FALSE,60*1000, FALSE);
		if (ret == WAIT_OBJECT_0 + 1)
		{
			WSANETWORKEVENTS NetworkEvents;
			WSAEnumNetworkEvents(sck, hAccept, &NetworkEvents);
			if(NetworkEvents.lNetworkEvents == FD_CLOSE && NetworkEvents.iErrorCode[FD_CLOSE_BIT] == 0)
				break;

			if(NetworkEvents.lNetworkEvents == FD_ACCEPT && NetworkEvents.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				int len = sizeof(addr);
				SOCKET client = accept(sck, (PSOCKADDR)(&addr), &len);
				if (client == INVALID_SOCKET)
					continue ;

// 				BOOL flg = 1;
// 				int RecvBuf = 1024*1024;
// 				setsockopt(client, SOL_SOCKET, TCP_NODELAY,	(char*)&flg,	 sizeof(BOOL));
// 				setsockopt(client, SOL_SOCKET, SO_RCVBUF,	(char*)&RecvBuf, sizeof(int) );
// 				setsockopt(client, SOL_SOCKET, SO_SNDBUF,	(char*)&RecvBuf, sizeof(int) );

				DWORD dwBytes = 0;
				tcp_keepalive sKA_Settings = {0}, sReturned = {0};
				sKA_Settings.onoff = 1;
				sKA_Settings.keepalivetime = 5000;
				sKA_Settings.keepaliveinterval = 3000;

				WSAIoctl(client, SIO_KEEPALIVE_VALS, &sKA_Settings, sizeof(sKA_Settings), 
					&sReturned, sizeof(sReturned), &dwBytes, NULL, NULL);

				linger li = {0};
				li.l_onoff = 1; 
				li.l_linger = 0;
				setsockopt(client, SOL_SOCKET, SO_LINGER,   (char*)&li,   sizeof(li));


				Param* pParam = new Param;
				pParam->pPlug = pThis;
				pParam->sck   = client;
				pParam->ip	  = addr.sin_addr.s_addr;
				HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, pParam, 0, NULL);
				if (hThread != NULL)
					CloseHandle(hThread);
			}
		}
		else if (ret == WAIT_TIMEOUT)
		{
			if (pThis->m_pServer != NULL && !pThis->m_pServer->IsConnected())
				pThis->m_pServer->ConnectServer(GetMainSvrIp().c_str());
		}
		else break;
	}
	closesocket(sck);
	WSACloseEvent(hAccept);
	return 0;
}

UINT32 __stdcall CUpdateSvrPlug::WorkThreadProc(LPVOID lpVoid)
{
	try {
	Param* param = reinterpret_cast<Param*>(lpVoid);
	wchar_t			g_dir[MAX_PATH] = {0};
	CUpdateSvrPlug* pThis	= param->pPlug;
	CXmlDatabase*   pXmlDb	= pThis->m_pXmlDb;
	SOCKET			sck		= param->sck;
	DWORD			peerip	= param->ip;
	DWORD			clientip= 0;
	DWORD			gid		= 0;
	HANDLE			hFile	= INVALID_HANDLE_VALUE;
	
	HANDLE		hEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE		hSendEvent	= WSACreateEvent();
	char*		pfilebuf	= (char*)VirtualAlloc(NULL, DEFAULT_BLOCK_SIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	char*		psckbuf		= new char[4096];
	_packageheader* pheader = (_packageheader*)psckbuf;
	
	while (1)
	{
		if (!RecvPackage(sck, pheader, pThis->m_hExited))
			break;
		switch (pheader->Cmd)
		{
		case CMD_SYNC_GETALLTASK:
			{
				int len = pThis->m_pSyncServer->handle_GetAllTask(pheader, pfilebuf);
				if (!SendNBytes(sck, pfilebuf, len, hSendEvent))
					goto do_exit;
			}
			break;
		case CMD_SYNC_GETTASKGAME:
			{
				int len = pThis->m_pSyncServer->handle_GetTaskGame(pheader, pfilebuf);
				if (!SendNBytes(sck, pfilebuf, len, hSendEvent))
					goto do_exit;
			}
			break;
		case CMD_SYNC_SETGMAEVER:
			{
				pThis->WriteLog("Set Game Version:%d:%d", 
					*(DWORD*)(&pheader->data[0]), *(DWORD*)(&pheader->data[4]));
				int len = pThis->m_pSyncServer->handle_SetGameVer(pheader, pfilebuf);
				if (!SendNBytes(sck, pfilebuf, len, hSendEvent))
					goto do_exit;
			}
			break;
		case CMD_UPT_GETGMAEINFO:
			{
				gid = *(DWORD*)(&pheader->data[0]);
				wchar_t file[MAX_PATH] = {0};
				swprintf(g_dir, L"%s", pheader->data + 8);
				swprintf(file, L"%si8desk.idx", g_dir);
				WIN32_FILE_ATTRIBUTE_DATA wfd = {0};
				BOOL bRet = GetFileAttributesExW(file, GetFileExInfoStandard, &wfd);
				char sendbuf[24] = {0};
				DWORD ret = ERROR_SUCCESS;
				if (!bRet || MAKEQWORD(wfd.nFileSizeLow, wfd.nFileSizeHigh) == 0)
				{
					pThis->WriteLog("get idxfile:%s:%d:%d", (LPCSTR)_bstr_t(file), wfd.nFileSizeLow, GetLastError());
					ret = 1;
				}
				*(DWORD*)(&sendbuf[ 0]) = ret;
				*(DWORD*)(&sendbuf[ 4]) = Idxfile_GetVersion(file);
				*(DWORD*)(&sendbuf[ 8])	= wfd.nFileSizeLow;
				*(DWORD*)(&sendbuf[12]) = wfd.nFileSizeHigh;
				*(DWORD*)(&sendbuf[16])	= wfd.ftLastWriteTime.dwLowDateTime;
				*(DWORD*)(&sendbuf[20]) = wfd.ftLastWriteTime.dwHighDateTime;
				if (!SendNBytes(sck, sendbuf, sizeof(sendbuf), hSendEvent))
				{
					goto do_exit;
				}
			}
			break;
		case CMD_UPT_RECVBLOCK:
			{
				wchar_t file[MAX_PATH]   = {0};
				LARGE_INTEGER liFileSize = {0};
				LARGE_INTEGER liOffset	 = {0};
				DWORD dwBlockSize		 = 0;
				DWORD dwReadBytes		 = 0;
				bool  bFirst			 = false;

				liFileSize.QuadPart = *(LONGLONG*)pheader->data;
				liOffset.QuadPart	= *(LONGLONG*)(&pheader->data[8]);
				dwBlockSize			= *(DWORD*)(&pheader->data[16]);
				bFirst				= *(DWORD*)(&pheader->data[20]) == 1;
				bool bUseNoBuf		= liFileSize.QuadPart >= 0x10000;

				if (bFirst || hFile == INVALID_HANDLE_VALUE)
				{
					if (hFile != INVALID_HANDLE_VALUE)
					{
						CloseHandle(hFile);
						hFile = INVALID_HANDLE_VALUE;
					}

					LARGE_INTEGER liSize;
					swprintf(file, L"%s%s", g_dir, pheader->data + 28);
					DWORD flag = (bUseNoBuf ? FILE_FLAG_NO_BUFFERING : 0) | 
						FILE_FLAG_OVERLAPPED|FILE_FLAG_SEQUENTIAL_SCAN;
					hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, flag, NULL);
					if (hFile == INVALID_HANDLE_VALUE || !GetFileSizeEx(hFile, &liSize) ||
						liSize.QuadPart != liFileSize.QuadPart)
					{
						DWORD dwError = GetLastError();
						pThis->WriteLog("open file fail:%s,err=%d,%I64d = %I64d", 
							(LPCSTR)_bstr_t(file), dwError, liFileSize.QuadPart, liSize.QuadPart);

						//fill error data,client will find block crc error.
						ZeroMemory(pfilebuf, dwBlockSize);
						if (!SendNBytes(sck, pfilebuf, dwBlockSize, hSendEvent))
							goto do_exit;
						break;
					}
				}
				OVERLAPPED ov	= {0};
				ov.Offset		= liOffset.LowPart;
				ov.OffsetHigh	= liOffset.HighPart;
				ov.hEvent		= hEvent;
				ResetEvent(hEvent);
				BOOL ret = ReadFile(hFile, pfilebuf, bUseNoBuf ? ALIGN_SIZE(dwBlockSize, 512) : dwBlockSize, 
					&dwReadBytes, &ov);
				if ((!ret && ERROR_IO_PENDING != GetLastError()))
				{
					pThis->WriteLog("read block error:%d", GetLastError());
					goto do_exit;
				}
				if (!GetOverlappedResult(hFile, &ov, &dwReadBytes, TRUE))
				{
					pThis->WriteLog("GetOverlappedResult fail:%d:%d:%d", dwBlockSize, dwReadBytes, GetLastError());
					goto do_exit;				
				}
				if (!SendNBytes(sck, pfilebuf, dwBlockSize, hSendEvent))
				{
					pThis->WriteLog("send block error:%d", WSAGetLastError());
					goto do_exit;
				}
			}
			break;
		case CMD_UPT_RPTPROGRESS:
			{
				clientip				= *(DWORD*)(&pheader->data[0]);
				gid						= *(DWORD*)(&pheader->data[4]);
				DWORD speed				= *(DWORD*)(&pheader->data[8]);;
				DWORD progress			= *(DWORD*)(&pheader->data[12]);;
				__int64 qNeedUptBytes	= *(__int64*)(&pheader->data[16]);
				__int64 qLeftBytes		= *(__int64*)(&pheader->data[24]);
				if (clientip == 0) clientip = peerip;
				pXmlDb->AddUpdateGame(clientip, gid, speed, progress, qNeedUptBytes, qLeftBytes);

				DWORD ret = ERROR_SUCCESS;
				if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), hSendEvent))
					goto do_exit;
			}
			break;
		case CMD_UPT_RPTFINISH:
			{
				clientip = *(DWORD*)(&pheader->data[0]);
				gid = *(DWORD*)(&pheader->data[4]);
				if (clientip == 0) clientip = peerip;
				DWORD ver = *(DWORD*)(&pheader->data[8]);
				if (ver == 0)
					pXmlDb->RemoveUpdateGame(clientip);
				else
					pXmlDb->AddDownloadGame(gid, clientip, ver, (LPCWSTR)(&pheader->data[16]));
				
				DWORD ret = ERROR_SUCCESS;
				if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), hSendEvent))
					goto do_exit;
			}
			break;
		case CMD_UPT_INDEXERR:
			{
				gid = *(DWORD*)(&pheader->data[0]);
				IPlug* pPlug = NULL;
				if (pThis->m_pIPlugMgr != NULL && (pPlug = pThis->m_pIPlugMgr->FindPlug(GAMEMGR_PLUG_NAME)) != NULL)
				{
					pThis->WriteLog("notify 3up gid=%d force update.", gid);
					pPlug->SendMessage(CMD_UPT_SM_NOFIGY3UPFORCE, gid, 0);
				}
				
				DWORD ret = ERROR_SUCCESS;
				if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), hSendEvent))
					goto do_exit;
			}
			break;
		}
		//从服务器更新状态命令包给主服务器的更新组件。
		if (pThis->m_pServer != NULL)
		{
			if (pheader->Cmd == CMD_UPT_RPTPROGRESS || pheader->Cmd == CMD_UPT_RPTFINISH ||
				pheader->Cmd == CMD_UPT_INDEXERR)
			{
				//修改客户机的ip.报给主服务器。
				 *(DWORD*)(&pheader->data[0]) = peerip;
				pThis->m_pServer->ConvertDataToMainServer(psckbuf, pheader->Length);
			}
		}
	}
do_exit:
	if (gid)
	{
		pXmlDb->RemoveUpdateGame(clientip);
		if (pThis->m_pServer != NULL)
		{
			pThis->m_pServer->ReportUpdateFinished(gid, 0, g_dir);
		}
	}
	closesocket(sck);
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	CloseHandle(hEvent);
	WSACloseEvent(hSendEvent);
	VirtualFree(pfilebuf, 0, MEM_RELEASE);
	delete []psckbuf;
	delete param;
	
	} catch (...)  { }
	return 0;
}

void __cdecl CUpdateSvrPlug::WriteLog(char* formater, ...)
{
	CAutoLock<CLock> lock(&m_lockWriteLog);
	if (m_pILogger == NULL)
	{
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger** ppILogger);
		char szFile[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, szFile, _countof(szFile));
		PathRemoveFileSpecA(szFile);
		PathAddBackslashA(szFile);
		lstrcatA(szFile, "DbEngine.dll");
		HMODULE hMod = LoadLibraryA(szFile);
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)GetProcAddress(hMod, MAKEINTRESOURCEA(3))))
		{
			pfnCreateLogger(&m_pILogger);
			if (m_pILogger != NULL)
			{
				m_pILogger->SetLogFileName(GAMEUPDATE_PLUG_NAME);
				m_pILogger->SetAddDateBefore(false);
				m_pILogger->WriteLog(LM_INFO, "===============================================");
				m_pILogger->SetAddDateBefore(true);
			}
		}
	}
	if (m_pILogger != NULL)
	{
		char szLog[4096] = {0};
		va_list marker;
		va_start(marker, formater);
		vsprintf(szLog, formater, marker);
		va_end(marker);
		m_pILogger->WriteLog(LM_INFO, szLog);
	}
}