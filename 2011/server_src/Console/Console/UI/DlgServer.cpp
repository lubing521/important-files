// DlgServer.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgServer.h"


#include "../ManagerInstance.h"
#include "../GridListCtrlEx/CGridColumnTraitText.h"

#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/ui/wtl/atlwinmisc.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/I8Type.hpp"
#include "../../../../include/UpdateGame.h"

#include "DlgServerInfo.h"
#include "DlgVDiskMgr.h"
#include "DlgVDiskInfo.h"
#include "../MessageBox.h"

#include "UIHelper.h"

#include "../Misc.h"

#include "../Business/ServerBusiness.h"
#include "../Business/SyncTaskBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/ClientBusiness.h"
#include "../Business/GameBusiness.h"

#include "WindowManager.h"
#include "../AsyncDataHelper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum { WM_ADD_NEW_SERVER_MSG = WM_USER + 0x10 };

namespace
{
	CCustomColumnTraitServerInfoStatus *svrInfo;

	namespace detail
	{
		CCustomColumnTraitServerInfoStatus *GetSvrInfoTrait()
		{
			svrInfo = new CCustomColumnTraitServerInfoStatus(7);
			svrInfo->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return svrInfo;
		}

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

}

// CDlgServer
IMPLEMENT_DYNCREATE(CDlgServer, CDialog)

CDlgServer::CDlgServer(CWnd* pParent)
: i8desk::ui::BaseWnd(CDlgServer::IDD, pParent)
, viewStatus_(BASIC)
{

}

CDlgServer::~CDlgServer()
{
}

void CDlgServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SERVER_SERVER, wndTreeServer_);
	DDX_Control(pDX, IDC_LIST_SERVER_SERVER, wndListServerBasic_);
	DDX_Control(pDX, IDC_LIST_SERVER_SERVER_VIRTUAL, wndListServerVDisk_);

	DDX_Control(pDX, IDC_BUTTON_SERVER_VIRTUAL_MGR, wndBtnVirtualMgr_);
	DDX_Control(pDX, IDC_BUTTON_SERVER_SERVER_MGR, wndBtnServerMgr_);
	DDX_Control(pDX, IDC_BUTTON_STATIC_SERVER_MGR, wndStaticServerMgr_);
	DDX_Control(pDX, IDC_EDIT_SERVER_SEARCH, wndEditQuery_);
	DDX_Control(pDX, IDC_BUTTON_SERVER_SEARCH, wndBtnQuery_);
}

BEGIN_EASYSIZE_MAP(CDlgServer)   
	EASYSIZE(IDC_LIST_SERVER_SERVER, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)

	EASYSIZE(IDC_BUTTON_STATIC_SERVER_MGR, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_SERVER_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_EDIT_SERVER_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)

END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgServer, i8desk::ui::BaseWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SERVER_SERVER, &CDlgServer::OnLvnGetdispinfoListServerBasic)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SERVER_SERVER_VIRTUAL, &CDlgServer::OnLvnGetdispinfoListServerVDisk)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SERVER_SERVER, &CDlgServer::OnTvnSelchangedTreeServerServer)

	ON_BN_CLICKED(IDC_BUTTON_SERVER_SERVER_MGR, &CDlgServer::OnBnClickedButtonServerServerMgr)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_VIRTUAL_MGR, &CDlgServer::OnBnClickedButtonServerVirtualMgr)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_SEARCH, &CDlgServer::ONBnClickedButtonSearch)


	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgServer::OnApplySelect)
	ON_MESSAGE(i8desk::ui::WM_ADD_VDISK_MSG, &CDlgServer::OnAddVDisk)
	ON_MESSAGE(i8desk::ui::WM_DEL_VDISK_MSG, &CDlgServer::OnDelVDisk)
	ON_MESSAGE(i8desk::ui::WM_MOD_VDISK_MSG, &CDlgServer::OnModVDisk)
	ON_MESSAGE(WM_ADD_NEW_SERVER_MSG, &CDlgServer::OnAddServer)
END_MESSAGE_MAP()




