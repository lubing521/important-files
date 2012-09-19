// DlgDownloadAddClass.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadClass.h"
#include "DlgCfgBrowseDir.h"
#include "../MessageBox.h"

#include <atlpath.h>

#include "../Misc.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/UI/IconHelper.h"
#include "../../../../include/ui/ImageHelpers.h"

#include "../Business/ClassBusiness.h"
#include "../Business/ConfigBusiness.h"

#include "../ui/Skin/SkinMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgDownloadAddClass 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadClass, CNonFrameChildDlg)

CDlgDownloadClass::CDlgDownloadClass(bool isAdd, ClassTraits::ElementType *val, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadClass::IDD, pParent)
	, isAdd_(isAdd)
	, curClass_(new ClassTraits::ElementType)
	, className_(_T(""))
	, defPath_(_T(""))
{
	if( !isAdd_ )
	{
		utility::Strcpy(curClass_->CID,		val->CID);
		utility::Strcpy(curClass_->Name,	val->Name);
		utility::Strcpy(curClass_->SvrPath, val->SvrPath);
		curClass_->Icon			= val->Icon;
		curClass_->Thumbnail	= val->Thumbnail;
		curClass_->ImgIndex		= val->ImgIndex;
	}
	else
	{
		utility::Strcpy(curClass_->CID, utility::CreateGuidString());
	}
}

CDlgDownloadClass::~CDlgDownloadClass()
{
}

void CDlgDownloadClass::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_CLASS_NAME, className_);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_CLASS_PATH, defPath_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_CLASS_NAME, wndEditclassName_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_CLASS_PATH, wndEditdefPath_);
	
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CLASS_NAME, wndclassName_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CLASS_PATH, wnddefPath_);

	DDX_Control(pDX, IDC_STATIC_CLASS_ICON, wndIcon_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_CLASS_DESKTOPICON, wndCheckDesktopIcon_);

	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_CLASS_BROWSEDIR, wndBtnBrowDir_);

}


BEGIN_MESSAGE_MAP(CDlgDownloadClass, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgDownloadClass::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownloadClass::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_CLASS_BROWSEDIR, &CDlgDownloadClass::OnBnClickedButtonBrowseDir)
	ON_STN_CLICKED(IDC_STATIC_CLASS_ICON, &CDlgDownloadClass::OnStnClickedStaticClassIcon)
END_MESSAGE_MAP()


// CDlgDownloadAddClass 消息处理程序

BOOL CDlgDownloadClass::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	
	if( !isAdd_  )
	{
		className_	= curClass_->Name;
		defPath_	= curClass_->SvrPath;
		if( defPath_.IsEmpty() )
			defPath_ = i8desk::business::Class::GetPath(curClass_).c_str();

		const stdex::tString className = (LPCTSTR)className_;
		if( i8desk::business::Class::IsDefault(curClass_->CID) )
			wndEditclassName_.EnableWindow(FALSE);

		wndBtnOk_.SetWindowText(_T("修改"));
		SetTitle(_T("修改类别"));

		wndCheckDesktopIcon_.SetCheck(i8desk::business::Class::IsSetDeskTopClass(curClass_) ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		wndBtnOk_.SetWindowText(_T("添加"));
		SetTitle(_T("增加类别"));

		defPath_ = i8desk::business::Class::GetPath(curClass_).c_str();
	}

	if( curClass_->Thumbnail == 0 )
	{
		curClass_->Thumbnail = i8desk::business::Class::GetIcon(curClass_);
	}

	wndIcon_.SetImage(curClass_->Icon);
	wndIcon_.SetThemeParent(GetSafeHwnd());

	// CheckBox
	HBITMAP checkBox[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckBoxChecked.png"))
	};
	wndCheckDesktopIcon_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckDesktopIcon_.SetThemeParent(this->GetSafeHwnd());
	bool ischeck = i8desk::business::Class::IsSetDeskTopClass(curClass_);
	wndCheckDesktopIcon_.SetCheck(ischeck);

	// Edit

	wndEditclassName_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditdefPath_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	HBITMAP edit[] =
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
	};
	wndEditclassName_.SetImages(edit[0], edit[1], edit[2]);
	wndEditdefPath_.SetImages(edit[0], edit[1], edit[2]);

	wndclassName_.SetThemeParent(GetSafeHwnd());
	wnddefPath_.SetThemeParent(GetSafeHwnd());

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};
	wndBtnBrowDir_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownloadClass::OnBnClickedOk()
{
	UpdateData(TRUE);

	utility::Strcpy(curClass_->Name, className_);
	if ( isAdd_ )
	{
		ATL::CPath path(defPath_);
		path.AddBackslash();
		defPath_ = (LPCTSTR)path;
		defPath_ += className_;
	}


		// 检测名字是否重复
	if( className_.IsEmpty() || i8desk::business::Class::HasSameName(curClass_) )
	{
		stdex::tString msg = _T("名字为空或已经存在该名字: ");
		msg += curClass_->Name;
		CMessageBox msgDlg(_T("提示"),msg);
		msgDlg.DoModal();
		return;
	}

	if( defPath_.IsEmpty() && !i8desk::IsValidDirName((LPCTSTR)defPath_) )
		return;

	CPath path(defPath_);
	path.AddBackslash();
	defPath_ = (LPCTSTR)path;

	//判断路径是否存在
	if(i8desk::IsRunOnServer())
	{
		if(!i8desk::business::config::IsValidPath((LPCTSTR)defPath_))
		{
			CMessageBox msgDlg(_T("提示"),_T("类别路径不存在,请重新设置\n"));
			msgDlg.DoModal();
			return ;
		}
	}

	utility::Strcpy(curClass_->SvrPath ,defPath_);


	// 图标
	if( icon_ != 0 )
		i8desk::business::Class::SetIcon(curClass_, icon_);

	// 设置桌面快捷方式
	if ( wndCheckDesktopIcon_.GetCheck() )
		i8desk::business::Class::SetDeskTopClass(curClass_);
	else
		i8desk::business::Class::UnSetDeskTopClass(curClass_);

	// 数据
	if( isAdd_ )
	{
		if( !i8desk::GetDataMgr().AddData(i8desk::GetDataMgr().GetClasses(), curClass_->CID, curClass_) )
		return;
	}
	else
	{
		if( !i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetClasses(), curClass_->CID, curClass_) )
			return;
	}

	OnOK();
}

void CDlgDownloadClass::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgDownloadClass::OnStnClickedStaticClassIcon()
{
	CFileDialog fileDlg(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));
	CString fileName;
	if( fileDlg.DoModal() == IDOK )
		fileName = fileDlg.GetPathName();
	else
		return;

	HICON icon = LoadIconFromFile(fileName);
	if( icon == 0 )
		return;

	icon_ = Icon2Bitmap(icon);
	wndIcon_.SetImage(icon_);
}



void CDlgDownloadClass::OnBnClickedButtonBrowseDir()
{
	UpdateData(TRUE);
	BROWSEINFO info = {0};
	info.hwndOwner = m_hWnd;
	info.lpszTitle = _T("设置游戏下载默认路径:");
	static TCHAR buf[MAX_PATH] = {0};

	LPITEMIDLIST lst = SHBrowseForFolder(&info);
	if (lst != NULL)
	{
		SHGetPathFromIDList(lst, buf);
		LPMALLOC pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(lst);
		pMalloc->Release();
		SetDlgItemText(IDC_EDIT_DOWNLOAD_CLASS_PATH, buf);
	}
}


