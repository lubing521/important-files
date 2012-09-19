#pragma once

static int WM_BROWSE_CLICK_MESSAGE	= RegisterWindowMessage(TEXT("_i8desk_browse_click_message"));

class CDockBrowseBar : public CStatic
{
	DECLARE_DYNAMIC(CDockBrowseBar)
public:
	CDockBrowseBar();
	~CDockBrowseBar();
public:
	CString GetText();
	void SetText(CString &str);
	void SetListText(std::vector<CString>& lst);
	void SetNoScroll();
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
	I8SkinCtrl_ns::CI8ComboBox m_CtrlUrl;
	CBitmap* m_pbkImage;
	I8SkinCtrl_ns::CI8OldButton m_BtnGroup[13];
};
