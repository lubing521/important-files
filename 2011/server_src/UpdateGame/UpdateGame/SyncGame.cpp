#include "stdafx.h"
#include "SyncGame.h"
#include "UpdateGame.h"
#include "idxfile.h"
#include <process.h>
#include <shlwapi.h>
#include <iphlpapi.h>
#include <algorithm>
#include <array>

#include "../../../include/Utility/ScopeGuard.hpp"
#include "../../../include/Utility/utility.h"
#include "../../../include/I8Type.hpp"
#include "../../../include/Serialize/Serialize.hpp"
#include "../../../include/Win32/System/NetworkHelper.hpp"


#pragma comment(lib, "shlwapi")
#pragma comment (lib,"Iphlpapi.lib")

#import "msxml3.dll"

enum 
{
	CMD_NOTIFY_ADD_GAME = 0x01,
	CMD_NOTFIY_DELETE_GAME,
	CMD_NOTIFY_START_TASK,
	CMD_NOTIFY_STOP_TASK,
	CMD_NOTIFY_SUSPEND,
	CMD_NOTIFY_RESUME,
	CMD_NOTIFY_SYNCGAME,
};


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

DWORD SendUdpPkg(const char* pPackage, int length, unsigned long ip)
{
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
		return 0;


	sockaddr_in server = {0};
	server.sin_family = AF_INET;
	server.sin_port   = htons(PORT_UDP_UPDATE);
	server.sin_addr.s_addr = ip;
	int left = length;
	while (left)
	{
		int len = sendto(sck, pPackage+(length-left), left, 0, (PSOCKADDR)&server, sizeof(server));
		if (len > 0)
			left -= len;
		else
			break;
	}
	closesocket(sck);
	return 1;
}

template < typename T, typename U >
void _SendPackage(T &t, U &u, const stdex::tString &m)
{
	struct OP
	{
		CPkgHelper &sendpkg_;
		const stdex::tString &SID_;
		OP(CPkgHelper &sendpkg ,const stdex::tString &SID)
			: sendpkg_(sendpkg)
			, SID_(SID)
		{}

		void operator()(const typename T::value_type &val) const
		{
			for(std::set<stdex::tString>::const_iterator iter = val.second.begin();
				iter != val.second.end(); ++iter)
			{
				if( *iter == SID_ )
					SendUdpPkg(sendpkg_.GetBuffer(), sendpkg_.GetLength(), val.first);
			}
		}
	};
	t.for_each(OP(u, m));
}


struct LogHelper
{
	ILogger *log_;
	CSocket *svr_;

	DWORD consoleIP_;
	LogHelper(CSocket *svr)
		: log_(0)
		, svr_(svr)
		, consoleIP_(0)
	{
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger**);

		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, _countof(szFile));
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Frame.dll"));
		HMODULE hMod = LoadLibrary(szFile);
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)GetProcAddress(hMod, MAKEINTRESOURCEA(5))))
		{
			pfnCreateLogger(&log_);
			if (log_ != NULL)
			{
				log_->SetLogFileName(TEXT("SyncGame"));
				log_->SetAddDateBefore(false);
				log_->WriteLog(LM_INFO, TEXT("==============================================="));
				log_->SetAddDateBefore(true);
			}
		}
	}
	~LogHelper()
	{
		log_->Release();
	}

	ILogger *operator->()
	{
		return log_;
	}

	DWORD GetConsoleIP()
	{
		if( consoleIP_ == 0 )
		{
			consoleIP_ = svr_->GetConsoleIP();
		}

		return consoleIP_;
	}
};


LogHelper &Log(CSocket *svr = 0)
{
	static LogHelper helper(svr);
	return helper;
}

void WriteLog(LPCTSTR formater, ...)
{
	TCHAR szLog[4096 + sizeof(pkgheader) + 4 + 1] = {0};
	TCHAR* p = szLog + sizeof(pkgheader) + 4;
	va_list marker;
	va_start(marker, formater);
	int len = _vstprintf_s(p, _countof(szLog) - (sizeof(pkgheader) + 4), formater, marker);
	va_end(marker);

	//write log to logfile.
	//write log to console.

	static DWORD dwIP = 0;
	if( dwIP == 0 )
	{
		dwIP = Log().GetConsoleIP();
		Log()->SetConsole(dwIP, 17900);
	}

	Log()->WriteLog(LM_INFO, p);
}

namespace sync
{


	std::map<stdex::tString, db::tSyncTask> MatchSameIPSyncTask(IRTDataSvr *rt, DWORD ip)
	{
		ISyncTaskTable *syncTask = rt->GetSyncTaskTable();
		ISyncTaskRecordset *records = 0;
		syncTask->Select(&records, 0, 0);

		std::map<stdex::tString, db::tSyncTask> syncTasks;
		size_t cnt = records->GetCount();
		for(size_t i = 0; i != cnt; ++i)
		{
			const db::tSyncTask *task = records->GetData(i);

			db::tServer svr;
			rt->GetServerTable()->GetData(task->SvrID, &svr);
			std::tr1::array<ulong, 4> svrIPs = { svr.Ip1, svr.Ip2, svr.Ip3, svr.Ip4};

			if( std::find(svrIPs.begin(), svrIPs.end(), ip) != svrIPs.end() )
			{
				if( std::find(svrIPs.begin(), svrIPs.end(), task->DestIp) != svrIPs.end() )
				{
					syncTasks.insert(std::make_pair(task->SID, *task));
					continue;
				}
			}
		}

		records->Release();
		return syncTasks;
	}

