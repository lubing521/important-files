#pragma once

#include "../../include/UI/Skin/SkinButton.h"
#include "../../include/UI/Skin/SkinLabel.h"
#include "../../include/UI/Frame/NonFrameDlg.h"

// CDlgModify 对话框

class CDlgModify : public ui::frame::CNonFrameDlg
{
	DECLARE_DYNAMIC(CDlgModify)

public:
	CDlgModify(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgModify();

// 对话框数据
	enum { IDD = IDD_DIALOG_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	ui::skin::Label wndLabelOldPsw_;
	ui::skin::Label wndLabelNewPsw_;
	ui::skin::Label wndLabelConfirmPsw_;
	
	ui::skin::Button wndBtnOK_;
	ui::skin::Button wndBtnCancel_;
	
	CEdit wndEditOldPsw_;
	CEdit wndEditNewPsw_;
	CEdit wndEditConfirmPsw_;

	ATL::CImage bkImg_;

public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonModifyOk();
	afx_msg void OnBnClickedButtonModifyCancel();
};
