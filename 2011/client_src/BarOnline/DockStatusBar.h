#pragma once

static int WM_STATUS_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_status_click_message"));

class CDockStatusBar : public CStatic
{
	DECLARE_DYNAMIC(CDockStatusBar)
public:
	CDockStatusBar();
	~CDockStatusBar();
public:
	void SetQuickRunGroup(int nIndex/*0--4*/, HICON hIcon, CString strCaption);
	void AddScroolText(LPCTSTR szScrollText, LPCTSTR url);
	void RemoveScroolText(LPCTSTR szScroolText);
	void RemoveAllScroolText();
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	DECLARE_MESSAGE_MAP()
private:
	CBitmap* m_pbkImage;
	CBitmap* m_pQuickImage;
	I8SkinCtrl_ns::CI8OldButton m_BtnGroup[15];
	I8SkinCtrl_ns::CI8HtmlView   m_WebPage;
};
