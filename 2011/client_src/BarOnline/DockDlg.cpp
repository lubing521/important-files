#include "stdafx.h"
#include "BarOnline.h"
#include "DockDlg.h"

IMPLEMENT_DYNAMIC(CDockDlg, CDialog)

CDockDlg::CDockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDockDlg::IDD, pParent)
{
    m_pbkImage = I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Dock/背景1像素.png"));
}

CDockDlg::~CDockDlg()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

void CDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDockDlg, CDialog)
	ON_MESSAGE(WM_DISPLAYCHANGE, &CDockDlg::OnDisplayChange)
	ON_BN_CLICKED(IDOK, &CDockDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDockDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CDockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    ResetDockPos();

	CRect rc;
	CRgn rgnDest, rgnRect, rgnRound;
	GetClientRect(&rc);
	rgnDest.CreateRectRgn(0, 0, rc.Width(), rc.Height()/2);
	rgnRect.CreateRectRgn(0, 0, rc.Width(), rc.Height()/2);
	rgnRound.CreateRoundRectRgn(0, 0, rc.Width()+1, rc.Height(), 5, 5);
	
	rgnDest.CombineRgn(&rgnRect, &rgnRound, RGN_OR);
	SetWindowRgn(rgnDest, TRUE);

	rc.left += 5; rc.right -= 5;
	m_DockNav.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_DockNav.SetDockMini(TRUE);
	m_DockNav.MoveWindow(&rc);

	return TRUE;
}
void CDockDlg::OnBnClickedOk()
{
}

void CDockDlg::OnBnClickedCancel()
{

}

void CDockDlg::PostNcDestroy()
{
	delete this;
}

LRESULT CDockDlg::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
    ResetDockPos();
	return 0;
}

void CDockDlg::ResetDockPos()
{
	int nSrcWidth = GetSystemMetrics(SM_CXSCREEN);
	int nSrcHeight = GetSystemMetrics(SM_CYSCREEN);
	CRect rc((nSrcWidth - FRAME_DOCK_WIDTH) / 2, 0, (nSrcWidth + FRAME_DOCK_WIDTH) / 2, FRAME_DOCK_HEIGHT);
	MoveWindow(&rc);
}

BOOL CDockDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rtClient, rtTemp;
	GetClientRect(&rtClient);

	rtTemp = rtClient; rtTemp.right = 5;	
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rtTemp);

	rtTemp = rtClient; rtTemp.left = rtTemp.right - 5;	
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rtTemp);

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}
