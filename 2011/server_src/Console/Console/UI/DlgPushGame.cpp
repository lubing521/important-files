// DlgPushGame.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgPushGame.h"
#include "DlgAddModify.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/ui/ImageHelpers.h"

#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridColumnTraitText.h"
#include "../Business/PushGameBusiness.h"
#include "../Business/GameBusiness.h"
#include "UIHelper.h"
#include "../MessageBox.h"

using namespace i8desk::data_helper;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgPushGame 对话框

IMPLEMENT_DYNAMIC(CDlgPushGame, CDialog)

CDlgPushGame::CDlgPushGame(CWnd* pParent /*=NULL*/)
	: i8desk::ui::BaseWnd(CDlgPushGame::IDD, pParent)
	, wndListGames_(false)
{
}

CDlgPushGame::~CDlgPushGame()
{
}

void CDlgPushGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PUSHGAME, wndTree_);
	DDX_Control(pDX, IDC_LIST_PUSHGAME_GAMES, wndListGames_);
	DDX_Control(pDX, IDC_LIST_PUSHGAME_CLIENTS, wndListClients_);
	DDX_Control(pDX, IDC_BUTTON_PUSHGAME_MGR, wndBtnMgr_);
	DDX_Control(pDX, IDC_BUTTON_PUSHGAME_ADD, wndBtnTaskAdd_);
	DDX_Control(pDX, IDC_BUTTON_PUSHGAME_DELETE, wndBtnTaskDel_);
	DDX_Control(pDX, IDC_BUTTON_PUSHGAME_MODIFY, wndBtnTaskMod_);
}

BEGIN_EASYSIZE_MAP(CDlgPushGame)   
	EASYSIZE(IDC_TREE_PUSHGAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, 0)
	
	EASYSIZE(IDC_BUTTON_PUSHGAME_ADD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_PUSHGAME_DELETE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_PUSHGAME_MODIFY, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	
	EASYSIZE(IDC_LIST_PUSHGAME_GAMES, IDC_TREE_PUSHGAME, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST_PUSHGAME_CLIENTS, IDC_TREE_PUSHGAME, IDC_LIST_PUSHGAME_GAMES, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgPushGame, i8desk::ui::BaseWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_PUSHGAME_ADD, &CDlgPushGame::OnButtonClickAdd)
	ON_BN_CLICKED(IDC_BUTTON_PUSHGAME_MODIFY, &CDlgPushGame::OnButtonClickModify)
	ON_BN_CLICKED(IDC_BUTTON_PUSHGAME_DELETE, &CDlgPushGame::OnButtonClickDelete)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PUSHGAME, &CDlgPushGame::OnTvnSelchangedTreePushgame)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PUSHGAME_CLIENTS, &CDlgPushGame::OnLvnGetdispinfoListPushgameClients)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PUSHGAME_GAMES, &CDlgPushGame::OnLvnGetdispinfoListPushgames)

	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgPushGame::OnApplySelect)
END_MESSAGE_MAP()

namespace 
{
	template < typename TreeCtrlT, typename PushGamesT >
	void InitTreeCtrl(TreeCtrlT &treeCtrl, const PushGamesT &pushGames)
	{
		treeCtrl.DeleteAllItems();
		HTREEITEM hRootItem = treeCtrl.InsertItem(_T("游戏推送任务"), 0, 0, TVI_ROOT);
		HTREEITEM hFirstItem = 0;
		for(typename PushGamesT::const_iterator iter = pushGames.begin();
			iter != pushGames.end(); ++iter)
		{
			HTREEITEM hTreeItem = treeCtrl.InsertItem(iter->second->Name, -1, 1, hRootItem);
			treeCtrl.SetItemData(hTreeItem, reinterpret_cast<DWORD_PTR>(iter->second.get()));

			if( hFirstItem == 0 )
				hFirstItem = hTreeItem;
		}

		treeCtrl.Expand(hRootItem, TVE_EXPAND);
		treeCtrl.SelectItem(hRootItem);
	}

	CCustomProgressColumnTrait *downloadProgress = 0;

	enum { ROW_SIZE = 10 };

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

		template < typename ListT >
		CCustomProgressColumnTrait *GetProgressTrait(ListT &list)
		{
			CCustomProgressColumnTrait *tmp = new CCustomProgressColumnTrait(list);
			tmp->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			tmp->SetProgressImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingGray.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/LoadingBlue.png")));

			return tmp;
		}


        std::vector<CCustomColumnTraitIconStyle*> rowTraitArray;