	SyncGameInfoPtr MakeSyncGame(const db::tGame *game, const db::tSyncTask &task)
	{
		SyncGameInfoPtr val(new SyncGameInfo);

		val->gid_					= game->GID;
		val->svrVer_				= game->SvrVer;
		val->forceRepair_			= game->Repair;
		val->maxSpeed_				= task.Speed;
		val->svrIP_					= task.SoucIp;
		val->cliIP_					= task.DestIp;

		utility::Strcpy(val->gameName_, game->Name);
		utility::Strcpy(val->svrPath_, game->SvrPath);
		utility::Strcpy(val->syncTaskName_, task.Name);
		utility::Strcpy(val->nodePath_, game->SvrPath);
		val->nodePath_[0] = task.NodeDir[0];
		utility::Strcpy(val->sid_, task.SID);

		return val;
	}

	template < typename SyncTaksT >
	SyncGamesInfo MatchSyncGames(IRTDataSvr *rt, const SyncTaksT &syncTasks, SyncGamesInfo &oldSyncGames)
	{
		ISyncGameTable *syncGames = rt->GetSyncGameTable();
		ISyncGameRecordset *records = 0;
		syncGames->Select(&records, (ISyncGameFilter *)0, 0);

		size_t cnt = records->GetCount();

		SyncGamesInfo newSyncGames;
		SyncGamesInfo syncGameTmp;

		// 遍历同步任务表
		for(size_t i = 0; i != cnt; ++i)
		{
			db::tSyncGame syncGame;
			records->GetData(i, &syncGame);

			std::map<stdex::tString, db::tSyncTask>::const_iterator syncTaskIter = syncTasks.find(syncGame.SID);
			if( syncTaskIter == syncTasks.end() )
				continue;

			const db::tSyncTask &syncTask = syncTaskIter->second;

			db::tGame game;
			rt->GetGameTable()->GetData(syncGame.GID, &game);
			if( game.Status != StatusLocal )
				continue;

			std::pair<long, stdex::tString> key = std::make_pair(game.GID, syncTask.SID);
			SyncGamesInfo::const_iterator iter = oldSyncGames.find(key);

			SyncGameInfoPtr syncGameInfo(MakeSyncGame(&game, syncTask));

			// 如果该游戏不存在于历史记录或者游戏存在于历史记录, 但版本号不同添加,则添加
			if( iter == oldSyncGames.end() || 
				game.SvrVer != iter->second->svrVer_ )
			{
				newSyncGames[key] = syncGameInfo;
				oldSyncGames[key] = syncGameInfo;
			}

			syncGameTmp.insert(std::make_pair(key, syncGameInfo));
		}

		std::set<std::pair<long, stdex::tString>> oldSIDs;
		for(SyncGamesInfo::const_iterator iter = oldSyncGames.begin(); iter != oldSyncGames.end(); ++iter)
		{
			if( syncGameTmp.find(iter->first) == syncGameTmp.end() )
				oldSIDs.insert(iter->first);
		}

		for(std::set<std::pair<long, stdex::tString>>::const_iterator iter = oldSIDs.begin(); iter != oldSIDs.end(); ++iter)
			oldSyncGames.erase(*iter);

		records->Release();


		return newSyncGames;
	}
}


CSyncServer::CSyncServer(ISvrPlugMgr* pIPlugMgr) 
: rtSvr_(0)
{
	WSAData data = {0};
	WSAStartup(0x0202, &data);

	if (pIPlugMgr != NULL)
	{
		ISvrPlug* RTDbPlug = pIPlugMgr->FindPlug(RTDATASVR_PLUG_NAME);
		if (RTDbPlug != NULL)
			rtSvr_ = (IRTDataSvr*)RTDbPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0);
	}
}

CSyncServer::~CSyncServer()
{
	WSACleanup();
}


SyncGamesInfo CSyncServer::GetSyncGame(long ip, bool first)
{
	// 匹配IP相同的同步任务
	std::map<stdex::tString, db::tSyncTask> syncTasks = sync::MatchSameIPSyncTask(rtSvr_, ip);

	struct AddNewGamesInfo
	{
		CSyncServer::ServerSyncGames &syncGames_;
		ulong ip_;
		AddNewGamesInfo(CSyncServer::ServerSyncGames &syncGames, ulong ip)
			: syncGames_(syncGames)
			, ip_(ip)
		{}
		void operator()() const
		{
			CSyncServer::SyncGamesInfoPtr gamesInfo(new SyncGamesInfo);
			syncGames_[ip_] = gamesInfo;
		}
	};

	// 匹配同步任务,如果没有找到对应IP的任务则添加
	severSyncGames_.not_if_op(ip, AddNewGamesInfo(severSyncGames_, ip));

	SyncGamesInfo &oldSyncGames = *(severSyncGames_[ip]);
	SyncGamesInfo syncGames = sync::MatchSyncGames(rtSvr_, syncTasks, *(severSyncGames_[ip]));
	if( first )
		syncGames = oldSyncGames;

	std::set<stdex::tString> SIDs;
	for(SyncGamesInfo::const_iterator iter = oldSyncGames.begin(); iter != oldSyncGames.end(); ++iter)
		SIDs.insert(iter->second->sid_);

	svrsIP_[ip] = SIDs;

	return syncGames;
}

