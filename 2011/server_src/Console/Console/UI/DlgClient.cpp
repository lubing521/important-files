// DlgClient.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "../resource.h"
#include "DlgClient.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"

#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/I8Type.hpp"

#include "../Misc.h"
#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridColumnTraitText.h"
#include "../Business/ClientBusiness.h"
#include "../Business/AreaBusiness.h"
#include "../Business/GameBusiness.h"
#include "../Business/MonitorBusiness.h"


#include "../ui/Skin/SkinMgr.h"
#include "../Data/DataHelper.h"
#include "../Data/CustomDataType.h"

#include "DlgClientArea.h"
#include "DlgClientAdd.h"
#include "DlgClientModify.h"

#include "DlgChkDisk.h"
#include "DlgProtArea.h"
#include "DlgClientClientViewGame.h"
#include "DlgClientSystemInfo.h"
#include "DlgClientRunExe.h"

#include "../MessageBox.h"

#include "UIHelper.h"

#include <bitset>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 客户端视图显示, 默认显示基本视图
UINT ClientViewShow = ID_CLIENT_CHANGE_BASIC;


// CDlgClient

IMPLEMENT_DYNCREATE(CDlgClient, CDialog)

CDlgClient::CDlgClient(CWnd* pParent)
: i8desk::ui::BaseWnd(CDlgClient::IDD, pParent)
, viewStatus_(Basic)
{

}

CDlgClient::~CDlgClient()
{
}

void CDlgClient::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CLIENT, wndTree_);
	DDX_Control(pDX, IDC_LIST_CLIENT_BASIC, wndListBasic_);
	DDX_Control(pDX, IDC_LIST_CLIENT_SYSTEM, wndListSystem_);
	DDX_Control(pDX, IDC_LIST_CLIENT_VERSION, wndListVersion_);

	DDX_Control(pDX, IDC_COMBO_CLIENT_SELECT_VIEW, wndComboChangeView_);

	DDX_Control(pDX, IDC_BUTTON_CLIENT_MGR, wndBtnClientMgr_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_AREA_ADD, wndBtnAreaAdd_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_AREA_DELETE, wndBtnAreaDel_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_AREA_MODIFY, wndBtnAreaMod_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_ADD, wndBtnAdd_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_MODIFY, wndBtnMod_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_DELETE, wndBtnDel_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_REMOTE, wndBtnRemote_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_AREAPROTECT, wndBtnProtect_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_INSTALLRESTORE, wndBtnRestore_);
	DDX_Control(pDX, IDC_BUTTON_CLIENT_SAVE_HARDWARE, wndBtnSaveHardware_);
}


BEGIN_EASYSIZE_MAP(CDlgClient)   
	EASYSIZE(IDC_TREE_CLIENT, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_BORDER, 0)
	EASYSIZE(IDC_LIST_CLIENT_BASIC, IDC_TREE_CLIENT, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_ADD, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_DELETE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_MODIFY, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_AREAPROTECT, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_REMOTE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_INSTALLRESTORE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_CLIENT_SAVE_HARDWARE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_COMBO_CLIENT_SELECT_VIEW, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgClient, i8desk::ui::BaseWnd)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_ADD, &CDlgClient::OnBnClickedButtonClientAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_MODIFY, &CDlgClient::OnBnClickedButtonClientModify)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_DELETE, &CDlgClient::OnBnClickedButtonClientDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_SELECT_VIEW, &CDlgClient::OnBnClickedButtonClientSelectView)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_REMOTE, &CDlgClient::OnBnClickedButtonClientControl)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_AREAPROTECT, &CDlgClient::OnBnClickedButtonClientAreaProtect)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_INSTALLRESTORE, &CDlgClient::OnBnClickedButtonClientInstallRestore)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_SAVE_HARDWARE, &CDlgClient::OnBnClickedButtonClientSaveHardware)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CLIENT, &CDlgClient::OnTvnSelchangedTreeClient)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_BASIC, &CDlgClient::OnLvnGetdispinfoListClientBasic)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_SYSTEM, &CDlgClient::OnLvnGetdispinfoListClientSystem)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLIENT_VERSION, &CDlgClient::OnLvnGetdispinfoListClientVersion)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_VERSION, &CDlgClient::OnNMRClickListClientVersion)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_SYSTEM, &CDlgClient::OnNMRClickListClientSystem)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CLIENT_BASIC, &CDlgClient::OnNMRClickListClientBasic)

	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CLIENT_VERSION, &CDlgClient::OnLvnColumnclickListClientVersion)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CLIENT_SYSTEM, &CDlgClient::OnLvnColumnclickListClientSystem)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CLIENT_BASIC, &CDlgClient::OnLvnColumnclickListClientBasic)

    ON_NOTIFY(NM_CLICK, IDC_LIST_CLIENT_VERSION, &CDlgClient::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CLIENT_VERSION, &CDlgClient::OnLvnKeydownList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_CLIENT_SYSTEM, &CDlgClient::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CLIENT_SYSTEM, &CDlgClient::OnLvnKeydownList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_CLIENT_BASIC, &CDlgClient::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CLIENT_BASIC, &CDlgClient::OnLvnKeydownList)

	ON_BN_CLICKED(IDC_BUTTON_CLIENT_AREA_ADD, &CDlgClient::OnBnClickedButtonClientAreaAdd)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_AREA_DELETE, &CDlgClient::OnBnClickedButtonClientAreaDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_AREA_MODIFY, &CDlgClient::OnBnClickedButtonClientAreaModify)

	ON_CBN_SELCHANGE(IDC_COMBO_CLIENT_SELECT_VIEW, &CDlgClient::OnComboChangeView)
	ON_COMMAND_RANGE(ID_CONTROL_START, ID_CONTROL_REFRESH, &CDlgClient::OnControlOperate)
    ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgClient::OnApplySelect)
    ON_MESSAGE(i8desk::ui::WM_REPORT_SELECT_STATE, &CDlgClient::OnReportSelectState)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()



