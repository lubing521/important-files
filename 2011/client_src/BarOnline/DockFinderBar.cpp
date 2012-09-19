#include "stdafx.h"
#include "DockFinderBar.h"
#include "UI/I8SkinCtrl/I8SkinCtrl.h"

#define DOCK_FINDER_INDEX			0x00002300

IMPLEMENT_DYNAMIC(CDockFinderBar, CStatic)

BEGIN_MESSAGE_MAP(CDockFinderBar, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(DOCK_FINDER_INDEX, DOCK_FINDER_INDEX + 0xff, OnClickButton)
END_MESSAGE_MAP()

CDockFinderBar::CDockFinderBar()
{
    m_pbkImage		= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/BrowseBar/±³¾°_ä¯ÀÀÆ÷¹¤¾ßÀ¸_1px.png"));
	m_pbkLeft		= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/BrowseBar/±³¾°_ä¯ÀÀÆ÷¹¤¾ßÀ¸_×ó.png"));
	m_pbkRight		= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/BrowseBar/±³¾°_ä¯ÀÀÆ÷¹¤¾ßÀ¸_ÓÒ.png"));
	m_pbAlpha		= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/FinderBar/±³¾°_×ÖÄ¸±³¾°_1px.png"));
	m_pbAlphaLeft	= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/FinderBar/±³¾°_×ÖÄ¸±³¾°_×ó.png"));
	m_pbAlphaRight	= I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/FinderBar/±³¾°_×ÖÄ¸±³¾°_ÓÒ.png"));
}

CDockFinderBar::~CDockFinderBar()
{
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
	if (m_pbkLeft)
	{
		m_pbkLeft->DeleteObject();
		delete m_pbkLeft;
	}
	if (m_pbkRight)
	{
		m_pbkRight->DeleteObject();
		delete m_pbkRight;
	}
	if (m_pbAlpha)
	{
		m_pbAlpha->DeleteObject();
		delete m_pbAlpha;
	}
	if (m_pbAlphaLeft)
	{
		m_pbAlphaLeft->DeleteObject();
		delete m_pbAlphaLeft;
	}
	if (m_pbAlphaRight)
	{
		m_pbAlphaRight->DeleteObject();
		delete m_pbAlphaRight;
	}
}

CString CDockFinderBar::GetText()
{
	CString str;
	m_edit.GetWindowText(str);
	if (str.CompareNoCase(DEFAULT_TIP_TEXT1) == 0)
		str = TEXT("");
	return str;
}

BOOL CDockFinderBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edit.m_hWnd)
	{
		if (pMsg->message == WM_KEYUP)
		{
			OnClickButton(DOCK_FINDER_INDEX+2);
		}
	}
	return CStatic::PreTranslateMessage(pMsg);
}

void CDockFinderBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	
	CRect rx = rc;
	rx.right = 6;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkLeft, rx);

	rx.left =  6;
	rx.right = rc.right - 6;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkImage, rx);

	rx.left = rc.right - 6;
	rx.right = rc.right;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbkRight, rx);
	
	rx = CRect(rc.right-613-6, 6, rc.right-6, rc.bottom - 6);
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbAlpha, rx);

	rx.right = rx.left + 5;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbAlphaLeft, rx);

	rx.left = rc.right - 11;
	rx.right = rx.left + 5;
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbAlphaRight, rx);
}

