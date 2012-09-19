#include "stdafx.h"
#include "UpdateGameSvr.h"
#include "../../../include/tablestruct.h"
#include "../../../include/Utility/utility.h"
#include "../../../include/Container/SyncContainer.hpp"
#include "../../../include/irtdatasvr.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/Utility/ScopeGuard.hpp"
#include "../../../include/Serialize/Serialize.hpp"

#include "idxfile.h"
#include <process.h>
#include <shlwapi.h>
#include <string>
#include <psapi.h>
#include <time.h>
#include <MSTcpIP.h>

#pragma comment(lib, "psapi.lib")

#define DEFAULT_BLOCK_SIZE	0xA0000


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

void CXmlDatabase::AddUpdateGame(DWORD ip, DWORD gid, DWORD type, DWORD speed, DWORD progress, __int64 uptbyte, __int64 leftbyte, LPCWSTR tid, LPCTSTR svrDir, LPCTSTR nodeDir)
{
	if (ip == 0 || gid == 0)
		return ;
	AutoLock lock(m_lock);
	MSXML2::IXMLDOMElementPtr updateinfo = m_xmlMemDb->selectSingleNode(TEXT("/database/updateinfo"));
	if (!updateinfo)
	{
		updateinfo = m_xmlMemDb->createElement(TEXT("updateinfo"));
		m_xmlMemDb->documentElement->appendChild(updateinfo);
	}

	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/updateinfo/task[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMElementPtr task = m_xmlMemDb->selectSingleNode(path);
	if (!task)
	{
		task = m_xmlMemDb->createElement(TEXT("task"));
		updateinfo->appendChild(task);
	}
	task->setAttribute(TEXT("ip"),			(long)ip);
	task->setAttribute(TEXT("type"),		type);
	task->setAttribute(TEXT("gid"),			gid);
	task->setAttribute(TEXT("speed"),		speed);
	task->setAttribute(TEXT("progress"),	progress);
	task->setAttribute(TEXT("uptsize"),		uptbyte);
	task->setAttribute(TEXT("leftsize"),	leftbyte);
	task->setAttribute(TEXT("lasttime"),	time(NULL));

	task->setAttribute(TEXT("tid"),	tid);
	task->setAttribute(_T("serverDir"), svrDir);
	task->setAttribute(_T("nodeDir"), nodeDir);
}

void CXmlDatabase::RemoveUpdateGame(DWORD ip)
{
	AutoLock lock(m_lock);
	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/updateinfo/task[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMNodePtr task = m_xmlMemDb->selectSingleNode(path);
	if (task)
		task->parentNode->removeChild(task);
}

void CXmlDatabase::AddDownloadGame(DWORD gid, DWORD ip, DWORD version, LPCWSTR dir)
{
	AutoLock lock(m_lock);
	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/gameinfo/game[@gid=\"%u\"]"), gid);
	MSXML2::IXMLDOMElementPtr game = m_XmlLocDb->selectSingleNode(path);
	if (!game)
	{
		MSXML2::IXMLDOMElementPtr element = m_XmlLocDb->createElement(TEXT("game"));
		m_XmlLocDb->selectSingleNode(TEXT("/database/gameinfo"))->appendChild(element);
		element->setAttribute(TEXT("gid"), gid);
		game = element;
	}

	_stprintf_s(path, TEXT("./item[@ip=\"%d\"]"), ip);
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

char* CXmlDatabase::GetUpdateGame(DWORD type)
{
	AutoLock lock(m_lock);
	MSXML2::IXMLDOMElementPtr status   = m_xmlMemDb->createElement(TEXT("status"));
	MSXML2::IXMLDOMElementPtr download = m_xmlMemDb->createElement(TEXT("download"));
	status->appendChild(download);

	bool bremove = false;
	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/updateinfo/task[@type=\"%d\"]"), type);
	MSXML2::IXMLDOMNodeListPtr nodelist = m_xmlMemDb->selectNodes(path);

	int nPos = 0;
	int nNodeLen= nodelist->Getlength();
	int nBufLen = nNodeLen *  sizeof(i8desk::db::tUpdateGameStatus) + sizeof(int);

	char* pData = (char*)CoTaskMemAlloc(nBufLen);
	if(pData == NULL)
		return NULL;

	ZeroMemory(pData, nBufLen);
	CopyMemory(pData, &nNodeLen, sizeof(nNodeLen));
	nPos += sizeof(nNodeLen);

	for (long idx=0; idx<nodelist->Getlength(); idx++)
	{
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);

		if (time(NULL) - (long)node->getAttribute(TEXT("lasttime")) < 5)
		{
			i8desk::db::tUpdateGameStatus ugs;

			ugs.ClientIP = (long)node->getAttribute(TEXT("ip"));
			ugs.GID = (uint32)node->getAttribute(TEXT("gid"));
			ugs.SizeLeft = (uint64)node->getAttribute(TEXT("leftsize"));
			utility::Strcpy(ugs.TID, node->getAttribute(TEXT("tid")).bstrVal);
			ugs.TransferRate = (DWORD)node->getAttribute(TEXT("speed"));
			ugs.Type = (DWORD)node->getAttribute(TEXT("type"));
			ugs.UpdateSize = (uint64)node->getAttribute(TEXT("uptsize"));
			utility::Strcpy(ugs.SvrDir, node->getAttribute(TEXT("serverDir")).bstrVal);
			utility::Strcpy(ugs.NodeDir, node->getAttribute(TEXT("nodeDir")).bstrVal);
			ugs.UpdateState = 1;

			/*CHAR szPrint[1024] = {0};
			sprintf(szPrint, "\nClientIP = %d------GID=%d  SizeLeft=%I64d  UpdateSize=%I64d", 
			ugs.ClientIP, ugs.GID, ugs.SizeLeft, ugs.UpdateSize);
			OutputDebugStringA(szPrint);*/

			MSXML2::IXMLDOMElementPtr task = m_xmlMemDb->createElement(TEXT("task"));
			task->setAttribute(TEXT("ip"),		ugs.ClientIP);
			task->setAttribute(TEXT("type"),	ugs.Type);
			task->setAttribute(TEXT("gid"),		ugs.GID);
			task->setAttribute(TEXT("speed"),	ugs.TransferRate);
			task->setAttribute(TEXT("progress"),node->getAttribute(TEXT("progress")));
			task->setAttribute(TEXT("uptsize"),	ugs.UpdateSize);
			task->setAttribute(TEXT("leftsize"),ugs.SizeLeft);
			task->setAttribute(TEXT("tid"),     ugs.TID);
			task->setAttribute(TEXT("serverDir"),	 ugs.SvrDir);
			task->setAttribute(TEXT("nodeDir"),	ugs.NodeDir);

			download->appendChild(task);

			CopyMemory(pData + nPos, &ugs, sizeof(ugs));

			/*OutputDebugStringA(szPrint);*/
			nPos += sizeof(ugs);
		}
		else 
		{ bremove = true; }
	}
	if (bremove)
	{
		m_xmlMemDb->documentElement->removeChild(m_xmlMemDb->selectSingleNode(TEXT("/database/updateinfo")));
		_bstr_t str = m_xmlMemDb->documentElement->text;
		OutputDebugStringA("find have client need remove.");
	}

	return pData;
}


