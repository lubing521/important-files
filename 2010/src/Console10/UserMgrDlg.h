#pragma once
#include "afxcmn.h"
#include "ConsoleDlg.h"

// CUserMgrDlg 对话框

class CUserMgrDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserMgrDlg)

public:
	CUserMgrDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserMgrDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_USERMGR };
	CConsoleDlg* pDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstUser;
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult);
};
