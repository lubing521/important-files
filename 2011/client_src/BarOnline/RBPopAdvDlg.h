#pragma once


// CRBPopAdvDlg 对话框

class CRBPopAdvDlg : public CDialog
{
	DECLARE_DYNAMIC(CRBPopAdvDlg)

public:
	CRBPopAdvDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRBPopAdvDlg();

// 对话框数据
	enum { IDD = IDD_DLG_RBPOPADV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedClose();
private:
	I8SkinCtrl_ns::CI8HtmlView m_WebPage;
	I8SkinCtrl_ns::CI8OldButton m_btnClose;
};
