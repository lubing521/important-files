#pragma once

#include <memory>

class CCustomProgress
	: public CStatic
{

public:
	CCustomProgress();
	virtual ~CCustomProgress();

public:
	virtual void PreSubclassWindow();

private:
	HWND parent_;
	WTL::CBrush bkBrush_;

	std::auto_ptr<ATL::CImage> normal_[3];	// left middle right
	std::auto_ptr<ATL::CImage> gray_[3];	// left middle right

	std::pair<size_t, size_t> range_;
	size_t pos_;

public:
	void SetThemeParent(HWND hWnd);

	void SetImages(HBITMAP normal[3], HBITMAP gray[3]);
	void SetRange(size_t start, size_t stop);
	void SetPos(size_t pos);
	
	size_t GetPos() const;

private:
	void _Init();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnMove(int, int);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnSetText();
	afx_msg void OnPaint();
	afx_msg LRESULT OnPrintClient(WPARAM, LPARAM);
	afx_msg LRESULT OnSetFont(WPARAM, LPARAM);
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
};