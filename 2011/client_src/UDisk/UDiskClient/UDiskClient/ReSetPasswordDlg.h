#pragma once


// CReSetPasswordDlg 对话框

class CReSetPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CReSetPasswordDlg)

public:
	CReSetPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReSetPasswordDlg();
	CString m_strNewPassword,m_strOldPassword;
// 对话框数据
	enum { IDD = IDD_ReSetDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
