// WZDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDialog.h"

#include "WZInterface.h"
#include "WZDlgWelcome.h"
#include "WZDlgArea.h"
#include "WZDlgVDisk.h"
#include "WZDlgClass.h"
#include "WZDlgDownload.h"
#include "WZDlgClient.h"
#include "WZDlgAddSome.h"
#include "WZDlgSafePwd.h"
#include "WZDlgCmpBootTask.h"
#include "WZDlgPushGame.h"
#include "WZDlgServers.h"
#include "WZDlgSyncTask.h"
#include "WZDlgSafeCenter.h"

#include "UIHelper.h"

#include <functional>

// CWZDialog 对话框

IMPLEMENT_DYNAMIC(CWZDialog, CNonFrameChildDlg)

CWZDialog::CWZDialog(CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CWZDialog::IDD, pParent)
	, curWnd_(0)
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

CWZDialog::~CWZDialog()
{
	workOutLine_.Detach();
}

void CWZDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WZ_TOP_LOGO, wndLogo_);
	DDX_Control(pDX, IDC_LIST_WZ_SELECT, wndListSel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_PREV, wndBtnPrev_);
	DDX_Control(pDX, IDC_BUTTON_WZ_NEXT, wndBtnNext_);
	DDX_Control(pDX, IDC_BUTTON_WZ_OK, wndBtnOK_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CANCEL, wndBtnCancel_);
}


BEGIN_MESSAGE_MAP(CWZDialog, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_WZ_PREV, &CWZDialog::OnBnClickedButtonWzPrev)
	ON_BN_CLICKED(IDC_BUTTON_WZ_NEXT, &CWZDialog::OnBnClickedButtonWzNext)
	ON_BN_CLICKED(IDC_BUTTON_WZ_OK, &CWZDialog::OnBnClickedButtonWzOk)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CANCEL, &CWZDialog::OnBnClickedButtonWzCancel)
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST_SELECT, &CWZDialog::OnLbnSelchangeListSelect)
	ON_WM_PAINT()

END_MESSAGE_MAP()

template < typename T >
void CWZDialog::_AddProperty(const CRect &rc, const stdex::tString &title)
{
	DialogPtr newDlg(new T);
	newDlg->Create(T::IDD, this);
	newDlg->MoveWindow(rc);

	wzWnds_.push_back(newDlg);
	int index = wndListSel_.AddString(title.c_str());
	wndListSel_.SetItemData(index, T::IDD);
}

void CWZDialog::_ShowProperty(size_t index)
{
	// 切换
	dynamic_cast<i8desk::wz::WZInterface *>(wzWnds_[curWnd_].get())->OnShow(SW_HIDE);
	dynamic_cast<i8desk::wz::WZInterface *>(wzWnds_[index].get())->OnShow(SW_SHOW);


	if( index == 0 )
	{
		wndBtnPrev_.EnableWindow(FALSE);
		wndBtnNext_.EnableWindow(TRUE);
	}
	else if( index == wzWnds_.size() - 1 )
	{
		wndBtnPrev_.EnableWindow(TRUE);
		wndBtnNext_.EnableWindow(FALSE);
	}
	else
	{
		wndBtnPrev_.EnableWindow(TRUE);
		wndBtnNext_.EnableWindow(TRUE);
	}
	
	wndListSel_.SetCurSel(index);
	curWnd_ = index;
}



BOOL CWZDialog::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("设置配置向导"));
	CRect rcDst;
	GetDlgItem(IDC_STATIC_WZ_BOARD)->GetWindowRect(rcDst);
	ScreenToClient(rcDst);
	GetDlgItem(IDC_STATIC_WZ_BOARD)->ShowWindow(SW_HIDE);

	wndLogo_.SetImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Main/Logo.png")));

	_AddProperty<WZDlgWelcome>(rcDst,		_T("欢迎使用向导"));
	_AddProperty<WZDlgSafePwd>(rcDst,		_T("设置安全密码"));
	_AddProperty<WZDlgArea>(rcDst,			_T("划分终端区域"));
	_AddProperty<WZDlgServers>(rcDst,		_T("部署服务器"));
	_AddProperty<WZDlgVDisk>(rcDst,			_T("设置虚拟磁盘"));
	_AddProperty<WZDlgDownload>(rcDst,		_T("设置三层资源"));
	_AddProperty<WZDlgClass>(rcDst,			_T("划分游戏类别"));
	_AddProperty<WZDlgAddSome>(rcDst,		_T("搜索添加游戏"));
	_AddProperty<WZDlgClient>(rcDst,		_T("设置客户端"));
	_AddProperty<WZDlgCmpBootTask>(rcDst,	_T("设置开机任务"));
	_AddProperty<WZDlgPushGame>(rcDst,		_T("设置游戏推送"));
	_AddProperty<WZDlgSyncTask>(rcDst,		_T("部署同步任务"));
	_AddProperty<WZDlgSafeCenter>(rcDst,	_T("设置安全中心"));

	wndListSel_.SetCurSel(curWnd_);
	OnLbnSelchangeListSelect();
	wndBtnPrev_.EnableWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWZDialog::OnDestroy()
{
	__super::OnDestroy();

	std::for_each(wzWnds_.begin(), wzWnds_.end(),
		std::tr1::bind(&CWnd::DestroyWindow, std::tr1::placeholders::_1));
}

void CWZDialog::OnPaint()
{
	WTL::CPaintDC dc(GetSafeHwnd());
	
	CRect rcList;
	wndListSel_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, dc, rcList, &workOutLine_);

	CRect rcWork;
	GetDlgItem(IDC_STATIC_WZ_BOARD)->GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, dc, rcWork, &workOutLine_);
}


// CWZDialog 消息处理程序

void CWZDialog::OnBnClickedButtonWzPrev()
{
	_ShowProperty(curWnd_ - 1);
}

void CWZDialog::OnBnClickedButtonWzNext()
{
	_ShowProperty(curWnd_ + 1);
}

void CWZDialog::OnBnClickedButtonWzOk()
{
	struct OnComplate
	{
		void operator()(const DialogPtr &wz)
		{
			dynamic_cast<i8desk::wz::WZInterface *>(wz.get())->OnComplate();
		}	
	};
	std::for_each(wzWnds_.begin(), wzWnds_.end(), OnComplate());

	OnOK();
}

void CWZDialog::OnBnClickedButtonWzCancel()
{
	OnCancel();
}



void CWZDialog::OnLbnSelchangeListSelect()
{
	int sel = wndListSel_.GetCurSel();
	if( sel == -1 )
		return;

	size_t id = wndListSel_.GetItemData(sel);
	_ShowProperty(sel);
}

BOOL CWZDialog::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	return TRUE;
}
