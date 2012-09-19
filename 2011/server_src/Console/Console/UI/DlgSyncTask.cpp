// DlgSyncTask.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgSyncTask.h"

#include <bitset>
#include <vector>


#include "../../../../include/Utility/utility.h"
#include "../../../../include/Extend STL/StdEx.h"
#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/MultiThread/Lock.hpp"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/UpdateGame.h"


#include "../Business/GameBusiness.h"
#include "../Business/ClassBusiness.h"
#include "../Business/UpdateBusiness.h"
#include "../Business/SyncTaskBusiness.h"
#include "../Business/ServerBusiness.h"

#include "../Misc.h"
#include "../MessageBox.h"
#include "../AsyncDataHelper.h"
#include "../ManagerInstance.h"

#include "DlgSyncTaskInfoEx.h"
#include "DlgSyncTaskDelInfo.h"
#include "DlgSyncTaskAddorCopy.h"

#include "UIHelper.h"

enum {WM_CUMMULATE = WM_USER + 0x700 };

enum { DT_TIMER = 1000 };

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{

	template < typename TreeCtrlT >
	void InitlocalTreeImg(TreeCtrlT &treeimg)
	{
		treeimg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
		treeimg.Add(CBitmap::FromHandle(i8desk::ui::SkinMgrInstance().GetSkin(_T("Download/svrresmgr.png"))), RGB(0, 0, 0));

		using i8desk::data_helper::ClassTraits;
		ClassTraits::MapType &classes = i8desk::GetDataMgr().GetClasses();

		for(ClassTraits::MapConstIterator iter = classes.begin(); iter != classes.end(); ++iter)
		{
			HICON icon = Bitmap2Icon(i8desk::business::Class::GetIcon(iter->second));
			int index = treeimg.Add(icon);
			if( index != -1 )
				iter->second->ImgIndex = index;
			else
				iter->second->ImgIndex = 0;
		}
	}

	template < typename TreeCtrlT >
	void InitsyncTreeImg(TreeCtrlT &treeimg)
	{
		treeimg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);

		HBITMAP tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("SyncTask/Ico_Sync_Running.png"));
		treeimg.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));

		tmp = i8desk::ui::SkinMgrInstance().GetSkin(_T("SyncTask/Ico_Sync_Stoped.png"));
		treeimg.Add(CBitmap::FromHandle(tmp), RGB(0, 0, 0));

	}

	static LPCTSTR LocalText	= _T("本地资源");


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


	template < typename TreeCtrlT, typename SyncTasksT >
	void InitTreeTaskCtrl(TreeCtrlT &treeSyncTask, const SyncTasksT &syncTasks)
	{
		treeSyncTask.DeleteAllItems();


		typedef i8desk::data_helper::SyncTaskTraits SyncTasksType;
		const SyncTasksType::MapType &synctasks = i8desk::GetDataMgr().GetSyncTasks();

		for(SyncTasksType::MapConstIterator iter = synctasks.begin();
			iter != synctasks.end(); ++iter)
		{
			int index = !i8desk::business::SyncTask::IsRunning(iter->second->SID) ? 1 : 0;
			HTREEITEM hItem = treeSyncTask.InsertItem(iter->second->Name, index, index);
			treeSyncTask.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(iter->second.get()));

		}

		treeSyncTask.SetItemHeight(25);
	}



	CCustomColumnTraitGameIcon *localIcon			= 0;
	CCustomColumnTrait *syncgameStatus				= 0;
	CCustomProgressColumnTrait *syncgameProgress	= 0;


	namespace detail
	{
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
		MASK_LOCAL_NAME = 1, MASK_LOCAL_GID, MASK_LOCAL_CLICK, MASK_LOCAL_TIME, MASK_LOCAL_SIZE, MASK_LOCAL_EXE, MASK_LOCAL_PATH, 
		MASK_LOCAL_CLASS
	};

	template < typename ListCtrlT, typename GamesT >
	void InitLocalList(ListCtrlT &listCtrl, GamesT &games)
	{
		listCtrl.InsertHiddenLabelColumn();

		CCustomColumnTraitGameIcon *col = detail::GetLocalIconTrait(listCtrl, games);

		col->SetDefaultBmp(i8desk::ui::SkinMgrInstance().GetIcon(0));;


		listCtrl.InsertColumnTrait(MASK_LOCAL_NAME,		TEXT("名称"),		LVCFMT_LEFT, 120,	MASK_LOCAL_NAME,	col);
		listCtrl.InsertColumnTrait(MASK_LOCAL_GID,		TEXT("GID"),		LVCFMT_LEFT, 70,	MASK_LOCAL_GID,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_CLICK,	TEXT("点击数"),		LVCFMT_RIGHT,60,	MASK_LOCAL_CLICK,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_TIME,		TEXT("本地更新时间"),	LVCFMT_LEFT, 130,	MASK_LOCAL_TIME,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_SIZE,		TEXT("大小(M)"),		LVCFMT_LEFT, 70,	MASK_LOCAL_SIZE,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_EXE,		TEXT("执行程序"),		LVCFMT_LEFT, 120,	MASK_LOCAL_EXE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_PATH,		TEXT("服务端路径"),	LVCFMT_LEFT, 200,	MASK_LOCAL_PATH,	detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_LOCAL_CLASS,	TEXT("类别"),		LVCFMT_LEFT, 75,	MASK_LOCAL_CLASS,	detail::GetTrait());

		listCtrl.SetItemHeight(24);
	}


	enum 
	{
		MASK_TASK_GAMENAME = 1, MASK_TASK_GID, MASK_TASK_CLASS, MASK_TASK_STATUS, MASK_TASK_SVRDIR, MASK_TASK_NODEDIR, 
		MASK_TASK_UPDATESIZE, MASK_TASK_PROGRESS , MASK_TASK_TRANSFERRATE
	};

	template < typename ListCtrlT, typename GamesT >
	void InitTaskList(ListCtrlT &listCtrl, GamesT &games)
	{
		listCtrl.InsertHiddenLabelColumn();

		listCtrl.InsertColumnTrait(MASK_TASK_GAMENAME,		TEXT("游戏名称"),		LVCFMT_LEFT, 120,	MASK_TASK_GAMENAME,			detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_GID,			TEXT("GID"),			LVCFMT_LEFT, 70,	MASK_TASK_GID,				detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_CLASS,			TEXT("类别"),			LVCFMT_LEFT, 70,	MASK_TASK_CLASS,			detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_STATUS,		TEXT("当前状态"),		LVCFMT_LEFT, 80,	MASK_TASK_STATUS,			syncgameStatus = detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_SVRDIR,		TEXT("源路径"),			LVCFMT_LEFT, 160,	MASK_TASK_SVRDIR,			detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_NODEDIR,		TEXT("目标路径"),		LVCFMT_RIGHT,160,	MASK_TASK_NODEDIR,			detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_UPDATESIZE,	TEXT("更新量"),			LVCFMT_LEFT, 70,	MASK_TASK_UPDATESIZE,		detail::GetTrait());
		listCtrl.InsertColumnTrait(MASK_TASK_PROGRESS,		TEXT("进度"),			LVCFMT_LEFT, 100,	MASK_TASK_PROGRESS,			syncgameProgress = detail::GetProgressTrait(listCtrl));
		listCtrl.InsertColumnTrait(MASK_TASK_TRANSFERRATE,	TEXT("速度"),			LVCFMT_LEFT, 80,	MASK_TASK_TRANSFERRATE,		detail::GetTrait());

		listCtrl.SetItemHeight(24);
	}


	// 同步游戏实时数据更新
	using i8desk::data_helper::SyncTaskStatusTraits;

	static struct
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

		SyncTaskStatusTraits::VectorType synctaskgames_;
		mutable Mutex mutex_;


		size_t Size() const
		{
			AutoLock lock(mutex_);
			return synctaskgames_.size();
		}

		template < typename GamesT >
		void Update(const GamesT &games)
		{
			AutoLock lock(mutex_);

			synctaskgames_.clear();
			synctaskgames_ = games;
		}

		SyncTaskStatusTraits::ValueType At(size_t index) const
		{
			AutoLock lock(mutex_);
			assert(index < synctaskgames_.size());

			if( synctaskgames_.empty() || index >= synctaskgames_.size() )
				return SyncTaskStatusTraits::ValueType(new SyncTaskStatusTraits::ElementType);

			return synctaskgames_[index];
		}
	}SyncTaskStatus;

	void SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &IsDelFile)
	{
		i8desk::AsyncDataHelper<bool>(std::tr1::bind(&i8desk::manager::ControlMgr::SyncTaskOperate, 
			std::tr1::ref(i8desk::GetControlMgr()), cmd, gid, SID, TaskName, IsDelFile));
	}


	// 三层排序
	std::bitset<MASK_TASK_TRANSFERRATE + 1> TaskCompare;
	std::bitset<MASK_LOCAL_CLASS + 1>	LocalCompare;

	template <size_t Status>
	struct Compare
	{
		Compare(int, bool)
		{}
		bool operator()()
		{ return true; }
	};

	template<>
	struct Compare<CDlgSyncTask::Tasks>
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
			case MASK_TASK_GAMENAME:
				return ascSort_ ? utility::Strcmp(lhs->GameName, rhs->GameName) > 0 : utility::Strcmp(lhs->GameName, rhs->GameName) < 0;
			case MASK_TASK_GID:
				return ascSort_ ? lhs->GID > rhs->GID : lhs->GID < rhs->GID;
			case MASK_TASK_CLASS:
				return ascSort_ ? i8desk::business::Game::GetClass(lhs->GID) > i8desk::business::Game::GetClass(rhs->GID) :
					i8desk::business::Game::GetClass(lhs->GID) < i8desk::business::Game::GetClass(rhs->GID);
			case MASK_TASK_STATUS:
				return ascSort_ ? lhs->UpdateState > rhs->UpdateState : lhs->UpdateState < rhs->UpdateState;
			case MASK_TASK_SVRDIR:
				return ascSort_ ? utility::Strcmp(lhs->SvrDir, rhs->SvrDir) > 0 : utility::Strcmp(lhs->SvrDir, rhs->SvrDir) < 0;
			case MASK_TASK_NODEDIR:
				return ascSort_ ? utility::Strcmp(lhs->NodeDir, rhs->NodeDir) > 0 : utility::Strcmp(lhs->NodeDir, rhs->NodeDir) < 0;
			case MASK_TASK_UPDATESIZE:
				return ascSort_ ? lhs->UpdateSize > rhs->UpdateSize : lhs->UpdateSize < rhs->UpdateSize;
			case MASK_TASK_PROGRESS:
				return ascSort_ ? lhs->BytesTransferred > rhs->BytesTransferred : lhs->BytesTransferred < rhs->BytesTransferred;
			case MASK_TASK_TRANSFERRATE:
				return ascSort_ ? lhs->TransferRate > rhs->TransferRate : lhs->TransferRate < rhs->TransferRate;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

	template<>
	struct Compare<CDlgSyncTask::LocalGames>
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
				break;
			default:
				assert(0);
				return false;
			}
		}
	};
}

