// DlgDownLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownLoad.h"

#include <bitset>


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/TaskInfo.h"
#include "../../../../include/MultiThread/Lock.hpp"
#include "../../../../include/ui/ImageHelpers.h"


#include "../Business/GameBusiness.h"
#include "../Business/ClassBusiness.h"
#include "../Business/UpdateBusiness.h"
#include "../Business/SyncTaskBusiness.h"


#include "../Misc.h"
#include "../MessageBox.h"
#include "../AsyncDataHelper.h"

#include "DlgDownloadClass.h"
#include "DlgDownloadCenterGame.h"
#include "DlgDownloadGameInfo.h"
#include "DlgDownloadDelGame.h"
#include "DlgDownloadSomeMod.h"
#include "DlgDownloadSomeAdd.h"
#include "DlgDownloadMakeIdx.h"
#include "DlgDownloadGameViewClient.h"
#include "DlgGameVersion.h"
#include "DlgDownloadSmartClean.h"
#include "../DlgViewRunState.h"

#include "UIHelper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



enum { WM_REFRESHPL = WM_USER + 0x222, WM_CUMMULATE };

enum { DT_TIMER = 1000 };

namespace
{
	struct GameCenter
	{
		CDlgDownLoad::CenterMask mask_;
		stdex::tString text_;
	}gameCenter[] =
	{
		{ CDlgDownLoad::CENTER_SYSTEM,		_T("系统公告区(0)") },
		{ CDlgDownLoad::CENTER_ACTIVE,		_T("增值活动区(0)")},
		{ CDlgDownLoad::CENTER_NO_DOWNLOAD,	_T("未下载游戏(0)") },
		{ CDlgDownLoad::CENTER_ADD_RECENT,	_T("新增游戏(0)")},
		{ CDlgDownLoad::CENTER_DELETE,		_T("已删除游戏(0)")},
		{ CDlgDownLoad::CENTER_UPDATE,		_T("有更新游戏(0)") }
	};


	struct MonitorCenter
	{
		CDlgDownLoad::MonitorMask mask_;
		stdex::tString text_;
	}monitorCenter[] =
	{
		{ CDlgDownLoad::MONITOR_DOWNLOAD,	_T("三层下载") },
		{ CDlgDownLoad::MONITOR_UPDATE,		_T("内网更新") }
	};

	static LPCTSTR CenterText	= _T("中心资源");
	static LPCTSTR LocalText	= _T("本地资源");
	static LPCTSTR MonitorText	= _T("实时监控");

	template < typename TreeCtrlT >
	void InitTreeCtrlCenter(TreeCtrlT &tree)
	{
		tree.DeleteAllItems();

		HTREEITEM hRoot = tree.InsertItem(CenterText, 0, 0);
		for(size_t i = 1; i != _countof(gameCenter); ++i)
		{
			HTREEITEM hItem = tree.InsertItem(gameCenter[i].text_.c_str(), i + 1, i + 1, hRoot);
			tree.SetItemData(hItem, gameCenter[i].mask_);
		}

		tree.SetItemHeight(24);
		tree.Expand(hRoot, TVE_EXPAND);
	}

	template < typename TreeCtrlT >
	void InitTreeCtrlLocal(TreeCtrlT &tree)
	{
		tree.DeleteAllItems();

		using i8desk::data_helper::ClassTraits;
		using std::tr1::placeholders::_1;


		ClassTraits::VectorType vecClasses;
		i8desk::business::Class::GetAllSortedClasses(vecClasses);

		HTREEITEM hRoot = tree.InsertItem(LocalText, 0, 0);

		for(size_t i = 0; i != vecClasses.size(); ++i)
		{
			stdex::tOstringstream os;
			os << vecClasses[i]->Name << _T("(0)");

			HTREEITEM hItem = tree.InsertItem(os.str().c_str(), vecClasses[i]->ImgIndex, vecClasses[i]->ImgIndex, hRoot);
			tree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(vecClasses[i].get()));
		}

