#pragma once
#include "afxwin.h"


// CDlgInfoTip 对话框

class CDlgInfoTip 
	: public CDialog
{
	DECLARE_DYNAMIC(CDlgInfoTip)


private:
	i8desk::std_string m_strContent;		// 提示框内容
	i8desk::std_string m_strUrl;			// 点击确定后定位到URL

	BOOL m_bShowCancel;						// 是否显示Cancel按钮标志


public:
	CDlgInfoTip(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInfoTip();

// 对话框数据
	enum { IDD = IDD_DIALOG_INFO_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void _InitCtrl();			// 初始化Ctrl
	void _InitContent();		// 初始化内容			
	void _InitUrl();			// 初始化Url
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonInfoTipOk();
	afx_msg void OnBnClickedCancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

};