// CDlgSyncTask 对话框

IMPLEMENT_DYNAMIC(CDlgSyncTask, CDialog)

CDlgSyncTask::CDlgSyncTask(CWnd* pParent /*=NULL*/)
: i8desk::ui::BaseWnd(CDlgSyncTask::IDD, pParent)
, curTreeItem_(NULL)
{

}

CDlgSyncTask::~CDlgSyncTask()
{
}

void CDlgSyncTask::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_SYNCTASK_GAMES, wndTreeLocalGame_);
	DDX_Control(pDX, IDC_TREE_SYNCTASK_TASKS, wndTreeSyncTask_);
	DDX_Control(pDX, IDC_LIST_SYNCTASK_LOCALRES, wndListLocalGames_);
	DDX_Control(pDX, IDC_LIST_SYNCTASK_TASKS, wndListTaskGames_);
	DDX_Control(pDX, IDC_EDIT_SYNCTASK_SEARCH, wndEditQuery_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_SEARCH, wndBtnQuery_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_RES_MGR, wndBtnLocalGameMgr_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_LOCAL_MGR, wndBtnSyncTaskMgr_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_LOCAL_ADD, wndBtnSyncTaskAdd_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_LOCAL_MODIFY, wndBtnSyncTaskModify_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_LOCAL_DELETE, wndBtnSyncTaskDelete_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_START, wndBtnTaskStart_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_PAUSE, wndBtnTaskPause_);

	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_NOTIFYSYNC, wndBtnNotifySync_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_DELSYNCGAME, wndBtnDelSyncGame_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_ADDSYNCGAME, wndBtnAddSyncGame_);
	DDX_Control(pDX, IDC_BUTTON_SYNCTASK_SYNCTASK_DELSYNCGAME, wndBtnDelSyncGameToSID_);

}


