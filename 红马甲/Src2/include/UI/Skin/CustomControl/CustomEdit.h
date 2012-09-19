#ifndef __CUSTOM_EDIT_HPP
#define __CUSTOM_EDIT_HPP

#include <memory>
#include <array>

class CCustomEdit 
	: public CEdit
{
public:
	CCustomEdit();
	virtual ~CCustomEdit();

public:
	void SetBkImg(HBITMAP bmp);
	void SetImages(HBITMAP left, HBITMAP mid, HBITMAP right);

private:
	CBrush brush_;
	std::auto_ptr<ATL::CImage> bkImage_;
	std::tr1::array<ATL::CImage, 3> bmps_;

protected:
	virtual void PreSubclassWindow();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnChange();

};





#endif