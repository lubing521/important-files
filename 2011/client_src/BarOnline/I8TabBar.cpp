#include "stdafx.h"
#include "I8TabBar.h"
#include <shlwapi.h>

using namespace I8SkinCtrl_ns;

#define SKIN_FILE_NAME              TEXT("Skin.ini")
#define DOCK_TAB_INDEX				0x00002400

IMPLEMENT_DYNAMIC(CI8TabBar, CStatic)

BEGIN_MESSAGE_MAP(CI8TabBar, CStatic)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_COMMAND_RANGE(DOCK_TAB_INDEX, DOCK_TAB_INDEX+16, OnClickButton)
END_MESSAGE_MAP()

CI8TabBar::CI8TabBar()
{
	m_pbkImage = I8_GetSkinImage(TEXT("Skin/TabBar/背景_标签栏_1px.png"));
	m_nActive = -1;

	GetTabSkinColor();
}

CI8TabBar::~CI8TabBar()
{
	RemoveAllTab();
	if (m_pbkImage)
	{
		m_pbkImage->DeleteObject();
		delete m_pbkImage;
	}
}

void CI8TabBar::GetTabSkinColor()
{
    m_clrNormal = RGB(255, 255, 255);;
	m_clrHover  = RGB(255, 216, 0);
	m_clrSel    = RGB(0, 0, 0);

	TCHAR szSkinPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szSkinPath, MAX_PATH);
	PathRemoveFileSpec(szSkinPath);
	PathAddBackslash(szSkinPath);
	lstrcat(szSkinPath, TEXT("Skin\\"));
	lstrcat(szSkinPath, SKIN_FILE_NAME);
    BOOL bRet = I8SkinCtrl_ns::I8_ExtractItem(TEXT("Skin/skin.ini"), szSkinPath);
	if (bRet)
	{
		TCHAR szValue[MAX_PATH] = {0}; TCHAR* pTemp = NULL; DWORD dwRet = 0;
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("NormalColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	 m_clrNormal = wcstol(szValue, &pTemp, 16);

        ZeroMemory(szValue, MAX_PATH);
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("HoverColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	 m_clrHover = wcstol(szValue, &pTemp, 16);

		ZeroMemory(szValue, MAX_PATH);
		dwRet = GetPrivateProfileString(TEXT("TabControl"), TEXT("SelectColor"), TEXT(""), szValue, MAX_PATH, szSkinPath);
		if (dwRet != 0)	m_clrSel = wcstol(szValue, &pTemp, 16);
	}
}


I8SkinCtrl_ns::CI8OldButton* CI8TabBar::AddTab(LPCTSTR lpszCap, LPCTSTR iconImage, LPCTSTR iconSelImage, BOOL bRefresh /* = FALSE */)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	I8SkinCtrl_ns::CI8OldButton* pTab = new I8SkinCtrl_ns::CI8OldButton;
	pTab->Create(TEXT("Tab"), BS_CENTER|WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, DOCK_TAB_INDEX+1+m_BtnTabGroup.size());
	pTab->SetImage(TEXT("Skin/TabBar/背景_未选中标签.png"), TEXT("Skin/TabBar/背景_未选中标签.png"),
		TEXT("Skin/TabBar/背景_选中标签.png"), TEXT("Skin/TabBar/背景_选中标签.png"));
	pTab->SetWindowText(lpszCap);
	pTab->SetTextStyle(DEFAULT_FONT_NAME, 12, m_clrNormal, m_clrHover, m_clrSel, m_clrSel, I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER, FALSE, FALSE, FALSE, FALSE, I8SkinCtrl_ns::CI8OldButton::BTYPE_NORMAL, TRUE);
	pTab->SetIconImage(iconImage, iconSelImage);
	m_BtnTabGroup.push_back(pTab);
	pTab->SetCursor(hCursor);
	if (bRefresh)
		PostMessage(WM_SIZE);
	return pTab;
}

