
// ServerManagerDlg.h : 头文件
//

#pragma once

#include "Impl/Manager.hpp"
#include "../../../include/ui/wtl/atltrayicon.h"
#include "../../../include/ui/wtl/atluser.h"
#include "afxcmn.h"

// CServerManagerDlg 对话框
class CServerManagerDlg 
	: public CDialog
	, public CTrayIconImpl<CServerManagerDlg>
{
	typedef CTrayIconImpl<CServerManagerDlg> TrayIconImpl;
// 构造
public:
	CServerManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVERMANAGER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

// 实现
private:
	HICON m_hIcon;

	WTL::CIcon statusIcon_[2];

	mgr::ServiceMgr svrMgr_;
	CListCtrl wndSvrs_;

	// 生成的消息映射函数
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	afx_msg void OnBnClickedButtonStartAll();
	afx_msg void OnBnClickedButtonStopAll();

	afx_msg LRESULT OnTrayIconMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTrayiconStartAll();
	afx_msg void OnTrayiconStopAll();
	afx_msg void OnTrayiconConfig();
	afx_msg void OnTrayiconOpen();
	afx_msg void OnTrayiconExit();

	afx_msg LRESULT OnStartSvr(WPARAM, LPARAM);
	afx_msg LRESULT OnStopSvr(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateList(WPARAM, LPARAM);
	
};