		tree.SetItemHeight(24);
		tree.Expand(hRoot, TVE_EXPAND);
	}

	template < typename TreeCtrlT >
	void InitTreeCtrlMonitor(TreeCtrlT &tree)
	{
		tree.DeleteAllItems();
		HTREEITEM hRoot = tree.InsertItem(MonitorText, 0, 0);
		for(size_t i = 0; i != _countof(monitorCenter); ++i)
		{
			HTREEITEM hItem = tree.InsertItem(monitorCenter[i].text_.c_str(), i + 1, i + 1, hRoot);
			tree.SetItemData(hItem, monitorCenter[i].mask_);
		}

		tree.SetItemHeight(24);
		tree.Expand(hRoot, TVE_EXPAND);
	}

	CCustomColumnTraitGameIcon *centerIcon			= 0;
	CCustomColumnTraitGameIcon *localIcon			= 0;
	CCustomColumnTraitGameIcon *downloadIcon		= 0;

	CCustomColumnTrait *downloadStatus				= 0;
	CCustomProgressColumnTrait *downloadProgress	= 0;
	CCustomProgressColumnTrait *updateProgress		= 0;


	namespace detail
	{
		template < typename ListT, typename GamesT >
		CCustomColumnTraitGameIcon *GetCenterIconTrait(ListT &list, GamesT &games)
		{
			centerIcon = new CCustomColumnTraitGameIcon(list, games);
			centerIcon->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));
	
			return centerIcon;
		}

		template < typename ListT, typename GamesT >
		CCustomColumnTraitGameIcon *GetLocalIconTrait(ListT &list, GamesT &games)
		{
			localIcon = new CCustomColumnTraitGameIcon(list, games);
			localIcon->SetBkImg(
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Table_LineFirst_Bg.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TablePerLine.png")),
				i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/TableRowSel.png")));

			return localIcon;
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
		MASK_CENTER_NAME = 1, MASK_CENTER_GID, MASK_CENTER_STATUS, MASK_CENTER_CLASS,
		MASK_CENTER_TIME, MASK_CENTER_SIZE, MASK_CENTER_EXE, MASK_CENTER_HOT, MASK_CENTER_REMARK, 
		MASK_CENTER_SOURCE 
	};

	template < typename ListCtrlT, typename GamesT >
	void InitCenterList(ListCtrlT &listCtrl, GamesT &games)
	{
		listCtrl.InsertHiddenLabelColumn();

		CCustomColumnTraitGameIcon *col = detail::GetCenterIconTrait(listCtrl, games);

		col->SetDefaultBmp(i8desk::ui::SkinMgrInstance().GetIcon(0));;

		listCtrl.InsertColumnTrait(MASK_CENTER_NAME,	TEXT("名称"),		LVCFMT_LEFT, 100,	MASK_CENTER_NAME,	col);
		listCtrl.InsertColumnTrait(MASK_CENTER_GID,		TEXT("GID"),		LVCFMT_LEFT, 80,	MASK_CENTER_GID,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_STATUS,	TEXT("当前状态"),		LVCFMT_LEFT, 90,	MASK_CENTER_STATUS, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_CLASS,	TEXT("类别"),		LVCFMT_LEFT, 70,	MASK_CENTER_CLASS,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_TIME,	TEXT("中心更新时间"),	LVCFMT_LEFT, 135,	MASK_CENTER_TIME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_SIZE,	TEXT("大小(M)"),		LVCFMT_LEFT, 70,	MASK_CENTER_SIZE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_EXE,		TEXT("执行程序"),		LVCFMT_LEFT, 120,	MASK_CENTER_EXE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_HOT,		TEXT("热门度"),		LVCFMT_LEFT, 80,	MASK_CENTER_HOT,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_REMARK,	TEXT("备注"),		LVCFMT_LEFT, 100,	MASK_CENTER_REMARK,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_CENTER_SOURCE,	TEXT("游戏来源"),		LVCFMT_LEFT, 100,	MASK_CENTER_SOURCE,	detail::GetTrait());

		listCtrl.SetItemHeight(24);
	}


	enum 
	{
		MASK_LOCAL_NAME = 1, MASK_LOCAL_GID, MASK_LOCAL_STATUS, MASK_LOCAL_UPDATE, MASK_LOCAL_TYPE, 
		MASK_LOCAL_CLICK, MASK_LOCAL_TIME, MASK_LOCAL_SIZE, MASK_LOCAL_EXE, MASK_LOCAL_PATH, 
		MASK_LOCAL_CLASS, MASK_LOCAL_SOURCE
	};

	template < typename ListCtrlT, typename GamesT >
	void InitLocalList(ListCtrlT &listCtrl, GamesT &games)
	{
		listCtrl.InsertHiddenLabelColumn();

		CCustomColumnTraitGameIcon *col = detail::GetLocalIconTrait(listCtrl, games);

		col->SetDefaultBmp(i8desk::ui::SkinMgrInstance().GetIcon(0));;


		listCtrl.InsertColumnTrait(MASK_LOCAL_NAME,		TEXT("名称"),		LVCFMT_LEFT, 120,	MASK_LOCAL_NAME,	col);
		listCtrl.InsertColumnTrait(MASK_LOCAL_GID,		TEXT("GID"),		LVCFMT_LEFT, 80,	MASK_LOCAL_GID,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_STATUS,	TEXT("当前状态"),		LVCFMT_LEFT, 80,	MASK_LOCAL_STATUS,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_UPDATE,	TEXT("更新方式"),		LVCFMT_LEFT, 80,	MASK_LOCAL_UPDATE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_TYPE,		TEXT("运行方式"),		LVCFMT_LEFT, 120,	MASK_LOCAL_TYPE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_CLICK,	TEXT("点击数"),		LVCFMT_RIGHT,60,	MASK_LOCAL_CLICK,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_TIME,		TEXT("本地更新时间"),	LVCFMT_LEFT, 130,	MASK_LOCAL_TIME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_SIZE,		TEXT("大小(M)"),		LVCFMT_LEFT, 70,	MASK_LOCAL_SIZE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_EXE,		TEXT("执行程序"),		LVCFMT_LEFT, 120,	MASK_LOCAL_EXE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_PATH,		TEXT("服务端路径"),	LVCFMT_LEFT, 200,	MASK_LOCAL_PATH,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_CLASS,	TEXT("类别"),		LVCFMT_LEFT, 70,	MASK_LOCAL_CLASS,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_SOURCE,	TEXT("游戏来源"),		LVCFMT_LEFT, 100,	MASK_LOCAL_SOURCE,	detail::GetTrait());
	
		listCtrl.SetItemHeight(24);
	}


	

	enum 
	{ 
		MASK_DOWNLOAD_GID = 1, MASK_DOWNLOAD_NAME, MASK_DOWNLOAD_STATUS, MASK_DOWNLOAD_PROGRESS, 
		MASK_DOWNLOAD_UPDATESIZE, MASK_DOWNLOAD_LEFTSIZE, MASK_DOWNLOAD_SPPED, MASK_DOWNLOAD_CONNECTS, 
		MASK_DOWNLOAD_USETIME, MASK_DOWNLOAD_LEFTTIME 
	};
	template < typename ListCtrlT >
	void InitDownloadList(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_GID,		TEXT("GID"),		LVCFMT_LEFT, 80,	MASK_DOWNLOAD_GID,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_NAME,		TEXT("游戏名称"),		LVCFMT_LEFT, 90,	MASK_DOWNLOAD_NAME,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_STATUS,	TEXT("当前状态"),		LVCFMT_LEFT, 80,	MASK_DOWNLOAD_STATUS,	downloadStatus = detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_PROGRESS,	TEXT("当前进度"),		LVCFMT_LEFT, 110,	MASK_DOWNLOAD_PROGRESS, downloadProgress = detail::GetProgressTrait(listCtrl));
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_UPDATESIZE,TEXT("更新量"),		LVCFMT_LEFT, 80,	MASK_DOWNLOAD_UPDATESIZE, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_LEFTSIZE,	TEXT("剩余"),		LVCFMT_LEFT, 70,	MASK_DOWNLOAD_LEFTSIZE, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_SPPED,		TEXT("速度"),		LVCFMT_LEFT, 70,	MASK_DOWNLOAD_SPPED,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_CONNECTS,	TEXT("连接数"),		LVCFMT_LEFT, 60,	MASK_DOWNLOAD_CONNECTS, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_USETIME,	TEXT("己用时间"),		LVCFMT_LEFT, 90,	MASK_DOWNLOAD_USETIME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_DOWNLOAD_LEFTTIME,	TEXT("剩余时间"),		LVCFMT_LEFT, 90,	MASK_DOWNLOAD_LEFTTIME, detail::GetTrait());
	
		listCtrl.SetItemHeight(24);
	}


	enum 
	{ 
		MASK_UPDATE_CLIENTNAME = 1, MASK_UPDATE_CLIENTIP, MASK_UPDATE_GAMENAME, MASK_UPDATE_GID, 
		MASK_UPDATE_STATE, MASK_UPDATE_PROGRESS, MASK_UPDATE_UPDATESIZE, MASK_UPDATE_SIZELEFT, 
		MASK_UPDATE_SPEED
	};
	template < typename ListCtrlT >
	void InitUpdateList(ListCtrlT &listCtrl)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_UPDATE_CLIENTNAME,		TEXT("机器名称"),		LVCFMT_LEFT, 80,	MASK_UPDATE_CLIENTNAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_CLIENTIP,		TEXT("机器 IP "),	LVCFMT_LEFT, 80,	MASK_UPDATE_CLIENTIP,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_GAMENAME,		TEXT("游戏名称"),		LVCFMT_LEFT, 80,	MASK_UPDATE_GAMENAME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_GID,				TEXT("游戏 ID "),	LVCFMT_LEFT, 70,	MASK_UPDATE_GID,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_STATE,			TEXT("当前状态"),		LVCFMT_LEFT, 80,	MASK_UPDATE_STATE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_PROGRESS,		TEXT("当前进度"),		LVCFMT_LEFT, 120,	MASK_UPDATE_PROGRESS,	updateProgress = detail::GetProgressTrait(listCtrl));
		listCtrl.InsertColumnTrait(MASK_UPDATE_UPDATESIZE,		TEXT("更新量"),		LVCFMT_LEFT, 80,	MASK_UPDATE_UPDATESIZE, detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_SIZELEFT,		TEXT("剩余量"),		LVCFMT_LEFT, 90,	MASK_UPDATE_SIZELEFT,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_UPDATE_SPEED,			TEXT("速度"),		LVCFMT_LEFT, 100,	MASK_UPDATE_SPEED,		detail::GetTrait());
	
		listCtrl.SetItemHeight(24);
	}
	


	// 内网更新实时数据更新
	using i8desk::data_helper::UpdateGameStatusTraits;

	static struct
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

		UpdateGameStatusTraits::VectorType updateGames_;
		mutable Mutex mutex_;


		size_t Size() const
		{
			AutoLock lock(mutex_);
			return updateGames_.size();
		}

		void Update(const UpdateGameStatusTraits::MapType &games)
		{
			AutoLock lock(mutex_);

			updateGames_.clear();
			using namespace std::tr1::placeholders;

			struct Func
			{
				Func(const int type, UpdateGameStatusTraits::VectorType &updateGames)
					: updateGames_(updateGames)
					, Type_(type)
				{}
				void operator ()(const UpdateGameStatusTraits::MapType::value_type &val ) const
				{
					if( Type_ == val.second->Type )
						updateGames_.push_back(val.second);
				}
				const int Type_;
				UpdateGameStatusTraits::VectorType &updateGames_;
			};
			//type = 2 为内网更新
			std::for_each(games.begin(), games.end(), Func( 2, updateGames_));

		}

		UpdateGameStatusTraits::ValueType At(size_t index) const
		{
			AutoLock lock(mutex_);
			assert(index < updateGames_.size());

			if( updateGames_.empty() || index >= updateGames_.size() )
				return UpdateGameStatusTraits::ValueType(new UpdateGameStatusTraits::ElementType);

			return updateGames_[index];
		}
	}UpdateGames;

	// 三层下载实时数据更新
	using i8desk::data_helper::TaskStatusTraits;

	static struct
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

		TaskStatusTraits::VectorType taskGames_;
		mutable Mutex mutex_;


		size_t Size() const
		{
			AutoLock lock(mutex_);
			return taskGames_.size();
		}

		template < typename GamesT >
		void Update(const GamesT &games)
		{
			AutoLock lock(mutex_);

			taskGames_.clear();
			taskGames_ = games;
			/*using namespace std::tr1::placeholders;
			std::for_each(games.begin(), games.end(),
				bind(&TaskStatusTraits::VectorType::push_back, std::tr1::ref(taskGames_),
				bind(&TaskStatusTraits::MapType::value_type::second, _1)));*/
		}

		TaskStatusTraits::ValueType At(size_t index) const
		{
			AutoLock lock(mutex_);
			assert(index < taskGames_.size());

			if( taskGames_.empty() || index >= taskGames_.size() )
				return TaskStatusTraits::ValueType(new TaskStatusTraits::ElementType);

			return taskGames_[index];
		}
	}TaskGames;
	

	// 三层下载中游戏操作
	template < typename ListCtrlT >
	void StatusTaskOperate(ListCtrlT &list, i8desk::TaskOperation op)
	{
		int nSel = -1;

		while( (nSel = list.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
		{
			const long gid = TaskGames.At(nSel)->GID;
			i8desk::business::Game::TaskOperate(gid, op);
		}
	}

	// 本地游戏操作
	template < typename ListCtrlT, typename GamesT >
	void LocalTaskOperate(ListCtrlT &list, const GamesT &games, i8desk::TaskOperation op, bool bIsFore)
	{
		int nSel = -1;

		while( (nSel = list.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
		{		
			assert(nSel < (int)games.size());
			i8desk::business::Game::LocalTaskOperate(games[nSel], op, bIsFore);
		}
	}


	template <typename ListCtrlT>
	void DeleteAllColumns(ListCtrlT &list)
	{
		for(size_t i = list.GetHeaderCtrl()->GetItemCount(); i > 0 ; --i)
		{
			list.DeleteColumn(0);
		}
	}


	// 图标显示结构
	typedef std::map<unsigned long, int> IconMap;
	IconMap iconMap;


	// 类别过滤最大数
	const UINT ClassFilerMax = 50;


	// 按类别过滤
	stdex::tString FilterCID;

	// 游戏类别过滤, 默认显示所有
	UINT ClassFilterCheck = ID_DOWNLOAD_CENTER_CLASS;

	// 是否切换页面
	bool HasChange = false;


	void ResetClassFilter()
	{ 
		ClassFilterCheck = ID_DOWNLOAD_CENTER_CLASS; 
		HasChange = false;
	}


	// 三层排序
	std::bitset<MASK_CENTER_SOURCE + 1> CenterCompare;
	std::bitset<MASK_LOCAL_SOURCE + 1>	LocalCompare;
	
	template <size_t Status>
	struct Compare
	{
		Compare(int, bool)
		{}
		bool operator()()
		{ return true; }
	};

	template<>
	struct Compare<CDlgDownLoad::Center>
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
			case MASK_CENTER_NAME:
				return ascSort_ ? utility::Strcmp(lhs->Name, rhs->Name) > 0 : utility::Strcmp(lhs->Name, rhs->Name) < 0;
			case MASK_CENTER_GID:
				return ascSort_ ? lhs->GID > rhs->GID : lhs->GID < rhs->GID;
			case MASK_CENTER_STATUS:
				return ascSort_ ? utility::Strcmp(i8desk::business::Game::GetStatusDesc(lhs), i8desk::business::Game::GetStatusDesc(rhs)) > 0 :
					utility::Strcmp(i8desk::business::Game::GetStatusDesc(lhs), i8desk::business::Game::GetStatusDesc(rhs)) < 0;
			case MASK_CENTER_CLASS:
				return ascSort_ ? utility::Strcmp(lhs->CID, rhs->CID) > 0 : utility::Strcmp(lhs->CID, rhs->CID) < 0;
			case MASK_CENTER_TIME:
				return ascSort_ ? lhs->IdcVer > rhs->IdcVer : lhs->IdcVer < rhs->IdcVer;
			case MASK_CENTER_SIZE:
				return ascSort_ ? lhs->Size > rhs->Size : lhs->Size < rhs->Size;
			case MASK_CENTER_EXE:
				return ascSort_ ? utility::Strcmp(lhs->Exe, rhs->Exe) > 0 : utility::Strcmp(lhs->Exe, rhs->Exe) < 0;
			case MASK_CENTER_HOT:
				return ascSort_ ? lhs->IdcClick > rhs->IdcClick : lhs->IdcClick < rhs->IdcClick;
			case MASK_CENTER_REMARK:
				return ascSort_ ? utility::Strcmp(lhs->Memo, rhs->Memo) > 0 : utility::Strcmp(lhs->Memo, rhs->Memo) < 0;
			case MASK_CENTER_SOURCE:
				return ascSort_ ? utility::Strcmp(lhs->GameSource, rhs->GameSource) > 0 : utility::Strcmp(lhs->GameSource, rhs->GameSource) < 0;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

	template<>
	struct Compare<CDlgDownLoad::Local>
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
			case MASK_LOCAL_NAME:
				return ascSort_ ? utility::Strcmp(lhs->Name, rhs->Name) > 0 : utility::Strcmp(lhs->Name, rhs->Name) < 0;
			case MASK_LOCAL_GID:
				return ascSort_ ? lhs->GID > rhs->GID : lhs->GID < rhs->GID;
			case MASK_LOCAL_STATUS:
				return ascSort_ ? utility::Strcmp(i8desk::business::Game::GetStatusDesc(lhs), i8desk::business::Game::GetStatusDesc(rhs)) > 0 :
					utility::Strcmp(i8desk::business::Game::GetStatusDesc(lhs), i8desk::business::Game::GetStatusDesc(rhs)) < 0;
			case MASK_LOCAL_UPDATE:
				return ascSort_ ? lhs->AutoUpt > rhs->AutoUpt : lhs->AutoUpt < rhs->AutoUpt;
			case MASK_LOCAL_TYPE:
				return ascSort_ ? i8desk::business::Game::GetRunTypeDesc(lhs) > i8desk::business::Game::GetRunTypeDesc(rhs) :
					i8desk::business::Game::GetRunTypeDesc(lhs) < i8desk::business::Game::GetRunTypeDesc(rhs);
			case MASK_LOCAL_CLICK:
				return ascSort_ ? lhs->SvrClick > rhs->SvrClick : lhs->SvrClick < rhs->SvrClick;
			case MASK_LOCAL_TIME:
				return ascSort_ ? lhs->SvrVer > rhs->SvrVer : lhs->SvrVer < rhs->SvrVer;
			case MASK_LOCAL_SIZE:
				return ascSort_ ? lhs->Size > rhs->Size : lhs->Size < rhs->Size;
			case MASK_LOCAL_EXE:
				return ascSort_ ? utility::Strcmp(lhs->Exe, rhs->Exe) > 0 : utility::Strcmp(lhs->Exe, rhs->Exe) < 0;
			case MASK_LOCAL_PATH:
				return ascSort_ ? utility::Strcmp(lhs->SvrPath, rhs->SvrPath) > 0 : utility::Strcmp(lhs->SvrPath, rhs->SvrPath) < 0;
			case MASK_LOCAL_CLASS:
				return ascSort_ ? utility::Strcmp(lhs->CID, rhs->CID) > 0 : utility::Strcmp(lhs->CID, rhs->CID) < 0;
			case MASK_LOCAL_SOURCE:
				return ascSort_ ? utility::Strcmp(lhs->GameSource, rhs->GameSource) > 0 : utility::Strcmp(lhs->GameSource, rhs->GameSource) < 0;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

}


// CDlgDownLoad

IMPLEMENT_DYNCREATE(CDlgDownLoad, CDialog)

CDlgDownLoad::CDlgDownLoad(CWnd* pParent)
	: i8desk::ui::BaseWnd(CDlgDownLoad::IDD, pParent)
{

}

CDlgDownLoad::~CDlgDownLoad()
{
}

void CDlgDownLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DOWNLOAD_CENTER, wndTreeCenter_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_CENTER, wndListCenter_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_DOWNLOAD, wndListDownload_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_UPDATE, wndListUpdate_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_LOCAL, wndListLocal_);
	DDX_Control(pDX, IDC_TREE_DOWNLOAD_LOCAL, wndTreeLocal_);
	DDX_Control(pDX, IDC_TREE_DOWNLOAD_MONITOR, wndTreeMonitor_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_START, wndBtnTaskStart_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_PAUSE, wndBtnTaskPause_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_STOP, wndBtnTaskStop_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_TOADD, wndBtnTaskAdd_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_FILTER, wndBtnClassFilter_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_ADD_GAME, wndBtnAddGame_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_CENTER_FORCE_UPDATE, wndBtnForceUpdate_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_ADD_SOME, wndBtnAddSomeGames_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_SMART_CLEAN, wndBtnSmartClean_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_MODIFY_GAME, wndBtnModifyGame_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_ERASE_GAME, wndBtnEraseGame_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_LOCAL_ADD, wndBtnClassAdd_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_LOCAL_DELETE, wndBtnClassDel_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_LOCAL_MODIFY, wndBtnClassMod_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_RES_MGR, wndBtnResMgr_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_LOCAL_MGR, wndBtnLocalMgr_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_DATA_MGR, wndBtnDataMgr_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_TOP, wndBtnDownloadTop_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_BOTTOM, wndBtnDownloadBottom_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_SHOWLOG, wndBtnShowLog_);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_SEARCH, wndEditQuery_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_SEARCH, wndBtnQuery_);
}

