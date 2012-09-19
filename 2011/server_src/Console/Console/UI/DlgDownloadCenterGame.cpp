// DlgDownloadCenterGame.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadCenterGame.h"

#include "../GridListCtrlEx/CGridColumnTraitCombo.h"

#include "../Business/GameBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/AreaBusiness.h"
#include "../Business/SyncTaskBusiness.h"

#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/Utility/utility.h"
#include "../UI/UIHelper.h"
#include "../MessageBox.h"

#include "DlgDownloadSmartClean.h"

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


// CDlgDownloadCenterGame 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadCenterGame, CNonFrameChildDlg)

CDlgDownloadCenterGame::CDlgDownloadCenterGame(bool isMultiSelect, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadCenterGame::IDD, pParent)
	, isMultiSelect_(isMultiSelect)
	, gameName_(_T(""))
	, gameSvrPath_(_T(""))
	, gameCliPath_(_T(""))
{

}

CDlgDownloadCenterGame::~CDlgDownloadCenterGame()
{
}

void CDlgDownloadCenterGame::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_GAMENAME, wndEditgameName_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_SVRPATH, wndEditgameSvrPath_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_CLIPATH, wndEditgameCliPath_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_GAMENAME, wndLabelgameName_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_SVRPATH, wndLabelgameSvrPath_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_CLIPATH, wndLabelgameCliPath_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_RUNTYPE, wndLabelgameRunType_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_SYNCTASK, wndLabelgameSyncTask_);

	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_GAMENAME, gameName_);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_SVRPATH, gameSvrPath_);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_CENTER_ADD_CLIPATH, gameCliPath_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_CENTER_ADD_AUTOUPT, wndComboGameUpdate_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_CENTER_ADD_DOWNPRIORITY, wndComboDownloadPriority_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_CENTER_ADD_RUNTYPE, wndListRunType_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_CENTER_ADD_SYNCTASK, wndListSyncTask_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_AUTOUPT, wndLabelGameUpdate_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_DOWNPRIORITY, wndLabelDownloadPriority_);

	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_CENTER_ADD_DECLARE, wndLabelDeclare_);

	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_CENTER_SELALL, wndChkSelSyncTaskAll_);

}


BEGIN_MESSAGE_MAP(CDlgDownloadCenterGame, CNonFrameChildDlg)
	ON_BN_CLICKED(IDOK, &CDlgDownloadCenterGame::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownloadCenterGame::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_CENTER_SELALL, &CDlgDownloadCenterGame::OnBnClickedCheckSelSyncTaskAll)

	ON_WM_ERASEBKGND()

	//ON_STN_CLICKED(IDC_STATIC_DOWNLOAD_CENTER_ADD_DECLARE, &CDlgDownloadCenterGame::OnStnClickedStaticDownloadCenterAddDeclare)
END_MESSAGE_MAP()


namespace 
{
	template < typename GameT, typename DialogT >
	bool Handle(GameT &game, DialogT &wnd)
	{
		game->AutoUpt		= wnd->wndComboGameUpdate_.GetCurSel();
		game->Priority		= wnd->wndComboDownloadPriority_.GetCurSel() + 1;

		// 设置运行方式
		using i8desk::data_helper::RunTypeTraits;
		RunTypeTraits::VectorType runTypes;

		for( int nIdx= 0; nIdx != wnd->wndListRunType_.GetItemCount(); ++nIdx )
		{
			RunTypeTraits::ValueType val(new RunTypeTraits::ElementType);

			val->Name = (LPCTSTR)wnd->wndListRunType_.GetItemText(nIdx, 1);
			utility::Strcpy(val->AID, i8desk::business::Area::GetAIDByName(val->Name));
			val->GID = game->GID;

			if( i8desk::business::Area::ParseArea((LPCTSTR)wnd->wndListRunType_.GetItemText(nIdx, 2), val) )
				runTypes.push_back(val);
		}
		
		return i8desk::business::Game::SetRunType(game, runTypes);
	};
}


// CDlgDownloadCenterGame 消息处理程序

