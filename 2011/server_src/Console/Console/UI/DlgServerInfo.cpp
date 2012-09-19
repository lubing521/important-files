// DlgServerInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgServerInfo.h"

#include "../Misc.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/I8Type.hpp"

#include "../Business/ServerBusiness.h"

#include "../UI/UIHelper.h"
#include "../MessageBox.h"
#include "../Misc.h"
#include "Skin/SkinMgr.h"
#include "WindowManager.h"

#include <bitset>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgServerInfo 对话框

IMPLEMENT_DYNAMIC(CDlgServerInfo, CNonFrameChildDlg)

CDlgServerInfo::CDlgServerInfo( CWnd* pParent)
: CNonFrameChildDlg(CDlgServerInfo::IDD, pParent)
, parent_(pParent)
{
	outboxLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));
}

CDlgServerInfo::~CDlgServerInfo()
{
	outboxLine_.Detach();
}

void CDlgServerInfo::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SERVER_INFO_AREA, wndTreeArea_);

	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_SERVERNAME, wndComboSvrName_);

	DDX_Control(pDX, IDC_RADIO_SERVER_INFO_POLL, wndRadioPoll_);
	DDX_Control(pDX, IDC_RADIO_SERVER_INFO_PRIORITY_AREA, wndRadioAreaPriority_);

	DDX_Control(pDX, IDC_BUTTON_SERVER_INFO_MODIFY, btnModify_);
	DDX_Control(pDX, IDC_BUTTON_SERVER_INFO_DEL, btnDel_);

	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_DEFAULT_IP, wndTipDefaultIP_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_SERVERNAME, wndServerName_);
	DDX_Control(pDX, IDC_STATIC_SERVER_INFO_SERVERMGR, wndServerMgr_);

	DDX_Control(pDX, IDC_COMBO_SERVER_INFO_DEFAULT_IP, wndComboDefaultIP_);
	DDX_Control(pDX, IDC_LIST_SERVER_INFO_SERVERS, wndListServers_);

}


BEGIN_MESSAGE_MAP(CDlgServerInfo, CNonFrameChildDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SERVER_INFO_MODIFY, &CDlgServerInfo::OnBnClickedButtonServerInfoModify)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_INFO_DEL, &CDlgServerInfo::OnBnClickedButtonServerInfoDel)
	ON_BN_CLICKED(IDC_RADIO_SERVER_INFO_POLL, &CDlgServerInfo::OnBnClickedRadioServerInfoPoll)
	ON_BN_CLICKED(IDC_RADIO_SERVER_INFO_PRIORITY_AREA, &CDlgServerInfo::OnBnClickedRadioServerInfoPriorityArea)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SERVER_INFO_SERVERS, &CDlgServerInfo::OnLvnItemchangedListServerInfo)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SERVER_INFO_SERVERS, &CDlgServerInfo::OnLvnColumnclickListServerInfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SERVER_INFO_SERVERS, &CDlgServerInfo::OnLvnGetdispinfoListServerInfo)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER_INFO_SERVERNAME, &CDlgServerInfo::OnCbnSelchangeComboSvrName)

END_MESSAGE_MAP()


namespace
{
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
		MASK_SERVER_SVRNAME = 1, MASK_SERVER_SVRTYPE, MASK_SERVER_IP, MASK_SERVER_BANLANCE, MASK_SERVER_AREA
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_SERVER_SVRNAME,		_T("服务器名称"),	LVCFMT_LEFT, 100, MASK_SERVER_SVRNAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_SERVER_SVRTYPE,		_T("服务器类型"),	LVCFMT_LEFT, 75, MASK_SERVER_SVRTYPE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_SERVER_IP,			_T("服务器IP"),		LVCFMT_LEFT, 100, MASK_SERVER_IP,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_SERVER_BANLANCE,	_T("运行方式"),	LVCFMT_LEFT, 70, MASK_SERVER_BANLANCE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_SERVER_AREA,		_T("优先区域"),	LVCFMT_LEFT, 100, MASK_SERVER_AREA,	detail::GetTrait());
	}

	struct AddSvrCombo
	{
		CComboBox &combo_;
		AddSvrCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(LPCTSTR name, LPCTSTR sid)
		{
			int index = combo_.AddString(name);
			combo_.SetItemData(index, reinterpret_cast<DWORD_PTR>(sid));
		}
	};

	struct AddCombo
	{
		CComboBox &combo_;
		AddCombo(CComboBox &combo)
			: combo_(combo)
		{}

		void operator()(int type, LPCTSTR desc)
		{
			int index = combo_.AddString(desc);
			combo_.SetItemData(index, type);
		}
	};

	std::bitset<MASK_SERVER_AREA + 1> ServerCompare;
	using namespace i8desk::business;

	struct Compare
	{
		int col_;
		bool ascSort_;

		Compare(int col, bool ascSort)
			: col_(col)
			, ascSort_(ascSort)
		{}

		template <typename T>
		bool operator()(const T &lhs, const T &rhs) const
		{
			switch(col_)
			{
			case MASK_SERVER_SVRNAME:
				return ascSort_ ? utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) > 0 :
					utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) < 0;
			case MASK_SERVER_SVRTYPE:
				return ascSort_ ? lhs->SvrType > rhs->SvrType : lhs->SvrType < rhs->SvrType;
			case MASK_SERVER_IP:
				return ascSort_ ? lhs->SvrIP > rhs->SvrIP : lhs->SvrIP < rhs->SvrIP;
			case MASK_SERVER_BANLANCE:
				return ascSort_ ? lhs->BalanceType > rhs->BalanceType : lhs->BalanceType < rhs->BalanceType;
			case MASK_SERVER_AREA:
				{
					stdex::tString lhstext, rhstext;
					Server::GetSvrAreaPriotyDescBySID(lhs->SvrID, lhstext);
					Server::GetSvrAreaPriotyDescBySID(rhs->SvrID, rhstext);
					return ascSort_ ? lhstext > rhstext : lhstext < rhstext;
				}
			default:
				assert(0);
				return false;
			}
		}
	};

}


