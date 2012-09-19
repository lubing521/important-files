#pragma once
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"
// CDlgCfgUserSet 对话框

class CDlgCfgUserSet 
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgCfgUserSet)

public:
	CDlgCfgUserSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCfgUserSet();

	// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG_USERSET };

public:
	void ReadUserSetData();

private:
	CString netname_;
	CString address_;
	CString telephone_;
	CString mobile_;

	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;

	i8desk::ui::SkinLabel wndNetName_;
	i8desk::ui::SkinLabel wndAddr_;
	i8desk::ui::SkinLabel wndCity_;
	i8desk::ui::SkinLabel wndAddress_;
	i8desk::ui::SkinLabel wndTel_;
	i8desk::ui::SkinLabel wndPhone_;
	i8desk::ui::SkinLabel wndDisktype_;
	i8desk::ui::SkinLabel wndNetstatus_;
	i8desk::ui::SkinLabel wndDisklessname_;
	i8desk::ui::SkinLabel wndFeesname_;
	i8desk::ui::SkinLabel wndMonitorname_;


	i8desk::ui::SkinEdit wndnetname_;
	i8desk::ui::SkinEdit wndaddress_;
	i8desk::ui::SkinEdit wndtelephone_;
	i8desk::ui::SkinEdit wndmobile_;


	i8desk::ui::SkinComboBox wndComboProvience_;
	i8desk::ui::SkinComboBox wndComboCity_;

	i8desk::ui::SkinComboBox wndComboDiskType_;
	i8desk::ui::SkinComboBox wndComboNetwork_;

	i8desk::ui::SkinComboBox wndComboDiskProductName_;
	i8desk::ui::SkinComboBox wndComboFeeProductName_;
	i8desk::ui::SkinComboBox wndComboCulturalName_;

	ATL::CImage outBox_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonCfgUserSetOK();
	afx_msg void OnBnClickedButtonCfgUserSetCancel();

	afx_msg void OnCbnSelchangeComboSetinfoProvience();
	afx_msg void OnCbnSelchangeComboSetinfoCity();
};
