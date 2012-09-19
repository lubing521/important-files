#pragma once

static int WM_NAV_CLICK_MESSAGE		= RegisterWindowMessage(TEXT("_i8desk_nav_click_message"));

typedef struct tagNavClick
{
	DWORD dwIdx;
	DWORD dwClick;
	DWORD dwRunTime;
}tagNavClick;

class CDockNavBar : public CStatic
{
	DECLARE_DYNAMIC(CDockNavBar)
public:
	CDockNavBar();
	~CDockNavBar();
public:
	void SetActive(int idx);
	int  GetActive() { return m_nActive; }
	void SetAllUnActive();
	void SetDockMini(BOOL bIsMini);
	BOOL GetIsDockMini()	{ return m_bIsDockMini; }
	void SetNetBarText(CString& str);
	void GetAllNavClick(tagNavClick* pNavClick);
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bIsDockMini;
	int m_nActive;
	CBitmap *m_pbkImage, *m_pbkLogoImage, *m_pbkTipImage, *m_pHeadRightImage;
	I8SkinCtrl_ns::CI8OldButton m_btnNetBar;
	I8SkinCtrl_ns::CI8OldButton m_btnNav[12];
	I8SkinCtrl_ns::CI8OldButton m_btnTool[6];
	//统计6个导航栏的点击数，以及时长。（ID是1-6）
	tagNavClick  m_NavClicks[6];
};
