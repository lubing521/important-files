
// ConsoleDlg.h : 头文件
//

#pragma once

#include "afxwin.h"


#include "./ui/WindowManager.h"
#include "../../../include/ui/UI.h"
#include "../../../include/ui/Frame/NonFrameDlg.h"

#include "EasySize.h"
#include "ManagerInstance.h"

#include "CustomControl/CustomLabel.h"
#include "CustomControl/CustomImage.h"
#include "CustomControl/CustomButton.h"
#include "CustomControl/CustomRadio.h"
#include "Business/MainBusiness.h"

// CConsoleDlg 对话框
class CConsoleDlg 
	: public CNonFrameDlg
	, public i8desk::ui::ChildMgr
{
	DECLARE_EASYSIZE

	typedef CNonFrameDlg BaseClass;

private:
	HICON m_hIcon;
	WTL::CAccelerator accel_;


	i8desk::ui::WindowMgr wndsMgr_;
	std::auto_ptr<CDialog> runTypeState_;

	CCustomButton wndBtnSetting_;
	CCustomButton wndBtnMin_;
	CCustomButton wndBtnMax_;
	CCustomButton wndBtnClose_;
	CCustomButton wndBtnHelp_;

	CButton *curRadio_;
	CCustomRadio wndBtnMonitor_;
	CCustomRadio wndBtnServer_;
	CCustomRadio wndBtnClient_;
	CCustomRadio wndBtnResource_;
	CCustomRadio wndBtnSyncTask_;
	CCustomRadio wndBtnPush_;
	CCustomRadio wndBtnStartup_;
	CCustomRadio wndBtnTool_;
	CCustomRadio wndBtnConfig_;

	CCustomLabel wndCenterRes_;
	CCustomLabel wndLocalRes_;
	CCustomLabel wndClients_;
    CCustomLabel wndServerStatus_;
    CCustomLabel wndSelectStatus_;

	CCustomImage wndStatusImg_;

	ATL::CImage topLogo_;
	ATL::CImage statusLine_;
	ATL::CImage controlBk_;
	
	std::tr1::array<ATL::CImage, 3> topBmp_;
	std::tr1::array<ATL::CImage, 3> contentBmp_;
	std::tr1::array<ATL::CImage, 3> bottomBmp_;

	i8desk::data_helper::StaticsStatusTraits::ElementType staticcStatus_;
    i8desk::business::CSelectState selectState_;
// 构造
public:
	CConsoleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CONSOLE_DIALOG };

private:
	void _UpdateBtnRect();
	void _UpdateStatics();
	void _RecvConsoleMsg();
	void _CheckUserNotify(bool suc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	virtual BOOL PreTranslateMessage(MSG *pMsg);


private:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	
	afx_msg LRESULT OnReInitMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectTab(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheckUserInfoMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecvConsoleMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSaveHardwareMsg(WPARAM, LPARAM);
	afx_msg LRESULT OnAddException(WPARAM, LPARAM);
	afx_msg LRESULT OnShowLog(WPARAM, LPARAM);
    afx_msg LRESULT OnShowLogMore(WPARAM, LPARAM);
    afx_msg LRESULT OnUpdateSelectState(WPARAM wParam,LPARAM lParam);

	afx_msg void OnBnClickedButtonMainClose();
	afx_msg void OnBnClickedButtonMainMax();
	afx_msg void OnBnClickedButtonMainMin();
	afx_msg void OnBnClickedButtonMainSet();
	afx_msg void OnBnClickedButtonHelp();

	afx_msg void OnBnClickedButtonMainTabMonitor();
	afx_msg void OnBnClickedButtonMainTabServer();
	afx_msg void OnBnClickedButtonMainTabClient();
	afx_msg void OnBnClickedButtonMainTabResource();
	afx_msg void OnBnClickedButtonMainTabSyncTask();
	afx_msg void OnBnClickedButtonMainTabPush();
	afx_msg void OnBnClickedButtonMainTabStartup();
	afx_msg void OnBnClickedButtonMainTabTool();
	afx_msg void OnBnClickedButtonMainTabConfig();

	afx_msg void OnMonitorException();
	afx_msg void OnViewRunState();
	afx_msg void OnVideoHelp();
	afx_msg void OnFix();
	afx_msg void OnBBS();
	afx_msg void OnOnline();
	afx_msg void OnHelpAbout();
};