BOOL CDockFinderBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int  CDockFinderBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
		m_BtnGroup[idx].Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_FINDER_INDEX + idx);
		if (idx>=2)
			m_BtnGroup[idx].SetCursor(hCursor);
	}

	//ÎÄ±¾¿ò
	m_BtnGroup[0].SetImage(TEXT("Skin/FinderBar/±³¾°_¿ìËÙ²éÕÒ_×ó.png"));
	m_BtnGroup[1].SetImage(TEXT("Skin/FinderBar/±³¾°_¿ìËÙ²éÕÒ_1px.png"));
	m_edit.SetDefaultText(DEFAULT_TIP_TEXT1);
	m_edit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, CRect(0, 0, 0, 0), &m_BtnGroup[1], DOCK_FINDER_INDEX + sizeof(m_BtnGroup));
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_font.CreateFontIndirect(&lf);
	m_edit.SetFont(&m_font);
	m_edit.SetWindowText(DEFAULT_TIP_TEXT1);
	m_BtnGroup[2].SetImage(TEXT("Skin/FinderBar/°´Å¥_GO_Ä¬ÈÏ×´Ì¬.png"), 
		TEXT("Skin/FinderBar/°´Å¥_GO_Êó±ê¾­¹ý.png"), TEXT("Skin/FinderBar/°´Å¥_GO_°´ÏÂ.png"));

	//×ÖÏß±í
	m_BtnGroup[3].SetImage(TEXT("Skin/FinderBar/a_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/a_¼¤»î.png"));
	m_BtnGroup[4].SetImage(TEXT("Skin/FinderBar/b_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/b_¼¤»î.png"));
	m_BtnGroup[5].SetImage(TEXT("Skin/FinderBar/c_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/c_¼¤»î.png"));
	m_BtnGroup[6].SetImage(TEXT("Skin/FinderBar/d_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/d_¼¤»î.png"));
	m_BtnGroup[7].SetImage(TEXT("Skin/FinderBar/e_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/e_¼¤»î.png"));
	m_BtnGroup[8].SetImage(TEXT("Skin/FinderBar/f_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/f_¼¤»î.png"));
	m_BtnGroup[9].SetImage(TEXT("Skin/FinderBar/g_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/g_¼¤»î.png"));
	m_BtnGroup[10].SetImage(TEXT("Skin/FinderBar/h_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/h_¼¤»î.png"));
	m_BtnGroup[11].SetImage(TEXT("Skin/FinderBar/i_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/i_¼¤»î.png"));
	m_BtnGroup[12].SetImage(TEXT("Skin/FinderBar/j_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/j_¼¤»î.png"));
	m_BtnGroup[13].SetImage(TEXT("Skin/FinderBar/k_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/k_¼¤»î.png"));

	m_BtnGroup[14].SetImage(TEXT("Skin/FinderBar/l_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/l_¼¤»î.png"));
	m_BtnGroup[15].SetImage(TEXT("Skin/FinderBar/m_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/m_¼¤»î.png"));
	m_BtnGroup[16].SetImage(TEXT("Skin/FinderBar/n_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/n_¼¤»î.png"));
	m_BtnGroup[17].SetImage(TEXT("Skin/FinderBar/o_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/o_¼¤»î.png"));
	m_BtnGroup[18].SetImage(TEXT("Skin/FinderBar/p_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/p_¼¤»î.png"));
	m_BtnGroup[19].SetImage(TEXT("Skin/FinderBar/q_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/q_¼¤»î.png"));
	m_BtnGroup[20].SetImage(TEXT("Skin/FinderBar/r_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/r_¼¤»î.png"));
	m_BtnGroup[21].SetImage(TEXT("Skin/FinderBar/s_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/s_¼¤»î.png"));
	m_BtnGroup[22].SetImage(TEXT("Skin/FinderBar/t_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/t_¼¤»î.png"));
	
	m_BtnGroup[23].SetImage(TEXT("Skin/FinderBar/u_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/u_¼¤»î.png"));
	m_BtnGroup[24].SetImage(TEXT("Skin/FinderBar/v_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/v_¼¤»î.png"));
	m_BtnGroup[25].SetImage(TEXT("Skin/FinderBar/w_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/w_¼¤»î.png"));
	m_BtnGroup[26].SetImage(TEXT("Skin/FinderBar/x_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/x_¼¤»î.png"));
	m_BtnGroup[27].SetImage(TEXT("Skin/FinderBar/y_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/y_¼¤»î.png"));
	m_BtnGroup[28].SetImage(TEXT("Skin/FinderBar/z_Ä¬ÈÏ×´Ì¬.png"), TEXT("Skin/FinderBar/z_¼¤»î.png"));
	for (int idx=3; idx<29;idx++)
	{
		HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
		m_BtnGroup[idx].SetCursor(hCursor);
	}

	return 0;
}

void CDockFinderBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);

	if (m_BtnGroup[0].m_hWnd == NULL)
		return ;

	CRect rcClient;
	GetClientRect(&rcClient);

	//¿ìËÙ²éÕÒ
	CRect rc(6, 6, 102+6, 31);
	m_BtnGroup[0].MoveWindow(&rc);

	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + (rcClient.Width() - 780);	// 603 + 5 + 5 + 4 + 6+ 6+ 49 + 102.
	m_BtnGroup[1].MoveWindow(&rc);
	{
		CRect rx;
		m_BtnGroup[1].GetClientRect(&rx);
		rx.DeflateRect(5, 7, 5, 5);
		m_edit.MoveWindow(&rx);
	}
	
	rc.OffsetRect(rc.Width(), 0);
	rc.right = rc.left + 49;
	m_BtnGroup[2].MoveWindow(&rc);

	//×ÖÏß±í
	rc.left = rcClient.right - 6 - 603 - 5;
	int width_alp[] = {26, 22, 24, 25, 22, 21, 25, 24, 16, 21, 23, 21, 27, 26, 25, 22, 26, 23, 22, 23, 24, 21, 28, 23, 23, 20};
	int nWid = 0;
	for (int idx=0; idx<_countof(width_alp); idx++)
	{
		rc.right = rc.left + width_alp[idx];
		m_BtnGroup[idx+3].MoveWindow(&rc);
		rc.OffsetRect(rc.Width(), 0);
		nWid +=width_alp[idx];
	}
}

void CDockFinderBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		id -= DOCK_FINDER_INDEX;
		LPARAM param = 0;
		if (id > 2 && id < 29)
		{			
			CString str;
			//m_edit.GetWindowText(str);
			//if (str.CompareNoCase(m_edit.GetDefaultText()) == 0)
			//	  str = TEXT("");
			str += TCHAR(TEXT('A') + id - 3);
			m_edit.SetWindowText(str);
			id = 2;
			param = 1;
		}
		pParent->SendMessage(WM_FINDER_CLICK_MESSAGE, id , param);
	}
}
