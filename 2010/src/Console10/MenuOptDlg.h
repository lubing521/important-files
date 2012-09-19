#pragma once
#include "afxwin.h"


// CMenuOptDlg 对话框

class CMenuOptDlg : public CDialog
{
	DECLARE_DYNAMIC(CMenuOptDlg)

private:
	CButton m_btnSetMenuPwd;
	i8desk::BarSkinList m_BarSkins;
	BOOL m_bModify;

public:
	CMenuOptDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMenuOptDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_MENUOPT };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedWinmode();
	afx_msg void OnBnClickedCheckSetpwd();
	afx_msg void OnCbnSelchangeMenuMode();
};
