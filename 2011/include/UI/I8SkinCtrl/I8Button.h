#pragma once

#include "I8SkinBitmap.h"
#include "I8BGCache.h"
#include "../../Extend STL/UnicodeStl.h"

class CI8Button 
	: public CButton
{
    DECLARE_DYNAMIC(CI8Button)
    DECLARE_MESSAGE_MAP()
private:
    enum ButtonState_en
    {
        E_BS_NORMAL = 0,
        E_BS_HOVER,
        E_BS_PRESS,
        E_BS_DISABLE,
        E_BS_MAX
    };
private:
    I8SkinCtrl_ns::CI8BGCache m_clBGCache;
    I8SkinCtrl_ns::CI8SkinBitmap m_aclBGImage[E_BS_MAX];
	BOOL m_bHover;
    BOOL m_bCreate;
	UINT m_uiTextColor[E_BS_MAX];
	CFont m_clFont;
	UINT m_uiTextStyle;
	CBrush m_clParentBGBrush;
public:
    CI8Button();
    virtual ~CI8Button();
    void Load(const stdex::tString& sName);
    CSize GetImageSize() const;
protected:
    virtual void Init();
    ButtonState_en GetState() const;
private:
    CI8Button(const CI8Button&);
    CI8Button& operator=(const CI8Button&);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnMove(int, int);
	afx_msg void OnMouseMove(UINT,CPoint);
    afx_msg void OnMouseLeave();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    afx_msg LRESULT OnSetState(IN WPARAM wParam, IN LPARAM lParam);
};
