#ifndef __sync_game_inc__
#define __sync_game_inc__

#include "public.h"
#include "socket.h"

#include <map>
#include <list>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include "UpdateGame.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MultiThread/Thread.hpp"
#include "../../../include/tablestruct.h"
#include "../../../include/Container/SyncContainer.hpp"


std::string GetMainSvrIp();


class CSyncServer
{
public:
	// 不同从服务器有不同的同步任务
	typedef std::tr1::shared_ptr<SyncGamesInfo> SyncGamesInfoPtr;
	typedef async::container::SyncAssocContainer<u_long, SyncGamesInfoPtr> ServerSyncGames;

	typedef async::container::SyncAssocContainer<u_long, std::set<stdex::tString>> SvrIPs;

private:
	// 所有从服务器同步任务
	ServerSyncGames	severSyncGames_;

	// 所有从服务器任务Name ID所对应的IP
	SvrIPs svrsIP_;

public:
	CSyncServer(ISvrPlugMgr* pIPlugMgr);
	~CSyncServer();

public:
	SyncGamesInfo GetSyncGame(long ip, bool first);
	bool	IsStarted(const stdex::tString &SID) const;

	void	StartSync(const stdex::tString &SID, const stdex::tString &taskName);
	void	StopSync(const stdex::tString &SID, const stdex::tString &taskName);
	void	AddGame(const stdex::tString &SID, long gid, const stdex::tString &taskName);
	void	DeleteGame(const stdex::tString &SID, long gid, const stdex::tString &taskName, bool hasFiles);
	
	DWORD   NotifyGame(const stdex::tString &SID, DWORD gid, const stdex::tString &taskName, bool bForce);

	void	SuspendTask(const stdex::tString &SID, const stdex::tString &taskName);
	void	ResumeTask(const stdex::tString &SID, const stdex::tString &taskName);
	
	DWORD   NotifySyncGame(DWORD gid, bool bForce);
	DWORD	NotifyDeleteGame(DWORD gid);
	int		handle_SetGameVer(pkgheader* pheader, char* pbuf, long length);

private:
	IRTDataSvr* rtSvr_;
};

class SyncTaskMgr;

class CSyncPlug 
	: public ISvrPlug
{
	enum GAMETYPE
	{
		sync_game,
		notify_game,
		delete_game,
	};


public:
	CSyncPlug();
	virtual ~CSyncPlug();
	void	Release()			{ delete this; }

	//implement INetLayerEvent interface.
	void	OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}
	void	OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}

	//implement IPlug interface.
	LPCTSTR GetPlugName()		{ return PLUG_SYNCDISK_NAME;						}
	DWORD  	GetVersion()		{ return 0x02000000;								}
	LPCTSTR GetCompany()		{ return TEXT("Goyoo");								}
	LPCTSTR GetDescription()	{ return TEXT("Game Sync Plug.");					}
	DWORD   GetListPort()		{ return 0;											}
	bool	Initialize(ISvrPlugMgr* pPlugMgr);
	bool	UnInitialize();
	DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);
private:
	static  UINT32 __stdcall WorkThread(LPVOID lpVoid);
	static  UINT32 __stdcall UdpThread(LPVOID lpVoid);
	bool	GetAllSyncTask();

private:
	ISvrPlugMgr*m_pIPlugMgr;
	async::thread::AutoEvent exitEvent_;
	std::auto_ptr<SyncTaskMgr> syncTaskMgr_;

	std::vector<HANDLE>				m_Threads;
	std::set<TCHAR>					m_setDriver;
	CSocket							m_Server;
};


class SyncGame;
typedef std::tr1::shared_ptr<SyncGame> SyncGamePtr;

class SyncTask;
typedef std::tr1::shared_ptr<SyncTask> SyncTaskPtr;


class SyncTaskMgr
{
public:
	typedef async::container::SyncAssocContainer<stdex::tString, SyncTaskPtr> SyncTasks;
	
private:
	SyncTasks syncTasks_;
	async::thread::AutoEvent &exitEvent_;
	CSocket &svr_;
	async::thread::AutoEvent refreshVDiskEvent_;

public:
	SyncTaskMgr(async::thread::AutoEvent &exitEvent, CSocket &svr);
	~SyncTaskMgr();

private:
	SyncTaskMgr(const SyncTaskMgr &);
	SyncTaskMgr &operator=(const SyncTaskMgr &);

public:
	void Start();
	void Stop();

public:
	void AddSyncTask(const stdex::tString &SID, const std::vector<SyncGameInfoPtr> &);
	void DelSyncTask(const stdex::tString &SID);

