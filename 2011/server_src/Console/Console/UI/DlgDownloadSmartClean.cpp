// DlgDownloadSmartClean.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadSmartClean.h"

#include <set>

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#include "../UI/UIHelper.h"
#include "../Business/GameBusiness.h"
#include "../Business/ClassBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/AreaBusiness.h"



#include "../ManagerInstance.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"
#include "../DlgWaitMsg.h"

// CDlgDownloadSmartClean 对话框

struct CDlgDownloadSmartClean::CleanInfo
{
	int keepDay_;
	bool greateHundred_;
	TCHAR drv_;
	size_t size_;
};

IMPLEMENT_DYNAMIC(CDlgDownloadSmartClean, CNonFrameChildDlg)

CDlgDownloadSmartClean::CDlgDownloadSmartClean(TCHAR drv, i8desk::ulonglong size, CWnd* pParent /*=NULL*/)
	: CNonFrameChildDlg(CDlgDownloadSmartClean::IDD, pParent)
	, drv_(drv)
	, size_(size)
{
	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));
}

CDlgDownloadSmartClean::~CDlgDownloadSmartClean()
{
	outboxLine_.Detach();
}

void CDlgDownloadSmartClean::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_SMART_CLEAN_DRV, wndTipDrv_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_SMART_CLEAN_SPACE, wndTipSpace_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_SMART_CLEAN_SPACE_CLEAN, wndTipSpaceClean_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_SMART_CLEAN_DRV, wndComboDrv_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_SMART_CLEAN_SPACE, wndEditSpace_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_SMART_CLEAN_DATE, wndComboKeepDay_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_SMART_CLEAN_GREATER_HUNDRED, wndCheckGreateHundred_);
	DDX_Control(pDX, IDC_CHECK_DOWNLOAD_SMART_CLEAN_ALL, wndCheckAll_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_SMART_CLEAN_CLEAN, wndBtnClean_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_SMART_CLEAN_GENERATE, wndBtnGenerate_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_SMART_CLEAN_, wndListClean_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_SMART_CLEAN_TOTAL_SIZE, wndTipTotalSize_);
}


BEGIN_MESSAGE_MAP(CDlgDownloadSmartClean, CNonFrameChildDlg)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_SMART_CLEAN_, &CDlgDownloadSmartClean::OnLvnGetdispinfoListClean)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_SMART_CLEAN_GENERATE, &CDlgDownloadSmartClean::OnBnClickedButtonDownloadSmartCleanGenerate)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_SMART_CLEAN_CLEAN, &CDlgDownloadSmartClean::OnBnClickedButtonDownloadSmartCleanClean)
	ON_BN_CLICKED(IDC_CHECK_DOWNLOAD_SMART_CLEAN_ALL, &CDlgDownloadSmartClean::OnBnClickedCheckDownloadSmartCleanAll)
END_MESSAGE_MAP()


namespace
{
	
	CCustomColumnTrait *GetTrait()
	{
		CCustomColumnTrait *trait = new CCustomColumnTrait();
		trait->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return trait;
	}

	CCustomColumnSmartCleanCheckTrait *checkTrait = 0;
	template < typename ListT >
	CCustomColumnSmartCleanCheckTrait *GetCheckTrait(ListT &list)
	{
		CCustomColumnSmartCleanCheckTrait *rowTrait = new CCustomColumnSmartCleanCheckTrait(list);
		rowTrait->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return rowTrait;
	}

	enum
	{
		MASK_CHECK = 1, MASK_NAME, MASK_GID, MASK_CLASS, MASK_UPDATE_DATE, MASK_SIZE, 
		MASK_LOCAL_CLICK, MASK_SVR_CLICK, MASK_SOURCE
	};

