#pragma once

#include "BootTaskParam.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinButton.h"


// CDlgStartupRunSvr 对话框

class CDlgStartupParamRunSvr 
	: public CDialog
	, public i8desk::ParamInterface
{
	DECLARE_DYNAMIC(CDlgStartupParamRunSvr)

public:
	CDlgStartupParamRunSvr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupParamRunSvr();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_RUN_SVR };

private:
	stdex::tString param_;

	i8desk::ui::SkinEdit wndEditPath_;
	i8desk::ui::SkinEdit wndEditParam_;
	i8desk::ui::SkinEdit wndEditCliPath_;

	i8desk::ui::SkinCheckBox wndCheckisPenetrated_;

	i8desk::ui::SkinLabel wndLabelFile_;
	i8desk::ui::SkinLabel wndLabelParam_;
	i8desk::ui::SkinLabel wndLabelDstPath_;

	
	i8desk::ui::SkinButton wndBtnCliPath_;
	i8desk::ui::SkinButton wndBtnOpen_;

public:
	virtual bool GetParam(stdex::tString &);
	virtual void SetParam(const stdex::tString &);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonStartupParamRunsvrOpen();
	
	
	afx_msg void OnBnClickedButtonStartupParamRunsvrOpenCliPath();
};
