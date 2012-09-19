#pragma once

#include "../../include/UI/Skin/SkinButton.h"
#include "../../include/UI/Skin/SkinLabel.h"
#include "../../include/UI/Skin/SkinCheckBox.h"
#include "../../include/UI/Frame/NonFrameDlg.h"


// CDlgLoad 对话框

class CDlgLoad 
	: public ui::frame::CNonFrameDlg
{
	DECLARE_DYNAMIC(CDlgLoad)

public:
	CDlgLoad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLoad();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	ui::skin::Button wndBtnMin_;
	ui::skin::Button wndBtnClose_;
	
	ui::skin::Button wndBtnLoad_;
	ui::skin::Button wndBtnReset_;
	ui::skin::Button wndBtnUnknwonPsw_;

	ui::skin::CheckBox wndBtnRemember_;
	ui::skin::CheckBox wndBtnAutoLoad_;

	ui::skin::Label wndLabelUserName_;
	ui::skin::Label wndLabelPsw_;

	bool autoPsw_;
	bool autoLoad_;
	
	CEdit wndEditUserName_;
	CEdit wndEditPsw_;
	
	ATL::CImage bkImg_;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonLoadMin();
	afx_msg void OnBnClickedButtonLoadClose();
	
	afx_msg void OnDestroy();
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCheckLoadRememberPsw();
	afx_msg void OnBnClickedCheckLoadAuto();
	afx_msg void OnBnClickedButtonLoadLoad();
	afx_msg void OnBnClickedButtonLoadReset();
	afx_msg void OnBnClickedButtonLoadUnknownPassword();
};
