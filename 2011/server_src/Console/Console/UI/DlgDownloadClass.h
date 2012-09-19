#pragma once

#include "../ManagerInstance.h"
#include "afxwin.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomEdit.h"
#include "../CustomControl/CustomLabel.h"
#include "../CustomControl/CustomCheckBox.h"
#include "../CustomControl/CustomButton.h"
#include "../CustomControl/CustomImage.h"

// CDlgDownloadAddClass 对话框
using i8desk::data_helper::ClassTraits;

class CDlgDownloadClass : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadClass)

public:
	CDlgDownloadClass(bool isAdd, ClassTraits::ElementType *val, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadClass();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNLOAD_CLASS };

private:
	bool isAdd_;
	ClassTraits::ValueType curClass_;
	
	CString className_;
	CString defPath_;
	CCustomEdit wndEditclassName_;
	CCustomEdit wndEditdefPath_;

	CCustomLabel wndclassName_;
	CCustomLabel wnddefPath_;

	CCustomButton wndBtnOk_;
	CCustomButton wndBtnCancel_;
	CCustomButton wndBtnBrowDir_;

	utility::BITMAPPtr icon_;
	CCustomImage wndIcon_;
	CCustomCheckBox wndCheckDesktopIcon_;

public:
	ClassTraits::ValueType &GetCurClass()
	{
		return curClass_;
	}
	const ClassTraits::ValueType &GetCurClass() const
	{
		return curClass_;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnStnClickedStaticClassIcon();
	afx_msg void OnBnClickedButtonBrowseDir();

};
