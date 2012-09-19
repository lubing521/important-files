#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinEdit.h"


// WZDlgVDisk 对话框
class WZDlgVDisk 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgVDisk)

public:
	WZDlgVDisk(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgVDisk();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_VIRTUAL_DISK };

private:
	CString   IPVDiskIP_;

	i8desk::ui::SkinComboBox wndComboSvrName_;
	i8desk::ui::SkinComboBox wndComboSvrIP_;

	i8desk::ui::SkinComboBox wndComboServerDriver_;
	i8desk::ui::SkinComboBox wndComboClientDriver_;
	i8desk::ui::SkinComboBox wndComboRefreshTime_;
	i8desk::ui::SkinComboBox wndComboSsdDriver_;

	i8desk::ui::SkinEdit wndEditSize_;
	CString textSize_;

	i8desk::ui::SkinListCtrl wndListVDisks_;

	i8desk::ui::SkinLabel wndLabelVDisksSet_;
	i8desk::ui::SkinLabel wndLabelSvrName_;
	i8desk::ui::SkinLabel wndLabelVDisksIP_;
	i8desk::ui::SkinLabel wndLabelServerDriver_;
	i8desk::ui::SkinLabel wndLabelClientDriver_;
	i8desk::ui::SkinLabel wndLabelRefreshTime_;
	i8desk::ui::SkinLabel wndLabelVDisksInfo_;
	i8desk::ui::SkinLabel wndLabelVDisksList_;
	i8desk::ui::SkinLabel wndLabelSize_;
	i8desk::ui::SkinLabel wndLabelMB_;
	i8desk::ui::SkinLabel wndLabelSsdDriver_;

	i8desk::ui::SkinButton wndbtnAdd_;
	i8desk::ui::SkinButton wndbtnDel_;
	i8desk::ui::SkinButton wndbtnModify_;

	bool IsAdd_;
	i8desk::data_helper::VDiskTraits::VectorType curvDisks_;

	ATL::CImage workOutLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool _CheckDatas(i8desk::data_helper::VDiskTraits::ValueType &val);
	void _ShowView();
	void _ReadDatas();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonVdiskAdd();
	afx_msg void OnBnClickedButtonVdiskDel();
	afx_msg void OnBnClickedButtonVdiskModify();
	afx_msg void OnLvnGetdispinfoListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboSvrName();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
