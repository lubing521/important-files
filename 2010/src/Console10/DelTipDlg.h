#pragma once
#include "afxwin.h"


// CDelTipDlg 对话框

class CDelTipDlg : public CDialog
{
	DECLARE_DYNAMIC(CDelTipDlg)

public:
	CDelTipDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDelTipDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DELTIP };
	BOOL m_bDelLocal;
	BOOL m_bDelClient;
	BOOL m_bDelSync;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnDelLocal;
	CButton m_btnDelClient;
	CButton m_btnDelSync;
	afx_msg void OnBnClickedOk();
};