BEGIN_EASYSIZE_MAP(CDlgSyncTask)   
	EASYSIZE(IDC_TREE_SYNCTASK_GAMES, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_TREE_SYNCTASK_TASKS, ES_BORDER, IDC_TREE_SYNCTASK_GAMES, ES_KEEPSIZE, ES_BORDER, 0)

	EASYSIZE(IDC_LIST_SYNCTASK_LOCALRES, IDC_TREE_SYNCTASK_GAMES, ES_BORDER, ES_BORDER, ES_BORDER, 0)

	EASYSIZE(IDC_BUTTON_SYNCTASK_LOCAL_MGR, ES_BORDER, IDC_TREE_SYNCTASK_GAMES, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_SYNCTASK_LOCAL_ADD, ES_BORDER, IDC_BUTTON_SYNCTASK_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_SYNCTASK_LOCAL_DELETE, IDC_BUTTON_SYNCTASK_LOCAL_ADD, IDC_BUTTON_SYNCTASK_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_SYNCTASK_LOCAL_MODIFY, IDC_BUTTON_SYNCTASK_LOCAL_DELETE, IDC_BUTTON_SYNCTASK_LOCAL_MGR, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_BUTTON_SYNCTASK_START, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_SYNCTASK_PAUSE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)

	EASYSIZE(IDC_EDIT_SYNCTASK_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_BUTTON_SYNCTASK_SEARCH, ES_KEEPSIZE, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)

END_EASYSIZE_MAP 


BEGIN_MESSAGE_MAP(CDlgSyncTask, i8desk::ui::BaseWnd)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()

	ON_WM_TIMER()

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SYNCTASK_GAMES, &CDlgSyncTask::OnTvnSelchangedTreeLocalGames)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SYNCTASK_TASKS, &CDlgSyncTask::OnTvnSelchangedTreeSyncTask)

	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnLvnGetdispinfoListLocalGames)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_SYNCTASK_TASKS, &CDlgSyncTask::OnLvnGetdispinfoListTaskGames)

	ON_NOTIFY(NM_CLICK, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnNMClickList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnLvnKeydownList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SYNCTASK_TASKS, &CDlgSyncTask::OnNMClickList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SYNCTASK_TASKS, &CDlgSyncTask::OnLvnKeydownList)

	ON_NOTIFY(LVN_ODCACHEHINT, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnLvnOdcachehintListLocalSyncGames)

	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_LOCAL_ADD, &CDlgSyncTask::OnBnClickedButtonSynctaskAdd)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_LOCAL_DELETE, &CDlgSyncTask::OnBnClickedButtonSynctaskDelete)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_LOCAL_MODIFY, &CDlgSyncTask::OnBnClickedButtonSynctaskModify)

	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_START, &CDlgSyncTask::OnBnClickedButtonSyncTaskStart)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_PAUSE, &CDlgSyncTask::OnBnClickedButtonSyncTaskPause)

	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_SEARCH, &CDlgSyncTask::ONBnClickedButtonSearch)
	ON_EN_CHANGE(IDC_EDIT_SYNCTASK_SEARCH, &CDlgSyncTask::OnEnChangeEditDownloadSearch)

	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_NOTIFYSYNC, &CDlgSyncTask::ONBnClickedButtonNotifySync)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_DELSYNCGAME, &CDlgSyncTask::ONBnClickedButtonDelSyncGame)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_ADDSYNCGAME, &CDlgSyncTask::ONBnClickedButtonAddSyncGame)
	ON_BN_CLICKED(IDC_BUTTON_SYNCTASK_SYNCTASK_DELSYNCGAME, &CDlgSyncTask::ONBnClickedButtonDelSyncGameToSID)


	ON_COMMAND(ID_SYNC_LOCAL_NOTIFY, &CDlgSyncTask::OnMenuLocalNotify)
	ON_COMMAND(ID_SYNC_LOCAL_ADDGAMETOALL, &CDlgSyncTask::OnMenuLocalAddGame)
	ON_COMMAND(ID_SYNC_LOCAL_DELGAMETOALL, &CDlgSyncTask::OnMenuLocalDelGame)
	ON_COMMAND(ID_SYNC_SYNCTASK_NOTIFY, &CDlgSyncTask::OnMenuSyncTaskNotify)
	ON_COMMAND(ID_SYNC_SYNCTASK_DELGAMETOTID, &CDlgSyncTask::OnMenuSyncTaskDelGame)
	ON_COMMAND(ID_SYNCTASK_TREETASK_COPY, &CDlgSyncTask::OnMenuSyncTreeCopy)
	ON_COMMAND(ID_SYNCTASK_TREETASK_DEL, &CDlgSyncTask::OnMenuSyncTreeDel)


	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnNMRClickListLocalGames)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYNCTASK_TASKS, &CDlgSyncTask::OnNMRClickListSyncTasks)

	ON_MESSAGE(WM_CUMMULATE, &CDlgSyncTask::OnCummulate)
	ON_MESSAGE(i8desk::ui::WM_APPLY_SELECT, &CDlgSyncTask::OnApplySelect)
	ON_MESSAGE(i8desk::ui::WM_REPORT_SELECT_STATE, &CDlgSyncTask::OnReportSelectState)

	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SYNCTASK_LOCALRES, &CDlgSyncTask::OnLvnColumnclickListLocalGames)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SYNCTASK_TASKS, &CDlgSyncTask::OnLvnColumnclickListSyncTasks)

	ON_NOTIFY(NM_RCLICK, IDC_TREE_SYNCTASK_TASKS, &CDlgSyncTask::OnNMRClickTreeSynctaskTasks)
END_MESSAGE_MAP()


// CDlgSyncTask 消息处理程序
void CDlgSyncTask::Register()
{
	SetTimer(DT_TIMER, 2000, 0);
	i8desk::GetRealDataMgr().Register(std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_SYNCTASKSTATUS);
};

void CDlgSyncTask::UnRegister()
{
	KillTimer(DT_TIMER);
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_SYNCTASKSTATUS);
};

void CDlgSyncTask::OnRealDataUpdate()
{
	i8desk::business::SyncTask::SetSyncStatus(curtaskGames_, curSID_);

	static size_t count = 0;
	size_t cnt = curtaskGames_.size();
	if( count != cnt )
	{
		count = cnt;
		wndListTaskGames_.SetItemCount(count);
	}
	else
	{
		wndListTaskGames_.RedrawCurPageItems();
	}
};