BYTE* CXmlDatabase::GetIpHaveGame(DWORD ip)
{
	AutoLock lock(m_lock);

	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/gameinfo/game/item[@ip=\"%d\"]"), ip);
	MSXML2::IXMLDOMNodeListPtr nodelist = m_XmlLocDb->selectNodes(path);

	int nPos = 0;
	int nNodeLen= nodelist->Getlength();
	int nBufLen = nNodeLen * sizeof(i8desk::db::tViewGame) + sizeof(int);

	BYTE* pData = (BYTE*)CoTaskMemAlloc(nBufLen);
	if(pData == NULL)
		return NULL;

	ZeroMemory(pData, nBufLen);
	CopyMemory(pData, &nNodeLen, sizeof(nNodeLen));
	nPos += sizeof(nNodeLen);

	for (long idx = 0; idx<nodelist->Getlength(); idx++)
	{
		i8desk::db::tViewGame Item;
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);
		MSXML2::IXMLDOMElementPtr parent = node->GetparentNode();

		Item.GID = (uint32)parent->getAttribute(TEXT("gid"));
		Item.UpdateTime = (uint32)node->getAttribute(TEXT("version"));


		CopyMemory(pData + nPos, &Item, sizeof(Item));
		nPos += sizeof(Item);
	}

	return pData;
}