namespace
{
	struct ErrorMsg
	{
		void operator()(LPCTSTR tip, LPCTSTR msg)
		{
			CMessageBox box(tip, msg);
			box.DoModal();
		}
	};

	template < typename TreeCtrlT , typename ServersT, typename VDiskT >
	void InitServerTreeCtrl(TreeCtrlT &treeServer, ServersT &servers, const VDiskT &vDisks)
	{
		treeServer.DeleteAllItems();

		using i8desk::data_helper::ServerTraits;
		ServerTraits::VectorType vecServers;

		for(typename ServersT::const_iterator iter = servers.begin(); iter != servers.end(); ++iter)
		{
			if( i8desk::business::Server::IsMainServer(iter->second) )
				vecServers.insert(vecServers.begin(), iter->second);
			else
				vecServers.push_back(iter->second);
		}

		for(ServerTraits::VecConstIterator iter = vecServers.begin(); iter != vecServers.end(); ++iter)
		{
			bool isMain = iter == vecServers.begin();
			stdex::tOstringstream os;
			os << (*iter)->SvrName << (isMain ? _T("(主)") : _T("(从)")); 
			HTREEITEM hParentItem = treeServer.InsertItem(os.str().c_str(), isMain ? 0 : 1, isMain ? 0 : 1);
			treeServer.SetItemData(hParentItem, reinterpret_cast<DWORD_PTR>((*iter).get()));

			for(typename VDiskT::const_iterator vIter = vDisks.begin(); vIter != vDisks.end(); ++vIter)
			{
				if( utility::Strcmp(vIter->second->SvrID, (*iter)->SvrID) == 0 )
				{
					HTREEITEM hItem = treeServer.InsertItem(i8desk::business::VDisk::GetVDiskName(vIter->second).c_str(), -1, -1, hParentItem);
					treeServer.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(vIter->second.get()));
				}
			}

			treeServer.Expand(hParentItem, TVE_EXPAND);
		}

