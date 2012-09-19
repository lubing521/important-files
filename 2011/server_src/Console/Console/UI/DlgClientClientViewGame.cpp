#include "stdafx.h"
#include "../ManagerInstance.h"

#include "DlgClientClientViewGame.h"
#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgClientClientViewGame 对话框

IMPLEMENT_DYNAMIC(CDlgClientClientViewGame, CNonFrameChildDlg)

CDlgClientClientViewGame::CDlgClientClientViewGame(unsigned long IP,CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgClientClientViewGame::IDD, pParent)
	, IP_(IP)
{

}

CDlgClientClientViewGame::~CDlgClientClientViewGame()
{
}

void CDlgClientClientViewGame::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT_VIEWCLIENTGAME, wndListView_);

}
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

BEGIN_MESSAGE_MAP(CDlgClientClientViewGame, CNonFrameChildDlg)
	
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_VIEWCLIENTGAME, &CDlgClientClientViewGame::OnLvnGetdispinfoListViewGameClient)

END_MESSAGE_MAP()

enum { MASK_NAME = 1, MASK_GID, MASK_UPDATETIME };

BOOL CDlgClientClientViewGame::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	SetTitle(_T("查看游戏信息"));
	// 初始化ListCtrl
	wndListView_.SetHeaderImage(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));


	wndListView_.InsertHiddenLabelColumn();

	wndListView_.InsertColumnTrait(MASK_NAME,		TEXT("游戏名"),		LVCFMT_LEFT, 100,MASK_NAME, GetTrait());
	wndListView_.InsertColumnTrait(MASK_GID,		TEXT("GID"),		LVCFMT_LEFT, 67, MASK_GID, GetTrait());
	wndListView_.InsertColumnTrait(MASK_UPDATETIME,	TEXT("更新时间"),	LVCFMT_LEFT, 180,MASK_UPDATETIME,GetTrait());

	if ( !i8desk::GetControlMgr().GetViewGame( IP_, curviewGames_ )) 
		return FALSE; 

	_ShowView();

	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));


	return TRUE; 
}

void CDlgClientClientViewGame::_ShowView()
{
	size_t cnt = curviewGames_.size();
	wndListView_.SetItemCount(cnt);
}

void CDlgClientClientViewGame::OnLvnGetdispinfoListViewGameClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curviewGames_.size() )
		return;

	const i8desk::data_helper::ViewGameTraits::ValueType &val = curviewGames_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = val->GameName;
			break;
		case MASK_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_UPDATETIME:
			text = i8desk::FormatTime(val->UpdateTime);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

BOOL CDlgClientClientViewGame::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListView_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	return TRUE;
}

