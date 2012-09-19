#include "stdafx.h"
#include "SyncGame.h"
#include "UpdateGame.h"
#include "idxfile.h"
#include <process.h>
#include <shlwapi.h>
#include <iphlpapi.h>
#include <algorithm>

#pragma comment(lib, "shlwapi")
#pragma comment (lib,"Iphlpapi.lib")

#import "msxml3.dll"

#define CMD_NOTIFY_SYNC_GAME		0x01
#define CMD_NOTFIY_DELETE_GAME		0x02


//旧版本的界面交互命令
#define NET_CMD_INTERACT_GUI		0x0400
#define NET_CMD_SYNCLOG				0x0401
#define NET_CMD_GETGAMEINFO			0x0402
#define NET_CMD_GETCONNECTINFO		0x0403
#define NET_CMD_IMMUPDATEALLGAME	0x0404
#define NET_CMD_REFRESHGUI			0x0405
#define NET_CMD_ENDSYNCGAME			0x0406

std::string GetMainSvrIp()
{
	char szIp[MAX_PATH] = {0};
	char szIniFile[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szIniFile, _countof(szIniFile));
	PathRemoveFileSpecA(szIniFile);
	PathAddBackslashA(szIniFile);
	lstrcatA(szIniFile, "Data\\I8SyncSvr.ini");
	GetPrivateProfileStringA("System", "SvrIp", "127.0.0.1", szIp, _countof(szIp), szIniFile);
	return std::string(szIp);
}

void SplitString(std::string strSource, std::vector<std::string>& subs, char ch = '|')
{
	subs.clear();
	size_t first = 0;
	size_t second = 0;
	do
	{
		second = strSource.find_first_of(ch, first);
		if (second != std::string::npos)
		{
			subs.push_back(strSource.substr(first, second-first));
			first = second + 1;
		}
		else
		{
			if (first< strSource.size())
			{
				subs.push_back(strSource.substr(first).c_str());
			}
		}
	}while (second != std::string::npos);
}

CSyncServer::CSyncServer(IPlugMgr* pIPlugMgr) : m_pIPlugMgr(pIPlugMgr)
{
	WSAData data = {0};
	WSAStartup(0x0202, &data);
}

CSyncServer::~CSyncServer()
{
	WSACleanup();
}

DWORD CSyncServer::NotifySyncGame(DWORD gid, bool bForce)
{
	char package[1024] = {0};
	_packageheader* pheader = (_packageheader*)package;
	InitPkgheader(pheader, CMD_NOTIFY_SYNC_GAME);
	CPackageHelper sendpkg(package);
	sendpkg.pushDWORD(gid);
	sendpkg.pushDWORD(bForce ? 1 : 0);
	pheader->Length = sendpkg.GetOffset();
	SendUdpPkg(package, pheader->Length);
	return 0;
}

DWORD CSyncServer::NotifyDeleteGame(DWORD gid)
{
	char package[1024] = {0};
	_packageheader* pheader = (_packageheader*)package;
	InitPkgheader(pheader, CMD_NOTFIY_DELETE_GAME);
	CPackageHelper sendpkg(package);
	sendpkg.pushDWORD(gid);
	pheader->Length = sendpkg.GetOffset();
	SendUdpPkg(package, pheader->Length);
	return 0;
}

inline IRTDataSvr* GetIRTDataSvr(IPlugMgr* pPlugMgr)
{
	if (pPlugMgr == NULL)
		return NULL;

	IPlug* RTDbPlug = pPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
	if (RTDbPlug == NULL)
		return NULL;

	return (IRTDataSvr*)RTDbPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0);
}

int CSyncServer::handle_GetAllTask(_packageheader* pheader, char* pbuf)
{
	CPackageHelper inpackage((LPSTR)pheader);
	CPackageHelper outpackage((LPSTR)pbuf);

	
	IRTDataSvr *pRTDataSvr = GetIRTDataSvr(m_pIPlugMgr);
	ISyncTaskRecordset* pRecordset = NULL;
	if (pRTDataSvr != NULL)
		pRTDataSvr->GetSyncTaskTable()->Select(&pRecordset, 0, 0);

	outpackage.pushDWORD(ERROR_SUCCESS);
	if (pRecordset == NULL)
	{
		outpackage.pushDWORD(0);
	}
	else
	{
		outpackage.pushDWORD(pRecordset->GetCount());
		for(UINT i=0; i<pRecordset->GetCount();i++)
		{
			db::tSyncTask* pSyncTask = pRecordset->GetData(i);
			outpackage.pushString(pSyncTask->SID,		lstrlenA(pSyncTask->SID));
			outpackage.pushString(pSyncTask->Name,		lstrlenA(pSyncTask->Name));
			outpackage.pushDWORD(pSyncTask->SyncType);
			outpackage.pushString(pSyncTask->SyncIP,	lstrlenA(pSyncTask->SyncIP));
			outpackage.pushString(pSyncTask->VDiskIP,	lstrlenA(pSyncTask->VDiskIP));
			outpackage.pushString(pSyncTask->UpdateIP,	lstrlenA(pSyncTask->UpdateIP));
			outpackage.pushDWORD(pSyncTask->DestDrv);
			outpackage.pushDWORD(pSyncTask->BalanceType);
			outpackage.pushDWORD(pSyncTask->MaxSyncSpeed);
		}
		pRecordset->Release();
	}
	pheader->Length = outpackage.GetOffset();
	outpackage.pushPackageHeader(*pheader);
	return pheader->Length;
}

