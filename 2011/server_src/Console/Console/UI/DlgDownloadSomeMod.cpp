// DlgDownloadSomeMod.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadSomeMod.h"
#include "../ui/Skin/SkinMgr.h"

#include "../GridListCtrlEx/CGridColumnTraitCombo.h"
#include "../Business/GameBusiness.h"
#include "../Business/ClassBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/AreaBusiness.h"
#include "../Business/ConfigBusiness.h"

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../MessageBox.h"

#include "../../../../include/Utility/utility.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
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
// CDlgDownloadSomeMod 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadSomeMod, CNonFrameChildDlg)

CDlgDownloadSomeMod::CDlgDownloadSomeMod(i8desk::data_helper::GameTraits::VectorType &games, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadSomeMod::IDD, pParent)
	, games_(games)
	, wndComboClass_(CSize(24, 24))
	, wndComboUpdate_(CSize(24, 24))
	, wndComboPriority_(CSize(24, 24))

{

}

CDlgDownloadSomeMod::~CDlgDownloadSomeMod()
{
}

void CDlgDownloadSomeMod::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_SVRPATH, wndCheckRepSvrPath_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_MODIFY_FROM, wndEditSvrPathSrc_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_MODIFY_TO, wndEditSvrPathDest_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_CLIPATH, wndCheckRepCliPath_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_MODIFY_CLIPATH, wndEditCliPathSrc_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_CLASS, wndCheckRepClass_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_MODIFY_CLASS, wndComboClass_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_AUTOUPT, wndCheckRepUpdate_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_MODIFY_AUTOUPT, wndComboUpdate_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_MODRUNTYPE, wndCheckRepRunType_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_MODIFY_RUNTYPE, wndListRunType_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_OTHER, wndCheckRepOther_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_TOOLBAR, wndCheckTool_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_DESKLNK, wndCheckDeskLink_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_MODIFY_PRIORITY, wndComboPriority_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_MODIFY_PRIORITY, wndCheckRepPriority_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_MODIFY_FROM, wndSvrPathSrc_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_MODIFY_TO, wndSvrPathDest_);

	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

}


BEGIN_MESSAGE_MAP(CDlgDownloadSomeMod, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgDownloadSomeMod::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownloadSomeMod::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_SVRPATH, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_CLIPATH, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_CLASS, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_AUTOUPT, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_PRIORITY, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_MODRUNTYPE, &CDlgDownloadSomeMod::OnCheck)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_MODIFY_OTHER, &CDlgDownloadSomeMod::OnCheck)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgDownloadSomeMod 消息处理程序