BOOL CDlgDownloadCenterGame::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("下载游戏"));

	if(isMultiSelect_)
	{
		wndEditgameName_.EnableWindow(FALSE);
		wndEditgameSvrPath_.EnableWindow(FALSE);
		wndEditgameCliPath_.EnableWindow(FALSE);
	}
	else
		wndEditgameName_.EnableWindow(FALSE);


	if( !isMultiSelect_ )
	{
		// 处理路径
		i8desk::business::Game::HandlePath(curGame_);

		gameName_		= curGame_->Name;
		gameSvrPath_	= curGame_->SvrPath;
		gameCliPath_	= curGame_->CliPath;
	}
	else
	{
		gameName_ = gameSvrPath_ = gameCliPath_ = _T("默认");
	}
	UpdateData(FALSE);

	i8desk::business::Game::GetUpdateDesc(std::tr1::bind(
		&CCustomComboBox::AddString, &wndComboGameUpdate_, std::tr1::placeholders::_1));
	
	i8desk::business::Game::GetPriorityDesc(std::tr1::bind(
		&CCustomComboBox::AddString, &wndComboDownloadPriority_, std::tr1::placeholders::_1));

	if( isMultiSelect_ || !curGame_->Status )
	{
		wndComboGameUpdate_.SetCurSel(0);
		wndComboDownloadPriority_.SetCurSel(0);
	} 
	else 
	{
		wndComboGameUpdate_.SetCurSel(curGame_->AutoUpt ? 0 : 1);
		wndComboDownloadPriority_.SetCurSel(curGame_->Priority);
	}


	wndListRunType_.InsertHiddenLabelColumn();
	wndListRunType_.InsertColumnTrait(1, _T("分区"), LVCFMT_LEFT, 130, 1, GetTrait());

	// 添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	wndListRunType_.InsertColumnTrait(2, _T("运行方式"), LVCFMT_LEFT, 256, 2, pComboTrait);
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
	
	int nItem = 0;
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		nItem = wndListRunType_.InsertItem(nItem, _T(""));
		wndListRunType_.SetItemText(nItem, 1, iter->second->Name);
		wndListRunType_.SetItemText(nItem, 2, i8desk::business::Area::ParseRunType(iter->first, szDefRunType).c_str());
		nItem++;
	}

	// 同步任务
	using i8desk::data_helper::SyncTaskTraits;
	SyncTaskTraits::MapType &synctasks = i8desk::GetDataMgr().GetSyncTasks();
	i8desk::GetDataMgr().GetAllData(synctasks);

	wndListSyncTask_.InsertHiddenLabelColumn();
	wndListSyncTask_.InsertColumn(0, _T("任务名称"), LVCFMT_LEFT, 180);
	wndListSyncTask_.SetExtendedStyle(wndListSyncTask_.GetExtendedStyle() | LVS_EX_CHECKBOXES);



	i8desk::data_helper::SyncTaskTraits::MapType &diffSyncTasks = i8desk::GetDataMgr().GetSyncTasks();
	BOOL IsSelAll = (BOOL)i8desk::GetDataMgr().GetOptVal(OPT_D_SELSYNCTASKALL, 0);
	wndChkSelSyncTaskAll_.SetCheck(IsSelAll);

	int itemSyncTask = 0;
	for(i8desk::data_helper::SyncTaskTraits::MapConstIterator iter = diffSyncTasks.begin(); 
		iter != diffSyncTasks.end(); ++iter)
	{
		itemSyncTask = wndListSyncTask_.InsertItem(itemSyncTask, iter->second->Name);
		wndListSyncTask_.SetCheck(itemSyncTask, IsSelAll);
		wndListSyncTask_.SetItemData(itemSyncTask, (DWORD_PTR)(iter->second.get()));
		++itemSyncTask;
	}

	
	// 声明红色
	//wndLabelDeclare_.SetTextColor(RGB(255, 0, 0));
	wndLabelDeclare_.SetURL(_T("http://bbs.i8desk.com/showtopic-26757.aspx"));

	// Out Line
	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownloadCenterGame::OnBnClickedCheckSelSyncTaskAll()
{
	BOOL IsSelAll = (BOOL)wndChkSelSyncTaskAll_.GetCheck();

	i8desk::data_helper::SyncTaskTraits::MapType &diffSyncTasks = i8desk::GetDataMgr().GetSyncTasks();

	int itemSyncTask = 0;
	for(i8desk::data_helper::SyncTaskTraits::MapConstIterator iter = diffSyncTasks.begin(); 
		iter != diffSyncTasks.end(); ++iter)
	{
		wndListSyncTask_.SetCheck(itemSyncTask, IsSelAll);
		++itemSyncTask;
	}

}

