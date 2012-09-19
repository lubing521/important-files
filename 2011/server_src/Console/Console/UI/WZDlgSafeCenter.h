#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinRadioBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"


// WZDlgSafeCenter 对话框


class WZDlgSafeCenter 
	: public CDialog
	, public i8desk::wz::WZInterface

{
	DECLARE_DYNAMIC(WZDlgSafeCenter)

public:
	WZDlgSafeCenter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgSafeCenter();

public:
// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_SAFECENTER };

private:
	i8desk::ui::SkinRadioBox	wndRadioStart_;
	i8desk::ui::SkinRadioBox	wndRadioDiskLess_;
	i8desk::ui::SkinRadioBox	wndRadioDisk_;
	i8desk::ui::SkinRadioBox	wndRadioStop_;
	i8desk::ui::SkinButton		wndbtnSet_;

	i8desk::ui::SkinLabel		wndLabelBasic_;
	i8desk::ui::SkinLabel		wndLabelAdvance_;

	CFont	   font_;
	int		   iWhichSC_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	void SafeCenterConfig();

public:
	virtual void OnComplate();
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadioStart();
	afx_msg void OnBnClickedRadioDiskLess();
	afx_msg void OnBnClickedRadioDisk();
	afx_msg void OnBnClickedRadioStop();
	afx_msg void OnBnClickedButtonSet();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
};