bool CDlgServerInfo::_CheckData(i8desk::data_helper::ServerTraits::ValueType &val)
{
	UpdateData(TRUE);

	CString defaultIP;
	wndComboDefaultIP_.GetWindowText(defaultIP);
	val->SvrIP = ::htonl(i8desk::String2IP((LPCTSTR)defaultIP));

	if( wndRadioPoll_.GetCheck() == BST_CHECKED )
		val->BalanceType = i8desk::data_helper::BALANCE_POLL;
	else if( wndRadioAreaPriority_.GetCheck() == BST_CHECKED )
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


// CDlgServerInfo 消息处理程序
BOOL CDlgServerInfo::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	SetTitle(_T("服务器管理"));

	i8desk::data_helper::AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);

	InitTree(wndTreeArea_, areas);

	InitListCtrl(wndListServers_);

	_ReadServer();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgServerInfo::_ReadServer()
{

	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	ServerTraits::MapType &vDisks = i8desk::GetDataMgr().GetServers();

	std::for_each(vDisks.begin(), vDisks.end(), [this](const ServerTraits::MapType::value_type &val)
	{
		curServers_.push_back(val.second);
	});

	if(curServers_.empty())
		return;

	i8desk::business::Server::GetAllService(AddSvrCombo(wndComboSvrName_));
	stdex::tString svrSID;
	for(int i = 0; i != wndComboSvrName_.GetCount(); ++i)
	{
		svrSID = reinterpret_cast<LPCTSTR>(wndComboSvrName_.GetItemData(i));
		if( i8desk::business::Server::IsMainServer(i8desk::business::Server::GetServiceBySID(svrSID)) )
		{
			wndComboSvrName_.SetCurSel(i);
			break;
		}
	}
	OnCbnSelchangeComboSvrName();

	_ShowView();

	UpdateData(FALSE);
}


BOOL CDlgServerInfo::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndTreeArea_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	CRect rc;
	wndListServers_.GetWindowRect(rc);
	i8desk::ui::DrawFrame(this, *pDC, rc, &outboxLine_);

	return TRUE;
}



bool CDlgServerInfo::_ModifyPriorityArea(LPCTSTR svrID)
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


void CDlgServerInfo::OnBnClickedButtonServerInfoModify()
{
	int nSel = wndListServers_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
	{
		CMessageBox msgdlg(_T("提示"), _T("修改设置请在列表中选中要修改的选项"));
		msgdlg.DoModal();
		return ;
	}
	i8desk::data_helper::ServerTraits::ValueType &val = curServers_[nSel];

	// 获取数据
	if( !_CheckData(val) )
		return;

	// 处理按区域优先情况
	if( val->BalanceType == i8desk::data_helper::BALANCE_AREAPRIORITY ) 
	{
		if( _ModifyPriorityArea(val->SvrID) == false )
			return;
	}

	if( i8desk::GetDataMgr().ModifyData(i8desk::GetDataMgr().GetServers(), val->SvrID, val) )
	{
		_ShowView();
	}
}

void CDlgServerInfo::OnBnClickedButtonServerInfoDel()
{
	int nSel = wndListServers_.GetNextItem(-1, LVIS_SELECTED);
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的项吗?"));

	if( nSel != -1 && msgDlg.DoModal() == IDOK )
	{
		i8desk::data_helper::ServerTraits::ValueType val = curServers_[nSel];

		if(val->SvrType == i8desk::MainServer)
		{
			CMessageBox msgDlg(_T("提示"),_T("不能删除主服务"));
			msgDlg.DoModal();
			return;
		}

		i8desk::data_helper::ServerTraits::MapType &Servers = i8desk::GetDataMgr().GetServers();

		if( i8desk::GetDataMgr().DelData(Servers, val->SvrID) )
		{
			curServers_.erase(curServers_.begin() + nSel);
			_ShowView();
		}
	}
}


