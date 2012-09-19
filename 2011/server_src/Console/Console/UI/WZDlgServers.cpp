// WZDlgMainServer.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgServers.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Utility/utility.h"
#include "../MessageBox.h"
#include "../Misc.h"
#include "../Business/ServerBusiness.h"
#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"

// WZDlgMainServer 对话框

IMPLEMENT_DYNAMIC(WZDlgServers, CDialog)

WZDlgServers::WZDlgServers(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgServers::IDD, pParent)
	, serverName_(_T(""))
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgServers::~WZDlgServers()
{
	workOutLine_.Detach();
}

void WZDlgServers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WZ_SERVER_AREA, wndTreeArea_);

	DDX_Text(pDX, IDC_EDIT_WZ_SVR_SVRNAME, serverName_);

	DDX_Control(pDX, IDC_EDIT_WZ_SVR_SVRNAME, wndEditSvrName_);
	DDX_Control(pDX, IDC_COMBO_WZ_SVR_SVRIP, wndComboSvrIP_);

	DDX_Control(pDX, IDC_RADIO_WZ_SERVER_DISTRIBUTE, wndradioDistribute_);
	DDX_Control(pDX, IDC_RADIO_WZ_SERVER_PRIORITY_AREA, wndradioAreaPriority_);
	DDX_Control(pDX, IDC_RADIO_WZ_SERVER_AUTOACTION, wndradioDynamic_);

	DDX_Control(pDX, IDC_BUTTON_WZ_SERVER_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_SERVER_DEL, wndbtnDel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_SERVER_MODIFY, wndbtnModify_);
	DDX_Control(pDX, IDC_STATIC_WZ_SERVER_NAME, wndServerName_);
	DDX_Control(pDX, IDC_LIST_WZ_SERVER_SERVERS, wndlistServers_);

	DDX_Control(pDX, IDC_STATIC_WZ_SERVER_NAME_DEFAULT_IP, wndLabelDefaultIP_);
}


BEGIN_MESSAGE_MAP(WZDlgServers, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SERVER_ADD, &WZDlgServers::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SERVER_DEL, &WZDlgServers::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_SERVER_MODIFY, &WZDlgServers::OnBnClickedButtonModify)

	ON_BN_CLICKED(IDC_RADIO_WZ_SERVER_AUTOACTION, &WZDlgServers::OnBnClickedRadioServerInfoDynamic)
	ON_BN_CLICKED(IDC_RADIO_WZ_SERVER_PRIORITY_AREA, &WZDlgServers::OnBnClickedRadioServerInfoPriorityArea)
	ON_BN_CLICKED(IDC_RADIO_WZ_SERVER_DISTRIBUTE, &WZDlgServers::OnBnClickedRadioServerInfoDistribute)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_SERVER_SERVERS, &WZDlgServers::OnLvnGetdispinfoListServerInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WZ_SERVER_SERVERS, &WZDlgServers::OnLvnItemchangedListServerInfo)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

namespace
{
	const TCHAR *RT_DISTRIBUTE			= _T("轮询分配");
	const TCHAR *RT_PRIORITY_AREA		= _T("区域优先");


	template< typename TreeCtrlT, typename AreasT >
	void InitTree(TreeCtrlT &tree, const AreasT &areas)
	{
		tree.ModifyStyle(TVS_CHECKBOXES, 0);
		tree.ModifyStyle(0, TVS_CHECKBOXES);

		tree.DeleteAllItems();
		for(typename AreasT::const_iterator iter = areas.begin();
			iter != areas.end(); ++iter)
		{
			HTREEITEM hItem = tree.InsertItem(iter->second->Name);
			tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(iter->second.get()));
		}

		tree.SetCheck(tree.GetRootItem());
	}

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
		MASK_SERVER_NAME = 1, MASK_SERVER_DEFAULT_IP, MASK_SERVER_AREAMODE
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_SERVER_NAME,		_T("服务器名称"),	LVCFMT_LEFT, 200, MASK_SERVER_NAME,			detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SERVER_DEFAULT_IP,	_T("默认IP"),	LVCFMT_LEFT, 120, MASK_SERVER_DEFAULT_IP,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_SERVER_AREAMODE,	_T("运行模式"),	LVCFMT_LEFT, 110, MASK_SERVER_AREAMODE,		detail::GetTrait() );
	}
}

