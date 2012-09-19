#include "stdafx.h"
#include "../ManagerInstance.h"

#include "DlgDownloadGameViewClient.h"
#include "../ui/Skin/SkinMgr.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../UI/UIHelper.h"
#include "../../../../include/Utility/utility.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomColumnTrait *GetTrait()
{
	CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
	rowTrait->SetBkImg(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

	return rowTrait;
}
// CDlgClientViewGame 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadGameViewClient, CNonFrameChildDlg)

CDlgDownloadGameViewClient::CDlgDownloadGameViewClient(long gid,CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadGameViewClient::IDD, pParent)
	, gid_(gid)
{

}

CDlgDownloadGameViewClient::~CDlgDownloadGameViewClient()
{
}

void CDlgDownloadGameViewClient::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_VIEWGAMECLIENT, wndListView_);

}


BEGIN_MESSAGE_MAP(CDlgDownloadGameViewClient, CNonFrameChildDlg)
	
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_VIEWGAMECLIENT, &CDlgDownloadGameViewClient::OnLvnGetdispinfoListViewGameClient)

END_MESSAGE_MAP()

enum { MASK_NAME = 1, MASK_IP, MASK_UPDATETIME };


BOOL CDlgDownloadGameViewClient::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();

	SetTitle(_T("查看游戏信息"));

	wndListView_.SetHeaderImage(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderBg.png")), 
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ListHeaderLine.png")));

	wndListView_.InsertHiddenLabelColumn();

	wndListView_.InsertColumnTrait(MASK_NAME,		TEXT("机器名"),		LVCFMT_LEFT, 100,	MASK_NAME,			GetTrait());
	wndListView_.InsertColumnTrait(MASK_IP,			TEXT("机器IP"),		LVCFMT_LEFT, 80,	MASK_IP,			GetTrait());
	wndListView_.InsertColumnTrait(MASK_UPDATETIME,	TEXT("更新时间"),		LVCFMT_LEFT, 180,	MASK_UPDATETIME,	GetTrait());

	if ( !i8desk::GetControlMgr().GetViewClient( gid_, curviewClients_ )) 
		return FALSE; 

	_ShowView();
	// Out Line

	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));


	return TRUE; 
}

void CDlgDownloadGameViewClient::_ShowView()
{
	static size_t count = 0;
	size_t cnt = curviewClients_.size();
	if( count != cnt )
	{
		count = cnt;
		wndListView_.SetItemCount(count);
	}
	else
	{
		wndListView_.RedrawCurPageItems();
	}

}

void CDlgDownloadGameViewClient::OnLvnGetdispinfoListViewGameClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curviewClients_.size() )
		return;

	const i8desk::data_helper::ViewClientTraits::ValueType &val = curviewClients_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = val->ClientName;
			break;
		case MASK_IP:
			text = i8desk::IP2String(val->Ip);
			break;
		case MASK_UPDATETIME:
			text = i8desk::FormatTime(val->UpdateTime);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

BOOL CDlgDownloadGameViewClient::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListView_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	return TRUE;
}


