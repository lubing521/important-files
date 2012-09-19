// DlgVDiskInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgVDiskInfo.h"


#include "../Business/ServerBusiness.h"
#include "../Business/VDiskBusiness.h"

#include "../Misc.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../MessageBox.h"
#include "WindowManager.h"

// CDlgVDiskInfo 对话框

IMPLEMENT_DYNAMIC(CDlgVDiskInfo, CNonFrameChildDlg)

CDlgVDiskInfo::CDlgVDiskInfo(bool isAdd, i8desk::data_helper::VDiskTraits::ElementType *val,  const i8desk::data_helper::ServerTraits::ElementType *parentSvr, CWnd* pParent)
	: CNonFrameChildDlg(CDlgVDiskInfo::IDD, pParent)
	, isAdd_(isAdd)
	, curVDisk_(new i8desk::data_helper::VDiskTraits::ElementType)
	, parentSvr_(parentSvr)
	, parentWnd_(pParent)
	, textSize_(_T(""))
{
	if( isAdd )
	{
		utility::Strcpy(curVDisk_->VID, utility::CreateGuidString());
		utility::Strcpy(curVDisk_->SvrID, parentSvr_->SvrID);
	}
	else
	{
		*curVDisk_ = *val;
	}
}

CDlgVDiskInfo::~CDlgVDiskInfo()
{
}

void CDlgVDiskInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_IP, wndTipSvrIP_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_SVRPORT, wndTipSvrDrv_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_CLI_PORT, wndTipCliDrv_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_REFRESH, wndTipRefresh_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_SSDDRV, wndTipSsdDrv_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_SIZE, wndTipSize_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_VDISK_MB, wndTipMB_);

	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_VDISK_IP, wndComboSvrIP_);
	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_VDISK_SVR_PORT, wndComboSvrDrv_);
	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_CLI_PORT, wndComboCliDrv_);
	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_REFRESH, wndComboRefresh_);
	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_VDISK_SSDDRV, wndComboSsdDrv_);
	DDX_Control(pDX, IDC_EDIT_SERVER_INFO_VDISK_SIZE, wndEditSize_);
	DDX_Text(pDX, IDC_EDIT_SERVER_INFO_VDISK_SIZE, textSize_);

	DDX_Control(pDX, IDOK, wndBtnOK_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
}


BEGIN_MESSAGE_MAP(CDlgVDiskInfo, CNonFrameChildDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CDlgVDiskInfo::OnBnClickOK)
	ON_BN_CLICKED(IDCANCEL, &CDlgVDiskInfo::OnBnClickCancel)
END_MESSAGE_MAP()


struct ErrorMsg
{
	void operator()(LPCTSTR tip, LPCTSTR info)
	{
		CMessageBox box(tip, info);
		box.DoModal();
	}
};


bool CDlgVDiskInfo::_CheckDatas()
{
	using namespace i8desk::data_helper;
	int SvrDrv = 0;
	int CliDrv = 0;

	SvrDrv = wndComboSvrDrv_.GetCurSel() + 'C';
	curVDisk_->Port = i8desk::business::VDisk::VDiskPort + (SvrDrv - 'A');
	curVDisk_->LoadType = wndComboRefresh_.GetItemData(wndComboRefresh_.GetCurSel());
	utility::Strcpy(curVDisk_->SvrMode, MAIN_SERVER_SID);

	CliDrv = wndComboCliDrv_.GetCurSel() + 'C';
	curVDisk_->SsdDrv = wndComboSsdDrv_.GetItemData(wndComboSsdDrv_.GetCurSel());

	CString ip;
	wndComboSvrIP_.GetWindowText(ip);
	if( !i8desk::IsValidIP(ip) )
	{
		CMessageBox msgDlg(_T("提示"), _T("IP地址不正确!"));
		msgDlg.DoModal();
		wndComboSvrIP_.SetFocus();
		return false;
	}
	curVDisk_->SoucIP = ::ntohl(i8desk::String2IP((LPCTSTR)ip));


	if( !i8desk::business::VDisk::IsValid(curVDisk_->VID, curVDisk_->SoucIP, SvrDrv, CliDrv, ErrorMsg()) )
	{
		return false;
	}

	curVDisk_->SvrDrv = SvrDrv;
	curVDisk_->CliDrv = CliDrv;
	stdex::tString size = textSize_;
	stdex::ToNumber(curVDisk_->Size, size);

	return true;
}