	template < typename ListCtrlT >
	void InitListCtrl(ListCtrlT &list)
	{
		list.InsertHiddenLabelColumn();

		list.InsertColumnTrait(MASK_CHECK,		_T(""),			LVCFMT_LEFT, 25,	MASK_CHECK,		checkTrait = GetCheckTrait(list));
		list.InsertColumnTrait(MASK_NAME,		_T("游戏名称"),	LVCFMT_LEFT, 140,	MASK_NAME,		GetTrait());
		list.InsertColumnTrait(MASK_GID,		_T("GID"),		LVCFMT_LEFT, 50,	MASK_GID,		GetTrait());
		list.InsertColumnTrait(MASK_CLASS,		_T("游戏类别"),	LVCFMT_LEFT, 100,	MASK_CLASS,		GetTrait());
		list.InsertColumnTrait(MASK_UPDATE_DATE,_T("本地更新时间"),LVCFMT_LEFT, 120,	MASK_UPDATE_DATE,GetTrait());
		list.InsertColumnTrait(MASK_SIZE,		_T("大小"),		LVCFMT_LEFT, 70,	MASK_SIZE,		GetTrait());
		list.InsertColumnTrait(MASK_LOCAL_CLICK,_T("本地热度"),	LVCFMT_LEFT, 80,	MASK_LOCAL_CLICK,GetTrait());
		list.InsertColumnTrait(MASK_SVR_CLICK,	_T("中心热度"),	LVCFMT_LEFT, 80,	MASK_SVR_CLICK,	GetTrait());
		list.InsertColumnTrait(MASK_SOURCE,		_T("来源"),		LVCFMT_LEFT, 76,	MASK_SOURCE,	GetTrait());
	
		list.SetItemHeight(24);
	}
}

bool CDlgDownloadSmartClean::_GetData(CleanInfo &cleanInfo)
{
	CString drv;
	wndComboDrv_.GetWindowText(drv);
	cleanInfo.drv_ = drv[0];

	CString sizeText;
	wndEditSpace_.GetWindowText(sizeText);
	if( sizeText.IsEmpty() )
	{
		CMessageBox box(_T("提示"), _T("没有输入需要删除空间大小"));
		box.DoModal();
		wndEditSpace_.SetFocus();
		return false;
	}
	cleanInfo.size_ = stdex::ToNumber<size_t>(stdex::tString(sizeText));

	cleanInfo.keepDay_ = wndComboKeepDay_.GetItemData(wndComboKeepDay_.GetCurSel());
	cleanInfo.greateHundred_ = wndCheckGreateHundred_.GetCheck() == BST_CHECKED;

	return true;
}

void CDlgDownloadSmartClean::_OnCheck(size_t row)
{
	i8desk::data_helper::GameTraits::VectorType selGames;
	selGames.reserve(curGames_.size());

	for(size_t i = 0; i != curGames_.size(); ++i)
	{
		if( row == i && !checkTrait->GetCheck(row) )
			continue;

		if( checkTrait->GetCheck(i) )
			selGames.push_back(curGames_[i]);
	}

	_SetTip(selGames);
}


void CDlgDownloadSmartClean::_SetTip(i8desk::data_helper::GameTraits::VectorType &selGames)
{
	i8desk::ulonglong totalSize = i8desk::business::Game::GetSize(curGames_);
	stdex::tString totalSizeText;
	i8desk::FormatSize(totalSize * 1024LL, totalSizeText);

	i8desk::ulonglong selSize = i8desk::business::Game::GetSize(selGames);
	stdex::tString selSizeText;
	i8desk::FormatSize(selSize * 1024LL, selSizeText);

	stdex::tOstringstream os;
	os << _T("目前列表资源总大小: ") << totalSizeText
		<< _T("        已选中的资源大小: ") << selSizeText;

	wndTipTotalSize_.SetWindowText(os.str().c_str());
}

// CDlgDownloadSmartClean 消息处理程序

