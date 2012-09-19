// DlgGameVersion.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgGameVersion.h"

#include "../ManagerInstance.h"

// CDlgGameVersion 对话框

IMPLEMENT_DYNAMIC(CDlgGameVersion, CNonFrameChildDlg)

CDlgGameVersion::CDlgGameVersion(CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgGameVersion::IDD, pParent)
{

}

CDlgGameVersion::~CDlgGameVersion()
{
}

void CDlgGameVersion::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_GAME_VERSION_TIP, wndLabel_);
	DDX_Control(pDX, IDC_CHECK_GAME_VERSION, wndCheckOK_);
}


BEGIN_MESSAGE_MAP(CDlgGameVersion, CNonFrameChildDlg)
END_MESSAGE_MAP()


// CDlgGameVersion 消息处理程序

BOOL CDlgGameVersion::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("版权提示"));

	wndLabel_.SetBkColor(RGB(255, 255, 255));
	wndLabel_.SetWindowText(_T("1：I8一点通上属于“网友上传”类的资源，均属热心网友共享或自行上传，并非我公司的资源，我公司未曾提供过此类资源的下载。请上传的网友在提供资源时，务必使用正版游戏软件，否则后果自负。\n\n")
		_T("2：已经安装或使用“网友上传”类资源的用户在无法确实是否为正版资源的，请于24小时内停止使用或共享此类资源，并予以删除，以避免陷入不必要的纠纷之中。\n\n")
		_T("3：光音公司建议所有的包括不限于i8平台网吧用户和渠道等相关操作者使用正版软件，支持正版."));

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgGameVersion::OnCancel()
{
	UpdateData(TRUE);

	if( IsDlgButtonChecked(IDC_CHECK_GAME_VERSION) != 0 )
		i8desk::GetDataMgr().SetOptVal(OPT_M_GAMEVERSION, 0);

	return __super::OnCancel();
}