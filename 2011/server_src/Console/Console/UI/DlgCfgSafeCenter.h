#pragma once
#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"

// DlgCfgSafeCenter 对话框

class CDlgCfgSafeCenter : public CDialog
{
	DECLARE_DYNAMIC(CDlgCfgSafeCenter)

public:
	CDlgCfgSafeCenter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCfgSafeCenter();

public:
// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG_SAFECENTER };
	enum { WM_MSG_OK = WM_USER + 100 };


private:
	i8desk::ui::SkinRadioBox	   wndRadioStart_;
	i8desk::ui::SkinRadioBox	   wndRadioDiskLess_;
	i8desk::ui::SkinRadioBox	   wndRadioDisk_;
	i8desk::ui::SkinRadioBox	   wndRadioStop_;
	i8desk::ui::SkinButton		   wndbtnSet_;

	i8desk::ui::SkinLabel		wndLabelDiskless_;
	i8desk::ui::SkinLabel		wndLabelDisk_;

	CFont	   font_;
	int		   iWhichSC_;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	void SafeCenterConfig();

public:
	void ReadData();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnBnClickedRadioStart();
	afx_msg void OnBnClickedRadioDiskLess();
	afx_msg void OnBnClickedRadioDisk();
	afx_msg void OnBnClickedRadioStop();
	afx_msg void OnBnClickedButtonSet();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);

};
