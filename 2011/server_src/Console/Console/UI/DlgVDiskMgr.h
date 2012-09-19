#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinIPCtrl.h"
#include "Skin/SkinEdit.h"

// CDlgVDiskMgr 对话框

class CDlgVDiskMgr : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgVDiskMgr)

	typedef std::tr1::function<void(const std::string, u_short, CString &)> Callback;

public:
	CDlgVDiskMgr(CWnd* pParent);   // 标准构造函数
	virtual ~CDlgVDiskMgr();

// 对话框数据
	enum { IDD = IDD_DIALOG_VDISKMGR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
private:
	bool _CheckDatas(i8desk::data_helper::VDiskTraits::ValueType &val);
	void _VDiskOperateOne(int nSel, const Callback &callback);
	void _ShowView();
	void _ReadVdisk();

private:
	CWnd *parent_;
	i8desk::ui::SkinComboBox wndComboSvrIP_;
	i8desk::ui::SkinComboBox wndComboSvrName_;
	i8desk::ui::SkinComboBox wndComboServerDriver_;
	i8desk::ui::SkinComboBox wndComboClientDriver_;
	i8desk::ui::SkinComboBox wndComboRefreshTime_;
	i8desk::ui::SkinComboBox wndComboSsdDriver_;

	i8desk::ui::SkinEdit wndEditSize_;

	i8desk::ui::SkinListCtrl wndListVDisks_;

	i8desk::ui::SkinLabel wndVDisksSet_;
	i8desk::ui::SkinLabel wndType_;
	i8desk::ui::SkinLabel wndVDisksIP_;
	i8desk::ui::SkinLabel wndServerDriver_;
	i8desk::ui::SkinLabel wndClientDriver_;
	i8desk::ui::SkinLabel wndRefreshTime_;
	i8desk::ui::SkinLabel wndVDisksInfo_;
	i8desk::ui::SkinLabel wndVDisksList_;
	i8desk::ui::SkinLabel wndSvrName_;
	i8desk::ui::SkinLabel wndSsdDriver_;
	i8desk::ui::SkinLabel wndSize_;
	i8desk::ui::SkinLabel wndMB_;

	i8desk::ui::SkinButton wndbtnAdd_;
	i8desk::ui::SkinButton wndbtnModify_;
	i8desk::ui::SkinButton wndbtnDelete_;
	i8desk::ui::SkinButton wndbtnCheck_;
	i8desk::ui::SkinButton wndbtnReflish_;
	i8desk::ui::SkinButton wndbtnReflishAll_;

	CEdit	wndEditmsg_;
	ATL::CImage outboxLine_;

	CString msg_;
	CString textSize_;

	bool isAdd_;
	i8desk::data_helper::VDiskTraits::VectorType curvDisks_;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchangeComboSvrName();

	afx_msg void OnBnClickedButtonVdiskAdd();
	afx_msg void OnBnClickedButtonVdiskModify();
	afx_msg void OnBnClickedButtonVdiskDelete();
	afx_msg void OnBnClickedButtonVdiskCheck();
	afx_msg void OnBnClickedButtonVdiskReflesh();
	afx_msg void OnBnClickedButtonVdiskRefleshAll();

	afx_msg void OnNMClickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult);

	

};
