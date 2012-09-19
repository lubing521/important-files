// DlgStartupParamDesktop.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartupParamDesktop.h"


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"

#include "../ManagerInstance.h"
#include "../ui/Skin/SkinMgr.h"
#include "../Business/BootTaskBusiness.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupParamDesktop 对话框

static const stdex::tString RandomWallPaper = _T("随机换壁纸");

IMPLEMENT_DYNAMIC(CDlgStartupParamDesktop, CDialog)

CDlgStartupParamDesktop::CDlgStartupParamDesktop(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStartupParamDesktop::IDD, pParent)
{

}

CDlgStartupParamDesktop::~CDlgStartupParamDesktop()
{
}

void CDlgStartupParamDesktop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_DESKTOP_OPEN, wndBtnOpen_);
	DDX_Control(pDX, IDC_COMBO_STARTUP_PARAM_DESKTOP_DIR, wndComboFile_);
	DDX_Control(pDX, IDC_COMBO_STARTUP_PARAM_DESKTOP_TYPE, wndComboParam_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_DESKTOP_DIR, wndLabelFile_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_DESKTOP_TYPE, wndLabelParam_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_DESKTOP_CONTENT, wndLabelContent_);

}


BEGIN_MESSAGE_MAP(CDlgStartupParamDesktop, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_DESKTOP_OPEN, &CDlgStartupParamDesktop::OnBnClickedButtonStartupParamDesktopOpen)
END_MESSAGE_MAP()


namespace
{
	const struct
	{
		size_t index;
		LPCTSTR name;
	}openType[] = 
	{
		{ 0, _T("居中") },
		{ 1, _T("平铺") },
		{ 2, _T("拉伸") }
	};


}

bool CDlgStartupParamDesktop::GetParam(stdex::tString &param)
{
	int nSel = 0;
	nSel = wndComboParam_.GetCurSel();
	
	CString path;
	wndComboFile_.GetWindowText(path);
	if( path.IsEmpty() )
		return false;
	stdex::tOstringstream os;
	os << nSel;

	if ( utility::Strcmp(path,RandomWallPaper.c_str()) == 0 )
	{
		std::vector<stdex::tString> files;
		i8desk::business::BootTask::GetImageFileName(files);
		if( files.size() == 0 )
		{
			stdex::tString msg = _T("没有找到合适的图片，请放入图片! ");
			CMessageBox msgDlg(_T("随机分配壁纸"), msg);
			msgDlg.DoModal();
			return false;
		}
		for( std::vector<stdex::tString>::iterator iter = files.begin();
			iter != files.end(); ++iter )
			os << _T("|") << _T("Data\\WallPaper\\") << iter->c_str();
	}
	else
		os <<  _T("|") << _T("Data\\WallPaper\\") << (LPCTSTR)path ;

	param = os.str();

	return true;
}

void CDlgStartupParamDesktop::SetParam(const stdex::tString &param)
{
	std::vector<stdex::tString> params;
	stdex::Split(params, param, _T('|'));

	wndComboParam_.SetCurSel(stdex::ToNumber<int>(params[0]));

	if( params.size() > 2)
		wndComboFile_.SelectString(-1,RandomWallPaper.c_str());
	else
	{
		stdex::tString desc = params[1];
		desc = stdex::Split(desc, _T('\\'), 2);
		desc = desc.substr(0,desc.length()-1);
		wndComboFile_.SelectString(-1, desc.c_str());
	}
		
}

// CDlgStartupParamDesktop 消息处理程序

void CDlgStartupParamDesktop::OnBnClickedButtonStartupParamDesktopOpen()
{
	if(!i8desk::IsRunOnServer())
	{
		CMessageBox  msgbox(_T("提示"), _T("远程登录控制台时, 无法进行此操作！"));
		msgbox.DoModal();
		return;
	}

	stdex::tOstringstream path;
	path << utility::GetAppPath() <<  _T("Data\\WallPaper");

	CreateDirectory(path.str().c_str(), NULL);
	ShellExecute(m_hWnd, _T("open"), path.str().c_str(), _T(""), NULL, SW_SHOWNORMAL);

}

BOOL CDlgStartupParamDesktop::OnInitDialog()
{
	__super::OnInitDialog();

	for(size_t i = 0; i != _countof(openType); ++i)
	{
		wndComboParam_.InsertString(openType[i].index, openType[i].name);
	}
	wndComboParam_.SetCurSel(0);

	wndComboFile_.AddString(RandomWallPaper.c_str());

	std::vector<stdex::tString> files;
	i8desk::business::BootTask::GetImageFileName(files);
	for( std::vector<stdex::tString>::iterator iter = files.begin();
		iter != files.end(); ++iter )
		wndComboFile_.AddString(iter->c_str());

	wndComboFile_.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgStartupParamDesktop::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;
}