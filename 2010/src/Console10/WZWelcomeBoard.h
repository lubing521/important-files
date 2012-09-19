#pragma once

#include "resource.h"

// CWZWelcomeBoard 对话框

class CWZWelcomeBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZWelcomeBoard)

public:
	CWZWelcomeBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZWelcomeBoard();

// 对话框数据
	enum { IDD = IDD_WZ_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
