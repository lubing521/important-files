#pragma once

#include "resource.h"

// CSCBaseConfigDlg 对话框

class CSCBaseConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CSCBaseConfigDlg)

public:
	CSCBaseConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSCBaseConfigDlg();

// 对话框数据
	enum { IDD = IDD_SC_BASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
