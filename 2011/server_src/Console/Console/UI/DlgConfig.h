#pragma once


#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "DlgCfgUserMgr.h"
#include "DlgCfgClientSet.h"
#include "DlgCfgDownloadSet.h"
#include "DlgCfgSafeCenter.h"

#include "../CustomControl/CustomButton.h"
#include "../CustomControl/CustomRadio.h"


// CDlgConfig 窗体视图

class CDlgConfig 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE
	DECLARE_DYNCREATE(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);           // 动态创建所使用的受保护的构造函数
	virtual ~CDlgConfig();

public:
	enum { IDD = IDD_DLG_CONFIG };
	enum { WM_MSG_OK = WM_USER + 100 };


private:
	i8desk::ui::ControlMgr wndMgr_;


	CDlgCfgUserMgr wndOptUserMgr_;
	CDlgCfgClientSet wndOptClientSet_;
	CDlgCfgDownloadSet wndOptDownloadSet_;
	CDlgCfgSafeCenter wndOptSafeSet_;

	CCustomRadio wndBtnAcount_;
	CCustomRadio wndBtnClient_;
	CCustomRadio wndBtnDownload_;
	CCustomRadio wndBtnSafe_;

	CCustomButton wndBtnOK_;
	CCustomButton wndBtnCancel_;

	CStatic wndStaConfig_;

	ATL::CImage workOutLine_;

public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonConfigAcount();
	afx_msg void OnBnClickedButtonConfigClient();
	afx_msg void OnBnClickedButtonConfigDownload();
	afx_msg void OnBnClickedButtonConfigSafe();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	afx_msg LRESULT OnApplySelect(WPARAM wParam, LPARAM lParam);

};


