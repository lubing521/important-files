#pragma once

#include "../ManagerInstance.h"
#include <set>
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinRadioBox.h"


// CDlgAddModify 对话框

class CDlgPushGameAddModify : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgPushGameAddModify)

public:
	CDlgPushGameAddModify(bool isAdd, i8desk::data_helper::PushGameTraits::ElementType *pushgame,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPushGameAddModify();

	// 对话框数据
	enum { IDD = IDD_DIALOG_PUSHGAME_ADDORMODIFY };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
	DECLARE_MESSAGE_MAP()

private:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnNMClickTreeGame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckIndex();
	afx_msg void OnBnClickedCheckFile();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	bool writeParam();
	bool writeName();
private:
	bool			isAdd_;
	CTreeCtrl		wndTreeGame_;
	CTreeCtrl		wndTreeArea_;
	CString			wndEdittaskName_;
	CString			wndEditmaxSpeed_;

	i8desk::ui::SkinEdit wndtaskName_;
	i8desk::ui::SkinEdit wndmaxSpeed_;
	i8desk::ui::SkinLabel wndlabeltaskName_;
	i8desk::ui::SkinLabel wndlabelGame_;
	i8desk::ui::SkinLabel wndlabelArea_;
	i8desk::ui::SkinLabel wndlabelParam_;
	i8desk::ui::SkinLabel wndlabelSpeed_;
	i8desk::ui::SkinLabel wndlabelPrase_;
	i8desk::ui::SkinLabel wndlabelType_;

	i8desk::ui::SkinRadioBox wndRadioIndex_;
	i8desk::ui::SkinRadioBox wndRadioFile_;
	
	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;

	ATL::CImage outboxLine_;

	typedef std::set<stdex::tString> AIDs;
	AIDs aids_;

	i8desk::data_helper::PushGameTraits::ValueType pushgame_;
	i8desk::data_helper::BootTaskAreaTraits::VectorType curTaskareas_;

	
};