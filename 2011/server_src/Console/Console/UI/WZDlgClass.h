#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinListCtrl.h"


// WZDlgClass 对话框

class WZDlgClass 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgClass)

public:
	WZDlgClass(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgClass();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_CLASS };

private:
	i8desk::ui::SkinListCtrl	wndListClasses_;
	i8desk::ui::SkinEdit		wndEditClassName;
	i8desk::ui::SkinEdit		wndEditClassPath;

	CString			className_;
	CString			classPath_;

	i8desk::ui::SkinLabel		wndLabelClassName;
	i8desk::ui::SkinLabel		wndLabelClassPath;

	i8desk::ui::SkinButton		wndbtnAdd_;
	i8desk::ui::SkinButton		wndbtnDel_;
	i8desk::ui::SkinButton		wndbtnModify_;
	i8desk::ui::SkinButton		wndBtnDir_;

	i8desk::data_helper::ClassTraits::VectorType curClasses_;
	stdex::tString defaultClassPath_;

	ATL::CImage outboxLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool _CheckDatas(i8desk::data_helper::ClassTraits::ValueType &val);
	void _ShowView();
	void _ReadDatas();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonClassAdd();
	afx_msg void OnBnClickedButtonClassDel();
	afx_msg void OnBnClickedButtonClassModify();
	afx_msg void OnBnClickedButtonClassPath();
	afx_msg void OnLvnGetdispinfoListClassInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListClassInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
