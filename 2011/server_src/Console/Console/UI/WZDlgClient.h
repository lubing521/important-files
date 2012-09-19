#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"


// WZDlgClient 对话框

class WZDlgClient 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgClient)

public:
	WZDlgClient(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgClient();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_CLIENT };

private:
	i8desk::ui::SkinComboBox	wndComboSkin_;
	i8desk::ui::SkinComboBox	wndComboDisplayMode_;
	i8desk::ui::SkinComboBox	wndComboOperateMode_;
	i8desk::ui::SkinComboBox	wndComboDrv_;

	i8desk::ui::SkinRadioBox	wndRadioAgree_;
	i8desk::ui::SkinRadioBox	wndRadioCustom_;
	i8desk::ui::SkinRadioBox	wndRadioNetBarPic_;
	i8desk::ui::SkinRadioBox	wndRadioGongGao_;

	i8desk::ui::SkinButton		wndBtnFont_;
	i8desk::ui::SkinButton		wndBtnGonggaoPic_;
	i8desk::ui::SkinButton		wndBtnNetBar_;

	i8desk::ui::SkinCheckBox	wndAutoStartMenu_;
	i8desk::ui::SkinCheckBox	wndCheckCliDir_;
	i8desk::ui::SkinCheckBox	wndCheckOnlyRead_;

	i8desk::ui::SkinEdit		wndEditTemperatrue_;
	i8desk::ui::SkinEdit		wndEditGonggaoPic_;
	i8desk::ui::SkinEdit		wndEditNetBar_;
	i8desk::ui::SkinEdit		wndEditIE_;

	CEdit		wndEditGonggao_;


	i8desk::ui::SkinLabel		wndLabelCliMenu_;
	i8desk::ui::SkinLabel		wndLabelCliMenuSkin_;
	i8desk::ui::SkinLabel		wndLabelCliMenuMode_;
	i8desk::ui::SkinLabel		wndLabelRunGame_;
	i8desk::ui::SkinLabel		wndLabelLocalDrv_;

	i8desk::ui::SkinLabel		wndLabelGonggaoPic_;
	i8desk::ui::SkinLabel		wndLabelNetBar_;
	i8desk::ui::SkinLabel		wndLabelGonggao_;
	i8desk::ui::SkinLabel		wndLabelTemperatrue_;
	i8desk::ui::SkinLabel		wndLabelIE_;

	CString    textComboDrv_;
	CString    textGonggaoPic_;
	CString    textNetBar_;
	CString    textGonggao_;
	CString    textTemperatrue_;
	CString    textIE_;

	CString		strFontString_;
	LOGFONT		logfont_;
	CFont		font_;
	COLORREF	fontColor_;

	int gonggaoRuntype ;

	ATL::CImage workOutLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	void _SetComboBarSkin();
	void _ReadDatas();
	stdex::tString _GetGonggao(const stdex::tString &text);
	void _SetGonggao( const stdex::tString &text);

public:
	virtual void OnComplate();
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckCustom();
	afx_msg void OnBnClickedCheckAgree();
	afx_msg void OnBnClickedGongGao();
	afx_msg void OnBnClickedNetBarPic();
	afx_msg void OnBnClickedNetbarJpg();
	afx_msg void OnBnClickedGonggaoPic();
	afx_msg void OnBnClickedButtonClientsetFont();
	afx_msg void OnCbnSelchangeComboMenuSkin();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
