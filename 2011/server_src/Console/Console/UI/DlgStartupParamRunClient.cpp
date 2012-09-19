// DlgStartupRunClient.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartupParamRunClient.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/Utility/utility.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupRunClient 对话框

IMPLEMENT_DYNAMIC(CDlgStartupParamRunClient, CDialog)

CDlgStartupParamRunClient::CDlgStartupParamRunClient(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStartupParamRunClient::IDD, pParent)
	, svrPath_(_T(""))
	, cliPath_(_T(""))
{

}

CDlgStartupParamRunClient::~CDlgStartupParamRunClient()
{
}

void CDlgStartupParamRunClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_RUN_SVR_PATH, svrPath_);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_RUN_CLI_PATH, cliPath_);
	DDX_Text(pDX, IDC_EDIT_STARTUP_INFO_CLIENT_FILE, cliFile_);

	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_RUN_SVR_PATH, wndEditSvrPath_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_RUN_CLI_PATH, wndEditDstPath_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNCLI_FILE, wndLabelFile_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNCLI_PARAM, wndLabelParam_);

	DDX_Control(pDX, IDC_BUTTON_STARTUP_SRC_PATH, wndBtnSrcPath_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_DST_PATH, wndBtnDstPath_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_DST_FILE, wndBtnDstFile_);
	DDX_Control(pDX, IDC_CHECK_STARTUP_INFO_ISRUN, wndCheckIsRun_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_INFO_CLIENT_FILE, wndEditFile_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_RUNCLI_PARAM_PARAM, wndLabelRunParam_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_RUN_CLI_PARAM, wndEditRunParam_);
}


BEGIN_MESSAGE_MAP(CDlgStartupParamRunClient, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_SRC_PATH, &CDlgStartupParamRunClient::OnBnClickedButtonStartupSrcPath)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_DST_PATH, &CDlgStartupParamRunClient::OnBnClickedButtonStartupDstPath)
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_DST_FILE, &CDlgStartupParamRunClient::OnBnClickedButtonStartupDstFile)
	ON_BN_CLICKED(IDC_CHECK_STARTUP_INFO_ISRUN, &CDlgStartupParamRunClient::OnBnClickedCheckStartupInfoIsrun)

	ON_MESSAGE(WM_MAKEIDX, &CDlgStartupParamRunClient::OnMessage)
END_MESSAGE_MAP()


bool CDlgStartupParamRunClient::GetParam(stdex::tString &param)
{
	UpdateData(TRUE);

	if( svrPath_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("服务器文件路径必须填写"));
		msgDlg.DoModal();
		wndEditSvrPath_.SetFocus();
		return false;
	}

	if( cliPath_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("客户机文件路径必须填写"));
		msgDlg.DoModal();
		wndEditDstPath_.SetFocus();
		return false;
	}

	bool isRun = wndCheckIsRun_.GetCheck() == BST_CHECKED;
	if( isRun )
	{
		if( cliFile_.IsEmpty() )
		{
			CMessageBox msgDlg(_T("提示"),_T("执行客户机文件必须填写"));
			msgDlg.DoModal();
			wndEditFile_.SetFocus();
			return false;
		}
	}

	CString runParam;
	wndEditRunParam_.GetWindowText(runParam);

	stdex::tOstringstream os;
	os << 1						<< _T("|") 
		<< (isRun ? 1 : 0)		<< _T("|") 
		<< 1					<< _T("|") 
		<< (LPCTSTR)svrPath_	<< _T("|")
		<< (LPCTSTR)cliPath_	<< _T("|")
		<< (LPCTSTR)cliFile_	<< _T("|")
		<< (LPCTSTR)runParam;

	param = os.str();

	if(!makeIdx_.StartMakeIdx((LPCTSTR)svrPath_,true,this->GetSafeHwnd()))
	{
		CMessageBox msgDlg(_T("提示"),_T("制作索引启动不成功!"));
		msgDlg.DoModal();
		return false;
	}

	if (!makeIdx_.StopMakeIdx(true))
	{
		return false;
	}

	return true;
}

void CDlgStartupParamRunClient::SetParam(const stdex::tString &param)
{
	std::vector<stdex::tString> params;
	stdex::Split(params, param, _T('|'));

	assert(params.size() >= 6);
	// params[0] 为是否需要下载
	assert(stdex::ToNumber<int>((params[0])) == 1);
	svrPath_ = params[3].c_str();
	cliPath_ = params[4].c_str();
	cliFile_ = params[5].c_str();
	
	if( !cliFile_.IsEmpty() )
		wndCheckIsRun_.SetCheck(BST_CHECKED);
	else
		wndCheckIsRun_.SetCheck(BST_UNCHECKED);

	if( params.size() == 7 )
		wndEditRunParam_.SetWindowText(params[6].c_str());

	UpdateData(FALSE);
}

// CDlgStartupRunClient 消息处理程序

BOOL CDlgStartupParamRunClient::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgStartupParamRunClient::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;
}

void CDlgStartupParamRunClient::OnBnClickedButtonStartupSrcPath()
{
	static stdex::tString initPath = utility::GetAppPath() + _T("Data\\BootExec");

	CString path;
	theApp.GetShellManager()->BrowseForFolder(path, this, initPath.c_str(), _T("选择目录:"));
	if( !path.IsEmpty() )
	{
		wndEditSvrPath_.SetWindowText(path);
	}
}

void CDlgStartupParamRunClient::OnBnClickedButtonStartupDstPath()
{
	CString path;
	theApp.GetShellManager()->BrowseForFolder(path, this, _T(""), _T("选择目录:"));

	if( !path.IsEmpty() )
	{
		wndEditDstPath_.SetWindowText(path);
	}
}

void CDlgStartupParamRunClient::OnBnClickedButtonStartupDstFile()
{
	CString srcPath;
	wndEditSvrPath_.GetWindowText(srcPath);

	CString initPath = srcPath + _T("\\run.exe");
	WTL::CFileDialog dlg(TRUE, 0, initPath);
	if( IDOK == dlg.DoModal() )
	{
		CString text = dlg.m_ofn.lpstrFile;

		text = text.Mid(srcPath.GetLength() + 1);
		wndEditFile_.SetWindowText(text);
	}
}

void CDlgStartupParamRunClient::OnBnClickedCheckStartupInfoIsrun()
{
	if( wndCheckIsRun_.GetCheck() == BST_CHECKED )
	{
		wndEditFile_.EnableWindow(TRUE);
		wndBtnDstFile_.EnableWindow(TRUE);
	}
	else
	{
		wndEditFile_.EnableWindow(FALSE);
		wndBtnDstFile_.EnableWindow(FALSE);
	}
}



LRESULT CDlgStartupParamRunClient::OnMessage(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case IDX_STATUS_ERROR:
		{
			exit_.SetEvent();
			isMakeIdx_ = false;
			makeIdx_.StopMakeIdx();

			CMessageBox msgDlg(_T("提示"),_T("制作索引启动不成功!"));
			msgDlg.DoModal();

		}
		break;
	case IDX_STATUS_FINISH:
		exit_.SetEvent();
		isMakeIdx_ = true;
		makeIdx_.StopMakeIdx();

		break;
	default:
		break;
	}

	return 0;
}