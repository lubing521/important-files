#pragma once
#include "afxwin.h"

class CGongGaoDlg : public CDialog
{
	DECLARE_DYNAMIC(CGongGaoDlg)

public:
	CGongGaoDlg(CWnd* pParent = NULL);
	virtual ~CGongGaoDlg();

	enum { IDD = IDD_DLG_GONGGAO};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	I8SkinCtrl_ns::CI8HtmlView	m_WebPage;
	I8SkinCtrl_ns::CI8OldButton m_btnWelcome;
	I8SkinCtrl_ns::CI8OldButton m_btnClose;
	I8SkinCtrl_ns::CI8OldButton m_btnContext;
	I8SkinCtrl_ns::CI8OldButton m_btnNbInfo;
};
