#include "stdafx.h"
#include "plugmgr.h"
#include "update.h"
#include "backup.h"
#include <Psapi.h>
#include <process.h>


#include "../../include/Win32/System/WinXPSP2FireWall.h"
#include "../../include/Win32/System/SystemHelper.hpp"
#include "../../include/Utility/utility.h"

#pragma comment(lib, "psapi.lib")

#define SERVICE_NAME			TEXT("I8DeskSvr")
#define SERVICE_DISCRIPTION		TEXT("I8Desk Server Manage")
#define SERVICE_PATH			(utility::GetAppPath() + _T("I8DeskSvr.exe")).c_str()

namespace i8desk
{
class CI8DeskSvr : public CWin32Svr
{
private:
	HANDLE		m_hThread;
	HANDLE		m_hExited;
	ILogger*	m_pLogger;
	CPlugMgr*	m_plugMgr;
 	CAutoUpate* m_pUpdate;
	CBackup*	m_pBackup;
	HMODULE		m_hRemoteCtrl;
	HANDLE		m_hOneInstance;

public:
	CI8DeskSvr():CWin32Svr(SERVICE_NAME, SERVICE_DISCRIPTION, SERVICE_PATH), m_hThread(NULL), m_hExited(NULL), 
		m_pLogger(NULL), m_plugMgr(NULL), m_pUpdate(NULL), m_pBackup(NULL),m_hRemoteCtrl(NULL), 
		m_hOneInstance(NULL)
	{
		CoInitialize(NULL);
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		CreateLogger(&m_pLogger);		
		AddWindowFireWall();
	}
	virtual ~CI8DeskSvr()
	{
		Release_Interface(m_pLogger);
		if (m_hExited != NULL)
		{
			CloseHandle(m_hExited);
			m_hExited = NULL;
		}
		CoUninitialize();
	}
public:
	virtual DWORD OnStart()
	{
		if (m_pLogger == NULL || m_hExited == NULL)
		{
			Stop();
			return ERROR_SUCCESS;
		}
		m_pLogger->SetLogFileName(SERVICE_NAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(LM_INFO, TEXT("============================="));
		m_pLogger->SetAddDateBefore(true);

		if (OneInstance())
		{
			Stop();
			return ERROR_SUCCESS;
		}


		if ((m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL)) == NULL)
		{
			Stop();
			return ERROR_SUCCESS;
		}

		return ERROR_SUCCESS;
	}

	virtual DWORD OnStop()
	{

		ReportStatusToSCM(SERVICE_STOP_PENDING);
		if (m_hThread != NULL && m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		if (m_pLogger)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("%s: stoped."), GetSvrName());
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(LM_INFO, TEXT("=============================\r\n\r\n"));
		}
		KillService(TEXT("DNAService.exe"));
		return ERROR_SUCCESS;
	}
	virtual DWORD OnShutdown() { return OnStop();}
private:
	static UINT32 __stdcall WorkThread(LPVOID lpVoid);

	static BOOL WINAPI EnumWindowsProc(HWND hwnd, LPARAM lParam);

public:
	void AddWindowFireWall();
	DWORD OneInstance();
	void RegDNAService();
	void KillService(LPCTSTR lpszSerive);
	void HideDosWindow() { EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(m_pLogger));}
	bool StartRemoteControl();
	void FreeRemoteCtrlPlug();

};


