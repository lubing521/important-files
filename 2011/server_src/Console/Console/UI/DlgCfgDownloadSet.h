#pragma once
#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"
#include "../../../../include/Utility/utility.h"
#include "../CustomControl/CustomListCtrl.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"
#include "Skin/SkinGroup.h"



// CDlgOptDownloadSet 对话框

class CDlgCfgDownloadSet : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CDlgCfgDownloadSet)

public:
	CDlgCfgDownloadSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCfgDownloadSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG_DOWNLOAD };
	enum { WM_MSG_OK = WM_USER + 100 };

	void ReadData();

private:
	CCustomListCtrl listRunTypeCtrl_;
	CCustomListCtrl listTimeCtrl_;
	i8desk::ui::SkinEdit wndEditTempDir_;
	i8desk::ui::SkinEdit wndEditDownPath_;
	i8desk::ui::SkinCheckBox wndCheckTimeUpt_;

	BOOL bModifyClass_;
	i8desk::ui::SkinComboBox wndComboTaskNumCtrl_;
	i8desk::ui::SkinComboBox wndComboDownSpeed_;

	CString tempdir_;
	CString downpath_;

	i8desk::ui::SkinGroup wndSet_;
	i8desk::ui::SkinLabel wndMaxTaskNum_;
	i8desk::ui::SkinLabel wndMaxSpeed_;
	i8desk::ui::SkinLabel wndMaxSpeedParam_;
	i8desk::ui::SkinLabel wndTempDir_;
	i8desk::ui::SkinGroup wndDownPath_;
	i8desk::ui::SkinLabel wndAgreeDownPath_;
	i8desk::ui::SkinLabel wndRun_;
	
	i8desk::ui::SkinButton	wndBtnAdd_;
	i8desk::ui::SkinButton	wndBtnDel_;
	i8desk::ui::SkinButton	wndBtnTempDir_;
	i8desk::ui::SkinButton	wndBtnPath_;

	ATL::CImage outboxLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	BOOL UpdateClassList();
	BOOL UpdateRunTypeList();
	void DrawRunTypeList();
	void DrawClassList();


public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnBnClickedCheckTimeupt();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedButtonPath();
	afx_msg void OnBnClickedSelectMsctmpdir();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnChangeAgreeDownPath();
	afx_msg void OnLvnGetdispinfoListTime(NMHDR *pNMHDR, LRESULT *pResult);

};