void CDlgSyncTask::OnReConnect()
{
	localTreeImageList_.DeleteImageList();
	syncTreeImageList_.DeleteImageList();

	i8desk::GetDataMgr().GetClasses().clear();
	i8desk::GetDataMgr().GetGames().clear();
	i8desk::GetDataMgr().GetSyncTasks().clear();
	i8desk::GetDataMgr().GetSyncGames().clear();
};

void CDlgSyncTask::OnAsyncData()
{
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetClasses());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetGames());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetSyncTasks());
	i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetSyncGames());
};

void CDlgSyncTask::OnDataComplate()
{
	// 初始化树的图片
	InitlocalTreeImg(localTreeImageList_);
	wndTreeLocalGame_.SetImageList(&localTreeImageList_, TVSIL_NORMAL);

	InitsyncTreeImg(syncTreeImageList_);
	wndTreeSyncTask_.SetImageList(&syncTreeImageList_, TVSIL_NORMAL);

	// 初始化树的数据
	InitTreeCtrlLocal(wndTreeLocalGame_);
	InitTreeTaskCtrl(wndTreeSyncTask_, i8desk::GetDataMgr().GetSyncTasks());

	// 注册窗口
	wndMgr_.Register(wndListLocalGames_.GetDlgCtrlID(), &wndListLocalGames_);
	wndMgr_.Register(wndListTaskGames_.GetDlgCtrlID(), &wndListTaskGames_);

	// 设置当前窗口
	wndMgr_.SetDestWindow(&wndListLocalGames_);
	wndMgr_[wndListLocalGames_.GetDlgCtrlID()];

	// 设置选中的树
	wndTreeLocalGame_.SelectItem(wndTreeLocalGame_.GetRootItem());
};



void CDlgSyncTask::_ShowView()
{
	using std::tr1::bind;
	using namespace std::tr1::placeholders;
	using i8desk::data_helper::GameTraits;
	using i8desk::data_helper::ClassTraits;


	switch(viewStatus_)
	{
	case LocalGames:
		{
			HTREEITEM localhItem = wndTreeLocalGame_.GetSelectedItem();
			HTREEITEM hRoot = wndTreeLocalGame_.GetParentItem(localhItem);

			const GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();
			localGames_.clear();
			if( hRoot != 0 )
			{
				stdex::tString CID = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocalGame_.GetItemData(localhItem))->CID;
				i8desk::business::Game::LocalFilter(games, localGames_, CID);
			}
			else
			{
				i8desk::business::Game::LocalAllFilter(games, localGames_);
			}

			wndListLocalGames_.SetItemCount(localGames_.size());
		}

		break;
	case Tasks:
		{
			curtaskGames_.clear();
			i8desk::business::SyncTask::TaskFilter(curtaskGames_, curSID_);
		}
		break;
	default:
		assert(0);
		break;
	}
	PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
}


void CDlgSyncTask::_ShowButton()
{
	switch(viewStatus_)
	{
	case LocalGames:
		wndListLocalGames_.ShowWindow(SW_SHOW);
		wndListTaskGames_.ShowWindow(SW_HIDE);
		wndBtnDelSyncGame_.ShowWindow(SW_SHOW);
		wndBtnDelSyncGameToSID_.ShowWindow(SW_HIDE);
		wndBtnAddSyncGame_.ShowWindow(SW_SHOW);
		break;
	case Tasks:
		wndListLocalGames_.ShowWindow(SW_HIDE);
		wndListTaskGames_.ShowWindow(SW_SHOW);
		wndBtnDelSyncGame_.ShowWindow(SW_HIDE);
		wndBtnDelSyncGameToSID_.ShowWindow(SW_SHOW);
		wndBtnAddSyncGame_.ShowWindow(SW_HIDE);
		break;
	default:
		assert(0);
		break;
	}

}

void CDlgSyncTask::_Cummulate()
{
	i8desk::business::Game::CummulateLocalGame(localGameCnt_);

	PostMessage(WM_CUMMULATE);
}

CListCtrl &CDlgSyncTask::_GetCurList()
{
	switch(viewStatus_)
	{
	case LocalGames:
		return wndListLocalGames_;
		break;
	case Tasks:
		return wndListTaskGames_;
		break;
	default:
		assert(0);
		return wndListLocalGames_;
		break;
	}
}



BOOL CDlgSyncTask::OnInitDialog()
{
	__super::OnInitDialog();

	INIT_EASYSIZE;



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
	wndBtnSyncTaskAdd_.SetImages(add[0], add[1], add[2]);

	HBITMAP del[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Del_Press.png"))
	};
	wndBtnSyncTaskDelete_.SetImages(del[0], del[1], del[2]);

	HBITMAP modify[] = 
	{
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Hover.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Btn_Modify_Press.png"))
	};
	wndBtnSyncTaskModify_.SetImages(modify[0], modify[1], modify[2]);


	wndBtnLocalGameMgr_.SetImages(
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")),
		i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/Left_Title_bg.png")));
	wndBtnSyncTaskMgr_.SetImages(
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

	InitLocalList(wndListLocalGames_, localGames_);
	InitTaskList(wndListTaskGames_, curtaskGames_);

	wndListLocalGames_.ShowWindow(SW_SHOW);
	wndListTaskGames_.ShowWindow(SW_HIDE);

	wndTreeSyncTask_.SelectItem(wndTreeSyncTask_.GetRootItem());

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgSyncTask::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	memDC.GetDC().FillSolidRect(rcClient, RGB(255, 255, 255));

	i8desk::ui::DrawWorkFrame(memDC, rcClient, leftArea_, rightArea_);

	// 右边工作区内容边框
	CRect rcWork;
	wndListLocalGames_.GetWindowRect(rcWork);
	i8desk::ui::DrawFrame(this, memDC, rcWork, workOutLine_);
}

BOOL CDlgSyncTask::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

void CDlgSyncTask::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;

	wndMgr_.UpdateSize();
	if( !::IsWindow(wndTreeLocalGame_.GetSafeHwnd()) )
		return;

	wndTreeLocalGame_.Invalidate();
	wndTreeSyncTask_.Invalidate();
	wndListLocalGames_.Invalidate();
	wndListTaskGames_.Invalidate();

	wndBtnQuery_.Invalidate();
	wndEditQuery_.Invalidate();

	wndBtnTaskStart_.Invalidate();
	wndBtnTaskPause_.Invalidate();
	wndBtnSyncTaskAdd_.Invalidate();
	wndBtnSyncTaskModify_.Invalidate();
	wndBtnSyncTaskDelete_.Invalidate();
	wndBtnNotifySync_.Invalidate();
	wndBtnLocalGameMgr_.Invalidate();
	wndBtnSyncTaskMgr_.Invalidate();
}

void CDlgSyncTask::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

}

