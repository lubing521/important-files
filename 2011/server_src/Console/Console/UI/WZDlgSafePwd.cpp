// WZDlgSafePwd.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgSafePwd.h"
#include "../ManagerInstance.h"
#include "../MessageBox.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"

#include "../DlgCheckUser.h"
#include "DlgCfgUserSet.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../ManagerInstance.h"

// WZDlgSafePwd 对话框

IMPLEMENT_DYNAMIC(WZDlgSafePwd, CDialog)

WZDlgSafePwd::WZDlgSafePwd(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgSafePwd::IDD, pParent)
{

}

WZDlgSafePwd::~WZDlgSafePwd()
{
}

void WZDlgSafePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WZ_SAFE_CONLOGINPWD, wndEditConsolePwd_);
	DDX_Text(pDX, IDC_EDIT_WZ_SAFE_CONLOGINPWD, consolepwd_);
	DDX_Control(pDX, IDC_EDIT_WZ_SAFE_NEWPWD, wndEditMenuPwd_);
	DDX_Text(pDX, IDC_EDIT_WZ_SAFE_NEWPWD, menupwd_);
	DDX_Control(pDX, IDC_CHECK_WZ_SAFE_MENUPWD, wndMenuPwd_);
	DDX_Control(pDX, IDC_STATIC_WZ_SAFE_CONLOGINPWD, wndLabelConsolePsw_);
	DDX_Control(pDX, IDC_STATIC_WZ_PSW_CONSOLE_DEF, wndLabelConsolePswDef_);
	DDX_Control(pDX, IDC_STATIC_WZ_PSW_MENU_DEF, wndLabelMenuPswDef_);
	DDX_Control(pDX, IDC_STATIC_WZ_PSW_NETBAR_ID, wndLabelNID_);
	DDX_Control(pDX, IDC_LABEL_WZ_PSW_NETBAR_ID, wndNetbarID_);
	DDX_Control(pDX, IDC_WZ_PSW_NETBAR_DATEEND, wndLabelDateEnd_);
	DDX_Control(pDX, IDC_WZ_PSW_NETBAR_BINDNAME, wndLabelParentName_);
	DDX_Control(pDX, IDC_WZ_LABEL_PSW_NETBAR_BINDNAME, wndParentName_);
	DDX_Control(pDX, IDC_STATIC_WZ_PSW_NETBAR_PARENTID, wndLabelParentID_);
	DDX_Control(pDX, IDC_WZ_LABEL_PSW_NETBAR_BINDID, wndParentID_);
	DDX_Control(pDX, IDC_BUTTON_WZ_PSW_NETBAR_MODIFY, wndBtnModify_);
	DDX_Control(pDX, IDC_LABEL_WZ_PSW_NETBAR_DATEEND, wndDateEnd_);
	DDX_Control(pDX, IDC_STATIC_WZ_PSW_NETBAR_NAME, wndLabelNetbarInfo_);
	DDX_Control(pDX, IDC_STATIC_WZ_SAFE_AUTHORIZE, wndLabelPswInfo_);
}


BEGIN_MESSAGE_MAP(WZDlgSafePwd, CDialog)
	ON_BN_CLICKED(IDC_CHECK_WZ_SAFE_MENUPWD, &WZDlgSafePwd::OnBnClickedCheckMenuPwd)
	ON_EN_CHANGE(IDC_EDIT_WZ_SAFE_ACCOUNT, &WZDlgSafePwd::OnEnChangeUsername)
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BUTTON_WZ_PSW_NETBAR_MODIFY, &WZDlgSafePwd::OnBnClickedButtonWzPswNetbarModify)
END_MESSAGE_MAP()


// WZDlgSafePwd 消息处理程序
BOOL WZDlgSafePwd::OnInitDialog()
{
	CDialog::OnInitDialog();

	stdex::tString str ;
	
	// 设置菜单高级设置密码
	if( wndMenuPwd_.GetCheck() )
		wndEditMenuPwd_.EnableWindow(TRUE);
	else
		wndEditMenuPwd_.EnableWindow(FALSE);

	wndNetbarID_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_NID, _T("")).c_str());
	wndDateEnd_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_DATEEND, _T("")).c_str());
	wndParentName_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_OEMNAME, _T("")).c_str());
	wndParentID_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_OEMID, _T("")).c_str());

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void WZDlgSafePwd::OnBnClickedCheckMenuPwd()
{
	if (wndMenuPwd_.GetCheck())
		wndEditMenuPwd_.EnableWindow(TRUE);
	else
		wndEditMenuPwd_.EnableWindow(FALSE);
}

void WZDlgSafePwd::OnEnChangeUsername()
{
	UpdateData(TRUE);
	
	CMessageBox msgDlg(_T("提示"),_T("请在设置安全密码 -> 输入账号名称!"));
	msgDlg.DoModal();
}

void WZDlgSafePwd::OnComplate()
{
	//确定按钮回写数据库选项表
	UpdateData(TRUE);

	
	if( consolepwd_.IsEmpty() )
		consolepwd_ = _T("1234567");

	i8desk::GetDataMgr().SetOptVal(OPT_U_CTLPWD, (LPCTSTR)consolepwd_);
	

	// 客户机高级设置密码
	if( wndMenuPwd_.GetCheck() )
	{
		CString textNewPwd;
		GetDlgItemText(IDC_EDIT_CFGCLIENTSET_NEWPWD,textNewPwd);
		if( textNewPwd.IsEmpty() )
			textNewPwd = _T("1234567");
		stdex::tString password;
		stdex::ToString(utility::CalBufCRC32((LPCTSTR)textNewPwd, textNewPwd.GetLength() * sizeof(TCHAR)), password);
		i8desk::GetDataMgr().SetOptVal(OPT_M_CLIPWD, password.c_str());
	}
}


void WZDlgSafePwd::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgSafePwd::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;

}

void WZDlgSafePwd::OnBnClickedButtonWzPswNetbarModify()
{
	CDlgCheckUser dlg;
	if( IDOK == dlg.DoModal() )
	{
		wndNetbarID_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_NID, _T("")).c_str());
		wndDateEnd_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_DATEEND, _T("")).c_str());
		wndParentName_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_OEMNAME, _T("")).c_str());
		wndParentID_.SetWindowText(i8desk::GetDataMgr().GetOptVal(OPT_U_OEMID, _T("")).c_str());

		CDlgCfgUserSet dlg;
		dlg.DoModal();
	}
}
