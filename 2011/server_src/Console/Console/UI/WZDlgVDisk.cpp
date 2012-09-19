// WZDlgVDisk.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "WZDlgVDisk.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/ServerBusiness.h"
#include "../MessageBox.h"
#include "../Misc.h"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"

#include "../ui/Skin/SkinMgr.h"
#include "../UI/UIHelper.h"
#include <bitset>
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
		MASK_VDISK_SVRNAME = 1, MASK_VDISK_IP, MASK_VDISK_SVRDRV, MASK_VDISK_CLIDRV, MASK_VDISK_REFESHTIME, MASK_VDISK_SIZE, MASK_VDISK_SSDDRV
	};

	template < typename ListCtrlT >
	static void InitListCtrl(ListCtrlT &listCtrl)
	{
		listCtrl.SetItemHeight(24);

		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_VDISK_SVRNAME,		_T("服务器名称"),	LVCFMT_LEFT, 88, MASK_VDISK_SVRNAME,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_IP,			_T("服务器IP"),		LVCFMT_LEFT, 90, MASK_VDISK_IP,		detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_SVRDRV,		_T("服务器盘符"),	LVCFMT_LEFT, 80, MASK_VDISK_SVRDRV,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_CLIDRV,		_T("客户机盘符"),	LVCFMT_LEFT, 80, MASK_VDISK_CLIDRV,	detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_REFESHTIME,	_T("刷盘时机"),	LVCFMT_LEFT, 80, MASK_VDISK_REFESHTIME, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_SIZE,			_T("缓存大小"),	LVCFMT_LEFT, 80, MASK_VDISK_SIZE, detail::GetTrait() );
		listCtrl.InsertColumnTrait(MASK_VDISK_SSDDRV,		_T("SSD盘符"),		LVCFMT_LEFT, 80, MASK_VDISK_SSDDRV, detail::GetTrait() );

	}

	std::bitset<MASK_VDISK_SSDDRV + 1> VDiskCompare;
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
			case MASK_VDISK_SVRNAME:
				return ascSort_ ? utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) > 0 :
					utility::Strcmp(Server::GetServiceBySID(lhs->SvrID)->SvrName, Server::GetServiceBySID(rhs->SvrID)->SvrName) < 0;
			case MASK_VDISK_IP:
				return ascSort_ ? lhs->SoucIP > rhs->SoucIP : lhs->SoucIP < rhs->SoucIP;
			case MASK_VDISK_SVRDRV:
				return ascSort_ ? lhs->SvrDrv > rhs->SvrDrv : lhs->SvrDrv < rhs->SvrDrv;
			case MASK_VDISK_CLIDRV:
				return ascSort_ ? lhs->CliDrv > rhs->CliDrv : lhs->CliDrv < rhs->CliDrv;
			case MASK_VDISK_REFESHTIME:
				return ascSort_ ? lhs->LoadType > rhs->LoadType : lhs->LoadType < rhs->LoadType;
			case MASK_VDISK_SIZE:
				return ascSort_ ? lhs->Size > rhs->Size : lhs->Size < rhs->Size;
			case MASK_VDISK_SSDDRV:
				return ascSort_ ? lhs->SsdDrv > rhs->SsdDrv : lhs->SsdDrv < rhs->SsdDrv;
			default:
				assert(0);
				return false;
			}
		}
	};
}

// WZDlgVDisk 对话框

IMPLEMENT_DYNAMIC(WZDlgVDisk, CDialog)

WZDlgVDisk::WZDlgVDisk(CWnd* pParent /*=NULL*/)
	: CDialog(WZDlgVDisk::IDD, pParent)
{
	workOutLine_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/OutBoxLine.png")));
}

WZDlgVDisk::~WZDlgVDisk()
{
	workOutLine_.Detach();
}