BYTE* CXmlDatabase::GetGameInMachine(DWORD gid)
{
	AutoLock lock(m_lock);

	TCHAR path[MAX_PATH] = {0};
	_stprintf_s(path, TEXT("/database/gameinfo/game[@gid=\"%u\"]/item"), gid);
	MSXML2::IXMLDOMNodeListPtr nodelist = m_XmlLocDb->selectNodes(path);

	int nPos = 0;
	int nNodeLen= nodelist->Getlength();
	int nBufLen = nNodeLen *  sizeof(i8desk::db::tViewClient) + sizeof(int);

	BYTE* pData = (BYTE*)CoTaskMemAlloc(nBufLen);
	if(pData == NULL)
		return NULL;

	ZeroMemory(pData, nBufLen);
	CopyMemory(pData, &nNodeLen, sizeof(nNodeLen));
	nPos += sizeof(nNodeLen);

	for (long idx=0; idx<nodelist->Getlength(); idx++)
	{
		i8desk::db::tViewClient Item;
		MSXML2::IXMLDOMElementPtr node = nodelist->Getitem(idx);

		Item.Ip = (uint32)node->getAttribute(TEXT("ip"));
		Item.UpdateTime = (uint32)node->getAttribute(TEXT("version"));

		CopyMemory(pData + nPos, &Item, sizeof(Item));
		nPos += sizeof(Item);
	}

	return pData;
}
void CXmlDatabase::SaveLocXmlDb()
{
	AutoLock lock(m_lock);
	TCHAR szXml[MAX_PATH] = {0};
	GetModuleFileName(NULL, szXml, _countof(szXml));
	PathRemoveFileSpec(szXml);
	PathAddBackslash(szXml);
	lstrcat(szXml, TEXT("Data\\gameinfo.xml"));
	m_XmlLocDb->save(szXml);
}

template<typename T, typename PointedBy>
struct CloseSocket
{
	void operator()(T sck)
	{
		::closesocket(sck);
	}
};

template<typename T, typename PointedBy>
struct CloseSocketHandle
{
	void operator()(T handle)
	{
		::WSACloseEvent(handle);
	}
};

template<typename T, typename PointedBy>
struct FreeVirtualBuffer
{
	void operator()(T buf)
	{
		::VirtualFree(buf, 0, MEM_RELEASE);
	}
};



typedef utility::CSmartHandle<SOCKET, CloseSocket, utility::CEmptyClass, INVALID_SOCKET> AutoSocket;
typedef utility::CSmartHandle<HANDLE, CloseSocketHandle> AutoSocketEvent;
typedef utility::CSmartHandle<char *, FreeVirtualBuffer> AutoVirtualBuffer;

struct Param
{
	CUpdateSvrPlug*			pPlug;
	AutoSocket				sck;
	DWORD					ip;
	HANDLE					exit_;
	utility::CAutoHandle	thread_;
};


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

bool CUpdateSvrPlug::Initialize(ISvrPlugMgr* pPlugMgr)
{
	WriteLog(TEXT("Initialize UpdateServer Plug ..."));
	m_pIPlugMgr		= pPlugMgr;
	m_pXmlDb		= new CXmlDatabase;
	m_pSyncServer	= new CSyncServer(pPlugMgr);

	if( pPlugMgr != 0 )
	{
		ISvrPlug* RTDbPlug = pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
		if(RTDbPlug == NULL)
			return false;
		rtSvr_ = (IRTDataSvr*)RTDbPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0);
	}

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
	WriteLog(TEXT("Initialize UpdateServer Plug Finish."));
	return (m_hThread != NULL);
}

bool CUpdateSvrPlug::UnInitialize()
{
	WriteLog(TEXT("UnInitialize UpdateServer Plug ..."));
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
	WriteLog(TEXT("UnInitialize UpdateServer Plug Success."));
	Release_Interface(m_pILogger);
	return true;
}


