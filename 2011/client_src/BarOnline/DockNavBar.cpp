#include "stdafx.h"
#include "DockNavBar.h"

#define DOCK_NAVBAR_INDEX			0x00002000

IMPLEMENT_DYNAMIC(CDockNavBar, CStatic)

BEGIN_MESSAGE_MAP(CDockNavBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_NAVBAR_INDEX, DOCK_NAVBAR_INDEX + 0xff, OnClickButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CDockNavBar::CDockNavBar()
{
    m_bIsDockMini = FALSE;
    m_pbkImage		= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Dock/背景1像素.png"));
    m_pbkLogoImage	= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Dock/logo.png"));
    m_pbkTipImage	= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Dock/关闭按钮_背景.png"));
    m_pHeadRightImage	= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Dock/head_right.png"));
    m_nActive = -1;
    for (int idx=0; idx<_countof(m_NavClicks); idx++)
    {
        m_NavClicks[idx].dwIdx = idx+1;
        m_NavClicks[idx].dwClick = 0;
        m_NavClicks[idx].dwRunTime = 0;
    }
}

CDockNavBar::~CDockNavBar()
{
    if (m_pbkImage)
    {
        m_pbkImage->DeleteObject();
        delete m_pbkImage;
    }
    if (m_pbkLogoImage)
    {
        m_pbkLogoImage->DeleteObject();
        delete m_pbkLogoImage;
    }
    if (m_pbkTipImage)
    {
        m_pbkTipImage->DeleteObject();
        delete m_pbkTipImage;
    }
    if (m_pHeadRightImage)
    {
        m_pHeadRightImage->DeleteObject();
        delete m_pHeadRightImage;
    }
}

void CDockNavBar::SetActive(int idx)
{
	for (int loop=0; loop<6; loop++)
	{
		if (loop == idx)
			m_btnNav[2*loop].SetActive(TRUE);
		else
			m_btnNav[2*loop].SetActive(FALSE);
	}
	m_nActive = idx;
	if (idx>=0 && idx<_countof(m_NavClicks))
		m_NavClicks[idx].dwClick++;
}

void CDockNavBar::SetAllUnActive()
{
	for (int loop=0; loop<6; loop++)
	{
		m_btnNav[2*loop].SetActive(FALSE);
	}
	m_nActive = -1;
}

void CDockNavBar::SetDockMini(BOOL bIsMini)
{
	m_bIsDockMini = bIsMini;
	if (m_btnNetBar.m_hWnd != NULL)
	{
		PostMessage(WM_SIZE);
		m_btnNetBar.ShowWindow(!bIsMini);
		for (int idx=0; idx<_countof(m_btnTool)&&idx!=3; idx++)
			m_btnTool[idx].ShowWindow(!bIsMini);

		RedrawWindow();		
	}
}

void CDockNavBar::SetNetBarText(CString& str)
{
	m_btnNetBar.SetWindowText(str);
}

void CDockNavBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	if (!m_bIsDockMini)
	{
		CRect rx = rc;
		rx.right = 65;
		I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkLogoImage, rx);

		//rx = rc;
		//rx.left = rc.right - 241;
		//rx.bottom = 27;
		//I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkTipImage, rx);

		rx = rc;
		rx.left = rx.right - 260;
		rx.bottom = rc.bottom;
		I8SkinCtrl_ns::I8_DrawImage(pDC, m_pHeadRightImage, rx);

		//6个导航栏按钮两边的拉伸(6个按钮的宽度(82+2)*6-2=502)
		rx = rc;
		rx.right = rc.right - 260;
		rx.left = (rc.Width() + 502)/2;
		I8SkinCtrl_ns::I8_DrawImage(pDC,m_pbkImage, rx);

		rx.left = 65+206;
		rx.right = (rc.Width() - 502)/2;
		I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rx);
	}
	else
	{
		//CRect rx = rc;
		//rx.right = 7;
		//I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rx);

		//rx = rc;
		//rx.left = rc.right - 7;
		//I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rx);
	}
}

int CDockNavBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	

	CRect rc(0, 0, 0, 0);	
	for (int idx=0; idx<6; idx++)
	{
		m_btnNav[2*idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 
			DOCK_NAVBAR_INDEX + idx);
		m_btnNav[2*idx+1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, 0xffff);
		m_btnNav[2*idx+1].SetImage(TEXT("Skin/Dock/背景_头部_按钮分割线.png"));
		m_btnNav[2*idx].SetCursor(hCursor);
	}
	m_btnNav[0].SetImage(TEXT("Skin/Dock/按钮_浏览器_默认状态.png"), TEXT("Skin/Dock/按钮_浏览器_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_浏览器_鼠标按下.png"), TEXT("Skin/Dock/按钮_浏览器_激活.png"), NULL, TRUE);
	m_btnNav[2].SetImage(TEXT("Skin/Dock/按钮_游戏大厅_默认状态.png"), TEXT("Skin/Dock/按钮_游戏大厅_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_游戏大厅_鼠标按下.png"), TEXT("Skin/Dock/按钮_游戏大厅_激活.png"), NULL, TRUE);
	m_btnNav[4].SetImage(TEXT("Skin/Dock/按钮_聊天工具_默认状态.png"), TEXT("Skin/Dock/按钮_聊天工具_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_聊天工具_鼠标按下.png"), TEXT("Skin/Dock/按钮_聊天工具_激活.png"), NULL, TRUE);
	m_btnNav[6].SetImage(TEXT("Skin/Dock/按钮_常用软件_默认状态.png"), TEXT("Skin/Dock/按钮_常用软件_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_常用软件_鼠标按下.png"), TEXT("Skin/Dock/按钮_常用软件_激活.png"), NULL, TRUE);
	m_btnNav[8].SetImage(TEXT("Skin/Dock/按钮_点卡商城_默认状态.png"), TEXT("Skin/Dock/按钮_点卡商城_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_点卡商城_鼠标按下.png"), TEXT("Skin/Dock/按钮_点卡商城_激活.png"), NULL, TRUE);
	m_btnNav[10].SetImage(TEXT("Skin/Dock/按钮_影视音乐_默认状态.png"), TEXT("Skin/Dock/按钮_影视音乐_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_影视音乐_鼠标按下.png"), TEXT("Skin/Dock/按钮_影视音乐_激活.png"), NULL, TRUE);

	DWORD dwID = DOCK_NAVBAR_INDEX + 6;
	m_btnTool[0].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[1].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[2].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[3].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[4].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnTool[5].Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnNetBar.Create(TEXT(""), WS_VISIBLE|WS_CHILD, rc, this, dwID++);
	m_btnNetBar.SetImage(TEXT("Skin/Dock/网吧名称_背景_默认状态.png"), TEXT("Skin/Dock/网吧名称_背景_鼠标经过.png"), 
		TEXT("Skin/Dock/网吧名称_背景_鼠标按下.png"));
	m_btnNetBar.SetTextStyle(DEFAULT_FONT_NAME, 12, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, FALSE, FALSE, FALSE, FALSE, I8SkinCtrl_ns::CI8OldButton::BTYPE_NBTITLE);
	m_btnNetBar.SetCursor(hCursor);

	m_btnTool[0].SetImage(TEXT("Skin/Dock/按钮_缩小_默认.png"), TEXT("Skin/Dock/按钮_缩小_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_缩小_按下.png"));
	m_btnTool[1].SetImage(TEXT("Skin/Dock/按钮_扩大_默认.png"), TEXT("Skin/Dock/按钮_扩大_鼠标经过.png"), 
		TEXT("Skin/Dock/按钮_扩大_按下.png"));
	m_btnTool[2].SetImage(TEXT("Skin/Dock/按钮_关闭_默认.png"), TEXT("Skin/Dock/按钮_关闭_鼠标经过.png"),
		TEXT("Skin/Dock/按钮_关闭_按下.png"));

	m_btnTool[3].SetImage(TEXT("Skin/Dock/按钮_个性设置_默认状态.png"), 
		TEXT("Skin/Dock/按钮_个性设置_鼠标经过.png"), TEXT("Skin/Dock/按钮_个性设置_鼠标按下.png"));
	m_btnTool[4].SetImage(TEXT("Skin/Dock/按钮_个人U盘_默认状态.png"),
		TEXT("Skin/Dock/按钮_个人U盘_鼠标经过.png"), TEXT("Skin/Dock/按钮_个人U盘_鼠标按下.png"));
	m_btnTool[5].SetImage(TEXT("Skin/Dock/按钮_收藏夹_默认状态.png"),
		TEXT("Skin/Dock/按钮_收藏夹_鼠标经过.png"), TEXT("Skin/Dock/按钮_收藏夹_鼠标按下.png"));

	m_btnTool[3].ShowWindow(SW_HIDE);
	m_btnTool[3].SetCursor(hCursor);
	m_btnTool[4].SetCursor(hCursor);
	m_btnTool[5].SetCursor(hCursor);

	SetTimer(1, 1000, NULL);
	return 0;
}

