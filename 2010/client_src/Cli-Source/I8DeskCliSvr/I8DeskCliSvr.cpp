// I8DeskCliSvr.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "win32svr.h"
#include "plugmgr.h"
#include "../include/except.h"
#include "WinXPSP2FireWall.h"
#include "../include/hy.h"

#include <psapi.h>
#include <shellapi.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "version.lib")

#ifdef _DEBUG
#include "vld.h"
#endif

class I8DeskCliSvr : public i8desk::CWin32Svr
{
private:
	i8desk::ILogger*  m_pLogger;
	i8desk::CPlugMgr* m_pPlugMgr;
public:
	I8DeskCliSvr():CWin32Svr(TEXT("I8DeskCliSvr"), TEXT("I8Desk ClientServer Manage")) 
	{
		SetUnhandledExceptionFilter(::Local_UnhandledExceptionFilter);
		m_pLogger = NULL;
		m_pPlugMgr = NULL;
		i8desk::CreateLogger(&m_pLogger);
	}
	virtual ~I8DeskCliSvr() {}
public:
	void AddSelfToFireWall()
	{
		WinXPSP2FireWall fw;
		BOOL bOn = FALSE;
		fw.IsWindowsFirewallOn(bOn);
		if (bOn)
		{
			wchar_t path[MAX_PATH] = {0};
			GetModuleFileNameW(NULL, path, MAX_PATH);
			fw.AddApplication(path, L"I8DeskCliSvr.exe");

			PathRemoveFileSpecW(path);
			PathAddBackslashW(path);
			lstrcatW(path, L"BarOnline.exe");
			fw.AddApplication(path, L"BarOnline.exe");
		}
	}
	virtual DWORD OnStart()
	{
		AddSelfToFireWall();

		if (m_pLogger == NULL)
		{
			OnStop();
			return ERROR_SUCCESS;
		}
		m_pLogger->SetLogFileName(TEXT("I8DeskCliSvr"));
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("============================================================"));
		m_pLogger->SetAddDateBefore(true);

		m_pPlugMgr = new i8desk::CPlugMgr(m_pLogger);
		bool bSucess = m_pPlugMgr->Start();
		m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("%s:%s"), GetSvrName(), bSucess ? TEXT("Success Started.") : TEXT("Start Fail."));
		if (!bSucess)
		{
			OnStop();
			return ERROR_SUCCESS;
		}
		ReportStatusToSCM(SERVICE_RUNNING);
		return ERROR_SUCCESS;
	}
	virtual DWORD OnStop()
	{
		ReportStatusToSCM(SERVICE_STOP_PENDING);
		if (m_pPlugMgr != NULL)
		{
			m_pPlugMgr->Stop();
			i8desk::Release_Interface(m_pPlugMgr);
		}

		if (m_pLogger)
		{
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("%s:stoped."), GetSvrName());
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(i8desk::LM_INFO, TEXT("==============================================================\r\n\r\n"));
			i8desk::Release_Interface(m_pLogger);
		}

		ReportStatusToSCM(SERVICE_STOPPED);
		return ERROR_SUCCESS;
	}
	virtual DWORD OnShutdown()
	{
		OnStop();
		return ERROR_SUCCESS;
	}
};

I8DeskCliSvr g_service;

void StartProcess(LPCTSTR lpszProcName)
{
	DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
	char path[MAX_PATH] = {0};
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return	;
	cProcesses = cbNeeded / sizeof(DWORD);
	for (DWORD i = 0; i < cProcesses; i++ )
	{
		HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
		if (hProcess != NULL)
		{
			GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
			if (lstrcmpi(path, lpszProcName) == 0)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);

				GetModuleFileName(NULL, path, MAX_PATH);
				PathRemoveFileSpecA(path);
				PathAddBackslash(path);
				lstrcat(path, lpszProcName);
				ShellExecute(NULL, TEXT("open"), path, NULL, NULL, SW_SHOWNORMAL);
				return ;
			}
			CloseHandle(hProcess);
		}		
	}
}

std::string GetFileVersion(const char* szFileName)
{
	DWORD dwSize = GetFileVersionInfoSize(szFileName, 0);
	if (dwSize)
	{
		BYTE *pInfo = new BYTE[dwSize+1];
		GetFileVersionInfo(szFileName, 0, dwSize, pInfo);

		UINT nQuerySize;
		DWORD* pTransTable = NULL;
		VerQueryValue(pInfo, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
		LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		char SubBlock[MAX_PATH] = {0};
		_stprintf(SubBlock, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
		LPSTR lpData;
		VerQueryValue(pInfo, SubBlock, (PVOID*)&lpData, &nQuerySize);
		std::string str(lpData);
		delete []pInfo;
		//replace ',' to '.'
		size_t pos = 0;
		while ( (pos = str.find(',', pos)) != std::string::npos)
		{
			str[pos] = '.';
		}
		//delete [SP]
		pos = 0;
		while ( (pos = str.find(' ', pos)) != std::string::npos)
		{
			str.erase(pos, 1);
		}
		return str;
	}
	return std::string("0.0.0.0");

}

int _tmain(int argc, _TCHAR* argv[])
{
	{
		using namespace i8desk;
		CHyOperator hy;
		ILogger* pLogger = NULL;
		CreateLogger(&pLogger);
		if (pLogger != NULL)
		{
			hy.SetLogger(pLogger);
			char path[MAX_PATH] = {0};
			GetModuleFileName(NULL, path, MAX_PATH);
			strupr(path);
			path[1] = 0;
			if (hy.IsInitialize() && hy.MountDosChar(path))
			{
				path[1] = ':';
				PathRemoveFileSpec(path);
				PathAddBackslash(path);				
				lstrcat(path, "wxProt.dll");
				std::string pre = std::string("\\\\.\\I8Desk") + path[0];
				std::string dst(path);
				dst.replace(0, 2, pre);

				if (GetFileVersion(path) != GetFileVersion(dst.c_str()))
				{
					CopyFile(path, dst.c_str(), FALSE);
				}
			}
			Release_Interface(pLogger);
		}
	}

	for (int idx=1; idx<argc; idx++)
	{
		if (strcmpi(argv[idx], TEXT("-AutoUpdate")) == 0)
		{
			Sleep(2000); //作个简单的延时，让他的服务端内部的后续处理完成。
			i8desk::CWin32Svr::StopService(TEXT("I8DeskCliSvr"));
			i8desk::CWin32Svr::StartService(TEXT("I8DeskCliSvr"));

			Sleep(3000);
			StartProcess(TEXT("BarOnline.exe"));

			return 0;
		}
	}
	g_service.ProcessCmdLine(argc, argv);
	return 0;
}

