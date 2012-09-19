// DlgClientArea.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgClientArea.h"

#include "../../../../include/Utility/utility.h"
#include "../Business/AreaBusiness.h"
#include "../Business/GameBusiness.h"

#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgClientArea 对话框

IMPLEMENT_DYNAMIC(CDlgClientArea, CNonFrameChildDlg)

CDlgClientArea::CDlgClientArea(bool isAdd, const AreaTraits::ElementType *area, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientArea::IDD, pParent)
	, isAdd_(isAdd)
	, areaName_(_T(""))
	, curArea_(new AreaTraits::ElementType)
{
	if( isAdd )
	{
		utility::Strcpy(curArea_->AID, utility::CreateGuidString());
	}
	else
		*curArea_ = *area;
}

CDlgClientArea::~CDlgClientArea()
{
}

void CDlgClientArea::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CLIENT_AREA_NAME, areaName_);
	DDX_Control(pDX, IDC_EDIT_CLIENT_AREA_NAME, wndareaName_);
	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_STATIC_CLIENT_AREA_NAME, wndLabelareaName_);
}


BEGIN_MESSAGE_MAP(CDlgClientArea, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgClientArea::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgClientArea::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgClientArea 消息处理程序
BOOL CDlgClientArea::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	if ( i8desk::business::Area::IsDefaultArea(curArea_) )
		wndareaName_.EnableWindow(FALSE);

	if( isAdd_ )
	{
		SetTitle(_T("添加新区域"));
		wndBtnOk_.SetWindowText(_T("添加"));
	}
	else
	{
		SetTitle(_T("修改区域: ")  );
		wndBtnOk_.SetWindowText(_T("修改"));
	}

	// Edit

	wndareaName_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	HBITMAP edit[] =
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
	};
	wndareaName_.SetImages(edit[0], edit[1], edit[2]);

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


void CDlgClientArea::OnBnClickedOk()
{
	UpdateData(TRUE);
	if ( areaName_.IsEmpty())
	{
		stdex::tString msg = _T("请填写区域名称!");
		CMessageBox msgDlg(_T("客户机区域"), msg);
		msgDlg.DoModal();

		return;
	}

	utility::Strcpy(curArea_->Name, (LPCTSTR)areaName_);

	using i8desk::data_helper::AreaTraits;

	// 检测名字是否重复
	if( i8desk::business::Area::HasSameName(curArea_) )
	{
		stdex::tString msg = _T("已经存在该名字: ");
		msg += curArea_->Name;
		CMessageBox msgDlg(_T("客户机区域"), msg);
		msgDlg.DoModal();

		return;
	}

	if( isAdd_ )
	{
		if( !i8desk::GetDataMgr().AddData(i8desk::GetDataMgr().GetAreas(), curArea_->AID, curArea_) )
			return;
	}
	else
	{
		if( !i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetAreas(), curArea_->AID, curArea_) )
			return;
	}
	
	OnOK();
}

void CDlgClientArea::OnBnClickedCancel()
{
	OnCancel();
}


