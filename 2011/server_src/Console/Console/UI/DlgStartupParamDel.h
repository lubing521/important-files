#pragma once

#include "BootTaskParam.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"

// CDlgStartupParamDel 对话框

class CDlgStartupParamDel 
	: public CDialog
	, public i8desk::ParamInterface
{
	DECLARE_DYNAMIC(CDlgStartupParamDel)

public:
	CDlgStartupParamDel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupParamDel();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_DEL };

private:
	stdex::tString param_;
	CString path_;
	i8desk::ui::SkinCheckBox  wndChkPenet_;
	i8desk::ui::SkinButton	wndBtnDelBrow_;
	i8desk::ui::SkinEdit	wndEditpath_;
	i8desk::ui::SkinLabel	wndLabelDir_;
	i8desk::ui::SkinLabel	wndLabelParam_;
public:
	virtual bool GetParam(stdex::tString &);
	virtual void SetParam(const stdex::tString &);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonStartupParamDelDirBrow();
};
