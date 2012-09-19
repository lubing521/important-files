#pragma once


#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"

// CDlgVDiskInfo 对话框

class CDlgVDiskInfo 
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgVDiskInfo)

public:
	CDlgVDiskInfo(bool isAdd, i8desk::data_helper::VDiskTraits::ElementType *val, const i8desk::data_helper::ServerTraits::ElementType *parentSvr, CWnd* pParent);   // 标准构造函数
	virtual ~CDlgVDiskInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_SERVER_VDISK_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool isAdd_;
	CWnd *parentWnd_;
	i8desk::data_helper::VDiskTraits::ValueType curVDisk_;
	const i8desk::data_helper::ServerTraits::ElementType *parentSvr_;

	i8desk::ui::SkinLabel wndTipSvrIP_;
	i8desk::ui::SkinLabel wndTipSvrDrv_;
	i8desk::ui::SkinLabel wndTipCliDrv_;
	i8desk::ui::SkinLabel wndTipRefresh_;
	i8desk::ui::SkinLabel wndTipSize_;
	i8desk::ui::SkinLabel wndTipSsdDrv_;
	i8desk::ui::SkinLabel wndTipMB_;

	i8desk::ui::SkinComboBox wndComboSvrIP_;
	i8desk::ui::SkinComboBox wndComboSvrDrv_;
	i8desk::ui::SkinComboBox wndComboCliDrv_;
	i8desk::ui::SkinComboBox wndComboRefresh_;
	i8desk::ui::SkinComboBox wndComboSsdDrv_;

	i8desk::ui::SkinEdit	 wndEditSize_;

	i8desk::ui::SkinButton wndBtnOK_;
	i8desk::ui::SkinButton wndBtnCancel_;

	CString textSize_;

private:
	bool _CheckDatas();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickOK();
	afx_msg void OnBnClickCancel();
	
};
