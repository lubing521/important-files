#pragma once

#include "resource.h"
// CWZServerBoard 对话框

class CWZServerBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZServerBoard)

public:
	CWZServerBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZServerBoard();

// 对话框数据
	enum { IDD = IDD_WZ_SERVER };

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
