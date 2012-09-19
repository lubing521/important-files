#pragma once

#include "resource.h"
// CWZAccountBoard 对话框

class CWZAccountBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZAccountBoard)

public:
	CWZAccountBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZAccountBoard();

// 对话框数据
	enum { IDD = IDD_WZ_ACCOUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUsername();
	afx_msg void OnEnChangePassword();
	afx_msg void OnEnChangePassword2();
private:
	CString m_strAccount;
	CString m_strPassword;
	// 控制台登录密码
	CString m_strPassword2;
	//菜单登录密码
	CString m_strCliPassword;
public:
	afx_msg void OnEnChangeClipassword();
};
