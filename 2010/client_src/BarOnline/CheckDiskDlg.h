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
	CButtonEx m_btnTitle;
	CButtonEx m_btnDrvList;
	CButtonEx m_btnCheckDrvList;

	CListBox  m_lstLeft;
	CListBox  m_lstRigth;

	CButtonEx m_btnAdd;
	CButtonEx m_btnAddAll;
	CButtonEx m_btnDel;
	CButtonEx m_btnDelAll;
	CButtonEx m_btnCheck;
	CButtonEx m_btnCancel;
};