	void StartSyncTask(const stdex::tString &SID);
	void StopSyncTask(const stdex::tString &SID);

	void SuspendSyncTask(const stdex::tString &SID);
	void ResumeSyncTask(const stdex::tString &SID);
	
	// 
	void AddSyncGame(const stdex::tString &SID, const SyncGameInfoPtr &);
	void DeleSyncGame(const stdex::tString &SID, long gid, bool hasFiles);
	void PutTopSyncGame(const stdex::tString &SID, long gid);
	void PutBottomSyncGame(const stdex::tString &SID, long gid);
	
	void NotifySyncGame(const stdex::tString &SID, long gid, bool force);

private:
	void _ReportStatus(const db::tSyncTaskStatus &status);
	void _ReportError(const stdex::tString &SID, long gid);
};

class SyncTask
{
public:
    typedef async::container::SyncSequenceContainer<SyncGamePtr, std::list<SyncGamePtr>> SyncGameContainer;

	typedef std::tr1::function<void(const db::tSyncTaskStatus &)> ReportCallback;
	typedef std::tr1::function<void(const stdex::tString &, long)> ErrorCallback;

private:
	bool isPaused_;
	async::thread::AutoEvent exitEvent_;
	async::thread::AutoEvent &refreshVDiskEvent_;

	async::thread::ThreadImplEx thread_;
	SyncGameContainer syncTaskGames_;
	SyncGameContainer complateGames_;
    SyncGameContainer failTaskGames_;

	SyncGameContainer delSyncGames_;
	
	ReportCallback reportCallback_;
	ErrorCallback errorCallback_;

public:
	SyncTask(async::thread::AutoEvent &refreshVDiskEvent,
		const ReportCallback &report, const ErrorCallback &errorCallback);

private:
	SyncTask(const SyncTask &);
	SyncTask &operator=(const SyncTask &);

public:
	void Start();
	void Stop();
	
public:
	void AddSyncGame(const SyncGameInfoPtr &syncGameInfo, bool force);
	void DelSyncGame(long gid, bool hasFiles);

	void Suspend();
	void Resume();

	void PutTop(long gid);
	void PutBottom(long gid);

	void NotifySyncGame(long gid, bool force);


	DWORD Run();

private:
	bool _UpdateBefore();
	bool _UpdateRunning(const SyncGameInfoPtr &syncGame, const db::tSyncTaskStatus &updateStatu);
	void _UpdateAfter(const db::tSyncTaskStatus &syncStatus);
	bool _IsPaused();
};



class SyncGame
{
	typedef std::tr1::function<bool ()> UpdateBefore;
	typedef std::tr1::function<bool (const SyncGameInfoPtr &, const db::tSyncTaskStatus &)> UpdateRunning;
	typedef std::tr1::function<void (const db::tSyncTaskStatus &)> UpdateAfter;
	typedef std::tr1::function<bool ()> IsPaused;

private:
	bool isDeleted_;

	SyncGameInfoPtr info_;

	UpdateBefore updateBefore_;
	UpdateRunning updateRunning_;
	UpdateAfter updateAfter_;
	IsPaused isPaused_;
	
public:
	SyncGame(const SyncGameInfoPtr &info,
		const UpdateBefore &before, const UpdateRunning &running, const UpdateAfter &after, const IsPaused &isPaused)
		: isDeleted_(false)
		, info_(info)
		, updateBefore_(before)
		, updateRunning_(running)
		, updateAfter_(after)
		, isPaused_(isPaused)
	{}
	~SyncGame()
	{

	}

public:
	bool operator==(const SyncGame &lhs)
	{
		return info_->gid_ == lhs.info_->gid_;
	}

	long GetGid() const
	{
		return info_->gid_;
	}

	SyncGameInfoPtr &GetInfo()
	{
		return info_;
	}
	const SyncGameInfoPtr &GetInfo() const
	{
		return info_;
	}

public:
	bool Run();
	void SetDelete();
	void DeleteGameFiles();
};

inline bool operator==(const SyncGame &lhs, const SyncGame &rhs)
{
	return lhs == rhs;
}


#endif