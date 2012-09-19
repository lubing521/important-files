// DlgStartupParamDel.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgStartupParamDel.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgStartupParamDel 对话框

IMPLEMENT_DYNAMIC(CDlgStartupParamDel, CDialog)

CDlgStartupParamDel::CDlgStartupParamDel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStartupParamDel::IDD, pParent)
	, path_(_T(""))
{

}

CDlgStartupParamDel::~CDlgStartupParamDel()
{
}

void CDlgStartupParamDel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STARTUP_PARAM_DEL_DIR, path_);
	DDX_Control(pDX, IDC_EDIT_STARTUP_PARAM_DEL_DIR, wndEditpath_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_DEL_PARAM, wndLabelParam_);
	DDX_Control(pDX, IDC_STATIC_STARTUP_PARAM_DEL_DIR, wndLabelDir_);
	DDX_Control(pDX, IDC_BUTTON_STARTUP_PARAM_DEL_DIR_BROW, wndBtnDelBrow_);
	DDX_Control(pDX, IDC_CHECK_STARTUP_PARAM_DEL_PENET, wndChkPenet_);
}


BEGIN_MESSAGE_MAP(CDlgStartupParamDel, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP_PARAM_DEL_DIR_BROW, &CDlgStartupParamDel::OnBnClickedButtonStartupParamDelDirBrow)
END_MESSAGE_MAP()



bool CDlgStartupParamDel::GetParam(stdex::tString &param)
{
	UpdateData(TRUE);

	if( path_.IsEmpty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("请输入正确路径"));
		msgDlg.DoModal();
		return false;
	}

	bool isProto = wndChkPenet_.GetCheck() == BST_CHECKED;

	stdex::tStringstream os;
	os << isProto << _T("|") << (LPCTSTR)path_;

	param = os.str();

	return true;
}

void CDlgStartupParamDel::SetParam(const stdex::tString &param)
{
	std::vector<stdex::tString> params;
	stdex::Split(params, param, _T('|'));

	assert(params.size() >= 2);

	bool isProto = stdex::ToNumber<int>(params[0]) == 1;
	if( isProto )
		wndChkPenet_.SetCheck(BST_CHECKED);
	else 
		wndChkPenet_.SetCheck(BST_UNCHECKED);

	path_ = params[1].c_str();

	UpdateData(FALSE);
}

// CDlgStartupParamDel 消息处理程序

BOOL CDlgStartupParamDel::OnInitDialog()
{
	__super::OnInitDialog();
	wndChkPenet_.SetCheck(BST_CHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CDlgStartupParamDel::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;
}

void CDlgStartupParamDel::OnBnClickedButtonStartupParamDelDirBrow()
{
	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	fileDlg.m_ofn.lpstrInitialDir = path_;
	if( fileDlg.DoModal() == IDOK )
		path_ = fileDlg.GetPathName();
	else
		return;

	UpdateData(FALSE);
}
