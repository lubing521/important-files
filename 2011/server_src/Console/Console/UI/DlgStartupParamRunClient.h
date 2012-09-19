#pragma once

#include "BootTaskParam.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomEdit.h"
#include "../CustomControl/CustomLabel.h"
#include "../CustomControl/CustomCheckBox.h"
#include "../../../../include/ui/wtl/atldlgs.h"

#include "../makeidxfile.h"
#include "../../../../include/MultiThread/Lock.hpp"

#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"

#include "Skin/SkinImage.h"
// CDlgStartupRunClient 对话框

class CDlgStartupParamRunClient 
	: public CDialog
	, public i8desk::ParamInterface
{
	DECLARE_DYNAMIC(CDlgStartupParamRunClient)

public:
	CDlgStartupParamRunClient(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupParamRunClient();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_RUN_CLIENT };

private:
	stdex::tString param_;

	CString svrPath_;
	CString cliPath_;
	CString cliFile_;

	i8desk::ui::SkinButton		wndBtnSrcPath_;
	i8desk::ui::SkinButton		wndBtnDstPath_;
	i8desk::ui::SkinButton		wndBtnDstFile_;
	i8desk::ui::SkinCheckBox		wndCheckIsRun_;

	i8desk::ui::SkinEdit			wndEditFile_;
	i8desk::ui::SkinEdit			wndEditSvrPath_;
	i8desk::ui::SkinEdit			wndEditDstPath_;
	i8desk::ui::SkinEdit			wndEditRunParam_;

	i8desk::ui::SkinLabel		wndLabelRunParam_;
	i8desk::ui::SkinLabel		wndLabelFile_;
	i8desk::ui::SkinLabel		wndLabelParam_;


	typedef async::thread::AutoEvent Event;
	Event exit_;
	i8desk::CMakeIdx makeIdx_;
	bool isMakeIdx_;

public:
	virtual bool GetParam(stdex::tString &);
	virtual void SetParam(const stdex::tString &);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	
	afx_msg void OnBnClickedButtonStartupSrcPath();
	afx_msg void OnBnClickedButtonStartupDstPath();
	afx_msg void OnBnClickedButtonStartupDstFile();
	
	afx_msg void OnBnClickedCheckStartupInfoIsrun();

	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
};
