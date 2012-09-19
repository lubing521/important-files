#pragma once

#include "BootTaskParam.h"
#include "afxwin.h"

#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinComboBox.h"

// CDlgStartupParamDesktop 对话框

class CDlgStartupParamDesktop 
	: public CDialog
	, public i8desk::ParamInterface
{
	DECLARE_DYNAMIC(CDlgStartupParamDesktop)

public:
	CDlgStartupParamDesktop(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupParamDesktop();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_DESKTOP };

private:
	stdex::tString param_;
	i8desk::ui::SkinButton wndBtnOpen_;
	i8desk::ui::SkinComboBox wndComboFile_;
	i8desk::ui::SkinComboBox wndComboParam_;
	i8desk::ui::SkinLabel	wndLabelFile_;
	i8desk::ui::SkinLabel	wndLabelParam_;
	i8desk::ui::SkinLabel	wndLabelContent_;

public:
	virtual bool GetParam(stdex::tString &);
	virtual void SetParam(const stdex::tString &);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonStartupParamDesktopOpen();
	
};
