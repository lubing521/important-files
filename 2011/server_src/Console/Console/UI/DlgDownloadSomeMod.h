#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridListCtrlEx.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomComboBox.h"
#include "../CustomControl/CustomButton.h"
#include "../CustomControl/CustomLabel.h"
#include "../CustomControl/CustomListCtrl.h"
#include "../CustomControl/CustomCheckBox.h"


// CDlgDownloadSomeMod 对话框

class CDlgDownloadSomeMod : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadSomeMod)

public:
	CDlgDownloadSomeMod(i8desk::data_helper::GameTraits::VectorType &games, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadSomeMod();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNLOAD_SOME_MODIFY };

private:
	i8desk::data_helper::GameTraits::VectorType &games_;

	CCustomEdit wndEditSvrPathSrc_;
	CCustomEdit wndEditSvrPathDest_;
	CCustomEdit wndEditCliPathSrc_;

	CCustomCheckBox wndCheckRepCliPath_;
	CCustomCheckBox wndCheckRepSvrPath_;
	CCustomCheckBox wndCheckRepClass_;
	CCustomCheckBox wndCheckRepUpdate_;
	CCustomCheckBox wndCheckRepPriority_;
	CCustomCheckBox wndCheckRepRunType_;
	CCustomCheckBox wndCheckRepOther_;
	CCustomCheckBox wndCheckTool_;
	CCustomCheckBox wndCheckDeskLink_;

	CCustomLabel wndSvrPathSrc_;
	CCustomLabel wndSvrPathDest_;

	CCustomComboBox wndComboClass_;
	CCustomComboBox wndComboUpdate_;
	CCustomComboBox wndComboPriority_;

	CCustomButton wndBtnOk_;
	CCustomButton wndBtnCancel_;
	CCustomListCtrl wndListRunType_;
	ATL::CImage outboxLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCheck();

	
};