BEGIN_EASYSIZE_MAP(CDlgDownLoad)   
	EASYSIZE(IDC_TREE_DOWNLOAD_CENTER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_TREE_DOWNLOAD_LOCAL, ES_BORDER, IDC_TREE_DOWNLOAD_CENTER, ES_KEEPSIZE, ES_BORDER, 0)
	EASYSIZE(IDC_TREE_DOWNLOAD_MONITOR, ES_BORDER, IDC_TREE_DOWNLOAD_LOCAL, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_LIST_DOWNLOAD_CENTER, IDC_TREE_DOWNLOAD_CENTER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	
	EASYSIZE(IDC_BUTTON_DOWNLOAD_LOCAL_MGR, ES_BORDER, IDC_TREE_DOWNLOAD_CENTER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_DATA_MGR, ES_BORDER, IDC_TREE_DOWNLOAD_LOCAL, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_DOWNLOAD_LOCAL_ADD, ES_BORDER, IDC_BUTTON_DOWNLOAD_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_LOCAL_DELETE, IDC_BUTTON_DOWNLOAD_LOCAL_ADD, IDC_BUTTON_DOWNLOAD_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_LOCAL_MODIFY, IDC_BUTTON_DOWNLOAD_LOCAL_DELETE, IDC_BUTTON_DOWNLOAD_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_EDIT_DOWNLOAD_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_DOWNLOAD_START, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_PAUSE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_STOP, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_DOWNLOAD_TOP, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_DOWNLOAD_BOTTOM, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

END_EASYSIZE_MAP 

BEGIN_MESSAGE_MAP(CDlgDownLoad, i8desk::ui::BaseWnd)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()

	ON_WM_TIMER()

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DOWNLOAD_CENTER, &CDlgDownLoad::OnTvnItemChangedTreeDownloadCenter)	
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DOWNLOAD_LOCAL, &CDlgDownLoad::OnTvnItemChangedTreeDownloadLocal)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DOWNLOAD_MONITOR, &CDlgDownLoad::OnTvnItemChangedTreeDownloadMonitor)
	
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnLvnGetdispinfoListDownloadCenter)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnLvnGetdispinfoListDownloadLocal)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_DOWNLOAD, &CDlgDownLoad::OnLvnGetdispinfoListDownloadDownload)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_UPDATE, &CDlgDownLoad::OnLvnGetdispinfoListDownloadUpdate)

	ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnLvnOdcachehintListDownloadCenter)
	ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnLvnOdcachehintListDownloadLocal)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnNMRClickListDownloadCenter)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnNMRClickListDownloadLocal)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnNMDblclkListDownloadLocal)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DOWNLOAD_DOWNLOAD, &CDlgDownLoad::OnNMRClickListDownloadDownload)

	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnLvnColumnclickListDownloadCenter)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnLvnColumnclickListDownloadLocal)

    ON_NOTIFY(NM_CLICK, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DOWNLOAD_CENTER, &CDlgDownLoad::OnLvnKeydownList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DOWNLOAD_LOCAL, &CDlgDownLoad::OnLvnKeydownList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_DOWNLOAD_DOWNLOAD, &CDlgDownLoad::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DOWNLOAD_DOWNLOAD, &CDlgDownLoad::OnLvnKeydownList)
    ON_NOTIFY(NM_CLICK, IDC_LIST_DOWNLOAD_UPDATE, &CDlgDownLoad::OnNMClickList)
    ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_DOWNLOAD_UPDATE, &CDlgDownLoad::OnLvnKeydownList)

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_START, &CDlgDownLoad::OnBnClickedButtonDownloadStart)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_PAUSE, &CDlgDownLoad::OnBnClickedButtonDwonloadPause)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_STOP, &CDlgDownLoad::OnBnClickedButtonDownloadStop)

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_LOCAL_ADD, &CDlgDownLoad::OnBnClickedButtonDownloadLocalAdd)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_LOCAL_DELETE, &CDlgDownLoad::OnBnClickedButtonDownloadLocalDelete)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_LOCAL_MODIFY, &CDlgDownLoad::OnBnClickedButtonDownloadLocalModify)

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_FILTER, &CDlgDownLoad::OnBnClickedButtonDownloadFilter)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_TOADD, &CDlgDownLoad::OnBnClickedButtonDownloadToadd)

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_ADD_GAME, &CDlgDownLoad::OnBnClickedButtonDownloadAddGame)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_ADD_SOME, &CDlgDownLoad::OnBnClickedButtonDownloadAddSomeGame)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_CENTER_FORCE_UPDATE, &CDlgDownLoad::OnBnClickedButtonDownloadForceupdate)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_MODIFY_GAME, &CDlgDownLoad::OnBnClickedButtonDownloadModifyGame)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_ERASE_GAME, &CDlgDownLoad::OnBnClickedButtonDownloadEraseGame)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_SMART_CLEAN, &CDlgDownLoad::OnBnClickedButtonDownloadSmartClean)

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_TOP, &CDlgDownLoad::OnBnClickedButtonDownloadTop)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_BOTTOM, &CDlgDownLoad::OnBnClickedButtonDownloadBottom)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_SHOWLOG, &CDlgDownLoad::OnBnClickedButtonShowLog)
	

	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_SEARCH, &CDlgDownLoad::ONBnClickedButtonSearch)
	ON_EN_CHANGE(IDC_EDIT_DOWNLOAD_SEARCH, &CDlgDownLoad::OnEnChangeEditDownloadSearch)

	ON_COMMAND_RANGE(ID_DOWNLOAD_CENTER_DOWN, ID_DOWNLOAD_CENTER_REFRESHPL, &CDlgDownLoad::OnMenuCenter)
	ON_COMMAND_RANGE(ID_DOWNLOAD_LOCAL_ADD, ID_DOWNLOAD_LOCAL_VIEW, &CDlgDownLoad::OnMenuLocal)
	ON_COMMAND_RANGE(ID_DOWNLOAD_CENTER_CLASS, ID_DOWNLOAD_CENTER_CLASS + ClassFilerMax, &CDlgDownLoad::OnMenuClassFilter)
	ON_COMMAND_RANGE(ID_TASKOPERATE_STOP, ID_TASKOPERATE_REFRESH, &CDlgDownLoad::OnMenuTask)

	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgDownLoad::OnApplySelect)
	ON_MESSAGE(WM_REFRESHPL, &CDlgDownLoad::OnRefreshPL)
	ON_MESSAGE(WM_CUMMULATE, &CDlgDownLoad::OnCummulate)
    ON_MESSAGE(i8desk::ui::WM_REPORT_SELECT_STATE, &CDlgDownLoad::OnReportSelectState)
END_MESSAGE_MAP()




void CDlgDownLoad::Register()
{
	SetTimer(DT_TIMER, 2000, 0);

	switch(viewStatus_)
	{
	case Download:
		i8desk::GetRealDataMgr().Register(std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_TASKSTATUS);
		break;
	case Update:
		i8desk::GetRealDataMgr().Register(std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_UPDATEGAMESTATUS);
		break;
	default:
		break;
	}
}

void CDlgDownLoad::UnRegister()
{
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_TASKSTATUS);
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_UPDATEGAMESTATUS);

	KillTimer(DT_TIMER);
}

void CDlgDownLoad::OnRealDataUpdate() 
{
	
	HTREEITEM item = wndTreeMonitor_.GetSelectedItem();
	if( item == NULL )
		return;

	switch( wndTreeMonitor_.GetItemData(item) )
	{
	case MONITOR_DOWNLOAD:
		{ 
			static size_t downCnt = 0;
			size_t cnt = 0;
			TaskGames.Update(i8desk::GetRealDataMgr().GetTaskStatus());

			cnt = TaskGames.Size();
			if( downCnt != cnt )
			{
				downCnt = cnt;
				wndListDownload_.SetItemCount(downCnt);
			}
			else
			{
				wndListDownload_.RedrawCurPageItems();
			}
		}
		
		break;
	case MONITOR_UPDATE:
		{
			static size_t count = 0;
			size_t cnt = 0;

			UpdateGames.Update(i8desk::GetRealDataMgr().GetUpdateGameStatus());

			cnt = UpdateGames.Size();
			if( count != cnt )
			{
				count = cnt;
				wndListUpdate_.SetItemCount(count);
			}
			else
				wndListUpdate_.RedrawCurPageItems();
		}
		
		break;
	default:
		assert(0);
		break;
	}

	
}


