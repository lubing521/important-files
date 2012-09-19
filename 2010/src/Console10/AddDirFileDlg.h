#pragma once
#include "afxwin.h"


// CAddDirFileDlg 对话框

class CAddDirFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddDirFileDlg)

public:
	CAddDirFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddDirFileDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADDDIRFILE };

	BOOL m_bIsDir;
	CString m_strDirFile;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_cboDirFile;
};
