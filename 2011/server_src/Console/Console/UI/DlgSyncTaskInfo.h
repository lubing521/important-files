#pragma once

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinGroup.h"

#include <vector>

// CDlgSyncTaskInfo 对话框

class CDlgSyncTaskInfo : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgSyncTaskInfo)

public:
	CDlgSyncTaskInfo(bool isAdd, const i8desk::data_helper::SyncTaskTraits::ElementType *parentsyncTask,
			i8desk::data_helper::GameTraits::VectorType &games,
			CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSyncTaskInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_SYNCTASK_INFO };

private:
	bool isAdd_;
	i8desk::data_helper::SyncTaskTraits::ValueType cursyncTask_;
	const i8desk::data_helper::SyncTaskTraits::ElementType *parentsyncTask_;
	i8desk::data_helper::GameTraits::VectorType &games_;


	CString maxSpeed_;
	CString syncTaskName_;
	
	i8desk::ui::SkinLabel wndTipSyncTaskName_;
	i8desk::ui::SkinLabel wndTipFrom_;
	i8desk::ui::SkinLabel wndTipTo_;
	i8desk::ui::SkinLabel wndTipSvrName_;
	i8desk::ui::SkinLabel wndTipSvrIP_;
	i8desk::ui::SkinLabel wndTipSvrDir_;
	i8desk::ui::SkinLabel wndTipCliName_;
	i8desk::ui::SkinLabel wndTipCliIP_;
	i8desk::ui::SkinLabel wndTipCliDir_;
	i8desk::ui::SkinLabel wndTipSvrDirParam_;

	i8desk::ui::SkinLabel wndTipSpeed_;
	i8desk::ui::SkinLabel wndTipSpeedKB_;
	i8desk::ui::SkinGroup wndGroupSyncTaskSet_;
	i8desk::ui::SkinGroup wndGroupSyncTaskInfo_;

	i8desk::ui::SkinComboBox wndComboSvrName_;
	i8desk::ui::SkinComboBox wndComboSvrIP_;
	i8desk::ui::SkinComboBox wndComboCliName_;
	i8desk::ui::SkinComboBox wndComboCliIP_;
	i8desk::ui::SkinComboBox wndComboCliDir_;

	i8desk::ui::SkinEdit  wndEditsyncTaskName_;
	i8desk::ui::SkinEdit  wndEdimaxSpeed_;

	i8desk::ui::SkinButton wndbtnOk_;
	i8desk::ui::SkinButton wndbtnCancel_;

	ATL::CImage outboxLine_;


public:
	i8desk::data_helper::SyncTaskTraits::ValueType GetCurSycTask( ){ return cursyncTask_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	
private:
	bool _CheckData();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnCbnSelchangeComboSynctaskSvrName();
	afx_msg void OnCbnSelchangeComboSynctaskCliName();
	afx_msg void OnBnClickedCheckSyncTaskInfoSelectAll();
};
