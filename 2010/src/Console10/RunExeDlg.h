#pragma once


// CRunExeDlg 对话框

class CRunExeDlg : public CDialog
{
	DECLARE_DYNAMIC(CRunExeDlg)

public:
	CRunExeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRunExeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_RUNEXE };
	CString m_strExeFile;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