struct Filter : public IGameFilter
{
	bool bypass(const db::tGame *const d) 
	{
		return d->Status != 1; 
	}
};

struct Filter2 : public IGameAreaFilter
{
	Filter2(int runtype):m_Runtype(runtype)
	{
	}
	bool bypass(const db::tGameArea *const c)
	{
		return c->RunType != m_Runtype; 
	}

private:
	int m_Runtype;
};

int CSyncServer::handle_GetTaskGame(_packageheader* pheader, char* pbuf)
{
	CPackageHelper inpackage((LPSTR)pheader);
	CPackageHelper outpackage((LPSTR)pbuf);

	DWORD dwSyncType = 0;
	char Sid[40] = {0};
	
	inpackage.popString(Sid);
	dwSyncType = inpackage.popDWORD();
	
	outpackage.pushDWORD(ERROR_SUCCESS);
	IRTDataSvr *pRTDataSvr = GetIRTDataSvr(m_pIPlugMgr);
	if (pRTDataSvr == NULL)
	{
		outpackage.pushDWORD(0);
	}
	else
	{
		std::set<DWORD> setGame;
		std::set<DWORD> setSyncGame;
		switch(dwSyncType)
		{
		case stVDisk:
			{
				IGameAreaRecordset* pGameAreaRecordset = NULL;
				Filter2 filter(2);
				if(pRTDataSvr->GetGameAreaTable()->Select(&pGameAreaRecordset, &filter, 0) ==0 )
				{
					for(UINT i =0; i<pGameAreaRecordset->GetCount();i++)
					{
						setGame.insert(pGameAreaRecordset->GetData(i)->GID);
					}
					pGameAreaRecordset->Release();
				}
			}
			break;
		case stUpdate:
			{
				IGameAreaRecordset* pGameAreaRecordset = NULL;
				Filter2 filter(1);
				if(pRTDataSvr->GetGameAreaTable()->Select(&pGameAreaRecordset, &filter, 0) == 0)
				{
					for(UINT i =0; i<pGameAreaRecordset->GetCount();i++)
					{
						setGame.insert(pGameAreaRecordset->GetData(i)->GID);
					}
					pGameAreaRecordset->Release();
				}
			}
			break;
		case stCustom:
			{
				ISyncGameRecordset* pCustomRecordset = NULL;
				if(pRTDataSvr->GetSyncGameTable()->Select(&pCustomRecordset, Sid, 0) == 0)
				{
					for(UINT i =0; i<pCustomRecordset->GetCount();i++)
					{
						setGame.insert(pCustomRecordset->GetData(i)->GID);
					}
				}
				pCustomRecordset->Release();
			}
			break;
		}
		IGameRecordset* pRecordset = NULL;
		Filter filter;
		if(pRTDataSvr->GetGameTable()->Select(&pRecordset, &filter, 0) == 0)
		{
			if(dwSyncType == stCustom || dwSyncType == stUpdate || dwSyncType == stVDisk)
			{
				for(size_t i = 0; i<pRecordset->GetCount(); i++)
				{
					DWORD gid  = pRecordset->GetData(i)->GID;
					std::set<DWORD>::iterator it = setGame.find(gid);
					if(it != setGame.end())
					{
						setSyncGame.insert(gid);
					}
				}
			}
			else
			{
				for(size_t i = 0;i<pRecordset->GetCount();i++)
				{
					setSyncGame.insert(pRecordset->GetData(i)->GID);
				}
			}
		}
		pRecordset->Release();
		outpackage.pushDWORD(setSyncGame.size());
		for(std::set<DWORD>::iterator it = setSyncGame.begin(); it != setSyncGame.end(); it++)
		{
			db::tGame game = {0};
			if(pRTDataSvr->GetGameTable()->GetData(*it, &game)==0)
			{
				outpackage.pushDWORD(game.GID);
				outpackage.pushDWORD(game.Size);
				outpackage.pushDWORD(game.SvrVer);
				outpackage.pushString(game.Name, lstrlenA(game.Name));
				outpackage.pushString(game.SvrPath, lstrlenA(game.SvrPath));
			}
		}
	}
	pheader->Length = outpackage.GetOffset();
	outpackage.pushPackageHeader(*pheader);
	return pheader->Length;
}

