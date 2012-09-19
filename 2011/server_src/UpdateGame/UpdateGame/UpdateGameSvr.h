#ifndef __update_game_svr_inc__
#define __update_game_svr_inc__

#include "public.h"
#include "SyncGame.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/Container/SyncContainer.hpp"

#import "msxml3.dll"

class CXmlDatabase
{
public:
	CXmlDatabase();
	~CXmlDatabase();
public:
	//注意所有使用ip的地主在xml都是保存为有符号数。因为控制台是当成有符号数来对待的。
	void	AddUpdateGame(DWORD ip, DWORD gid, DWORD type, DWORD speed, DWORD progress, 
						  __int64 uptbyte, __int64 leftbyte, LPCWSTR tid, LPCTSTR svrDir, LPCTSTR nodeDir);
	void	RemoveUpdateGame(DWORD ip);
	void	AddDownloadGame(DWORD gid, DWORD ip, DWORD version, LPCWSTR dir);
	char*  GetUpdateGame(DWORD type);
	BYTE*  GetIpHaveGame(DWORD ip);
	BYTE*  GetGameInMachine(DWORD gid);
private:
	void	SaveLocXmlDb();
private:
	typedef async::thread::AutoCriticalSection	MetuxLock;
	typedef async::thread::AutoLock<MetuxLock>	AutoLock;

	MetuxLock	m_lock;
	MSXML2::IXMLDOMDocumentPtr m_XmlLocDb;
	MSXML2::IXMLDOMDocumentPtr m_xmlMemDb;
};


class CUpdateSvrPlug : public ISvrPlug
{
public:
	typedef async::container::SyncSequenceContainer<db::tSyncTaskStatus, std::list<db::tSyncTaskStatus>> SyncStatus;

public:
	CUpdateSvrPlug();
	virtual ~CUpdateSvrPlug();
	void	Release()			{ delete this; }

	//implement INetLayerEvent interface.
	void	OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}
	void	OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)	{}
	void	OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)					{}

	//implement IPlug interface.
	LPCTSTR GetPlugName()		{ return PLUG_UPTSVR_NAME;							}
	DWORD  	GetVersion()		{ return 0x02000000;								}
	LPCTSTR GetCompany()		{ return TEXT("Goyoo");								}
	LPCTSTR GetDescription()	{ return TEXT("Game Update Server Plug.");			}
	DWORD   GetListPort()		{ return 0;											}
	bool	Initialize(ISvrPlugMgr* pPlugMgr);
	bool	UnInitialize();
	DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);

public:
	IRTDataSvr *GetRtSvr()
	{ return rtSvr_; }

private:
	static UINT32 __stdcall ListenThreadProc(LPVOID lpVoid);
	static UINT32 __stdcall WorkThreadProc(LPVOID lpVoid);
	void   __cdecl WriteLog(LPCTSTR formater, ...);
private:
	typedef async::thread::AutoCriticalSection	MetuxLock;
	typedef async::thread::AutoLock<MetuxLock>	AutoLock;

	HANDLE			m_hExited;
	HANDLE			m_hThread;
	ISvrPlugMgr*	m_pIPlugMgr;
	IRTDataSvr*		rtSvr_;
	CXmlDatabase*	m_pXmlDb;
	CSyncServer*	m_pSyncServer;
	CSocket*		m_pServer;
	ILogger*		m_pILogger;
	MetuxLock		m_lockWriteLog;

	
	SyncStatus syncStatus_;
};

#endif