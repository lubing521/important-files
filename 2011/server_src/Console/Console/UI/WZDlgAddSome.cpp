// WZDlgAddSome.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgAddSome.h"
#include "DlgDownloadSomeAdd.h"
#include "../Business/ClassBusiness.h"
#include "../MessageBox.h"


// WZDlgAddSome 对话框

IMPLEMENT_DYNAMIC(WZDlgAddSome, CDialog)

WZDlgAddSome::WZDlgAddSome(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgAddSome::IDD, pParent)
{

}

WZDlgAddSome::~WZDlgAddSome()
{
}

void WZDlgAddSome::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_WZ_ADDSOME_ADD, wndRadioAdd_);
	DDX_Control(pDX, IDC_RADIO_WZ_ADDSOME_NOTADD, wndRadioNotAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_ADDSOME_ADD, wndbtnAdd_);

	
	DDX_Control(pDX, IDC_STATIC_WZ_ADDSOME_PARARMADD, wndLabelParamAdd_);
	DDX_Control(pDX, IDC_STATIC_WZ_ADDSOME_PARARMNOTADD, wndLabelParamNoAdd_);
}


BEGIN_MESSAGE_MAP(WZDlgAddSome, CDialog)
	ON_BN_CLICKED(IDC_RADIO_WZ_ADDSOME_ADD, &WZDlgAddSome::OnBnClickedRadioAdd)
	ON_BN_CLICKED(IDC_RADIO_WZ_ADDSOME_NOTADD, &WZDlgAddSome::OnBnClickedRadioNotAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_ADDSOME_ADD, &WZDlgAddSome::OnBnClickedButtonAdd)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgAddSome 消息处理程序
BOOL WZDlgAddSome::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT lf;
	this->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	font_.CreateFontIndirect(&lf);

	wndRadioAdd_.SetFont(&font_);
	wndRadioNotAdd_.SetFont(&font_);

	wndRadioAdd_.SetCheck(TRUE);


	return TRUE;
}

void WZDlgAddSome::OnBnClickedRadioAdd()
{
	wndRadioNotAdd_.SetCheck(FALSE);
	wndbtnAdd_.EnableWindow(TRUE);

}

void WZDlgAddSome::OnBnClickedRadioNotAdd()
{
	wndRadioAdd_.SetCheck(FALSE);
	wndbtnAdd_.EnableWindow(FALSE);

}

void WZDlgAddSome::OnBnClickedButtonAdd()
{
	if(!i8desk::IsRunOnServer())
	{
		CMessageBox  msgbox(_T("提示"), _T("远程登录控制台时, 无法进行此操作！"));
		msgbox.DoModal();
		return;
	}

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
	i8desk::data_helper::GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
	if( games.size() == 0 )
	{
	}

	CDlgDownloadSomeAdd dlg(i8desk::business::Class::GetName(CLASS_WL_GUID).c_str());
	dlg.DoModal();
}

void WZDlgAddSome::OnShow(int showtype)
{
	ShowWindow(showtype);
}

BOOL WZDlgAddSome::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	return TRUE;
}