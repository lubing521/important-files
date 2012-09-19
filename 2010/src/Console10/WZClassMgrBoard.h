#pragma once

#include "resource.h"
// CWZClassMgrBoard 对话框

class CWZClassMgrBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZClassMgrBoard)

public:
	CWZClassMgrBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZClassMgrBoard();

// 对话框数据
	enum { IDD = IDD_WZ_CLASSMGR };

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