BOOL CDlgDownloadSomeMod::OnInitDialog()
{

	// 设置ComboBox皮肤
	HBITMAP arrow[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboBoxArrow.png"))

	};
	wndComboClass_.SetArrow(arrow[0], arrow[1], arrow[2]);
	wndComboUpdate_.SetArrow(arrow[0], arrow[1], arrow[2]);
	wndComboPriority_.SetArrow(arrow[0], arrow[1], arrow[2]);



	HBITMAP bk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png"));
	HBITMAP editBk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboEditBK.png"));
	wndComboClass_.SetBk(bk, editBk);
	wndComboUpdate_.SetBk(bk, editBk);
	wndComboPriority_.SetBk(bk, editBk);


	CNonFrameChildDlg::OnInitDialog();

	SetTitle(_T("批量修改游戏"));

	i8desk::business::Game::GetUpdateDesc(std::tr1::bind(
		&CCustomComboBox::AddString, &wndComboUpdate_, std::tr1::placeholders::_1));
	wndComboUpdate_.SetCurSel(0);

	i8desk::business::Game::GetPriorityDesc(std::tr1::bind(
		&CCustomComboBox::AddString, &wndComboPriority_, std::tr1::placeholders::_1));
	wndComboPriority_.SetCurSel(0);


	i8desk::data_helper::ClassTraits::VectorType classes; 
	i8desk::data_helper::ClassTraits::VecConstIterator iter;
	i8desk::business::Class::GetAllSortedClasses(classes);

	for( iter = classes.begin(); iter != classes.end(); ++iter )
	{
		wndComboClass_.AddString((*iter)->Name);
	}
	wndComboClass_.SetCurSel(0);


	wndListRunType_.SetHeaderImage(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));


	wndListRunType_.InsertHiddenLabelColumn();	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;

	wndListRunType_.InsertColumnTrait(1, _T("分区"), LVCFMT_LEFT, 100,1,GetTrait());
	wndListRunType_.InsertColumnTrait(2, _T("运行方式"), LVCFMT_LEFT, 358, 2, pComboTrait);

	// 添加组合框的所有运行方式
	pComboTrait->SetStyle(pComboTrait->GetStyle() | CBS_DROPDOWNLIST);

	int nIndex = 0;
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_UNKNOWN.c_str());
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_LOCAL.c_str());

	i8desk::business::VDisk::RunTypeDesc(std::tr1::bind(&CGridColumnTraitCombo::AddItem, pComboTrait, 
		nIndex++, std::tr1::placeholders::_1));
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_DIRECT.c_str());


	// 添加游戏在每个分区的运行方式
	stdex::tString szDefRunType = i8desk::GetDataMgr().GetOptVal(OPT_D_AREADEFRUNTYPE, _T(""));

	using i8desk::data_helper::AreaTraits;
	AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);
	int nIdx = 0; 
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		nIdx = wndListRunType_.InsertItem(nIdx, iter->second->Name);
		wndListRunType_.SetItemText(nIdx, 1, iter->second->Name);
		wndListRunType_.SetItemText(nIdx, 2, i8desk::business::Area::ParseRunType(iter->first, szDefRunType).c_str());
		nIdx++;
	}

	OnCheck();

	// Edit

	wndEditSvrPathSrc_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditSvrPathDest_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	wndEditCliPathSrc_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png")));
	HBITMAP edit[] =
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputLeft.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputMid.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputRight.png"))
	};
	wndEditSvrPathSrc_.SetImages(edit[0], edit[1], edit[2]);
	wndEditSvrPathDest_.SetImages(edit[0], edit[1], edit[2]);
	wndEditCliPathSrc_.SetImages(edit[0], edit[1], edit[2]);

	// CheckBox
	HBITMAP checkBox[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckboxNormal.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/CheckBoxChecked.png"))
	};
	wndCheckRepCliPath_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepSvrPath_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepClass_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepUpdate_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepPriority_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepRunType_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckRepOther_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckTool_.SetImages(checkBox[0], checkBox[1], checkBox[2]);
	wndCheckDeskLink_.SetImages(checkBox[0], checkBox[1], checkBox[2]);

	wndCheckRepCliPath_.SetThemeParent(GetSafeHwnd());
	wndCheckRepSvrPath_.SetThemeParent(GetSafeHwnd());
	wndCheckRepClass_.SetThemeParent(GetSafeHwnd());
	wndCheckRepUpdate_.SetThemeParent(GetSafeHwnd());
	wndCheckRepPriority_.SetThemeParent(GetSafeHwnd());
	wndCheckRepRunType_.SetThemeParent(GetSafeHwnd());
	wndCheckRepOther_.SetThemeParent(GetSafeHwnd());
	wndCheckTool_.SetThemeParent(GetSafeHwnd());
	wndCheckDeskLink_.SetThemeParent(GetSafeHwnd());

	// 画Static的背景
	wndSvrPathSrc_.SetThemeParent(GetSafeHwnd());
	wndSvrPathDest_.SetThemeParent(GetSafeHwnd());

	// 画Button的背景
	HBITMAP btn[] = 
	{

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Btn_Press.png"))
	};

	wndBtnOk_.SetImages(btn[0],btn[1],btn[2]);
	wndBtnCancel_.SetImages(btn[0],btn[1],btn[2]);

	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownloadSomeMod::OnBnClickedOk()
{	
	UpdateData(TRUE);

	stdex::tString svrPathSrc, svrPathDest, cliPath;
	TCHAR path[MAX_PATH] = {0};
	wndEditSvrPathSrc_.GetWindowText(path, MAX_PATH);
	svrPathSrc = path;

	std::uninitialized_fill_n(path, MAX_PATH, 0);
	wndEditSvrPathDest_.GetWindowText(path, MAX_PATH);
	svrPathDest = path;

	std::uninitialized_fill_n(path, MAX_PATH, 0);
	wndEditCliPathSrc_.GetWindowText(path, MAX_PATH);
	::PathAddBackslash(path);
	cliPath = path;

	stdex::Trim(svrPathSrc);
	stdex::Trim(svrPathDest);
	stdex::Trim(cliPath);

	if( wndCheckRepSvrPath_.GetCheck() && i8desk::IsRunOnServer())
	{
		if(!i8desk::business::config::IsValidPath(svrPathSrc.c_str()))
		{
			CMessageBox msgDlg(_T("提示"),_T("服务端源路径盘符不存在或者文件夹创建不成功\n"));
			msgDlg.DoModal();
			wndEditSvrPathSrc_.SetFocus();
			return ;
		}	
		if(!i8desk::business::config::IsValidPath(svrPathDest.c_str()))
		{
			CMessageBox msgDlg(_T("提示"),_T("服务端替换路径盘符不存在或者文件夹创建不成功\n"));
			msgDlg.DoModal();
			wndEditSvrPathDest_.SetFocus();
			return ;
		}	


	}
	
	using i8desk::data_helper::GameTraits;
	for(GameTraits::VecIterator iter = games_.begin(); iter != games_.end(); ++iter)
	{
		i8desk::uint64 mask = 0;
		GameTraits::ValueType &val = *iter;

		if( wndCheckRepSvrPath_.GetCheck() && !svrPathSrc.empty() )
		{
			int pos = 0;
			stdex::tString tmp = val->SvrPath;
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
			std::transform(svrPathSrc.begin(), svrPathSrc.end(), svrPathSrc.begin(), ::tolower);
			if( (pos = tmp.find(svrPathSrc, pos)) != stdex::tString::npos )
			{
				tmp.replace(pos, svrPathSrc.size(), svrPathDest);
				pos += svrPathDest.length();

				utility::Strcpy(val->SvrPath, tmp);
				mask |= MASK_TGAME_SVRPATH;
			}
		}

		if( wndCheckRepCliPath_.GetCheck() && !cliPath.empty() )
		{
			utility::Strcpy(val->CliPath, (cliPath + val->Name + _T("\\")).c_str());
			mask |= MASK_TGAME_CLIPATH;
		}

		if( wndCheckRepClass_.GetCheck() && val->GID < MIN_IDC_GID ) 
		{
			CString className;
			wndComboClass_.GetWindowText(className);

			utility::Strcpy(val->CID, i8desk::business::Class::GetCID((LPCTSTR)className));
			mask |= MASK_TGAME_CID;
		}

		if( wndCheckRepUpdate_.GetCheck() && val->GID >= MIN_IDC_GID ) 
		{
			val->AutoUpt = wndComboUpdate_.GetCurSel() == 0;
			mask |= MASK_TGAME_AUTOUPT;
		}

		if( wndCheckRepPriority_.GetCheck() ) 
		{
			val->Priority = wndComboPriority_.GetCurSel() ;
			mask |= MASK_TGAME_PRIORITY;
		}

		if( wndCheckRepOther_.GetCheck() )
		{
			val->Toolbar = wndCheckTool_.GetCheck();
			val->DeskLink = wndCheckDeskLink_.GetCheck();
			mask |= MASK_TGAME_TOOLBAR | MASK_TGAME_DESKLINK;
		}

		if( wndCheckRepRunType_.GetCheck() )
		{
			using i8desk::data_helper::RunTypeTraits;
			RunTypeTraits::VectorType runTypes;

			if( wndCheckRepRunType_.GetCheck() )
			{
				for( int nIdx= 0; nIdx != wndListRunType_.GetItemCount(); ++nIdx )
				{
					RunTypeTraits::ValueType val(new RunTypeTraits::ElementType);

					val->Name = (LPCTSTR)wndListRunType_.GetItemText(nIdx, 1);
					utility::Strcpy(val->AID, i8desk::business::Area::GetAIDByName(val->Name));

					if( i8desk::business::Area::ParseArea((LPCTSTR)wndListRunType_.GetItemText(nIdx, 2), val) )
						runTypes.push_back(val);
				}
			}

			if( !i8desk::business::Game::SetRunType(val, runTypes) )
				return;
		}

		if( !i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetGames(), 
			val->GID, val, mask) )
			return;
	}

	OnOK();
}

