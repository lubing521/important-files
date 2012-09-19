#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinListCtrl.h"

// WZDlgCmpBootTask 对话框

class WZDlgCmpBootTask 
	: public CDialog
	, public i8desk::wz::WZInterface

{
	DECLARE_DYNAMIC(WZDlgCmpBootTask)

public:
	WZDlgCmpBootTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgCmpBootTask();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_CMPBOOTTASK };

private:
	i8desk::ui::SkinListCtrl	wndListBootTasks_;
	i8desk::ui::SkinButton		wndbtnAdd_;
	i8desk::ui::SkinButton		wndbtnDel_;
	i8desk::ui::SkinButton		wndbtnModify_;

	i8desk::data_helper::BootTaskTraits::VectorType curBootTasks_;

	ATL::CImage workOutLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
//	bool _CheckDatas(i8desk::data_helper::BootTaskTraits::ValueType &val);
	void _ShowView();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonBootTaskAdd();
	afx_msg void OnBnClickedButtonBootTaskDel();
	afx_msg void OnBnClickedButtonBootTaskModify();
	afx_msg void OnLvnGetdispinfoListBootTaskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRDblclkListBootTaskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
