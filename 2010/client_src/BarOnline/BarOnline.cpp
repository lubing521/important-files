#include "stdafx.h"
#include "BarOnline.h"
#include "MainFrame.h"
//#include "hook.h"
#include "winsock2.h"


#define CRASH_DUMP_FILE TEXT("BarOnline.dmp")
#include "except.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBarOnlineApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CBarOnlineApp::CBarOnlineApp()
{
	SetUnhandledExceptionFilter(::Local_UnhandledExceptionFilter);
	//hook::InitHookApi();
}

CBarOnlineApp theApp;

BOOL CBarOnlineApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	AfxOleInit();
	
	WSADATA wsaData = {0};
	WSAStartup(0x0202, &wsaData);

	m_hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(tagFileMap), FILE_MAP_NAME);
	if (m_hFileMap == NULL)
	{
		AfxWriteLog(TEXT("Create FileMapp Fail.Program will exit."));
		return FALSE;
	}
	DWORD dwError = GetLastError();

	tagFileMap* pFileMap = (tagFileMap*)MapViewOfFile(m_hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 
		0, 0, sizeof(tagFileMap));
	if (pFileMap == NULL)
	{
		AfxWriteLog(TEXT("Map file view error.Program will exit."));
		return FALSE;
	}

	//把命令行参数写入共享内存
	lstrcpy(pFileMap->szCmdLine, m_lpCmdLine);

	//如果己经运行了菜单
	if (dwError == ERROR_ALREADY_EXISTS)
	{
		//如果带有参数，则向己运行的菜单发送消息，否则退出。
		HWND hWnd = pFileMap->hWnd;
		if (IsWindow(hWnd))
		{
			//if (!CString(m_lpCmdLine).IsEmpty())
			{
			//	::ShowWindow(hWnd, SW_RESTORE);
			//	::SetForegroundWindow(hWnd);
			//	::SetFocus(hWnd);
				::SendMessage(hWnd, WM_EXEC_START_PARAM, 1, 0);
			}
		}
		return FALSE;
	}

	CWndShadow::Initialize(AfxGetInstanceHandle());
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//加载皮肤
	CString strSkinInfo = AfxGetDbMgr()->GetStrOpt(OPT_M_CLISKIN);
	CZipSkin::getInstance()->LoadSkinInfo(strSkinInfo);

	CMainFrame* pFrame = new CMainFrame(TEXT("游戏菜单"));
	m_pMainWnd = pFrame;

	//设置第一个实例运行时的主窗口句柄。
	pFileMap->hWnd = pFrame->m_hWnd;

	if (pFrame->m_bShowDock)
	{
		pFrame->ShowDockWnd();
	}

	if (AfxGetDbMgr()->GetIsShowGongGao() && !pFrame->m_bShowDock)
		pFrame->ShowGongGao();
	//pFrame->ShowRBPopAdv();

	UnmapViewOfFile(pFileMap);
	return TRUE;
}

CString CBarOnlineApp::GetCmdLine()
{
	CString szCmdLine;
	if (m_hFileMap != NULL)
	{
		tagFileMap* pFileMap = (tagFileMap*)MapViewOfFile(m_hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 
			0, 0, sizeof(tagFileMap));
		if (pFileMap != NULL)
		{
			szCmdLine = pFileMap->szCmdLine;
			UnmapViewOfFile(pFileMap);
		}
	}

	return szCmdLine;
}

int CBarOnlineApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);

	//hook::UnHookApi();
	if (m_hFileMap != NULL)
		CloseHandle(m_hFileMap);
	WSACleanup();
	return CWinAppEx::ExitInstance();
}