UINT32 __stdcall CI8DeskSvr::WorkThread(LPVOID lpVoid)
{
	CI8DeskSvr* pThis = reinterpret_cast<CI8DeskSvr*>(lpVoid);


	//check service config.
	if (pThis->IsService())
	{
		pThis->ChangeConfig();
	}
	else
	{
		CWin32Svr::StopService(TEXT("i8desksvr"));
		CWin32Svr::RemoveService(TEXT("I8DeskSvr"));
		CWin32Svr::RemoveService(TEXT("DNAS"));
	}

	//registry DNAS.
	pThis->RegDNAService();

	//load all plug.
	pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Create Core Interface."));
	pThis->m_plugMgr = new CPlugMgr(pThis->m_pLogger);
 	pThis->m_pUpdate = new CAutoUpate(pThis->m_plugMgr, pThis->m_pLogger);
	pThis->m_pBackup = new CBackup(pThis->m_pLogger);

	if (!pThis->m_plugMgr->Start() || !pThis->m_pUpdate->StartUpdate() 	|| !pThis->m_pBackup->StartBackup() )
	{
		//.....
	}

	pThis->StartRemoteControl();

	while (WAIT_OBJECT_0 != WaitForSingleObject(pThis->m_hExited, 5000))
	{
		pThis->HideDosWindow();
	}

	pThis->FreeRemoteCtrlPlug();

	//clean all plug.
	pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Free Core Interface."));
	Delete_Pointer(pThis->m_pBackup);
	Delete_Pointer(pThis->m_pUpdate);
	Release_Interface(pThis->m_plugMgr);
	return 0;
}

bool CI8DeskSvr::StartRemoteControl()
{
	stdex::tString strFilePath = utility::GetAppPath() + _T("WinVNC\\");
	SetDllDirectory(strFilePath.c_str());

	strFilePath += TEXT("WinVNC.dll");
	m_hRemoteCtrl = LoadLibrary(strFilePath.c_str());
	if (m_hRemoteCtrl == NULL)
		return false;

	typedef BOOL (WINAPI* PFNSTARTVNC)();
	PFNSTARTVNC pfnStartVNC = GetProcAddress(m_hRemoteCtrl, "_StartVNC@0");
	if (pfnStartVNC == NULL)
		return false;

	try
	{
		pfnStartVNC(); 
	}
	catch(...)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("加载远程控制客户端失败。\r\n"));
		BOOL (WINAPI* pfnStopVNC)();
		pfnStopVNC = GetProcAddress(m_hRemoteCtrl, "_StopVNC@0");
		if (pfnStopVNC == NULL)
			return false;

		pfnStopVNC(); 
	}

	m_pLogger->WriteLog(LM_INFO, TEXT("加载远程控制客户端成功。\r\n"));
	return true;
}

void CI8DeskSvr::FreeRemoteCtrlPlug()
{
	if (m_hRemoteCtrl != NULL)
	{
		BOOL (WINAPI* pfnStopVNC)();
		pfnStopVNC = GetProcAddress(m_hRemoteCtrl, "StopVNC");
		if (pfnStopVNC != NULL)
			pfnStopVNC(); 
		FreeLibrary(m_hRemoteCtrl);
	}
	m_hRemoteCtrl = NULL;

	m_pLogger->WriteLog(LM_INFO, TEXT("释放远程控制客户端成功。"));
}

void CI8DeskSvr::AddWindowFireWall()
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

		GetModuleFileNameW(NULL, path, MAX_PATH);
		PathRemoveFileSpecW(path);
		lstrcatW(path, L"\\SafeSvr\\Server\\UndiskSafe.exe");
		Fw.AddApplication(path, L"I8DeskDisklessSafeSvr");
	}
}

DWORD CI8DeskSvr::OneInstance()
{
	utility::CEvenyOneSD sd;
	m_hOneInstance = CreateEvent(sd.GetSA(), TRUE, FALSE, TEXT("__i8desksvr_one_instance__"));
	if (m_hOneInstance == NULL)
		return GetLastError();

	if (ERROR_ALREADY_EXISTS == GetLastError())
		return -1;
	return 0;
}


