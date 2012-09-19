#pragma once

static int WM_FINDER_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_finder_click_message"));

class CDockFinderBar : public CStatic
{
	DECLARE_DYNAMIC(CDockFinderBar)
public:
	CDockFinderBar();
	~CDockFinderBar();
public:
	CString GetText();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()
private:
	CFont m_font;
    I8SkinCtrl_ns::CI8Edit m_edit;
	CBitmap* m_pbkImage;
	CBitmap* m_pbkLeft;
	CBitmap* m_pbkRight;
	CBitmap* m_pbAlpha;
	CBitmap* m_pbAlphaLeft;
	CBitmap* m_pbAlphaRight;

	I8SkinCtrl_ns::CI8OldButton m_BtnGroup[29];
};
