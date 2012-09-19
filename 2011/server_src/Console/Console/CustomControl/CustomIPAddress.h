#pragma once

class CCustomIPAddress
	: public CIPAddressCtrl
{

private:
	ATL::CWindow parent_;
	WTL::CBrush backBrush_;
	ATL::CImage image_;

public:
	CCustomIPAddress();
	virtual ~CCustomIPAddress();

public:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void SetThemeParent(HWND hWnd);
	void SetImage(HBITMAP hBitmap);
	void UpdateBk();

private:
	void _Init();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnMove(int, int);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();

	afx_msg LRESULT OnPrintClient(WPARAM, LPARAM);

	
    afx_msg void OnEnable(BOOL bEnable);
};

