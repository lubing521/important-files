
// UDiskServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "UDiskServer.h"
#include "UDiskServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUDiskServerApp

BEGIN_MESSAGE_MAP(CUDiskServerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUDiskServerApp 构造

CUDiskServerApp::CUDiskServerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CUDiskServerApp 对象

CUDiskServerApp theApp;


// CUDiskServerApp 初始化
void CUDiskServerApp::SetMainHWnd(HWND hWnd)
{
	char buf[1024] = {0};
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE, 0, 1024, _T("I8Desk_Udisk_Server"));	
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE) 
		return ;

	LPSTR pBuf = (LPSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 
		0, 0, 1024);
	if (pBuf == NULL)
		return ;
	*(HWND*)pBuf = hWnd;
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);	
}
BOOL CUDiskServerApp::OneInstance()
{
	char buf[1024] = {0};
	m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE, 0, 1024, _T("I8Desk_Udisk_Server"));	
	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE) 
	{
		printf(buf, "Could not create file mapping object (%d).\n", GetLastError());
		AfxMessageBox(buf);
		return FALSE;
	}
	DWORD dwErr = GetLastError();
	if (ERROR_ALREADY_EXISTS == dwErr)
	{
		LPSTR pBuf = (LPSTR) MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 
			0, 0, 1024);
		if (pBuf == NULL)
			return FALSE;
		HWND hWnd = *(HWND*)(pBuf);
		if (hWnd != NULL)		
		{
			try		{
				::ShowWindow(hWnd, SW_RESTORE);
				::SetForegroundWindow(hWnd);
			}	
			catch (...) {	}

		}
		UnmapViewOfFile(pBuf);
		CloseHandle(m_hMapFile);
		m_hMapFile = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	return TRUE;
}
BOOL CUDiskServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	m_pConfigInfo = new SrvConfigInfo;
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	if(!OneInstance())
		return FALSE;

	WSADATA data;
	WSAStartup(0x0202,&data);

	CUDiskServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CUDiskServerApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
		if(m_pConfigInfo)
			delete m_pConfigInfo;
		WSACleanup();
	return CWinAppEx::ExitInstance();
}

