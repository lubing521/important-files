
// ClientDlg.h : 头文件
//

#pragma once


#include "../../include/UI/Skin/SkinButton.h"
#include "../../include/UI/Frame/NonFrameDlg.h"
#include "../../include/UI/Skin/CustomControl/CustomHtml.h"


// CClientDlg 对话框
class CClientDlg 
	: public ui::frame::CNonFrameDlg
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();


private:
	ui::skin::Button wndBtnMain_;
	ui::skin::Button wndBtnNei_;
	ui::skin::Button wndBtnVideo_;
	ui::skin::Button wndBtnPush_;
	ui::skin::Button wndBtnGolden_;
	ui::skin::Button wndBtnDiamond_;
	ui::skin::Button wndBtnInstitute_;

	ui::skin::Button wndBtnMin_;
	ui::skin::Button wndBtnClose_;

	ATL::CImage bkImg_;
	CHtmlCtrl htmlCtrl_;

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg HCURSOR OnQueryDragIcon();
	
	
	afx_msg void OnBnClickedButtonMain();
	afx_msg void OnBnClickedButtonNei();
	afx_msg void OnBnClickedButtonVideo();
	afx_msg void OnBnClickedButtonPush();
	afx_msg void OnBnClickedButtonGolden();
	afx_msg void OnBnClickedButtonHigh();
	afx_msg void OnBnClickedButtonJigou();
	afx_msg void OnBnClickedButtonMainMin();
	afx_msg void OnBnClickedButtonMainClose();

	afx_msg void OnModifyPsw();
};
