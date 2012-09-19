#pragma once

#include "I8UIFrame/NonFrameChildDlg.h"
#include "CustomControl/CustomButton.h"
#include "CustomControl/CustomLabel.h"

// CDlgInfoMsg 对话框

class CDlgInfoMsg 
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgInfoMsg)

public:
	CDlgInfoMsg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInfoMsg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INFO_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
private:
	bool isShowCancel_;						// 是否显示Cancel按钮标志

	CString infoMsg_;
	stdex::tString url_;					// 点击确定后定位到URL

	CCustomButton wndBtnOK_;
	CCustomButton wndBtnCancel_;
	CCustomLabel wndText_;

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
};
