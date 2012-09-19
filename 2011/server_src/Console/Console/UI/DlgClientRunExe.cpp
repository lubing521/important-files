// DlgClientArea.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgClientRunExe.h"

#include "../../../../include/Utility/utility.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// DlgClientRunExe 对话框

IMPLEMENT_DYNAMIC(CDlgClientRunExe, CNonFrameChildDlg)

CDlgClientRunExe::CDlgClientRunExe( CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientRunExe::IDD, pParent)
	, runExe_(_T(""))
{

}

CDlgClientRunExe::~CDlgClientRunExe()
{
}

void CDlgClientRunExe::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLIENT_RUNCLIENTEXE, runExe_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_RUNCLIENTEXE, wndrunExe_);
	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_RUNCLIENTEXE, wndLabelareaName_);
}


BEGIN_MESSAGE_MAP(CDlgClientRunExe, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgClientRunExe::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgClientRunExe::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgClientArea 消息处理程序
BOOL CDlgClientRunExe::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	SetTitle(_T("执行客户机文件: ")  );

	// Edit

	wndrunExe_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	HBITMAP edit[] =
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
	};
	wndrunExe_.SetImages(edit[0], edit[1], edit[2]);

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);

	wndLabelareaName_.SetThemeParent(GetSafeHwnd());

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control

}


void CDlgClientRunExe::OnBnClickedOk()
{
	UpdateData(TRUE);
	OnOK();
}

void CDlgClientRunExe::OnBnClickedCancel()
{
	OnCancel();
}


