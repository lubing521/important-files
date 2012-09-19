// WZDlgPushGame.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgPushGame.h"
#include "../Business/PushGameBusiness.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/I8Type.hpp"
#include "../../../../include/Utility/utility.h"
#include "DlgAddModify.h"
#include "../MessageBox.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"

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
		MASK_PUSH_NAME = 1, MASK_PUSH_AREA, MASK_PUSH_MAXSPEED, MASK_PUSH_CHECKTYPE, MASK_PUSH_GID
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_PUSH_NAME,		_T("名称"),		LVCFMT_LEFT, 80, MASK_PUSH_NAME, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_PUSH_AREA,		_T("区域"),		LVCFMT_LEFT, 80, MASK_PUSH_AREA, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_PUSH_MAXSPEED,	_T("限速"),		LVCFMT_LEFT, 80, MASK_PUSH_MAXSPEED, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_PUSH_CHECKTYPE,	_T("对比方式"),	LVCFMT_LEFT, 100, MASK_PUSH_CHECKTYPE, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_PUSH_GID,		_T("推送游戏"),	LVCFMT_LEFT, 80, MASK_PUSH_GID, detail::GetTrait() );
	}
}

// WZDlgPushGame 对话框

IMPLEMENT_DYNAMIC(WZDlgPushGame, CDialog)

WZDlgPushGame::WZDlgPushGame(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgPushGame::IDD, pParent)
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgPushGame::~WZDlgPushGame()
{
	workOutLine_.Detach();
}

void WZDlgPushGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_WZ_PUSHGAME_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_PUSHGAME_DEL, wndbtnDel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_PUSHGAME_MODIFY, wndbtnModify_);

	DDX_Control(pDX, IDC_LIST_WZ_PUSHGAME, wndListPushGames_);

}


BEGIN_MESSAGE_MAP(WZDlgPushGame, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_PUSHGAME_ADD, &WZDlgPushGame::OnBnClickedButtonPushGameAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_PUSHGAME_DEL, &WZDlgPushGame::OnBnClickedButtonPushGameDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_PUSHGAME_MODIFY, &WZDlgPushGame::OnBnClickedButtonPushGameModify)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_PUSHGAME, &WZDlgPushGame::OnLvnGetdispinfoListPushGameInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WZ_PUSHGAME, &WZDlgPushGame::OnNMRDblclkListPushGameInfo)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgPushGame 消息处理程序
BOOL WZDlgPushGame::OnInitDialog()
{
	CDialog::OnInitDialog();

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetPushGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetBootTaskAreas());

	InitListCtrl(wndListPushGames_);

	_ShowView();

	return TRUE;
}

void WZDlgPushGame::OnBnClickedButtonPushGameAdd()
{
	CDlgPushGameAddModify dlg(true,0);
	if( dlg.DoModal() == IDOK )
		_ShowView();
}

void WZDlgPushGame::OnBnClickedButtonPushGameDel()
{
	if( wndListPushGames_.GetSelectedCount() == 0 )
		return;
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的推送任务吗?"));

	if( msgDlg.DoModal() == IDCANCEL)
		return;

	int nSel = -1;


	typedef std::vector<DWORD> SelPushGame;
	SelPushGame vecIdx;
	while((nSel = wndListPushGames_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		vecIdx.push_back(nSel);
	}

	for(SelPushGame::const_iterator rit = vecIdx.begin(); rit != vecIdx.end(); ++rit)
	{
		nSel = *rit;
		if( !i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetPushGames(), curPushGames_[nSel]->TID) )
			break;
	}

	_ShowView();

}

void WZDlgPushGame::OnBnClickedButtonPushGameModify()
{
	int nSel = wndListPushGames_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return;

	assert(nSel < (int)curPushGames_.size() );

	CDlgPushGameAddModify dlg(false,curPushGames_[nSel].get());
	if(dlg.DoModal() == IDOK )
		_ShowView();
}

void WZDlgPushGame::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::PushGameTraits;

	curPushGames_.clear();

	const PushGameTraits::MapType &tasks = i8desk::GetDataMgr().GetPushGames();
	stdex::map2vector(tasks, curPushGames_);

	size_t count = curPushGames_.size();
	wndListPushGames_.SetItemCount(count);
}

void WZDlgPushGame::OnNMRDblclkListPushGameInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	OnBnClickedButtonPushGameModify();
}


void WZDlgPushGame::OnLvnGetdispinfoListPushGameInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curPushGames_.size() )
		return;

	const i8desk::data_helper::PushGameTraits::ValueType &val = curPushGames_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_PUSH_NAME:
			text = val->Name;
			break;
		case MASK_PUSH_AREA:
			text = i8desk::business::PushGame::GetAreaDesc(val);
			break;
		case MASK_PUSH_MAXSPEED:
			text = i8desk::business::PushGame::GetParameterDesc(val,i8desk::fst_maxspeed);
			break;
		case MASK_PUSH_CHECKTYPE:
			text = i8desk::business::PushGame::GetParameterDesc(val,i8desk::scd_cheecktype);
			break;
		case MASK_PUSH_GID:
			text = i8desk::business::PushGame::GetParameterDesc(val,i8desk::trd_gid);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void WZDlgPushGame::OnShow(int showtype)
{
	ShowWindow(showtype);

}


BOOL WZDlgPushGame::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndListPushGames_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &workOutLine_);

	return TRUE;

}