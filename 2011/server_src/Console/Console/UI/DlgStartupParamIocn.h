#pragma once

#include "BootTaskParam.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../ManagerInstance.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"


// CDlgStartupParamIocn 对话框

class CDlgStartupParamIocn 
	: public CDialog
	, public i8desk::ParamInterface
{
	DECLARE_DYNAMIC(CDlgStartupParamIocn)

public:
	CDlgStartupParamIocn(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupParamIocn();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_ICON };

private:
	stdex::tString param_;

	CString shortcutName_;
	CString shortcutFile_;
	CString shortcutParam_;
	CString iconPath_;
	CString selPath_;


	i8desk::ui::SkinButton		wndBtnNameBrow_;
	i8desk::ui::SkinButton		wndBtnFileBrow_;
	i8desk::ui::SkinButton		wndBtnIconBrow_;

	i8desk::ui::SkinEdit		wndEditshortcutName_;
	i8desk::ui::SkinEdit		wndEditshortcutFile_;
	i8desk::ui::SkinEdit		wndEditshortcutParam_;
	i8desk::ui::SkinEdit		wndEditiconPath_;
	i8desk::ui::SkinLabel	wndLabelshortcutName_;
	i8desk::ui::SkinLabel	wndLabelshortcutFile_;
	i8desk::ui::SkinLabel	wndLabelshortcutParam_;
	i8desk::ui::SkinLabel	wndLabeliconPath_;

public:
	virtual bool GetParam(stdex::tString &);
	virtual void SetParam(const stdex::tString &);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonStartupParamIconNameBrow();
	afx_msg void OnBnClickedButtoncEditStartupParamIconFileBrow();
	afx_msg void OnBnClickedButtonStartupParamIconIconBrow();
};
