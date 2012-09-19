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
	I8SkinCtrl_ns::CI8OldButton m_btnCaption;
	I8SkinCtrl_ns::CI8OldButton m_btnClose;
	I8SkinCtrl_ns::CI8OldButton m_btnSvrIp;
	CEdit     m_edtSvrIp;
	I8SkinCtrl_ns::CI8OldButton m_btnAreaSet;
	I8SkinCtrl_ns::CI8OldButton m_btnUnProtArea;
	I8SkinCtrl_ns::CI8OldButton m_btnProtArea;

	CListBox  m_lstLeft;
	CListBox  m_lstRigth;

	I8SkinCtrl_ns::CI8OldButton m_btnAdd;
	I8SkinCtrl_ns::CI8OldButton m_btnAddAll;
	I8SkinCtrl_ns::CI8OldButton m_btnDel;
	I8SkinCtrl_ns::CI8OldButton m_btnDelAll;
	I8SkinCtrl_ns::CI8OldButton m_btnCheckDisk;
	I8SkinCtrl_ns::CI8OldButton m_btnOk;
	I8SkinCtrl_ns::CI8OldButton m_btnCancel;
	CString	  m_strProtDrv;
};
