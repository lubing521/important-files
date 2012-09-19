
// BarOnline11Dlg.h : 头文件
//

#pragma once

#include "../../include/ui/Frame/NonFrameDlg.h"


// CBarOnlineDlg 对话框
class CBarOnlineDlg 
	: public CNonFrameDlg
{
	typedef CNonFrameDlg ParentWnd;
// 构造
public:
	CBarOnlineDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BARONLINE11_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
};