DWORD CUpdateSvrPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	stdex::tString result;
	switch (cmd)
	{
	case CMD_UPT_SM_GETUPDATGAME:
		{
			return reinterpret_cast<DWORD>(m_pXmlDb->GetUpdateGame(param1));
		}
		break;
	case CMD_UPT_SM_GETSYNCGAME:
		break;
	case CMD_UPT_SM_IPHAVEGAME:
		return reinterpret_cast<DWORD>(m_pXmlDb->GetIpHaveGame(param1));
		break;
	case CMD_UPT_SM_GAMEINIP:
		return reinterpret_cast<DWORD>( m_pXmlDb->GetGameInMachine(param1));
		break;
	case CMD_UPT_SM_NOTIFYSYNC:
		{
			WriteLog(TEXT("recv frame notify sync game:%d:%d"), param1, param2);

			return (m_pSyncServer != NULL) ? m_pSyncServer->NotifySyncGame(param1, param2 == 1) : 0;
		}
	case CMD_UPT_SM_NOTIFYDELETE:
		{
			WriteLog(TEXT("recv frame notify delete game:%d:%d"), param1, param2);

			return m_pSyncServer != NULL ? m_pSyncServer->NotifyDeleteGame(param1) : 0;
		}
	case CMD_UPT_SM_OPERATE:
		{
			long gid = 0;
			stdex::tString SID, taskName;
			bool hasDeleteFiles = false;

			char *buf = (char *)(param2);
			utility::serialize::Serialize os(buf);
			os >> gid >> SID >> taskName >> hasDeleteFiles;

			switch(param1)
			{
			case StartTask:
				m_pSyncServer->StartSync(SID, taskName);
				break;
			case StopTask:
				m_pSyncServer->StopSync(SID, taskName);
				break;
			case SuspendTask:
				m_pSyncServer->SuspendTask(SID, taskName);
				break;
			case ResumeTask:
				m_pSyncServer->ResumeTask(SID, taskName);
				break;
			case AddGame:
				m_pSyncServer->AddGame(SID, gid, taskName);
				break;
			case DeleteGame:
				m_pSyncServer->DeleteGame(SID, gid, taskName, hasDeleteFiles);
				break;
			case PutTopGame:
				break;
			case PutBottonGame:
				break;
			case NotifySync:
				m_pSyncServer->NotifyGame(SID, gid, taskName, true);
				break;
			default:
				break;

			}
		}
		
		break;
	default:
		return 0;
	}
	LPTSTR pData = (LPTSTR)CoTaskMemAlloc((result.size()+1) * sizeof(TCHAR));
	lstrcpy(pData, result.c_str());
	return reinterpret_cast<DWORD>(pData);
}

bool RecvPackage(SOCKET sck, pkgheader* pheader, HANDLE hExited)
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
			if (nRecvBytes >= sizeof(pkgheader))
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

bool SendNBytes(SOCKET sck, const char* pData, int length, HANDLE hEvent)
{
	int left = length;
	while (left)
	{
		if( ::WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0 )
			return false;

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
	return true;
}


typedef async::container::SyncAssocContainer<HANDLE, HANDLE> WorkerThread;
WorkerThread threads;


UINT32 __stdcall CUpdateSvrPlug::ListenThreadProc(LPVOID lpVoid)
{
	CUpdateSvrPlug* pThis = reinterpret_cast<CUpdateSvrPlug*>(lpVoid);
	SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);
	if (sck == INVALID_SOCKET)
		return 0;

	async::thread::AutoEvent exit;
	exit.Create(0, TRUE, FALSE);

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
				pParam->exit_ = exit;

				HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, pParam, 0, NULL);

				pParam->thread_ = hThread;
				threads.insert(hThread, hThread);
			}
		}
		else if (ret == WAIT_TIMEOUT)
		{
			if (pThis->m_pServer != NULL && !pThis->m_pServer->IsConnected())
				pThis->m_pServer->ConnectServer(GetMainSvrIp().c_str());
		}
		else break;
	}

	exit.SetEvent();

	struct OP
	{
		void operator()(std::pair<HANDLE, HANDLE> val)
		{
			::Sleep(500);
			::TerminateThread(val.first, 0);
		}
	};
	threads.for_each(OP());

	closesocket(sck);
	WSACloseEvent(hAccept);
	return 0;
}


