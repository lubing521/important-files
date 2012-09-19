// WZDlgWelcome.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgWelcome.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include "../../../../include/ui/ImageHelpers.h"


// WZDlgWelcome 对话框

IMPLEMENT_DYNAMIC(WZDlgWelcome, CDialog)

WZDlgWelcome::WZDlgWelcome(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgWelcome::IDD, pParent)
{

}

WZDlgWelcome::~WZDlgWelcome()
{
}

void WZDlgWelcome::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WZ_WELCOME, wndLabelWelCome_);
}


BEGIN_MESSAGE_MAP(WZDlgWelcome, CDialog)
	ON_WM_ERASEBKGND()
	
END_MESSAGE_MAP()


// WZDlgWelcome 消息处理程序
BOOL WZDlgWelcome::OnInitDialog()
{
	CDialog::OnInitDialog();

	wndLabelWelCome_.SetTextColor(RGB(0, 0, 0));
	return TRUE;
}

void WZDlgWelcome::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgWelcome::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;

}