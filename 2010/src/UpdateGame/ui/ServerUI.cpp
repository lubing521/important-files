#include "stdafx.h"
#include "afxwinappex.h"
#include "ServerUI.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CServerUIApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CServerUIApp::OnAppAbout)
END_MESSAGE_MAP()

CServerUIApp::CServerUIApp()
{
}

CServerUIApp theApp;

BOOL CServerUIApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	if(!OneInstance())
		return FALSE;
	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	WSADATA wd={0};
	WSAStartup(0x0202,&wd);
	SetRegistryKey(_T("i8NodeServer"));
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	SetMainHWnd(m_pMainWnd->m_hWnd);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

BOOL CServerUIApp::OneInstance()
{
	char buf[1024] = {0};
	HANDLE 	m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE, 0, 1024, _T("I8Desk_NodeServer"));	
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
void CServerUIApp::SetMainHWnd(HWND hWnd)
{
	char buf[1024] = {0};
	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
		PAGE_READWRITE, 0, 1024, _T("I8Desk_NodeServer"));	
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

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CServerUIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

int CServerUIApp::ExitInstance()
{
	WSACleanup();
	return CWinApp::ExitInstance();
}
