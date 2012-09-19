#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinCheckBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "afxwin.h"

// WZDlgSafePwd 对话框

class WZDlgSafePwd 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgSafePwd)

public:
	WZDlgSafePwd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgSafePwd();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_SAFE_PSW };

private:
	i8desk::ui::SkinEdit wndEditConsolePwd_;
	i8desk::ui::SkinEdit wndEditMenuPwd_;

	i8desk::ui::SkinLabel wndLabelAccountPsw_;
	i8desk::ui::SkinLabel wndLabelConsolePsw_;
	i8desk::ui::SkinLabel wndLabelConsolePswDef_;
	i8desk::ui::SkinLabel wndLabelMenuPswDef_;

	i8desk::ui::SkinLabel wndLabelNID_;
	i8desk::ui::SkinLabel wndNetbarID_;
	i8desk::ui::SkinLabel wndLabelDateEnd_;
	i8desk::ui::SkinLabel wndDateEnd_;
	i8desk::ui::SkinLabel wndLabelParentName_;
	i8desk::ui::SkinLabel wndParentName_;
	i8desk::ui::SkinLabel wndLabelParentID_;
	i8desk::ui::SkinLabel wndParentID_;

	i8desk::ui::SkinLabel wndLabelNetbarInfo_;
	i8desk::ui::SkinLabel wndLabelPswInfo_;

	i8desk::ui::SkinButton wndBtnModify_;
	i8desk::ui::SkinCheckBox wndMenuPwd_;

	CString consolepwd_;
	CString menupwd_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	virtual void OnComplate();
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckMenuPwd();
	afx_msg void OnEnChangeUsername();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	afx_msg void OnBnClickedButtonWzPswNetbarModify();
	
};