void CI8DeskSvr::RegDNAService()
{
	m_pLogger->WriteLog(LM_INFO, TEXT("Start Check DNAService."));
	TCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path, MAX_PATH);
	PathRemoveFileSpec(path);
	PathAddBackslash(path);
	lstrcat(path, TEXT("DNAService\\DNAService.exe"));
	stdex::tString szDNAService(path);
	szDNAService.insert(0, TEXT("\""));
	szDNAService += TEXT("\"");

	std::pair<stdex::tString, bool> cmd[] =
	{
		std::make_pair(TEXT("-UnRegServer"),	true),
		std::make_pair(TEXT("-RegServer"),		true),
		std::make_pair(TEXT("-Service"),		true),
	};
	if (!IsService())
	{
		cmd[2].second = false;
	}
	else
	{
		utility::CAutoService schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager.IsValid())
		{
			utility::CAutoService schService = OpenService(schSCManager, TEXT("DNAS"), SERVICE_ALL_ACCESS);
			if (schService.IsValid())
			{
				char buf[4096] = {0};
				LPQUERY_SERVICE_CONFIG lpConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIG>(buf);
				DWORD dwSize = sizeof(buf);
				DWORD dwNeedSize = 0;
				QueryServiceConfig(schService, lpConfig, dwSize, &dwNeedSize);
				if (lstrcmpi(lpConfig->lpBinaryPathName, szDNAService.c_str()) == 0)
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("Registry DNAService Success."));
					return ;
				}
			}
		}
	}
	for (int idx=0; idx<_countof(cmd);idx++)
	{
		if (!cmd[idx].second)
			continue;

		TCHAR line[1024] = {0};
		_stprintf(line, TEXT("%s %s"), szDNAService.c_str(), cmd[idx].first.c_str());

		STARTUPINFO si = {sizeof(si)};
		PROCESS_INFORMATION pi = {0};

		if (!CreateProcess(NULL, line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("[%s]:fail:%d:%s"), line, dwError, Error.ErrorMessage());
			m_pLogger->WriteLog(LM_INFO, TEXT("Registry DNAService Fail."));
			return ;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	m_pLogger->WriteLog(LM_INFO, TEXT("Registry DNAService Success."));
}

void CI8DeskSvr::KillService(LPCTSTR lpszSerive)
{
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

BOOL WINAPI CI8DeskSvr::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
#ifdef _DEBUG
	return TRUE;
#else
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
			return TRUE;
		}
	}
	return TRUE;
#endif
}

};


int _tmain(int argc, _TCHAR* argv[])
{
	i8desk::CI8DeskSvr g_Service;

	if (argc == 2 && lstrcmpi(argv[1], TEXT("-AutoUpdate")) == 0)
	{
		try
		{
			g_Service.KillService(TEXT("DNAService.exe"));
			i8desk::CWin32Svr::StopService(TEXT("I8VDiskSvr"));
			i8desk::CWin32Svr::StopService(TEXT("I8DeskSvr"));
			g_Service.KillService(TEXT("I8DeskSvr.exe"));
			g_Service.KillService(TEXT("I8VDiskSvr.exe"));
		}
		catch (...){ }
		Sleep(1000);

		bool bIsService = false;
		utility::CAutoService hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (hSCM.IsValid())
		{
			utility::CAutoService hService = OpenService(hSCM, TEXT("I8DeskSvr"), SERVICE_START | SERVICE_QUERY_STATUS);
			if (hService.IsValid())
				bIsService = true;
		}
		if (bIsService)
		{
			i8desk::CWin32Svr::StartService(TEXT("I8DeskSvr"));
			i8desk::CWin32Svr::StartService(TEXT("I8VDiskSvr"));
		}
		else
		{
			TCHAR parameter[2][MAX_PATH] = 
			{
				{TEXT("\"%sI8DeskSvr.exe\" -debug")},
				{TEXT("\"%sI8VDiskSvr.exe\" -debug")}
			};
			TCHAR path[MAX_PATH] = {0};
			for (size_t idx=0; idx<_countof(parameter); idx++)
			{
				STARTUPINFO			si = {0};
				si.cb = sizeof(si);
				PROCESS_INFORMATION pi = {0};

				_stprintf(path, parameter[idx], utility::GetAppPath().c_str());
				if (CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject(pi.hProcess, INFINITE);
					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);
				}
			}
		}

		return 0;
	}

	g_Service.ProcessCmdLine(argc, argv);
}