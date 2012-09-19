#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../ManagerInstance.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinRadioBox.h"


// WZDlgAddSome 对话框

class WZDlgAddSome 
	: public CDialog
	, public i8desk::wz::WZInterface

{
	DECLARE_DYNAMIC(WZDlgAddSome)

public:
	WZDlgAddSome(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgAddSome();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_ADDSOME };

private:
	i8desk::ui::SkinLabel		wndLabelParamAdd_;
	i8desk::ui::SkinLabel		wndLabelParamNoAdd_;

	i8desk::ui::SkinRadioBox	wndRadioAdd_;
	i8desk::ui::SkinRadioBox	wndRadioNotAdd_;
	i8desk::ui::SkinButton		wndbtnAdd_;
	CFont						font_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadioAdd();
	afx_msg void OnBnClickedRadioNotAdd();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