int	CSyncServer::handle_SetGameVer(_packageheader* pheader, char* pbuf)
{
	CPackageHelper inpackage((LPSTR)pheader);
	CPackageHelper outpackage((LPSTR)pbuf);

	try
	{
		DWORD gid = inpackage.popDWORD();
		DWORD ver = inpackage.popDWORD();
		IRTDataSvr *pRTDataSvr = GetIRTDataSvr(m_pIPlugMgr);
		if (pRTDataSvr != NULL)
		{
			db::tGame game = {0};
			game.SvrVer = game.IdcVer = ver;
			pRTDataSvr->GetGameTable()->Update(gid, &game, MASK_TGAME_SVRVER|MASK_TGAME_IDCVER);
		}
	}
	catch (...) { }
	outpackage.pushDWORD(ERROR_SUCCESS);
	pheader->Length = outpackage.GetOffset();
	outpackage.pushPackageHeader(*pheader);
	return pheader->Length;
}

void CSyncServer::SendUdpPkg(char* pPackage, int length)
{
	std::map<DWORD, DWORD> SyncSvrList;
	{
		IRTDataSvr *pRTDataSvr = GetIRTDataSvr(m_pIPlugMgr);
		ISyncTaskRecordset* pRecordset = NULL;
		if (pRTDataSvr != NULL)
			pRTDataSvr->GetSyncTaskTable()->Select(&pRecordset, 0, 0);
		if (pRecordset != NULL)
		{
			for(UINT i=0; i<pRecordset->GetCount();i++)
			{
				db::tSyncTask* pSyncTask = pRecordset->GetData(i);
				DWORD ip = inet_addr(pSyncTask->SyncIP);
				if (ip != 0)
					SyncSvrList.insert(std::make_pair(ip, ip));
			}
			pRecordset->Release();
		}
	}
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
		return ;

	for (std::map<DWORD, DWORD>::iterator it = SyncSvrList.begin(); 
		it != SyncSvrList.end(); it++)
	{
		sockaddr_in server = {0};
		server.sin_family = AF_INET;
		server.sin_port   = htons(PORT_UDP_UPDATE);
		server.sin_addr.s_addr = it->first;
		int left = length;
		while (left)
		{
			int len = sendto(sck, pPackage+(length-left), left, 0, (PSOCKADDR)&server, sizeof(server));
			if (len > 0)
				left -= len;
			else
				break;
		}
	}
	closesocket(sck);
}

bool CSyncPlug::Initialize(IPlugMgr* pPlugMgr)
{
	m_pIPlugMgr = pPlugMgr;
	m_hExited   = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hNotifySyncAll = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (m_hExited == NULL || m_hNotifySyncAll == NULL)
		return false;

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_Threads.push_back(hThread);
	hThread = (HANDLE)_beginthreadex(NULL, 0, UdpThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_Threads.push_back(hThread);
	return true;
}

bool CSyncPlug::UnInitialize()
{
	if (m_hExited != NULL)
	{
		SetEvent(m_hExited);
		if (m_Threads.size())
		{
			WaitForMultipleObjects(m_Threads.size(), &m_Threads[0], TRUE, INFINITE);
			for (size_t idx=0; idx<m_Threads.size(); idx++)
				CloseHandle(m_Threads[idx]);
			m_Threads.clear();
		}
		CloseHandle(m_hExited);
		m_hExited = NULL;
	}
	if (m_hNotifySyncAll != NULL)
	{
		CloseHandle(m_hNotifySyncAll);
		m_hNotifySyncAll = NULL;
	}
	{
		CAutoLock<CLock> lock(&m_lockAllGame);
		for (std::map<DWORD, tagGameInfo*>::iterator it = m_mapAllGame.begin(); 
			it != m_mapAllGame.end(); it++)
		{
			delete it->second;
		}
		m_mapAllGame.clear();
	}
	Release_Interface(m_pILogger);
	return true;
}

DWORD CSyncPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	if (cmd == 1)
	{
		CAutoLock<CLock> lock(&m_lockAllGame);
		*(DWORD*)param2 = m_setDriver.size();
		char* p = reinterpret_cast<char*>(param1);
		std::string str;
		for (std::set<char>::iterator it = m_setDriver.begin(); it != m_setDriver.end(); it++)
		{
			*p++ = (*it);
		}
		*p = 0;
	}
	return 0;
}

