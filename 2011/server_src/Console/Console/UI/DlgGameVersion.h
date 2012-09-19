#pragma once

#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinCheckBox.h"


// CDlgGameVersion 对话框

class CDlgGameVersion 
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgGameVersion)

public:
	CDlgGameVersion(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGameVersion();

// 对话框数据
	enum { IDD = IDD_DIALOG_GAME_VERSION_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

private:
	i8desk::ui::SkinLabel wndLabel_;
	i8desk::ui::SkinCheckBox wndCheckOK_;

public:
	DECLARE_MESSAGE_MAP()
	
};
