// Console.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"

#pragma comment(lib, "gdiplus.lib")

#define CRASH_DUMP_FILE "Console.dmp"

#include "Console.h"
#include "ConsoleDlg.h"
#include "except.h"

#include "oem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CConsoleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CConsoleApp::CConsoleApp()
{
	SetUnhandledExceptionFilter(::Local_UnhandledExceptionFilter);
}

CConsoleApp theApp;

BOOL CConsoleApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	AfxInitRichEdit2();
	AfxOleInit();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, 0);

	WSAData wsaData;
	WSAStartup(0x0202, &wsaData);

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	//设置控制台主窗口标题
	free((void*)m_pszAppName);

	CString strAppName;
	if (I8DESK_OEM->LoadString(strAppName))
		m_pszAppName=_tcsdup((LPCTSTR)strAppName);
	else 
		m_pszAppName=_tcsdup(_T("i8desk控制台"));

	//保证程序只能运行一个实例.
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "__i8desk_console_instance__");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox("本机已有一个控制台连接上服务端,点击确认后退出本控制台.");
		CloseHandle(hEvent);

		HWND hWnd = ::FindWindow("#32770", m_pszAppName);
		if (hWnd != NULL)
		{
			::ShowWindow(hWnd, SW_RESTORE);
			::SetForegroundWindow(hWnd);
		}
		return TRUE;
	}
	
	CConsoleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}	
	CloseHandle(hEvent);

	return FALSE;
}

BOOL CConsoleApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return CWinApp::ExitInstance();
}


void CConsoleApp::WinHelp(DWORD dwData, UINT nCmd)
{
	// TODO: 在此添加专用代码和/或调用基类

	//CWinApp::WinHelp(dwData, nCmd);
}