bool CSyncServer::IsStarted(const stdex::tString &SID) const
{
	db::tSyncTask syncTask;
	rtSvr_->GetSyncTaskTable()->GetData(SID.c_str(), &syncTask);

	return syncTask.Status == TaskStarted;
}

int	CSyncServer::handle_SetGameVer(pkgheader* pheader, char* pbuf, long length)
{
	CPkgHelper inpackage((LPSTR)pheader);
	CPkgHelper outpackage((LPSTR)pbuf, length, pheader->Cmd, (OBJECT_TYPE)pheader->Resv[0]);

	try
	{
		DWORD gid = 0, ver = 0;
		inpackage.Pop(gid);
		inpackage.Pop(ver);
		db::tGame game;
		game.SvrVer = game.IdcVer = ver;
		rtSvr_->GetGameTable()->Update(gid, &game, MASK_TGAME_SVRVER|MASK_TGAME_IDCVER);
	}
	catch (...) { }
	outpackage.Push(ERROR_SUCCESS);
	return outpackage.GetLength();
}


void CSyncServer::StartSync(const stdex::tString &SID, const stdex::tString &taskName)
{	
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTIFY_START_TASK, OBJECT_NODESERVER);
	sendpkg << SID << taskName;
	_SendPackage(svrsIP_, sendpkg, SID);
}


void CSyncServer::StopSync(const stdex::tString &SID, const stdex::tString &taskName)
{
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTIFY_STOP_TASK, OBJECT_NODESERVER);
	sendpkg << SID << taskName;
	_SendPackage(svrsIP_, sendpkg, SID);
}

void CSyncServer::AddGame(const stdex::tString &SID, long gid, const stdex::tString &taskName)
{
	// ahh~~ ahh~~
}

void CSyncServer::DeleteGame(const stdex::tString &SID, long gid, const stdex::tString &taskName, bool hasFiles)
{
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTFIY_DELETE_GAME, OBJECT_NODESERVER);
	sendpkg << SID << gid << taskName << (hasFiles ? 1L : 0L);
	_SendPackage(svrsIP_, sendpkg, SID);
}

void CSyncServer::SuspendTask(const stdex::tString &SID, const stdex::tString &taskName)
{
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTIFY_SUSPEND, OBJECT_NODESERVER);

	sendpkg << SID << taskName;

	_SendPackage(svrsIP_, sendpkg, SID);
}

void CSyncServer::ResumeTask(const stdex::tString &SID, const stdex::tString &taskName)
{
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTIFY_RESUME, OBJECT_NODESERVER);

	sendpkg << SID << taskName;
	_SendPackage(svrsIP_, sendpkg, SID);
}

DWORD CSyncServer::NotifyGame(const stdex::tString &SID, DWORD gid, const stdex::tString &taskName, bool bForce)
{
	char package[4096] = {0};
	CPkgHelper sendpkg(package, CMD_NOTIFY_SYNCGAME, OBJECT_NODESERVER);
	sendpkg << SID << gid << taskName << (bForce ? 1L : 0L);

	_SendPackage(svrsIP_, sendpkg, SID);

	return 1;
}

DWORD CSyncServer::NotifySyncGame(DWORD gid, bool force)
{
	struct OP
	{
		typedef std::tr1::function<DWORD(const stdex::tString &, DWORD, const stdex::tString &)> Callback;
		Callback callback_;
		long gid_;
		OP(long gid, const Callback &callback)
			: gid_(gid)
			, callback_(callback)
		{}
		void operator()(const ServerSyncGames::value_type &val) const
		{
			for(SyncGamesInfo::const_iterator iter = val.second->begin(); iter != val.second->end(); ++iter)
			{
				if( iter->second->gid_ == gid_ )
					callback_(iter->second->sid_, gid_, _T(" "));
			}
		}
	};

	using namespace std::tr1::placeholders;
	severSyncGames_.for_each(OP(gid, std::tr1::bind(&CSyncServer::NotifyGame, this, _1, _2, _3, force)));

	return 1;
}