CString  CI8TabBar::GetTabText(int index)
{
	CString str;
	if (m_BtnTabGroup.size() > (size_t)index)
	{
		m_BtnTabGroup[index]->GetWindowText(str);
	}
	return str;
}
void CI8TabBar::RemoveAllTab()
{
	for (std::vector<I8SkinCtrl_ns::CI8OldButton*>::iterator it = m_BtnTabGroup.begin(); 
		it != m_BtnTabGroup.end(); it++)
	{
		I8SkinCtrl_ns::CI8OldButton* pTab = *it;
		if (pTab != NULL)
		{
			pTab->DestroyWindow();
			delete pTab;
		}
	}
	if (m_hWnd != NULL)
		PostMessage(WM_SIZE);
	m_BtnTabGroup.clear();
}

void CI8TabBar::ShowUrlNav(BOOL bShow)
{
	if (m_btnFind.GetSafeHwnd() != NULL)
		m_btnFind.ShowWindow(bShow);
}

void CI8TabBar::SetActive(int idx)
{
	for (size_t loop=0; loop<m_BtnTabGroup.size(); loop++)
	{
		if (idx == loop)
			m_BtnTabGroup[loop]->SetActive(TRUE);
		else
			m_BtnTabGroup[loop]->SetActive(FALSE);
	}
	m_nActive = idx;
}

void CI8TabBar::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CRect rc(lpDis->rcItem);
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	I8_DrawImage(pDC, m_pbkImage, rc);
	return ;
}

BOOL CI8TabBar::OnEraseBkgnd(CDC* pDC)
{
	//CStatic::OnEraseBkgnd(pDC);
	return TRUE;
}

int CI8TabBar:: OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
	if (-1 == CStatic::OnCreate(lpCreateStruct))
		return -1;

	BOOL b = ModifyStyle(0, SS_OWNERDRAW);

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= WS_CLIPCHILDREN;
	lStyle |= WS_CLIPSIBLINGS;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	m_btnBoderLeft.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 6, 32), this, 0xffff);
	m_btnBorderRight.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, 0xffff);
	m_btnBoderLeft.SetImage(TEXT("Skin/TabBar/背景_标签栏_左.png"));
	m_btnBorderRight.SetImage(TEXT("Skin/TabBar/背景_标签栏_右.png"));

	m_btnFind.Create(TEXT(""), WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, DOCK_TAB_INDEX);
	m_btnFind.SetImage(TEXT("Skin/TabBar/选游戏_未选中状态.png"), NULL, TEXT("Skin/TabBar/选游戏_选中状态.png"));
	m_btnFind.SetCursor(hCursor);

	return 0;
}

void CI8TabBar::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	
	if (m_btnFind.m_hWnd == NULL)
		return ;

	CRect rcClient, rc;
	GetClientRect(&rcClient);
	rc = rcClient;
	rc.right = 6;
	m_btnBoderLeft.MoveWindow(&rc);

	rc = CRect(6, 2, 91, 29);
	for (std::vector<I8SkinCtrl_ns::CI8OldButton*>::iterator it = m_BtnTabGroup.begin(); 
		it != m_BtnTabGroup.end(); it++)
	{
		I8SkinCtrl_ns::CI8OldButton* pTab = *it;
		if (pTab == NULL || pTab->m_hWnd == NULL)
			continue;

		int nHeight = rc.Height();
		if (rc.right > rcClient.right-7)
			break;
		pTab->MoveWindow(&rc);
		rc.OffsetRect(rc.Width()+3, 0);
	}

// 	rc.left = rcClient.right - 130; rc.right = rc.left + 124;
// 	m_btnFind.MoveWindow(&rc);

	rc.left = rcClient.right - 6; 
	rc.top = 0;
	rc.right = rcClient.right;
	rc.bottom = 32;
	m_btnBorderRight.MoveWindow(&rc);
}

void CI8TabBar::OnClickButton(UINT id)
{
	CWnd* pParent = AfxGetMainWnd();
	if (pParent != NULL && pParent->m_hWnd != NULL)
	{
		pParent->SendMessage(WM_TAB_CLICK_MESSAGE, id - DOCK_TAB_INDEX, 0);
	}
}
