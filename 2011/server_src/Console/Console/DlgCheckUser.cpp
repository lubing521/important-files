// DlgCheckUser.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DlgCheckUser.h"
#include "ManagerInstance.h"
#include "UI/Skin/SkinMgr.h"

#include "MessageBox.h"
#include "DlgCheckUser.h"
#include "DlgWaitMsg.h"
#include "UI/UIHelper.h"

// CDlgCheckUser 对话框


enum { WM_CHECK_ERROR_MSG = WM_USER + 0x11 };

IMPLEMENT_DYNAMIC(CDlgCheckUser, CNonFrameChildDlg)

CDlgCheckUser::CDlgCheckUser(CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgCheckUser::IDD, pParent)
{
	outLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));
}

CDlgCheckUser::~CDlgCheckUser()
{
	outLine_.Detach();
}

void CDlgCheckUser::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, wndBtnOK_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_STATIC_CHECK_USER, wndUserText_);
	DDX_Control(pDX, IDC_STATIC_CHECK_PSW, wndPswText_);
	DDX_Control(pDX, IDC_STATIC_CHECK_ERROR_MSG, wndErrorText_);
	DDX_Control(pDX, IDC_EDIT_CHECK_USER_USERNAME, wndEditUser_);
	DDX_Control(pDX, IDC_EDIT_CHECK_USER_PSW, wndEditPsw_);
}


BEGIN_MESSAGE_MAP(CDlgCheckUser, CNonFrameChildDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CDlgCheckUser::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgCheckUser::OnBnClickedCancel)
	ON_MESSAGE(WM_CHECK_ERROR_MSG, &CDlgCheckUser::OnCheckErrorMsg)
END_MESSAGE_MAP()


stdex::tString CDlgCheckUser::GetUserName() const
{
	CString user;
	wndEditUser_.GetWindowText(user);
	
	return (LPCTSTR)user;
}

stdex::tString CDlgCheckUser::GetPassword() const
{
	CString psw;
	wndEditPsw_.GetWindowText(psw);

	return (LPCTSTR)psw;
}

// CDlgCheckUser 消息处理程序
BOOL CDlgCheckUser::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("用户验证"));
	


	UpdateData(FALSE);

	return TRUE;
}

BOOL CDlgCheckUser::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgCheckUser::Check()
{
	CString user, psw;
	wndEditUser_.GetWindowText(user);
	wndEditPsw_.GetWindowText(psw);

	if( user.IsEmpty() )
	{
		CMessageBox msg(_T("提示"), _T("用户名不能名空"));
		msg.DoModal();

		wndEditUser_.SetFocus();
		return ;
	}

	if( psw.IsEmpty() )
	{
		CMessageBox msg(_T("提示"), _T("密码不能为空"));
		msg.DoModal();

		wndEditPsw_.SetFocus();
		return ;
	}

	struct CheckUser
		: public std::tr1::enable_shared_from_this<CheckUser>
	{
		CDlgCheckUser *wnd_;
		i8desk::manager::RecvDataMgr::CheckUserCallback oldCallback_;
		
		CheckUser(CDlgCheckUser *wnd)
			: wnd_(wnd)
		{
			
		}
		~CheckUser()
		{
			
		}

		void _OnNotify(bool suc)
		{
			if( ::IsWindow(wnd_->GetSafeHwnd()) )
				wnd_->OnCheckError(suc);

			i8desk::GetRecvDataMgr().RegisterCheckUser(oldCallback_);
		}

		void Run()
		{
			i8desk::GetControlMgr().CheckUser(wnd_->GetUserName(), wnd_->GetPassword());

			oldCallback_ = i8desk::GetRecvDataMgr().RegisterCheckUser(
				std::tr1::bind(&CheckUser::_OnNotify, shared_from_this(), std::tr1::placeholders::_1));
		}
	};
	typedef std::tr1::shared_ptr<CheckUser> CheckUserPtr;

	wndBtnOK_.EnableWindow(FALSE);
	
	CheckUserPtr checkUser(new CheckUser(this));
	checkUser->Run();
}

void CDlgCheckUser::OnBnClickedOk()
{
	Check();
}

void CDlgCheckUser::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgCheckUser::OnCheckError(bool suc)
{
	PostMessage(WM_CHECK_ERROR_MSG, suc ? 1 : 0);
}

LRESULT CDlgCheckUser::OnCheckErrorMsg(WPARAM wParam, LPARAM)
{
	if( wParam == 1 )
		OnOK();
	else
	{
		wndBtnOK_.EnableWindow(TRUE);

		stdex::tString errText = _T("验证失败: ");
		errText = errText + i8desk::GetDataMgr().GetOptVal(OPT_U_ERRINFO, errText);
		wndErrorText_.SetTextColor(RGB(255, 0, 0));
		wndErrorText_.SetWindowText(errText.c_str());

	}

	return TRUE;
}

BOOL CDlgCheckUser::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndEditPsw_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outLine_);

	return TRUE;
}