DWORD CSyncServer::NotifyDeleteGame(DWORD gid)
{
	if (rtSvr_ == 0)
		return 0;

	struct OP
	{
		typedef std::tr1::function<void(const stdex::tString &, DWORD, const stdex::tString &)> Callback;
		Callback callback_;
		long gid_;
		OP(long gid, const Callback &callback)
			: gid_(gid)
			, callback_(callback)
		{}
		void operator()(const ServerSyncGames::value_type &val)
		{
			for(SyncGamesInfo::const_iterator iter = val.second->begin(); iter != val.second->end(); ++iter)
			{
				callback_(iter->second->sid_, iter->second->gid_, _T(" "));
			}
		}
	};

	using namespace std::tr1::placeholders;
	severSyncGames_.for_each(OP(gid, std::tr1::bind(&CSyncServer::DeleteGame, this, _1, _2, _3, true)));

	return 1;
}




CSyncPlug::CSyncPlug()
: m_pIPlugMgr(0)
{
	exitEvent_.Create(NULL, TRUE, FALSE);
	syncTaskMgr_.reset(new SyncTaskMgr(exitEvent_, m_Server));
}
CSyncPlug::~CSyncPlug()
{

};

bool CSyncPlug::Initialize(ISvrPlugMgr* pPlugMgr)
{
	m_pIPlugMgr = pPlugMgr;


	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_Threads.push_back(hThread);
	hThread = (HANDLE)_beginthreadex(NULL, 0, UdpThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_Threads.push_back(hThread);

	syncTaskMgr_->Start();

	Log(&m_Server);
	return true;
}

bool CSyncPlug::UnInitialize()
{
	exitEvent_.SetEvent();
	syncTaskMgr_->Stop();	

	if (m_Threads.size())
	{
		WaitForMultipleObjects(m_Threads.size(), &m_Threads[0], TRUE, INFINITE);
		for (size_t idx=0; idx<m_Threads.size(); idx++)
			CloseHandle(m_Threads[idx]);
		m_Threads.clear();
	}

	return true;
}

DWORD CSyncPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	switch(cmd)
	{
	case Driver:
		{
			*(DWORD*)param2 = m_setDriver.size();
			LPTSTR p = reinterpret_cast<LPTSTR>(param1);
			for (std::set<TCHAR>::iterator it = m_setDriver.begin(); it != m_setDriver.end(); it++)
			{
				*p++ = (*it);
			}
			*p = 0;
		}
		break;
	default:
		assert(0);
		break;
	}


	return 0;
}

UINT32 __stdcall CSyncPlug::WorkThread(LPVOID lpVoid)
{
	CSyncPlug* pThis = reinterpret_cast<CSyncPlug*>(lpVoid);

	while (1)
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject(pThis->exitEvent_, 5000) )
			break;

		//check connect.
		if( !pThis->m_Server.IsConnected() )
			pThis->m_Server.ConnectServer(GetMainSvrIp().c_str(), NULL, PORT_TCP_UPDATE);

		if (pThis->m_Server.IsConnected())
		{
			//refresh sync list.
			pThis->GetAllSyncTask();
		}
	}
	pThis->m_Server.CloseServer();
    WriteLog(_T("关闭同步模块"));
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
		HANDLE hEvents[] = {pThis->exitEvent_, hEvent};
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
					pkgheader* pheader = reinterpret_cast<pkgheader*>(buf);

					int len = recv(sck, buf, sizeof(buf), 0);
					if (len < sizeof(pkgheader) || pheader->Length != len || pheader->StartFlag != START_FLAG)
						continue ;
					buf[len] = 0;

					CPkgHelper package(buf);
					switch (pheader->Cmd)
					{
					case CMD_NOTIFY_ADD_GAME:
						{
							stdex::tString taskName;
							SyncGameInfoPtr syncGame(new SyncGameInfo);
							package >> *(syncGame.get()) >> taskName;

							WriteLog(TEXT("添加同步任务的游戏:%s, %d."), taskName.c_str(), syncGame->gid_);
							pThis->syncTaskMgr_->AddSyncGame(syncGame->sid_, syncGame);
						}
						break;
					case CMD_NOTFIY_DELETE_GAME:
						{
							long gid = 0;
							stdex::tString SID, taskName;
							DWORD hasFiles = 0;
							package >> SID >> gid >> taskName >> hasFiles;

							WriteLog(TEXT("删除同步任务的游戏: %s, %d."), taskName.c_str(), gid);
							pThis->syncTaskMgr_->DeleSyncGame(SID, gid, (hasFiles == 1L ? true : false));
						}
						break;

					case CMD_NOTIFY_START_TASK:
						{
							stdex::tString SID, taskName;
							package >> SID >> taskName;

							WriteLog(TEXT("开始同步任务:%s"), taskName.c_str());
							pThis->syncTaskMgr_->StartSyncTask(SID);
						}
						break;

					case CMD_NOTIFY_STOP_TASK:
						{
							stdex::tString SID, taskName;
							package >> SID >> taskName;

							WriteLog(TEXT("结束同步任务:%s"), taskName.c_str());
							pThis->syncTaskMgr_->StopSyncTask(SID);
						}
						break;

					case CMD_NOTIFY_SUSPEND:
						{
							stdex::tString SID, taskName;
							package >> SID >> taskName;

							WriteLog(TEXT("暂停同步任务:%s"), taskName.c_str());
							pThis->syncTaskMgr_->SuspendSyncTask(SID);
						}
						break;

					case CMD_NOTIFY_RESUME:
						{
							stdex::tString SID, taskName;
							package >> SID >> taskName;

                            WriteLog(TEXT("恢复同步任务:%s"), taskName.c_str());
							pThis->syncTaskMgr_->ResumeSyncTask(SID);
						}
						break;

					case CMD_NOTIFY_SYNCGAME:
						{
							long gid = 0;
							stdex::tString SID, taskName;
							DWORD force = 0;
							package >> SID >> gid >> taskName >> force;

							WriteLog(TEXT("通知同步任务:%s"), taskName.c_str());
							pThis->syncTaskMgr_->NotifySyncGame(SID, gid, (force ? true : false));
						}
						break;
					}
				}
				catch (...) 
				{ 
					WriteLog(TEXT("Udp Thread Process Error.")); 
				}
			}
		}
	}
	closesocket(sck);
	WSACloseEvent(hEvent);
	return 0;
}


