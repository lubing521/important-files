#include "stdafx.h"
#include "DockStatusBar.h"

#define DOCK_STABAR_INDEX			0x00002100

IMPLEMENT_DYNAMIC(CDockStatusBar, CStatic)

BEGIN_MESSAGE_MAP(CDockStatusBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_STABAR_INDEX, DOCK_STABAR_INDEX + 0xff, OnClickButton)
END_MESSAGE_MAP()

CDockStatusBar::CDockStatusBar()
{
	m_pbkImage = I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/StatusBar/背景_状态栏_1px.png"));
	m_pQuickImage = I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/StatusBar/背景_状态栏_左.png"));
}

CDockStatusBar::~CDockStatusBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pQuickImage)
	{
		m_pQuickImage->DeleteObject();
		delete m_pQuickImage;
	}
}

void CDockStatusBar::SetQuickRunGroup(int nIndex, HICON hIcon, CString strCaption)
{
	if (nIndex <0 || nIndex > 4)
		return ;

	m_BtnGroup[nIndex].SetIconImage(hIcon);
	m_BtnGroup[nIndex].SetWindowText(strCaption);

	PostMessage(WM_SIZE);
}

void CDockStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rc);
	rc.right = rc.left + 65;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pQuickImage, rc);
}

BOOL CDockStatusBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int  CDockStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_STABAR_INDEX+idx);
		m_BtnGroup[idx].SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
			I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT, FALSE, FALSE, FALSE, FALSE, I8SkinCtrl_ns::CI8OldButton::BTYPE_QUIKRUN);
		if (idx<5)
			m_BtnGroup[idx].SetImage(TEXT("Skin/StatusBar/背景_状态栏_16x16图标背景.png"));
	}

	m_BtnGroup[5].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_左.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_1px.png"));

	m_WebPage.SetNoScroll();
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), &m_BtnGroup[6], 0xffff);
	m_WebPage.SetSilent(TRUE);

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar10_wz.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID),  szMacAddr);
	m_WebPage.Navigate2(strUrl);

	m_BtnGroup[7].SetImage(TEXT("Skin/StatusBar/背景_网吧公告_右.png"));
	m_BtnGroup[8].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_分辨率_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_分辨率_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_分辨率_鼠标按下.png"));
	m_BtnGroup[9].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_鼠标_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_鼠标_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_鼠标_鼠标按下.png"));
	m_BtnGroup[10].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_音量_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_音量_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_音量_鼠标按下.png"));

	m_BtnGroup[11].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_锁定_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_锁定_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_锁定_鼠标按下.png"));
	m_BtnGroup[12].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_工具_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_工具_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_工具_鼠标按下.png"));

	m_BtnGroup[13].SetImage(TEXT("Skin/StatusBar/背景_状态栏_右.png"));

	m_BtnGroup[14].SetImage(TEXT("Skin/StatusBar/按钮_状态栏_输入法_默认状态.png"), 
		TEXT("Skin/StatusBar/按钮_状态栏_输入法_鼠标经过.png"), TEXT("Skin/StatusBar/按钮_状态栏_输入法_鼠标按下.png"));

	for (int idx=0; idx<_countof(m_BtnGroup); idx++)
	{
		m_BtnGroup[idx].SetCursor(hCursor);
	}
	return 0;
}

void CDockStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (m_BtnGroup[0].m_hWnd == NULL)
		return ;
	CRect rcClient;
	GetClientRect(&rcClient);

	//快速启动按钮.
	CRect rc(65, 5, 89, 28);
	for (int idx=0; idx<5; idx++)
	{
		if (m_BtnGroup[idx].IsHasIcon())
		{
			m_BtnGroup[idx].MoveWindow(&rc);
		}
		else
		{
			m_BtnGroup[idx].MoveWindow(&CRect(0, 0, 0, 0));
		}
		rc.OffsetRect(rc.Width()+4, 0);
	}

	//网吧公告按钮
	rc.left += 2;
	rc.right = rc.left + 88;
	m_BtnGroup[5].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rcClient.Width() - 450 + 11;
	m_BtnGroup[6].MoveWindow(&rc);
	{
		CRect rx;
		m_BtnGroup[6].GetClientRect(&rx);
		m_WebPage.MoveWindow(&rx);
	}

	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 3;
	m_BtnGroup[7].MoveWindow(&rc);

	//鼠标等工具栏.	
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 73;
	m_BtnGroup[14].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 73;
	m_BtnGroup[8].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 62;
	m_BtnGroup[9].MoveWindow(&rc);
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 65;
	m_BtnGroup[10].MoveWindow(&rc);

	//锁定按钮
	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 117;
	m_BtnGroup[11].MoveWindow(&rc);

	rc.OffsetRect(rc.Width() + 4, 0);
	rc.right = rc.left + 28;
	m_BtnGroup[12].MoveWindow(&rc);

	//缩放按钮
	int right = rc.right;
	GetClientRect(&rc);
	if (rc.right > right)
	{		
		rc.left = rc.right - 13;
		//m_BtnGroup[12].MoveWindow(&rc);
	}
}

void CDockStatusBar::OnClickButton(UINT id)
{
	switch (id)
	{
	case 0x2108:
		WinExec("rundll32.exe shell32.dll, Control_RunDLL desk.cpl,,3", SW_SHOWNORMAL);
		break;
	case 0x2109:
		WinExec("rundll32.exe shell32.dll, Control_RunDLL main.cpl @0", SW_SHOWNORMAL);
		break;
	case 0x210a:
		//WinExec("rundll32.exe shell32.dll, Control_RunDLL mmsys.cpl @1", SW_SHOWNORMAL);
		WinExec("Sndvol32.exe", SW_SHOWNORMAL);
		break;
	}
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		pParent->SendMessage(WM_STATUS_CLICK_MESSAGE, id - DOCK_STABAR_INDEX, 0);
	}
}
