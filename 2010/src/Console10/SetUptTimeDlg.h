#pragma once
#include "afxdtctl.h"


// CSetUptTimeDlg 对话框

class CSetUptTimeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUptTimeDlg)

public:
	CSetUptTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetUptTimeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETTIME };

	CTime tmStart;
	CTime tmEnd;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_dtStart;
	CDateTimeCtrl m_dtEnd;
	afx_msg void OnBnClickedOk();
};
