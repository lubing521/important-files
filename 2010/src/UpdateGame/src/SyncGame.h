#ifndef __sync_game_inc__
#define __sync_game_inc__

#include "public.h"
#include "socket.h"
#include <map>
#include <list>
#include <set>
#include <vector>
#include <string>
#include "UpdateGame.h"

std::string GetMainSvrIp();

class CSyncServer
{
public:
	CSyncServer(IPlugMgr* pIPlugMgr);
	~CSyncServer();
public:
	DWORD   NotifySyncGame(DWORD gid, bool bForce);
	DWORD	NotifyDeleteGame(DWORD gid);
	int		handle_GetAllTask (_packageheader* pheader, char* pbuf);
	int		handle_GetTaskGame(_packageheader* pheader, char* pbuf);
	int		handle_SetGameVer(_packageheader* pheader, char* pbuf);
private:
	void    SendUdpPkg(char* pPackage, int length);
private:
	IPlugMgr* m_pIPlugMgr;
};

class CSyncPlug : public IPlug
{
	struct FastCopyStatusEx: public FastCopyStatus
	{

		bool bConnect;
		DWORD gid;
		char GameName[MAX_PATH];
	};
	typedef struct tagGameInfo
	{
		DWORD	gid;
		DWORD	size;
		DWORD   version;
		DWORD	MaxSpeed;
		char	Name[64];
		char	CliIp[20];
		char	SvrPath[MAX_PATH];
		char	CliPath[1024];
		bool	needsync;
	}tagGameInfo;
	enum GAMETYPE
	{
		sync_game,
		notify_game,
		delete_game
	};
public:
	CSyncPlug() : m_pIPlugMgr(NULL), m_pILogger(NULL), m_hExited(NULL), m_hRefreshVDisk(NULL), m_hNotifySyncAll(NULL)
	{
		CEvenyOneSD sd;
		m_hRefreshVDisk = CreateEvent(sd.GetSA(), FALSE, FALSE, VDISK_EVENT_NAME);
		m_pFastCopyStatus = new FastCopyStatusEx;
		ZeroMemory(m_pFastCopyStatus, sizeof(FastCopyStatusEx));
	}
	virtual ~CSyncPlug()
	{
		if (m_hRefreshVDisk != NULL)
		{
			CloseHandle(m_hRefreshVDisk);
			m_hRefreshVDisk = NULL;
		}
		Delete_Pointer(m_pFastCopyStatus);
	}
	void	Release()			{ delete this; }

	//implement INetLayerEvent interface.
	void	OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}
	void	OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}

	//implement IPlug interface.
	LPCTSTR GetPlugName()		{ return SYNC_PLUG_NAME;							}
	DWORD  	GetVersion()		{ return 0x02000000;								}
	LPCTSTR GetCompany()		{ return TEXT("Goyoo");								}
	LPCTSTR GetDescription()	{ return TEXT("Game Sync Plug.");					}
	DWORD   GetListPort()		{ return 0;											}
	bool	Initialize(IPlugMgr* pPlugMgr);
	bool	UnInitialize();
	DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);
private:
	static  UINT32 __stdcall WorkThread(LPVOID lpVoid);
	static  UINT32 __stdcall UdpThread(LPVOID lpVoid);
	bool	GetAllSyncTask(bool bNotifyUI);
	void	AddGame(DWORD gid, bool bForce, GAMETYPE type);
	bool    GetNextGame(DWORD& gid, bool& bForce, GAMETYPE type);
	void	SyncGame(DWORD gid, bool bForce);
	void	DeleteGame(DWORD gid);
	void	__cdecl WriteLog(char* formater, ...);

	void	SendUdp2UI(char* pData, int length);
	void	SendSyncFinish2UI(DWORD gid, bool bSuccess);
	void	NotifyUiRefresh();
	void	RefreshUIData();
private:
	IPlugMgr*   m_pIPlugMgr;
	ILogger*	m_pILogger;
	HANDLE		m_hExited;
	HANDLE		m_hRefreshVDisk;
	CLock		m_lockWriteLog;
	FastCopyStatusEx* m_pFastCopyStatus;

	//所有需要同步的游戏的信息
	CLock							m_lockAllGame;
	std::map<DWORD, tagGameInfo*>   m_mapAllGame;
	std::set<char>					m_setDriver;

	//自动获取的同步游戏列表
	CLock							m_lockSync;
	std::map<DWORD, DWORD>			m_mapSync;

	//通知需要同步的游戏列表
	HANDLE							m_hNotifySyncAll;
	CLock							m_lockNotify;
	std::map<DWORD, bool>			m_mapNotify;

	//删除游戏的列表
	CLock							m_lockDelete;
	std::list<DWORD>				m_lstDelete;

	std::vector<HANDLE>				m_Threads;

	CSocket							m_Server;
};

#endif