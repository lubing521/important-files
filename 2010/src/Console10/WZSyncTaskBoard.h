#pragma once

#include "resource.h"
// CWZSyncTaskBoard 对话框

class CWZSyncTaskBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZSyncTaskBoard)

public:
	CWZSyncTaskBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZSyncTaskBoard();

// 对话框数据
	enum { IDD = IDD_WZ_SYNCTASK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	CFont m_font;
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