		template < typename ListCtrlT, typename GamesT >
		CCustomColumnTraitIconStyle *GetTrait(size_t index, ListCtrlT &listCtrl, GamesT &games)
		{
			assert(index < rowTraitArray.size());
			rowTraitArray[index] = new CCustomColumnTraitIconStyle(listCtrl, games);

			rowTraitArray[index]->SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")));
			rowTraitArray[index]->SetDefaultBmp(i8desk::ui::SkinMgrInstance().GetIcon(0));
			rowTraitArray[index]->SetRowSize(rowTraitArray.size());
			return rowTraitArray[index];
		}

        void ResetTraitRowNum(int iRowNum)
        {
            for (UINT i = 0; i < rowTraitArray.size(); ++i)
            {
                rowTraitArray[i]->SetRowSize(iRowNum);
            }
            rowTraitArray.resize(iRowNum);
        }
    }

    UINT GetTraitRowNum()
    {
        return detail::rowTraitArray.size();
    }


	
	
	template < typename ListCtrlT, typename GamesT >
	void InitListGames(ListCtrlT &listCtrl, GamesT &games)
	{
		
		listCtrl.InsertHiddenLabelColumn();
		listCtrl.SetExtendedStyle(listCtrl.GetExtendedStyle() & ~(LVS_EX_FULLROWSELECT |
			LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP));
	}

    template < typename ListCtrlT, typename GamesT >
    void UpdateColumnListGames(ListCtrlT &listCtrl, GamesT &games)
    {
        CRect rtClient;
        listCtrl.GetClientRect(rtClient);
        int iOriColumnCount = listCtrl.GetHeaderCtrl()->GetItemCount() - 1;
        int iNewColumnCount = rtClient.Width() / 80;
        if (iOriColumnCount > iNewColumnCount)
        {
            listCtrl.ShowWindow(SW_HIDE);
            listCtrl.ShowWindow(SW_SHOW);
            return;
        }
        detail::ResetTraitRowNum(iNewColumnCount);
        if (iOriColumnCount < iNewColumnCount)
        {
            int iLastRightColumn = iOriColumnCount;
            while (iOriColumnCount < iNewColumnCount)
            {
                listCtrl.InsertColumnTrait(iOriColumnCount + 1, _T(""), LVCFMT_LEFT, 80, iOriColumnCount + 1,	detail::GetTrait(iOriColumnCount, listCtrl, games));
                ++iOriColumnCount;
            }
            for (int i = iLastRightColumn; i < iOriColumnCount; ++i)
            {
                listCtrl.SetColumnWidth(i, 80);
            }
        }
    }

	enum 
	{ 
		MASK_PUSH_CLIENTNAME = 1, MASK_PUSH_STATE, MASK_PUSH_GAMENAME, MASK_PUSH_LEFTSIZE,
		MASK_PUSH_UPSIZE, MASK_PUSH_PROGRESS, MASK_PUSH_SPEED, MASK_PUSH_LIMIT
	};

	template< typename ListCtrlT >
	void InitListClients(ListCtrlT &listCtrl)
	{
		CCustomColumnTraitIcon *pIconTrait = new CCustomColumnTraitIcon(listCtrl);
		pIconTrait->SetBkImg(

			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
		pIconTrait->AddIcon(i8desk::ui::SkinMgrInstance().GetSkin(_T("PushGame/Ico_Client.png")));

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_PUSH_CLIENTNAME,	_T("客户机名称"),		LVCFMT_LEFT, 130, MASK_PUSH_CLIENTNAME,	pIconTrait);
		listCtrl.InsertColumnTrait(MASK_PUSH_STATE,			_T("当前状态"),		LVCFMT_LEFT, 80, MASK_PUSH_STATE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PUSH_GAMENAME,		_T("推送游戏"),		LVCFMT_LEFT, 130, MASK_PUSH_GAMENAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PUSH_LEFTSIZE,		_T("剩余量"),			LVCFMT_LEFT, 80, MASK_PUSH_LEFTSIZE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PUSH_UPSIZE,		_T("推送量"),			LVCFMT_LEFT, 80, MASK_PUSH_UPSIZE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PUSH_PROGRESS,		_T("进度"),			LVCFMT_LEFT, 130, MASK_PUSH_PROGRESS,	downloadProgress = detail::GetProgressTrait(listCtrl));
		listCtrl.InsertColumnTrait(MASK_PUSH_SPEED,			_T("当前速度"),		LVCFMT_LEFT, 130, MASK_PUSH_SPEED,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PUSH_LIMIT,			_T("限速"),			LVCFMT_LEFT, 83, MASK_PUSH_LIMIT,		detail::GetTrait());
		
		listCtrl.SetItemHeight(24);
	}
}