namespace 
{
	CCustomColumnTraitIcon *iconCol[3] = {0};

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
		CCustomColumnTraitIcon *GetIconTrait(ListT &list, size_t index)
		{
			assert(index < _countof(iconCol));
			iconCol[index] = new CCustomColumnTraitIcon(list);
			iconCol[index]->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			iconCol[index]->AddIcon(i8desk::ui::SkinMgrInstance().GetSkin(_T("Client/Ico_Offline.png")));
			iconCol[index]->AddIcon(i8desk::ui::SkinMgrInstance().GetSkin(_T("Client/Ico_Online.png")));
			
			return iconCol[index];
		}
	}

	template < typename TreeCtrlT, typename AreaT >
	void InitTreeCtrl(TreeCtrlT &treeCtrl, const AreaT &areas)
	{
		treeCtrl.DeleteAllItems();

		using i8desk::data_helper::AreaTraits;
		AreaTraits::VectorType vecAreas;
		for(typename AreaT::const_iterator iter = areas.begin(); iter != areas.end(); ++iter)
		{
			if( i8desk::business::Area::IsDefaultArea(iter->second) )
				vecAreas.insert(vecAreas.begin(), iter->second);
			else
				vecAreas.push_back(iter->second);
		}

		HTREEITEM hRootItem = treeCtrl.InsertItem(_T("客户机分区"), 0, 0);
		HTREEITEM hFirstItem = 0;
		for(AreaTraits::VecConstIterator iter = vecAreas.begin(); iter != vecAreas.end(); ++iter)
		{
			HTREEITEM hTreeItem = treeCtrl.InsertItem((*iter)->Name, -1, 1, hRootItem);
			treeCtrl.SetItemData(hTreeItem, reinterpret_cast<DWORD_PTR>(areas.find((*iter)->AID)->second.get()));

			if( hFirstItem == 0 )
				hFirstItem = hTreeItem;
		}
		treeCtrl.Expand(hRootItem, TVE_EXPAND);
		treeCtrl.SetItemHeight(25);

		treeCtrl.SelectItem(hFirstItem);
	}

	enum { MASK_NAME = 1, MASK_PROTINSTALL, MASK_PARTITION, MASK_IP, MASK_MARK, MASK_GATE, MASK_DNS, MASK_MAC };

	template < typename ListCtrlT >
	void InitListBasic(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();
 
		listCtrl.InsertColumnTrait(MASK_NAME,		_T("机器名"),		LVCFMT_LEFT, 100, MASK_NAME,	detail::GetIconTrait(listCtrl, 0));
		listCtrl.InsertColumnTrait(MASK_PROTINSTALL,_T("还原状态"),	LVCFMT_LEFT, 70, MASK_PROTINSTALL, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_PARTITION,	_T("所有分区/保护分区"), LVCFMT_LEFT, 120, MASK_PROTINSTALL, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_IP,			_T("IP地址"),	LVCFMT_LEFT, 100, MASK_IP,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_MARK,		_T("子网掩码"),	LVCFMT_LEFT, 100, MASK_MARK,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_GATE,		_T("网关"),		LVCFMT_LEFT, 100, MASK_GATE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DNS,		_T("DNS"),		LVCFMT_LEFT, 100, MASK_DNS,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_MAC,		_T("MAC"),		LVCFMT_LEFT, 148, MASK_MAC,		detail::GetTrait());
	
		listCtrl.SetItemHeight(24);
	}

	enum { MASK_SYSTEM = MASK_NAME + 1, MASK_CPU, MASK_TEMPRATURE, MASK_MAINBOARD, MASK_MEMORY, MASK_DISK, MASK_VIDEO, MASK_AUDIO, MASK_NETWORK, MASK_CAMERA };
	
	// 根据掩码得到列
	std::map<UINT, CCustomColumnTrait *> HardwareTrait;
	std::map<UINT, UINT> Col2Mask;

	template < typename ListCtrlT >
	void InitListHardware(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_NAME,		_T("机器名"),		LVCFMT_LEFT, 100, MASK_NAME,	detail::GetIconTrait(listCtrl, 1));
		listCtrl.InsertColumnTrait(MASK_SYSTEM,		_T("系统信息"),	LVCFMT_LEFT, 70, MASK_SYSTEM,	HardwareTrait[MASK_SYSTEM]	= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CPU,		_T("CPU信息"),		LVCFMT_LEFT, 100, MASK_CPU,		HardwareTrait[MASK_CPU]		= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TEMPRATURE,	_T("CPU温度"),		LVCFMT_LEFT, 70,  MASK_TEMPRATURE,	HardwareTrait[MASK_TEMPRATURE] = detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_MAINBOARD,	_T("主板信息"),	LVCFMT_LEFT, 100, MASK_MAINBOARD, HardwareTrait[MASK_MAINBOARD] = detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_MEMORY,		_T("内存信息"),	LVCFMT_LEFT, 70, MASK_MEMORY,	HardwareTrait[MASK_MEMORY]	= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DISK,		_T("磁盘信息"),	LVCFMT_LEFT, 100, MASK_DISK,	HardwareTrait[MASK_DISK]	= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VIDEO,		_T("显卡信息"),	LVCFMT_LEFT, 100, MASK_VIDEO,	HardwareTrait[MASK_VIDEO]	= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_AUDIO,		_T("声卡信息"),	LVCFMT_LEFT, 100, MASK_AUDIO,	HardwareTrait[MASK_AUDIO]	= detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_NETWORK,	_T("网卡信息"),	LVCFMT_LEFT, 100, MASK_NETWORK, HardwareTrait[MASK_NETWORK] = detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CAMERA,		_T("摄像头信息"), LVCFMT_LEFT, 100, MASK_CAMERA,	HardwareTrait[MASK_CAMERA]	= detail::GetTrait());

		Col2Mask[MASK_SYSTEM]	= MASK_TCLIENT_SYSTEM;
		Col2Mask[MASK_CPU]		= MASK_TCLIENT_CPU;
		Col2Mask[MASK_TEMPRATURE]= MASK_TCLIENT_TEMPERATURE;
		Col2Mask[MASK_MAINBOARD]= MASK_TCLIENT_MAINBOARD;
		Col2Mask[MASK_MEMORY]	= MASK_TCLIENT_MEMORY;
		Col2Mask[MASK_DISK]		= MASK_TCLIENT_DISK;
		Col2Mask[MASK_VIDEO]	= MASK_TCLIENT_VIDEO;
		Col2Mask[MASK_AUDIO]	= MASK_TCLIENT_AUDIO;
		Col2Mask[MASK_NETWORK]	= MASK_TCLIENT_NETWORK;
		Col2Mask[MASK_CAMERA]	= MASK_TCLIENT_CAMERA;
		Col2Mask[MASK_TEMPRATURE]	= MASK_TCLIENT_TEMPERATURE;
	
		listCtrl.SetItemHeight(24);
	}

	enum { MASK_PROTVER = MASK_NAME + 1, MASK_VDISKVER, MASK_MENUVER, MASK_CLIENTSVRVER };
	template < typename ListCtrlT >
	void InitListVersion(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_NAME,		_T("机器名"),		LVCFMT_LEFT, 198, MASK_NAME,		detail::GetIconTrait(listCtrl, 2));
		listCtrl.InsertColumnTrait(MASK_PROTVER,	_T("还原版本"),	LVCFMT_LEFT, 100, MASK_PROTVER,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_VDISKVER,	_T("虚拟盘版本"), LVCFMT_LEFT, 180, MASK_VDISKVER,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_MENUVER,	_T("菜单版本"),	LVCFMT_LEFT, 180, MASK_MENUVER,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CLIENTSVRVER, _T("服务版本"), LVCFMT_LEFT, 180, MASK_CLIENTSVRVER, detail::GetTrait());
	
		listCtrl.SetItemHeight(24);
	}


	// 三层排序
	std::bitset<MASK_MAC + 1>			BasicCompare;
	std::bitset<MASK_CAMERA + 1>		HardwareCompare;
	std::bitset<MASK_CLIENTSVRVER + 1>	VersionCompare;

	template <size_t Status>
	struct Compare
	{
		Compare(int, bool)
		{}
		bool operator()()
		{ return true; }
	};

	template<>
	struct Compare<CDlgClient::Basic>
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
			case MASK_NAME:
				return ascSort_ ? lhs->Online > rhs->Online : lhs->Online < rhs->Online;
			case MASK_PROTINSTALL:
				return ascSort_ ? lhs->ProtInstall > rhs->ProtInstall : lhs->ProtInstall < rhs->ProtInstall;
			case MASK_PARTITION:
				return ascSort_ ? utility::Strcmp(lhs->Partition, rhs->Partition) > 0 : utility::Strcmp(lhs->Partition, rhs->Partition) < 0;
			case MASK_IP:
				return ascSort_ ? lhs->IP > rhs->IP : lhs->IP < rhs->IP;
			case MASK_MARK:
				return ascSort_ ? lhs->Mark > rhs->Mark : lhs->Mark < rhs->Mark;
			case MASK_GATE:
				return ascSort_ ? lhs->Gate > rhs->Gate : lhs->Gate < rhs->Gate;
			case MASK_DNS:
				return ascSort_ ? lhs->DNS > rhs->DNS : lhs->DNS < rhs->DNS;
			case MASK_MAC:
				return ascSort_ ? utility::Strcmp(lhs->MAC, rhs->MAC) > 0 : utility::Strcmp(lhs->MAC, rhs->MAC) < 0;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

	template<>
	struct Compare<CDlgClient::System>
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
			case MASK_NAME:
				return ascSort_ ? lhs->Online > rhs->Online : lhs->Online < rhs->Online;
			case MASK_SYSTEM:
				return ascSort_ ? utility::Strcmp(lhs->System, rhs->System) > 0 : utility::Strcmp(lhs->System, rhs->System) < 0;
			case MASK_CPU:
				return ascSort_ ? utility::Strcmp(lhs->CPU, rhs->CPU) > 0 : utility::Strcmp(lhs->CPU, rhs->CPU) < 0;
			case MASK_TEMPRATURE:
				return ascSort_ ? lhs->Temperature > rhs->Temperature : lhs->Temperature < rhs->Temperature;
			case MASK_MAINBOARD:
				return ascSort_ ? utility::Strcmp(lhs->Mainboard, rhs->Mainboard) > 0 : utility::Strcmp(lhs->Mainboard, rhs->Mainboard) < 0;
			case MASK_MEMORY:
				{
					DWORD lhsize = 0, rhsize = 0;
					stdex::ToNumber(lhsize, lhs->Memory);
					stdex::ToNumber(rhsize, rhs->Memory);
					return ascSort_ ? lhsize > rhsize : lhsize < rhsize;
				}
			case MASK_DISK:
				return ascSort_ ? utility::Strcmp(lhs->Disk, rhs->Disk) > 0 : utility::Strcmp(lhs->Disk, rhs->Disk) < 0;
			case MASK_VIDEO:
				return ascSort_ ? utility::Strcmp(lhs->Video, rhs->Video) > 0 : utility::Strcmp(lhs->Video, rhs->Video) < 0;
			case MASK_AUDIO:
				return ascSort_ ? utility::Strcmp(lhs->Audio, rhs->Audio) > 0 : utility::Strcmp(lhs->Audio, rhs->Audio) < 0;
			case MASK_NETWORK:
				return ascSort_ ? utility::Strcmp(lhs->Network, rhs->Network) > 0 : utility::Strcmp(lhs->Network, rhs->Network) < 0;
			case MASK_CAMERA:
				return ascSort_ ? utility::Strcmp(lhs->Camera, rhs->Camera) > 0 : utility::Strcmp(lhs->Camera, rhs->Camera) < 0;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

	template<>
	struct Compare<CDlgClient::Version>
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
			case MASK_NAME:
				return ascSort_ ? lhs->Online > rhs->Online : lhs->Online < rhs->Online;
			case MASK_PROTVER:
				return ascSort_ ? utility::Strcmp(lhs->ProtVer, rhs->ProtVer) > 0 : utility::Strcmp(lhs->ProtVer, rhs->ProtVer) < 0;
			case MASK_VDISKVER:
				return ascSort_ ? utility::Strcmp(lhs->VDiskVer, rhs->VDiskVer) > 0 : utility::Strcmp(lhs->VDiskVer, rhs->VDiskVer) < 0;
			case MASK_MENUVER:
				return ascSort_ ? utility::Strcmp(lhs->MenuVer, rhs->MenuVer) > 0 : utility::Strcmp(lhs->MenuVer, rhs->MenuVer) < 0;
			case MASK_CLIENTSVRVER:
				return ascSort_ ? utility::Strcmp(lhs->CliSvrVer, rhs->CliSvrVer) > 0 : utility::Strcmp(lhs->CliSvrVer, rhs->CliSvrVer) < 0;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

}



