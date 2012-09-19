#include "stdafx.h"
#include "BarOnline.h"
#include "MainFrame.h"
#include "winsock2.h"
#include "I8SkinCommonFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBarOnlineApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CBarOnlineApp::CBarOnlineApp()
{
}

CBarOnlineApp theApp;

BOOL CBarOnlineApp::InitInstance()
{
    //MessageBox(0, m_lpCmdLine, _T("启动菜单"), 0);
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	AfxOleInit();

    I8CD_Init();
    if (I8CD_GetVersion() != C_CLIENT_DATA_CORE_VERSION)
    {
        AfxMessageBox(_T("客户端数据层版本不匹配。"));
        return FALSE;
    }
    if (!I8CD_CheckRepeatedProcess(m_lpCmdLine))
    {
        return FALSE;
    }
    //加载皮肤
    WCHAR szPath[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, szPath, MAX_PATH);
    PathRemoveFileSpecW(szPath);
    PathAddBackslashW(szPath);
    I8SkinCtrl_ns::I8_InitSkinCtrl(CString(szPath) + _T("Skin\\Skin.zip"));
    CString sSkinFile = I8CD_GetSkinFile();

	CMainFrame* pFrame = new CMainFrame(TEXT("游戏菜单"));
	m_pMainWnd = pFrame;

	if (pFrame->m_bShowDock)//XXXX
	{
		pFrame->ShowDockWnd();
	}

    if (I8CD_IsShowPost() && !pFrame->m_bShowDock)
    {
		pFrame->ShowGongGao();
    }
	//pFrame->ShowRBPopAdv();

	return TRUE;
}

int CBarOnlineApp::ExitInstance()
{
    I8SkinCtrl_ns::I8_ReleaseSkinCtrl();
    I8CD_Release();
	return CWinAppEx::ExitInstance();
}