UINT32 __stdcall CSyncPlug::WorkThread(LPVOID lpVoid)
{
	CSyncPlug* pThis = reinterpret_cast<CSyncPlug*>(lpVoid);

	DWORD gid = 0;
	bool  bForece = false;

	bool isAllSync = false;	// 是否等待30分钟扫描
	ulonglong waitTime = 30 * 60 * 10000000LL;
	HANDLE hNotifyEvent = ::CreateEvent(0, FALSE, FALSE, _T("i8SyncEvent"));

	FILETIME ftLastTime = {0};
	GetSystemTimeAsFileTime(&ftLastTime);
	while (WAIT_TIMEOUT == WaitForSingleObject(pThis->m_hExited, 1000))
	{
		//check connect.
		if (!pThis->m_Server.IsConnected())
			pThis->m_Server.ConnectServer(GetMainSvrIp().c_str(), NULL, PORT_TCP_UPDATE);

		if (pThis->m_Server.IsConnected())
		{
			//refresh sync list.
			{
				FILETIME ftCurTime = {0};
				GetSystemTimeAsFileTime(&ftCurTime);
				*(__int64*)&ftCurTime -= waitTime;
				if (WaitForSingleObject(pThis->m_hNotifySyncAll, 0) == WAIT_OBJECT_0 || 
					CompareFileTime(&ftCurTime, &ftLastTime) >= 0)
				{
					if (pThis->GetAllSyncTask(WaitForSingleObject(pThis->m_hNotifySyncAll, 0) != WAIT_OBJECT_0))
					{
						isAllSync = true;

						GetSystemTimeAsFileTime(&ftLastTime);
						ResetEvent(pThis->m_hNotifySyncAll);
					}
				}
			}

			//delete game.
			if (pThis->GetNextGame(gid, bForece, delete_game))
			{
				pThis->DeleteGame(gid);
				continue ;
			}
			
			//sync game.
			if (pThis->GetNextGame(gid, bForece, notify_game) || 
				pThis->GetNextGame(gid, bForece, sync_game))
			{
				pThis->SyncGame(gid, bForece);
				continue ;
			}

			// 完成后通知事件
			CAutoLock<CLock> lock(&(pThis->m_lockAllGame));
			if( isAllSync && pThis->m_mapAllGame.empty() )
			{
				::SetEvent(hNotifyEvent);

				HANDLE openEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("i8AckEvent"));
				if( openEvent != 0 )
				{
					DWORD ret = ::WaitForSingleObject(openEvent, waitTime);
					if( ret == WAIT_TIMEOUT )
					{
						pThis->WriteLog(_T("等待同步回复事件超时"));
					}
					else if( ret == WAIT_OBJECT_0 )
					{
						pThis->WriteLog(_T("等待同步回复时间成功"));
						isAllSync = false;
					}

					::CloseHandle(openEvent);
				}
				else
				{
					pThis->WriteLog(_T("打开同步回复事件失败"));
					isAllSync = false;
				}
			}
		}
	}

	::CloseHandle(hNotifyEvent);
	pThis->m_Server.CloseServer();
	return 0;
}

UINT32 __stdcall CSyncPlug::UdpThread(LPVOID lpVoid)
{
	CSyncPlug* pThis = reinterpret_cast<CSyncPlug*>(lpVoid);
	HANDLE hEvent = WSACreateEvent();
	if (hEvent == NULL)
		return 0;

	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(PORT_UDP_UPDATE);
	if (INVALID_SOCKET == sck || 
		SOCKET_ERROR == bind(sck, (PSOCKADDR)&addr, sizeof(addr)) || 
		SOCKET_ERROR == WSAEventSelect(sck, hEvent, FD_READ))
	{
		if (sck != INVALID_SOCKET)
			closesocket(sck);
		WSACloseEvent(hEvent);
		return 0;
	}

	while (1)
	{
		HANDLE hEvents[] = {pThis->m_hExited, hEvent};
		DWORD ret = WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, INFINITE);
		if (ret != WAIT_OBJECT_0 + 1)
			break;
		else
		{
			WSANETWORKEVENTS NetworkEvents;
			WSAEnumNetworkEvents(sck, hEvent, &NetworkEvents);
			if(NetworkEvents.lNetworkEvents == FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] == 0)
			{
				try
				{
					char buf[0x10000] = {0};
					_packageheader* pheader = reinterpret_cast<_packageheader*>(buf);

					int len = recv(sck, buf, sizeof(buf), 0);
					if (len < sizeof(_packageheader) || pheader->Length != len || pheader->StartFlag != START_FLAG)
						continue ;
					buf[len] = 0;

					CPackageHelper package(buf);
					switch (pheader->Cmd)
					{
					case CMD_NOTIFY_SYNC_GAME:
						{
							DWORD gid = package.popDWORD();
							bool  bForce = package.popDWORD() == 1;
							pThis->WriteLog("通知同步游戏:%d.", gid);
							pThis->AddGame(gid, bForce, notify_game);
						}
						break;
					case CMD_NOTFIY_DELETE_GAME:
						{
							DWORD gid = package.popDWORD();
							pThis->WriteLog("通知删除游戏:%d.", gid);
							pThis->AddGame(gid, false, delete_game);
						}
						break;

					//////////////////////////////////////////////////////////////////////////
					//兼容旧版本界面交互
					case NET_CMD_INTERACT_GUI:
						{
							pThis->RefreshUIData();
						}
						break;
					case NET_CMD_GETGAMEINFO:
						{
							char buf[0x10000] = {0};
							CPackageHelper outpackage(buf);

							CAutoLock<CLock> lock(&pThis->m_lockAllGame);
							outpackage.pushDWORD(pThis->m_mapAllGame.size());
							for (std::map<DWORD, tagGameInfo*>::iterator it = pThis->m_mapAllGame.begin();
								it != pThis->m_mapAllGame.end(); it++)
							{
								outpackage.pushDWORD(it->second->gid);
								outpackage.pushString(it->second->Name, lstrlenA(it->second->Name));
								outpackage.pushDWORD(it->second->needsync ? 1 : 0);
							}
							pheader->Length = outpackage.GetOffset();
							outpackage.pushPackageHeader(*pheader);
							pThis->SendUdp2UI(buf, pheader->Length);
						}
						break;
					case NET_CMD_GETCONNECTINFO:
						{
							char buf[1024] = {0};
							CPackageHelper outpackage(buf);
							std::string ip = GetMainSvrIp();
							outpackage.pushString(ip.c_str(), ip.size());
							outpackage.pushDWORD(pThis->m_Server.IsConnected() ? 1 : 0);
							pheader->Length = outpackage.GetOffset();
							outpackage.pushPackageHeader(*pheader);
							pThis->SendUdp2UI(buf, pheader->Length);
						}
						break;					
					case NET_CMD_IMMUPDATEALLGAME:
						{
							SetEvent(pThis->m_hNotifySyncAll);
						}
						break;
					}
				}
				catch (...) { pThis->WriteLog("Udp Thread Process Error."); }
			}
		}
	}
	closesocket(sck);
	WSACloseEvent(hEvent);
	return 0;
}