BOOL CDockNavBar::OnEraseBkgnd(CDC* pDC)
{
	//return CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

void CDockNavBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	
	CRect rcClient, rc;
	GetClientRect(&rcClient);
	rc.bottom = 59;
	rc = rcClient;

	if (!m_bIsDockMini)
	{
		//netbar.
		rc.left = 65;
		rc.right = rc.left + 206;
		m_btnNetBar.MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);

		//让导航栏剧中
		rc.left = (rcClient.Width() - 502)/2;
		if (rc.left<65+206)
			rc.left = 65+206;
	}
	else
	{
	//	rc.left = 7;
	}
	rc.right = rc.left + 82;
	for (int idx=0; idx<6; idx++)
	{
		m_btnNav[idx*2].MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);
		if (idx < 5)
		{
			m_btnNav[idx*2+1].MoveWindow(CRect(rc.left, rc.top, rc.left + 2, rc.bottom));
			rc.OffsetRect(2, 0);
		}
	}

	if (!m_bIsDockMini)
	{
		//tool.
		rc.top = 4;
		rc.left = rcClient.right- 75;
		rc.right = rc.left + 22;
		rc.bottom = rc.top + 20;
		m_btnTool[0].MoveWindow(&rc);

		rc.OffsetRect(rc.Width(), 0);
		rc.right = rc.left + 31;
		m_btnTool[1].MoveWindow(&rc);

		rc.OffsetRect(rc.Width(), 0);
		rc.right = rc.left + 17;
		m_btnTool[2].MoveWindow(&rc);

		//return ;
		rc = rcClient;
		rc.left = rc.right - 241;
		rc.right = rc.left + 104;
		rc.top = 27; rc.bottom = 59;
		m_btnTool[3].MoveWindow(&rc);

		rc.OffsetRect(rc.Width()+3, 0);
		rc.right = rc.left + 63;
		m_btnTool[4].MoveWindow(&rc);

		rc.OffsetRect(rc.Width()+3, 0);
		rc.right = rc.left + 63;
		m_btnTool[5].MoveWindow(&rc);
	}
}

void CDockNavBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		DWORD dwId = id - DOCK_NAVBAR_INDEX;
		pParent->SendMessage(WM_NAV_CLICK_MESSAGE, dwId, 0);
	}
}

void CDockNavBar::OnTimer(UINT_PTR nIDEvent)
{
	CStatic::OnTimer(nIDEvent);
	int nSel = GetActive();
	if (nSel == -1 || m_bIsDockMini)
		return ;

	m_NavClicks[nSel].dwRunTime++;
// #ifdef _DEBUG
// 	OutputDebugString(TEXT("\r\n"));
// 	for (int idx=0; idx<_countof(m_NavClicks); idx++)
// 	{
// 		TCHAR szLog[MAX_PATH] = {0};
// 		_stprintf(szLog, TEXT("Idx=%d, Click=%d, RunTime=%d; "), m_NavClicks[idx].dwIdx, 
// 			m_NavClicks[idx].dwClick, m_NavClicks[idx].dwRunTime);
// 		OutputDebugString(szLog);
// 	}
// 	OutputDebugString(TEXT("\r\n"));
// #endif // _DEBUG
}

void CDockNavBar::GetAllNavClick(tagNavClick* pNavClick)
{
	for (int idx=0; idx<_countof(m_NavClicks); idx++)
	{
		pNavClick[idx].dwIdx = m_NavClicks[idx].dwIdx;
		pNavClick[idx].dwClick = m_NavClicks[idx].dwClick;
		pNavClick[idx].dwRunTime = m_NavClicks[idx].dwRunTime;

		m_NavClicks[idx].dwClick  = 0;
		m_NavClicks[idx].dwRunTime = 0;
	}
}