UINT32 __stdcall CUpdateSvrPlug::WorkThreadProc(LPVOID lpVoid)
{
	std::auto_ptr<Param> param(reinterpret_cast<Param*>(lpVoid));

	struct Scope
	{
		void Run(CXmlDatabase* pXmlDb, CSocket* svr_, long gid, DWORD clientIP, LPCWSTR dir)
		{
			if (gid)
			{
				pXmlDb->RemoveUpdateGame(clientIP);
				if (svr_ != NULL)
				{
					svr_->ReportUpdateFinished(gid, 0, dir);
				}
			}
		}
	}scope;

	try 
	{
		
		wchar_t			g_dir[MAX_PATH] = {0};
		wchar_t *dir = g_dir;
		CUpdateSvrPlug* pThis	= param->pPlug;
		CXmlDatabase*   pXmlDb	= pThis->m_pXmlDb;
		SOCKET			sck	= param->sck;
		const DWORD		&peerip	= param->ip;
		DWORD			clientip= 0;
		DWORD			gid		= 0;
		utility::CAutoFile	hFile;

		utility::CAutoHandle hEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
		AutoVirtualBuffer pfilebuf		= (char*)::VirtualAlloc(NULL, DEFAULT_BLOCK_SIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		char 		psckbuf[4096]		= {0};
		pkgheader * pheader				= (pkgheader*)psckbuf;

		async::ScopeGuard<void> scope(std::tr1::bind(&Scope::Run, scope, 
			pXmlDb, pThis->m_pServer, std::tr1::ref(gid), std::tr1::ref(clientip), std::tr1::ref(dir)));
		
		while (1)
		{
			if( ::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0 )
				break;

			if (!RecvPackage(sck, pheader, pThis->m_hExited))
				break;

			switch (pheader->Cmd)
			{
			case CMD_SYNC_GETALLTASK:
				{
					CPkgHelper in(psckbuf);

					stdex::tString name;
					u_long ip = 0;
					bool first = false;

					try
					{
						in >> ip >> name >> first;
					}
					catch(...)
					{
						pThis->WriteLog(TEXT("解析同步服务器请求更新游戏出错"));
						break;
					}


					SyncGamesInfo needUpdateGames = pThis->m_pSyncServer->GetSyncGame(::htonl(ip), first);
					const size_t len = needUpdateGames.size() * sizeof(SyncGameInfo) + 64;
					std::vector<char> buf(len);
					CPkgHelper out(&buf[0], len);
					out << ERROR_SUCCESS << needUpdateGames.size();

					for(SyncGamesInfo::const_iterator iter = needUpdateGames.begin(); iter != needUpdateGames.end(); ++iter)
					{
						std::wcout << iter->second->cliIP_ << std::endl;
						out << *(iter->second);
					}

					if (!SendNBytes(sck, out.GetBuffer(), out.GetLength(), pThis->m_hExited))
						return 0;
				}
				break;

			case CMD_SYNC_GETSTATUS:
				{
					CPkgHelper in(psckbuf);
					stdex::tString SID;
					in >> SID;

					bool isStarted = pThis->m_pSyncServer->IsStarted(SID);

					char buf[1024] = {0};
					CPkgHelper out(buf);
					out << isStarted;

					if (!SendNBytes(sck, out.GetBuffer(), out.GetLength(), pThis->m_hExited))
						return 0;
				}
				break;

			case CMD_SYNC_SETGMAEVER:
				{
					pThis->WriteLog(TEXT("Set Game Version:%d:%d"), 
						*(DWORD*)(&pheader->data[0]), *(DWORD*)(&pheader->data[4]));
					int len = pThis->m_pSyncServer->handle_SetGameVer(pheader, pfilebuf, DEFAULT_BLOCK_SIZE);
					if (!SendNBytes(sck, pfilebuf, len, pThis->m_hExited))
						return 0;
				}
				break;
			case CMD_UPT_GETGMAEINFO:
				{
					gid = *(DWORD*)(&pheader->data[0]);
					wchar_t file[MAX_PATH] = {0};
					swprintf_s(g_dir, L"%s", pheader->data + 8);
					swprintf_s(file, L"%si8desk.idx", g_dir);
					WIN32_FILE_ATTRIBUTE_DATA wfd = {0};
					BOOL bRet = GetFileAttributesExW(file, GetFileExInfoStandard, &wfd);
					char sendbuf[24] = {0};
					DWORD ret = ERROR_SUCCESS;
					if (!bRet || MAKEQWORD(wfd.nFileSizeLow, wfd.nFileSizeHigh) == 0)
					{
						pThis->WriteLog(TEXT("get idxfile:%s:%d:%d"), (LPCTSTR)_bstr_t(file), wfd.nFileSizeLow, GetLastError());
						ret = 1;
					}

					::SetLastError(0);
					DWORD suc = Idxfile_GetVersion(file);
					DWORD err = ::GetLastError();
					if( suc == -1L )
					{
						pThis->WriteLog(_T("获取索引失败 file: %s, err: %d"), (LPCTSTR)_bstr_t(file), err);
					}

					*(DWORD*)(&sendbuf[ 0]) = ret;
					*(DWORD*)(&sendbuf[ 4]) = suc;
					*(DWORD*)(&sendbuf[ 8])	= wfd.nFileSizeLow;
					*(DWORD*)(&sendbuf[12]) = wfd.nFileSizeHigh;
					*(DWORD*)(&sendbuf[16])	= wfd.ftLastWriteTime.dwLowDateTime;
					*(DWORD*)(&sendbuf[20]) = wfd.ftLastWriteTime.dwHighDateTime;
					if (!SendNBytes(sck, sendbuf, sizeof(sendbuf), pThis->m_hExited))
						return 0;
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

					if (bFirst || hFile.IsValid() )
					{
						if( !hFile.IsValid() )
						{
							hFile.CleanUp();
						}

						LARGE_INTEGER liSize;
						swprintf_s(file, L"%s%s", g_dir, pheader->data + 28);
						DWORD flag = (bUseNoBuf ? FILE_FLAG_NO_BUFFERING : 0) | 
							FILE_FLAG_OVERLAPPED|FILE_FLAG_SEQUENTIAL_SCAN;
						hFile = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, flag, NULL);
						if (hFile == INVALID_HANDLE_VALUE || !GetFileSizeEx(hFile, &liSize) ||
							liSize.QuadPart != liFileSize.QuadPart)
						{
							DWORD dwError = GetLastError();
							pThis->WriteLog(TEXT("open file fail:%s,err=%d,%I64d = %I64d"), 
								(LPCTSTR)_bstr_t(file), dwError, liFileSize.QuadPart, liSize.QuadPart);

							//fill error data,client will find block crc error.
							ZeroMemory(pfilebuf, dwBlockSize);
							if (!SendNBytes(sck, pfilebuf, dwBlockSize, pThis->m_hExited))
								return 0;
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
						pThis->WriteLog(TEXT("read block error:%d"), GetLastError());
						return 0;
					}
					if (!GetOverlappedResult(hFile, &ov, &dwReadBytes, TRUE))
					{
						pThis->WriteLog(TEXT("GetOverlappedResult fail:%d:%d:%d"), dwBlockSize, dwReadBytes, GetLastError());
						return 0;				
					}
					if (!SendNBytes(sck, pfilebuf, dwBlockSize, pThis->m_hExited))
					{
						pThis->WriteLog(TEXT("send block error:%d"), WSAGetLastError());
						return 0;
					}
				}
				break;
			case CMD_UPT_SYNCTASK_RPTPROGRESS:
				{
					db::tSyncTaskStatus status;

					CPkgHelper in(psckbuf);
					in >> clientip >> status;

					if (clientip == 0) 
						clientip = peerip;

					DWORD flag = 0;
					if( status.UpdateState == GameRunning || status.UpdateState == GamePaused )
						flag = ~0L;
					else if( status.UpdateState == GameError )
						flag = ~0L;
					else
						flag |= MASK_TSYNCTASKSTATUS_UPDATESTATE;

					param->pPlug->rtSvr_->GetSyncTaskStatusTable()->Ensure(&status, flag);

					DWORD ret = ERROR_SUCCESS;
					if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), pThis->m_hExited))
						return 0;
				}
				break;
			case CMD_UPT_RPTPROGRESS:
				{
					DWORD gid	= 0;
					DWORD speed = 0;
					DWORD progress			= 0;
					DWORD type	= 0;
					__int64 qNeedUptBytes	= 0;
					__int64 qLeftBytes		= 0;

					TCHAR  m_szTid[MAX_GUID_LEN] = {0};
					TCHAR svrDir[MAX_PATH]	= {0};
					TCHAR nodeDir[MAX_PATH] = {0};

					CPkgHelper in(psckbuf);
					in >> clientip >> gid >> speed >> progress >> qNeedUptBytes 
						>> qLeftBytes >> m_szTid >> type >> svrDir >> nodeDir;

					if (clientip == 0) 
						clientip = peerip;

					pXmlDb->AddUpdateGame(clientip, gid, type, speed, progress, qNeedUptBytes, qLeftBytes, m_szTid, svrDir, nodeDir);

					DWORD ret = ERROR_SUCCESS;
					if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), pThis->m_hExited))
						return 0;
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
					pThis->WriteLog(TEXT("CMD_UPT_RPTFINISH: GUID=%d  clientip=%d"), gid, clientip);

					DWORD ret = ERROR_SUCCESS;
					if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), pThis->m_hExited))
						return 0;
				}
				break;
			case CMD_UPT_INDEXERR:
				{
					gid = *(DWORD*)(&pheader->data[0]);
					ISvrPlug* pPlug = NULL;
					if (pThis->m_pIPlugMgr != NULL && (pPlug = pThis->m_pIPlugMgr->FindPlug(PLUG_GAMEMGR_NAME)) != NULL)
					{
						pThis->WriteLog(TEXT("notify 3up gid=%d force update."), gid);
						pPlug->SendMessage(CMD_UPT_SM_NOFIGY3UPFORCE, gid, 0);
					}

					DWORD ret = ERROR_SUCCESS;
					if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), pThis->m_hExited))
						return 0;
				}
				break;

			case CMD_UPT_SYNCGAME_ERROR:
				{
					stdex::tString SID;
					long gid;

					CPkgHelper in(psckbuf);
					in >> SID >> gid;

					ISvrPlug* pPlug = NULL;
					if (pThis->m_pIPlugMgr != NULL && (pPlug = pThis->m_pIPlugMgr->FindPlug(PLUG_GAMEMGR_NAME)) != NULL)
					{
						pThis->WriteLog(TEXT("通知三层强制更新游戏: %d"), gid);
						pPlug->SendMessage(CMD_UPT_SM_NOFIGY3UPFORCE, gid, 0);
					}

					DWORD ret = ERROR_SUCCESS;
					if (!SendNBytes(sck, (char*)&ret, sizeof(DWORD), pThis->m_hExited))
						return 0;
				}
				break;

			case CMD_GET_CONSOLE_IP:
				{
					ISvrPlug* pPlug = NULL;
					DWORD consoleIP = 0;
					if (pThis->m_pIPlugMgr != NULL && 
						(pPlug = pThis->m_pIPlugMgr->FindPlug(PLUG_RTDATASVR_NAME)) != NULL)
					{
						consoleIP = pPlug->SendMessage(RTDS_CMD_GET_CONSOLE_IP, 0, 0);
					}

					char buf[64] = {0};
					CPkgHelper out(buf);
					out << ERROR_SUCCESS << consoleIP;

					if (!SendNBytes(sck, out.GetBuffer(), out.GetLength(), pThis->m_hExited))
						return 0;
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
	} 
	catch (...)  
	{ }

	threads.erase(param->thread_);
	return 0;
}

void __cdecl CUpdateSvrPlug::WriteLog(LPCTSTR formater, ...)
{
	AutoLock lock(m_lockWriteLog);
	if (m_pILogger == NULL)
	{
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger** ppILogger);
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, _countof(szFile));
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Frame.dll"));
		HMODULE hMod = LoadLibrary(szFile);
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)GetProcAddress(hMod, MAKEINTRESOURCEA(5))))
		{
			pfnCreateLogger(&m_pILogger);
			if (m_pILogger != NULL)
			{
				m_pILogger->SetLogFileName(PLUG_UPTSVR_NAME);
				m_pILogger->SetAddDateBefore(false);
				m_pILogger->WriteLog(LM_INFO, TEXT("==============================================="));
				m_pILogger->SetAddDateBefore(true);
			}
		}
	}
	if (m_pILogger != NULL)
	{
		TCHAR szLog[4096] = {0};
		va_list marker;
		va_start(marker, formater);
		_vstprintf_s(szLog, formater, marker);
		va_end(marker);
		m_pILogger->WriteLog(LM_INFO, szLog);
	}
}