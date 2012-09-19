#ifndef __i8desk_plugmgr_inc__
#define __i8desk_plugmgr_inc__

#include <IPHlpApi.h>
#include <Shlwapi.h>
#include <process.h>
#include <string>
#include <map>
#include <memory>

#include "../../../include/frame.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "socket.h"
#include "AutoUpdate.h"


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


#define HEART_INTERVAL			10000
#define VDISK_EVENT_NAME		TEXT("__i8desk_vdisk_refresh_event_")

namespace i8desk
{

	class CPlugMgr 
		: public ISvrPlugMgr
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

	public:
		CPlugMgr(ILogger* pLogger);
		virtual ~CPlugMgr();

	public:
		virtual void Release() { delete this; }
		
		virtual IDatabase*	GetDatabase()	{ return NULL; }
		virtual ISysOpt*	GetSysOpt()		{ return NULL; }
		virtual ILogger*	CreateLogger()  { return NULL; }
		virtual ISvrPlug*	FindPlug(LPCTSTR plugName);
		virtual bool		Start();
		virtual bool		Stop();

		BOOL CPlugMgr::IsLocalMachine(LPCTSTR lpszIP);
		stdex::tString GetAppPath()
		{
			TCHAR path[MAX_PATH] = {0};
			GetModuleFileName(NULL, path, MAX_PATH);
			PathRemoveFileSpec(path);
			PathAddBackslash(path);
			return stdex::tString(path);
		}
	private:
		ISvrPlug* LoadPlug(stdex::tString& szPlugDLL, ISvrPlugMgr* pPlugMgr, DWORD);
		static UINT __stdcall WorkThread(LPVOID lpVoid);
		static UINT __stdcall RecvThread(LPVOID lpVoid);

	private:
		ILogger* m_pLogger;
		std::auto_ptr<CSocket> sock_;
		std::auto_ptr<AutoUpdateFile> update_;

		Mutex    m_lock;
		DWORD m_bInnerUpdate;
		std::map<stdex::tString, ISvrPlug*> m_mapPlugs;
		std::map<DWORD, tagVDInfoEx*> VDList_;

		typedef async::thread::AutoCriticalSection Mutex;
		typedef async::thread::AutoLock<Mutex> Lock;

		Mutex mutex_;
		HANDLE m_hThread;
		HANDLE m_hRcvThread;
		HANDLE m_hExited;
	};
};

#endif