#pragma once

#include <vector>
#include "I8OldButton.h"

static int WM_TAB_CLICK_MESSAGE		= RegisterWindowMessage(TEXT("_i8desk_tab_click_message"));

class CI8TabBar : public CStatic
{
	DECLARE_DYNAMIC(CI8TabBar)
public:
	CI8TabBar();
	~CI8TabBar();
public:
	I8SkinCtrl_ns::CI8OldButton* AddTab(LPCTSTR lpszCap, LPCTSTR iconImage, LPCTSTR iconSelImage, BOOL bRefresh = FALSE);
	CString  GetTabText(int index);
	DWORD    GetTabCount()	 { return m_BtnTabGroup.size(); }
	void RemoveAllTab();
	void ShowUrlNav(BOOL bShow);
	void SetActive(int idx);
	int  GetActive() { return m_nActive; }
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()

private:
	void      GetTabSkinColor();
private:
	CBitmap *m_pbkImage;
	int m_nActive;
	I8SkinCtrl_ns::CI8OldButton m_btnFind, m_btnBoderLeft, m_btnBorderRight;
    std::vector<I8SkinCtrl_ns::CI8OldButton*> m_BtnTabGroup;
	COLORREF  m_clrNormal, m_clrHover, m_clrSel;
};