		treeServer.SetItemHeight(25);
		treeServer.SelectItem(treeServer.GetRootItem());
	}

	CCustomColumnTrait *textClrCol = 0;

	template < typename ListT, typename GamesT >
	CCustomColumnTraitGameIcon *GetIconTrait(ListT &list, GamesT &games)
	{
		CCustomColumnTraitGameIcon *icon = new CCustomColumnTraitGameIcon(list, games);
		icon->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return icon;
	}

	enum { MASK_NAME = 1, MASK_IP, MASK_READ_BYTES, MASK_READ_RATE, MASK_READ_UP, MASK_CONNECT_TIME, MASK_CONNECT_CATCHSHOT };

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



	CCustomColumnTrait *GetTrait()
	{
		CCustomColumnTrait *rowTrait = new CCustomColumnTrait;
		rowTrait->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

		return rowTrait;
	}

	template < typename ListCtrlT >
	void InitListCtrl(ListCtrlT &listBasic, ListCtrlT &listVirtual)
	{
		listBasic.InsertHiddenLabelColumn();

		listBasic.InsertColumnTrait(1,					_T("类型"),		LVCFMT_LEFT, 200, 1,	detail::GetTrait());
		listBasic.InsertColumnTrait(2,					_T("状态"),		LVCFMT_LEFT, 500, 2,	detail::GetSvrInfoTrait());

		CCustomColumnTraitIcon *pIconTrait = new CCustomColumnTraitIcon(listVirtual);
		pIconTrait->SetBkImg(
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
			i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
		pIconTrait->AddIcon(i8desk::ui::SkinMgrInstance().GetSkin(_T("Server/Icon_Client.png")));
		listBasic.SetItemHeight(24);


		listVirtual.InsertHiddenLabelColumn();
		listVirtual.InsertColumnTrait(MASK_NAME,		_T("机器名"),		LVCFMT_LEFT, 150, MASK_NAME,		pIconTrait);
		listVirtual.InsertColumnTrait(MASK_IP,			_T("IP地址"),	LVCFMT_LEFT, 100, MASK_IP,			textClrCol = detail::GetTrait());
		listVirtual.InsertColumnTrait(MASK_READ_BYTES,	_T("读取数据量"),	LVCFMT_LEFT, 100, MASK_READ_BYTES,	detail::GetTrait());
		listVirtual.InsertColumnTrait(MASK_READ_RATE,	_T("读取速度"),	LVCFMT_LEFT, 100, MASK_READ_RATE,	detail::GetTrait());
		listVirtual.InsertColumnTrait(MASK_READ_UP,		_T("读取峰值"),	LVCFMT_LEFT, 100, MASK_READ_UP,		detail::GetTrait());
		listVirtual.InsertColumnTrait(MASK_CONNECT_TIME,_T("连接时长"),	LVCFMT_LEFT, 100, MASK_CONNECT_TIME,detail::GetTrait());
		listVirtual.InsertColumnTrait(MASK_CONNECT_CATCHSHOT,_T("缓存命中率"),	LVCFMT_LEFT, 100, MASK_CONNECT_CATCHSHOT,detail::GetTrait());

		listVirtual.SetItemHeight(24);
	}


	CCustomProgressColumnTrait *syncProgTrait = 0;
	CCustomColumnTrait *syncClrState = 0;
	CCustomColumnTraitGameIcon *syncIcon = 0;

	enum 
	{ 
		MASK_SYNC_NAME = 1, MASK_SYNC_GID, MASK_SYNC_SIZE, MASK_SYNC_STATE, MASK_SYNC_PROGRESS, 
		MASK_SYNC_SPEED, MASK_SYNC_MAINSVR, MASK_SYNC_NODESVR, MASK_SYNC_SRC, MASK_SYNC_DST 
	};

	template < typename ListCtrlT, typename GamesT >
	void InitSyncTaskListCtrl(ListCtrlT &list, GamesT &games)
	{
		list.SetItemHeight(24);

		list.InsertHiddenLabelColumn();
		list.InsertColumnTrait(MASK_SYNC_NAME,		_T("游戏名称"),	LVCFMT_LEFT, 100, MASK_SYNC_NAME,		GetTrait()/*syncIcon = GetIconTrait(list, games)*/);
		list.InsertColumnTrait(MASK_SYNC_GID,		_T("GID"),		LVCFMT_LEFT, 50,  MASK_SYNC_GID,		GetTrait());
		list.InsertColumnTrait(MASK_SYNC_SIZE,		_T("游戏大小"),	LVCFMT_LEFT, 80,  MASK_SYNC_SIZE,		GetTrait());
		list.InsertColumnTrait(MASK_SYNC_STATE,		_T("状态"),		LVCFMT_LEFT, 50,  MASK_SYNC_STATE,		syncClrState = GetTrait());
		list.InsertColumnTrait(MASK_SYNC_PROGRESS,	_T("当前进度"),	LVCFMT_LEFT, 100, MASK_SYNC_PROGRESS,	syncProgTrait = GetProgressTrait(list));
		list.InsertColumnTrait(MASK_SYNC_SPEED,		_T("速度"),		LVCFMT_LEFT, 50,  MASK_SYNC_SPEED,		GetTrait());
		list.InsertColumnTrait(MASK_SYNC_MAINSVR,	_T("源服务器"),	LVCFMT_LEFT, 100, MASK_SYNC_MAINSVR,	GetTrait());
		list.InsertColumnTrait(MASK_SYNC_NODESVR,	_T("目标服务器"),	LVCFMT_LEFT, 100, MASK_SYNC_NODESVR,	GetTrait());
		list.InsertColumnTrait(MASK_SYNC_SRC,		_T("源路径"),		LVCFMT_LEFT, 100, MASK_SYNC_SRC,		GetTrait());
		list.InsertColumnTrait(MASK_SYNC_DST,		_T("目标路径"),	LVCFMT_LEFT, 100, MASK_SYNC_DST,		GetTrait());
	}


	template < typename TreeCtrlT, typename ValueT >
	void GetTreeCurSel(TreeCtrlT &tree, ValueT &val, bool isServer = true)
	{
		HTREEITEM hSelItem = tree.GetSelectedItem();
		if( hSelItem == NULL )
			return;

		if( isServer )
		{
			HTREEITEM hParentItem = tree.GetParentItem(hSelItem);
			if( hParentItem != NULL )
				hSelItem = hParentItem;
		}

		val = reinterpret_cast<ValueT>(tree.GetItemData(hSelItem));
	}


	enum MaskServerStatus
	{ 
		MASK_SERVER_STATUS,		MASK_SERVER_IP,			MASK_VDISK_STATUS,	
		MASK_UPDATE_STATUS,		MASK_DOWNLOAD_STATUS,	MASK_CPU_USAGE,		
		MASK_MEMORY_USAGE,		MASK_DISK_STATUS
	};
	typedef std::map<MaskServerStatus, stdex::tString> MapServerStatus;
	static MapServerStatus ServerColName;

	struct InitServerStatus
	{
		InitServerStatus(MapServerStatus &serverColName)
		{
			serverColName[MASK_SERVER_STATUS]	= _T("主服务状态");
			ServerColName[MASK_SERVER_IP]		= _T("服务器IP");
			serverColName[MASK_VDISK_STATUS]	= _T("虚拟盘服务状态");
			serverColName[MASK_UPDATE_STATUS]	= _T("内网更新服务状态");
			serverColName[MASK_DOWNLOAD_STATUS] = _T("三层下载服务状态");
			serverColName[MASK_CPU_USAGE]		= _T("CPU占用率");
			serverColName[MASK_MEMORY_USAGE]	= _T("内存占用率");
			serverColName[MASK_DISK_STATUS]		= _T("硬盘状态");
		}
	};
	static const InitServerStatus initStatus(ServerColName);

	typedef std::map<int, std::vector<std::pair<stdex::tString, COLORREF>>> StatusInfo;
	StatusInfo statusInfo;
	static struct InitStatusInfo
	{
		int svrType;
		std::pair<stdex::tString, COLORREF> info;

		InitStatusInfo(StatusInfo &statusInfo)
		{
			std::vector<std::pair<stdex::tString, COLORREF>> mainSvr;
			mainSvr.push_back(std::make_pair(_T("未启用"), RGB(255, 255, 0)));
			mainSvr.push_back(std::make_pair(_T("异常"), RGB(255, 0, 0)));
			mainSvr.push_back(std::make_pair(_T("正常"), RGB(0, 0, 0)));

			std::vector<std::pair<stdex::tString, COLORREF>> otherSvr;
			otherSvr.push_back(std::make_pair(_T("无效"), RGB(0, 0, 0)));
			otherSvr.push_back(std::make_pair(_T("异常"), RGB(255, 0, 0)));
			otherSvr.push_back(std::make_pair(_T("正常"), RGB(0, 0, 0)));

			statusInfo.insert(std::make_pair(1, mainSvr));
			statusInfo.insert(std::make_pair(0, otherSvr));
		}

	}init(statusInfo);
}


