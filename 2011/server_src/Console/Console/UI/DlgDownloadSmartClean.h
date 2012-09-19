#pragma once

#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinCheckBox.h"

// CDlgDownloadSmartClean 对话框

class CDlgDownloadSmartClean : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadSmartClean)

public:
	CDlgDownloadSmartClean(TCHAR drv = 0, i8desk::ulonglong size = 0, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadSmartClean();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNLOAD_SMART_CLEAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

private:
	TCHAR drv_;
	i8desk::ulonglong size_;

	i8desk::ui::SkinLabel wndTipDrv_;
	i8desk::ui::SkinLabel wndTipSpace_;
	i8desk::ui::SkinLabel wndTipSpaceClean_;
	i8desk::ui::SkinLabel wndTipTotalSize_;

	i8desk::ui::SkinComboBox	wndComboDrv_;
    i8desk::ui::SkinEdit		wndEditSpace_;
    i8desk::ui::SkinComboBox	wndComboKeepDay_;
	i8desk::ui::SkinCheckBox	wndCheckGreateHundred_;
	i8desk::ui::SkinCheckBox	wndCheckAll_;
	i8desk::ui::SkinButton		wndBtnClean_;
	i8desk::ui::SkinButton		wndBtnGenerate_;
	i8desk::ui::SkinListCtrl	wndListClean_;

	i8desk::data_helper::GameTraits::VectorType curGames_;
	ATL::CImage outboxLine_;

private:
	struct CleanInfo;
	bool _GetData(CleanInfo &);

	void _OnCheck(size_t row);

	void _SetTip(i8desk::data_helper::GameTraits::VectorType &selGames);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnLvnGetdispinfoListClean(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDownloadSmartCleanGenerate();
	afx_msg void OnBnClickedButtonDownloadSmartCleanClean();
	afx_msg void OnBnClickedCheckDownloadSmartCleanAll();
};