void CDlgDownLoad::OnReConnect()
{
	localTreeImageList_.DeleteImageList();
	menuClassFilter_.DestroyMenu();

	i8desk::GetDataMgr().GetClasses().clear();
	i8desk::GetDataMgr().GetGames().clear();
	i8desk::GetDataMgr().GetRunTypes().clear();
	i8desk::GetDataMgr().GetVDisks().clear();
	i8desk::GetDataMgr().GetAreas().clear();
}

void CDlgDownLoad::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClasses());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetRunTypes());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetAreas());
}

void CDlgDownLoad::OnDataComplate()
{
	localTreeImageList_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	localTreeImageList_.Add(CBitmap::FromHandle(i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/svrresmgr.png"))), RGB(0, 0, 0));

	using i8desk::data_helper::ClassTraits;
	const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

	for(ClassTraits::MapConstIterator iter = classes.begin(); iter != classes.end(); ++iter)
	{
		HICON icon = Bitmap2Icon(i8desk::business::Class::GetIcon(iter->second));
		int index = localTreeImageList_.Add(icon);
		if( index != -1 )
			iter->second->ImgIndex = index;
		else
			iter->second->ImgIndex = 0;
	}
	wndTreeLocal_.SetImageList(&localTreeImageList_, TVSIL_NORMAL);

	// Menu
	menuClassFilter_.LoadMenu(IDR_MENU_CLASS_FILTER);


	InitTreeCtrlCenter(wndTreeCenter_);
	InitTreeCtrlLocal(wndTreeLocal_);
	InitTreeCtrlMonitor(wndTreeMonitor_);


	wndMgr_.Register(wndListCenter_.GetDlgCtrlID(), &wndListCenter_);
	wndMgr_.Register(wndListLocal_.GetDlgCtrlID(), &wndListLocal_);
	wndMgr_.Register(wndListDownload_.GetDlgCtrlID(), &wndListDownload_);
	wndMgr_.Register(wndListUpdate_.GetDlgCtrlID(), &wndListUpdate_);

	wndMgr_.SetDestWindow(&wndListCenter_);
	wndMgr_[wndListCenter_.GetDlgCtrlID()];

	wndTreeCenter_.SelectItem(wndTreeCenter_.GetRootItem());
}


void CDlgDownLoad::_HandleTask(i8desk::TaskNotify notify, long gid, DWORD err)
{
	if( notify == i8desk::TASK_COMPLATE )
	{
		using i8desk::data_helper::GameTraits;
		GameTraits::ValueType val(new GameTraits::ElementType);
		val->GID = gid;

		i8desk::GetDataMgr().GetData(val);

		val->RuntypeDesc = i8desk::GetDataMgr().GetGames()[gid]->RuntypeDesc;
		i8desk::GetDataMgr().GetGames()[gid] = val;
		//_ShowView();
	}
	else if( notify == i8desk::TASK_DOWNLOADFAILED )
	{

	}
}

void CDlgDownLoad::_HandleRefreshPL()
{
	PostMessage(WM_REFRESHPL);
}


void CDlgDownLoad::_Cummulate()
{
	i8desk::business::Game::CummulateCenterGame(centerGameSize_);
	i8desk::business::Game::CummulateLocalGame(localGameCnt_);

	PostMessage(WM_CUMMULATE);
}

void CDlgDownLoad::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::GameTraits;
	using i8desk::data_helper::ClassTraits;


	switch(viewStatus_)
	{
	case Center:
		{
			const GameTraits::MapType &centerGames = i8desk::GetDataMgr().GetGames();
			
			HTREEITEM centerhItem = wndTreeCenter_.GetSelectedItem();
			wndTreeCenter_.SelectItem(centerhItem);

			HTREEITEM hRoot = wndTreeCenter_.GetParentItem(centerhItem);
			
			CenterMask mask = CENTER_DEFAULT;
			if( hRoot != 0 )
				mask = static_cast<CenterMask>(wndTreeCenter_.GetItemData(centerhItem));

			curGames_.clear();

			size_t count = 0;
			switch(mask)
			{
			case CENTER_SYSTEM:
				i8desk::business::Game::CenterFilter(centerGames, curGames_);
				break;
			case CENTER_ACTIVE:
				i8desk::business::Game::CenterI8PlayFilter(centerGames, curGames_);
				break;
			case CENTER_NO_DOWNLOAD:
				i8desk::business::Game::NoDownloadFilter(centerGames, curGames_);
				break;
			case CENTER_ADD_RECENT:
				i8desk::business::Game::RecentFilter(centerGames, curGames_);
				break;
			case CENTER_DELETE:
				i8desk::business::Game::DeleteFilter(curGames_);
				break;
			case CENTER_UPDATE:
				i8desk::business::Game::UpdateFilter(centerGames, curGames_);
				break;
			default:
				i8desk::business::Game::CenterFilter(centerGames, curGames_);
				//stdex::map2vector(centerGames, curGames_);
				break;
			}

			if( !FilterCID.empty() )
			{
				GameTraits::VectorType tmp(curGames_.size());
				tmp.swap(curGames_);
				curGames_.clear();
				i8desk::business::Game::ClassFilter(tmp, curGames_, FilterCID);
				FilterCID.clear();
			}

			// 如果切换页面则重置
			if( HasChange )
				ResetClassFilter();

			wndListCenter_.SetItemCount(curGames_.size());

		}
		break;
	case Local:
		{
			HTREEITEM localhItem = wndTreeLocal_.GetSelectedItem();
			wndTreeLocal_.SelectItem(localhItem);
			HTREEITEM hRoot = wndTreeLocal_.GetParentItem(localhItem);

			const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
			curGames_.clear();
			if( hRoot != 0 )
			{
				stdex::tString CID = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocal_.GetItemData(localhItem))->CID;
				i8desk::business::Game::LocalFilter(games, curGames_, CID);
			}
			else
			{
				i8desk::business::Game::LocalAllFilter(games, curGames_);
			}

			wndListLocal_.SetItemCount(curGames_.size());
		}

		ResetClassFilter();
		break;
	case Download:
		{
			ResetClassFilter();
		}
		break;
	case Update:
		{
			ResetClassFilter();
		}
		break;
	default:
		assert(0);
		break;
	}
}

void CDlgDownLoad::_ShowTree()
{
	


}



CListCtrl &CDlgDownLoad::_GetCurList()
{
	switch(viewStatus_)
	{
	case Center:
		return wndListCenter_;
		break;
	case Local:
		return wndListLocal_;
		break;
	case Download:
		return wndListDownload_;
		break;
	case Update:
		return wndListUpdate_;
		break;
	default:
		assert(0);
		return wndListCenter_;
		break;
	}
}

void CDlgDownLoad::_ShowButton()
{
	switch(viewStatus_)
	{
	case Center:
		{
			HTREEITEM hItem = wndTreeCenter_.GetSelectedItem();
			if( hItem != 0 )
			{
				CenterMask mask = static_cast<CenterMask>(wndTreeCenter_.GetItemData(hItem));
				if( mask == CENTER_UPDATE )
					wndBtnForceUpdate_.ShowWindow(SW_SHOW);
				else
					wndBtnForceUpdate_.ShowWindow(SW_HIDE);
			}

			wndBtnTaskStart_.ShowWindow(SW_HIDE);
			wndBtnTaskPause_.ShowWindow(SW_HIDE);
			wndBtnTaskStop_.ShowWindow(SW_HIDE);

			wndBtnAddGame_.ShowWindow(SW_HIDE);
			wndBtnAddSomeGames_.ShowWindow(SW_HIDE);
			wndBtnSmartClean_.ShowWindow(SW_HIDE);
			wndBtnModifyGame_.ShowWindow(SW_HIDE);
			wndBtnEraseGame_.ShowWindow(SW_HIDE);

			wndBtnTaskAdd_.ShowWindow(SW_SHOW);
			wndBtnClassFilter_.ShowWindow(SW_SHOW);

			wndListCenter_.ShowWindow(SW_SHOW);
			wndListLocal_.ShowWindow(SW_HIDE);
			wndListDownload_.ShowWindow(SW_HIDE);
			wndListUpdate_.ShowWindow(SW_HIDE);

			wndBtnDownloadTop_.ShowWindow(SW_HIDE);
			wndBtnDownloadBottom_.ShowWindow(SW_HIDE);

			wndBtnShowLog_.ShowWindow(SW_HIDE);

			wndBtnQuery_.ShowWindow(SW_SHOW);
			wndEditQuery_.ShowWindow(SW_SHOW);
		}
		break;

	case Local:
		wndBtnTaskStart_.ShowWindow(SW_HIDE);
		wndBtnTaskPause_.ShowWindow(SW_HIDE);
		wndBtnTaskStop_.ShowWindow(SW_HIDE);

		wndBtnAddGame_.ShowWindow(SW_SHOW);
		wndBtnAddSomeGames_.ShowWindow(SW_SHOW);
		wndBtnSmartClean_.ShowWindow(SW_SHOW);
		wndBtnModifyGame_.ShowWindow(SW_SHOW);
		wndBtnEraseGame_.ShowWindow(SW_SHOW);

		wndBtnTaskAdd_.ShowWindow(SW_HIDE);
		wndBtnForceUpdate_.ShowWindow(SW_HIDE);
		wndBtnClassFilter_.ShowWindow(SW_HIDE);

		wndListCenter_.ShowWindow(SW_HIDE);
		wndListLocal_.ShowWindow(SW_SHOW);
		wndListDownload_.ShowWindow(SW_HIDE);
		wndListUpdate_.ShowWindow(SW_HIDE);

		wndBtnDownloadTop_.ShowWindow(SW_HIDE);
		wndBtnDownloadBottom_.ShowWindow(SW_HIDE);


		wndBtnQuery_.ShowWindow(SW_SHOW);
		wndEditQuery_.ShowWindow(SW_SHOW);
		break;
	case Download:
		wndBtnTaskStart_.ShowWindow(SW_SHOW);
		wndBtnTaskPause_.ShowWindow(SW_SHOW);
		wndBtnTaskStop_.ShowWindow(SW_SHOW);

		wndBtnAddGame_.ShowWindow(SW_HIDE);
		wndBtnAddSomeGames_.ShowWindow(SW_HIDE);
		wndBtnSmartClean_.ShowWindow(SW_HIDE);
		wndBtnModifyGame_.ShowWindow(SW_HIDE);
		wndBtnEraseGame_.ShowWindow(SW_HIDE);

		wndBtnTaskAdd_.ShowWindow(SW_HIDE);
		wndBtnForceUpdate_.ShowWindow(SW_HIDE);
		wndBtnClassFilter_.ShowWindow(SW_HIDE);

		wndListCenter_.ShowWindow(SW_HIDE);
		wndListLocal_.ShowWindow(SW_HIDE);
		wndListDownload_.ShowWindow(SW_SHOW);
		wndListUpdate_.ShowWindow(SW_HIDE);

		wndBtnDownloadTop_.ShowWindow(SW_SHOW);
		wndBtnDownloadBottom_.ShowWindow(SW_SHOW);
		wndBtnShowLog_.ShowWindow(SW_SHOW);

		wndBtnQuery_.ShowWindow(SW_SHOW);
		wndEditQuery_.ShowWindow(SW_SHOW);

		break;
	case Update:
		wndBtnTaskStart_.ShowWindow(SW_HIDE);
		wndBtnTaskPause_.ShowWindow(SW_HIDE);
		wndBtnTaskStop_.ShowWindow(SW_HIDE);

		wndBtnAddGame_.ShowWindow(SW_HIDE);
		wndBtnAddSomeGames_.ShowWindow(SW_HIDE);
		wndBtnSmartClean_.ShowWindow(SW_HIDE);
		wndBtnModifyGame_.ShowWindow(SW_HIDE);
		wndBtnEraseGame_.ShowWindow(SW_HIDE);

		wndBtnTaskAdd_.ShowWindow(SW_HIDE);
		wndBtnForceUpdate_.ShowWindow(SW_HIDE);
		wndBtnClassFilter_.ShowWindow(SW_HIDE);

		wndListCenter_.ShowWindow(SW_HIDE);
		wndListLocal_.ShowWindow(SW_HIDE);
		wndListDownload_.ShowWindow(SW_HIDE);
		wndListUpdate_.ShowWindow(SW_SHOW);

		wndBtnDownloadTop_.ShowWindow(SW_HIDE);
		wndBtnDownloadBottom_.ShowWindow(SW_HIDE);

		wndBtnShowLog_.ShowWindow(SW_HIDE);

		wndBtnQuery_.ShowWindow(SW_HIDE);
		wndEditQuery_.ShowWindow(SW_HIDE);
		break;
	default:
		assert(0);
		break;
	}

}


