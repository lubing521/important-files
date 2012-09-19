#include "stdafx.h"
#include "Manager.hpp"

#include "../../../../include/Win32/WinService/win32svr.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Win32/FileSystem/IniConfig.hpp"

namespace mgr
{
	LPCTSTR MainServerName	= _T("I8DeskSvr");
	LPCTSTR VDiskServerName = _T("I8VDiskSvr");

	ServiceMgr::ServiceMgr()
	{

	}


	bool ServiceMgr::IsManService() const
	{
		return i8desk::CWin32Svr::HasService(MainServerName);
	}

	bool ServiceMgr::IsVDiskService() const
	{
		return i8desk::CWin32Svr::HasService(VDiskServerName);
	}

	bool ServiceMgr::IsRunMainService() const
	{
		return i8desk::CWin32Svr::IsServiceRunning(MainServerName);
	}

	bool ServiceMgr::IsRunVDiskService() const
	{
		return i8desk::CWin32Svr::IsServiceRunning(VDiskServerName);
	}

	bool ServiceMgr::RunMainService()
	{
		return i8desk::CWin32Svr::StartService(MainServerName) == ERROR_SUCCESS;
	}


	bool ServiceMgr::RunVDiskService()
	{
		return i8desk::CWin32Svr::StartService(VDiskServerName) == ERROR_SUCCESS;
	}


	bool ServiceMgr::StopMainService()
	{
		return i8desk::CWin32Svr::StopService(MainServerName) == ERROR_SUCCESS;
	}

	bool ServiceMgr::StopVDiskService()
	{
		return i8desk::CWin32Svr::StopService(VDiskServerName) == ERROR_SUCCESS;
	}

	void ServiceMgr::RunMainConfig()
	{
		stdex::tString appPath = utility::GetAppPath() + _T("Console.exe");
		ShellExecute(0, _T("open"), appPath.c_str(), 0, 0, SW_SHOW);

	}

	void ServiceMgr::RunVDiskConfig(const RunVDiskCallback &callback)
	{
		const stdex::tString appPath = utility::GetAppPath() + _T("Data\\I8SyncSvr.ini");
		stdex::tString val = i8desk::GetIniConfig(_T("System"), _T("SvrIP"), _T("127.0.0.1"), appPath.c_str());

		stdex::tString newVal = callback(std::tr1::cref(val));
		bool suc = i8desk::SetIniConfig(_T("System"), _T("SvrIP"), newVal.c_str(), appPath.c_str());
		assert(suc);
	}


}