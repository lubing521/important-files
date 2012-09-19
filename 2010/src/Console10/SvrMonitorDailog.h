#pragma once
#include "afxcmn.h"

#include <memory>
#include <deque>

// CSvrMonitorDailog 对话框

class CSvrMonitorDailog : public CDialog
{
	DECLARE_DYNAMIC(CSvrMonitorDailog)

public:
	CSvrMonitorDailog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSvrMonitorDailog();

	void AddLog(i8desk::PluginLogRecord* log);

// 对话框数据
	enum { IDD = IDD_SVR_MONITOR };

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnLvnCustomDrawSvrLog(NMHDR *pNmHdr, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoSvrLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

private:

	typedef std::tr1::shared_ptr<i8desk::PluginLogRecord> LogRecord_Ptr;
	typedef std::deque<LogRecord_Ptr> LogList;

	size_t m_nMaxLogNum;
	LogList m_logs;
	CListCtrl m_LogView;

public:
	afx_msg void OnClose();
};