bool CSyncPlug::GetAllSyncTask()
{
	char* pdata  = NULL;
	int   length = 0;
	static bool first = true;

	if( !m_Server.GetAllSyncTask(pdata, length, first) )
		return false;

	CPkgHelper in(pdata);
	size_t ret = 0, cnt = 0;
	in >> ret >> cnt;

	typedef std::vector<SyncGameInfoPtr> SyncGamesVec;
	std::map<stdex::tString, SyncGamesVec> syncGames;
	for(size_t i = 0; i != cnt; ++i)
	{
		SyncGameInfoPtr game(new SyncGameInfo);
		in >> *game;

		syncGames[game->sid_].push_back(game);
	}

	for(std::map<stdex::tString, SyncGamesVec>::const_iterator iter = syncGames.begin();
		iter != syncGames.end(); ++iter)
	{
		syncTaskMgr_->AddSyncTask(iter->first, iter->second);
	}
	delete []pdata;

	for(std::map<stdex::tString, SyncGamesVec>::const_iterator iter = syncGames.begin();
		iter != syncGames.end(); ++iter)
	{
		if( m_Server.IsStart(iter->first) )
		{
				syncTaskMgr_->StartSyncTask(iter->first);
		}
	}
	

	first = false;
	return true;
}






SyncTaskMgr::SyncTaskMgr(async::thread::AutoEvent &exitEvent, CSocket &svr)
: exitEvent_(exitEvent)
, svr_(svr)
{

}

SyncTaskMgr::~SyncTaskMgr()
{

}

void SyncTaskMgr::Start()
{
	CEvenyOneSD sd;
	refreshVDiskEvent_.Create(VDISK_EVENT_NAME, FALSE, FALSE, sd.GetSA());
}

void SyncTaskMgr::Stop()
{
	struct OP
	{
		void operator()(const SyncTasks::value_type &val) const
		{
			val.second->Stop();
		}
	};
	syncTasks_.for_each(OP());
}

void SyncTaskMgr::StartSyncTask(const stdex::tString &SID)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::Start, std::tr1::placeholders::_1));
}

void SyncTaskMgr::StopSyncTask(const stdex::tString &SID)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::Stop, std::tr1::placeholders::_1));
}

void SyncTaskMgr::AddSyncTask(const stdex::tString &SID, const std::vector<SyncGameInfoPtr> &syncGamesInfo)
{
	if( !syncTasks_.exsit(SID) )
	{
		using namespace std::tr1::placeholders;
		SyncTaskPtr syncTask(new SyncTask(refreshVDiskEvent_, 
			std::tr1::bind(&SyncTaskMgr::_ReportStatus, this, _1),
			std::tr1::bind(&SyncTaskMgr::_ReportError, this, _1, _2)));

		for(std::vector<SyncGameInfoPtr>::const_iterator iter = syncGamesInfo.begin();
			iter != syncGamesInfo.end(); ++iter)
		{
			syncTask->AddSyncGame(*iter, false);
		}
		syncTasks_[SID] = syncTask;
	}
	else
	{
		for(std::vector<SyncGameInfoPtr>::const_iterator iter = syncGamesInfo.begin();
			iter != syncGamesInfo.end(); ++iter)
		{
			AddSyncGame(SID, *iter);
		}
	}
}

void SyncTaskMgr::DelSyncTask(const stdex::tString &SID)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::Stop, std::tr1::placeholders::_1));
	syncTasks_.erase(SID);
}

void SyncTaskMgr::SuspendSyncTask(const stdex::tString &SID)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::Suspend, std::tr1::placeholders::_1));
}

void SyncTaskMgr::ResumeSyncTask(const stdex::tString &SID)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::Resume, std::tr1::placeholders::_1));
}


void SyncTaskMgr::AddSyncGame(const stdex::tString &SID, const SyncGameInfoPtr &syncGameInfo)
{
	assert(syncTasks_.exsit(SID));

	SyncTaskPtr &syncTask = syncTasks_[SID];
	syncTask->AddSyncGame(syncGameInfo, true);
}

void SyncTaskMgr::DeleSyncGame(const stdex::tString &SID, long gid, bool hasFiles)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::DelSyncGame, std::tr1::placeholders::_1, gid, hasFiles));
}

