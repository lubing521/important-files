// OptLgnDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "OptLgnDlg.h"
#include "MsgDlg.h"


// COptLgnDlg 对话框

IMPLEMENT_DYNAMIC(COptLgnDlg, CDialog)

COptLgnDlg::COptLgnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptLgnDlg::IDD, pParent)
{

}

COptLgnDlg::~COptLgnDlg()
{
}

void COptLgnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD,	m_edtPassword);
	DDX_Control(pDX, IDC_CAPTION,	m_btnCaption);
	DDX_Control(pDX, IDC_CLOSE,		m_btnClose);
	DDX_Control(pDX, IDC_TIP,		m_btnTip);
	DDX_Control(pDX, IDOK,			m_btnOK);
	DDX_Control(pDX, IDCANCEL,		m_btnCancel);
}

BEGIN_MESSAGE_MAP(COptLgnDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CLOSE, &COptLgnDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDOK, &COptLgnDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COptLgnDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL COptLgnDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcClient(0, 0, 469, 179);
	MoveWindow(&rcClient, FALSE);
	CRgn rgn;
	rgn.CreateRoundRectRgn(0, 0, rcClient.Width(), rcClient.Height(), 10, 10);

	CRect rc = CRect(10, 5, 10 + 100, 30);
	m_btnCaption.MoveWindow(&rc);

	rc = CRect(rcClient.right-56, 3, rcClient.right-15, 24+3);
	m_btnClose.MoveWindow(&rc);
	m_btnClose.SetImage(TEXT("Skin/Wnd/SysSet/按钮_关闭_默认状态.png"), 
		TEXT("Skin/Wnd/SysSet/按钮_关闭_鼠标经过.png"), TEXT("Skin/Wnd/SysSet/按钮_关闭_鼠标按下.png"));

	rc = CRect(188, 60, 400, 85);
	m_btnTip.MoveWindow(&rc);

	rc.OffsetRect(5, 32);
	rc.right = rc.left + 245; rc.bottom = rc.top + 16;
	m_edtPassword.MoveWindow(&rc);

	rc = CRect(200, 130, 200+97, 130+25);
	m_btnOK.SetImage(TEXT("Skin/Wnd/optlogin/按钮_登陆_默认状态.png"),
		TEXT("Skin/Wnd/optlogin/按钮_登陆_鼠标经过.png"));
	m_btnOK.MoveWindow(&rc);

	rc.OffsetRect(rc.Width() + 20, 0);
	m_btnCancel.SetImage(TEXT("Skin/Wnd/optlogin/按钮_取消_默认状态.png"),
		TEXT("Skin/Wnd/optlogin/按钮_取消_鼠标经过.png"));
	m_btnCancel.MoveWindow(&rc);

	SetWindowRgn(rgn, TRUE);
	CenterWindow();

	m_edtPassword.SetFocus();
	return FALSE;
}

BOOL COptLgnDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Wnd/optlogin/背景_管理员登陆.png"), rc);
	return TRUE;
}

LRESULT COptLgnDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

void COptLgnDlg::OnBnClickedClose()
{
	OnCancel();
}

void COptLgnDlg::OnBnClickedOk()
{
	CString szInput;
	m_edtPassword.GetWindowText(szInput);
	szInput.Trim();

	CString szPwd = AfxGetDbMgr()->GetStrOpt(OPT_M_CLIPWD);
	szPwd.Trim();

	char szBuf[MAX_PATH] = {0};
	strcpy(szBuf, _bstr_t(szInput));
	if (szInput.IsEmpty() || szPwd.IsEmpty() || CalBufCRC32((BYTE*)szBuf, strlen(szBuf)) != _ttoi(szPwd))
	{
		AfxMessageBoxEx(TEXT("密码不正确."));
		return;
	}

	OnOK();
}

void COptLgnDlg::OnBnClickedCancel()
{
	OnCancel();
}

HBRUSH COptLgnDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
