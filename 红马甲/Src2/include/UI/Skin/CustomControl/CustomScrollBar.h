#pragma once


class CCustomVScrollbar 
	: public CStatic
{

private:
	CListCtrl *parent_;
	
	bool bMouseDownArrowUp_;
	bool bMouseDownArrowDown_;
	bool bDragging_;
	bool bMouseDown_;

	int nThumbTop_;
	double dbThumbInterval_;
	
	//  Scroll Top,Bottom   Bar Top BK Bottom Thumb
	ATL::CImage img_[6];
	
public:
	CCustomVScrollbar();

public:
	void SetImages(HBITMAP img[6]);
	void UpdateThumbPosition();
	void ScrollDown();
	void ScrollUp();
	void PageUp();
	void PageDown();

	BOOL Create(CListCtrl *list, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd);

private:
	void _Draw(CDC &dc);
	void _LimitThumbPosition();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
};