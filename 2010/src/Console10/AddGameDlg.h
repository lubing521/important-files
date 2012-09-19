#pragma once
#include "afxcmn.h"


// CAddGameDlg 对话框

class CAddGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddGameDlg)

public:
	CAddGameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddGameDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADDGAME };

	i8desk::GameInfoMap m_GameInfos;
	//i8desk::GameInfoMap m_GameInfos;
	std::set<DWORD> m_IDs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstGame;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedUnselect();
};
