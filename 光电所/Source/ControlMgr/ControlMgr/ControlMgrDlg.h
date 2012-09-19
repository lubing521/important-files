
// ControlMgrDlg.h : 头文件
//

#pragma once


#include "WindowManager.h"
#include "DlgStatusMonitor.h"
#include "DlgTaskMgr.h"
#include "DlgTaskLog.h"

#include "EasySize.h"


// CControlMgrDlg 对话框
class CControlMgrDlg : public CDialog
{
// 构造
public:
	CControlMgrDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CONTROLMGR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();

// 实现
protected:
	bool isOK_;
	HICON m_hIcon;
	ui::ControlMgr ctrlMgr_;

	std::auto_ptr<CDlgStatusMonitor> statusMonitor_;
	std::auto_ptr<CDlgTaskMgr> taskMgr_;
	std::auto_ptr<CDlgTaskLog> taskLog_;

	CImage bkImgs_[3];
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnTimer(UINT_PTR);
	
	afx_msg void OnBnClickedButtonMonitorStatus();
	afx_msg void OnBnClickedButtonTaskManager();
	afx_msg void OnBnClickedButtonOperateLog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