namespace
{
	void SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &IsDelFile = false)
	{
		i8desk::AsyncDataHelper<bool>(std::tr1::bind(&i8desk::manager::ControlMgr::SyncTaskOperate, 
			std::tr1::ref(i8desk::GetControlMgr()), cmd, gid, SID, TaskName, IsDelFile));
	}
}



void CDlgServer::Register()
{

	i8desk::GetRealDataMgr().Register(
		std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_VDISK | MASK_PARSE_SERVER | MASK_PARSE_DISK);
}

void CDlgServer::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_VDISK | MASK_PARSE_SERVER | MASK_PARSE_DISK | MASK_PARSE_SYNCTASKSTATUS);
}

void CDlgServer::OnRealDataUpdate() 
{
	using namespace i8desk::data_helper;


	if( viewStatus_ == VDISK )
	{
		diskClients_.clear();
		i8desk::business::Server::VDiskFilter(diskClients_, curSvrID_, curVDiskID_);

		wndListServerVDisk_.SetItemCount(diskClients_.size());
	}
	else if( viewStatus_ == BASIC )
	{
		serverStatus_.clear();
		i8desk::business::Server::ServerFilter(serverStatus_, curSvrID_);

		if( serverStatus_.empty() )
			wndTreeServer_.SetItemImage(wndTreeServer_.GetSelectedItem(), 2, 2);
		else
		{
			i8desk::data_helper::ServerTraits::ValueType val = i8desk::business::Server::GetServiceBySID(serverStatus_[0]->SvrID);
			int isMain =  i8desk::business::Server::IsMainServer(val) ? 0 : 1;
			wndTreeServer_.SetItemImage(wndTreeServer_.GetSelectedItem(), isMain, isMain);
		}

		wndListServerBasic_.SetItemCount(serverStatus_.size() * ServerColName.size());
	}
	else
	{
		assert(0);
	}
}


