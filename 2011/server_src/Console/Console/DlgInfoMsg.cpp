// DlgInfoMsg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DlgInfoMsg.h"

#include "ManagerInstance.h"
#include "UI/Skin/SkinMgr.h"

// CDlgInfoMsg 对话框

IMPLEMENT_DYNAMIC(CDlgInfoMsg, CNonFrameChildDlg)

CDlgInfoMsg::CDlgInfoMsg(CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgInfoMsg::IDD, pParent)
	, infoMsg_(_T(""))
	, isShowCancel_(false)
{

}

CDlgInfoMsg::~CDlgInfoMsg()
{
}

void CDlgInfoMsg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_STATIC_INFO_MSG, infoMsg_);
	DDX_Control(pDX, IDOK, wndBtnOK_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_STATIC_INFO_MSG, wndText_);
}


BEGIN_MESSAGE_MAP(CDlgInfoMsg, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgInfoMsg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgInfoMsg::OnBnClickedCancel)
END_MESSAGE_MAP()



BOOL CDlgInfoMsg::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("提示"));

	// 是否有取消按钮
	isShowCancel_ = i8desk::GetDataMgr().GetOptVal(OPT_U_CONSHOWCANCEL, 0) == 1;
	if( !isShowCancel_ )
	{
		CRect rcCancelRect;
		wndBtnOK_.GetWindowRect(rcCancelRect);
		wndBtnCancel_.ShowWindow(SW_HIDE);

		ScreenToClient(rcCancelRect);
		wndBtnOK_.MoveWindow(rcCancelRect);
	}

	// 读取URL
	url_ = i8desk::GetDataMgr().GetOptVal(OPT_U_CONOKURL, url_);

	// 读取提示内容
	infoMsg_ = i8desk::GetDataMgr().GetOptVal(OPT_U_CONMESSAGE, _T("")).c_str();

	// Button
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnOK_.SetImages(btn[0], btn[1], btn[2]);
	wndBtnCancel_.SetImages(btn[0], btn[1], btn[2]);

	wndText_.SetThemeParent(GetSafeHwnd());

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}


// CDlgInfoMsg 消息处理程序

void CDlgInfoMsg::OnBnClickedOk()
{
	if( !url_.empty() )
	{
		ShellExecute(GetSafeHwnd(), _T("open"), url_.c_str(), _T(""), _T(""), SW_SHOWNORMAL);
	}

	OnOK();
}

void CDlgInfoMsg::OnBnClickedCancel()
{
	OnCancel();
}

