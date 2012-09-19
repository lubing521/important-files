#pragma once
#include "afxcmn.h"


// CViewGameDlg 对话框

class CViewGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CViewGameDlg)

public:
	CViewGameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CViewGameDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIEWGAME };
	DWORD gid;
	bool m_bViewCmpAllGame;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstCtrl;
};