void CDlgPushGame::Register()
{
	i8desk::GetRealDataMgr().Register(
		std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_PUSHGAMESTATUS);
}

void CDlgPushGame::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_PUSHGAMESTATUS);
}

void CDlgPushGame::OnRealDataUpdate()
{	
	_ShowView();
}

void CDlgPushGame::OnReConnect()
{
	i8desk::GetDataMgr().GetPushGames().clear();
	i8desk::GetDataMgr().GetGames().clear();
	i8desk::GetDataMgr().GetBootTaskAreas().clear();
}

void CDlgPushGame::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetPushGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTaskAreas());

}

void CDlgPushGame::OnDataComplate()
{
	_Init();
}

void CDlgPushGame::_Init()
{
	const PushGameTraits::MapType &pushGames = i8desk::GetDataMgr().GetPushGames();
	
	InitTreeCtrl(wndTree_, pushGames);
}

void CDlgPushGame::_ShowView()
{
	const PushGameStatusTraits::MapType pushgames = i8desk::GetRealDataMgr().GetPushGameStatus();

	struct Func
	{
		typedef PushGameStatusTraits  PushTraite;
		Func(const stdex::tString &TID,PushTraite::VectorType &pushes)
			: pushes_(pushes)
			, TID_(TID)
		{}
		void operator ()(const PushTraite::MapType::value_type &val ) const
		{
			if( TID_ == val.second->TID )
				pushes_.push_back(val.second);
		}
		const stdex::tString &TID_;
		PushTraite::VectorType &pushes_;
	};

	pushGames_.clear();
	std::for_each(pushgames.begin(), pushgames.end(), Func( TID_, pushGames_ ));


	static size_t count = 0;
	size_t cnt = pushGames_.size();
	if( count != cnt )
	{
		count = cnt;
		wndListClients_.SetItemCount(count);
	}
	else
	{
		wndListClients_.RedrawCurPageItems();
	}
}

void CDlgPushGame::_ChangeList(LPCTSTR TID, const stdex::tString &param)
{
	std::vector<i8desk::ulong> gids;
	stdex::Split(gids, param, _T('|'));

	
	localGames_.clear();

	// 第一个参数是限速大小,第二个参数是对比方式
	const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
	for(size_t i = 2; i != gids.size(); ++i)
	{
		GameTraits::MapConstIterator iter = games.find(gids[i]);
		if( iter != games.end() && iter->second->Status == 1 )
		{
			localGames_.push_back(iter->second);
			i8desk::business::Game::GetIcon(iter->second);
		}
	}

    wndListGames_.SetItemCount(localGames_.size() / GetTraitRowNum() + 1);
}


// CDlgPushGame 消息处理程序

BOOL CDlgPushGame::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	imageTreeList_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	imageTreeList_.Add(CBitmap::FromHandle(i8desk::ui::SkinMgrInstance().GetSkin(_T("PushGame/Ico_Push.png"))), RGB(0, 0, 0));
	wndTree_.SetImageList(&imageTreeList_, TVSIL_NORMAL);
	
	
	// 初始化 List Ctrl
	InitListClients(wndListClients_);
	InitListGames(wndListGames_, localGames_);


	wndListClients_.SetHeaderImage(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));


	// 初始化工作区图片
	HBITMAP rightArea[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Left_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Bottom_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Content_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Top_Line.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Right_Right_Line.png"))
	};
	for(size_t i = 0; i != _countof(rightArea); ++i)
	{
		rightArea_[i].reset(new CImage);
		rightArea_[i]->Attach(rightArea[i]);
	}

	HBITMAP leftArea[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Content_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Bottom_bg.png"))
	};
	for(size_t i = 0; i != _countof(leftArea); ++i)
	{
		leftArea_[i].reset(new CImage);
		leftArea_[i]->Attach(leftArea[i]);
	}

	workOutLine_.reset(new CImage);

	workOutLine_->Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
	// 初始化Button
	HBITMAP add[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Press.png"))
	};
	wndBtnTaskAdd_.SetImages(add[0], add[1], add[2]);

	HBITMAP del[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Press.png"))
	};
	wndBtnTaskDel_.SetImages(del[0], del[1], del[2]);

	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndBtnTaskMod_.SetImages(modify[0], modify[1], modify[2]);

	wndBtnMgr_.SetImages(leftArea[0], leftArea[0], leftArea[0]);

	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgPushGame::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	// 左边工作区
	i8desk::ui::DrawWorkFrame(memDC, rcClient, leftArea_, rightArea_);

	// 右边工作区内容边框
	CRect rcWork;
	wndListClients_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);

	wndListGames_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);
}


