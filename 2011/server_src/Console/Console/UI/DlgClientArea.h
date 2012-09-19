#pragma once

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomEdit.h"
#include "../CustomControl/CustomButton.h"
#include "../CustomControl/CustomLabel.h"

// CDlgClientArea 对话框
using i8desk::data_helper::AreaTraits;

class CDlgClientArea : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgClientArea)

public:
	CDlgClientArea(bool isAdd, const AreaTraits::ElementType *area, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgClientArea();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIENT_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

private:
	bool isAdd_;
	AreaTraits::ValueType curArea_;
	CString areaName_;
	CCustomEdit wndareaName_;
	CCustomButton wndBtnOk_;
	CCustomButton wndBtnCancel_;
	CCustomLabel wndLabelareaName_;

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
};
