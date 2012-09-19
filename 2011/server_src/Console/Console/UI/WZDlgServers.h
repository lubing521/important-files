#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinComboBox.h"

// WZDlgMainServer 对话框

class WZDlgServers 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgServers)

public:
	WZDlgServers(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgServers();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_SERVER };

private:
	i8desk::ui::SkinEdit	wndEditSvrName_;
	i8desk::ui::SkinComboBox wndComboSvrIP_;

	i8desk::ui::SkinRadioBox wndradioDistribute_;
	i8desk::ui::SkinRadioBox wndradioAreaPriority_;
	i8desk::ui::SkinRadioBox wndradioDynamic_;

	i8desk::ui::SkinButton	wndbtnAdd_;
	i8desk::ui::SkinButton	wndbtnDel_;
	i8desk::ui::SkinButton	wndbtnModify_;

	i8desk::ui::SkinLabel	wndServerName_;
	i8desk::ui::SkinLabel	wndLabelDefaultIP_;
	

	i8desk::ui::SkinListCtrl wndlistServers_;
	CTreeCtrl				wndTreeArea_;

	CString serverName_;

	i8desk::data_helper::ServerTraits::VectorType curservers_;
	ATL::CImage workOutLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool _CheckData(i8desk::data_helper::ServerTraits::ValueType &val);
	bool _ModifyPriorityArea(LPCTSTR svrName);
	void _ShowView();
	void _ReadDatas();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadioServerInfoDynamic();
	afx_msg void OnBnClickedRadioServerInfoDistribute();
	afx_msg void OnBnClickedRadioServerInfoPriorityArea();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonModify();

	afx_msg void OnLvnGetdispinfoListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
};
