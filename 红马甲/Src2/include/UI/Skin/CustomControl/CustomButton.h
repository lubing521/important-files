#pragma once
#include <memory>
#include <array>

class CCustomButton 
	: public CButton
{
	DECLARE_DYNAMIC(CCustomButton)

public:
	CCustomButton();
	virtual ~CCustomButton();

private:
	std::auto_ptr<ATL::CImage> normal_;
	std::auto_ptr<ATL::CImage> hover_;
	std::auto_ptr<ATL::CImage> press_;
	std::auto_ptr<ATL::CImage> disable_;

	bool isHover_;

	std::tr1::array<COLORREF, 4> clrText_;
	CFont		 font_;
	UINT		 textStyle_;

	ATL::CWindow parent_;
	WTL::CBrush brBack_;

public:
	void SetThememParent(HWND wnd);
	void SetTextClr(COLORREF normal, COLORREF clrHover, COLORREF clrPress, COLORREF clrDisable);
	void SetBitmap(HBITMAP normal, HBITMAP hover, HBITMAP press, HBITMAP disable = NULL);

private:
	void _Init();

public:
	// set image.
	void SetImages(HBITMAP normal, HBITMAP hover, HBITMAP press, HBITMAP disable = 0);

	// set caption text.
	void SetTextStyle(COLORREF clrBack, COLORREF clrNoral, COLORREF clrHover, COLORREF clrPress, COLORREF clrDisable);

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDis);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnMove(int, int);
	afx_msg void OnMouseMove(UINT,CPoint);
	afx_msg void OnMouseLeave();
	
};