void SyncTaskMgr::PutTopSyncGame(const stdex::tString &SID, long gid)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::PutTop, std::tr1::placeholders::_1, gid));
}

void SyncTaskMgr::PutBottomSyncGame(const stdex::tString &SID, long gid)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::PutBottom, std::tr1::placeholders::_1, gid));
}

void SyncTaskMgr::NotifySyncGame(const stdex::tString &SID, long gid, bool force)
{
	syncTasks_.op_if(SID, std::tr1::bind(&SyncTask::NotifySyncGame, std::tr1::placeholders::_1, gid, force));
}

void SyncTaskMgr::_ReportStatus(const db::tSyncTaskStatus &status)
{
    if (svr_.ReportSyncStatus(status, 3))
    {
#ifdef _DEBUG
        WriteLog(TEXT("报告游戏更新状态：游戏[%d][%s] 状态[%d] 进度[%d]"), status.GID, status.GameName, status.UpdateState, status.UpdateSize == 0 ? -1 : 100 * status.BytesTransferred / status.UpdateSize);
#endif
    }
    else
    {
#ifdef _DEBUG
        WriteLog(TEXT("报告游戏更新状态失败"));
#endif
    }
}

void SyncTaskMgr::_ReportError(const stdex::tString &SID, long gid)
{
	svr_.ReportSyncGameError(SID, gid);
}


namespace policy
{
	struct Find
	{
		SyncGamePtr syncGame_;
		long gid_;
		explicit Find(const SyncGamePtr &syncGame)
			: syncGame_(syncGame)
			, gid_(0)
		{}
		explicit Find(long gid)
			: gid_(gid)
		{}

		bool operator()(const SyncGamePtr &val) const
		{
			if( gid_ == 0 )
				return *val == *syncGame_;
			else
				return val->GetGid() == gid_;
		}

		bool operator()(const db::tSyncTaskStatus &val) const
		{
			return val.GID == gid_;
		}
	};

}

SyncTask::SyncTask(async::thread::AutoEvent &refreshVDiskEvent
				   , const ReportCallback &report, const ErrorCallback &errorCallback)
				   : isPaused_(false) 
				   , refreshVDiskEvent_(refreshVDiskEvent)
				   , reportCallback_(report)
				   , errorCallback_(errorCallback)
{
    exitEvent_.Create(NULL, TRUE, FALSE);
}

void SyncTask::Start()
{
	if( thread_.IsRunning() )
		return;

	thread_.RegisterFunc(std::tr1::bind(&SyncTask::Run, this));
    thread_.Start();
    WriteLog(TEXT("开始同步任务"));
}

void SyncTask::Stop()
{
	if( !thread_.IsRunning() )
		return;

    exitEvent_.SetEvent();
    thread_.Stop();
    WriteLog(TEXT("停止同步任务"));
}

void SyncTask::AddSyncGame(const SyncGameInfoPtr &syncGameInfo, bool notify)
{
	using namespace std::tr1;

	SyncGamePtr syncGame(new SyncGame(syncGameInfo,
		bind(&SyncTask::_UpdateBefore, this),
		bind(&SyncTask::_UpdateRunning, this, placeholders::_1, placeholders::_2),
		bind(&SyncTask::_UpdateAfter, this, placeholders::_1),
		bind(&SyncTask::_IsPaused, this)));

	if( notify )
		syncTaskGames_.push_front(syncGame);
	else
        syncTaskGames_.push_back(syncGame);
    WriteLog(TEXT("增加同步游戏[%d][%s]"), syncGameInfo->gid_, syncGameInfo->gameName_);
}

void SyncTask::DelSyncGame(long gid, bool hasFiles)
{
    class SetSyncGameDeleteSign
    {
    public:
        void operator()(SyncGamePtr& syncGame) const
        {
            syncGame->SetDelete();
        }
    };
    WriteLog(TEXT("删除同步游戏[%d]"), gid);
	if( hasFiles )
	{
		SyncGameContainer::const_iterator iter = syncTaskGames_.find_if(policy::Find(gid));
		if( iter != syncTaskGames_.end() )
		{
			delSyncGames_.push_back(*iter);
		}
		else
		{
			SyncGameContainer::const_iterator iter2 = complateGames_.find_if(policy::Find(gid));
			if( iter2 != complateGames_.end() )
            {
                delSyncGames_.push_back(*iter2);
            }
            else
            {
                SyncGameContainer::const_iterator iter3 = failTaskGames_.find_if(policy::Find(gid));
                if( iter3 != failTaskGames_.end() )
                    delSyncGames_.push_back(*iter3);
            }
		}
	}

    failTaskGames_.erase(policy::Find(gid));
    syncTaskGames_.op_if(policy::Find(gid), SetSyncGameDeleteSign());
	syncTaskGames_.erase(policy::Find(gid));
	complateGames_.erase(policy::Find(gid));
}

void SyncTask::Suspend()
{
    isPaused_ = true;
    WriteLog(TEXT("暂停同步任务"));
}

