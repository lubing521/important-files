#pragma once
#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"

#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinImage.h"
#include "Skin/SkinGroup.h"

class CDlgConfig;

// CDlgOptUserMgr 对话框

class CDlgCfgUserMgr : public CDialog
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CDlgCfgUserMgr)

public:
	CDlgCfgUserMgr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCfgUserMgr();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONFIG_USERMGR };
	enum { WM_MSG_OK = WM_USER + 100 };

public:
	void ReadData();

private:
	BOOL bModify_;
	CString consolepwd_;
	CString dateend_;
	CString bindid_;
	CString netname_;
	CString netid_;
	CString addr_;
	CString address_;
	CString telephone_;
	CString mobile_;
	CString bindname_;
	CString disktype_;
	CString netstatus_;
	CString feesname_;
	CString disklessname_;
	CString monitorname_;
	CString	textNetbarJpg_;

	
	CEdit wndEditConsolePwd_;
	i8desk::ui::SkinEdit   wndEditNetbarJpg_;

	i8desk::ui::SkinButton wndBtnModify_;
	i8desk::ui::SkinButton wndBtnModifyInfo_;
	i8desk::ui::SkinButton wndBtnNetbarJpg_;

	i8desk::ui::SkinLabel wndDateEnd_;
	i8desk::ui::SkinLabel wndBindID_;
	i8desk::ui::SkinLabel wndNetName_;
	i8desk::ui::SkinLabel wndNetID_;
	i8desk::ui::SkinLabel wndAddr_;
	i8desk::ui::SkinLabel wndAddress_;
	i8desk::ui::SkinLabel wndTelephone_;
	i8desk::ui::SkinLabel wndMobile_;
	i8desk::ui::SkinLabel wndBindName_;
	i8desk::ui::SkinGroup wndAuthorize_;
	i8desk::ui::SkinLabel wndAccount_;
	i8desk::ui::SkinLabel wndPwd_;
	i8desk::ui::SkinGroup wndReg_;
	i8desk::ui::SkinLabel wndTel_;
	i8desk::ui::SkinGroup wndConPwd_;
	i8desk::ui::SkinGroup wndLabelNetbar_;
	i8desk::ui::SkinLabel wndConLoginPwd_;
	i8desk::ui::SkinLabel wndPhone_;
	i8desk::ui::SkinLabel wndDisktype_;
	i8desk::ui::SkinLabel wndNetstatus_;
	i8desk::ui::SkinLabel wndDisklessname_;
	i8desk::ui::SkinLabel wndFeesname_;
	i8desk::ui::SkinLabel wndMonitorname_;
	i8desk::ui::SkinLabel wndLabelNetbarJpg_;
	i8desk::ui::SkinLabel wndNetBarReview_;


	i8desk::ui::SkinLabel wndconsolepwd_;
	i8desk::ui::SkinLabel wnddateend_;
	i8desk::ui::SkinLabel wndregdateend_;
	i8desk::ui::SkinLabel wndbindid_;
	i8desk::ui::SkinLabel wndregbindid_;
	i8desk::ui::SkinLabel wndnetname_;
	i8desk::ui::SkinLabel wndnetid_;
	i8desk::ui::SkinLabel wndaddr_;
	i8desk::ui::SkinLabel wndaddress_;
	i8desk::ui::SkinLabel wndtelephone_;
	i8desk::ui::SkinLabel wndbindname_;
	i8desk::ui::SkinLabel wndmobile_;
	i8desk::ui::SkinLabel wnddisktype_;	
	i8desk::ui::SkinLabel wndfeesname_;
	i8desk::ui::SkinLabel wndmonitorname_;
	i8desk::ui::SkinLabel wndnetstatus_;	
	i8desk::ui::SkinLabel wnddisklessname_;	

	i8desk::ui::SkinImage wndNetBarPic_;


	ATL::CImage outBox_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	void DrawPic( LPCTSTR filename, i8desk::ui::SkinImage &wndpic);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnErrorMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonCfgUserMgrModify();
	afx_msg void OnBnClickedButtonCfgUserMgrModifyInfo();
	afx_msg void OnBnClickedNetbarJpg();


};
