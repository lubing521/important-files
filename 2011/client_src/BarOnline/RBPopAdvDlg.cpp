// RBPopAdvDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "RBPopAdvDlg.h"
#include "MainFrame.h"

IMPLEMENT_DYNAMIC(CRBPopAdvDlg, CDialog)

CRBPopAdvDlg::CRBPopAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRBPopAdvDlg::IDD, pParent)
{

}

CRBPopAdvDlg::~CRBPopAdvDlg()
{
}

void CRBPopAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, m_btnClose);
}


BEGIN_MESSAGE_MAP(CRBPopAdvDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLOSE, &CRBPopAdvDlg::OnBnClickedClose)
END_MESSAGE_MAP()

BOOL CRBPopAdvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar09_ct.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID), szMacAddr);
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(5, 111, 500+5, 250+111), this, AFX_IDW_PANE_FIRST);
	m_WebPage.SetSilent(TRUE);
	m_WebPage.Navigate2(strUrl);
							  
	MONITORINFO info = {sizeof(info)};
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &info);

	CRect rcWorkArea = info.rcWork;
	CRect rc(rcWorkArea.right-270, rcWorkArea.bottom, rcWorkArea.right, rcWorkArea.bottom + 190);

	MoveWindow(&rc);

	SetTimer(1, 1, NULL);
	m_btnClose.SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
	return TRUE;
}

void CRBPopAdvDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_pDlgRbPopAdv = NULL;
}

void CRBPopAdvDlg::OnOK()
{
	OnBnClickedClose();
}

void CRBPopAdvDlg::OnCancel()
{
	OnBnClickedClose();
}

BOOL CRBPopAdvDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Wnd/rbad_bg.png"), rc);
	return TRUE;
}

void CRBPopAdvDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_WebPage.GetSafeHwnd() == NULL)
		return;

	CRect rc(0, 20, 270, 190);
	rc.DeflateRect(1, 0, 1, 1);

	m_WebPage.MoveWindow(&rc);

	rc = CRect(230, 0, 270, 20);
	m_btnClose.MoveWindow(&rc);

}

LRESULT CRBPopAdvDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	ScreenToClient(&point);
	if (nHitTest == HTCLIENT && point.y - 20)
		return HTCAPTION;
	return nHitTest;
}

void CRBPopAdvDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	static DWORD dwTick = GetTickCount();
	if (GetTickCount() - dwTick <5000)
		return ;

	CRect rc;
	GetWindowRect(&rc);
	rc.OffsetRect(0, -1);
	MoveWindow(&rc);

	MONITORINFO info = {sizeof(info)};
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &info);
	CRect rcWorkArea = info.rcWork;
	if (rc.top <= rcWorkArea.bottom - 190)
	{
		KillTimer(1);
	}
}

void CRBPopAdvDlg::OnBnClickedClose()
{
	DestroyWindow();
}