void WZDlgVDisk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_SVRNAME, wndComboSvrName_);
	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_SVRIP, wndComboSvrIP_);

	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_SVRDRV, wndComboServerDriver_);
	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_CLIDRV, wndComboClientDriver_);
	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_LOADTYPE, wndComboRefreshTime_);
	DDX_Control(pDX, IDC_COMBO_WZ_VDISK_SSDDRV, wndComboSsdDriver_);

	DDX_Control(pDX, IDC_EDIT_WZ_VDISK_SIZE, wndEditSize_);
	DDX_Text(pDX, IDC_EDIT_WZ_VDISK_SIZE, textSize_);

	DDX_Control(pDX, IDC_LIST_WZ_VDISK_VDISKS, wndListVDisks_);

	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_SET, wndLabelVDisksSet_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_SVRNAME, wndLabelSvrName_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_IP, wndLabelVDisksIP_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_SVRDRV, wndLabelServerDriver_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_CLIDRV, wndLabelClientDriver_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_LOADTYPE, wndLabelRefreshTime_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_SIZE, wndLabelSize_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_MB, wndLabelMB_);
	DDX_Control(pDX, IDC_STATIC_WZ_VDISK_SSDDRV, wndLabelSsdDriver_);

	
	DDX_Control(pDX, IDC_BUTTON_WZ_VDISK_ADD, wndbtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_WZ_VDISK_DEL, wndbtnDel_);
	DDX_Control(pDX, IDC_BUTTON_WZ_VDISK_MODIFY, wndbtnModify_);
}


BEGIN_MESSAGE_MAP(WZDlgVDisk, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WZ_VDISK_ADD, &WZDlgVDisk::OnBnClickedButtonVdiskAdd)
	ON_BN_CLICKED(IDC_BUTTON_WZ_VDISK_DEL, &WZDlgVDisk::OnBnClickedButtonVdiskDel)
	ON_BN_CLICKED(IDC_BUTTON_WZ_VDISK_MODIFY, &WZDlgVDisk::OnBnClickedButtonVdiskModify)
	
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_WZ_VDISK_VDISKS, &WZDlgVDisk::OnLvnGetdispinfoListVdiskInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WZ_VDISK_VDISKS, &WZDlgVDisk::OnLvnItemchangedListVdiskInfo)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_WZ_VDISK_VDISKS, &WZDlgVDisk::OnLvnColumnclickListVdiskInfo)
	
	ON_CBN_SELCHANGE(IDC_COMBO_WZ_VDISK_SVRNAME, &WZDlgVDisk::OnCbnSelchangeComboSvrName)

	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


// WZDlgVDisk 消息处理程序
namespace 
{
	const TCHAR *RT_COMP_BOOT				= _T("开机刷盘");
	const TCHAR *RT_MENU_RUN				= _T("运行菜单");
	const TCHAR *RT_GAME_RUN				= _T("运行游戏");

	template < typename StringT, typename MsgT, typename DlgT >
	void AddMsg(StringT &msg, const MsgT &info, DlgT &dlg)
	{
		msg += info;
		msg += _T("\r\n");

		dlg->UpdateData(FALSE);
	}
}

BOOL WZDlgVDisk::OnInitDialog()
{

	CDialog::OnInitDialog();

	InitListCtrl(wndListVDisks_);


	for(TCHAR chDrv='C'; chDrv<='Z'; ++chDrv)
	{
		CString text;
		text += chDrv;
		wndComboServerDriver_.AddString(text);
		wndComboClientDriver_.AddString(text);		
	}

	wndComboServerDriver_.SetCurSel(0);
	wndComboClientDriver_.SetCurSel(0);

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

	i8desk::business::VDisk::GetLoadTypes(AddCombo(wndComboRefreshTime_));
	wndComboRefreshTime_.SetCurSel(0);

	i8desk::business::VDisk::GetSsdDrv(AddCombo(wndComboSsdDriver_));
	wndComboSsdDriver_.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void WZDlgVDisk::_ReadDatas()
{
	curvDisks_.clear();
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetServers());

	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using namespace i8desk::data_helper;
	VDiskTraits::MapType &vDisks = i8desk::GetDataMgr().GetVDisks();

	stdex::map2vector(vDisks, curvDisks_);

	if(curvDisks_.empty())
		return;

	TCHAR svrDrv[] = {curvDisks_[0]->SvrDrv, 0};
	wndComboServerDriver_.SelectString(-1, svrDrv);

	TCHAR cliDrv[] = {curvDisks_[0]->CliDrv, 0};
	wndComboClientDriver_.SelectString(-1, cliDrv);

	wndComboRefreshTime_.SelectString(-1, i8desk::business::VDisk::GetLoadTypeDescByType(curvDisks_[0]->LoadType));
	wndComboSsdDriver_.SelectString(-1, i8desk::business::VDisk::GetSsdDescByType(curvDisks_[0]->LoadType));


	stdex::tString size;
	stdex::ToString(curvDisks_[0]->Size, size);
	textSize_ = size.c_str();

	_ShowView();

	UpdateData(FALSE);
}

