#ifndef __force_update_inc__
#define __force_update_inc__

#include "public.h"
#define MAX_TASK_NUMS 2000

//implement client force plug.
class CForceUpdatePlug : public ICliPlug
{
	typedef struct tagUpGameinfo
	{
		DWORD			dwGid;
		DWORD			dwFlag;//对比方式
		DWORD			SrvIp;
		DWORD			dwSpeed;
		DWORD           Resv;
		TCHAR			Tid[MAX_GUID_LEN];
		TCHAR			SvrPath[MAX_PATH];
		TCHAR			CliPath[MAX_PATH];
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
	virtual LPCTSTR			GetPlugName()					{ return PLUG_FORCEUPT_NAME;		}
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
	void __cdecl WriteLog(LPCTSTR formater, ...);
private:
	typedef async::thread::AutoCriticalSection MetuxLock;
	typedef async::thread::AutoLock<MetuxLock>	AutoLock;

	HANDLE					m_hThread;
	HANDLE					m_hExited;
	HANDLE					m_hMenuUpdate;
	MetuxLock				m_lockWriteLog;
	ILogger*				m_pILogger;
	PCMomule<UpGameinfo, _func_<UpGameinfo>, MAX_TASK_NUMS>*   m_pPipe;
};
#endif