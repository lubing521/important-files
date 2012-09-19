#pragma once
#include "WZInterface.h"
#include "../I8UIFrame/NonFrameChildDlg.h"

#include "Skin/SkinLabel.h"

// WZDlgWelcome 对话框

class WZDlgWelcome 
	: public CDialog
	, public i8desk::wz::WZInterface
{
	DECLARE_DYNAMIC(WZDlgWelcome)

public:
	WZDlgWelcome(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WZDlgWelcome();

// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_WELCOME };

private:
	i8desk::ui::SkinLabel wndLabelWelCome_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	virtual void OnComplate(){}
	virtual void OnShow(int showtype);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};