BOOL CDlgDownLoad::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	// 初始化Tree Image List
	centerTreeImageList_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/idcres.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/systemnotice.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/valueres.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/notdownloadres.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/stoprungames.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/needuptres.png"))), RGB(0, 0, 0));
	centerTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/svrresmgr.png"))), RGB(0, 0, 0));
	wndTreeCenter_.SetImageList(&centerTreeImageList_, TVSIL_NORMAL);


	monitorTreeImageList_.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
	monitorTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/monitorcenter.png"))), RGB(0, 0, 0));
	monitorTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/3upstatus.png"))), RGB(0, 0, 0));
	monitorTreeImageList_.Add(CBitmap::FromHandle(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/inupdatestatus.png"))), RGB(0, 0, 0));

	wndTreeMonitor_.SetImageList(&monitorTreeImageList_, TVSIL_NORMAL);
	
	// 初始化List Ctrl
	InitCenterList(wndListCenter_, curGames_);
	InitLocalList(wndListLocal_, curGames_);
	InitDownloadList(wndListDownload_);
	InitUpdateList(wndListUpdate_);

	wndListDownload_.SetItemHeight(24);
	wndListUpdate_.SetItemHeight(24);

	wndListCenter_.ShowWindow(SW_SHOW);
	wndListLocal_.ShowWindow(SW_HIDE);
	wndListDownload_.ShowWindow(SW_HIDE);
	wndListUpdate_.ShowWindow(SW_HIDE);


	wndEditQuery_.SetBkImg(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Input.png")));
	wndBtnQuery_.SetImages(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Query_Btn_Press.png")));


	HBITMAP add[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Add_Press.png"))
	};
	wndBtnClassAdd_.SetImages(add[0], add[1], add[2]);

	HBITMAP del[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Press.png"))
	};
	wndBtnClassDel_.SetImages(del[0], del[1], del[2]);

	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndBtnClassMod_.SetImages(modify[0], modify[1], modify[2]);


	wndBtnResMgr_.SetImages(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")));
	wndBtnLocalMgr_.SetImages(

		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")));
	wndBtnDataMgr_.SetImages(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")));

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

	wndEditQuery_.SetFocus();
	wndEditQuery_.SetSel(0, 0);

	wndTreeLocal_.SelectItem(wndTreeLocal_.GetRootItem());

	using namespace std::tr1::placeholders;
	i8desk::GetRecvDataMgr().RegisterTaskStatus(std::tr1::bind(
		&CDlgDownLoad::_HandleTask, this, _1, _2, _3));
	i8desk::GetRecvDataMgr().RegisterRefreshPL(std::tr1::bind(
		&CDlgDownLoad::_HandleRefreshPL, this));

	centerGameSize_[CENTER_ACTIVE];
	centerGameSize_[CENTER_NO_DOWNLOAD];
	centerGameSize_[CENTER_ADD_RECENT];
	centerGameSize_[CENTER_DELETE];
	centerGameSize_[CENTER_UPDATE];
	centerGameSize_[CENTER_DEFAULT];

	return TRUE;  // return TRUE unless you set the focus to a control
}


BOOL CDlgDownLoad::PreTranslateMessage(MSG* pMsg)
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
			_GetCurList().SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
            				PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);			return FALSE;
		}
		else
		{
			if( viewStatus_ == Download )
				return FALSE;

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

// CDlgDownLoad 消息处理程序

void CDlgDownLoad::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, leftArea_, rightArea_);

	// 右边工作区内容边框
	CRect rcWork;
	wndListCenter_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);
}

BOOL CDlgDownLoad::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

void CDlgDownLoad::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	wndMgr_.UpdateSize();
	if( !::IsWindow(wndListCenter_.GetSafeHwnd()) )
		return;

	wndTreeCenter_.Invalidate();
	wndTreeLocal_.Invalidate();
	wndTreeMonitor_.Invalidate();
	wndListCenter_.Invalidate();
	wndListLocal_.Invalidate();
	wndListUpdate_.Invalidate();
	wndListDownload_.Invalidate();
	wndBtnQuery_.Invalidate();
	wndEditQuery_.Invalidate();

	wndBtnTaskStart_.Invalidate();
	wndBtnTaskPause_.Invalidate();
	wndBtnTaskStop_.Invalidate();
	wndBtnTaskAdd_.Invalidate();
	wndBtnClassFilter_.Invalidate();

	wndBtnAddGame_.Invalidate();
	wndBtnAddSomeGames_.Invalidate();
	wndBtnSmartClean_.Invalidate();
	wndBtnModifyGame_.Invalidate();
	wndBtnEraseGame_.Invalidate();

	wndBtnClassAdd_.Invalidate();
	wndBtnClassDel_.Invalidate();
	wndBtnClassMod_.Invalidate();

	wndBtnResMgr_.Invalidate();
	wndBtnLocalMgr_.Invalidate();
	wndBtnDataMgr_.Invalidate();

	wndBtnDownloadTop_.Invalidate();
	wndBtnDownloadBottom_.Invalidate();
	wndBtnShowLog_.Invalidate();
}

void CDlgDownLoad::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

}

void CDlgDownLoad::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CDlgDownLoad::OnTimer(UINT_PTR id)
{
	i8desk::AsyncDataHelper(std::tr1::bind(&CDlgDownLoad::_Cummulate, this));
}

void CDlgDownLoad::OnDestroy()
{
	CDialog::OnDestroy();
}


LRESULT CDlgDownLoad::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 1 )
		wndTreeCenter_.SelectItem(wndTreeCenter_.GetRootItem());
	else if( wParam == 2 )
		wndTreeLocal_.SelectItem(wndTreeLocal_.GetRootItem());
	else if( wParam == 3 )
		wndTreeMonitor_.SelectItem(wndTreeMonitor_.GetRootItem());
	else if( wParam == 4 )
	{
		HTREEITEM item = wndTreeMonitor_.GetNextSiblingItem(
			wndTreeMonitor_.GetChildItem(wndTreeMonitor_.GetRootItem()));
		wndTreeMonitor_.SelectItem(item);
	}
	else
	{
		// LOG
	}

	return TRUE;
}


LRESULT CDlgDownLoad::OnRefreshPL(WPARAM, LPARAM)
{
	i8desk::data_helper::GameTraits::MapType tmpGames;
	i8desk::GetDataMgr().GetAllData(tmpGames);

	i8desk::GetDataMgr().GetGames().swap(tmpGames);

	
	return TRUE;
}

LRESULT CDlgDownLoad::OnCummulate(WPARAM, LPARAM)
{
	static std::map<int, size_t> centerGameSizeTmp;
	static std::map<stdex::tString, size_t> localGameCntTmp;

	stdex::tOstringstream os;
	for(HTREEITEM centerTree = wndTreeCenter_.GetChildItem(wndTreeCenter_.GetRootItem());
		centerTree != NULL; centerTree = wndTreeCenter_.GetNextSiblingItem(centerTree))
	{
		int mask = wndTreeCenter_.GetItemData(centerTree);
		
		if( centerGameSizeTmp[mask] != centerGameSize_[mask] )
		{
			os.str(_T(""));
			stdex::tString text = gameCenter[mask].text_;
			os << text.substr(0, text.find(_T('('))) << _T("(") << centerGameSize_[mask] << _T(")");
			wndTreeCenter_.SetItemText(centerTree, os.str().c_str());

			centerGameSizeTmp[mask] = centerGameSize_[mask];
		}

		centerGameSize_[mask] = 0;
	}

	for(HTREEITEM localTree = wndTreeLocal_.GetChildItem(wndTreeLocal_.GetRootItem());
		localTree != NULL; localTree = wndTreeLocal_.GetNextSiblingItem(localTree))
	{
		
		os.str(_T(""));
		stdex::tString text = wndTreeLocal_.GetItemText(localTree);
		text = text.substr(0, text.find(_T('(')));

		stdex::tString CID = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocal_.GetItemData(localTree))->CID;

		if( localGameCntTmp[CID] != localGameCnt_[CID] )
		{
			os << text << _T("(") << localGameCnt_[CID] << _T(")");
			wndTreeLocal_.SetItemText(localTree, os.str().c_str());

			localGameCntTmp[CID] = localGameCnt_[CID];
		}

		localGameCnt_[CID] = 0;
	}

	return TRUE;
}

void CDlgDownLoad::OnTvnItemChangedTreeDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if( pNMTreeView->itemNew.hItem == 0 )
		return;

// 	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE*>(pNMHDR);
// 	*pResult = 0;
	HTREEITEM hSelItem = wndTreeCenter_.GetSelectedItem();
	if( hSelItem == 0 )
		return;

	viewStatus_ = Center;
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);	centerMask_ = (CenterMask)wndTreeCenter_.GetItemData(hSelItem);
	
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_TASKSTATUS);
	wndMgr_[wndListCenter_.GetDlgCtrlID()];
	
	HasChange = true;

	wndTreeLocal_.SelectItem(0);
	wndTreeMonitor_.SelectItem(0);

	_ShowView();
	_ShowButton();


}


void CDlgDownLoad::OnTvnItemChangedTreeDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == 0 )
		return;

	viewStatus_ = Local;
    PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_TASKSTATUS);
	wndMgr_[wndListLocal_.GetDlgCtrlID()];

	wndTreeCenter_.SelectItem(0);
	wndTreeMonitor_.SelectItem(0);

	_ShowView();
	_ShowButton();
}

