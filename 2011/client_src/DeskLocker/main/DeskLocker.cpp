#include "stdafx.h"
#include "DeskLocker.h"
#include "MainFrame.h"
#include "DlgLgn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDeskLockerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CDeskLockerApp::CDeskLockerApp()
{
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

CDeskLockerApp theApp;

BOOL CDeskLockerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	bool bAutoLock = false;
	CString szCmdLine(m_lpCmdLine != NULL ? m_lpCmdLine : TEXT(""));
	szCmdLine.Trim();
	CString szLock, szPassword;
	AfxExtractSubString(szLock, szCmdLine, 0, TEXT('|'));
	AfxExtractSubString(szPassword, szCmdLine, 1, TEXT('|'));

	//lock|password. for auto lock for not to show input password dialog.
	if (szLock.CompareNoCase(TEXT("lock")) == 0 && !szPassword.IsEmpty())
	{
		bAutoLock = true;
	}

	if (!bAutoLock)
	{
		CDlgLgn login;
		if (login.DoModal() != IDOK)
			return FALSE;
		szPassword = login.m_strPwd;
	}

	CMainFrame dlg;
	dlg.m_strPwd = szPassword;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}

int CDeskLockerApp::ExitInstance()
{
	GdiplusShutdown(gdiplusToken);

	return CWinAppEx::ExitInstance();
}