BOOL IsLocalMachine(DWORD  dwip)
{
	DWORD ip = dwip;
	if ((ip & 0xff) == 127)
		return TRUE;

	IP_ADAPTER_INFO info[16] = {0};
	DWORD dwSize = sizeof(info);
	if (ERROR_SUCCESS != GetAdaptersInfo(info, &dwSize))
		return TRUE;

	PIP_ADAPTER_INFO pAdapter = info;
	while (pAdapter != NULL)
	{
		PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
		while (pAddr != NULL)
		{
			DWORD dwIp = inet_addr(pAddr->IpAddress.String);
			if (dwIp == ip)
				return TRUE;
			pAddr = pAddr->Next;
		}
		pAdapter = pAdapter->Next;
	}
	return FALSE;
}

bool CSyncPlug::GetAllSyncTask(bool bNotifyUI)
{
	//clear history data.
	{
		CAutoLock<CLock> lock(&m_lockSync);
		for (std::map<DWORD, tagGameInfo*>::iterator it = m_mapAllGame.begin(); 
			it != m_mapAllGame.end(); it++)
		{
			delete it->second;
		}
		m_mapAllGame.clear();
		m_setDriver.clear();
	}
	
	//recv new data.
	char* pdata  = NULL;
	int   length = 0;
	if (!m_Server.GetAllSyncTask(pdata, length))
		return false;

	try
	{
		CPackageHelper inpackage(pdata);
		if (inpackage.popDWORD() != ERROR_SUCCESS)
		{
			delete[] pdata;
			return false;
		}

		BOOL bMainServer = IsLocalMachine(inet_addr(GetMainSvrIp().c_str()));
		int nTaskCount = inpackage.popDWORD();
		for (int idx=0; idx<nTaskCount; idx++)
		{
			db::tSyncTask task = {0};
			inpackage.popString(task.SID);
			inpackage.popString(task.Name);
			task.SyncType = inpackage.popDWORD();
			inpackage.popString(task.SyncIP);
			inpackage.popString(task.VDiskIP);
			inpackage.popString(task.UpdateIP);
			task.DestDrv = inpackage.popDWORD();
			task.BalanceType = inpackage.popDWORD();
			task.MaxSyncSpeed = inpackage.popDWORD();
			
			//如果同步ip不是自己，则不算是自己的同步任务。 (记得过虑一些全0的ip,有些虚拟设备会产生这个ip,被识破认为是自己了)
			if (!IsLocalMachine(inet_addr(task.SyncIP)) || inet_addr(task.SyncIP) == 0)
				continue; 

			//主服务上过滤掉镜像任务, 非主服务器过滤掉127的本地任务
			if ((bMainServer && task.SyncType == stMirror) || (!bMainServer && lstrcmpiA(task.SyncIP, "127.0.0.1") == 0))
				continue;
			char* pdata2 = NULL;
			int   length2 = 0;
			if (m_Server.GetSyncTaskInfo(task.SID, task.SyncType, pdata2, length2))
			{
				CPackageHelper inpackage(pdata2);
				if (inpackage.popDWORD() == ERROR_SUCCESS)
				{
					DWORD nCount = inpackage.popDWORD();
					for (DWORD idx=0; idx<nCount; idx++)
					{
						char path[MAX_PATH] = {0};
						tagGameInfo* pGame = new tagGameInfo;
						pGame->gid = inpackage.popDWORD();
						pGame->size = inpackage.popDWORD();
						pGame->version = inpackage.popDWORD();
						inpackage.popString(pGame->Name);
						inpackage.popString(pGame->SvrPath);
						PathAddBackslashA(pGame->SvrPath);
						lstrcpyA(pGame->CliPath, pGame->SvrPath);
						if (task.DestDrv != 0)
							pGame->CliPath[0] = task.DestDrv;
						lstrcpyA(pGame->CliIp, task.SyncIP);
						pGame->MaxSpeed		  = task.MaxSyncSpeed;
						
						bool bdel = false;
						m_lockAllGame.Lock();
						std::map<DWORD, tagGameInfo*>::iterator it = m_mapAllGame.find(pGame->gid);
						if (it == m_mapAllGame.end())
						{
							m_mapAllGame.insert(std::make_pair(pGame->gid, pGame));
						}
						else
						{
							lstrcatA(it->second->CliPath, "|");
							lstrcatA(it->second->CliPath, pGame->CliPath);
							bdel = true;
						}
						m_setDriver.insert(toupper(pGame->CliPath[0]));
						m_lockAllGame.UnLock();

						wchar_t idxfile[MAX_PATH] = {0};
						swprintf(idxfile, L"%sI8Desk.idx", (LPCWSTR)_bstr_t(pGame->CliPath));
						pGame->needsync = false;
						if (Idxfile_GetVersion(idxfile) != pGame->version)
						{
							AddGame(pGame->gid, false, sync_game);
							pGame->needsync = true;
						}

						if (bdel)
							delete pGame;
					}
				}
				delete[] pdata2;
			}
		}
	}
	catch (...) { WriteLog("Get Sync Game List Error."); }

	delete[] pdata;
	m_lockAllGame.Lock();
	DWORD dwGameCount = m_mapAllGame.size();
	m_lockAllGame.UnLock();

	CAutoLock<CLock> lock(&m_lockSync);
	//WriteLog("总共游戏:%d个, 需要同步的游戏数:%d个", dwGameCount, m_mapSync.size());
	if (bNotifyUI)
		NotifyUiRefresh();
	return true;
}