void CDlgDownloadSomeMod::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgDownloadSomeMod::OnCheck()
{
	wndCheckDeskLink_.EnableWindow( wndCheckRepOther_.GetCheck() );
	wndCheckTool_.EnableWindow( wndCheckRepOther_.GetCheck() );
	wndListRunType_.EnableWindow( wndCheckRepRunType_.GetCheck() );

	wndEditSvrPathDest_.EnableWindow( wndCheckRepSvrPath_.GetCheck() );
	wndEditSvrPathSrc_.EnableWindow( wndCheckRepSvrPath_.GetCheck() );

	wndEditCliPathSrc_.EnableWindow( wndCheckRepCliPath_.GetCheck() );

	wndComboClass_.EnableWindow( wndCheckRepClass_.GetCheck() );
	wndComboUpdate_.EnableWindow( wndCheckRepUpdate_.GetCheck() );
	wndComboPriority_.EnableWindow(	wndCheckRepPriority_.GetCheck() );

	GetDlgItem(IDOK)->EnableWindow(
		wndCheckRepOther_.GetCheck() ||
		wndCheckRepRunType_.GetCheck() ||
		wndCheckRepSvrPath_.GetCheck() ||
		wndCheckRepCliPath_.GetCheck() ||
		wndCheckRepClass_.GetCheck() ||
		wndCheckRepUpdate_.GetCheck() ||
		wndCheckRepPriority_.GetCheck()
		);
}

BOOL CDlgDownloadSomeMod::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListRunType_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	return TRUE;
}

