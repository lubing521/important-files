// GongGaoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "GongGaoDlg.h"
#include "MainFrame.h"

IMPLEMENT_DYNAMIC(CGongGaoDlg, CDialog)

CGongGaoDlg::CGongGaoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDockDlg::IDD, pParent)
{
}

CGongGaoDlg::~CGongGaoDlg()
{
}

void CGongGaoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_WELCOME, m_btnWelcome);
	DDX_Control(pDX, IDC_CONTEXT, m_btnContext);
	DDX_Control(pDX, IDC_NBINFO,  m_btnNbInfo);
}

BEGIN_MESSAGE_MAP(CGongGaoDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_CLOSE, &CGongGaoDlg::OnBnClickedClose)
END_MESSAGE_MAP()

BOOL CGongGaoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcClient(0, 0, 528, 629), rc;
	CRgn rgn;
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 10, 10);

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar09_gg.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID), szMacAddr);
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(14, 40, 500+14, 250+40), this, AFX_IDW_PANE_FIRST);
	m_WebPage.SetSilent(TRUE);
	m_WebPage.Navigate2(strUrl);

	rc = CRect(35, 10, 220, 34);
	m_btnWelcome.MoveWindow(&rc);
	m_btnWelcome.SetTextStyle(DEFAULT_FONT_NAME, 16, 
		RGB(255, 255, 0), RGB(255, 255, 0), RGB(255, 255, 0), RGB(255, 255, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, TRUE);

	rc = CRect(25, 345, rcClient.right - 25, 590);
	m_btnContext.MoveWindow(&rc);
	CString szContent = AfxGetDbMgr()->GetStrOpt(OPT_M_GGMSG);
	CString szFont = AfxGetDbMgr()->GetStrOpt(OPT_M_GGFONT);
	CString szFontName, szFontSize, szFontColor, szFontFlag;
	AfxExtractSubString(szFontName, szFont, 0, TEXT('|'));
	AfxExtractSubString(szFontSize, szFont, 1, TEXT('|'));
	AfxExtractSubString(szFontColor,szFont, 2, TEXT('|'));
	AfxExtractSubString(szFontFlag, szFont, 3, TEXT('|'));
	COLORREF clr = _ttoi(szFontColor);
	DWORD dwFlag = _ttoi(szFontFlag);
	//标志(bit0:粗体，bit1:斜线,bit2:下画线,bit3:删除线))
	m_btnContext.SetTextStyle(szFontName, _ttoi(szFontSize), clr, clr, clr, clr, I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT,
		dwFlag & 0x1, dwFlag & 0x2, dwFlag & 0x4, dwFlag & 0x8, I8SkinCtrl_ns::CI8OldButton::BTYPE_NBCONTENT, FALSE);
	m_btnContext.SetWindowText(szContent);

	CString szNbInfo = AfxGetDbMgr()->GetStrOpt(OPT_U_NBNAME) + TEXT("  ") + 
		AfxGetDbMgr()->GetStrOpt(OPT_U_NBTITLE) + TEXT("  ") + AfxGetDbMgr()->GetStrOpt(OPT_U_NBSUBTITLE);
	rc = CRect(20, 600, rcClient.right - 20, 629);
	m_btnNbInfo.MoveWindow(&rc);
	m_btnNbInfo.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), 
		RGB(255, 255, 255), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER, TRUE);
	m_btnNbInfo.SetWindowText(szNbInfo);

	rc = CRect(rcClient.right-56, 3, rcClient.right-15, 24+3);
	m_btnClose.MoveWindow(&rc);
	m_btnClose.SetImage(TEXT("Skin/Wnd/GongGao/按钮_关闭_默认状态.png"), 
		TEXT("Skin/Wnd/GongGao/按钮_关闭_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_关闭_按下.png"));

	MoveWindow(&rcClient, FALSE);
	SetWindowRgn(rgn, TRUE);
	CenterWindow();

	return TRUE;
}

void CGongGaoDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_pDlgGongGao = NULL;
}

BOOL CGongGaoDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
void CGongGaoDlg::OnPaint()
{
	CRect rc;
	GetClientRect(&rc);
	CPaintDC dc(this);
	I8SkinCtrl_ns::I8_DrawImage(&dc, TEXT("Skin/Wnd/GongGao/背景_网吧公告.png"), rc);
}

LRESULT CGongGaoDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

void CGongGaoDlg::OnOK()
{
	OnBnClickedClose();
}

void CGongGaoDlg::OnCancel()
{
	OnBnClickedClose();
}

void CGongGaoDlg::OnBnClickedClose()
{
	DestroyWindow();
}

void CGongGaoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}