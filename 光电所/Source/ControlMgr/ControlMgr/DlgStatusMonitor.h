#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgStatusMonitor 对话框

class CDlgStatusMonitor : public CDialog
{
	DECLARE_DYNAMIC(CDlgStatusMonitor)

public:
	CDlgStatusMonitor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStatusMonitor();

// 对话框数据
	enum { IDD = IDD_DIALOG_STATUS_MONITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	CString wndLabelUDPProxyIP_;
	CString wndLabelUDPProxyPort_;
	CString wndLabelUDPProxyStatus_;

	CString wndLabelSendSvrIP_;
	CString wndLabelSendSvrPort_;
	CString wndLabelSendSvrStatus_;

	CString wndLabelRecvSvrIP_;
	CString wndLabelRecvSvrPort_;
	CString wndLabelRecvSvrStatus_;

	CListCtrl wndListClients_;

	CButton wndBtnProxyStart_;
	CButton wndBtnProxySuspend_;
	CButton wndBtnSendSvrStart_;
	CButton wndBtnSendSvrSuspend_;
	CButton wndBtnRecvSvrStart_;
	CButton wndBtnRecvSvrSuspend_;


	
public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedButtonUdpproxyStart();
	afx_msg void OnBnClickedButtonUdpproxyStop();
	afx_msg void OnBnClickedButtonSendsvrStart();
	afx_msg void OnBnClickedButtonSendsvrStop();
	afx_msg void OnBnClickedButtonRecvsvrStart();
	afx_msg void OnBnClickedButtonRecvsvrStop();
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnRefresh(WPARAM, LPARAM);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg void OnPaint();
	
	
};