// WZDlgMainServer 消息处理程序
BOOL WZDlgServers::OnInitDialog()
{
	CDialog::OnInitDialog();

	wndradioDistribute_.SetCheck(FALSE);
	wndradioAreaPriority_.SetCheck(TRUE);
	wndTreeArea_.EnableWindow(TRUE);

	InitListCtrl(wndlistServers_);

	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

	stdex::map2vector(servers, curservers_);

	if( servers.empty() )
		return FALSE;

	if( curservers_[0]->BalanceType == i8desk::BALANCE_POLL || curservers_[0]->BalanceType == i8desk::BALANCE_UNKNOWN )
	{
		wndradioDistribute_.SetCheck(TRUE);
		wndradioAreaPriority_.SetCheck(FALSE);
		wndTreeArea_.EnableWindow(FALSE);
	}
	if(curservers_[0]->BalanceType == i8desk::BALANCE_AREAPRIORITY)
	{
		wndradioDistribute_.SetCheck(FALSE);
		wndradioAreaPriority_.SetCheck(TRUE);
		wndTreeArea_.EnableWindow(TRUE);
	}

	if( curservers_.size() != 0 )
	{
		// 初始化服务器默认IP
		if( curservers_[0]->Ip1 != 0 )
			wndComboSvrIP_.AddString(i8desk::IP2String(curservers_[0]->Ip1).c_str());
		if( curservers_[0]->Ip2 != 0 )
			wndComboSvrIP_.AddString(i8desk::IP2String(curservers_[0]->Ip2).c_str());
		if( curservers_[0]->Ip3 != 0 )
			wndComboSvrIP_.AddString(i8desk::IP2String(curservers_[0]->Ip3).c_str());
		if( curservers_[0]->Ip4 != 0 )
			wndComboSvrIP_.AddString(i8desk::IP2String(curservers_[0]->Ip4).c_str());
	}


	serverName_ = curservers_[0]->SvrName;
	wndComboSvrIP_.SelectString(-1, i8desk::IP2String(curservers_[0]->SvrIP).c_str());


	return TRUE;
}

void WZDlgServers::_ReadDatas()
{
	curservers_.clear();
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetServers());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetAreas());

	i8desk::data_helper::AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);

	InitTree(wndTreeArea_, areas);

	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

	stdex::map2vector(servers, curservers_);


	_ShowView();
	UpdateData(FALSE);
}

void WZDlgServers::OnBnClickedRadioServerInfoDynamic()
{
	wndTreeArea_.EnableWindow(FALSE);
}

void WZDlgServers::OnBnClickedRadioServerInfoDistribute()
{
	wndTreeArea_.EnableWindow(FALSE);
}

void WZDlgServers::OnBnClickedRadioServerInfoPriorityArea()
{
	wndTreeArea_.EnableWindow(TRUE);
}

bool WZDlgServers::_CheckData(i8desk::data_helper::ServerTraits::ValueType &val)
{
	UpdateData(TRUE);

	// 名称
	if( utility::IsEmpty(serverName_) ) 
	{
		CMessageBox msgDlg(_T("提示"),_T("没有设定服务器名称"));
		msgDlg.DoModal();
		return false;
	}


	if( i8desk::business::Server::HasSameName(val->SvrID, (LPCTSTR)serverName_) )
	{
		stdex::tString msg = _T("设定服务器名称冲突: ");
		msg += serverName_;
		CMessageBox msgDlg(_T("服务器名称"), msg);
		msgDlg.DoModal();

		return false;
	}

	utility::Strcpy(val->SvrName, serverName_);

	// IP
	CString defIP;
	wndComboSvrIP_.GetWindowText(defIP);

	if( defIP.IsEmpty() || !i8desk::IsValidIP(defIP) )
	{
		CMessageBox msgDlg(_T("提示"),_T("默认IP不正确,请重新输入"));
		msgDlg.DoModal();
		return false;
	}
	val->SvrIP	= ::htonl(i8desk::String2IP((LPCTSTR)defIP));
	val->Ip1	= val->SvrIP;

	// BalanceType
	if( wndradioDistribute_.GetCheck() )
		val->BalanceType = i8desk::data_helper::BALANCE_POLL;
	else if ( wndradioAreaPriority_.GetCheck() )
		val->BalanceType = i8desk::data_helper::BALANCE_AREAPRIORITY;
	else
		val->BalanceType = i8desk::data_helper::BALANCE_DYNAMIC;

	bool IsSelect = false;
	if( val->BalanceType == i8desk::data_helper::BALANCE_AREAPRIORITY )
	{
		for(HTREEITEM hItem = wndTreeArea_.GetRootItem(); 
			hItem != NULL; hItem = wndTreeArea_.GetNextSiblingItem(hItem)) 
		{
			if( wndTreeArea_.GetCheck(hItem) == TRUE )
				IsSelect = true;
		}

		if( !IsSelect )
		{
			CMessageBox msgDlg(_T("提示"),_T("没有选择分区!"));
			msgDlg.DoModal();
			return false;
		}
	}

	return true;
}


