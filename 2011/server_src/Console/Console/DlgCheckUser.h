#pragma once

#include "I8UIFrame/NonFrameChildDlg.h"
#include "UI/Skin/SkinButton.h"
#include "UI/Skin/SkinLabel.h"
#include "UI/Skin/SkinEdit.h"



class CDlgCheckUser 
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgCheckUser)

public:
	CDlgCheckUser(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCheckUser();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHECK_USER };

private:
	i8desk::ui::SkinButton wndBtnOK_;
	i8desk::ui::SkinButton wndBtnCancel_;
	i8desk::ui::SkinLabel wndUserText_;
	i8desk::ui::SkinLabel wndPswText_;
	i8desk::ui::SkinLabel wndErrorText_;
	i8desk::ui::SkinEdit wndEditUser_;
	CEdit wndEditPsw_;
	ATL::CImage outLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	stdex::tString GetUserName() const;
	stdex::tString GetPassword() const;
	void Check();
	void OnCheckError(bool suc);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnCheckErrorMsg(WPARAM, LPARAM);
};
