#pragma once

#include <array>


class CCustomCheckBox
	: public CButton
{
	enum State { Normal, Hover, Down };

private:
	std::tr1::array<ATL::CImage, 3> bmps_;
	WTL::CBrush brBack_;
	WTL::CFontHandle font_;
	ATL::CWindow parent_;
	State state_;

public:
	CCustomCheckBox();
	virtual ~CCustomCheckBox();

public:
	virtual void PreSubclassWindow();

public:
	void SetImages(HBITMAP normal, HBITMAP hover, HBITMAP down);
	void SetThemeParent(HWND hWnd);

private:
	void _Init();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnMove(int, int);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};