void CDlgServerInfo::OnBnClickedRadioServerInfoPoll()
{
	wndTreeArea_.EnableWindow(FALSE);
}

void CDlgServerInfo::OnBnClickedRadioServerInfoPriorityArea()
{
	wndTreeArea_.EnableWindow(TRUE);
}

void CDlgServerInfo::OnLvnItemchangedListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = wndListServers_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 ) 
		return ;

	i8desk::data_helper::ServerTraits::ValueType &val = curServers_[nSel];

	wndComboSvrName_.SelectString(-1, wndListServers_.GetItemText(nSel, MASK_SERVER_SVRNAME));
	OnCbnSelchangeComboSvrName();

	UpdateData(FALSE);
}

void CDlgServerInfo::_ShowView()
{
	size_t cnt = curServers_.size();
	wndListServers_.SetItemCount(cnt);
}

void CDlgServerInfo::OnLvnColumnclickListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListServers_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curServers_.begin(), curServers_.end(), Compare(pNMLV->iSubItem, ServerCompare[pNMLV->iSubItem]));
	ServerCompare.set(pNMLV->iSubItem, !ServerCompare[pNMLV->iSubItem]);
	wndListServers_.RedrawCurPageItems();

	wndListServers_.SetSortArrow(pNMLV->iSubItem, ServerCompare[pNMLV->iSubItem]);
}


void CDlgServerInfo::OnLvnGetdispinfoListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curServers_.size() )
		return;
	using namespace i8desk::data_helper;
	const ServerTraits::ValueType &val = curServers_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_SERVER_SVRNAME:
			text = val->SvrName;
			break;
		case MASK_SERVER_SVRTYPE:
			text = i8desk::business::Server::GetSvrTypeDescByType(val->SvrType);
			break;
		case MASK_SERVER_IP:
			text = i8desk::IP2String(val->SvrIP);
			break;
		case MASK_SERVER_BANLANCE:
			text = i8desk::business::Server::GetBalanceTypeDescByType(val->BalanceType);
			break;
		case MASK_SERVER_AREA:
			if(val->BalanceType == BALANCE_AREAPRIORITY)
				i8desk::business::Server::GetSvrAreaPriotyDescBySID(val->SvrID, text);
			else
				text = _T(" ");
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

void CDlgServerInfo::OnCbnSelchangeComboSvrName()
{
	int index = wndComboSvrName_.GetCurSel();
	if( index == -1 )
		return;

	stdex::tString sid = (LPCTSTR)wndComboSvrName_.GetItemData(index);
	const i8desk::data_helper::ServerTraits::ValueType &val = i8desk::business::Server::GetServiceBySID(sid);

	wndComboDefaultIP_.ResetContent();
	if( val->Ip1 != 0 )
		wndComboDefaultIP_.AddString(i8desk::IP2String(val->Ip1).c_str());
	if( val->Ip2 != 0 )
		wndComboDefaultIP_.AddString(i8desk::IP2String(val->Ip2).c_str());
	if( val->Ip3 != 0 )
		wndComboDefaultIP_.AddString(i8desk::IP2String(val->Ip3).c_str());
	if( val->Ip4 != 0 )
		wndComboDefaultIP_.AddString(i8desk::IP2String(val->Ip4).c_str());

	wndComboDefaultIP_.SelectString(-1, i8desk::IP2String(val->SvrIP).c_str());

	switch(val->BalanceType)
	{
	case i8desk::data_helper::BALANCE_UNKNOWN:
		wndTreeArea_.EnableWindow(FALSE);
		wndRadioPoll_.SetCheck(TRUE);
		wndRadioAreaPriority_.SetCheck(FALSE);
		break;
	case i8desk::data_helper::BALANCE_POLL:
		wndTreeArea_.EnableWindow(FALSE);
		wndRadioPoll_.SetCheck(TRUE);
		wndRadioAreaPriority_.SetCheck(FALSE);
		break;
	case i8desk::data_helper::BALANCE_AREAPRIORITY:
		wndTreeArea_.EnableWindow(TRUE);
		wndRadioPoll_.SetCheck(FALSE);
		wndRadioAreaPriority_.SetCheck(TRUE);

		for(HTREEITEM hItem = wndTreeArea_.GetRootItem(); 
			hItem != NULL; hItem = wndTreeArea_.GetNextSiblingItem(hItem)) 
		{
			typedef i8desk::data_helper::AreaTraits::ElementType AreaElement;
			AreaElement *areaval = reinterpret_cast<AreaElement *>(wndTreeArea_.GetItemData(hItem));
			assert(areaval);

			BOOL check = utility::Strcmp(areaval->SvrID, val->SvrID) == 0;
			wndTreeArea_.SetCheck(hItem, check);
		}

		break;
	}

}