void CDlgServer::OnReConnect()
{
	// 清空数据
	i8desk::GetDataMgr().GetServers().clear();
	i8desk::GetDataMgr().GetSyncTasks().clear();
	i8desk::GetDataMgr().GetSyncGames().clear();
	i8desk::GetDataMgr().GetVDisks().clear();
	i8desk::GetDataMgr().GetClients().clear();
}

void CDlgServer::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetServers());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetSyncTasks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetSyncGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClients());
}

void CDlgServer::OnDataComplate()
{
	_Init();
}


void CDlgServer::_Init()
{
	// 重置Tree Ctrl
	InitServerTreeCtrl(wndTreeServer_, i8desk::GetDataMgr().GetServers(), i8desk::GetDataMgr().GetVDisks());
}


void CDlgServer::_AddNewServer()
{
	i8desk::GetDataMgr().GetServers().clear();
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetServers());

	PostMessage(WM_ADD_NEW_SERVER_MSG);
}

// CDlgServer 消息处理程序

BOOL CDlgServer::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	i8desk::GetRecvDataMgr().RegisterNewServer(std::tr1::bind(&CDlgServer::_AddNewServer, this));

	// 初始化Tree
	treeServerImg_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);

	HBITMAP tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("Server/Ico_ServerMain.png"));
	treeServerImg_.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));

	tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("Server/Ico_ServerOther.png"));
	treeServerImg_.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));

	tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("Server/Ico_ServerOffline.png"));
	treeServerImg_.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));

	wndTreeServer_.SetImageList(&treeServerImg_, TVSIL_NORMAL);


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




	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndStaticServerMgr_.SetImages(leftArea[0], leftArea[0], leftArea[0]);

	wndEditQuery_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Input.png")));
	wndBtnQuery_.SetImages(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn_Press.png")));


	// 初始化List Ctrl
	InitListCtrl(wndListServerBasic_, wndListServerVDisk_);
	wndListServerVDisk_.ShowWindow(SW_HIDE);


	// 初始化控件管理器
	wndMgr_.Register(wndListServerBasic_.GetDlgCtrlID(), &wndListServerBasic_);
	wndMgr_.Register(wndListServerVDisk_.GetDlgCtrlID(), &wndListServerVDisk_);
	wndMgr_.SetDestWindow(&wndListServerBasic_);

	wndMgr_[wndListServerBasic_.GetDlgCtrlID()];


	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgServer::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )     
	{     
		if( pMsg->wParam == VK_RETURN )     
		{
			ONBnClickedButtonSearch();
			return TRUE;
		}

		// Ctrl按下 
		if( ::GetKeyState(VK_CONTROL) < 0 && ::toupper(pMsg->wParam) == 'A' )   
		{ 
			return FALSE;
		}
		else
		{
			if( ((pMsg->wParam >= '0' && pMsg->wParam <= '9') ||
				(pMsg->wParam >= 'A' && pMsg->wParam <= 'Z')) &&
				pMsg->hwnd != wndEditQuery_.GetSafeHwnd() )
			{
				wndEditQuery_.SendMessage(WM_CHAR, pMsg->wParam, 0);
			}
		}
	}

	return FALSE;
}


void CDlgServer::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	WTL::CClientRect rcClient(GetSafeHwnd());
	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, leftArea_, rightArea_);

	// 右边工作区内容边框
	CRect rcWork;
	wndListServerBasic_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);

}

