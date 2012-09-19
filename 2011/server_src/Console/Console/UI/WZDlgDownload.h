#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "../CustomControl/CustomListCtrl.h"

#include "Skin/SkinImage.h"



// WZDlgDownload 对话框

class WZDlgDownload 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgDownload)

public:
	WZDlgDownload(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgDownload();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_DOWNLOAD };
private:
	i8desk::ui::SkinComboBox  wndComboMaxTaskNum_;
	i8desk::ui::SkinComboBox  wndComboMaxSpeed_;
	i8desk::ui::SkinEdit	  wndEditTempDir_;
	i8desk::ui::SkinEdit	  wndEditDownPath_;
	i8desk::ui::SkinButton	  wndBtnTempDir_;
	i8desk::ui::SkinButton	  wndBtnPath_;

	i8desk::ui::SkinLabel	  wndLabelSet_;
	i8desk::ui::SkinLabel	  wndLabelMaxNum_;
	i8desk::ui::SkinLabel	  wndLabelSpeed_;
	i8desk::ui::SkinLabel	  wndLabelDir_;
	i8desk::ui::SkinLabel	  wndLabelPath_;
	i8desk::ui::SkinLabel	  wndLabelParam_;
	i8desk::ui::SkinLabel	  wndLabelRunType_;

	CCustomListCtrl	  listRunTypeCtrl_;
	ATL::CImage outboxLine_;

	CString   MaxTaskNum_;
	CString   MaxSpeed_;
	CString   TempDir_;
	CString   DownPath_;

private:
	void _DrawRunTypeList();
	BOOL _UpdateRunTypeList();
	void _ReadDatas();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	virtual void OnComplate();
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonPath();
	afx_msg void OnBnClickedSelectMsctmpdir();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