void CDlgClient::_Init()
{
	i8desk::business::Area::EnsureDefaultArea();

	// 初始化TreeCtrl
	InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetAreas());
}

void CDlgClient::_ShowRealClients()
{
	using namespace i8desk::data_helper;

	ClientTraits::VectorType curClients;
	curClients = curClients_;
	curClients_.clear();
	// 统计符合当前AID的个数
	i8desk::business::Client::AreaRealFilter(curClients, curClients_, AID_);
	

	static size_t count = 0;
	size_t cnt = curClients_.size();
	if( count != cnt || _GetCurList()->GetItemCount() != count )
	{
		count = cnt;
		_GetCurList()->SetItemCount(cnt);
	}
	else
	{
		_GetCurList()->RedrawCurPageItems();
	}

	// 监测温度是否超过阀值
	const int temprature = i8desk::GetDataMgr().GetOptVal(OPT_M_CPU_TEMPRATURE, 60);
	for(size_t i = 0; i != curClients_.size(); ++i)
	{
		if( curClients[i]->Temperature > temprature )
		{
			if( !i8desk::business::monitor::IsException(curClients_[i]->Name, i8desk::IP2String(curClients_[i]->IP)).first )
			{
				i8desk::business::monitor::PushExcept(std::tr1::make_tuple(curClients_[i], (DWORD)MASK_TCLIENT_TEMPERATURE));
				AfxGetApp()->GetMainWnd()->PostMessage(WM_USER + 0x112);
			}
		}
	}
}


