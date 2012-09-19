#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "../ManagerInstance.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinListCtrl.h"

// WZDlgArea 对话框

class WZDlgArea 
	: public CDialog
	, public i8desk::wz::WZInterface

{
	DECLARE_DYNAMIC(WZDlgArea)

public:
	WZDlgArea(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgArea();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_AREA };

private:
	CString			AreaName_;

	i8desk::ui::SkinLabel		wndLabelName_;
	i8desk::ui::SkinListCtrl	wndListAreas_;
	i8desk::ui::SkinEdit		wndEditArea_;

	i8desk::ui::SkinLabel		wndLabelAreaName_;
	i8desk::ui::SkinButton		wndbtnAdd_;
	i8desk::ui::SkinButton		wndbtnDel_;
	i8desk::ui::SkinButton		wndbtnModify_;

	i8desk::data_helper::AreaTraits::VectorType curAreas_;

	ATL::CImage workOutLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool _CheckDatas(i8desk::data_helper::AreaTraits::ValueType &val);
	void _ShowView();
	void _ReadDatas();

public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonAreaAdd();
	afx_msg void OnBnClickedButtonAreaDel();
	afx_msg void OnBnClickedButtonAreaModify();
	afx_msg void OnLvnGetdispinfoListAreaInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListAreaInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