bool WZDlgVDisk::_CheckDatas(i8desk::data_helper::VDiskTraits::ValueType &val)
{
	UpdateData(TRUE);

	using namespace i8desk::data_helper;

	int SvrDrv = 0;
	int CliDrv = 0;

	val->Type = I8VDisk;

	CString IP;
	wndComboSvrIP_.GetWindowText(IP);

	SvrDrv = wndComboServerDriver_.GetCurSel() + 'C';
	val->Port = i8desk::business::VDisk::VDiskPort + (SvrDrv - 'A');
	val->LoadType = wndComboRefreshTime_.GetItemData(wndComboRefreshTime_.GetCurSel());
	utility::Strcpy(val->SvrMode, MAIN_SERVER_SID);

	CliDrv = wndComboClientDriver_.GetCurSel() + 'C';
	val->SsdDrv = wndComboSsdDriver_.GetItemData(wndComboSsdDriver_.GetCurSel());


	if( utility::IsEmpty((LPCTSTR)IP) || !i8desk::IsValidIP((LPCTSTR)IP) )
	{
		CMessageBox msgDlg(_T("提示"),_T("IP地址不正确!"));
		msgDlg.DoModal();
		return false;
	}
	val->SoucIP = ::htonl(i8desk::String2IP((LPCTSTR)IP));

	stdex::tString svrID = i8desk::business::Server::GetServerIDBySvrIP(val->SoucIP);
	if( svrID.empty() )
	{
		CMessageBox msgDlg(_T("提示"),_T("没有该IP的服务器!"));
		msgDlg.DoModal();
		return false;
	}
	else
		utility::Strcpy(val->SvrID, svrID);

	struct ErrorMsg
	{
		void operator()(LPCTSTR tip, LPCTSTR info)
		{
			CMessageBox box(tip, info);
			box.DoModal();
		}
	};

	if( !i8desk::business::VDisk::IsValid(val->VID, val->SoucIP, SvrDrv, CliDrv, ErrorMsg()) )
	{
		return false;
	}

	val->SvrDrv = SvrDrv;
	val->CliDrv = CliDrv;

	stdex::tString size = textSize_;
	stdex::ToNumber(val->Size, size);

	return true;
}

void WZDlgVDisk::OnBnClickedButtonVdiskAdd()
{
	using namespace i8desk::data_helper;
	VDiskTraits::ValueType val(new VDiskTraits::ElementType);
	utility::Strcpy(val->VID, utility::CreateGuidString());
	IsAdd_ = true;
	if( !_CheckDatas(val) )
		return;

	VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

	if( i8desk::GetDataMgr().AddData(VDisks, val->VID, val) )
	{
		curvDisks_.push_back(val);	
		_ShowView();
	}
}

void WZDlgVDisk::OnBnClickedButtonVdiskDel()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	CMessageBox msgDlg(_T("提示"),_T("确定要删除选定的项吗?"));

	if( nSel != -1 && msgDlg.DoModal() == IDOK )
	{
		i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

		i8desk::data_helper::VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

		if( i8desk::GetDataMgr().DelData(VDisks, val->VID) )
		{
			curvDisks_.erase(curvDisks_.begin() + nSel);
			_ShowView();
		}

	}
}

void WZDlgVDisk::OnBnClickedButtonVdiskModify()
{
	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 )
		return ;
	IsAdd_ = false;
	i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

	if( !_CheckDatas(val) )
		return;

	i8desk::data_helper::VDiskTraits::MapType &VDisks = i8desk::GetDataMgr().GetVDisks();

	if( i8desk::GetDataMgr().ModifyData(VDisks, val->VID, val) )
		_ShowView();
}

