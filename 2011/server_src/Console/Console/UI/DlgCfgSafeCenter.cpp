// DlgCfgSafeCenter.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgCfgSafeCenter.h"
#include "../ManagerInstance.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/Utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"
#include "../Misc.h"
#include "../../../../include/Win32/System/SystemHelper.hpp"


// DlgCfgSafeCenter 对话框

IMPLEMENT_DYNAMIC(CDlgCfgSafeCenter, CDialog)

CDlgCfgSafeCenter::CDlgCfgSafeCenter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfgSafeCenter::IDD, pParent)
	, iWhichSC_(0)
{

}

CDlgCfgSafeCenter::~CDlgCfgSafeCenter()
{
}

void CDlgCfgSafeCenter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_RADIO_CFGSAFECENTER_START,wndRadioStart_);
	DDX_Control(pDX,IDC_RADIO_CFGSAFECENTER_DISKLESS,wndRadioDiskLess_);
	DDX_Control(pDX,IDC_RADIO_CFGSAFECENTER_DISK,wndRadioDisk_);
	DDX_Control(pDX,IDC_RADIO_CFGSAFECENTER_STOP,wndRadioStop_);

	DDX_Control(pDX,IDC_BUTTON_CFGSAFECENTER_SET,wndbtnSet_);

	DDX_Control(pDX,IDC_STATIC_CFGSAFECENTER_DISKLESS,wndLabelDiskless_);
	DDX_Control(pDX,IDC_STATIC_CFGSAFECENTER_DISK,wndLabelDisk_);



}


BEGIN_MESSAGE_MAP(CDlgCfgSafeCenter, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_RADIO_CFGSAFECENTER_START,	&CDlgCfgSafeCenter::OnBnClickedRadioStart)
	ON_BN_CLICKED(IDC_RADIO_CFGSAFECENTER_DISKLESS, &CDlgCfgSafeCenter::OnBnClickedRadioDiskLess)
	ON_BN_CLICKED(IDC_RADIO_CFGSAFECENTER_DISK,		&CDlgCfgSafeCenter::OnBnClickedRadioDisk)
	ON_BN_CLICKED(IDC_RADIO_CFGSAFECENTER_STOP,		&CDlgCfgSafeCenter::OnBnClickedRadioStop)
	ON_BN_CLICKED(IDC_BUTTON_CFGSAFECENTER_SET,		&CDlgCfgSafeCenter::OnBnClickedButtonSet)
	ON_MESSAGE(WM_MSG_OK, &CDlgCfgSafeCenter::OnApplyMessage)

END_MESSAGE_MAP()


// WZDlgSafeCenter 消息处理程序
BOOL CDlgCfgSafeCenter::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT lf;
	this->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	font_.CreateFontIndirect(&lf);

	wndRadioStart_.SetFont(&font_);
	wndRadioStop_.SetFont(&font_);

	ReadData();

	return TRUE;
}

void CDlgCfgSafeCenter::ReadData()
{
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
}

void CDlgCfgSafeCenter::OnBnClickedRadioStart()
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

void CDlgCfgSafeCenter::OnBnClickedRadioDiskLess()
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

void CDlgCfgSafeCenter::OnBnClickedRadioDisk()
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

void CDlgCfgSafeCenter::OnBnClickedRadioStop()
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


void CDlgCfgSafeCenter::OnBnClickedButtonSet()
{
	SafeCenterConfig();
}


void CDlgCfgSafeCenter::SafeCenterConfig()
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

LRESULT CDlgCfgSafeCenter::OnApplyMessage(WPARAM wParam, LPARAM lParam)
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

	return TRUE;
}

BOOL CDlgCfgSafeCenter::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(*pDC, rcClient);
	memDC.GetDC().FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
		RGB(255, 255, 255));

	return TRUE;
}