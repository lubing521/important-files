#ifndef __i8desk_plugmgr_inc__
#define __i8desk_plugmgr_inc__

#include <IPHlpApi.h>
#include <Shlwapi.h>
#include <process.h>
#include <string>
#include <map>
#include <memory>

#include "../../../include/plug.h"
#include "socket.h"
#include "AutoUpdate.h"


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define CONFIG_FILE				"Data\\I8SyncSvr.ini"
#define HEART_INTERVAL			10000
#define VDISK_EVENT_NAME		TEXT("__i8desk_vdisk_refresh_event_")

namespace i8desk
{

	class CPlugMgr : IPlugMgr
	{
	public:
		CPlugMgr(ILogger* pLogger);
		virtual ~CPlugMgr();

	public:
		virtual void Release() { delete this; }
		
		virtual IDatabase*	GetDatabase()	{ return NULL; }
		virtual ISysOpt*	GetSysOpt()		{ return NULL; }
		virtual ILogger*	CreateLogger()  { return NULL; }
		virtual IPlug*		FindPlug(LPCTSTR plugName);
		virtual bool		Start();
		virtual bool		Stop();

		BOOL CPlugMgr::IsLocalMachine(LPCTSTR lpszIP);
		tstring GetAppPath()
		{
			TCHAR path[MAX_PATH] = {0};
			GetModuleFileName(NULL, path, MAX_PATH);
			PathRemoveFileSpec(path);
			PathAddBackslash(path);
			return tstring(path);
		}
	private:
		IPlug* LoadPlug(std::string& szPlugDLL, IPlugMgr* pPlugMgr);
		static UINT __stdcall WorkThread(LPVOID lpVoid);


	private:
		ILogger* m_pLogger;
		CSocket  *sock_;
		AutoUpdateFile *update_;

		CLock    m_lock;
		DWORD m_bInnerUpdate;
		std::map<tstring, IPlug*> m_mapPlugs;

		HANDLE m_hThread;
		HANDLE m_hExited;
	};
};

#endif