void CDlgSyncTask::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CDlgSyncTask::OnTimer(UINT_PTR id)
{
	i8desk::AsyncDataHelper(std::tr1::bind(&CDlgSyncTask::_Cummulate, this));
}

void CDlgSyncTask::OnDestroy()
{
	CDialog::OnDestroy();
}


void CDlgSyncTask::OnBnClickedButtonSynctaskAdd()
{
	CDlgSyncTaskAddorCopy dlg;
	if( dlg.DoModal() == IDOK )
	{
		InitTreeTaskCtrl(wndTreeSyncTask_, i8desk::GetDataMgr().GetSyncTasks());
	}
}

void CDlgSyncTask::OnBnClickedButtonSynctaskDelete()
{	
	typedef i8desk::data_helper::SyncTaskTraits SyncTasks;
	HTREEITEM HSelItem = wndTreeSyncTask_.GetSelectedItem();
	if(HSelItem == NULL)
	{	
		CMessageBox msgDlg(_T("提示"), _T("请选择要删除的同步任务"));
		msgDlg.DoModal();
		return;
	}


	SyncTasks::ElementType *val = 0;
	val = reinterpret_cast<SyncTasks::ElementType*>(wndTreeSyncTask_.GetItemData(HSelItem));

	if( val == 0 )
		return;

	CMessageBox msgDlg(val->Name, _T("确定删除同步任务吗?"));
	if( msgDlg.DoModal() == IDOK )
	{		
		stdex::tString taskname = i8desk::business::SyncTask::GetTaskNameBySID(curSID_);

		if(!i8desk::business::SyncTask::DelSyncTask(curSID_))
			return;

		SyncTaskOperate(StopTask, 0, curSID_, taskname, false);

		InitTreeTaskCtrl(wndTreeSyncTask_, i8desk::GetDataMgr().GetSyncTasks());
		wndTreeSyncTask_.SelectItem(wndTreeSyncTask_.GetRootItem());

		_ShowView();
	}
}

void CDlgSyncTask::OnBnClickedButtonSynctaskModify()
{
	typedef i8desk::data_helper::SyncTaskTraits SyncTasks;

	HTREEITEM hSelItem = wndTreeSyncTask_.GetSelectedItem();
	if( hSelItem == NULL )
		return;

	SyncTasks::ElementType *val = reinterpret_cast<SyncTasks::ElementType*>(wndTreeSyncTask_.GetItemData(hSelItem));

	if( val == 0 )
		return;

	if( !i8desk::business::Server::IsServerExsit(val->SvrID) )
	{
		CMessageBox box(_T("提示"), _T("该同步任务所在服务器已被删除,请删除该同步任务"));
		box.DoModal();
		return;
	}

	CDlgSyncTaskInfoEx dlg(ismodify, val);
	if( dlg.DoModal() == IDOK )
	{
		InitTreeTaskCtrl(wndTreeSyncTask_, i8desk::GetDataMgr().GetSyncTasks());
		_ShowView();
	}

}

LRESULT CDlgSyncTask::OnCummulate(WPARAM, LPARAM)
{
	using i8desk::data_helper::ClassTraits;
	static std::map<stdex::tString, size_t> localGameCntTmp;

	stdex::tOstringstream os;
	for(HTREEITEM localTree = wndTreeLocalGame_.GetChildItem(wndTreeLocalGame_.GetRootItem());
		localTree != NULL; localTree = wndTreeLocalGame_.GetNextSiblingItem(localTree))
	{

		os.str(_T(""));
		stdex::tString text = wndTreeLocalGame_.GetItemText(localTree);
		text = text.substr(0, text.find(_T('(')));

		stdex::tString CID = reinterpret_cast<ClassTraits::ElementType *>(wndTreeLocalGame_.GetItemData(localTree))->CID;

		if( localGameCntTmp[CID] != localGameCnt_[CID] )
		{
			os << text << _T("(") << localGameCnt_[CID] << _T(")");
			wndTreeLocalGame_.SetItemText(localTree, os.str().c_str());

			localGameCntTmp[CID] = localGameCnt_[CID];
		}

		localGameCnt_[CID] = 0;
	}

	return TRUE;
}


void CDlgSyncTask::OnTvnSelchangedTreeLocalGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == 0 || pNMTreeView->action == 4096 )
	{
		wndTreeLocalGame_.SelectItem(0);
		return;
	}


	viewStatus_ = LocalGames;
	i8desk::GetRealDataMgr().UnRegister(MASK_PARSE_SYNCTASKSTATUS );

	wndTreeSyncTask_.SelectItem(0);

	wndMgr_[wndListLocalGames_.GetDlgCtrlID()];

	_ShowView();
	_ShowButton();

}

void CDlgSyncTask::OnTvnSelchangedTreeSyncTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	if( pNMTreeView->itemNew.hItem == 0 )
		return;

	curTreeItem_ = pNMTreeView->itemNew.hItem;

	viewStatus_ = Tasks;

	i8desk::GetRealDataMgr().Register(
		std::tr1::bind(&i8desk::ui::BaseWnd::UpdateCallback, this), MASK_PARSE_SYNCTASKSTATUS);

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	if( wndTreeSyncTask_.ItemHasChildren(hTreeItem) )
		hTreeItem = wndTreeSyncTask_.GetChildItem(hTreeItem);

	typedef i8desk::data_helper::SyncTaskTraits::ElementType SyncTaskType;
	SyncTaskType *val = reinterpret_cast<SyncTaskType *>(wndTreeSyncTask_.GetItemData(hTreeItem));
	if( val != 0 )
	{
		curSID_ = val->SID;
	}


	wndTreeLocalGame_.SelectItem(0);
	wndMgr_[wndListTaskGames_.GetDlgCtrlID()];

	_ShowView();
	_ShowButton();

}

void CDlgSyncTask::OnLvnColumnclickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListLocalGames_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(localGames_.begin(), localGames_.end(), Compare<LocalGames>(pNMLV->iSubItem, LocalCompare[pNMLV->iSubItem]));
	LocalCompare.set(pNMLV->iSubItem, !LocalCompare[pNMLV->iSubItem]);
	wndListLocalGames_.RedrawCurPageItems();

	wndListLocalGames_.SetSortArrow(pNMLV->iSubItem, LocalCompare[pNMLV->iSubItem]);
}