void WZDlgVDisk::_ShowView()
{
	size_t cnt = curvDisks_.size();
	wndListVDisks_.SetItemCount(cnt);
}


void WZDlgVDisk::OnLvnItemchangedListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = wndListVDisks_.GetNextItem(-1, LVIS_SELECTED);
	if( nSel == -1 ) 
		return ;

	i8desk::data_helper::VDiskTraits::ValueType &val = curvDisks_[nSel];

	wndComboSvrName_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_SVRNAME));
	OnCbnSelchangeComboSvrName();
	wndComboServerDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_SVRDRV));
	wndComboRefreshTime_.SelectString(-1, wndListVDisks_.GetItemText(nSel, MASK_VDISK_REFESHTIME));
	wndComboClientDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel ,MASK_VDISK_CLIDRV));	
	wndComboSsdDriver_.SelectString(-1, wndListVDisks_.GetItemText(nSel ,MASK_VDISK_SSDDRV));	

	stdex::tString size;
	stdex::ToString(val->Size, size);
	textSize_ = size.c_str();

	UpdateData(FALSE);

}

void WZDlgVDisk::OnLvnGetdispinfoListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curvDisks_.size() )
		return;
	using namespace i8desk::data_helper;
	const VDiskTraits::ValueType &val = curvDisks_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_VDISK_SVRNAME:
			text = i8desk::business::Server::GetServiceBySID(val->SvrID)->SvrName;
			break;
		case MASK_VDISK_IP:
			text = i8desk::IP2String(val->SoucIP);
			break;
		case MASK_VDISK_SVRDRV:
			text = val->SvrDrv;
			break;
		case MASK_VDISK_CLIDRV:
			text = val->CliDrv;
			break;
		case MASK_VDISK_REFESHTIME:
			text = i8desk::business::VDisk::GetLoadTypeDescByType(val->LoadType);
			break;
		case MASK_VDISK_SSDDRV:
			text = i8desk::business::VDisk::GetSsdDescByType(val->SsdDrv);
			break;
		case MASK_VDISK_SIZE:
			stdex::ToString(val->Size, text);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

}

void WZDlgVDisk::OnLvnColumnclickListVdiskInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListVDisks_.GetItemCount() == 0 )
		return;


	CWaitCursor wc;
	std::stable_sort(curvDisks_.begin(), curvDisks_.end(), Compare(pNMLV->iSubItem, VDiskCompare[pNMLV->iSubItem]));
	VDiskCompare.set(pNMLV->iSubItem, !VDiskCompare[pNMLV->iSubItem]);
	wndListVDisks_.RedrawCurPageItems();

	wndListVDisks_.SetSortArrow(pNMLV->iSubItem, VDiskCompare[pNMLV->iSubItem]);
}

void WZDlgVDisk::OnShow(int showtype)
{
	ShowWindow(showtype);
	_ReadDatas();
}

BOOL WZDlgVDisk::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);

	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	HBRUSH brush = AtlGetBackgroundBrush(GetSafeHwnd(), GetParent()->GetSafeHwnd());
	memDC.FillRect(rcClient, brush);

	CRect rcList;
	wndListVDisks_.GetWindowRect(rcList);
	i8desk::ui::DrawFrame(this, memDC, rcList, &workOutLine_);

	return TRUE;

}


void WZDlgVDisk::OnCbnSelchangeComboSvrName()
{
	int index = wndComboSvrName_.GetCurSel();
	if( index == -1 )
		return;

	stdex::tString sid = (LPCTSTR)wndComboSvrName_.GetItemData(index);
	const i8desk::data_helper::ServerTraits::ValueType &val = i8desk::business::Server::GetServiceBySID(sid);

	wndComboSvrIP_.ResetContent();
	if( val->Ip1 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip1).c_str());
	if( val->Ip2 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip2).c_str());
	if( val->Ip3 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip3).c_str());
	if( val->Ip4 != 0 )
		wndComboSvrIP_.AddString(i8desk::IP2String(val->Ip4).c_str());

	wndComboSvrIP_.SetCurSel(0);
	
}
