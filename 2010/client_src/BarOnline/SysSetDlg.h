#pragma once

class CSysSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CSysSetDlg)

public:
	CSysSetDlg(CWnd* pParent = NULL);
	virtual ~CSysSetDlg();

	enum { IDD = IDD_DLG_SYSSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCheckDisk();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedAddall();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedDelall();
	afx_msg void OnBnClickedClose();
	DECLARE_MESSAGE_MAP()
private:
	CButtonEx m_btnCaption;
	CButtonEx m_btnClose;
	CButtonEx m_btnSvrIp;
	CEdit     m_edtSvrIp;
	CButtonEx m_btnAreaSet;
	CButtonEx m_btnUnProtArea;
	CButtonEx m_btnProtArea;

	CListBox  m_lstLeft;
	CListBox  m_lstRigth;

	CButtonEx m_btnAdd;
	CButtonEx m_btnAddAll;
	CButtonEx m_btnDel;
	CButtonEx m_btnDelAll;
	CButtonEx m_btnCheckDisk;
	CButtonEx m_btnOk;
	CButtonEx m_btnCancel;
	CString	  m_strProtDrv;
};