void CDlgSyncTask::OnLvnColumnclickListSyncTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListTaskGames_.GetItemCount() == 0 )
		return;

	CWaitCursor wc;
	std::stable_sort(curtaskGames_.begin(), curtaskGames_.end(), Compare<Tasks>(pNMLV->iSubItem, TaskCompare[pNMLV->iSubItem]));
	TaskCompare.set(pNMLV->iSubItem, !TaskCompare[pNMLV->iSubItem]);
	wndListTaskGames_.RedrawCurPageItems();

	wndListTaskGames_.SetSortArrow(pNMLV->iSubItem, TaskCompare[pNMLV->iSubItem]);
}


void CDlgSyncTask::OnLvnGetdispinfoListLocalGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;
	if( static_cast<size_t>(itemIndex) >= localGames_.size() )
		return;

	const i8desk::data_helper::GameTraits::ValueType &val = localGames_[itemIndex];

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
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgSyncTask::OnLvnGetdispinfoListTaskGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	LV_ITEM *pItem = &(pDispInfo)->item;
	int itemIndex = pItem->iItem;

	if( static_cast<size_t>(itemIndex) >= curtaskGames_.size() )
		return;

	const SyncTaskStatusTraits::ValueType &val = curtaskGames_[itemIndex];

	static stdex::tString text;
	if( pItem->mask & LVIF_TEXT )
	{
		switch(pItem->iSubItem)
		{
		case MASK_TASK_GAMENAME:
			text = val->GameName;
			break;
		case MASK_TASK_GID:
			stdex::ToString(val->GID, text);
			break;
		case MASK_TASK_CLASS:
			text = i8desk::business::Game::GetClass(val->GID);
			break;
		case MASK_TASK_STATUS:
			{
				static COLORREF Red		= RGB(255, 0, 0);
				static COLORREF Green	= RGB(0, 255, 0);
				static COLORREF Blue	= RGB(0, 0, 255);
				static COLORREF Black	= RGB(0, 0, 0);
				if(val->UpdateState == i8desk::GameError)
				{
					stdex::tOstringstream os;
					os << i8desk::business::SyncTask::GetStateDesc(val) << _T("(") << val->Error << _T(")");
					text = os.str();
				}
				else
					text = i8desk::business::SyncTask::GetStateDesc(val);
				switch(val->UpdateState)
				{
				case i8desk::GameComplate:
					syncgameStatus->SetTextColor(itemIndex, MASK_TASK_STATUS, Green); 
					break;
				case i8desk::GameUnComplate:
					syncgameStatus->SetTextColor(itemIndex, MASK_TASK_STATUS, Black); 
					break;
				case i8desk::GameRunning:
					syncgameStatus->SetTextColor(itemIndex, MASK_TASK_STATUS, Blue); 
					break;
				case i8desk::GamePaused:
					syncgameStatus->SetTextColor(itemIndex, MASK_TASK_STATUS, Blue); 
					break;
				case i8desk::GameError:
					syncgameStatus->SetTextColor(itemIndex, MASK_TASK_STATUS, Red); 
					break;
				}
			}
			break;
		case MASK_TASK_SVRDIR:
			stdex::ToString(val->SvrDir, text);
			break;
		case MASK_TASK_NODEDIR:
			stdex::ToString(val->NodeDir, text);
			break;
		case MASK_TASK_TRANSFERRATE:
			if(val->UpdateState == i8desk::GameRunning)
				i8desk::FormatSize(val->TransferRate, text);
			else
				text = _T("--");
			break;
		case MASK_TASK_UPDATESIZE:
			i8desk::FormatSize(val->UpdateSize, text);
			break;
		case MASK_TASK_PROGRESS:
			{
				size_t progress = 0;

				if(val->UpdateSize != 0)
				{
					if( val->BytesTransferred  > val->UpdateSize )
						progress = size_t(val->BytesTransferred * 1.1 * 100 / val->UpdateSize);
					else
						progress = size_t(val->BytesTransferred * 1.0 * 100 / val->UpdateSize);
				}

				if(val->UpdateState == i8desk::GameComplate)
					progress = 100;

				if( val->UpdateState == i8desk::GameRunning 
					|| val->UpdateState == i8desk::GamePaused 
					|| val->UpdateState == i8desk::GameComplate)
					syncgameProgress->SetProgress(itemIndex, progress);
				else
					syncgameProgress->SetProgress(itemIndex, 0);

				stdex::ToString(progress, text, 2);
				text += _T("%");
			}
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	}
}

void CDlgSyncTask::OnLvnOdcachehintListLocalSyncGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);
	*pResult = 0;

	size_t start	= pCacheHint->iFrom;
	size_t stop		= pCacheHint->iTo;

	for(size_t i = start; i <= stop; ++i)
		localGames_[i]->Icon = i8desk::business::Game::GetIcon(localGames_[i]);
}


void CDlgSyncTask::OnBnClickedButtonSyncTaskStart()
{
	if( curtaskGames_.empty() )
	{
		CMessageBox msgDlg(_T("提示"), _T("该同步任务中没有同步游戏, 请手动添加") );
		msgDlg.DoModal();
		return;
	}

	stdex::tString taskname = i8desk::business::SyncTask::GetTaskNameBySID(curSID_);

	stdex::tOstringstream msg;
	msg << _T("确定开始同步任务[ ")  << taskname << _T(" ]吗?");
	CMessageBox msgDlg(_T("提示"), msg.str().c_str() );
	if( msgDlg.DoModal() == IDOK )
	{
		bool isPaused = i8desk::business::SyncTask::IsPaused(curSID_);

		i8desk::business::SyncTask::SetSyncTaskStatus(curSID_, i8desk::TaskStarted);

		HTREEITEM hitem = wndTreeSyncTask_.GetSelectedItem();
		wndTreeSyncTask_.SetItemImage(hitem, 0, 0);
		SyncTaskOperate(!isPaused ? StartTask : ResumeTask, 0, curSID_, taskname, false);
	}
}