void CDlgClient::_ShowClients()
{
	using i8desk::data_helper::ClientTraits;

	curClients_.clear();

	// 统计符合当前AID的个数
	i8desk::business::Client::AreaFilter(curClients_, AID_);

	static size_t count = 0;
	size_t cnt = curClients_.size();
	if( count != cnt || _GetCurList()->GetItemCount() != count )
	{
		count = cnt;
		_GetCurList()->SetItemCount(cnt);
	}
	else
	{
		_GetCurList()->RedrawCurPageItems();
	}

	// 监测温度是否超过阀值
	const int temprature = i8desk::GetDataMgr().GetOptVal(OPT_M_CPU_TEMPRATURE, 60);
	for(size_t i = 0; i != curClients_.size(); ++i)
	{
		if( curClients_[i]->Temperature > temprature )
		{
			if( !i8desk::business::monitor::IsException(curClients_[i]->Name, i8desk::IP2String(curClients_[i]->IP)).first )
			{
				i8desk::business::monitor::PushExcept(std::tr1::make_tuple(curClients_[i], (DWORD)MASK_TCLIENT_TEMPERATURE));
				AfxGetApp()->GetMainWnd()->PostMessage(WM_USER + 0x112);
			}
		}
	}
}


void CDlgClient::_ShowControlMenu(int nItem)
{
	if( nItem == -1 )
		return;

	CPoint oPoint;
	GetCursorPos( &oPoint );

	CMenu menu;
	menu.LoadMenu(IDR_MENU_CLIENT_CONTROL);
	CMenu* pMenu = menu.GetSubMenu(0);

	// 调整下载菜单
	using i8desk::data_helper::ClientTraits;
	int isOnline = curClients_[nItem]->Online;

	pMenu->EnableMenuItem(ID_CONTROL_START,			MF_BYCOMMAND | (isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_RESTART,		MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_SHUTDOWN,		MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_RUN_CLIENT_FILE,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_DISK_RECORVER,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_INSTALL_ALL,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_INSTALL,		MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_PROTO_AREA,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_RSTORE,		MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_VIEW_SYSTEM,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_VIEW_PROCESS,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_VIEW_SERVICE,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_VIEW_GAMES,	MF_BYCOMMAND | (isOnline ? MF_ENABLED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_REMOTE_CONTROL,MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_REMOTE_VIEW,	MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));
	pMenu->EnableMenuItem(ID_CONTROL_REFRESH,		MF_BYCOMMAND | (!isOnline ? MF_GRAYED : MF_ENABLED));


	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, oPoint.x, oPoint.y, this);
}