void CSyncPlug::AddGame(DWORD gid, bool bForce, GAMETYPE type)
{
	if (type == notify_game)
	{
		CAutoLock<CLock> lock(&m_lockNotify);
		m_mapNotify.insert(std::make_pair(gid, bForce));
	}
	else if (type == sync_game)
	{
		CAutoLock<CLock> lock(&m_lockSync);
		m_mapSync.insert(std::make_pair(gid, gid));
	}
	else if (type == delete_game)
	{
		CAutoLock<CLock> lock(&m_lockDelete);
		m_lstDelete.push_back(gid);
	}
}

bool CSyncPlug::GetNextGame(DWORD& gid, bool& bForce, GAMETYPE type)
{
	if (type == notify_game)
	{
		CAutoLock<CLock> lock(&m_lockNotify);
		if (m_mapNotify.size())
		{
			std::map<DWORD, bool>::iterator it = m_mapNotify.begin();
			gid = it->first;
			bForce = it->second;
			m_mapNotify.erase(it);
			return true;
		}
	}
	else if (type == sync_game)
	{
		CAutoLock<CLock> lock(&m_lockSync);
		if (m_mapSync.size())
		{
			std::map<DWORD, DWORD>::iterator it = m_mapSync.begin();
			gid = it->first;
			bForce = false;
			m_mapSync.erase(it);
			return true;
		}
	}
	else if (type == delete_game)
	{
		CAutoLock<CLock> lock(&m_lockDelete);
		if (m_lstDelete.size())
		{
			std::list<DWORD>::iterator it = m_lstDelete.begin();
			gid = *it;
			m_lstDelete.erase(it);
			return true;
		}
	}
	return false;
}