BOOL CDlgServer::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CDlgServer::OnDestroy()
{
	CDialog::OnDestroy();

}

void CDlgServer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( wndTreeServer_.GetSafeHwnd() == 0 )
		return;

	{
		WTL::CWindowRect rcWindow(GetSafeHwnd());

		WTL::CWindowRect rcTreeServer(wndTreeServer_.GetSafeHwnd());

		WTL::CWindowRect rcServerMgr(wndStaticServerMgr_.GetSafeHwnd());

		const size_t height = (rcWindow.Height() - 5 * rcServerMgr.Height());
		rcTreeServer.top = rcWindow.top + 2 * rcServerMgr.Height() + 2 * 10;
		rcTreeServer.bottom = rcTreeServer.top +  height / 2;


		ScreenToClient(rcTreeServer);

		wndTreeServer_.MoveWindow(rcTreeServer);
	}

	UPDATE_EASYSIZE;

	wndMgr_.UpdateSize();

	wndTreeServer_.Invalidate();
	wndListServerBasic_.Invalidate();
	wndListServerVDisk_.Invalidate();
	wndBtnVirtualMgr_.Invalidate();
	wndBtnServerMgr_.Invalidate();


	wndStaticServerMgr_.Invalidate();

	wndEditQuery_.Invalidate();
	wndBtnQuery_.Invalidate();
}

void CDlgServer::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}


LRESULT CDlgServer::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 1 )
	{
		wndTreeServer_.SelectItem(wndTreeServer_.GetRootItem());
	}
	else if( wParam == 2 )
	{
	}	

	return TRUE;
}

LRESULT CDlgServer::OnAddVDisk(WPARAM wParam, LPARAM lParam)
{
	typedef i8desk::data_helper::VDiskTraits::ElementType VDiskType;
	typedef i8desk::data_helper::ServerTraits::ElementType ServerType;

	const VDiskType *vDisk = reinterpret_cast<VDiskType *>(wParam);


	for(HTREEITEM hItem = wndTreeServer_.GetRootItem(); 
		hItem != NULL; hItem = wndTreeServer_.GetNextSiblingItem(hItem)) 
	{	
		ServerType *svr = reinterpret_cast<ServerType *>(wndTreeServer_.GetItemData(hItem));
		if( utility::Strcmp(svr->SvrID, vDisk->SvrID) == 0 )
		{
			i8desk::data_helper::VDiskTraits::ValueType val(new VDiskType);
			*val = *vDisk;
			HTREEITEM addItem = wndTreeServer_.InsertItem(i8desk::business::VDisk::GetVDiskName(val).c_str(), -1, -1, hItem);
			wndTreeServer_.SetItemData(addItem, reinterpret_cast<DWORD_PTR>(vDisk));
		}
	}

	wndTreeServer_.Expand(wndTreeServer_.GetRootItem(), TVE_EXPAND);
	wndTreeServer_.Invalidate(TRUE);

	return TRUE;
}

LRESULT CDlgServer::OnDelVDisk(WPARAM wParam, LPARAM lParam)
{
	typedef i8desk::data_helper::VDiskTraits::ElementType VDiskType;
	const VDiskType *vDisk = reinterpret_cast<VDiskType *>(wParam);

	typedef i8desk::data_helper::ServerTraits::ElementType ServerType;

	for(HTREEITEM hItem = wndTreeServer_.GetRootItem(); 
		hItem != NULL; hItem = wndTreeServer_.GetNextSiblingItem(hItem)) 
	{	
		ServerType *svr = reinterpret_cast<ServerType *>(wndTreeServer_.GetItemData(hItem));
		if( utility::Strcmp(svr->SvrID, vDisk->SvrID) == 0 )
		{
			for(HTREEITEM childItem = wndTreeServer_.GetChildItem(hItem); childItem != NULL; childItem = wndTreeServer_.GetNextSiblingItem(childItem))
			{
				const VDiskType *dstVDisk = reinterpret_cast<VDiskType *>(wndTreeServer_.GetItemData(childItem));
				if( utility::Strcmp(dstVDisk->VID, vDisk->VID) == 0 )
				{
					wndTreeServer_.DeleteItem(childItem);
					return TRUE;
				}
			}
		}
	}

	return TRUE;
}

