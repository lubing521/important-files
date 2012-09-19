#pragma once

#include "resource.h"

// CWidgetBlacklist 对话框

class CWidgetBlacklist : public CDialog
{
	DECLARE_DYNAMIC(CWidgetBlacklist)

public:
	CWidgetBlacklist(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWidgetBlacklist();

// 对话框数据
	enum { IDD = IDD_WIDGET_BLACKLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWidget;
	CString m_strDescription;
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeWidget();
};
