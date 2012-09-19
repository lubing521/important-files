#pragma once
#include "CustomControl/CustomImage.h"
#include "I8UIFrame/NonFrameChildDlg.h"
#include "UI/Skin/SkinButton.h"


// CDlgAbout 对话框

class CDlgAbout : public CDialog
{
	DECLARE_DYNAMIC(CDlgAbout)

public:
	CDlgAbout(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAbout();

// 对话框数据
	enum { IDD = IDD_DIALOG_ABOUT };

private:
	CString GetFileVersionString(CString file);
	CString GetProductVersionString(CString file);

	CCustomImage bk_;
	i8desk::ui::SkinButton wndbtnOK_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOk();

};
