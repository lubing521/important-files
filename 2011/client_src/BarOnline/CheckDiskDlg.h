#pragma once

class CCheckDiskDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckDiskDlg)

public:
	CCheckDiskDlg(CWnd* pParent = NULL);
	virtual ~CCheckDiskDlg();

	enum { IDD = IDD_DLG_CHECKDISK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedAddall();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedDelall();
	DECLARE_MESSAGE_MAP()

private:
	I8SkinCtrl_ns::CI8OldButton m_btnTitle;
	I8SkinCtrl_ns::CI8OldButton m_btnDrvList;
	I8SkinCtrl_ns::CI8OldButton m_btnCheckDrvList;

	CListBox  m_lstLeft;
	CListBox  m_lstRigth;

	I8SkinCtrl_ns::CI8OldButton m_btnAdd;
	I8SkinCtrl_ns::CI8OldButton m_btnAddAll;
	I8SkinCtrl_ns::CI8OldButton m_btnDel;
	I8SkinCtrl_ns::CI8OldButton m_btnDelAll;
	I8SkinCtrl_ns::CI8OldButton m_btnCheck;
	I8SkinCtrl_ns::CI8OldButton m_btnCancel;
};
