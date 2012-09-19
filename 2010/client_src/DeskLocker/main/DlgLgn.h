#pragma once

#include "../../BarOnline/UI.h"

class CDlgLgn : public CDialog
{
	DECLARE_DYNAMIC(CDlgLgn)

public:
	CDlgLgn(CWnd* pParent = NULL);
	virtual ~CDlgLgn();
	
	enum { IDD = IDD_DLG_LOGIN };
public:
	CString m_strPwd;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	CEdit	  m_edtPwd;
	CEdit	  m_edtPwd2;
	CButtonEx m_btnOk;
	CButtonEx m_btnCancel;
	CButtonEx m_btnClose;
};
