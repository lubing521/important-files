#pragma once


// CUptOptDlg 对话框

class CUptOptDlg : public CDialog
{
	DECLARE_DYNAMIC(CUptOptDlg)

public:
	CUptOptDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUptOptDlg();

// 对话框数据
	enum { IDD = IDD_DLG_UPTOPT };

	BOOL m_bIsQuick;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedClose();
	DECLARE_MESSAGE_MAP()
private:
	CButton   m_btnQuick;
	CButton   m_btnForce;
	CButtonEx m_btnClose;
	CButtonEx m_btnOK;
	CButtonEx m_btnCancel;
};