void CDlgSyncTask::OnBnClickedButtonSyncTaskPause()
{
	if( curtaskGames_.empty() )
		return;

	stdex::tString taskname = i8desk::business::SyncTask::GetTaskNameBySID(curSID_);
	stdex::tOstringstream msg;
	msg << _T("确定暂停同步任务[ ") << taskname << _T(" ]吗?");
	CMessageBox msgDlg(_T("提示"), msg.str().c_str() );
	if( msgDlg.DoModal() == IDOK )
	{
		HTREEITEM hitem = wndTreeSyncTask_.GetSelectedItem();
		wndTreeSyncTask_.SetItemImage(hitem, 1, 1);

        i8desk::business::SyncTask::SetSyncTaskStatus(curSID_, i8desk::TaskPaused);
		SyncTaskOperate(SuspendTask, 0, curSID_, taskname, false);

	}
}


void CDlgSyncTask::ONBnClickedButtonSearch()
{
	CString text;
	wndEditQuery_.GetWindowText(text);

	i8desk::data_helper::GameTraits::VectorType tmp;
	i8desk::data_helper::SyncTaskStatusTraits::VectorType tasktmp;

	CListCtrl *curList = 0;
	switch(viewStatus_)
	{
	case LocalGames:
		i8desk::business::Game::FindGame(text, localGames_, tmp);
		curList = &wndListLocalGames_;
		break;
	case Tasks:
		i8desk::business::SyncTask::FindSync(text, curtaskGames_, tasktmp);
		curList = &wndListTaskGames_;
		break;
	default:
		assert(0);
		break;
	}

	if( curList == 0 )
		return;

	if( viewStatus_ != Tasks )
	{
		localGames_.swap(tmp);
		curList->SetItemCount(localGames_.size());
	}
	else
	{
		/*curtaskGames_.swap(tasktmp);
		curList->SetItemCount(curtaskGames_.size());*/

		curList->SetItemState(-1, 0, -1);

		for(size_t i = 0; i != curtaskGames_.size(); ++i)
		{
			struct Find
			{
				const long gid_;
				Find(const long gid)
					: gid_(gid)
				{}
				bool operator()(const i8desk::data_helper::SyncTaskStatusTraits::ValueType &val) const
				{
					return gid_ == val->GID;
				}
			};
			if( std::find_if(tasktmp.begin(), tasktmp.end(), Find(curtaskGames_[i]->GID)) != tasktmp.end() )
			{
				curList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
	}
}

void CDlgSyncTask::OnQueryTextChanged()
{

}

BOOL CDlgSyncTask::PreTranslateMessage(MSG* pMsg)
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
			PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
			return FALSE;
		}
	}

	return FALSE;
}

void CDlgSyncTask::OnEnChangeEditDownloadSearch()
{
	_ShowView();
	ONBnClickedButtonSearch();
}


void CDlgSyncTask::ONBnClickedButtonNotifySync()
{
	std::vector<long> Gids;
	std::vector<stdex::tString> Sids;
	typedef i8desk::data_helper::SyncTaskTraits::MapType SyncMap;
	SyncMap synctasks = i8desk::GetDataMgr().GetSyncTasks();

	int nSel = -1;
	switch(viewStatus_)
	{
	case LocalGames:
		while((nSel = _GetCurList().GetNextItem(nSel, LVIS_SELECTED)) != -1)
		{
			assert(nSel < (int)localGames_.size());
			Gids.push_back(localGames_[nSel]->GID); 
		}

		std::for_each(synctasks.begin(), synctasks.end(), [&Sids](const SyncMap::value_type &val)
		{
			Sids.push_back(val.first);
		});

		break;
	case Tasks:
		while((nSel = _GetCurList().GetNextItem(nSel, LVIS_SELECTED)) != -1)
		{
			assert(nSel < (int)curtaskGames_.size());
			Gids.push_back(curtaskGames_[nSel]->GID);
		}

		Sids.push_back(curSID_);
	}

	if(Sids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步任务，请选择要通知的同步任务"));
		msgDlg.DoModal();
		return;
	}

	if(Gids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步游戏，请选择要通知的同步游戏"));
		msgDlg.DoModal();
		return;
	}


	for(auto iter = Sids.cbegin(); iter != Sids.cend(); ++iter)
	{
		stdex::tString SID = *iter;
		stdex::tString taskname = i8desk::business::SyncTask::GetTaskNameBySID(SID);
		for(auto giter = Gids.cbegin(); giter != Gids.cend(); ++giter)
		{
			SyncTaskOperate(NotifySync, *giter, SID, taskname, false);
		}
	}

}

