// DlgStartupRunSvr.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "../../../../include/ui/wtl/atldlgs.h"

#include "DlgStartupParamRunSvr.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/Utility/utility.h"

#include "../ManagerInstance.h"
#include "../MessageBox.h"
#include "../ui/Skin/SkinMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupRunSvr 对话框

IMPLEMENT_DYNAMIC(CDlgStartupParamRunSvr, CDialog)

CDlgStartupParamRunSvr::CDlgStartupParamRunSvr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStartupParamRunSvr::IDD, pParent)
{

}

CDlgStartupParamRunSvr::~CDlgStartupParamRunSvr()
{
}

void CDlgStartupParamRunSvr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_STARTUP_RUNSVR_PATH, wndEditPath_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_RUNSVR_PATH_PARAM, wndEditParam_);

	DDX_Control(pDX, IDC_CHECK_STARTUP_PARAM_RUNSVR_PENET, wndCheckisPenetrated_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNSVR_FILE, wndLabelFile_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNSVR_FILEZ_PARAM, wndLabelParam_);

	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_RUNSVR_OPEN, wndBtnOpen_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNSVR_FILE_CLI_PATH, wndLabelDstPath_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_RUNSVR_CLI_PATH, wndEditCliPath_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_RUNSVR_OPEN_CLI_PATH, wndBtnCliPath_);
}


BEGIN_MESSAGE_MAP(CDlgStartupParamRunSvr, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_RUNSVR_OPEN, &CDlgStartupParamRunSvr::OnBnClickedButtonStartupParamRunsvrOpen)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_RUNSVR_OPEN_CLI_PATH, &CDlgStartupParamRunSvr::OnBnClickedButtonStartupParamRunsvrOpenCliPath)
END_MESSAGE_MAP()

namespace 
{
	const stdex::tString BootExe = _T("Data\\BootExec");

}


bool CDlgStartupParamRunSvr::GetParam(stdex::tString &param)
{
	UpdateData(TRUE);

	CString text;
	wndEditPath_.GetWindowText(text);

	if( text.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"), _T("执行服务器文件路径必须填写"));
		msgDlg.DoModal();
		wndEditPath_.SetFocus();
		return false;
	}

	CString runParam;
	wndEditParam_.GetWindowText(runParam);

	CString runCliPath;
	wndEditCliPath_.GetWindowText(runCliPath);

	bool isProto = wndCheckisPenetrated_.GetCheck() == BST_CHECKED;

	stdex::tOstringstream os;
	os << 1						<< _T("|") 
		<< 1L					<< _T("|") 
		<< (isProto ? 1 : 0)	<< _T("|") 
		<< (LPCTSTR)text		<< _T("|") 
		<< (LPCTSTR)runCliPath	<< _T("|")
		<< (LPCTSTR)runParam;
	
	param = os.str();

	return true;
}

void CDlgStartupParamRunSvr::SetParam(const stdex::tString &param)
{
	std::vector<stdex::tString> params;
	stdex::Split(params, param, _T('|'));

	assert(params.size() >= 5);
	// params[0] 为是否需要下载
	assert(stdex::ToNumber<int>((params[0])) == 1);
	bool isProto = stdex::ToNumber<int>(params[2]) == 1;
	wndEditPath_.SetWindowText(params[3].c_str());
	wndEditCliPath_.SetWindowText(params[4].c_str());
	if (params.size() != 5)
		wndEditParam_.SetWindowText(params[5].c_str());


	if( isProto )
		wndCheckisPenetrated_.SetCheck(BST_CHECKED);
	
	UpdateData(FALSE);
}


// CDlgStartupRunSvr 消息处理程序

void CDlgStartupParamRunSvr::OnBnClickedButtonStartupParamRunsvrOpen()
{
	stdex::tOstringstream path;
	path << utility::GetAppPath() <<  BootExe.c_str();

	CreateDirectory(path.str().c_str(), NULL);
	WTL::CFileDialog dlg(TRUE);
	if( IDOK == dlg.DoModal() )
	{
		CString text = dlg.m_ofn.lpstrFile;

		wndEditPath_.SetWindowText(text);
	}
}

void CDlgStartupParamRunSvr::OnBnClickedButtonStartupParamRunsvrOpenCliPath()
{
	CString path;
	theApp.GetShellManager()->BrowseForFolder(path, this, _T(""), _T("选择目录:"));

	if( !path.IsEmpty() )
	{
		wndEditCliPath_.SetWindowText(path);
	}
}


BOOL CDlgStartupParamRunSvr::OnInitDialog()
{
	__super::OnInitDialog();

	wndCheckisPenetrated_.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgStartupParamRunSvr::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;
}