bool WZDlgServers::_ModifyPriorityArea(LPCTSTR svrID)
{
	for(HTREEITEM hItem = wndTreeArea_.GetRootItem(); 
		hItem != NULL; hItem = wndTreeArea_.GetNextSiblingItem(hItem)) 
	{
		typedef i8desk::data_helper::AreaTraits::ElementType AreaType;
		AreaType *val = reinterpret_cast<AreaType *>(wndTreeArea_.GetItemData(hItem));

		bool suc = i8desk::business::Server::ModifyAreaPriority(wndTreeArea_.GetCheck(hItem) == TRUE ? true : false, val, svrID);
		if( !suc )
			return false;
	}

	return true;
}


void WZDlgServers::OnBnClickedButtonAdd()
{
	CMessageBox msgDlg(_T("提示"), _T("从服务器连接上主服务后会自动添加"));

	msgDlg.DoModal();
	
}

void WZDlgServers::OnBnClickedButtonDel()
{
	int nSel = wndlistServers_.GetNextItem(-1, LVIS_SELECTED);
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的项吗?"));

	if( nSel != -1 && msgDlg.DoModal() == IDOK )
	{
		i8desk::data_helper::ServerTraits::ValueType &val = curservers_[nSel];
		if( val->SvrType == i8desk::MainServer )
		{
			CMessageBox msgDlg(_T("提示"),_T("主服务不允许删除"));
			msgDlg.DoModal();
			return ;
		}

		i8desk::data_helper::ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

		if( i8desk::GetDataMgr().DelData(servers, val->SvrID) )
		{
			curservers_.erase(curservers_.begin() + nSel);
			_ShowView();
		}

	}
}

void WZDlgServers::OnBnClickedButtonModify()
{
	int nSel = wndlistServers_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return ;

	i8desk::data_helper::ServerTraits::ValueType &val = curservers_[nSel];

	if( !_CheckData(val) )
		return;

	// 处理按区域优先情况
	if( val->BalanceType == i8desk::data_helper::BALANCE_AREAPRIORITY ) 
	{
		if( _ModifyPriorityArea(val->SvrID) == false )
			return;
	}

	i8desk::data_helper::ServerTraits::MapType &servers = i8desk::GetDataMgr().GetServers();

	if( i8desk::GetDataMgr().ModifyData(servers, val->SvrID, val) )
		_ShowView();
}

void WZDlgServers::_ShowView()
{
	size_t cnt = curservers_.size();
	wndlistServers_.SetItemCount(cnt);
}

void WZDlgServers::OnLvnItemchangedListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = wndlistServers_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 ) 
		return;

	i8desk::data_helper::ServerTraits::ValueType &val = curservers_[nSel];

	serverName_	= val->SvrName;

	wndComboSvrIP_.ResetContent();
	// 初始化服务器默认IP
	if( val->Ip1 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip1).c_str());
	if( val->Ip2 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip2).c_str());
	if( val->Ip3 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip3).c_str());
	if( val->Ip4 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip4).c_str());
	
	wndComboSvrIP_.SelectString(-1, i8desk::IP2String(val->SvrIP).c_str());

	if ( val->BalanceType != i8desk::data_helper::BALANCE_AREAPRIORITY )
	{
		wndradioDistribute_.SetCheck(TRUE);
		wndradioAreaPriority_.SetCheck(FALSE);
		wndTreeArea_.EnableWindow(FALSE);
	}
	else
	{
		wndradioDistribute_.SetCheck(FALSE);
		wndradioAreaPriority_.SetCheck(TRUE);
		wndTreeArea_.EnableWindow(TRUE);

		for(HTREEITEM hItem = wndTreeArea_.GetRootItem(); 
			hItem != NULL; hItem = wndTreeArea_.GetNextSiblingItem(hItem)) 
		{
			typedef i8desk::data_helper::AreaTraits::ElementType AreaElement;
			AreaElement *treeval = reinterpret_cast<AreaElement *>(wndTreeArea_.GetItemData(hItem));
			assert(treeval);

			BOOL check = utility::Strcmp(treeval->SvrID, val->SvrID) == 0;
			wndTreeArea_.SetCheck(hItem, check);
		}

	}

	UpdateData(FALSE);
}

void WZDlgServers::OnLvnGetdispinfoListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curservers_.size() )
		return;
	using namespace i8desk::data_helper;
	const ServerTraits::ValueType &val = curservers_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_SERVER_NAME:
			text = val->SvrName ;
			break;
		case MASK_SERVER_DEFAULT_IP:
			text = i8desk::IP2String(val->SvrIP);
			break;
		case MASK_SERVER_AREAMODE:
			val->BalanceType == BALANCE_POLL ? text = RT_DISTRIBUTE :  text = RT_PRIORITY_AREA ;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

void WZDlgServers::OnShow(int showtype)
{
	ShowWindow(showtype);
	_ReadDatas();
}

BOOL WZDlgServers::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndlistServers_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &workOutLine_);

	CRect rcTree;
	wndTreeArea_.GetWindowRect(rcTree);
	i8desk::ui::DrawFrame(this, memDC, rcTree, &workOutLine_);

	return TRUE;

}