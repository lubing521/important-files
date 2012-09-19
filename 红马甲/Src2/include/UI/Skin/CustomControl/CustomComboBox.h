#ifndef __CUSTON_COMBOBOX_HPP
#define __CUSTON_COMBOBOX_HPP

#include <array>
#include "CustomEdit.h"

class CCustomComboBox
	: public CComboBox
{

public:
	CCustomComboBox(const CSize &szArrow);
	virtual ~CCustomComboBox();

private:
	CCustomEdit edit_;
	std::tr1::array<ATL::CImage, 3> bmpArrow_;	// 下拉箭头背景
	ATL::CImage bk_;							// 背景图
    HBITMAP editBmp_;							// Edit 背景
	CSize szArrow_;								// 下拉箭头大小
    CFont font_;

	bool isComboHover_;							// Combo Hover
	bool isArrowHover_;							// Arrow Hover
	bool isArrowDown_;							// Arrow Down

public:
	void SetBk(HBITMAP bk, HBITMAP editBmp);
	void SetArrow(HBITMAP normalArrow, HBITMAP hoverArrow, HBITMAP downArrow);

protected:
    virtual void PreSubclassWindow();
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    void _Init();


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSetFocus(CWnd* pNewWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg BOOL OnCbnSelchange();
};




#endif