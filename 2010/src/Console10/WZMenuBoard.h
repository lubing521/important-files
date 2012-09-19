#pragma once

#include "resource.h"
// CWZMenuBoard 对话框

class CWZMenuBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZMenuBoard)

public:
	CWZMenuBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZMenuBoard();

// 对话框数据
	enum { IDD = IDD_WZ_MENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeSkin();
	afx_msg void OnCbnSelchangeMenuMode();
	afx_msg void OnCbnSelchangeActrunmenu();
	afx_msg void OnBnClickedDisplayNotice();
	afx_msg void OnEnChangeNotice();
	afx_msg void OnBnClickedRunmenu();
	afx_msg void OnBnClickedBrowsedir();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedPreview();
private:
	CString m_strSkin;
	int m_nMenuMode;
	int m_nActRunMenu;
	int m_nNotice;
	CString m_strNotice;
	int m_nAutoRun;
	int m_nBrowserDir;

	i8desk::BarSkinList m_BarSkins;
};
