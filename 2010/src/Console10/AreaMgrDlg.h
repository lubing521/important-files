#pragma once
#include "afxcmn.h"
#include "ConsoleDlg.h"


// CAreaMgrDlg 对话框

class CAreaMgrDlg : public CDialog
{
	DECLARE_DYNAMIC(CAreaMgrDlg)

public:
	CAreaMgrDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAreaMgrDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_AREAMGR };

	bool m_bModified;
	CConsoleDlg* pDlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstArea;

	
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
};
