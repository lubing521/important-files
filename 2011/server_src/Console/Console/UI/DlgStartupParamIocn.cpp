// DlgStartupParamIocn.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartupParamIocn.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupParamIocn 对话框

IMPLEMENT_DYNAMIC(CDlgStartupParamIocn, CDialog)

CDlgStartupParamIocn::CDlgStartupParamIocn(CWnd* pParent /*=NULL*/)
: CDialog(CDlgStartupParamIocn::IDD, pParent)
	, shortcutName_(_T(""))
	, shortcutFile_(_T(""))
	, shortcutParam_(_T(""))
	, iconPath_(_T(""))
{

}

CDlgStartupParamIocn::~CDlgStartupParamIocn()
{
}

void CDlgStartupParamIocn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_ICON_NAME, shortcutName_);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_ICON_FILE, shortcutFile_);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_ICON_PARAM, shortcutParam_);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_ICON_ICON, iconPath_);

	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_ICON_NAME, wndEditshortcutName_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_ICON_FILE, wndEditshortcutFile_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_ICON_PARAM, wndEditshortcutParam_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_ICON_ICON, wndEditiconPath_);

	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_ICON_NAME, wndLabelshortcutName_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_ICON_FILE, wndLabelshortcutFile_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_ICON_PARAM, wndLabelshortcutParam_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_ICON_ICON, wndLabeliconPath_);

	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_ICON_NAME_BROW, wndBtnNameBrow_);
	DDX_Control(pDX, IDC_BUTTONC_EDIT_STARTUP_PARAM_ICON_FILE_BROW, wndBtnFileBrow_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_ICON_ICON_BROW, wndBtnIconBrow_);
}


BEGIN_MESSAGE_MAP(CDlgStartupParamIocn, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_ICON_NAME_BROW, &CDlgStartupParamIocn::OnBnClickedButtonStartupParamIconNameBrow)
	ON_BN_CLICKED(IDC_BUTTONC_EDIT_STARTUP_PARAM_ICON_FILE_BROW, &CDlgStartupParamIocn::OnBnClickedButtoncEditStartupParamIconFileBrow)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_ICON_ICON_BROW, &CDlgStartupParamIocn::OnBnClickedButtonStartupParamIconIconBrow)
END_MESSAGE_MAP()



bool CDlgStartupParamIocn::GetParam(stdex::tString &param)
{
	UpdateData(TRUE);
	
	if( shortcutName_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("请输入快捷名称"));
		msgDlg.DoModal();
		return false;
	}

	if( shortcutFile_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("请输入文件名称"));
		msgDlg.DoModal();
		return false;
	}


	CString tmp = shortcutName_ + _T("|") + shortcutFile_ + _T("|") + 
		iconPath_ + _T("|") + shortcutParam_;
	param = (LPCTSTR)tmp;

	return true;
}

void CDlgStartupParamIocn::SetParam(const stdex::tString &param)
{
	std::vector<stdex::tString> params;
	stdex::Split(params, param, _T('|'));

	shortcutName_ = params[0].c_str();
	shortcutFile_ = params[1].c_str();
	iconPath_	  = params[2].c_str();
	if( params.size() > 3 )
		shortcutParam_ = params[3].c_str();

	UpdateData(FALSE);
}

// CDlgStartupParamIocn 消息处理程序

BOOL CDlgStartupParamIocn::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgStartupParamIocn::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);


	return TRUE;
}
void CDlgStartupParamIocn::OnBnClickedButtonStartupParamIconNameBrow()
{
	UpdateData(TRUE);

	if( theApp.GetShellManager()->BrowseForFolder(selPath_, this, shortcutName_, _T("选择快捷名称:")) )
	{
		if( selPath_.IsEmpty() )
			return ;
		size_t pos = 0;
		stdex::tString path = selPath_;
		pos = path.find_last_of(_T('\\'));
		path = path.substr(pos + 1, path.length());

		shortcutName_ = path.c_str();
	}

	UpdateData(FALSE);
}

void CDlgStartupParamIocn::OnBnClickedButtoncEditStartupParamIconFileBrow()
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	fileDlg.m_ofn.lpstrInitialDir = selPath_;
	if( fileDlg.DoModal() == IDOK )
		shortcutFile_ = fileDlg.GetPathName();
	else
		return;

	UpdateData(FALSE);
}

void CDlgStartupParamIocn::OnBnClickedButtonStartupParamIconIconBrow()
{
	UpdateData(TRUE);

	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	fileDlg.m_ofn.lpstrInitialDir = selPath_;
	if( fileDlg.DoModal() == IDOK )
		iconPath_ = fileDlg.GetPathName();
	else
		return;

	UpdateData(FALSE);
}
