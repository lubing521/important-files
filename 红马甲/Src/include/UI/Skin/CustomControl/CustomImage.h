#pragma once


class CCustomImage
	: public CStatic
{

private:
	ATL::CWindow parent_;
	WTL::CBrush backBrush_;
	ATL::CImage image_;
	WTL::CIconHandle icon_;

public:
	CCustomImage();
	virtual ~CCustomImage();

public:
	virtual void PreSubclassWindow();

public:
	void SetThemeParent(HWND hWnd);
	void SetImage(HBITMAP hBitmap, bool IsSizetoFit = true);
	void SetImage(HICON hIcon, bool IsSizetoFit = true);
	void SizeToFit();
	void UpdateBk();

private:
	void _Init();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnMove(int, int);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPaint();
	afx_msg LRESULT OnPrintClient(WPARAM, LPARAM);
};