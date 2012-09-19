#include "stdafx.h"
#include <atlenc.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

#define CRASH_DUMP_FILE		TEXT("I8DeskSvr")

#include "../../include/except.h"
#include "../../include/WinXPSP2FireWall.h"
#include "../../include/Utility/SmartHandle.hpp"
#include "../../include/Utility/SmartPtr.hpp"

#include "win32svr.h"
#include "plugmgr.h"
#include "update.h"
#include "backup.h"
#include "CleanLog.h"

#ifdef _DEBUG
#include "../../include/vld.h"
#endif

#define LOG_FILENAME	TEXT("I8DeskSvr")

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);


// forward declare
namespace i8desk
{
	class CleanLog;
}


class CI8DeskSvr : public i8desk::CWin32Svr
{
private:
	i8desk::ILogger*	m_pLogger;
	i8desk::CPlugMgr*	m_plugMgr;
	i8desk::CAutoUpate* m_pUpdate;
	i8desk::CBackup*	m_pBackup;

	std::auto_ptr<i8desk::CleanLog> cleanlog_;
	HANDLE				m_hOneInstance;
public:
	CI8DeskSvr():CWin32Svr(TEXT("I8DeskSvr"), TEXT("I8Desk Server Manage")), 
		m_pLogger(NULL), m_plugMgr(NULL), m_pUpdate(NULL)
	{
		CoInitialize(NULL);
		SetUnhandledExceptionFilter(::Local_UnhandledExceptionFilter);
		CreateLogger(&m_pLogger);
		m_plugMgr = NULL;
		m_hOneInstance = NULL;
	}
	virtual ~CI8DeskSvr() {  Release_Interface(m_pLogger); }
public:
	static void KillService(LPCTSTR lpszSerive)
	{
		//CWin32Svr::StopService("DNAS");
		
		//停止服务变成kill process.
		DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		char path[MAX_PATH] = {0};
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return	;
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			i8desk::CAutoHandle hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
			if (hProcess.IsValid() && GetCurrentProcessId() != aProcesses[i])
			{
				GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
				if (lstrcmpi(path, lpszSerive) == 0)
				{
					TerminateProcess(hProcess, 0);
					return ;
				}
			}
		}
	}

	void RegDNAService()
	{
		m_pLogger->WriteLog(i8desk::LM_INFO, "Start Registry DNAService...");
		char path[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, path, MAX_PATH);
		PathRemoveFileSpecA(path);
		PathAddBackslashA(path);
		lstrcatA(path, "DNAService\\DNAService.exe");
		std::string szDNAService(path);
		szDNAService.insert(0, "\"");
		szDNAService += "\"";

		std::pair<std::string, bool> cmd[] =
		{
			std::make_pair("-UnRegServer",	true),
			std::make_pair("-RegServer",	true),
			std::make_pair("-Service",		true),
		};
		if (IsService())
		{
			i8desk::CAutoService schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (schSCManager.IsValid())
			{
				i8desk::CAutoService schService = OpenService(schSCManager, "dnas", SERVICE_ALL_ACCESS);
				if (schService.IsValid())
				{
					char buf[4096] = {0};
					LPQUERY_SERVICE_CONFIG lpConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(buf);
					DWORD dwSize = sizeof(buf);
					DWORD dwNeedSize = 0;
					QueryServiceConfig(schService, lpConfig, dwSize, &dwNeedSize);
					//服务存在，并且路径正确，则认为注册是成功的，不需要重新注册。
					if (_strcmpi(lpConfig->lpBinaryPathName, szDNAService.c_str()) == 0)
					{
						m_pLogger->WriteLog(i8desk::LM_INFO, "Registry DNAService Success.");
						return ;
					}
				}
			}
		}
		else
		{
			cmd[2].second = false;
		}

		for (int idx=0; idx<_countof(cmd);idx++)
		{
			if (!cmd[idx].second)
				continue;

			char line[1024] = {0};
			_stprintf(line, "%s %s", szDNAService.c_str(), cmd[idx].first.c_str());

			STARTUPINFO si = {sizeof(si)};
			PROCESS_INFORMATION pi = {0};

			m_pLogger->WriteLog(i8desk::LM_INFO, "cmd:%s", line);
			ReportStatusToSCM(SERVICE_START_PENDING);
			if (!CreateProcessA(NULL, line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(i8desk::LM_INFO, "[%s]:fail:%d:%s", line, dwError, Error.ErrorMessage());
				return ;
			}
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		m_pLogger->WriteLog(i8desk::LM_INFO, "Registry DNAService Success.");
	}

	void AddWindowFireWall()
	{
		WinXPSP2FireWall Fw;
		BOOL bOn = FALSE;
		Fw.IsWindowsFirewallOn(bOn);
		if (bOn)
		{
			wchar_t path[MAX_PATH] = {0};
			GetModuleFileNameW(NULL, path, MAX_PATH);
			Fw.AddApplication(path, L"I8DeskSvr");

			PathRemoveFileSpecW(path);
			lstrcatW(path, L"\\I8VDiskSvr.exe");
			Fw.AddApplication(path, L"I8VDiskSvr");

			PathRemoveFileSpecW(path);
			lstrcatW(path, L"\\DNAService\\DNAService.exe");
			Fw.AddApplication(path, L"DNAS");

			GetModuleFileNameW(NULL, path, MAX_PATH);
			PathRemoveFileSpecW(path);
			lstrcatW(path, L"\\SafeSvr\\KHPrtSvr.exe");
			Fw.AddApplication(path, L"I8DeskSafeSvr");
		}
	}

	DWORD OneInstance()
	{
		i8desk::CEvenyOneSD sd;
		m_hOneInstance = CreateEvent(sd.GetSA(), TRUE, FALSE, TEXT("__i8desksvr_one_instance__"));
		if (m_hOneInstance == NULL)
			return GetLastError();

		if (ERROR_ALREADY_EXISTS == GetLastError())
			return -1;
		return 0;
	}

	inline void HideDosWindow()
	{
		//if (!i8desk::AfxGetWin32Svr()->IsService())
			EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(m_pLogger));
	}

	virtual DWORD OnStart()
	{
		m_pLogger->SetLogFileName(LOG_FILENAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("=============================================================="));
		m_pLogger->SetAddDateBefore(true);

		if (OneInstance())
		{
			Stop();
			return -1;
		}

		HideDosWindow();
		AddWindowFireWall();

		if (IsService())	//改变服务的配置
			ChangeConfig();
		else				//如果是-d运行，停止服务，并删除服务
		{
			CWin32Svr::StopService("i8desksvr");
			CWin32Svr::RemoveService("i8desksvr");
			CWin32Svr::RemoveService("dnas");
		}
		
		RegDNAService();
		
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("Create Core interface."));
		m_plugMgr = new i8desk::CPlugMgr(m_pLogger);
		m_pUpdate = new i8desk::CAutoUpate(m_pLogger, m_plugMgr);
		m_pBackup = new i8desk::CBackup(m_pLogger);
		cleanlog_.reset(new i8desk::CleanLog(m_pLogger));

		ReportStatusToSCM(SERVICE_START_PENDING);
		if (!m_plugMgr->Start() 
			|| !m_pUpdate->StartUpdate() 
			|| !m_pBackup->StartBackup()
			/*|| !cleanlog_->Start()*/ )
		{
			Stop();
			return ERROR_SUCCESS;
		}
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("%s already started."), GetSvrName());

		HideDosWindow();

		return ERROR_SUCCESS;
	}

	virtual DWORD OnStop()
	{
		ReportStatusToSCM(SERVICE_STOP_PENDING);

		/*if( cleanlog_.get() )
			cleanlog_->Stop();*/

		if (m_pBackup)
		{
			m_pBackup->StopBackup();
			delete m_pBackup;
			m_pBackup = NULL;
		}
		if (m_pUpdate)
		{
			m_pUpdate->StopUpdate();
			delete m_pUpdate;
			m_pUpdate = NULL;
		}

		if (m_plugMgr)
		{
			m_plugMgr->Stop();
			Release_Interface(m_plugMgr);
		}

		ReportStatusToSCM(SERVICE_STOP_PENDING);

		KillService("DNAService.exe");

		if (m_pLogger)
		{
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("%s: stoped."), GetSvrName());
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("==============================================================\r\n\r\n"));
		}
		
		return ERROR_SUCCESS;
	}
	virtual DWORD OnShutdown()
	{
		OnStop();
		return ERROR_SUCCESS;
	}
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
#ifndef _DEBUG
	TCHAR szClassName[MAX_PATH] = {0};
	GetClassName(hwnd, szClassName, _countof(szClassName));

	if (_tcsicmp(szClassName, TEXT("ConsoleWindowClass")) == 0)
	{
		TCHAR szWinText[MAX_PATH] = {0};
		TCHAR szExe[MAX_PATH] = {0};
		GetWindowText(hwnd, szWinText, _countof(szWinText));
		GetModuleFileName(NULL, szExe, _countof(szExe));
		_tcslwr(szWinText);
		_tcslwr(szExe);

		if (_tcsstr(szWinText, szExe) != 0)
		{
			ShowWindow(hwnd, SW_HIDE);
			i8desk::ILogger* pLogger = reinterpret_cast<i8desk::ILogger*>(lParam);
			if (pLogger)
			{
				pLogger->WriteLog(i8desk::LM_INFO, "i8desk hide console windows.");
			}
			return TRUE;
		}
	}
	return TRUE;