CCustomListCtrl *CDlgClient::_GetCurList()
{
	CCustomListCtrl *curList = &wndListBasic_;
	switch(viewStatus_)
	{
	case Basic:
		curList = &wndListBasic_;
		break;
	case System:
		curList = &wndListSystem_;
		break;
	case Version:
		curList = &wndListVersion_;
		break;
	default:
		assert(0);
		break;
	}

	return curList;
}



void CDlgClient::Register()
{
	i8desk::GetRealDataMgr().Register(
		std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_CLIENT);
}

void CDlgClient::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_CLIENT);
}

void CDlgClient::OnRealDataUpdate()
{
	_ShowRealClients();
}

void CDlgClient::OnReConnect()
{
	i8desk::GetDataMgr().GetClients().clear();
	i8desk::GetDataMgr().GetAreas().clear();
}

void CDlgClient::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClients());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetAreas());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
}

void CDlgClient::OnDataComplate()
{
	_Init();
}


BOOL CDlgClient::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	// 初始化ComboBox
	wndComboChangeView_.AddString(_T("基本视图"));
	wndComboChangeView_.AddString(_T("硬件视图"));
	wndComboChangeView_.AddString(_T("版本视图"));
	wndComboChangeView_.SetCurSel(0);

	// 初始化ImageList
	treeImages_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);

	HBITMAP tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("Client/Ico_Clients.png"));
	treeImages_.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));
	wndTree_.SetImageList(&treeImages_, TVSIL_NORMAL);


	// 初始化ListCtrl
	InitListBasic(wndListBasic_);
	InitListHardware(wndListSystem_);
	InitListVersion(wndListVersion_);

	wndListBasic_.ShowWindow(SW_SHOW);
	wndListSystem_.ShowWindow(SW_HIDE);
	wndListVersion_.ShowWindow(SW_HIDE);

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
	outBox_.Attach(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png")));

	
	HBITMAP bk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/InputBg.png"));
	HBITMAP editBk = i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ComboEditBK.png"));

	// 初始化Button
	HBITMAP add[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Press.png"))
	};
	wndBtnAreaAdd_.SetImages(add[0], add[1], add[2]);

	HBITMAP del[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Press.png"))
	};
	wndBtnAreaDel_.SetImages(del[0], del[1], del[2]);

	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndBtnAreaMod_.SetImages(modify[0], modify[1], modify[2]);

	wndBtnClientMgr_.SetImages(leftArea[0], leftArea[0], leftArea[0]);


	// 注册窗口
	wndMgr_.Register(wndListBasic_.GetDlgCtrlID(), &wndListBasic_);
	wndMgr_.Register(wndListSystem_.GetDlgCtrlID(), &wndListSystem_);
	wndMgr_.Register(wndListVersion_.GetDlgCtrlID(), &wndListVersion_);

	wndMgr_.SetDestWindow(&wndListBasic_);
	wndMgr_[wndListBasic_.GetDlgCtrlID()];

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgClient::OnDestroy()
{
	CDialog::OnDestroy();
}


void CDlgClient::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, leftArea_, rightArea_);

	// 右边工作区内容边框
	CRect rcWork;
	wndListBasic_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);

	CRect rcWindow;
	wndComboChangeView_.GetWindowRect(rcWindow);
	i8desk::ui::DrawFrame(this, memDC, rcWindow, &outBox_);
}

BOOL CDlgClient::OnEraseBkgnd(CDC *pDC)
{

	return TRUE;//__super::OnEraseBkgnd(pDC);
}

void CDlgClient::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	wndMgr_.UpdateSize();
	if( !::IsWindow(wndListBasic_.GetSafeHwnd()) )
		return;

	wndTree_.Invalidate();
	wndListBasic_.Invalidate();
	wndListSystem_.Invalidate();
	wndListVersion_.Invalidate();

	wndBtnClientMgr_.Invalidate();
	wndBtnAreaAdd_.Invalidate();
	wndBtnAreaDel_.Invalidate();
	wndBtnAreaMod_.Invalidate();

	wndBtnAdd_.Invalidate();
	wndBtnMod_.Invalidate();
	wndBtnDel_.Invalidate();

	wndBtnRemote_.Invalidate();
	wndBtnProtect_.Invalidate();
	wndBtnRestore_.Invalidate();
	wndBtnSaveHardware_.Invalidate();

	wndComboChangeView_.Invalidate();
}

void CDlgClient::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
}

LRESULT CDlgClient::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	wndComboChangeView_.SetCurSel(wParam);
	switch(wParam)
	{
	case 0:
		_ChangeView(ID_CLIENT_CHANGE_BASIC);
		break;
	case 1:
		_ChangeView(ID_CLIENT_CHANGE_HARDWARE);
		break;
	case 2:
		_ChangeView(ID_CLIENT_CHANGE_VERSION);
		break;
	default:
		break;
	}

	return TRUE;
}

void CDlgClient::OnBnClickedButtonClientSelectView()
{

}


void CDlgClient::OnTvnSelchangedTreeClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	using namespace i8desk::data_helper;
	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndTree_.GetItemData(hTreeItem));

	if( val != 0 )
		AID_ = val->AID;
	else
		AID_.clear();

    _ShowClients();
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
}

void CDlgClient::OnLvnColumnclickListClientBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListBasic_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curClients_.begin(), curClients_.end(), Compare<Basic>(pNMLV->iSubItem, BasicCompare[pNMLV->iSubItem]));
	BasicCompare.set(pNMLV->iSubItem, !BasicCompare[pNMLV->iSubItem]);
	wndListBasic_.RedrawCurPageItems();

	wndListBasic_.SetSortArrow(pNMLV->iSubItem, BasicCompare[pNMLV->iSubItem]);
}

void CDlgClient::OnLvnColumnclickListClientSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListSystem_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curClients_.begin(), curClients_.end(), Compare<System>(pNMLV->iSubItem, HardwareCompare[pNMLV->iSubItem]));
	HardwareCompare.set(pNMLV->iSubItem, !HardwareCompare[pNMLV->iSubItem]);
	wndListSystem_.RedrawCurPageItems();

	wndListSystem_.SetSortArrow(pNMLV->iSubItem, HardwareCompare[pNMLV->iSubItem]);
}

void CDlgClient::OnLvnColumnclickListClientVersion(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListVersion_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curClients_.begin(), curClients_.end(), Compare<Version>(pNMLV->iSubItem, VersionCompare[pNMLV->iSubItem]));
	VersionCompare.set(pNMLV->iSubItem, !VersionCompare[pNMLV->iSubItem]);
	wndListVersion_.RedrawCurPageItems();

	wndListVersion_.SetSortArrow(pNMLV->iSubItem, VersionCompare[pNMLV->iSubItem]);
}

void CDlgClient::OnLvnGetdispinfoListClientBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	if( static_cast<size_t>(itemIndex) >= curClients_.size() )
		return;

	const i8desk::data_helper::ClientTraits::ValueType &val = curClients_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = val->Name;
			break;
		case MASK_PROTINSTALL:
			text = val->ProtInstall ? _T("安装") : _T("未安装");
			break;
		case MASK_PARTITION:
			text = val->Partition;
			break;
		case MASK_IP:
			text = i8desk::IP2String(val->IP);
			break;
		case MASK_MARK:
			text = i8desk::IP2String(val->Mark);
			break;
		case MASK_GATE:
			text = i8desk::IP2String(val->Gate);
			break;
		case MASK_DNS:
			{
				if( val->DNS2 != 0 )
				{
					stdex::tString DNS = i8desk::IP2String(val->DNS);
					stdex::tString DNS2 = i8desk::IP2String(val->DNS2);
					text = DNS + _T(":") + DNS2;
				}
				else
					text = i8desk::IP2String(val->DNS);
			}
			break;
		case MASK_MAC:
			text = val->MAC;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

	if( (pItem->mask & LVIF_IMAGE) && pItem->iSubItem == 0 )
	{
		iconCol[0]->SetIconState(itemIndex, val->Online ? 1 : 0);
	}

}

void CDlgClient::OnLvnGetdispinfoListClientSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	if( static_cast<size_t>(itemIndex) >= curClients_.size() )
		return;

	const i8desk::data_helper::ClientTraits::ValueType &val = curClients_[itemIndex];
	if(pItem->iSubItem > 1)
	{
		std::pair<bool, UINT> ret = i8desk::business::monitor::IsException(val->Name, i8desk::IP2String(val->IP));
		if( ret.first && (ret.second & Col2Mask[pItem->iSubItem]) )
			HardwareTrait[pItem->iSubItem]->SetTextColor(itemIndex, pItem->iSubItem, RGB(255, 0, 0));
		else
			HardwareTrait[pItem->iSubItem]->SetTextColor(itemIndex, pItem->iSubItem, RGB(0, 0, 0));
	}

	stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		stdex::tOstringstream os;

		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = val->Name;
			break;
		case MASK_SYSTEM:
			text = val->System;
			break;
		case MASK_CPU:
			text = val->CPU;
			break;
		case MASK_TEMPRATURE:
			stdex::ToString(val->Temperature, text);
			break;
		case MASK_MAINBOARD:
			text = val->Mainboard;
			break;
		case MASK_MEMORY:
			if(utility::Strcmp(val->Memory ,_T("")) != 0 )
				os << val->Memory << _T(" MB");
			text = os.str();
			break;
		case MASK_DISK:
			text = val->Disk;
			break;
		case MASK_VIDEO:
			text = val->Video;
			break;
		case MASK_AUDIO:
			text = val->Audio;
			break;
		case MASK_NETWORK:
			text = val->Network;
			break;
		case MASK_CAMERA:
			text = val->Camera;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

	if( (pItem->mask & LVIF_IMAGE) && pItem->iSubItem == 0 )
	{
		iconCol[1]->SetIconState(itemIndex, val->Online ? 1 : 0);
	}
}

void CDlgClient::OnLvnGetdispinfoListClientVersion(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curClients_.size() )
		return;

	const i8desk::data_helper::ClientTraits::ValueType &val = curClients_[itemIndex];


	stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = val->Name;
			break;
		case MASK_PROTVER:
			text = val->ProtVer;
			break;
		case MASK_VDISKVER:
			text = val->VDiskVer;
			break;
		case MASK_MENUVER:
			text = val->MenuVer;
			break;
		case MASK_CLIENTSVRVER:
			text = val->CliSvrVer;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

	if( (pItem->mask & LVIF_IMAGE) && pItem->iSubItem == 0 )
	{
		iconCol[2]->SetIconState(itemIndex, val->Online ? 1 : 0);
	}
}



void CDlgClient::OnNMRClickListClientSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	_ShowControlMenu(pNMItemActivate->iItem);

}