void SyncTask::Resume()
{
    if( !thread_.IsRunning() )
    {
        Start();
    }

    isPaused_ = false;
    WriteLog(TEXT("恢复同步任务"));
}

void SyncTask::PutTop(long gid)
{

}

void SyncTask::PutBottom(long gid)
{

}

void SyncTask::NotifySyncGame(long gid, bool force)
{
	SyncGamePtr syncGame;
	SyncGameContainer::const_iterator iter = syncTaskGames_.find_if(policy::Find(gid));
	if( iter == syncTaskGames_.end() )
	{
		SyncGameContainer::const_iterator iter2 = complateGames_.find_if(policy::Find(gid));
        if( iter2 == complateGames_.end() )
        {
            SyncGameContainer::const_iterator iter3 = failTaskGames_.find_if(policy::Find(gid));
            if( iter3 == failTaskGames_.end() )
                return;
            else
                syncGame = *iter3;
        }
        else
            syncGame = *iter2;
	}
	else
		syncGame = *iter;

	syncGame->GetInfo()->forceRepair_ = force;

    syncTaskGames_.erase(policy::Find(gid));
    failTaskGames_.erase(policy::Find(gid));
    complateGames_.erase(policy::Find(gid));
	syncTaskGames_.push_front(syncGame);
}

struct SyncTaskError
	: public db::tSyncTaskStatus
{
	size_t errCode;

	SyncTaskError(LPCTSTR sid, long gid, LPCTSTR gameName, LPCTSTR svrPath, LPCTSTR nodePath)
	{
		utility::Strcpy(tSyncTaskStatus::SID, sid);
		tSyncTaskStatus::GID = gid;
		utility::Strcpy(tSyncTaskStatus::GameName, gameName);
		utility::Strcpy(tSyncTaskStatus::SvrDir, svrPath);
		utility::Strcpy(tSyncTaskStatus::NodeDir, nodePath);
	}
};


DWORD SyncTask::Run()
{
    class MoveTopToOtherContainer
    {
        SyncGameContainer& container_;
        SyncGamePtr checkData_;
    public:
        MoveTopToOtherContainer(SyncGameContainer& container, const SyncGamePtr checkData = SyncGamePtr())
            : container_(container)
            , checkData_(checkData)
        {
        }
        bool operator()(const SyncGamePtr& data)
        {
            if (checkData_ == NULL || (checkData_.get() != NULL && checkData_ == data))
            {
                container_.push_back(data);
                return true;
            }
            return false;
        }
    };
	SyncGamePtr syncGame;
    bool lastFailSyncGame = false;
    bool lastRunNull = false;

	while(!thread_.IsAborted())
    {
        if (syncTaskGames_.empty() && lastFailSyncGame)
        {
            for (int i = 0; i < 10 * 60 && syncTaskGames_.empty(); ++i)
            {
                if( exitEvent_.WaitForEvent(1000) )
                    break;
            }
        }
        else
        {
            if( exitEvent_.WaitForEvent(1000) )
                break;
        }

        lastFailSyncGame = false;

		// del game files
		if( !delSyncGames_.empty() )
		{
            SyncGamePtr delSyncGame = delSyncGames_.top();
            WriteLog(TEXT("删除同步游戏文件[%d][%s]"), delSyncGame->GetGid(), delSyncGame->GetInfo()->gameName_);
			delSyncGame->DeleteGameFiles();
			delSyncGames_.pop_top();
			continue;
		}
        
		// sync game
		if (syncTaskGames_.empty())
        {
            if (failTaskGames_.empty())
            {
                if (!lastRunNull)
                {
                    WriteLog(TEXT("同步任务空转"));
                }
                lastRunNull = true;
                continue;
            }
            else
            {
                failTaskGames_.erase(MoveTopToOtherContainer(syncTaskGames_));
            }
        }

        lastRunNull = false;

		syncGame = syncTaskGames_.top();

		try
		{
			if (!syncGame->Run())
            {
                WriteLog(TEXT("同步游戏因删除而中止[%d][%s]"), syncGame->GetGid(), syncGame->GetInfo()->gameName_);
                continue;
            }
		}
		catch(const SyncTaskError &status)
		{
			reportCallback_(std::tr1::cref(status));

			if( status.errCode == UPT_ERR_GETGAMEINFO ||
				status.errCode == UPT_ERR_DWNIDX )
			{
				errorCallback_(syncGame->GetInfo()->sid_, syncGame->GetInfo()->gid_);
			}
			else
			{
                syncTaskGames_.erase(MoveTopToOtherContainer(failTaskGames_, syncGame));
                lastFailSyncGame = true;
                WriteLog(TEXT("同步游戏出错[%d][%s]"), syncGame->GetGid(), syncGame->GetInfo()->gameName_);
				continue;
			}
		}

        WriteLog(TEXT("完成同步游戏[%d][%s]"), syncGame->GetGid(), syncGame->GetInfo()->gameName_);
        syncTaskGames_.erase(MoveTopToOtherContainer(complateGames_, syncGame));
	}

    WriteLog(TEXT("同步任务结束"));
	return 0;
}

bool SyncTask::_UpdateBefore()
{
	return !exitEvent_.WaitForEvent(1000);
}

