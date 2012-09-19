#include "stdafx.h"
#include "DeskLocker.h"
#include "MainFrame.h"
#include "hook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMainFrame::CMainFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CMainFrame::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nSreed = 100;
}

void CMainFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD, m_edtPwd);
	DDX_Control(pDX, IDC_UNLOCKER, m_btnUnLocker);
	DDX_Control(pDX, IDC_PLUS,	   m_btnPlus);
	DDX_Control(pDX, IDC_DECREASE, m_btnDecrease);
}

BEGIN_MESSAGE_MAP(CMainFrame, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CMainFrame::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMainFrame::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_PLUS, &CMainFrame::OnBnClickedPlus)
	ON_BN_CLICKED(IDC_DECREASE, &CMainFrame::OnBnClickedDecrease)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_UNLOCKER, &CMainFrame::OnBnClickedUnlocker)
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_AUTO_UNLOCK, &CMainFrame::OnAtuoUnlock)
END_MESSAGE_MAP()

void CMainFrame::SetWndParent(BYTE value)
{
	SetLayeredWindowAttributes(0, BYTE((255 * value) / 100), LWA_ALPHA);
}

BOOL CMainFrame::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetTimer(1, 100, NULL);

	InstallHook();
	InjectToWinlogon();

	SetWindowText(TEXT("{7ED87DDD-AE3D-4517-B6E8-69621C3BDA8F}"));
	int nScrWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScrHeight = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(&wndTopMost, 0, 0, nScrWidth, nScrHeight, SWP_FRAMECHANGED);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetWndParent(m_nSreed);

	CRect rc;
	rc.left = (nScrWidth - 655)/2 + 88;
	rc.top  = (nScrHeight - 100) + 15;
	rc.right = rc.left + 152;
	rc.bottom = rc.top + 15;
	m_edtPwd.MoveWindow(&rc);

	rc.left = (nScrWidth - 655)/2 + 245;
	rc.top  = (nScrHeight - 100) + 7;
	rc.right = rc.left + 63;
	rc.bottom = rc.top + 27;
	m_btnUnLocker.MoveWindow(&rc);
	m_btnUnLocker.SetImage(IDB_UNLOCK, IDB_UNLOCK1, IDB_UNLOCK2);

	rc.left = (nScrWidth - 655)/2 + 410;
	rc.top  = (nScrHeight - 100) + 12;
	rc.right = rc.left + 18;
	rc.bottom = rc.top + 18;
	m_btnPlus.MoveWindow(&rc);
	m_btnPlus.SetImage(IDB_PLUS, IDB_PLUS1, IDB_PLUS2);
	
	rc.OffsetRect(23, 0);
	m_btnDecrease.MoveWindow(&rc);
	m_btnDecrease.SetImage(IDB_DECREASE, IDB_DECREASE1, IDB_DECREASE2);

	m_edtPwd.SetFocus();

	return FALSE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtPwd.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedUnlocker();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMainFrame::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CMainFrame::PaintTime(CDC* pDC)
{
	static DWORD dwStartTime = GetTickCount();
	int nMode = pDC->SetBkMode(1);
	CString szMsg;
	int nSecond = (GetTickCount() - dwStartTime) / 1000;
	szMsg.Format(TEXT("%02d:%02d:%02d"), nSecond / 3600, (nSecond % 3600) / 60, nSecond % 60);

	CRect rc;
	rc.left = (GetSystemMetrics(SM_CXSCREEN) - 585)/2 + 505;
	rc.top  = (GetSystemMetrics(SM_CYSCREEN) - 100) + 12;
	rc.right = rc.left + 80;
	rc.bottom = rc.top + 18;
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->FillRect(&rc, &CBrush(RGB(220, 220, 220)));
	pDC->DrawText(szMsg, &rc, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	pDC->SetBkMode(nMode);
}

HCURSOR CMainFrame::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainFrame::OnDestroy()
{
	StopHook();
	UnInjectToWinlogon();
	CDialog::OnDestroy();
}

void CMainFrame::OnBnClickedOk()
{
	//OnBnClickedUnlocker();
}

void CMainFrame::OnBnClickedCancel()
{
	//OnCancel();
}

HBRUSH CMainFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(255, 255, 255));
	if (nCtlColor == CTLCOLOR_DLG)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(1);
		return brush;
	}
	return hbr;
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	//return CDialog::OnEraseBkgnd(pDC);

	int nScrWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScrHeight = GetSystemMetrics(SM_CYSCREEN);
	CRect rc, rx;
	GetClientRect(&rc);

	TCHAR szOemBmp[MAX_PATH] = {0};
	GetModuleFileName(NULL, szOemBmp, MAX_PATH - 1);
	PathRemoveFileSpec(szOemBmp);
	PathAddBackslash(szOemBmp);
	
	if (nScrWidth * 3 == nScrHeight * 4)
		lstrcat(szOemBmp, TEXT("Skin\\oemlock.png"));
	else
		lstrcat(szOemBmp, TEXT("Skin\\oemlockw.png"));
	if (!PathFileExists(szOemBmp))
	{
		pDC->FillRect(&rc, &CBrush(RGB(0, 0, 0)));

		rx = CRect(5, 5, 0, 0);
		rx.right = rx.left + 159;
		rx.bottom = rx.top + 51;
 		AfxDrawImage(pDC, IDB_LOGO, rx);
	}
	else
	{
		AfxDrawImage(pDC, szOemBmp, rc);
	}

	rx.left = (rc.Width() - 139) / 2;
	rx.top = rc.Height() / 2 - 139;
	rx.right  = rx.left + 139;
	rx.bottom = rx.top + 139;
	AfxDrawImage(pDC, IDB_LOCK, rx);

	rx.left = (rc.Width() - 425) / 2;
	rx.top = rc.Height() / 2 + 10;
	rx.right  = rx.left + 425;
	rx.bottom = rx.top + 53;
	AfxDrawImage(pDC, IDB_TEXT, rx);

	rx.top = rc.Height() - 100;
	rx.left = (rc.Width() - 655)/2;
	rx.right  = rx.left + 655;
	rx.bottom = rx.top + 41;
	AfxDrawImage(pDC, IDB_TOOLBAR, rx);

	PaintTime(pDC);

	return TRUE;
}

void CMainFrame::OnBnClickedPlus()
{
	if (m_nSreed > 50)
	{
		m_nSreed -= 5;
		if (m_nSreed < 50)
			m_nSreed = 50;
		SetWndParent(m_nSreed);
	}
}

void CMainFrame::OnBnClickedDecrease()
{
	if (m_nSreed < 100)
	{
		m_nSreed += 5;
		if (m_nSreed > 100)
			m_nSreed = 100;
		SetWndParent(m_nSreed);
	}
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	CClientDC dc(this);
	PaintTime(&dc);
}

void CMainFrame::OnBnClickedUnlocker()
{
	CString msg;
	m_edtPwd.GetWindowText(msg);
	if (msg.CompareNoCase(m_strPwd) == 0)
		OnOK();
}

void CMainFrame::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
}


LRESULT CMainFrame::OnAtuoUnlock(WPARAM wParam, LPARAM lParam)
{
	OnOK();
	return 0;
}