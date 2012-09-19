#include "stdafx.h"
#include "DeskLocker.h"
#include "DlgLgn.h"

IMPLEMENT_DYNAMIC(CDlgLgn, CDialog)

CDlgLgn::CDlgLgn(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLgn::IDD, pParent)
{
	m_strPwd = TEXT("");
}

CDlgLgn::~CDlgLgn()
{
}

void CDlgLgn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE,		m_btnClose);
	DDX_Control(pDX, IDC_PASSWORD,	m_edtPwd);
	DDX_Control(pDX, IDC_PASSWORD2,	m_edtPwd2);
	DDX_Control(pDX, IDOK,			m_btnOk);
	DDX_Control(pDX, IDCANCEL,		m_btnCancel);
}

BEGIN_MESSAGE_MAP(CDlgLgn, CDialog)
	ON_BN_CLICKED(IDOK,		&CDlgLgn::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgLgn::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLOSE,&CDlgLgn::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL CDlgLgn::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(TEXT("挂机锁"));


	CRect rc(0, 0, 463, 215);
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	MoveWindow(&rc, FALSE);

	CRgn rgn;
	rgn.CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 5, 5);

	CRect rx(rc.right - 51, 3, rc.right - 10, 27);
	m_btnClose.MoveWindow(&rx);
	m_btnClose.SetImage(IDB_CLOSE, IDB_CLOSE1, IDB_CLOSE2);

	rx = CRect(250, 100, 435, 100+15);
 	m_edtPwd.MoveWindow(&rx);

	rx.OffsetRect(0, 30);
	m_edtPwd2.MoveWindow(&rx);

	rx = CRect(182, 160, 182+97, 160+25);
	m_btnOk.MoveWindow(&rx);
	m_btnOk.SetImage(IDB_OK);

	rx.OffsetRect(rx.Width() + 10, 0);
	m_btnCancel.MoveWindow(&rx);
	m_btnCancel.SetImage(IDB_CANCEL);

	MoveWindow(&rc, FALSE);
	SetWindowRgn(rgn, TRUE);
	CenterWindow();

	m_edtPwd.SetFocus();
	return FALSE;
}

void CDlgLgn::OnBnClickedOk()
{
	CString m_strPwd2;
	m_edtPwd.GetWindowText(m_strPwd);
	m_edtPwd2.GetWindowText(m_strPwd2);

	if (m_strPwd.IsEmpty() || m_strPwd != m_strPwd2)
	{
		AfxMessageBox(TEXT("两次输入的密码不对，或者密码为空!"));
		return ;
	}

	OnOK();
}

void CDlgLgn::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CDlgLgn::OnEraseBkgnd(CDC* pDC)
{
	int nOldMode = pDC->SetBkMode(1);
	COLORREF nOldColor = pDC->SetTextColor(RGB(255, 255, 255));
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	lf.lfHeight = -12;
	CFont font;
	font.CreateFontIndirect(&lf);
	CFont* pOldFont = pDC->SelectObject(&font);

	CRect rc;
	GetClientRect(&rc);
	AfxDrawImage(pDC, IDB_LOGIN, rc);

	CRect rx(5, 5, 100, 25);
	pDC->DrawText(TEXT("锁定计算机"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	rx = CRect(182, 50, 400, 70);
	pDC->DrawText(TEXT("请设定一个锁定密码"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	rx.OffsetRect(0, rx.Height());
	pDC->SetTextColor(RGB(0, 255, 0));
	pDC->DrawText(TEXT("当你需要解除锁定的时候需要此密码"), -1, &rx, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	
	pDC->SetBkMode(nOldMode);
	pDC->SetTextColor(nOldColor);
	pDC->SelectObject(pOldFont);

	return TRUE;
}

HBRUSH CDlgLgn::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(250, 250, 250));
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		pDC->SetBkColor(RGB(250, 250, 250));
		return brush;
	}
	return hbr;
}

LRESULT CDlgLgn::OnNcHitTest(CPoint point)
{
	LRESULT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
	{
		nHitTest = HTCAPTION;
	}
	return nHitTest;
}