void CDlgDownloadCenterGame::OnBnClickedOk()
{
	UpdateData(TRUE);
	
	i8desk::GetDataMgr().SetOptVal(OPT_D_SELSYNCTASKALL, wndChkSelSyncTaskAll_.GetCheck());

	//  设置路径
	if( !isMultiSelect_ )
	{
		gameSvrPath_.Trim();
		if( !i8desk::IsValidDirName((LPCTSTR)gameSvrPath_) )
		{
			CMessageBox msgDlg(_T("提示"),_T("服务端路径不合法"));
			msgDlg.DoModal();
			return;
		}

		//判断路径是否存在
		if(i8desk::IsRunOnServer())
		{
			TCHAR SvrPath[ MAX_PATH ] ={0};
			utility::Strcpy(SvrPath, (LPCTSTR)gameSvrPath_);

			stdex::tOstringstream os;
			os << SvrPath[0] << _T(":\\");

			if( !::PathFileExists(os.str().c_str()) )
			{
				CMessageBox msgDlg(_T("提示"),_T("服务端盘符不存在,请重新设置\n"));
				msgDlg.DoModal();
				return;
			}
		}

		utility::Strcpy(curGame_->SvrPath, (LPCTSTR)gameSvrPath_);

		gameCliPath_.Trim();
		if( !i8desk::IsValidDirName((LPCTSTR)gameCliPath_) )
		{
			CMessageBox msgDlg(_T("提示"),_T("客户端路径不合法"));
			msgDlg.DoModal();
			return ;
		}
		utility::Strcpy(curGame_->CliPath, (LPCTSTR)gameCliPath_);

		// 设置Runtype
		if( !Handle(curGame_, this) )
			return;

		// 设置Game 更新方式
		CString updateText;
		wndComboGameUpdate_.GetWindowText(updateText);
		
		i8desk::business::Game::SetUpdate(curGame_, (LPCTSTR)updateText);
		i8desk::GetDataMgr().EnsureData(i8desk::GetDataMgr().GetGames(), curGame_->GID, curGame_, MASK_TGAME_SVRPATH | MASK_TGAME_CLIPATH);
	}
	else
	{
		for(i8desk::data_helper::GameTraits::VecIterator iter = curGames_.begin();
			iter != curGames_.end(); ++iter)
		{
			// 处理路径
			i8desk::business::Game::HandlePath(*iter);

			if( !Handle(*iter, this) )
				return;
		}
	}


	// 如果已有同步游戏记录，则略过
	// 添加游戏到同步任务
	using namespace i8desk::data_helper;
	for(size_t i = 0; i != wndListSyncTask_.GetItemCount(); ++i)
	{
		if( !wndListSyncTask_.GetCheck(i) )
			continue;

		SyncTaskTraits::ElementType *val = reinterpret_cast<SyncTaskTraits::ElementType *>(wndListSyncTask_.GetItemData(i));
		stdex::tString SID = val->SID;

		// 单选
		if( isMultiSelect_ == FALSE )
		{
			i8desk::business::SyncTask::AddSyncGame(SID, curGame_->GID);
		}
		else // 多选
		{
			for(i8desk::data_helper::GameTraits::VecIterator iter = curGames_.begin();
				iter != curGames_.end(); ++iter)
			{
				i8desk::business::SyncTask::AddSyncGame(SID, (*iter)->GID);
			}
		}
	}


	// 检测硬盘空间是否足够
	if( !isMultiSelect_ )
	{
		unsigned long long totalsize = curGame_->Size ;
		if( !i8desk::business::Game::IsEnoughSpace(curGame_->SvrPath, totalsize * 1024) )
		{
			stdex::tOstringstream os;
			os << curGame_->SvrPath[0] << _T(" 盘已没有足够空间, 是否需要删除游戏");
			CMessageBox box(_T("提示"), os.str().c_str());
			if( box.DoModal() == IDOK )
			{
				CDlgDownloadSmartClean dlg(curGame_->SvrPath[0], curGame_->Size );
				dlg.DoModal();
			}
		}
	}
	else
	{
		for(size_t i = 0; i != curGames_.size(); ++i)
		{
			unsigned long long totalsize = curGames_[i]->Size;
			if( !i8desk::business::Game::IsEnoughSpace(curGames_[i]->SvrPath, totalsize * 1024) )
			{
				stdex::tOstringstream os;
				os << curGames_[i]->SvrPath[0] << _T(" 盘已没有足够空间, 是否需要删除游戏");
				CMessageBox box(_T("提示"), os.str().c_str());
				if( box.DoModal() == IDOK )
				{
					CDlgDownloadSmartClean dlg(curGames_[i]->SvrPath[0], curGames_[i]->Size );
					dlg.DoModal();
				}
			}
		}
	}

	OnOK();
}

void CDlgDownloadCenterGame::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgDownloadCenterGame::OnStnClickedStaticDownloadCenterAddDeclare()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("http://bbs.i8desk.com/showtopic-26757.aspx"), _T(""), _T(""), SW_SHOWNORMAL);
}


BOOL CDlgDownloadCenterGame::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListRunType_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	wndListSyncTask_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}


