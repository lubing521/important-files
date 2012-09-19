#pragma once


// CSetDialog 对话框

class CSetDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetDialog)

public:
	CSetDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDialog();

	bool m_bSet;
	CString m_srvip;
// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
