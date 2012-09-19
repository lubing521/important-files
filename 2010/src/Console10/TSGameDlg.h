#pragma once
#include "afxcmn.h"


// CTSGameDlg 对话框

class CTSGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CTSGameDlg)

public:
	CTSGameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTSGameDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TSGAME };

protected:
	i8desk::GameInfoMap m_GameInfos;
	std::set<DWORD> m_IDArray;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstGame;

	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
