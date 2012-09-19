#pragma once
#include "afxwin.h"

class CCliSysInfo : public CDialog
{
	DECLARE_DYNAMIC(CCliSysInfo)

public:
	CCliSysInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCliSysInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIINFO };
	enum Info_Type { CLI_SYSINFO, CLI_PROCINFO, CLI_SVIINFO};
	Info_Type m_InfoType;
	DWORD	  m_dwIp;
protected:
	CListCtrl m_lstCtrl;
	CEdit m_edtSysInfo;
	void  RefreshInfo();
	void  KillProcSvr();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillRefresh();
	afx_msg void OnKillProcsvr();
};
