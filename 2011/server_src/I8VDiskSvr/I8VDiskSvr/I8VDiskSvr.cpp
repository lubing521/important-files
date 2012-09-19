// I8VDiskSvr.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../../../include/Win32/System/except.h"
#include "../../../include/Win32/System/WinXPSP2FireWall.h"
#include "../../../include/Win32/WinService/win32svr.h"

#include "../../../include/Utility/utility.h"


#include "plugmgr.h"



#ifdef _DEBUG
#include "../../../include/vld.h"
#endif // _DEBUG

#ifdef _DEBUG 
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif

#pragma comment(lib, "frame.lib")


#define SERVICE_NAME			TEXT("I8VDiskSvr")
#define SERVICE_DISCRIPTION		TEXT("I8VDisk Service Manage")
#define LOG_FILENAME			TEXT("I8VDiskSvr")

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

class CI8VDiskSvr : public i8desk::CWin32Svr
{
private:
	i8desk::ILogger* m_pLogger;
	i8desk::CPlugMgr* m_pPlugMgr;
	HANDLE hTimer;
	HANDLE m_hOneInstance;

	except::MiniDump minDump_;

public:
	CI8VDiskSvr()
		: CWin32Svr(SERVICE_NAME, SERVICE_DISCRIPTION)
		, minDump_()
	{
		i8desk::CreateLogger(&m_pLogger);
		m_pPlugMgr = new i8desk::CPlugMgr(m_pLogger);
		hTimer	   = NULL;
		m_hOneInstance = NULL;
	}
	virtual ~CI8VDiskSvr() 
	{
		Release_Interface(m_pPlugMgr);
		Release_Interface(m_pLogger);
	}
public:
	void AddWindowFireWall()
	{
		WinXPSP2FireWall Fw;
		BOOL bOn = FALSE;
		Fw.Initialize();
		Fw.IsWindowsFirewallOn(bOn);
		if (bOn)
		{
			wchar_t path[MAX_PATH] = {0};
			GetModuleFileNameW(NULL, path, MAX_PATH);
			Fw.AddApplication(path, L"I8VDiskSvr");

			PathRemoveFileSpecW(path);
			lstrcatW(path, L"\\I8DeskSvr.exe");
			Fw.AddApplication(path, L"I8DeskSvr");

			PathRemoveFileSpecW(path);
			lstrcatW(path, L"\\DNAService\\DNAService.exe");
			Fw.AddApplication(path, L"DNAS");
		}
		Fw.Uninitialize();
	}

	inline void HideDosWindow()
	{
#ifndef _DEBUG
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(m_pLogger));
#endif
	}
	
	DWORD OneInstance()
	{
		utility::CEvenyOneSD sd;
		m_hOneInstance = CreateEvent(sd.GetSA(), TRUE, FALSE, TEXT("__i8vdisksvr_one_instance__"));
		if (m_hOneInstance == NULL)
			return GetLastError();

		if (ERROR_ALREADY_EXISTS == GetLastError())
			return -1;
		return 0;
	}

	virtual DWORD OnStart()
	{
		if (OneInstance())
		{
			Stop();
			return -1;
		}

		AddWindowFireWall();
		if (IsService())
			ChangeConfig();
		else
		{
			CWin32Svr::StopService(_T("i8vdisksvr"));
			CWin32Svr::RemoveService(_T("i8vdisksvr"));
		}

		CreateTimerQueueTimer(&hTimer, NULL, TimerRoutine, this, 5000, 1000, 0);

		m_pLogger->SetLogFileName(LOG_FILENAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("================================================================="));
		m_pLogger->SetAddDateBefore(true);
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("Start I8VDisk Service..."));
	
		if (!m_pPlugMgr->Start())
		{
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("start plugmgr fail."));
			Stop();
			return ERROR_SUCCESS;
		}
		HideDosWindow();

		return ERROR_SUCCESS;
	}

	virtual DWORD OnStop()
	{
		if (hTimer != NULL)
		{
			DeleteTimerQueueTimer(NULL, hTimer, NULL);
			hTimer = NULL;
		}
		if (m_hOneInstance != NULL)
		{
			CloseHandle(m_hOneInstance);
			m_hOneInstance = NULL;
		}
		ReportStatusToSCM(SERVICE_STOP_PENDING);
		if (m_pPlugMgr != NULL)
		{
			m_pPlugMgr->Stop();
		}
		if (m_pLogger)
		{
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("I8VDisk Service already stoped."));
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("==============================================================\r\n\r\n"));
		}
		ReportStatusToSCM(SERVICE_STOPPED);

		return ERROR_SUCCESS;
	}
	virtual DWORD OnShutdown()
	{
		OnStop();
		return ERROR_SUCCESS;
	}
private:
	static VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		SYSTEMTIME st = {0};
		GetLocalTime(&st);
		if (st.wHour == 2 && st.wMinute == 0 && st.wSecond == 0)
		{
			TCHAR szExe[MAX_PATH] = {0};
			szExe[0] = '\"';
			GetModuleFileName(NULL, &szExe[1], MAX_PATH-2);
			lstrcat(szExe, _T("\" -AutoStart"));

			STARTUPINFO si = {0};
			PROCESS_INFORMATION pi = {0};
			si.cb= sizeof(STARTUPINFO);
			if (CreateProcess(NULL, szExe, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
		}

		CI8VDiskSvr *pThis = reinterpret_cast<CI8VDiskSvr *>(lpParam);
		pThis->HideDosWindow();
	}
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	static bool bHide = false;
	TCHAR szClassName[MAX_PATH] = {0};
	GetClassName(hwnd, szClassName, _countof(szClassName));

	if( bHide == true )
		return TRUE;

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
			bHide = true;
			ShowWindow(hwnd, SW_HIDE);
			return TRUE;
		}
	}
	return TRUE;
}

CI8VDiskSvr g_service;

#include <Psapi.h>
static void KillProcess(LPCTSTR lpszSerive)
{
	//停止服务变成kill process.
	DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
	TCHAR path[MAX_PATH] = {0};
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return	;
	cProcesses = cbNeeded / sizeof(DWORD);
	for (DWORD i = 0; i < cProcesses; i++ )
	{
		utility::CAutoHandle hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
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

void StartProcess(LPCTSTR lpszPath)
{
	TCHAR path[MAX_PATH] = {0};
	stdex::tString tmp = utility::GetAppPath() + lpszPath;
	_tcscpy(path, tmp.c_str());

	::OutputDebugString(path);

	STARTUPINFO si	= {sizeof(si)};
	si.dwFlags		= STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_HIDE;
	PROCESS_INFORMATION pi = {0};
	BOOL ret = ::CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if( ret )
	{
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if( argc >= 2 )
		::OutputDebugString(argv[1]);

	//程序自动重启
	if (argc == 2 && lstrcmpi(argv[1], TEXT("-A")) == 0)
	{
		try
		{
			i8desk::CWin32Svr::StopService(_T("I8VDiskSvr"));
			KillProcess(_T("i8NodeServer.exe"));
		}
		catch(...)
		{ 
			// do nothing
		}

		Sleep(500);

		// 重启服务
		i8desk::CWin32Svr::StartService(_T("I8VDiskSvr"));
		//StartProcess(_T("I8VDiskSvr.exe -d"));

		return 0;
	}

	g_service.ProcessCmdLine(argc, argv);
	return 0;
}