void CDlgSyncTask::ONBnClickedButtonDelSyncGame()
{
	std::vector<long> Gids;
	std::vector<stdex::tString> Sids;
	typedef i8desk::data_helper::SyncTaskTraits::MapType SyncMap;
	SyncMap synctasks = i8desk::GetDataMgr().GetSyncTasks();

	int nSel = -1;

	while((nSel = _GetCurList().GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		assert(nSel < (int)localGames_.size());
		Gids.push_back(localGames_[nSel]->GID); 
	}

	std::for_each(synctasks.begin(), synctasks.end(), [&Sids](const SyncMap::value_type &val)
	{
		Sids.push_back(val.first);
	});



	if(Sids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步任务，请选择要删除的同步任务"));
		msgDlg.DoModal();
		return;
	}

	if(Gids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步游戏，请选择要删除的同步游戏"));
		msgDlg.DoModal();
		return;
	}


	CDlgSyncTaskDelInfo info(Gids, Sids, viewStatus_ == Tasks);
	if( info.DoModal() == IDOK )
	{
		OnRealDataUpdate();
		_ShowView();
	}

}

void CDlgSyncTask::ONBnClickedButtonDelSyncGameToSID()
{
	std::vector<long> Gids;
	std::vector<stdex::tString> Sids;
	typedef i8desk::data_helper::SyncTaskTraits::MapType SyncMap;
	SyncMap synctasks = i8desk::GetDataMgr().GetSyncTasks();

	int nSel = -1;

	while((nSel = _GetCurList().GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		assert(nSel < (int)curtaskGames_.size());
		Gids.push_back(curtaskGames_[nSel]->GID);
	}

	Sids.push_back(curSID_);


	if(Sids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步任务，请选择要删除的同步任务"));
		msgDlg.DoModal();
		return;
	}

	if(Gids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步游戏，请选择要删除的同步游戏"));
		msgDlg.DoModal();
		return;
	}


	CDlgSyncTaskDelInfo info(Gids, Sids, viewStatus_ == Tasks);
	if( info.DoModal() == IDOK )
	{
		OnRealDataUpdate();
		_ShowView();
	}

}

void CDlgSyncTask::ONBnClickedButtonAddSyncGame()
{
	std::vector<long> Gids;
	std::vector<stdex::tString> Sids;
	typedef i8desk::data_helper::SyncTaskTraits::MapType SyncMap;
	SyncMap synctasks = i8desk::GetDataMgr().GetSyncTasks();

	int nSel = -1;

	while((nSel = _GetCurList().GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		assert(nSel < (int)localGames_.size());
		Gids.push_back(localGames_[nSel]->GID); 
	}

	std::for_each(synctasks.begin(), synctasks.end(), [&Sids](const SyncMap::value_type &val)
	{
		Sids.push_back(val.first);
	});

	if(Sids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步任务，请选择要添加的同步任务"));
		msgDlg.DoModal();
		return;
	}

	if(Gids.empty())
	{
		CMessageBox msgDlg(_T("提示"), _T("没有选中的同步游戏，请选择要添加的同步游戏"));
		msgDlg.DoModal();
	}

	CMessageBox msgDlg(_T("提示"), _T("确认添加选中游戏到所有同步任务吗?"));
	if(msgDlg.DoModal() == IDOK)
	{
		for(auto iter = Sids.begin(); iter != Sids.end(); ++iter)
		{
			for(auto giditer = Gids.begin(); giditer != Gids.end(); ++giditer )
			{
				i8desk::business::SyncTask::AddSyncGame(*iter, *giditer);
			}
		}

		_ShowView();
	}
}


void CDlgSyncTask::OnMenuLocalNotify()
{
	ONBnClickedButtonNotifySync();
}

void CDlgSyncTask::OnMenuLocalAddGame()
{
	ONBnClickedButtonAddSyncGame();
}

void CDlgSyncTask::OnMenuLocalDelGame()
{
	ONBnClickedButtonDelSyncGame();
}


void CDlgSyncTask::OnMenuSyncTaskNotify()
{
	ONBnClickedButtonNotifySync();
}

void CDlgSyncTask::OnMenuSyncTaskDelGame()
{
	ONBnClickedButtonDelSyncGameToSID();
}

void CDlgSyncTask::OnMenuSyncTreeDel()
{
	OnBnClickedButtonSynctaskDelete();
}

void CDlgSyncTask::OnMenuSyncTreeCopy()
{

	using i8desk::data_helper::SyncTaskTraits;
	const SyncTaskTraits::MapType &syncTasks = i8desk::GetDataMgr().GetSyncTasks();
	SyncTaskTraits::MapConstIterator iter = syncTasks.find(curSID_.c_str());

	if( iter == syncTasks.end())
		return;

	if( !i8desk::business::Server::IsServerExsit(iter->second->SvrID) )
	{
		CMessageBox box(_T("提示"), _T("该同步任务所在服务器已被删除,请删除该同步任务"));
		box.DoModal();
		return;
	}

	CDlgSyncTaskInfoEx dlg(iscopy, (iter->second).get());
	if( dlg.DoModal() == IDOK )
	{
		InitTreeTaskCtrl(wndTreeSyncTask_, i8desk::GetDataMgr().GetSyncTasks());
		_ShowView();
	}

}

void CDlgSyncTask::OnNMRClickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_SYNC_LOCALGAME);
	CMenu *subMenu = menu.GetSubMenu(0);

	CPoint point;
	::GetCursorPos(&point);

	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CDlgSyncTask::OnNMRClickListSyncTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu menu;
	menu.LoadMenu(IDR_MENU_SYNC_SYNCTASK);
	CMenu *subMenu = menu.GetSubMenu(0);

	CPoint point;
	::GetCursorPos(&point);

	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

LRESULT CDlgSyncTask::OnApplySelect(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 1 )
	{
		wndTreeSyncTask_.SelectItem(wndTreeSyncTask_.GetRootItem());
	}
	else if( wParam == 2 )
	{
	}	

	return TRUE;
}

void CDlgSyncTask::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
	*pResult = 0;
}

void CDlgSyncTask::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow != NULL && (pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_DOWN || (pLVKeyDow->wVKey == 'A' && (pLVKeyDow->flags & MK_CONTROL))))
	{
		PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);
	}
	*pResult = 0;
}

stdex::tString CDlgSyncTask::GetSizeText(ULONGLONG totalSize)
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

LRESULT CDlgSyncTask::OnReportSelectState(WPARAM wParam, LPARAM lParam)
{
	stdex::tString sInfoEx;
	switch (viewStatus_)
	{
	case LocalGames:
		sInfoEx = GetSizeText(GetSelectedLocalGameTotalSize());
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListLocalGames_.GetSelectedCount());
		break;
	case Tasks:
		sInfoEx = GetSizeText(GetSelectedTaskGameTotalSize());
		GetParent()->SendMessage(i8desk::ui::WM_UPDATE_SELECT_STATE, reinterpret_cast<WPARAM>(&sInfoEx), wndListTaskGames_.GetSelectedCount());
		break;
	default:
		break;
	}
	return 1;
}

void CDlgSyncTask::OnNMRClickTreeSynctaskTasks(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt = GetCurrentMessage()->pt;   //获取当前鼠标点击消息的坐标点

	wndTreeSyncTask_.ScreenToClient(&pt);   //将鼠标的屏幕坐标，转换成树形控件的客户区坐标

	UINT uFlags = 0;

	HTREEITEM hItem = wndTreeSyncTask_.HitTest(pt, &uFlags);//然后做点击测试

	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))//如果点击的位置是在节点位置上面
	{
		wndTreeSyncTask_.SelectItem(hItem);
	}

	CMenu menu;
	menu.LoadMenu(IDR_MENU_SYNCTASK_TREETASK);
	CMenu *subMenu = menu.GetSubMenu(0);

	CPoint point;
	::GetCursorPos(&point);

	subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);


}

ULONGLONG CDlgSyncTask::GetSelectedLocalGameTotalSize()
{
	ULONGLONG totalSize = 0;
	int nSel = -1;
	while( (nSel = wndListLocalGames_.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		totalSize += localGames_[nSel]->Size * 1024LL;
	}
	return totalSize;
}

ULONGLONG CDlgSyncTask::GetSelectedTaskGameTotalSize()
{
	std::vector<ULONG> gids;
	gids.resize(wndListTaskGames_.GetSelectedCount());
	int index = 0;
	int nSel = -1;
	while( (nSel = wndListTaskGames_.GetNextItem(nSel, LVIS_SELECTED)) != -1 )
	{
		if (nSel < int(curtaskGames_.size()))
		{
			gids[index] = curtaskGames_[nSel]->GID;
		}
		++index;
	}
	return i8desk::business::Game::GetGameSize(gids);
}

