#pragma once

#include "../CustomControl/CustomButton.h"


class CNonFrameChildDlg 
	: public CDialogEx
{
	DECLARE_DYNAMIC(CNonFrameChildDlg)

public:
	CNonFrameChildDlg(UINT uID, CWnd* pParent = NULL);
	virtual ~CNonFrameChildDlg();

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	ATL::CImage title_;
	ATL::CImage border_;
	CCustomButton wndBtnClose_;

	CString titleText_;
	WTL::CFontHandle font_;
	CBrush brush_;

public:
	void SetTitle(const CString &text);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
};