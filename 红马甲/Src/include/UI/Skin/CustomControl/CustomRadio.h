#ifndef __CUSTOM_RADIO_BUTTON_HPP
#define __CUSTOM_RADIO_BUTTON_HPP

#include <array>

class CCustomRadio
	: public CButton
{
private:
	bool isHover_;
	bool isChecked_;
	WTL::CBrush brBack_;
	ATL::CWindow parent_;
	WTL::CFontHandle font_;

	std::tr1::array<ATL::CImage, 3> bmps_;

public:
	CCustomRadio();
	virtual ~CCustomRadio();

public:
	virtual void PreSubclassWindow();

public:
	void SetImages(HBITMAP normal, HBITMAP hover, HBITMAP down);
	void SetThemeParent(HWND hWnd);

private:
	void _Init();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int, int);
	afx_msg void OnMouseMove(UINT,CPoint);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT, CPoint);
};






#endif