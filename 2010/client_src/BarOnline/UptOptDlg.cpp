// UptOptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "UptOptDlg.h"


// CUptOptDlg 对话框

IMPLEMENT_DYNAMIC(CUptOptDlg, CDialog)

CUptOptDlg::CUptOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUptOptDlg::IDD, pParent)
{
	m_bIsQuick = TRUE;
}

CUptOptDlg::~CUptOptDlg()
{
}

void CUptOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RD_QUICK,  m_btnQuick);
	DDX_Control(pDX, IDC_RD_FORCE,  m_btnForce);
	DDX_Control(pDX, IDC_CLOSE,		m_btnClose);
	DDX_Control(pDX, IDOK,			m_btnOK);
	DDX_Control(pDX, IDCANCEL,		m_btnCancel);
}

BEGIN_MESSAGE_MAP(CUptOptDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CUptOptDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLOSE, &CUptOptDlg::OnBnClickedClose)
END_MESSAGE_MAP()

BOOL CUptOptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcClient(0, 0, 356, 218);
	CRgn rgn;
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);

	CRect rc = rcClient;
	rc.left = rc.right - 51;
	rc.right = rc.right - 10;
	rc.bottom = rc.top + 24;
	m_btnClose.MoveWindow(&rc);
	m_btnClose.SetImage(TEXT("Skin/Wnd/GongGao/x.png"), TEXT("Skin/Wnd/GongGao/x_a.png"));

	rc = CRect(60, 52, 260, 76);
	m_btnQuick.MoveWindow(&rc);
	m_btnQuick.SetCheck(BST_CHECKED);

	rc.OffsetRect(0, 40);
	m_btnForce.MoveWindow(&rc);

	rc.OffsetRect(-10, 60);
	rc.right = rc.left + 97; rc.bottom = rc.top + 25;
	m_btnOK.SetImage(TEXT("Skin/Wnd/GongGao/按钮_确定_默认状态.png"), 
		TEXT("Skin/Wnd/GongGao/按钮_确定_鼠标经过.png"));
	m_btnOK.MoveWindow(&rc);

	rc.OffsetRect(rc.Width() + 20, 0);
	m_btnCancel.SetImage(TEXT("Skin/Wnd/GongGao/button03.png"), 
		TEXT("Skin/Wnd/GongGao/按钮_取消_鼠标经过.png"));
	m_btnCancel.MoveWindow(&rc);

	MoveWindow(&rcClient, FALSE);
	CenterWindow();
	SetWindowRgn(rgn, TRUE);

	return TRUE;
}

BOOL CUptOptDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	AfxDrawImage(pDC, TEXT("Skin/Wnd/GongGao/bg_a.png"), rc);
	return TRUE;
}

LRESULT CUptOptDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

HBRUSH CUptOptDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(255, 255, 255));
	pDC->SetBkMode(1);
	pDC->SetBkColor(RGB(255, 255, 255));
	
	return brush; //hbr;
}

void CUptOptDlg::OnBnClickedOk()
{
	m_bIsQuick = m_btnQuick.GetCheck() == BST_CHECKED;
	OnOK();
}

void CUptOptDlg::OnBnClickedClose()
{
	OnCancel();
}
