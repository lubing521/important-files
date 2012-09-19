#pragma once

#include "resource.h"
// CWZSafeCenterBoard 对话框

class CWZSafeCenterBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZSafeCenterBoard)

public:
	CWZSafeCenterBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZSafeCenterBoard();

// 对话框数据
	enum { IDD = IDD_WZ_SAFECENTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton();

private:
	BOOL m_bAutoStartSafeCenter;
	BOOL m_bModify;
	CFont m_font;
public:
	afx_msg void OnBnClickedCheckAutoStartSafecenter();
	afx_msg void OnBnClickedRadioI8Sc();
	afx_msg void OnBnClickedRadioKangheSc();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
