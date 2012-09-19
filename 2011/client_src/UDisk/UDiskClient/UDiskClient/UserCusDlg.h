#pragma once


// CUserCusDlg 对话框

class CUserCusDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserCusDlg)

public:
	CUserCusDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserCusDlg();

// 对话框数据
	enum { IDD = IDD_UserCusDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
