#pragma once
#include "afxdtctl.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <set>

#include "../ManagerInstance.h"
#include "BootTaskParam.h"
#include "WindowManager.h"

#include "DlgStartupParamDesktop.h"
#include "DlgStartupParamDel.h"
#include "DlgStartupParamIocn.h"
#include "DlgStartupParamRunSvr.h"
#include "DlgStartupParamRunClient.h"
#include "DlgStartupParamDirectUpdate.h"

#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinGroup.h"

//#include "../CustomControl/CustomRadio.h"

using i8desk::data_helper::BootTaskTraits;

// CDlgStartupInfo 对话框

class CDlgStartupInfo : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgStartupInfo)

public:
	CDlgStartupInfo(bool isAdd, const BootTaskTraits::ElementType *val, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgStartupInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_STARTUP_INFO };


private:

	bool isAdd_;
	BootTaskTraits::ValueType curBootTask_;

	typedef std::set<stdex::tString> AIDs;
	AIDs aids_;

	CString taskName_;


	i8desk::ui::SkinLabel wndTipArea_;
	i8desk::ui::SkinLabel wndTipRunDate_;
	i8desk::ui::SkinLabel wndTipStartTime_;
	i8desk::ui::SkinLabel wndTipEndTime_;
	i8desk::ui::SkinLabel wndTipRunType_;
	i8desk::ui::SkinLabel wndTipDelay_;
	i8desk::ui::SkinLabel wndTipDelaySecond_;

	i8desk::ui::SkinGroup wndGroupSet_;
	i8desk::ui::SkinLabel wndLableName_;
	i8desk::ui::SkinLabel wndLableTime_;
	i8desk::ui::SkinLabel wndLableType_;
	
	i8desk::ui::SkinRadioBox wndRadioRunAtBoot_;
	i8desk::ui::SkinRadioBox wndRadioRunAtMenu_;
	i8desk::ui::SkinRadioBox wndRadioDirectRun_;
	i8desk::ui::SkinRadioBox wndRadioProtoRun_;

	CTreeCtrl wndTreeAreas_;
	CTreeCtrl wndTreeTimeRange_;

	CDateTimeCtrl wndTime_;
	CDateTimeCtrl wndTimeStart_;
	CDateTimeCtrl wndTimeEnd_;

	i8desk::ui::SkinEdit wndEditDelayTime_;
	i8desk::ui::SkinEdit wndEdittaskName_;

	i8desk::ui::SkinCheckBox wndCheckboxStatus_;
	i8desk::ui::SkinComboBox wndComboType_;
	
	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;

	CDlgStartupParamDesktop			dlgDesktop_;
	CDlgStartupParamDel				dlgDel_;
	CDlgStartupParamIocn			dlgIcon_;
	CDlgStartupParamRunSvr			dlgRunSvr_;
	CDlgStartupParamRunClient		dlgRunCli_;
	CDlgStartupParamDirectUpdate	dlgUpdate_;

	i8desk::ui::ControlMgr wndMgr_;
	typedef std::map<UINT, i8desk::ParamInterface *> Params;
	Params params_;
	ATL::CImage outboxLine_;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedProto();
	afx_msg void OnBnClickedDirect();
	afx_msg void OnCbnSelchangeComboStartupInfoType();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	
};