#else
	return TRUE;
#endif
}

CI8DeskSvr g_service;

int _tmain(int argc, _TCHAR* argv[])
{
	//过虑以带参方式来启动程序
	for (int idx=1; idx<argc; idx++)
	{
		if (strcmpi(argv[idx], TEXT("-AutoUpdate")) == 0)		//以自动更新方式启动，则是停止原来的服务，再启动.再删除临时文件
		{
			//停止三个系统服务
			try
			{
				CI8DeskSvr::KillService("DNAService.exe");
				i8desk::CWin32Svr::StopService("I8VDiskSvr");
				i8desk::CWin32Svr::StopService("I8DeskSvr");
				CI8DeskSvr::KillService("I8DeskSvr.exe");
			}
			catch (...){ }
			Sleep(1000);

			//重启服务
			i8desk::CWin32Svr::StartService("I8VDiskSvr");
			bool bIsService = false;
			{
				i8desk::CAutoService hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
				if (hSCM.IsValid())
				{
					i8desk::CAutoService hService = OpenService(hSCM, TEXT("I8DeskSvr"), SERVICE_START | SERVICE_QUERY_STATUS);
					if (hService.IsValid())
					{
						bIsService = true;
					}
				}
			}
			if (bIsService)
				i8desk::CWin32Svr::StartService("I8DeskSvr");
			else
			{
				char path[MAX_PATH] = {0};
				GetModuleFileNameA(NULL, path, MAX_PATH);
				PathRemoveFileSpecA(path);
				PathAddBackslashA(path);
				lstrcatA(path, "I8DeskSvr.exe -debug");
				
				STARTUPINFO			si = {0};
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi = {0};

				if (!CreateProcessA(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
					return 0;
				WaitForSingleObject( pi.hProcess, INFINITE );
				CloseHandle( pi.hProcess );
				CloseHandle( pi.hThread );
			}

			return 0;
		}
		else if (strcmpi(argv[idx], TEXT("-encode")) == 0)			//编码目录下的i8desksys.config
		{
			tstring szSysConfig = i8desk::GetAppPath() + TEXT("Oem\\oem.ini");
			SetFileAttributes(szSysConfig.c_str(), FILE_ATTRIBUTE_NORMAL);
			i8desk::CAutoFile hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0, NULL);
			if ( hFile.IsValid() )
			{
				DWORD dwSize = GetFileSize(hFile, NULL);
				ATL::CAutoVectorPtr<BYTE> pSrc(new BYTE[dwSize]);

				DWORD dwReadBytes = 0;
				ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL);

				int dwNewSize = ATL::Base64EncodeGetRequiredLength(dwSize);
				ATL::CAutoVectorPtr<char> pDst(new char[dwNewSize]);
				ATL::Base64Encode(pSrc, dwSize, pDst, &dwNewSize);
				
				
				i8desk::CAutoFile hFileConfig = CreateFile(szSysConfig.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if( hFileConfig.IsValid() )
				{
					WriteFile(hFileConfig, pDst, dwNewSize, &dwReadBytes, NULL);
					SetEndOfFile(hFileConfig);
				}

			}
			return 0;
		}
		else if (strcmpi(argv[idx], TEXT("-decode")) == 0)			//解码目录下的i8desksys.config
		{
			tstring szSysConfig = i8desk::GetAppPath() + TEXT("Oem\\oem.ini");
			SetFileAttributes(szSysConfig.c_str(), FILE_ATTRIBUTE_NORMAL);
			i8desk::CAutoFile hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0, NULL);
			if (hFile.IsValid())
			{
				DWORD dwSize = GetFileSize(hFile, NULL);
				ATL::CAutoVectorPtr<char> pSrc(new char[dwSize]);
				DWORD dwReadBytes = 0;
				ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL);

				int dwNewSize = dwSize;
				ATL::CAutoVectorPtr<BYTE> pDst(new BYTE[dwNewSize]);
				ATL::Base64Decode(pSrc, dwSize, pDst, &dwNewSize);
				i8desk::CAutoFile hFileConfig = CreateFile(szSysConfig.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if( hFileConfig.IsValid() )
				{
					WriteFile(hFileConfig, pDst, dwNewSize, &dwReadBytes, NULL);
					SetEndOfFile(hFileConfig);
				}
			}
			return 0;
		}
	}

	//以服务或者其它方式来运行程序.
	g_service.ProcessCmdLine(argc, argv);
	return 0;
}