void CSyncPlug::SyncGame(DWORD gid, bool bForce)
{
	char SvrPath[MAX_PATH] = {0};
	char CliPath[1024] = {0};
	char gName[MAX_PATH]   = {0};
	char CliIp[MAX_PATH]   = {0};
	DWORD MaxSpeed = 0;
	DWORD version  = 0;
	DWORD flag = 0;
	{
		CAutoLock<CLock> lock(&m_lockAllGame);
		std::map<DWORD, tagGameInfo*>::iterator it = m_mapAllGame.find(gid);
		if (it == m_mapAllGame.end())
			return ;
		tagGameInfo* pGame = it->second;
		lstrcpyA(SvrPath, pGame->SvrPath);
		lstrcpyA(CliPath, pGame->CliPath);
		lstrcpyA(gName,   pGame->Name);
		lstrcpyA(CliIp,   pGame->CliIp);
		version	= pGame->version;
		if (bForce)
			flag |= UPDATE_FLAG_FORCE_UPDATE;
	}
	std::vector<std::string> list;
	SplitString(CliPath, list);
	for (size_t idx=0;idx<list.size(); idx++)
	{
		std::string dir = list[idx];
		WriteLog("开始同步游戏:%d, %s, %s", gid, gName, dir.c_str());

		//避免用户把游戏设置到第一级目录下（E:\\网络游戏\\）,删除多余文件时，把同类游戏都删除了。
		if (std::count(dir.begin(), dir.end(), '\\') > 2)
			flag |= UPDATE_FLAG_DELETE_MOREFILE;
		else
			WriteLog("把游戏设置到第一级目录下了，避免误删游戏，同步后不执行删除多余文件.");
		
		HANDLE hUpdate = UG_StartUpdate(gid, _bstr_t(SvrPath), _bstr_t(dir.c_str()), flag, 
			_bstr_t(GetMainSvrIp().c_str()), _bstr_t(CliIp), 0/*MaxSpeed*/);
		m_pFastCopyStatus->gid = gid;

		bool bSucess = true;
		while (WAIT_TIMEOUT == WaitForSingleObject(m_hExited, 1000))
		{
			FastCopy_GetSpeedEx((DWORD)hUpdate, m_pFastCopyStatus);
			if (m_pFastCopyStatus->Progress == FCS_ERROR)
			{
				bSucess = false;
				WriteLog("同步失败:gid:%d, %s", gid, (LPCSTR)_bstr_t(m_pFastCopyStatus->szCurFile));
				break;
			}
			else if (m_pFastCopyStatus->Progress == FCS_FINISH)
			{
				bSucess = true;
				if (m_pFastCopyStatus->TotalFileSize == 0)
				{
					DWORD ver = Idxfile_GetVersion(_bstr_t(dir.c_str()) + _bstr_t(INDEX_FILE_NAMEW));
					m_Server.SyncSetGameVersion(gid, ver);
				}
				else 
				{
					if (m_hRefreshVDisk != NULL)
						SetEvent(m_hRefreshVDisk);
				}
				WriteLog("同步完成:gid:%d, %0.2fMBytes.", gid, (float)(m_pFastCopyStatus->TotalFileSize/1024.0/1024.0));
				break;
			}
		}
		UG_StopUpdate(hUpdate);
		ZeroMemory(m_pFastCopyStatus, sizeof(FastCopyStatusEx));
		SendSyncFinish2UI(gid,bSucess);
	}	
}

void DeleteGameImp(LPCWSTR dir, HANDLE hExited)
{
	wchar_t filename[MAX_PATH] = {0};
	wchar_t find[MAX_PATH] = {0};
	swprintf(find, L"%s*.*", dir);

	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFileW(find, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;
	do 
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(hExited, 0))
			break;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//filter dot directory.
			if (!((wfd.cFileName[0] == L'.') && 
				((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				swprintf(filename, L"%s%s\\", dir, wfd.cFileName);
				DeleteGameImp(filename, hExited);
			}
		}
		else
		{
			swprintf(filename, L"%s%s", dir, wfd.cFileName);
			SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);
			DeleteFileW(filename);
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);
	RemoveDirectoryW(dir);
}

void CSyncPlug::DeleteGame(DWORD gid)
{
	char path[1024] = {0};
	{
		CAutoLock<CLock> lock(&m_lockSync);
		std::map<DWORD, tagGameInfo*>::iterator it = m_mapAllGame.find(gid);
		if (it == m_mapAllGame.end())
			return ;
		lstrcpyA(path, it->second->CliPath);
		delete it->second;
		m_mapAllGame.erase(gid);
	}
	std::vector<std::string> list;
	SplitString(path, list);
	for (size_t idx=0;idx<list.size(); idx++)
	{
		WriteLog("执行删除游戏:%d, %s", gid, list[idx].c_str());
		DeleteGameImp(_bstr_t(list[idx].c_str()), m_hExited);
	}
}

void __cdecl CSyncPlug::WriteLog(char* formater, ...)
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
				m_pILogger->SetLogFileName(SYNC_PLUG_NAME);
				m_pILogger->SetAddDateBefore(false);
				m_pILogger->WriteLog(LM_INFO, "===============================================");
				m_pILogger->SetAddDateBefore(true);
			}
		}
	}
	char szLog[4096 + sizeof(_packageheader) + 4 + 1] = {0};
	char* p = szLog + sizeof(_packageheader) + 4;
	va_list marker;
	va_start(marker, formater);
	int len = vsprintf(p, formater, marker);
	va_end(marker);

	//write log to logfile.
	if (m_pILogger != NULL)
		m_pILogger->WriteLog(LM_INFO, p);

	//write log to ui.
	_packageheader* pheader = (_packageheader*)szLog;
	InitPkgheader(pheader, NET_CMD_SYNCLOG);
	CPackageHelper outpackage(szLog);
	outpackage.pushDWORD(len);
	pheader->Length = sizeof(_packageheader) + 4 + len + 1;
	SendUdp2UI(szLog, pheader->Length);

	//write log to console.
}

void CSyncPlug::SendUdp2UI(char* pData, int length)
{
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
		return ;

	sockaddr_in  addr = {0};
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT_UDP_UI);
	addr.sin_family = AF_INET;

	int left = length;
	while(left)
	{
		int len = sendto(sck, pData + length - left, left > 0x10000 ? 0x10000 : left, 0, (PSOCKADDR)&addr, sizeof(addr));
		if(len < 0)
			return ;
		left -= len;
	}
	closesocket(sck);
}