void CDlgDownLoad::OnTvnItemChangedTreeDownloadMonitor(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == 0 )
		return;

	HTREEITEM monitorhItem = wndTreeMonitor_.GetSelectedItem();
	if( monitorhItem == 0 )
		return;

	wndTreeCenter_.SelectItem(0);
	wndTreeLocal_.SelectItem(0);

	DWORD mask = wndTreeMonitor_.GetItemData(monitorhItem);
	switch ( mask )
	{
	case MONITOR_DOWNLOAD:
		{
			i8desk::GetRealDataMgr().Register(
				std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_TASKSTATUS );
			i8desk::GetRealDataMgr().UnRegister( MASK_PARSE_UPDATEGAMESTATUS );
			viewStatus_ = Download;
            PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);			wndMgr_[wndListDownload_.GetDlgCtrlID()];
		}
		break;
	case MONITOR_UPDATE:
		{
			i8desk::GetRealDataMgr().Register(
				std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_UPDATEGAMESTATUS );
			i8desk::GetRealDataMgr().UnRegister( MASK_PARSE_TASKSTATUS );
			viewStatus_ = Update;
            PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);			wndMgr_[wndListUpdate_.GetDlgCtrlID()];
		}
		break;
	default:
		assert(0);
		break;
	}

	_ShowView();
	_ShowButton();
}



void CDlgDownLoad::OnLvnGetdispinfoListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curGames_.size() )
		return;

	const i8desk::data_helper::GameTraits::ValueType &val = curGames_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_CENTER_NAME:
			text = val->Name;
			break;
		case MASK_CENTER_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_CENTER_STATUS:
			text = i8desk::business::Game::GetStatusDesc(val);
			break;
		case MASK_CENTER_CLASS:
			text = i8desk::business::Class::GetName(val->CID);
			break;
		case MASK_CENTER_TIME:
			text = i8desk::FormatTime(val->IdcVer);
			break;
		case MASK_CENTER_SIZE:
			stdex::ToString(val->Size * 1.0 / 1024, text, 2);
			break;
		case MASK_CENTER_EXE:
			text = val->Exe;
			break;
		case MASK_CENTER_HOT:
			stdex::ToString(val->IdcClick, text);
			break;
		case MASK_CENTER_REMARK:
			text = val->Memo;
			break;
		case MASK_CENTER_SOURCE:
			text = val->GameSource;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}

	if( pItem->mask & LVIF_IMAGE && pItem->iSubItem == 0 )
	{
		// 默认
		
	}
}


void CDlgDownLoad::OnLvnGetdispinfoListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= curGames_.size() )
		return;

	const i8desk::data_helper::GameTraits::ValueType &val = curGames_[itemIndex];
	
	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_LOCAL_NAME:
			text = val->Name;
			break;
		case MASK_LOCAL_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_LOCAL_STATUS:
			text = i8desk::business::Game::GetStatusDesc(val);
			break;
		case MASK_LOCAL_UPDATE:
			text = val->AutoUpt ? _T("自动") : _T("手动");
			break;
		case MASK_LOCAL_TYPE:
			text = i8desk::business::Game::GetRunTypeDesc(val);
			break;
		case MASK_LOCAL_CLICK:
			stdex::ToString(val->SvrClick, text);
			break;
		case MASK_LOCAL_TIME:
			text = i8desk::FormatTime(val->SvrVer);
			break;
		case MASK_LOCAL_SIZE:
			stdex::ToString(val->Size * 1.0 / 1024 , text, 2);
			break;
		case MASK_LOCAL_EXE:
			text = val->Exe;
			break;
		case MASK_LOCAL_PATH:
			text = val->SvrPath;
			break;
		case MASK_LOCAL_CLASS:
			text = i8desk::business::Class::GetName(val->CID);
			break;
		case MASK_LOCAL_SOURCE:
			text = val->GameSource;
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
	
	if( pItem->mask & LVIF_IMAGE && pItem->iSubItem == 0 )
	{

	}


}

void CDlgDownLoad::OnLvnGetdispinfoListDownloadDownload(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	
	if( TaskGames.Size() == 0 )
		return;

	TaskStatusTraits::ValueType &val = TaskGames.At(itemIndex);

	const i8desk::data_helper::GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_DOWNLOAD_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_DOWNLOAD_NAME:
			{
				i8desk::data_helper::GameTraits::MapConstIterator iter = games.find(val->GID);
				if( iter != games.end() )
					text = iter->second->Name;
				else
					text = _T("未知游戏名称");
			}
			break;
		case MASK_DOWNLOAD_STATUS:
			{
				static COLORREF Red		= RGB(255, 0, 0);
				static COLORREF Green	= RGB(0, 255, 0);
				static COLORREF Blue	= RGB(0, 0, 255);
				static COLORREF Black	= RGB(0, 0, 0);
				
				switch(val->State) 
				{
				case i8desk::tsWaitSeed:	
					text = _T("等待索引");
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsSelfQueue:	
					text = _T("正在排队");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsHashmap:		
					text = _T("校验索引");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsCopyfile:	
					text = _T("复制文件");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsChecking:	
					text = _T("正在对比");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsQueue:		
					text = _T("等待下载");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsDownloading: 
					text = _T("正在下载");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Blue);  
					break;
				case i8desk::tsSuspend:		
					text = _T("暂停下载");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Red);   
					break;
				case i8desk::tsDeleteOldFile: 
					text = _T("清理文件");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				case i8desk::tsComplete:	
					text = _T("下载完成");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Green); 
					break;
				case i8desk::tsCommit:	
					text = _T("正在合并");	
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Red); 
					break;
				default:					
					text = _T("");			
					downloadStatus->SetTextColor(itemIndex, MASK_DOWNLOAD_STATUS, Black); 
					break;
				}

				i8desk::data_helper::GameTraits::MapConstIterator iter = games.find(val->GID);
				if( iter == games.end() )
					break;
				
				i8desk::business::Game::SetStatusDesc(iter->second, text);
			}
			break;
		case MASK_DOWNLOAD_PROGRESS:
			if( val->State == i8desk::tsDownloading 
				|| val->State == i8desk::tsQueue  
				|| val->State == i8desk::tsSuspend 
				|| val->State == i8desk::tsHashmap
				|| val->State == i8desk::tsCommit)
				downloadProgress->SetProgress(itemIndex, val->Progress);
			else
				downloadProgress->SetProgress(itemIndex, 0);

			stdex::ToString(val->Progress, text, 2);
			text += _T("%");
			break;
		case MASK_DOWNLOAD_UPDATESIZE:
			if( val->State == i8desk::tsDownloading 
				|| val->State == i8desk::tsQueue 
				|| val->State == i8desk::tsSuspend )
			{
				i8desk::FormatSize(val->UpdateSize, text);
			}
			else
				text = _T("--");
				
			break;
		case MASK_DOWNLOAD_LEFTSIZE:
			if( val->State == i8desk::tsDownloading 
				|| val->State == i8desk::tsQueue 
				|| val->State == i8desk::tsSuspend )
			{
				i8desk::FormatSize(val->BytesTotal - val->BytesTransferred , text);
			}
			else
				text = _T("--");
			break;
		case MASK_DOWNLOAD_SPPED:
			if( val->State == i8desk::tsDownloading )
				i8desk::FormatSize(val->TransferRate, text);
			else
				text = _T("--");
			break;
		case MASK_DOWNLOAD_CONNECTS:
			if( val->State == i8desk::tsDownloading )
				stdex::ToString(val->Connect, text, 2);
			else
				text = _T("--");
			break;
		case MASK_DOWNLOAD_USETIME:
			if( val->State == i8desk::tsDownloading )
			{
				text = i8desk::TotalTime(val->TimeElapsed);
			}
			else
				text = _T("--:--:--");
			break;
		case MASK_DOWNLOAD_LEFTTIME:
			if( val->State == i8desk::tsDownloading )
			{
				text = i8desk::TotalTime(val->TimeLeft);
			}
			else
				text = _T("--:--:--");
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}


void CDlgDownLoad::OnLvnGetdispinfoListDownloadUpdate(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	UpdateGameStatusTraits::ValueType &val = UpdateGames.At(itemIndex);

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_UPDATE_CLIENTNAME:
			text = val->ClientName;
			break;
		case MASK_UPDATE_CLIENTIP:
			text = i8desk::IP2String(val->ClientIP);
			break;
		case MASK_UPDATE_GAMENAME:
			text = val->GameName;
			break;
		case MASK_UPDATE_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_UPDATE_STATE:
			text = i8desk::business::update::GetState(val);
			break;
		case MASK_UPDATE_PROGRESS:
			{
				long progress = static_cast<long>((val->UpdateSize - val->SizeLeft) * 100 / val->UpdateSize);
				updateProgress->SetProgress(itemIndex, progress);
				
				text.clear();
			}
			break;
		case MASK_UPDATE_UPDATESIZE:
			i8desk::FormatSize(val->UpdateSize , text);
			break;
		case MASK_UPDATE_SIZELEFT:
			i8desk::FormatSize(val->SizeLeft, text);
			break;
		case MASK_UPDATE_SPEED:
			i8desk::FormatSize(val->TransferRate , text);
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}


void CDlgDownLoad::OnLvnOdcachehintListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	*pResult = 0;

	size_t start	= pCacheHint->iFrom;
	size_t stop		= pCacheHint->iTo;

	for(size_t i = start; i <= stop; ++i)
		curGames_[i]->Icon = i8desk::business::Game::GetIcon(curGames_[i]);
}