// CDlgVDiskInfo 消息处理程序

BOOL CDlgVDiskInfo::OnInitDialog()
{
	__super::OnInitDialog();

	// 初始化虚拟盘控件信息
	if( parentSvr_->Ip1 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(parentSvr_->Ip1).c_str());
	if( parentSvr_->Ip2 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(parentSvr_->Ip2).c_str());
	if( parentSvr_->Ip3 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(parentSvr_->Ip3).c_str());
	if( parentSvr_->Ip4 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(parentSvr_->Ip4).c_str());

	for(TCHAR chDrv= _T('C'); chDrv <= _T('Z'); ++chDrv)
	{
		CString text(chDrv);
		wndComboSvrDrv_.AddString(text);
		wndComboCliDrv_.AddString(text);	
	}

	struct AddCombo
	{
		CComboBox &combo_;
		AddCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(int type, LPCTSTR desc)
		{
			int index = combo_.AddString(desc);
			combo_.SetItemData(index, type);
		}
	};
	i8desk::business::VDisk::GetLoadTypes(AddCombo(wndComboRefresh_));

	i8desk::business::VDisk::GetSsdDrv(AddCombo(wndComboSsdDrv_));

	if( isAdd_ )
	{
		SetTitle(_T("添加虚拟盘"));
		SetDlgItemText(IDOK, _T("添加"));

		wndComboSvrIP_.SetCurSel(0);
		wndComboSvrDrv_.SetCurSel(0);
		wndComboCliDrv_.SetCurSel(0);
		wndComboRefresh_.SetCurSel(0);
		wndComboSsdDrv_.SetCurSel(0);
	}
	else
	{
		SetTitle(_T("修改虚拟盘"));
		SetDlgItemText(IDOK, _T("修改"));
	
		wndComboSvrIP_.SelectString(-1, i8desk::IP2String(curVDisk_->SoucIP).c_str());
		
		TCHAR svrDrv[] = {curVDisk_->SvrDrv, 0};
		wndComboSvrDrv_.SelectString(-1, svrDrv);

		TCHAR cliDrv[] = {curVDisk_->CliDrv, 0};
		wndComboCliDrv_.SelectString(-1, cliDrv);

		wndComboRefresh_.SelectString(-1, i8desk::business::VDisk::GetLoadTypeDescByType(curVDisk_->LoadType));

		wndComboSsdDrv_.SelectString(-1, i8desk::business::VDisk::GetSsdDescByType(curVDisk_->SsdDrv));

		
		stdex::tString size;
		stdex::ToString(curVDisk_->Size, size);
		textSize_ = size.c_str();
	}

	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CDlgVDiskInfo::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);
	return TRUE;
}


void CDlgVDiskInfo::OnBnClickOK()
{
	UpdateData(TRUE);

	if( !_CheckDatas() )
		return;

	i8desk::data_helper::VDiskTraits::MapType &vDisks = i8desk::GetDataMgr().GetVDisks();
	if( isAdd_ )
	{
		if( !i8desk::GetDataMgr().AddData(vDisks, curVDisk_->VID, curVDisk_) )
		{
			ErrorMsg()(_T("提示"), _T("添加虚拟盘失败"));
			return;
		}

		parentWnd_->SendMessage(i8desk::ui::WM_ADD_VDISK_MSG, (WPARAM)curVDisk_.get());
	}
	else
	{
		if( !i8desk::GetDataMgr().ModifyData(vDisks, curVDisk_->VID, curVDisk_) )
		{
			ErrorMsg()(_T("提示"), _T("修改虚拟盘失败"));
			return;
		}

		parentWnd_->SendMessage(i8desk::ui::WM_MOD_VDISK_MSG, (WPARAM)curVDisk_.get());
	}

	__super::OnOK();
}


void CDlgVDiskInfo::OnBnClickCancel()
{
	__super::OnCancel();
}
