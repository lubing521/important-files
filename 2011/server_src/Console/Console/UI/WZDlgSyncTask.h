#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinListCtrl.h"



// WZDlgSyncTask 对话框

class WZDlgSyncTask 
	: public CDialog
	, public i8desk::wz::WZInterface

{
	DECLARE_DYNAMIC(WZDlgSyncTask)

public:
	WZDlgSyncTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgSyncTask();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_SYNCTASK };
	enum BtnStatus { isadd = 0, ismodify, iscopy };


private:
	i8desk::ui::SkinButton wndbtnAdd_;
	i8desk::ui::SkinButton wndbtnDel_;
	i8desk::ui::SkinButton wndbtnModify_;

	i8desk::ui::SkinListCtrl wndListSyncTasks_;

	i8desk::data_helper::SyncTaskTraits::VectorType curSyncTasks_;
	ATL::CImage workOutLine_;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	//bool _CheckDatas(i8desk::data_helper::SyncTaskTraits::ValueType &val);
	void _ShowView();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSyncTaskAdd();
	afx_msg void OnBnClickedButtonSyncTaskDel();
	afx_msg void OnBnClickedButtonSyncTaskModify();
	afx_msg void OnLvnGetdispinfoListSyncTaskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRDblclkListSyncTaskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