void CDlgDownLoad::OnLvnOdcachehintListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	*pResult = 0;

	size_t start	= pCacheHint->iFrom;
	size_t stop		= pCacheHint->iTo;

	for(size_t i = start; i <= stop; ++i)
		curGames_[i]->Icon = i8desk::business::Game::GetIcon(curGames_[i]);
}
void CDlgDownLoad::OnNMRClickListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_DOWNLOAD_CENTER);
	CMenu *subMenu = menu.GetSubMenu(0);

	if( centerMask_ == CENTER_UPDATE )
		subMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, ID_DOWNLOAD_CENTER_DOWN + 1, _T("强制更新"));

	CPoint point;
	::GetCursorPos(&point);

	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CDlgDownLoad::OnNMRClickListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_DOWNLOAD_LOCAL);
	CMenu *subMenu = menu.GetSubMenu(0);

	CPoint point;
	::GetCursorPos(&point);


	if(!i8desk::IsRunOnServer())
	{
		subMenu->EnableMenuItem(ID_DOWNLOAD_LOCAL_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_DOWNLOAD_LOCAL_OPEN_DIR, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_DOWNLOAD_LOCAL_ADD_SOME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_DOWNLOAD_LOCAL_MAKE_IDX, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_DOWNLOAD_LOCAL_TEST_IDX, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}


	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CDlgDownLoad::OnNMRClickListDownloadDownload(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_TASK_OPERATE);
	CMenu *subMenu = menu.GetSubMenu(0);

	CPoint point;
	::GetCursorPos(&point);

	if( pNMItemActivate->iItem == -1 )
	{
		subMenu->EnableMenuItem(ID_TASKOPERATE_STOP,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_TASKOPERATE_SUSPEND, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_TASKOPERATE_RESUME,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_TASKOPERATE_TOP,		MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		subMenu->EnableMenuItem(ID_TASKOPERATE_BOTTOM,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	else
	{
		using namespace i8desk;
		using data_helper::TaskStatusTraits;
		
		const TaskStatusTraits::ValueType &task = TaskGames.At(pNMItemActivate->iItem);
		if( task->State == tsDownloading || task->State == tsCommit)
		{
			subMenu->EnableMenuItem(ID_TASKOPERATE_RESUME,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			subMenu->EnableMenuItem(ID_TASKOPERATE_TOP,		MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			subMenu->EnableMenuItem(ID_TASKOPERATE_BOTTOM,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if( task->State == tsSuspend )
		{
			subMenu->EnableMenuItem(ID_TASKOPERATE_SUSPEND, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			subMenu->EnableMenuItem(ID_TASKOPERATE_TOP,		MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			subMenu->EnableMenuItem(ID_TASKOPERATE_BOTTOM,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else if( task->State == tsQueue || task->State == tsSelfQueue )
		{
			subMenu->EnableMenuItem(ID_TASKOPERATE_RESUME,	MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			subMenu->EnableMenuItem(ID_TASKOPERATE_SUSPEND, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}
	
	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}



void CDlgDownLoad::OnNMDblclkListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	OnBnClickedButtonDownloadModifyGame();
}


void CDlgDownLoad::OnBnClickedButtonDownloadLocalAdd()
{
	CDlgDownloadClass dlg(true, 0);
	if( dlg.DoModal() == IDOK )
	{
		HICON hIcon = Bitmap2Icon(dlg.GetCurClass()->Icon);
		int index = localTreeImageList_.Add(hIcon);
		dlg.GetCurClass()->ImgIndex = index;
		InitTreeCtrlLocal(wndTreeLocal_);
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadLocalDelete()
{
	HTREEITEM hItem = wndTreeLocal_.GetSelectedItem();
	if( hItem == 0 )
		return;

	using i8desk::data_helper::ClassTraits;
	ClassTraits::ElementType *val = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocal_.GetItemData(hItem));
	if( val == 0 )
		return;

//	stdex::tString tempCID = val->CID;
	if( i8desk::business::Class::IsDefault(val->CID) )
	{
		stdex::tString msg = _T("不能删除默认类别: ");
		msg += val->Name;
		CMessageBox msgDlg(_T("提示"),msg);
		msgDlg.DoModal();
		return;
	}

	stdex::tString  msg = _T("是否需要删除类别: ");
	msg += val->Name;
	CMessageBox msgDlg(_T("提示"),msg);
	
	if( msgDlg.DoModal() == IDOK )
	{
		if ( i8desk::business::Class::HasGame(val->CID) )
		{
			stdex::tString tmpmsg;
			tmpmsg = _T("类别下存在游戏,请先删除该类别下所有游戏 ");
			CMessageBox tmpmsgDlg(_T("提示"),tmpmsg);
			tmpmsgDlg.DoModal();
			return;
		}
		else
		{
			if( i8desk::GetDataMgr().DelData(i8desk::GetDataMgr().GetClasses(), val->CID) )
				InitTreeCtrlLocal(wndTreeLocal_);
		}
	}
	
}

void CDlgDownLoad::OnBnClickedButtonDownloadLocalModify()
{
	HTREEITEM hItem = wndTreeLocal_.GetSelectedItem();
	if( hItem == 0 )
		return;

	using i8desk::data_helper::ClassTraits;
	ClassTraits::ElementType *val = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocal_.GetItemData(hItem));
	if( val == 0 )
		return;

	CDlgDownloadClass dlg(false, val);
	if( dlg.DoModal() == IDOK )
	{		
		HICON icon = Bitmap2Icon(dlg.GetCurClass()->Thumbnail);
		localTreeImageList_.Replace(dlg.GetCurClass()->ImgIndex, icon);
		InitTreeCtrlLocal(wndTreeLocal_);
	}
}



void CDlgDownLoad::OnBnClickedButtonDownloadStart()
{
	i8desk::AsyncDataHelper(std::tr1::bind(&StatusTaskOperate<CListCtrl>,
		std::tr1::ref(wndListDownload_), i8desk::toRestart));
}

void CDlgDownLoad::OnBnClickedButtonDwonloadPause()
{
	i8desk::AsyncDataHelper(std::tr1::bind(&StatusTaskOperate<CListCtrl>,
		std::tr1::ref(wndListDownload_), i8desk::toSuspend));
}

void CDlgDownLoad::OnBnClickedButtonDownloadStop()
{
	i8desk::AsyncDataHelper(std::tr1::bind(&StatusTaskOperate<CListCtrl>,
		std::tr1::ref(wndListDownload_), i8desk::toDel));
}


void CDlgDownLoad::OnBnClickedButtonDownloadFilter()
{
	using i8desk::data_helper::ClassTraits;
	const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

	CMenu *subMenu = menuClassFilter_.GetSubMenu(0);

	if( subMenu->GetMenuItemCount() > 2 )
	{
		for(size_t i = subMenu->GetMenuItemCount(); i != 1; --i)
		{
			subMenu->DeleteMenu(i, MF_BYPOSITION);
		}
	}

	int nIndex = 1;
	for(ClassTraits::MapConstIterator iter = classes.begin(); iter != classes.end(); ++iter)
	{
		assert(ID_DOWNLOAD_CENTER_CLASS + nIndex <= ID_DOWNLOAD_CENTER_CLASS + ClassFilerMax);

		subMenu->InsertMenu(0xFFFF, MF_STRING | MF_BYCOMMAND, ID_DOWNLOAD_CENTER_CLASS + nIndex, iter->second->Name);
		++nIndex;
	}

	subMenu->CheckMenuItem(ClassFilterCheck, MF_BYCOMMAND | MF_CHECKED);

	CRect rcBtn;
	wndBtnClassFilter_.GetWindowRect(rcBtn);
	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rcBtn.left, rcBtn.bottom, this);
}

void CDlgDownLoad::OnBnClickedButtonDownloadToadd()
{
	if( viewStatus_ != Center )
		return;

	using i8desk::data_helper::GameTraits;
	GameTraits::VectorType games;

	// 选择
	int nSel = -1;
	while( (nSel = wndListCenter_.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		games.push_back(curGames_[nSel]);
	}

	if( games.empty() )
		return;


	{
		bool isShowVersion = i8desk::GetDataMgr().GetOptVal(OPT_M_GAMEVERSION, 1) == 1;
		if( isShowVersion )
		{
			// 下载单机游戏时提示版权
			for(size_t i = 0; i != games.size(); ++i)
			{
				if( i8desk::business::Class::IsSingleGame(games[i]->CID) )
				{
					CDlgGameVersion dlg;
					dlg.DoModal();
					break;
				}
			}
		}
	}

	// 判断
	if( games.size() == 1 )
	{
		const GameTraits::ValueType &game = games[0];

		if( !i8desk::business::Game::CouldDownload(game) )
			return;

		// 非本地游戏在下载之前要做些基本设置
		if( !game->Status || utility::IsEmpty(game->SvrPath) )
		{
			CDlgDownloadCenterGame dlg(false);
			dlg.SetCurrentGame(game);
			if( dlg.DoModal() != IDOK )
				return;
		}

		if( i8desk::GetTaskMgr().TaskOperate(game->GID, i8desk::toAdd, 
			game->Priority, game->IdcVer, false, game->IdcClick) )
		{
			//InsertLog("下载游戏:%05d:%s", pGame->GID, pGame->Name);
		}
	}
	else
	{
		CDlgDownloadCenterGame dlg(true);
		dlg.SetCurrentGames(games);

		if( dlg.DoModal() != IDOK )
			return;

		for(GameTraits::VecConstIterator iter = games.begin(); 
			iter != games.end(); ++iter)
		{
			if( !i8desk::business::Game::CouldDownload(*iter) )
				continue;

			if( i8desk::GetTaskMgr().TaskOperate((*iter)->GID, i8desk::toAdd, 
				(*iter)->Priority, (*iter)->IdcVer, false, (*iter)->IdcClick) )
			{
				//InsertLog("下载游戏:%05d:%s", pGame->GID, pGame->Name);
			}
		}
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadAddGame()
{
	if(!i8desk::IsRunOnServer())
	{
		CMessageBox  msgbox(_T("提示"), _T("远程登录控制台时, 无法进行此操作！"));
		msgbox.DoModal();
		return;
	}

	stdex::tString CID = _T("");
	HTREEITEM localhItem = wndTreeLocal_.GetSelectedItem();

	ClassTraits::ElementType *val = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocal_.GetItemData(localhItem));
	if( val != 0 )
		CID = val->CID;
	CDlgDownloadGameInfo dlg(true, 0 , CID);
	if( dlg.DoModal() == IDOK )
	{		
		_ShowView();
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadAddSomeGame()
{
	if(!i8desk::IsRunOnServer())
	{
		CMessageBox  msgbox(_T("提示"), _T("远程登录控制台时, 无法进行此操作！"));
		msgbox.DoModal();
		return;
	}

	CString classes = wndTreeLocal_.GetItemText(wndTreeLocal_.GetSelectedItem());
	CDlgDownloadSomeAdd dlg((LPCTSTR)classes);
	if( dlg.DoModal() != IDOK )
		return;

	_ShowView();
}

void CDlgDownLoad::OnBnClickedButtonDownloadForceupdate()
{
	GameTraits::VectorType games;

	int nSel = -1;
	while ((nSel = wndListCenter_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		if( nSel >= (int)curGames_.size() )
			return;

		games.push_back(curGames_[nSel]); 
	}

	LocalTaskOperate(wndListLocal_, games, i8desk::toAdd, true);
}

void CDlgDownLoad::OnBnClickedButtonDownloadSmartClean()
{
	CDlgDownloadSmartClean dlg;
	dlg.DoModal();
}

void CDlgDownLoad::OnBnClickedButtonDownloadModifyGame()
{
	if( wndListLocal_.GetSelectedCount() == 1 )
	{	
		size_t sel = wndListLocal_.GetNextItem(-1, LVIS_SELECTED);
		i8desk::data_helper::GameTraits::ValueType &curGame = curGames_[sel];
		stdex::tString CID = curGame.get()->CID;

		CDlgDownloadGameInfo dlg(false, curGame.get(), CID);
		if( dlg.DoModal() != IDOK )
			return;
	}
	else
	{
		using i8desk::data_helper::GameTraits;
		GameTraits::VectorType games;
		// 选择
		int nSel = -1;
		while( (nSel = wndListLocal_.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
			games.push_back(curGames_[nSel]);

		if( games.empty() )
			return;

		CDlgDownloadSomeMod dlg(games);
		if( dlg.DoModal() != IDOK )
			return;
		

	}

	_ShowView();
}

void CDlgDownLoad::OnBnClickedButtonDownloadEraseGame()
{
	if( viewStatus_ != Local )
		return;

	using i8desk::data_helper::GameTraits;
	GameTraits::VectorType games;

	// 选择
	int nSel = -1;
	while( (nSel = wndListLocal_.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		games.push_back(curGames_[nSel]);
	}

	if( games.empty() )
		return;

	CDlgDownloadDelGame dlg(games);

	if( dlg.DoModal() == IDOK )
	{
		_ShowView();	
	}

}

void CDlgDownLoad::OnQueryTextChanged()
{

}

void CDlgDownLoad::ONBnClickedButtonSearch()
{
	CString text;
	wndEditQuery_.GetWindowText(text);

	i8desk::data_helper::GameTraits::VectorType tmp;
	CListCtrl *curList = 0;
	switch(viewStatus_)
	{
	case Center:
		i8desk::business::Game::FindGame(text, curGames_, tmp);
		curList = &wndListCenter_;
		break;
	case Local:
		i8desk::business::Game::FindGame(text, curGames_, tmp);
		curList = &wndListLocal_;
		break;
	case Download:
		i8desk::business::Game::FindGame(text, curGames_, tmp);
		curList = &wndListDownload_;
		break;
	case Update:
		break;
	default:
		assert(0);
		break;
	}

	if( curList == 0 )
		return;

	if( viewStatus_ != Download )
	{
		curGames_.swap(tmp);
		curList->SetItemCount(curGames_.size());
	}
	else
	{
		curList->SetItemState(-1, 0, -1);

		for(size_t i = 0; i != TaskGames.Size(); ++i)
		{
			struct Find
			{
				const long gid_;
				Find(const long gid)
					: gid_(gid)
				{}
				bool operator()(const i8desk::data_helper::GameTraits::ValueType &val) const
				{
					return gid_ == val->GID;
				}
			};
			if( std::find_if(tmp.begin(), tmp.end(), Find(TaskGames.At(i)->GID)) != tmp.end() )
			{
				curList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadTop()
{
	if( wndListDownload_.GetSelectedCount() != 1 )
	{
		CMessageBox msgDlg(_T("提示"), _T("请对单个下载任务做置顶操作！"));
		msgDlg.DoModal();
		return;
	}

	int nIndex = 0;
	nIndex = wndListDownload_.GetNextItem(nIndex, LVIS_SELECTED);
	if( nIndex != -1 )
	{
		const TaskStatusTraits::ValueType &val = TaskGames.At(nIndex);
		i8desk::GetTaskMgr().PutTopOrBottom(val->GID, true);
	}
}

void CDlgDownLoad::OnBnClickedButtonDownloadBottom()
{
	if( wndListDownload_.GetSelectedCount() != 1 )
	{
		CMessageBox msgDlg(_T("提示"), _T("请对单个下载任务做置底操作！"));
		msgDlg.DoModal();
		return;
	}

	int nIndex = 0;
	nIndex = wndListDownload_.GetNextItem(nIndex, LVIS_SELECTED);
	if( nIndex != -1 )
	{
		const TaskStatusTraits::ValueType &val = TaskGames.At(nIndex);
		i8desk::GetTaskMgr().PutTopOrBottom(val->GID, false);
	}
}

void CDlgDownLoad::OnBnClickedButtonShowLog()
{
	AfxGetMainWnd()->PostMessage(i8desk::ui::WM_SHOW_LOG);
}

void CDlgDownLoad::OnMenuClassFilter(UINT uID)
{
	using i8desk::data_helper::GameTraits;
	using i8desk::data_helper::ClassTraits;

	using std::tr1::bind;
	using namespace std::tr1::placeholders;

	const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

	if( uID == ID_DOWNLOAD_CENTER_CLASS )
	{
		FilterCID.clear();
	}
	else
	{
		CMenu *subMenu = menuClassFilter_.GetSubMenu(0);
		CString menuText;
		subMenu->GetMenuString(uID, menuText, MF_BYCOMMAND);

		const ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();
		for(ClassTraits::MapConstIterator iter = classes.begin(); iter != classes.end(); ++iter)
		{
			if( iter->second->Name == menuText )
			{	
				FilterCID = iter->second->CID;
				break;
			}
		}
		subMenu->CheckMenuItem(ClassFilterCheck, MF_BYCOMMAND | MF_UNCHECKED);
	}

	ClassFilterCheck = uID;

	_ShowView();
}

void CDlgDownLoad::OnMenuCenter(UINT uID)
{
	switch(uID)
	{
	case ID_DOWNLOAD_CENTER_DOWN:
		OnBnClickedButtonDownloadToadd();
		break;
	case ID_DOWNLOAD_CENTER_DOWN + 1:
		OnBnClickedButtonDownloadForceupdate();
		break;
	case ID_DOWNLOAD_CENTER_REFRESHPL:
		i8desk::GetTaskMgr().RefreshPL();
		break;
	}
}

void CDlgDownLoad::OnMenuLocal(UINT uID)
{

	switch(uID)
	{
	case ID_DOWNLOAD_LOCAL_ADD:
		OnBnClickedButtonDownloadAddGame();
		return;
	case ID_DOWNLOAD_LOCAL_MODIFY:
		OnBnClickedButtonDownloadModifyGame();
		return;
	case ID_DOWNLOAD_LOCAL_DELETE:
		OnBnClickedButtonDownloadEraseGame();
		return;
	case ID_DOWNLOAD_LOCAL_ADD_SOME:
		OnBnClickedButtonDownloadAddSomeGame();
		break;
	case ID_DOWNLOAD_LOCAL_NOTIFYSYNCTASK:
		{
			using i8desk::data_helper::GameTraits;
			GameTraits::VectorType games;

			int nSel = -1;
			while((nSel = wndListLocal_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
			{
				assert(nSel < (int)curGames_.size());
				games.push_back(curGames_[nSel]); 
			}

			struct Msg
			{
				void operator()(const stdex::tString &msg)
				{
					CMessageBox dlg(_T("提示"), msg);
					dlg.DoModal();
				}
			};

			if( !games.empty() )
				i8desk::business::SyncTask::CanNotifyGame(games, Msg());
		}
		break;
	case ID_DOWNLOAD_LOCAL_MAKE_IDX:
	case ID_DOWNLOAD_LOCAL_TEST_IDX:
		{
			using i8desk::data_helper::GameTraits;

			GameTraits::VectorType games;

			int nSel = -1;
			while ((nSel = wndListLocal_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
			{
				assert(nSel < (int)curGames_.size());
				games.push_back(curGames_[nSel]); 
			}

			if( games.empty() )
				return;

			CDlgDownloadIdx dlg(games, uID == ID_DOWNLOAD_LOCAL_MAKE_IDX ? true : false);
			dlg.DoModal();
		}

		break;
	case ID_DOWNLOAD_LOCAL_OPEN_DIR:
		{
			int nSel = -1;
			while( (nSel = wndListLocal_.GetNextItem(nSel, LVIS_SELECTED)) != -1)
			{
				if( (size_t)nSel >= curGames_.size() ) 
					return;

				stdex::tString path = curGames_[nSel]->SvrPath;
				if( ::PathFileExists(path.c_str()) )
					::ShellExecute(m_hWnd, _T("open"), path.c_str(), _T(""), _T(""), SW_SHOWNORMAL);
				else
				{
					stdex::tOstringstream os;
					os << _T("打开目录错:[") << path << _T("]:路径不存在");

					CMessageBox box(_T("提示"), os.str());
					box.DoModal();
				}
			}
		}
		break;
	case ID_DOWNLOAD_LOCAL_UPDATE:
		LocalTaskOperate(wndListLocal_, curGames_, i8desk::toAdd, false);
		break;
	case ID_DOWNLOAD_LOCAL_FORCE_UPDATE:
		LocalTaskOperate(wndListLocal_, curGames_, i8desk::toAdd, true);
		break;
	case ID_DOWNLOAD_LOCAL_VIEW:
		{
			int sel = wndListLocal_.GetNextItem(-1, LVIS_SELECTED);
			if( sel != -1 )
			{
				CDlgDownloadGameViewClient dlg(curGames_[sel]->GID);
				dlg.DoModal();
			}
		}
		break;
	default:
		break;
	}

}


void CDlgDownLoad::OnMenuTask(UINT uID)
{
	switch(uID)
	{
	case ID_TASKOPERATE_RESUME:
		StatusTaskOperate(wndListDownload_, i8desk::toRestart);
		break;
	case ID_TASKOPERATE_STOP:
		StatusTaskOperate(wndListDownload_, i8desk::toDel);
		break;
	case ID_TASKOPERATE_SUSPEND:
		StatusTaskOperate(wndListDownload_, i8desk::toSuspend);
		break;
	case ID_TASKOPERATE_TOP:
		OnBnClickedButtonDownloadTop();
		break;
	case ID_TASKOPERATE_BOTTOM:
		OnBnClickedButtonDownloadBottom();
		break;
	case ID_TASKOPERATE_REFRESH:
		i8desk::GetTaskMgr().RefreshPL();
		break;
	default:
		break;
	}
}
	
void CDlgDownLoad::OnLvnColumnclickListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListCenter_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curGames_.begin(), curGames_.end(), Compare<Center>(pNMLV->iSubItem, CenterCompare[pNMLV->iSubItem]));
	CenterCompare.set(pNMLV->iSubItem, !CenterCompare[pNMLV->iSubItem]);
	wndListCenter_.RedrawCurPageItems();

	wndListCenter_.SetSortArrow(pNMLV->iSubItem, CenterCompare[pNMLV->iSubItem]);
}

void CDlgDownLoad::OnLvnColumnclickListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListLocal_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curGames_.begin(), curGames_.end(), Compare<Local>(pNMLV->iSubItem, LocalCompare[pNMLV->iSubItem]));
	LocalCompare.set(pNMLV->iSubItem, !LocalCompare[pNMLV->iSubItem]);
	wndListLocal_.RedrawCurPageItems();

	wndListLocal_.SetSortArrow(pNMLV->iSubItem, LocalCompare[pNMLV->iSubItem]);
}


void CDlgDownLoad::OnEnChangeEditDownloadSearch()
{
	_ShowView();
	ONBnClickedButtonSearch();
}

void CDlgDownLoad::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
	*pResult = 0;
}

void CDlgDownLoad::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow != NULL && (pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_DOWN || (pLVKeyDow->wVKey == 'A' && (pLVKeyDow->flags & MK_CONTROL))))
	{
		PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
	}
	*pResult = 0;
}

stdex::tString CDlgDownLoad::GetSizeText(ULONGLONG totalSize)
{
    if (totalSize == 0)
    {
        return _T("");
    }
    stdex::tString totalSizeText;
    i8desk::FormatSize(totalSize, totalSizeText);
    stdex::tString sInfoEx = stdex::tString(_T("大小")) + totalSizeText;
    return sInfoEx;
}

LRESULT CDlgDownLoad::OnReportSelectState(WPARAM wParam,LPARAM lParam)
{
    stdex::tString sInfoEx;
	switch (viewStatus_)
	{
	case Center:
        sInfoEx = GetSizeText(GetSelectedGameTotalSize(wndListCenter_));
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListCenter_.GetSelectedCount());
		break;
    case Local:
        sInfoEx = GetSizeText(GetSelectedGameTotalSize(wndListLocal_));
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListLocal_.GetSelectedCount());
		break;
    case Download:
        sInfoEx = GetSizeText(GetSelectedGameTotalSize(wndListDownload_));
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListDownload_.GetSelectedCount());
		break;
    case Update:
        sInfoEx = GetSizeText(GetSelectedGameTotalSize(wndListUpdate_));
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListUpdate_.GetSelectedCount());
		break;
	default:
		break;
	}
	return 1;
}

ULONGLONG CDlgDownLoad::GetSelectedGameTotalSize(CListCtrl& listCtrl)
{
    ULONGLONG totalSize = 0;
    int nSel = -1;
    while( (nSel = listCtrl.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
    {
        if (nSel >= 0 && nSel < curGames_.size())
        {
            totalSize += curGames_[nSel]->Size * 1024LL;
        }
    }
    return totalSize;
}