bool SyncTask::_UpdateRunning(const SyncGameInfoPtr &syncGame, const db::tSyncTaskStatus &syncStatus)
{
	reportCallback_(std::tr1::cref(syncStatus));

	return exitEvent_.IsSignalled() != TRUE;
}

void SyncTask::_UpdateAfter(const db::tSyncTaskStatus &syncStatus)
{
	refreshVDiskEvent_.SetEvent();

	reportCallback_(std::tr1::cref(syncStatus));
}

bool SyncTask::_IsPaused()
{
	return isPaused_;
}


template < typename T, typename P >
struct UpdateClose
{
	void operator()(T handle)
	{
		::UG_StopUpdate(handle);
	}
};
typedef utility::CSmartHandle<HANDLE, UpdateClose> UGAutoHandle;

bool SyncGame::Run()
{
	DWORD flag = 0;

	if( info_->forceRepair_ )
		flag |= UPDATE_FLAG_FORCE_UPDATE;

	if( std::count(info_->nodePath_, info_->nodePath_ + _tcslen(info_->nodePath_), '\\') > 2 )
		flag |= UPDATE_FLAG_DELETE_MOREFILE;
	else
		WriteLog(TEXT("把游戏设置到第一级目录下了，避免误删游戏，同步后不执行删除多余文件."));

	UGAutoHandle update = ::UG_StartUpdate(info_->gid_, NULL, info_->svrPath_, info_->nodePath_, flag, 
		win32::network::IP2String(info_->svrIP_).c_str(), win32::network::IP2String(info_->cliIP_).c_str(), info_->maxSpeed_, Sync);

	tagUpdateStatus updateStatus;
	updateStatus.cbSize = sizeof(tagUpdateStatus);

	SyncTaskError syncGameStatus(info_->sid_, info_->gid_, info_->gameName_, info_->svrPath_, info_->nodePath_);

	WriteLog(TEXT("开始同步游戏:%d, %s, %s"), info_->gid_, info_->gameName_, info_->svrPath_);
	while (true)
    {
        if (updateBefore_ != 0 && !updateBefore_())
        {
            return true;  //表示无需同步
        }
		if (isDeleted_)
			return false;

		if( isPaused_() )
			::UG_PauseUpdate(update);
		else
			::UG_ResumeUpdate(update);

		if( !::UG_GetUpdateStatus(update, &updateStatus) || 
			updateStatus.dwStatus == UPDATE_STATUS_ERROR )
		{
			WriteLog(_T("同步游戏: %d 失败: %s"), info_->gid_, updateStatus.awInfo);
			syncGameStatus.UpdateState = GameError;
			syncGameStatus.errCode = updateStatus.enErrCode;
			utility::Strcpy(syncGameStatus.Error, updateStatus.awInfo);
			throw syncGameStatus;
		}
		else if( updateStatus.dwStatus == UPDATE_STATUS_FINISH )
		{
			syncGameStatus.UpdateState = GameComplate;

			if( updateAfter_ != 0 )
				updateAfter_(syncGameStatus);

			WriteLog(TEXT("同步完成:gid:%d, %0.2fMBytes."), syncGameStatus.GID, (float)(syncGameStatus.UpdateSize / 1024.0 / 1024.0));
			break;
		}
		else
		{
			syncGameStatus.BytesTotal = 0;
			syncGameStatus.UpdateSize = updateStatus.qNeedsUpdateBytes;
			syncGameStatus.BytesTransferred = updateStatus.qUpdatedBytes;
			syncGameStatus.TransferRate = updateStatus.dwSpeed * 1024;
            syncGameStatus.UpdateState = isPaused_() ? GamePaused : GameRunning;

			if( updateRunning_ != 0 )
			{
				if( !updateRunning_(info_, syncGameStatus) )
					break; //表示同步顺利完成
			}
		}
	}
    return true;
}


namespace impl
{
	void DeleteGameImp(LPCWSTR dir)
	{
		wchar_t filename[MAX_PATH] = {0};
		wchar_t find[MAX_PATH] = {0};
		swprintf_s(find, L"%s*.*", dir);

		WIN32_FIND_DATAW wfd = {0};
		HANDLE hFinder = ::FindFirstFileW(find, &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return ;
		do 
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//filter dot directory.
				if (!((wfd.cFileName[0] == L'.') && 
					((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
				{
					swprintf_s(filename, L"%s%s\\", dir, wfd.cFileName);
					DeleteGameImp(filename);
				}
			}
			else
			{
				swprintf_s(filename, L"%s%s", dir, wfd.cFileName);
				::SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);
				::DeleteFileW(filename);
			}
		}while (::FindNextFileW(hFinder, &wfd));
		::FindClose(hFinder);
		::RemoveDirectoryW(dir);
	}
}

void SyncGame::SetDelete()
{
	isDeleted_ = true;
}

void SyncGame::DeleteGameFiles()
{
	WriteLog(TEXT("执行删除游戏:%d, %s"), info_->gid_, info_->nodePath_);
	impl::DeleteGameImp(info_->nodePath_);
}