LRESULT CDlgServer::OnModVDisk(WPARAM wParam, LPARAM lParam)
{
	typedef i8desk::data_helper::VDiskTraits::ElementType VDiskType;
	const VDiskType *vDisk = reinterpret_cast<VDiskType *>(wParam);

	HTREEITEM hTree = wndTreeServer_.GetSelectedItem();
	wndTreeServer_.SetItemData(hTree, (DWORD_PTR)vDisk);

	i8desk::data_helper::VDiskTraits::ValueType val(new VDiskType);
	*val = *vDisk;
	wndTreeServer_.SetItemText(hTree, i8desk::business::VDisk::GetVDiskName(val).c_str());


	return TRUE;
}


LRESULT CDlgServer::OnAddServer(WPARAM wParam, LPARAM lParam)
{
	i8desk::GetDataMgr().GetServers().clear();
	i8desk::GetDataMgr().GetVDisks().clear();

	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetServers());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());

	InitServerTreeCtrl(wndTreeServer_, i8desk::GetDataMgr().GetServers(), i8desk::GetDataMgr().GetVDisks());
	return TRUE;
}

void CDlgServer::OnLvnGetdispinfoListServerBasic(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	using i8desk::data_helper::ServerStatusTraits;
	if( serverStatus_.empty() )
		return;

	const ServerStatusTraits::ValueType &val = serverStatus_[0];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case 1:
			if( val->SvrType == i8desk::OtherServer && itemIndex == 0 )
				text = _T("从服务状态");
			else
				text = ServerColName[static_cast<MaskServerStatus>(itemIndex)];
			break;
		case 2:
			switch(itemIndex)
			{
			case MASK_SERVER_STATUS:
				if( val->SvrType == i8desk::MainServer)
				{
					text = statusInfo[val->SvrType][val->I8DeskSvr + 1].first;
					svrInfo->SetTextColor(MASK_SERVER_STATUS, statusInfo[val->SvrType][val->I8DeskSvr + 1].second);
				}
				else
				{
					text = statusInfo[val->SvrType][val->I8VDiskSvr + 1].first;
					svrInfo->SetTextColor(MASK_SERVER_STATUS, statusInfo[val->SvrType][val->I8VDiskSvr + 1].second);
				}
				break;

			case MASK_SERVER_IP:
				i8desk::business::Server::FormatIP(val->SvrID, text);
				break;

			case MASK_VDISK_STATUS:
				text = statusInfo[val->SvrType][val->I8VDiskSvr + 1].first;
				svrInfo->SetTextColor(MASK_VDISK_STATUS, statusInfo[val->SvrType][val->I8VDiskSvr + 1].second);
				break;
			case MASK_UPDATE_STATUS:
				text = statusInfo[val->SvrType][val->I8UpdateSvr + 1].first;
				svrInfo->SetTextColor(MASK_UPDATE_STATUS, statusInfo[val->SvrType][val->I8UpdateSvr + 1].second);
				break;
			case MASK_DOWNLOAD_STATUS:
				if( val->SvrType == i8desk::MainServer )
				{
					text = statusInfo[val->SvrType][val->DNAService + 1].first;
					svrInfo->SetTextColor(MASK_DOWNLOAD_STATUS, statusInfo[val->SvrType][val->DNAService + 1].second);
				}
				else
				{
					text = statusInfo[val->SvrType][0].first;
					svrInfo->SetTextColor(MASK_DOWNLOAD_STATUS, statusInfo[val->SvrType][0].second);
				}

				break;
			case MASK_CPU_USAGE:
				stdex::ToString(val->CPUUtilization, text);
				svrInfo->SetTextColor(MASK_CPU_USAGE, val->CPUUtilization > 50 ? RGB(255, 0, 0) : RGB(0, 0, 0));
				text += _T("%");
				break;
			case MASK_MEMORY_USAGE:
				stdex::ToString(val->MemoryUsage, text);
				svrInfo->SetTextColor(MASK_MEMORY_USAGE, val->MemoryUsage > 50 ? RGB(255, 0, 0) : RGB(0, 0, 0));
				text += _T("%");
				break;
			case MASK_DISK_STATUS:
				i8desk::business::Server::FormatDisk(val->SvrID, text);
				break;
			}
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgServer::OnLvnGetdispinfoListServerVDisk(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	using i8desk::data_helper::VDiskClientTraits;
	assert(itemIndex < (int)diskClients_.size());
	const VDiskClientTraits::ValueType &val = diskClients_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_NAME:
			text = i8desk::business::Client::GetNameByIP(val->ClientIP);
			break;
		case MASK_IP:
			text = i8desk::IP2String(val->ClientIP);
			break;
		case MASK_READ_BYTES:
			i8desk::FormatSize(val->ReadCount, text);
			break;
		case MASK_READ_RATE:
			i8desk::FormatSize(val->ReadSpeed, text);
			break;
		case MASK_READ_UP:
			i8desk::FormatSize(val->ReadMax, text);
			break;
		case MASK_CONNECT_TIME:
			{
				text = i8desk::TotalTime(val->ConnectTime);
				/*i8desk::ulong time = val->ConnectTime / 1000;
				CString szText;
				szText.Format(_T("%02d:%02d:%02d"), time / 3600, (time % 3600) / 60, time % 60);
				szText;*/
			}
			break;
		case MASK_CONNECT_CATCHSHOT:
			stdex::ToString(val->CacheShooting, text);
			break;
		default:
			//assert(0);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

	if( pItem->mask & LVIF_IMAGE && pItem->iSubItem == 0 )
	{
		//pItem->iImage = 0;
	}
}



void CDlgServer::OnTvnSelchangedTreeServerServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == 0 )
		return;

	HTREEITEM hSelItem = wndTreeServer_.GetSelectedItem();
	HTREEITEM hParentItem = wndTreeServer_.GetParentItem(hSelItem);

	if( hSelItem == 0 && hParentItem == 0 )
		return;

	bool isSelSvr = hParentItem == NULL;

	typedef i8desk::data_helper::ServerTraits::ElementType ServerType;
	ServerType *val = (ServerType *)wndTreeServer_.GetItemData(isSelSvr ? hSelItem : hParentItem);
	assert(val);
	curSvrID_ = val->SvrID;
	viewStatus_ = isSelSvr ? BASIC : VDISK;

	if( viewStatus_ == BASIC )
	{
		i8desk::GetRealDataMgr().Register(
			std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_SERVER | MASK_PARSE_DISK);

		i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_VDISK);
		wndMgr_[wndListServerBasic_.GetDlgCtrlID()];
	}
	else
	{
		i8desk::GetRealDataMgr().Register(
			std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_VDISK);

		i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_SERVER | MASK_PARSE_DISK);
		wndMgr_[wndListServerVDisk_.GetDlgCtrlID()];

		typedef i8desk::data_helper::VDiskTraits::ElementType VDiskType;
		VDiskType *val = (VDiskType *)wndTreeServer_.GetItemData(hSelItem);
		assert(val);
		curVDiskID_ = val->VID;
	}

	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_SYNCTASKSTATUS);
}




void CDlgServer::OnBnClickedButtonServerVirtualMgr()
{
	CDlgVDiskMgr dlg(this);
	dlg.DoModal();

	InitServerTreeCtrl(wndTreeServer_, i8desk::GetDataMgr().GetServers(), i8desk::GetDataMgr().GetVDisks());
}


void CDlgServer::OnBnClickedButtonServerServerMgr()
{
	CDlgServerInfo dlg(this);
	dlg.DoModal();

	InitServerTreeCtrl(wndTreeServer_, i8desk::GetDataMgr().GetServers(), i8desk::GetDataMgr().GetVDisks());
}


void CDlgServer::ONBnClickedButtonSearch()
{
	CString text;
	wndEditQuery_.GetWindowText(text);

	using namespace i8desk::data_helper;
	GameTraits::VectorType out;
}