BOOL CDlgDownloadSmartClean::OnInitDialog()
{
	__super::OnInitDialog();

	SetTitle(_T("智能删除"));

	// Drivers
	std::set<TCHAR> drvs;
	i8desk::business::Game::GetLocalDrv(drvs);
    if (drv_ != 0)
    {
	    drvs.insert(drv_);
    }
	for(std::set<TCHAR>::const_iterator iter = drvs.begin(); iter != drvs.end(); ++iter)
		wndComboDrv_.AddString(CString(*iter));
	wndComboDrv_.SetCurSel(0);

	if( drv_ != 0 )
		 wndComboDrv_.SelectString(-1, CString(drv_));

	// Space
	wndEditSpace_.SetWindowText(_T("10"));
	if( size_ != 0 )
	{
		stdex::tString text;
		stdex::ToString(size_ * 1.0 / 1024 / 1024 , text, 2);
		wndEditSpace_.SetWindowText(text.c_str());
	}

	// 保留日期
    i8desk::business::Game::KeepDayDesc(
        std::tr1::bind(&CComboBox::AddString, &wndComboKeepDay_, std::tr1::placeholders::_1),
        std::tr1::bind(&CComboBox::SetItemData, &wndComboKeepDay_, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    wndComboKeepDay_.SetCurSel(1); //默认15天

	// 大于100MB
	wndCheckGreateHundred_.SetCheck(TRUE);

	// List
	InitListCtrl(wndListClean_);
	using namespace std::tr1::placeholders;
	checkTrait->RegisterCallback(std::tr1::bind(&CDlgDownloadSmartClean::_OnCheck, this, _1));

	
	assert(!drvs.empty());
	wndCheckAll_.SetCheck(BST_CHECKED);
	OnBnClickedButtonDownloadSmartCleanGenerate();
	

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDownloadSmartClean::OnDestroy()
{
	__super::OnDestroy();

}

BOOL CDlgDownloadSmartClean::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListClean_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}


void CDlgDownloadSmartClean::OnLvnGetdispinfoListClean(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	const i8desk::data_helper::GameTraits::ValueType &val = curGames_[itemIndex];
	static stdex::tString text;
	text.clear();
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_CHECK:
			text = _T("--");
			break;
		case MASK_NAME:
			text = val->Name;
			break;
		case MASK_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_CLASS:
			text = i8desk::business::Class::GetName(val->CID);
			break;
		case MASK_UPDATE_DATE:
			text = i8desk::FormatTime(val->SvrVer);
			break;
		case MASK_SIZE:
			i8desk::FormatSize(val->Size * 1024, text);
			break;
		case MASK_LOCAL_CLICK:
			stdex::ToString(val->SvrClick, text);
			break;
		case MASK_SVR_CLICK:
			stdex::ToString(val->IdcClick, text);
			break;
		case MASK_SOURCE:
			text = val->Memo;
			break;
		default:
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}


void CDlgDownloadSmartClean::OnBnClickedButtonDownloadSmartCleanGenerate()
{
	// 保留天数
	int iKeepDay = wndComboKeepDay_.GetItemData(wndComboKeepDay_.GetCurSel());

	// 100MB
	bool greateHundred = wndCheckGreateHundred_.GetCheck() == BST_CHECKED;

	// Size
	i8desk::ulonglong size = 0;
	CString sizeText;
	wndEditSpace_.GetWindowText(sizeText);
	stdex::ToNumber<i8desk::ulonglong, TCHAR>(size, (LPCTSTR)sizeText);
	if( size <= 0 )
	{
		CMessageBox box(_T("提示"), _T("请输入有效磁盘空间大小"));
		box.DoModal();	
		wndEditSpace_.SetFocus();
		return;
	}
	size *= 1024LL * 1024LL;	// 中心游戏大小按照KB为单位

	// Driver
	CString drvText;
	wndComboDrv_.GetWindowText(drvText);
	if( drvText.IsEmpty() )
	{
		CMessageBox box(_T("提示"), _T("请选择磁盘盘符"));
		box.DoModal();	
		wndComboDrv_.SetFocus();
		return;
	}
	TCHAR drv = drvText.GetAt(0);

	curGames_.clear();
	i8desk::business::Game::GetCleanGames(curGames_, drv, size, iKeepDay, greateHundred);
	
	wndListClean_.SetItemCount(curGames_.size());
	checkTrait->Resize(curGames_.size());
	
	OnBnClickedCheckDownloadSmartCleanAll();
	
}

void CDlgDownloadSmartClean::OnBnClickedButtonDownloadSmartCleanClean()
{
	if(curGames_.size() == 0)
		return;

	std::vector<long> gids;
	for(size_t i = 0; i != curGames_.size(); ++i)
	{
		if( checkTrait->GetCheck(i) )
			gids.push_back(curGames_[i]->GID);
	}

	struct OP
	{
		const std::vector<long> &gids_;
		OP(const std::vector<long> &gids)
			: gids_(gids)
		{}

		void operator()()
		{
			i8desk::GetControlMgr().NotifySmartClean(gids_);
			i8desk::data_helper::GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
			for(size_t i = 0; i != gids_.size(); ++i)
			{
				const i8desk::data_helper::GameTraits::ValueType &game = games[gids_[i]];

				i8desk::business::Game::Delete(game);
			}
		}
	};
	CDlgWaitMsg dlg;
	dlg.Exec(OP(gids));

	
	OnBnClickedButtonDownloadSmartCleanGenerate();
}

void CDlgDownloadSmartClean::OnBnClickedCheckDownloadSmartCleanAll()
{
	bool flag = wndCheckAll_.GetCheck() == BST_CHECKED;
	checkTrait->SelectAll(flag);

	if( flag )
	{
		_SetTip(curGames_);
	}
	else
	{
		i8desk::data_helper::GameTraits::VectorType emptyGames;
		_SetTip(emptyGames);
	}
}
