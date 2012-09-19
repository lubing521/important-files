#pragma once

#include "resource.h"

// CWZVDiskMgrBoard 对话框

class CWZVDiskMgrBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZVDiskMgrBoard)

public:
	CWZVDiskMgrBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZVDiskMgrBoard();

// 对话框数据
	enum { IDD = IDD_WZ_VDISKMGR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton();
private:
	CFont m_font;
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
