// WZDlgClass.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgClass.h"
#include "DlgCfgBrowseDir.h"

#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Utility/utility.h"


#include "../Business/ClassBusiness.h"
#include "../MessageBox.h"

#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"

#include <atlpath.h>


namespace
{
	namespace detail
	{
		CCustomColumnTrait *GetTrait()
		{
			CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
			rowTrait->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return rowTrait;
		}

	}

	enum
	{ 
		MASK_CLASS_NAME = 1, MASK_CLASS_PATH 
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_CLASS_NAME,		_T("类别名称"),			LVCFMT_LEFT, 95, MASK_CLASS_NAME, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_CLASS_PATH,		_T("游戏默认路径"),		LVCFMT_LEFT, 150, MASK_CLASS_PATH, detail::GetTrait() );
	}
}


// WZDlgClass 对话框

IMPLEMENT_DYNAMIC(WZDlgClass, CDialog)

WZDlgClass::WZDlgClass(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgClass::IDD, pParent)
	, defaultClassPath_(_T(""))
{
	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgClass::~WZDlgClass()
{
	outboxLine_.Detach();
}

void WZDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_WZ_CLASS_CLASSNAME, wndEditClassName);
	DDX_Text(pDX, IDC_EDIT_WZ_CLASS_CLASSNAME, className_);

	DDX_Control(pDX, IDC_EDIT_WZ_CLASS_DEFAULTPATH, wndEditClassPath);
	DDX_Text(pDX, IDC_EDIT_WZ_CLASS_DEFAULTPATH, classPath_);

	DDX_Control(pDX, IDC_STATIC_WZ_CLASS_CLASSNAME, wndLabelClassName);
	DDX_Control(pDX, IDC_STATIC_WZ_CLASS_DEFAULTPATH, wndLabelClassPath);

	DDX_Control(pDX, IDC_LIST_WZ_CLASS_CLASSES, wndListClasses_);

	DDX_Control(pDX, IDC_BUTTON_WZ_CLASS_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CLASS_DEL, wndbtnDel_);	
	DDX_Control(pDX, IDC_BUTTON_WZ_CLASS_MODIFY, wndbtnModify_);
	DDX_Control(pDX, IDC_BUTTON_WZ_CLASS_CLASSPATH, wndBtnDir_);
}


BEGIN_MESSAGE_MAP(WZDlgClass, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CLASS_ADD, &WZDlgClass::OnBnClickedButtonClassAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CLASS_DEL, &WZDlgClass::OnBnClickedButtonClassDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CLASS_MODIFY, &WZDlgClass::OnBnClickedButtonClassModify)
	ON_BN_CLICKED(IDC_BUTTON_WZ_CLASS_CLASSPATH, &WZDlgClass::OnBnClickedButtonClassPath)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_CLASS_CLASSES, &WZDlgClass::OnLvnGetdispinfoListClassInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WZ_CLASS_CLASSES, &WZDlgClass::OnLvnItemchangedListClassInfo)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgClass 消息处理程序
BOOL WZDlgClass::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitListCtrl(wndListClasses_);


	return TRUE;
}

void WZDlgClass::_ReadDatas()
{
	curClasses_.clear();
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClasses());

	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	ClassTraits::MapType &Classes = i8desk::GetDataMgr().GetClasses();
	stdex::map2vector(Classes, curClasses_);

	className_ = curClasses_[0]->Name;
	classPath_ = curClasses_[0]->SvrPath;

	_ShowView();

	UpdateData(FALSE);

}
bool WZDlgClass::_CheckDatas(i8desk::data_helper::ClassTraits::ValueType &val)
{
	UpdateData(TRUE);

	using i8desk::data_helper::ClassTraits;

	if( classPath_.IsEmpty() )
		classPath_ = defaultClassPath_.c_str();

	// 检测名字是否是官方类别
	stdex::tString CID = val->CID;
	if( i8desk::business::Class::IsDefault(CID) && utility::Strcmp(className_,val->Name) != 0)
	{
		stdex::tString msg = _T("名字是官方类别，不能被修改: ");
		msg += val->Name;
		CMessageBox msgDlg(_T("提示"),msg);
		msgDlg.DoModal();
		return false;
	}

	utility::Strcpy(val->Name ,className_);


	CPath path(classPath_);
	path.AddBackslash();
	classPath_ = (LPCTSTR)path;

	//判断路径是否存在
	if(i8desk::IsRunOnServer())
	{
		if(!PathFileExists((LPCTSTR)classPath_))
		{
			CMessageBox msgDlg(_T("提示"),_T("类别路径不存在,请重新设置\n"));
			msgDlg.DoModal();
			return false;
		}
	}

	// 检测名字是否重复
	if( className_.IsEmpty() || i8desk::business::Class::HasSameName(val) )
	{
		stdex::tString msg = _T("名字为空或已经存在该名字: ");
		msg += val->Name;
		CMessageBox msgDlg(_T("提示"),msg);
		msgDlg.DoModal();
		return false;
	}

	utility::Strcpy(val->SvrPath ,classPath_);

	return true;
}