void CSyncPlug::SendSyncFinish2UI(DWORD gid, bool bSuccess)
{
	char buf[1024] = {0};
	_packageheader* pheader = (_packageheader*)buf;
	CPackageHelper outpackage(buf);
	InitPkgheader(pheader, NET_CMD_ENDSYNCGAME);
	outpackage.pushDWORD(gid);
	outpackage.pushDWORD(bSuccess ? 1 : 0);
	pheader->Length = outpackage.GetOffset();
	SendUdp2UI(buf, pheader->Length);	
}

void CSyncPlug::NotifyUiRefresh()
{
	char buf[1024] = {0};
	_packageheader* pheader = (_packageheader*)buf;
	InitPkgheader(pheader, NET_CMD_REFRESHGUI);
	SendUdp2UI(buf, pheader->Length);	
}

typedef struct tagVDInfo
{
	DWORD dwIp;
	WORD  nPort;
	TCHAR szSvrDrv;
	TCHAR szCliDrv;
	DWORD dwConnect;
	DWORD dwVersion;//(time_t);
	TCHAR szTempDir[MAX_PATH];
}tagVDInfo;

typedef struct tagUpdataInfo
{
	DWORD	dwip;
	DWORD	gid;
	DWORD	speed;
	DWORD	progress;
	__int64 uptsize;
	__int64	leftsize;
	char	Name[64];
}tagUpdataInfo;

inline _bstr_t GetNodeAttr(const MSXML2::IXMLDOMNodePtr& node, LPCSTR name)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attr = node->Getattributes();
	if (!attr)
		return _bstr_t("");
	MSXML2::IXMLDOMNodePtr node2 = attr->getNamedItem(_bstr_t(name));
	if (!node2)
		return _bstr_t("");
	return node2->text;
}

void CSyncPlug::RefreshUIData()
{
	#define VDISK_CMD_GETALLINFO	0x05
	char*		szxml = NULL;
	DWORD		nVdiskCount = 0;
	tagVDInfo	VdList[10] = {0};
	if(m_pIPlugMgr)
	{
		IPlug* pIPlug= m_pIPlugMgr->FindPlug(GAMEUPDATE_PLUG_NAME);
		if(pIPlug)
			szxml =(char*)pIPlug->SendMessage(1, 0, 0);
		
		pIPlug = m_pIPlugMgr->FindPlug(VDISK_PLUG_NAME);
		if(pIPlug)
		{
			nVdiskCount = pIPlug->SendMessage(VDISK_CMD_GETALLINFO,(DWORD)VdList, _countof(VdList));
			if (nVdiskCount > _countof(VdList) || nVdiskCount < 0)
				nVdiskCount = 0;
		}
	}
	char buf[0x10000] = {0};
	_packageheader* pheader = (_packageheader*)buf;
	InitPkgheader(pheader, 0x0400);
	CPackageHelper outpackage(buf);
	
	/////////////////上报虚拟盘////////////////////////
	outpackage.pushDWORD(nVdiskCount);
	for(DWORD i =0; i<nVdiskCount; i++)
		outpackage.pushString((char*)&VdList[i],sizeof(tagVDInfo));
	
	///////////////上报同步状///////////////////
	outpackage.pushString((char*)m_pFastCopyStatus, sizeof(FastCopyStatusEx));
	
	//////////上报内网更新状态/////////////
	std::vector<tagUpdataInfo> UpdateInfos;
	if (szxml == NULL)
		outpackage.pushDWORD(0);
	else
	{
		CoInitialize(NULL);
		try
		{
			MSXML2::IXMLDOMDocumentPtr xml(__uuidof(XMLDocument));
			if (VARIANT_TRUE == xml->loadXML(szxml))
			{
				MSXML2::IXMLDOMNodeListPtr list = xml->selectNodes(_bstr_t("/status/download/task"));
				for (int idx=0; idx<list->length; idx++)
				{
					tagUpdataInfo UpdateInfo = {0};
					MSXML2::IXMLDOMNodePtr node = list->item[idx];
					UpdateInfo.dwip		= _ttoi(GetNodeAttr(node, "ip"));
					UpdateInfo.gid		= _ttoi(GetNodeAttr(node, "gid"));
					UpdateInfo.progress = _ttoi(GetNodeAttr(node, "progress"));
					UpdateInfo.speed	= _ttoi(GetNodeAttr(node, "speed"));
					UpdateInfo.uptsize	= _atoi64(GetNodeAttr(node, "uptsize"));
					UpdateInfo.leftsize = _atoi64(GetNodeAttr(node, "leftsize"));
					UpdateInfos.push_back(UpdateInfo);
				}
			}
			CoTaskMemFree(szxml);
		} catch (...) { }
		CoUninitialize();
		outpackage.pushDWORD(UpdateInfos.size());
		for(size_t i =0; i<UpdateInfos.size(); i++)
			outpackage.pushString((char*)&UpdateInfos[i],sizeof(UpdateInfos));
	}
	pheader->Length = outpackage.GetOffset();
	SendUdp2UI(buf, pheader->Length);
}