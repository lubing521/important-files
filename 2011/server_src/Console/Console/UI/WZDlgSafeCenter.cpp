// WZDlgSafeCenter.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgSafeCenter.h"
#include "../ManagerInstance.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../MessageBox.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/Win32/System/SystemHelper.hpp"

// WZDlgSafeCenter 对话框

IMPLEMENT_DYNAMIC(WZDlgSafeCenter, CDialog)

WZDlgSafeCenter::WZDlgSafeCenter(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgSafeCenter::IDD, pParent)
	, iWhichSC_(0)
{

}

WZDlgSafeCenter::~WZDlgSafeCenter()
{
}

void WZDlgSafeCenter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_RADIO_WZ_SAFECENTER_START,wndRadioStart_);
	DDX_Control(pDX,IDC_RADIO_WZ_SAFECENTER_DISKLESS,wndRadioDiskLess_);
	DDX_Control(pDX,IDC_RADIO_WZ_SAFECENTER_DISK,wndRadioDisk_);
	DDX_Control(pDX,IDC_RADIO_WZ_SAFECENTER_STOP,wndRadioStop_);
	DDX_Control(pDX,IDC_BUTTON_WZ_SAFECENTER_SET,wndbtnSet_);
	DDX_Control(pDX, IDC_STATIC_WZ_SAFECENTER_DISKLESS, wndLabelBasic_);
	DDX_Control(pDX, IDC_STATIC_WZ_SAFECENTER_DISK, wndLabelAdvance_);
}


BEGIN_MESSAGE_MAP(WZDlgSafeCenter, CDialog)
	ON_BN_CLICKED(IDC_RADIO_WZ_SAFECENTER_START,	&WZDlgSafeCenter::OnBnClickedRadioStart)
	ON_BN_CLICKED(IDC_RADIO_WZ_SAFECENTER_DISKLESS, &WZDlgSafeCenter::OnBnClickedRadioDiskLess)
	ON_BN_CLICKED(IDC_RADIO_WZ_SAFECENTER_DISK,		&WZDlgSafeCenter::OnBnClickedRadioDisk)
	ON_BN_CLICKED(IDC_RADIO_WZ_SAFECENTER_STOP,		&WZDlgSafeCenter::OnBnClickedRadioStop)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SAFECENTER_SET,		&WZDlgSafeCenter::OnBnClickedButtonSet)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgSafeCenter 消息处理程序
BOOL WZDlgSafeCenter::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT lf;
	this->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	font_.CreateFontIndirect(&lf);

	wndRadioStart_.SetFont(&font_);
	wndRadioStop_.SetFont(&font_);
	if(!i8desk::GetDataMgr().GetOptVal(OPT_M_SAFECENTER, 0))
		OnBnClickedRadioStop();
	else
	{
		iWhichSC_ = i8desk::GetDataMgr().GetOptVal(OPT_M_WHICHONE_SC, 0);
		
		switch (iWhichSC_)  
		{
		case i8desk::UNKOWN:
			OnBnClickedRadioStop();
			break;
		case i8desk::DISKLESS:
			OnBnClickedRadioDiskLess();
			break;
		case i8desk::KONGHE:
			OnBnClickedRadioDisk();
			break;
		}
	}

	return TRUE;
}

void WZDlgSafeCenter::OnBnClickedRadioStart()
{
	iWhichSC_ = i8desk::KONGHE;

	wndbtnSet_.EnableWindow(TRUE);
	wndRadioDisk_.EnableWindow(TRUE);
	wndRadioDiskLess_.EnableWindow(TRUE);

	wndRadioStart_.SetCheck(TRUE);
	wndRadioDisk_.SetCheck(TRUE);
	wndRadioDiskLess_.SetCheck(FALSE);
	wndRadioStop_.SetCheck(FALSE);

	wndRadioStart_.Invalidate(FALSE);
	wndRadioStop_.Invalidate(FALSE);
	wndRadioDisk_.Invalidate(FALSE);
	wndRadioDiskLess_.Invalidate(FALSE);
}