void WZDlgClass::OnBnClickedButtonClassPath()
{
	UpdateData(TRUE);
	if (i8desk::IsRunOnServer())
	{
		CString selPath;
		if( theApp.GetShellManager()->BrowseForFolder(selPath, this, classPath_, _T("选择三层下载目录:")) )
		{
			if( selPath.IsEmpty() )
				return ;

			if( selPath.Right(1) == _T("\\") )
				selPath.Delete(selPath.GetLength()-1);

			selPath += _T("\\");
			selPath.Trim();

			classPath_ = selPath;
		}
	}
	else
	{
		CDlgCfgBrowseDir dlg;
		GetDlgItemText(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, dlg.strDirOrFile_);
		if (dlg.DoModal() != IDOK)
			return;
		SetDlgItemText(IDC_EDIT_CFGDOWNLOADSET_AGREEDOWNPATH, dlg.strDirOrFile_);
	}

	UpdateData(FALSE);
}


void WZDlgClass::OnBnClickedButtonClassAdd()
{
	using namespace i8desk::data_helper;
	ClassTraits::ValueType val(new ClassTraits::ElementType);
	utility::Strcpy(val->CID, utility::CreateGuidString());
	if( !_CheckDatas(val) )
		return;

	ClassTraits::MapType &Classes = i8desk::GetDataMgr().GetClasses();

	if( i8desk::GetDataMgr().AddData(Classes, val->CID, val) )
	{
		curClasses_.push_back(val);	
		_ShowView();
	}
}

void WZDlgClass::OnBnClickedButtonClassDel()
{
	int nSel = wndListClasses_.GetNextItem(-1, LVIS_SELECTED);
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的项吗?"));

	if( nSel != -1 && msgDlg.DoModal() == IDOK )
	{
		i8desk::data_helper::ClassTraits::ValueType &val = curClasses_[nSel];

		// 检测名字是否是官方类别
		stdex::tString CID = val->CID;
		if( i8desk::business::Class::IsDefault(CID) )
		{
			stdex::tString msg = _T("名字是官方类别，不能被删除: ");
			msg += val->Name;
			CMessageBox msgDlg(_T("提示"),msg);
			msgDlg.DoModal();
			return ;
		}

		i8desk::data_helper::ClassTraits::MapType &Classes = i8desk::GetDataMgr().GetClasses();

		if( i8desk::GetDataMgr().DelData(Classes, val->CID) )
		{
			curClasses_.erase(curClasses_.begin() + nSel);
			_ShowView();
		}

	}
}

void WZDlgClass::OnBnClickedButtonClassModify()
{
	int nSel = wndListClasses_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return ;

	i8desk::data_helper::ClassTraits::ValueType &val = curClasses_[nSel];

	if( !_CheckDatas(val) )
		return;

	i8desk::data_helper::ClassTraits::MapType &Classes = i8desk::GetDataMgr().GetClasses();

	if( i8desk::GetDataMgr().ModifyData(Classes, val->CID, val) )
		_ShowView();
}

void WZDlgClass::_ShowView()
{
	size_t cnt = curClasses_.size();
	wndListClasses_.SetItemCount(cnt);
}

void WZDlgClass::OnLvnItemchangedListClassInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = wndListClasses_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 ) 
		return ;

	i8desk::data_helper::ClassTraits::ValueType &val = curClasses_[nSel];

	className_ = val->Name;
	classPath_ = val->SvrPath;
	UpdateData(FALSE);

}

void WZDlgClass::OnLvnGetdispinfoListClassInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curClasses_.size() )
		return;
	using namespace i8desk::data_helper;
	const ClassTraits::ValueType &val = curClasses_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_CLASS_NAME:
			text = val->Name;
			break;
		case MASK_CLASS_PATH:
			text = val->SvrPath;
			break;

		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void WZDlgClass::OnShow(int showtype)
{
	ShowWindow(showtype);
	_ReadDatas();
}

BOOL WZDlgClass::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndListClasses_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &outboxLine_);

	return TRUE;
}