void CDlgClient::OnNMRClickListClientBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	_ShowControlMenu(pNMItemActivate->iItem);
}

void CDlgClient::OnNMRClickListClientVersion(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	_ShowControlMenu(pNMItemActivate->iItem);
}


void CDlgClient::OnBnClickedButtonClientAreaAdd()
{
	CDlgClientArea dlg(true, 0);
	if( dlg.DoModal() == IDOK )
	{
		InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetAreas());
	}
}

void CDlgClient::OnBnClickedButtonClientAreaDelete()
{
	HTREEITEM hItem = wndTree_.GetSelectedItem();
	using i8desk::data_helper::AreaTraits;
	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndTree_.GetItemData(hItem));
	if( val == 0 )
		return;

	if( i8desk::business::Area::IsDefaultArea(i8desk::GetDataMgr().GetAreas()[val->AID]) ) 
	{
		stdex::tString msg = _T("默认分区不能被删除,请选择别的分区! ");
		CMessageBox msgDlg(_T("删除区域"), msg);
		msgDlg.DoModal();
		return;
	}

	stdex::tString msg = _T("是否需要删除区域: ");
	msg += val->Name;
	CMessageBox msgDlg(_T("删除区域"), msg);

	if( msgDlg.DoModal() == IDOK)
	{
		i8desk::business::Game::EraseRunType(val->AID);

		if( i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetAreas(), val->AID) )
		{
			InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetAreas());
		}
	}
}

void CDlgClient::OnBnClickedButtonClientAreaModify()
{
	HTREEITEM hItem = wndTree_.GetSelectedItem();
	using i8desk::data_helper::AreaTraits;
	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndTree_.GetItemData(hItem));
	if( val == 0 )
		return;
	
	if ( utility::Strcmp(val->AID, DEFAULT_AREA_GUID) == 0  )
	{
		stdex::tString msg = _T("默认区域不能修改！ ");
		CMessageBox msgDlg(_T("客户机区域"), msg);
		msgDlg.DoModal();
		return;
	}

	CDlgClientArea dlg(false, val);
	if( dlg.DoModal() == IDOK )
	{
		InitTreeCtrl(wndTree_, i8desk::GetDataMgr().GetAreas());
	}
}


void CDlgClient::OnBnClickedButtonClientAdd()
{
	HTREEITEM hItem = wndTree_.GetSelectedItem();
	using i8desk::data_helper::AreaTraits;
	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndTree_.GetItemData(hItem));

	CDlgClientAdd dlg(val);
	if( dlg.DoModal() == IDOK )
	{
		_ShowClients();
	}
}

void CDlgClient::OnBnClickedButtonClientModify()
{
	HTREEITEM hItem = wndTree_.GetSelectedItem();
	using i8desk::data_helper::AreaTraits;
	AreaTraits::ElementType *val = reinterpret_cast<AreaTraits::ElementType *>(wndTree_.GetItemData(hItem));

	ClientTraits::VectorType clients;

	GetSelectClients(clients);
	if( clients.empty() )
		return;

	CDlgClientModify dlg(val, clients);
	if( dlg.DoModal() == IDOK )
	{
		_ShowClients();
	}
}

void CDlgClient::OnBnClickedButtonClientDelete()
{
	CListCtrl *curList = _GetCurList();

	if( curList->GetSelectedCount() == 0 )
		return;

	CMessageBox msgDlg(_T("删除客户端"), _T("确定要删除选定的客户端吗?"));
	if( msgDlg.DoModal() == IDCANCEL )
		return;

	int nSel = -1;;

	typedef std::vector<DWORD> SelClient;
	SelClient vecIdx;
	while((nSel = curList->GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		vecIdx.push_back(nSel);
	}

	using i8desk::data_helper::ClientTraits;
	ClientTraits::MapType &clients = i8desk::GetDataMgr().GetClients();
	for(SelClient::const_iterator rit = vecIdx.begin(); rit != vecIdx.end(); ++rit)
	{
		nSel = *rit;
		stdex::tString name = (LPCTSTR)curList->GetItemText(nSel, 1);

		ClientTraits::MapConstIterator iter = clients.find(name);
		if( iter == clients.end() )
			break;

		if( !i8desk::GetDataMgr().DelData(clients, iter->second->Name) )
			break;
	}

    _ShowClients();
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
}

namespace
{
	using namespace i8desk::business::Client;
	using i8desk::data_helper::ClientTraits;
}

void CDlgClient::OnBnClickedButtonClientControl()
{
	ClientTraits::VectorType clients;
	GetSelectClients(clients);
	if( clients.size()  && clients[0]->Online == 1 )
		ControlRemoteControl(clients) ;
}

void CDlgClient::OnBnClickedButtonClientAreaProtect()
{
	ClientTraits::VectorType clients;
	GetSelectClients(clients);

	stdex::tString protAreas;
	if( _GetCurList()->GetSelectedCount() == 1 )
	{ 
		stdex::tString partition = clients[0]->Partition;
		i8desk::business::Client::GetProtArea( partition,protAreas);
	}

	CDlgProtArea dlg(_T(""), protAreas.c_str());
	if( dlg.DoModal() == IDOK )
	{
		ControlProtoArea(clients, (LPCTSTR)dlg.ProtAreas(), (LPCTSTR)dlg.UnProtAreas());
	}

}

void CDlgClient::OnBnClickedButtonClientInstallRestore()
{
	ClientTraits::VectorType clients;
	GetSelectClients(clients);
	ControlInstall(clients);
}


void CDlgClient::OnBnClickedButtonClientSaveHardware()
{
	i8desk::GetControlMgr().SaveHardware();
	i8desk::business::monitor::ClearException();

	wndListSystem_.RedrawCurPageItems();
	AfxGetMainWnd()->PostMessage(i8desk::ui::WM_SAVE_HARDWARE_MSG);
}

void CDlgClient::_ChangeView(UINT uID)
{
	switch(uID)
	{
	case ID_CLIENT_CHANGE_BASIC:
		wndMgr_[wndListBasic_.GetDlgCtrlID()];
		viewStatus_ = Basic;
		break;
	case ID_CLIENT_CHANGE_HARDWARE:
		wndMgr_[wndListSystem_.GetDlgCtrlID()];
		viewStatus_ = System;
		break;
	case ID_CLIENT_CHANGE_VERSION:
		wndMgr_[wndListVersion_.GetDlgCtrlID()];
		viewStatus_ = Version;
		break;
	default:
		assert(0);
		break;
    }
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);

	ClientViewShow  = uID;
	_ShowClients();
}