void WZDlgSafeCenter::OnBnClickedRadioDiskLess()
{
	iWhichSC_ = i8desk::DISKLESS;
	wndbtnSet_.EnableWindow(TRUE);

	wndRadioStart_.SetCheck(TRUE);
	wndRadioDiskLess_.SetCheck(TRUE);
	wndRadioDisk_.SetCheck(FALSE);
	wndRadioStop_.SetCheck(FALSE);

	wndRadioStart_.Invalidate(FALSE);
	wndRadioStop_.Invalidate(FALSE);
	wndRadioDisk_.Invalidate(FALSE);
	wndRadioDiskLess_.Invalidate(FALSE);
}

void WZDlgSafeCenter::OnBnClickedRadioDisk()
{
	iWhichSC_ = i8desk::KONGHE;
	wndbtnSet_.EnableWindow(TRUE);

	wndRadioStart_.SetCheck(TRUE);
	wndRadioDisk_.SetCheck(TRUE);
	wndRadioDiskLess_.SetCheck(FALSE);
	wndRadioStop_.SetCheck(FALSE);

	wndRadioStart_.Invalidate(FALSE);
	wndRadioStop_.Invalidate(FALSE);
	wndRadioDisk_.Invalidate(FALSE);
	wndRadioDiskLess_.Invalidate(FALSE);

}

void WZDlgSafeCenter::OnBnClickedRadioStop()
{
	iWhichSC_ = i8desk::UNKOWN;
	wndRadioDisk_.EnableWindow(FALSE);
	wndRadioDiskLess_.EnableWindow(FALSE);
	wndbtnSet_.EnableWindow(FALSE);

	wndRadioStop_.SetCheck(TRUE);
	wndRadioStart_.SetCheck(FALSE);
	wndRadioDisk_.SetCheck(FALSE);
	wndRadioDiskLess_.SetCheck(FALSE);

	wndRadioStart_.Invalidate(FALSE);
	wndRadioStop_.Invalidate(FALSE);
	wndRadioDisk_.Invalidate(FALSE);
	wndRadioDiskLess_.Invalidate(FALSE);

}

void WZDlgSafeCenter::OnBnClickedButtonSet()
{
	SafeCenterConfig();
}


void WZDlgSafeCenter::SafeCenterConfig()
{
	if (!i8desk::IsRunOnServer())
	{
		CMessageBox msgDlg(_T("提示"),_T("无法远程配置和启动安全中心服务端\n"));
		msgDlg.DoModal();
		return ;
	}

	stdex::tString szExe;
	if ( iWhichSC_ == i8desk::DISKLESS)
	{
		szExe = utility::GetAppPath() + _T("SafeSvr\\Server\\UndiskSafe.exe");
		win32::system::KillProcess(_T("KHPrtSvr.exe"));
	}
	else 
	{
		szExe = utility::GetAppPath() + _T("SafeSvr\\KHPrtSvr.exe");
		win32::system::KillProcess(_T("UndiskSafe.exe"));
	}

	win32::system::StartProcess(szExe.c_str());
}

void WZDlgSafeCenter::OnComplate()
{
	i8desk::GetDataMgr().SetOptVal(OPT_M_WHICHONE_SC, iWhichSC_);

	if( wndRadioStop_.GetCheck() == TRUE )
	{
		i8desk::GetDataMgr().SetOptVal(OPT_M_SAFECENTER, 0 );
		win32::system::KillProcess(_T("KHPrtSvr.exe"));
		win32::system::KillProcess(_T("UndiskSafe.exe"));
	}
	else
	{
		stdex::tString szExe; 
		i8desk::GetDataMgr().SetOptVal(OPT_M_SAFECENTER, 1 );
		if ( iWhichSC_ == i8desk::DISKLESS)
		{
			win32::system::KillProcess(_T("KHPrtSvr.exe"));
			szExe = utility::GetAppPath() + _T("SafeSvr\\Server\\UndiskSafe.exe");
		}
		else
		{
			win32::system::KillProcess(_T("UndiskSafe.exe"));
			szExe = utility::GetAppPath() + _T("SafeSvr\\KHPrtSvr.exe");
		}

		win32::system::StartProcess(szExe.c_str());
	}

}

void WZDlgSafeCenter::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgSafeCenter::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;

}