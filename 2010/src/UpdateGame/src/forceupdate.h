#ifndef __force_update_inc__
#define __force_update_inc__

#include "public.h"

//declare client plugmgr, plug interface.
struct ICliPlug;
struct ICommunication;

#define MAX_TASK_NUMS 2000

struct ICliPlugMgr
{
	virtual void			Release()					= 0;
	virtual ILogger*		CreateLogger()				= 0;
	virtual ICommunication*	GetCommunication()			= 0;
	virtual ICliPlug*		FindPlug(LPCTSTR plugName)	= 0;
	virtual bool			Start()						= 0;
	virtual void			Stop()						= 0;
};

struct ICliPlug
{
	virtual void			Release()							= 0;
	virtual LPCTSTR			GetPlugName()						= 0;
	virtual DWORD  			GetVersion() 						= 0;
	virtual LPCTSTR			GetCompany()						= 0;
	virtual LPCTSTR			GetDescription() 					= 0;
	virtual bool			NeedUdpNotity()						= 0;
	virtual bool			Initialize(ICliPlugMgr* pPlugMgr)	= 0;
	virtual bool			UnInitialize() 						= 0;
	virtual DWORD			SendMessage(DWORD cmd, DWORD param1, DWORD param2) = 0;
	virtual void			UdpRecvNotify(char* pData, int length, sockaddr_in& from) = 0;
};

//implement client force plug.
class CForceUpdatePlug : public ICliPlug
{
	typedef struct tagUpGameinfo
	{
		DWORD			dwGid;
		FC_COPY_MODE	dwFlag;
		DWORD			SrvIp;
		DWORD			dwSpeed;
		DWORD           Resv;
		char			SvrPath[MAX_PATH];
		char			CliPath[MAX_PATH];
	}UpGameinfo;
public:
	CForceUpdatePlug():m_hThread(NULL), m_hExited(NULL), m_pILogger(NULL), m_pPipe(NULL) 
	{
		CEvenyOneSD sd;
		m_hExited     = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hMenuUpdate = CreateEvent(sd.GetSA(), TRUE, FALSE, UPDATE_EVENT_NAME);
	}
	virtual ~CForceUpdatePlug() 
	{
		if (m_hExited != NULL)
		{
			CloseHandle(m_hExited);
			m_hExited = NULL;
		}
		if (m_hMenuUpdate != NULL)
		{
			CloseHandle(m_hMenuUpdate);
			m_hMenuUpdate = NULL;
		}
	}
public:
	virtual void			Release()						{ delete this;						}
	virtual LPCTSTR			GetPlugName()					{ return TEXT("ForceUpdate_Plug");	}
	virtual DWORD  			GetVersion() 					{ return 0x02000000;				}
	virtual LPCTSTR			GetCompany()					{ return TEXT("Goyoo");				}
	virtual LPCTSTR			GetDescription() 				{ return TEXT("forceupdate plug");	}
	virtual bool			NeedUdpNotity()					{ return false;						}
	virtual bool			Initialize(ICliPlugMgr* pPlugMgr);
	virtual bool			UnInitialize();
	virtual DWORD			SendMessage(DWORD cmd, DWORD param1, DWORD param2);
	virtual void			UdpRecvNotify(char* pData, int length, sockaddr_in& from) {			}
private:
	static UINT32 __stdcall WorkThreadProc(LPVOID lpVoid);
	void SyncGame();
	void __cdecl WriteLog(char* formater, ...);
private:
	HANDLE					m_hThread;
	HANDLE					m_hExited;
	HANDLE					m_hMenuUpdate;
	CLock					m_lockWriteLog;
	ILogger*				m_pILogger;
	PCMomule<UpGameinfo, _func_<UpGameinfo>, MAX_TASK_NUMS>*   m_pPipe;
};

#endif