void CDlgPushGame::OnDestroy()
{
	CDialog::OnDestroy();

}

void CDlgPushGame::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	UPDATE_EASYSIZE;

	if( !::IsWindow(wndListClients_.GetSafeHwnd()) )
        return;

    UpdateColumnListGames(wndListGames_, localGames_);
    wndListGames_.SetItemCount(localGames_.size() / GetTraitRowNum() + 1);

	wndTree_.Invalidate();
	wndListClients_.Invalidate();
	wndListGames_.Invalidate();

	wndBtnMgr_.Invalidate();
	wndBtnTaskAdd_.Invalidate();
	wndBtnTaskDel_.Invalidate();
	wndBtnTaskMod_.Invalidate();
}

LRESULT CDlgPushGame::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 0 )
		wndTree_.SelectItem(wndTree_.GetRootItem());
	else
		;	// LOG

	return TRUE;
}

void CDlgPushGame::OnTvnSelchangedTreePushgame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	HTREEITEM hItem = wndTree_.GetSelectedItem();
	typedef PushGameTraits::ElementType PushGameValType;
	PushGameValType *val = reinterpret_cast< PushGameValType *>(wndTree_.GetItemData(hItem));
	if( val != 0 )
	{
		_ChangeList(val->TID, val->Parameter);
		TID_ = val->TID;
	}
	else
		wndListGames_.SetItemCount(0);
		
}


void CDlgPushGame::OnLvnGetdispinfoListPushgameClients(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= pushGames_.size() )
		return;

	const PushGameStatusTraits::ValueType &val = pushGames_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_PUSH_CLIENTNAME:
			text = val->ClientName;
			break;
		case MASK_PUSH_STATE:
			text = _T("进行中");
			break;
		case MASK_PUSH_GAMENAME:
			text = val->GameName;
			break;
		case MASK_PUSH_LEFTSIZE:
			i8desk::FormatSize(val->SizeLeft, text);
			break;
		case MASK_PUSH_UPSIZE:
			i8desk::FormatSize(val->UpdateSize, text);
			break;
		case MASK_PUSH_PROGRESS:
			{
				double progress = (val->UpdateSize - val->SizeLeft) * 100.0 / val->UpdateSize;
				downloadProgress->SetProgress(itemIndex, (size_t)progress);

				stdex::ToString(progress, text, 0);
				text += _T("%");
			}
			break;
		case MASK_PUSH_SPEED:
			i8desk::FormatSize(val->TransferRate, text);
			text += _T("/S");
			break;
		case MASK_PUSH_LIMIT:
			{
				const PushGameTraits::MapType &pushGames = i8desk::GetDataMgr().GetPushGames();
				PushGameTraits::MapConstIterator iter = pushGames.find(val->TID);
				if( iter != pushGames.end() )
				{
					static stdex::tString limit; 
					limit = iter->second->Parameter;
					size_t pos = limit.find(_T('|'));
					if( stdex::tString::npos != pos )
						limit = limit.substr(0, pos);
					else
						limit.clear();

					text = limit;
					text += _T(" KB/S");
				}
				else
					text.clear();
			}
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgPushGame::OnLvnGetdispinfoListPushgames(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
}

void CDlgPushGame::OnButtonClickModify()
{
	PushGameTraits::ElementType *val = 0;
	val = reinterpret_cast<PushGameTraits::ElementType*>(wndTree_.GetItemData(wndTree_.GetSelectedItem()));

	if( val == 0 )
		return;

	CDlgPushGameAddModify dlg(false,val);
	if(dlg.DoModal() == IDOK )
		InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetPushGames());
}

void CDlgPushGame::OnButtonClickAdd()
{
	CDlgPushGameAddModify dlg(true,0);
	if(dlg.DoModal() == IDOK )
		InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetPushGames());
}

void CDlgPushGame::OnButtonClickDelete()
{
	PushGameTraits::ElementType *val = 0;
	val = reinterpret_cast<PushGameTraits::ElementType*>(wndTree_.GetItemData(wndTree_.GetSelectedItem()));

	if( val == 0 )
		return;

	stdex::tString msg = _T("是否需要删除游戏推送任务: ");
	msg += val->Name;
	CMessageBox msgDlg(_T("提示"),msg);

	if( msgDlg.DoModal() == IDOK )
	{
		if( !i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetPushGames(), val->TID) )
			return;

		InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetPushGames());
		wndTree_.SelectItem(wndTree_.GetRootItem());
	}
}