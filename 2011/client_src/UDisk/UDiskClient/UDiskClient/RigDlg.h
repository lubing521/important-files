#pragma once


// CRigDlg 对话框

class CRigDlg : public CDialog
{
	DECLARE_DYNAMIC(CRigDlg)

public:
	CRigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRigDlg();

	CString m_strName,m_strPassword;
// 对话框数据
	enum { IDD = IDD_RigDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