void CDlgClient::_ChangeHardware(UINT mask)
{
	
}

void CDlgClient::OnComboChangeView()
{
	int nSel = wndComboChangeView_.GetCurSel();
	
	UINT id = 0;
	if( nSel == 0 )
		id = ID_CLIENT_CHANGE_BASIC;
	else if( nSel == 1 )
		id = ID_CLIENT_CHANGE_HARDWARE;
	else if( nSel == 2 )
		id = ID_CLIENT_CHANGE_VERSION;
	else
		id = ID_CLIENT_CHANGE_BASIC;

	_ChangeView(id);
}

void CDlgClient::OnControlOperate(UINT uID)
{
	size_t i = 0;
	ClientTraits::VectorType clients;

	GetSelectClients(clients);

	switch(uID)
	{
	case ID_CONTROL_START:
		ControlStart(clients);
		break;
	case ID_CONTROL_RESTART:
		ControlRestart(clients);
		break;
	case ID_CONTROL_SHUTDOWN:
		ControlShutdown(clients);
		break;
	case ID_CONTROL_DISK_RECORVER:
		{
			CDlgChkDisk dlg(_T(""), _T(""));
			if( dlg.DoModal() == IDOK )
			{
				ControlDiskRecorver(clients, (LPCTSTR)dlg.ChkAreas());
			}
		}
		break;
	case ID_CONTROL_RUN_CLIENT_FILE:
		{
			CDlgClientRunExe dlg;
			if( dlg.DoModal() == IDOK )
			{
				ControlRunClientFile(clients,(LPCTSTR)dlg.GetPath());
			}
		}
		break;
	case ID_CONTROL_INSTALL_ALL:
		ControlInstallAll(clients);
		break;
	case ID_CONTROL_INSTALL:
		ControlInstall(clients);
		break;
	case ID_CONTROL_PROTO_AREA:
		{
			stdex::tString protAreas;
			if( _GetCurList()->GetSelectedCount() == 1 )
			{
				stdex::tString partition = clients[0]->Partition;
				i8desk::business::Client::GetProtArea( partition,protAreas);
			}

			CDlgProtArea dlg(_T(""), protAreas.c_str());
			if( dlg.DoModal() == IDOK )
			{
				ControlProtoArea(clients, (LPCTSTR)dlg.ProtAreas(), (LPCTSTR)dlg.UnProtAreas());
			}

		}
		break;
	case ID_CONTROL_RSTORE:
		ControlRestore(clients);
		break;
	case ID_CONTROL_VIEW_SYSTEM:
		{
			CDlgClientSysInfo dlg(clients,i8desk::Ctrl_ViewSysInfo);
			dlg.DoModal();
		}
		break;
	case ID_CONTROL_VIEW_PROCESS:
		{	
			CDlgClientSysInfo dlg(clients,i8desk::Ctrl_ViewProcInfo);
			dlg.DoModal();
		}
		break;
	case ID_CONTROL_VIEW_SERVICE:
		{
			CDlgClientSysInfo dlg(clients,i8desk::Ctrl_ViewSvrInfo);
			dlg.DoModal();
		}
		break;
	case ID_CONTROL_VIEW_GAMES:
		{
			size_t sel = _GetCurList()->GetNextItem(-1, LVIS_SELECTED);
			CDlgClientClientViewGame dlg(curClients_[sel]->IP);
			dlg.DoModal();
		}
		break;
	case ID_CONTROL_REMOTE_CONTROL:
		ControlRemoteControl(clients) ;
		break;
	case ID_CONTROL_REMOTE_VIEW:
		ControlRemoteView(clients);
		break;
	case ID_CONTROL_REFRESH:
		//ControlRefresh;
		break;
	default:
		assert(0);
	}
}

void CDlgClient::GetSelectClients(i8desk::data_helper::ClientTraits::VectorType &clients )
{
	int nSel = -1;
	while((nSel = _GetCurList()->GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		clients.push_back(curClients_[nSel]);
	}
}

void CDlgClient::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
    *pResult = 0;
}

void CDlgClient::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    if (pLVKeyDow != NULL && (pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_DOWN || (pLVKeyDow->wVKey == 'A' && (pLVKeyDow->flags & MK_CONTROL))))
    {
        PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
    }
    *pResult = 0;
}

LRESULT CDlgClient::OnReportSelectState(WPARAM wParam, LPARAM lParam)
{
    switch (viewStatus_)
    {
    case Basic:
        GetParent()->PostMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, 0, wndListBasic_.GetSelectedCount());
        break;
    case System:
        GetParent()->PostMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, 0, wndListSystem_.GetSelectedCount());
        break;
    case Version:
        GetParent()->PostMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, 0, wndListVersion_.GetSelectedCount());
        break;
    default:
        break;
    }
    return 1;
}
