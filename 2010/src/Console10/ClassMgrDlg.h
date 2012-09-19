#pragma once
#include "afxcmn.h"

#include "ConsoleDlg.h"

// CClassMgrDlg 对话框

class CClassMgrDlg : public CDialog
{
	DECLARE_DYNAMIC(CClassMgrDlg)

public:
	CClassMgrDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClassMgrDlg();

	// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSMGR };

	CConsoleDlg* pDlg;
	bool m_bModified;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL CClassMgrDlg::OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAdd();
	CListCtrl m_lstClass;
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
};
