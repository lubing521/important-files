// DlgDownloadSomeAdd.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgDownloadSomeAdd.h"

#include <fstream>
#include <set>

#include "../../../../include/Extend STL/StringAlgorithm.h"
#include "../../../../include/win32/Filesystem/FileOperator.hpp"
#include "../../../../include/Utility/utility.h"
#include "../../../../include/ui/ImageHelpers.h"
#include "../../../../include/UI/IconHelper.h"
#include "../UI/UIHelper.h"
#include "../Business/GameBusiness.h"
#include "../Business/ClassBusiness.h"
#include "../Business/VDiskBusiness.h"
#include "../Business/AreaBusiness.h"
#include "../Business/SyncTaskBusiness.h"

#include "../GridListCtrlEx/CGridColumnTraitCombo.h"
#include "../Misc.h"

#include "../Network/IOService.h"
#include "../ManagerInstance.h"
#include "../ui/Skin/SkinMgr.h"
#include "../MessageBox.h"
#include "../DlgWaitMsg.h"

#include <bitset>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

enum 
{ 
	MASK_SOMEADD_PATH = 1, MASK_SOMEADD_STATUS, MASK_SOMEADD_RESULT, MASK_SOMEADD_GID
};

// 三层排序
std::bitset<MASK_SOMEADD_GID + 1>	SomeAddCompare;




enum { WM_UPDATE = WM_USER + 100 };

// CDlgDownloadSomeAdd 对话框

IMPLEMENT_DYNAMIC(CDlgDownloadSomeAdd, CNonFrameChildDlg)

CDlgDownloadSomeAdd::CDlgDownloadSomeAdd(const stdex::tString &classes, CWnd* pParent /*=NULL*/)
: CNonFrameChildDlg(CDlgDownloadSomeAdd::IDD, pParent)
, searchDir_(_T(""))
, searchDeep_(1)
, curClass_(classes)
, bCanClick_(false)

{
	stop_.Create();
}

CDlgDownloadSomeAdd::~CDlgDownloadSomeAdd()
{
}

void CDlgDownloadSomeAdd::DoDataExchange(CDataExchange* pDX)
{
	CNonFrameChildDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DOWNLOAD_ADD_SVRPATH, wndEditSearch_);
	DDX_Text(pDX, IDC_EDIT_DOWNLOAD_ADD_SVRPATH, searchDir_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_ADD_BROWSE_DIR, wndBtnSearch_);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_ADD_EXECUTE, wndBtnStart_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_ADD_DEEP, wndComboDeep_);
	DDX_Text(pDX, IDC_COMBO_DOWNLOAD_ADD_DEEP, searchDeep_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_ADD_CLASS, wndComboClass_);
	DDX_Control(pDX, IDC_STATIC_ADD_PROGRESS, wndProgress_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_ADD_RESULT, wndListResult_);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD_ADD_RUNTYPE, wndListRuntype_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_ADD_STATUS, wndEditTip_);
	DDX_Control(pDX, IDC_COMBO_DOWNLOAD_RERSULTS, wndComboResulte_);

	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_SET, wndSet_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_ADD_SVRPATH, wndSearchDir_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_ADD_DEEP, wndDeep_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_ADD_CLASS, wndClass_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_RESULT, wndResulte_);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_RUNTYPE, wndRuntype_);

	DDX_Control(pDX, IDOK, wndBtnOk_);
	DDX_Control(pDX, IDCANCEL, wndBtnCancel_);
}


BEGIN_MESSAGE_MAP(CDlgDownloadSomeAdd, CNonFrameChildDlg)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_ADD_BROWSE_DIR, &CDlgDownloadSomeAdd::OnBnClickedButtonDownloadAddBrowseDir)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_ADD_EXECUTE, &CDlgDownloadSomeAdd::OnBnClickedButtonDownloadAddExecute)
	ON_BN_CLICKED(IDOK, &CDlgDownloadSomeAdd::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgDownloadSomeAdd::OnBnClickedCancel)

	ON_MESSAGE(WM_UPDATE, &CDlgDownloadSomeAdd::OnMessage)
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_DOWNLOAD_ADD_RESULT, &CDlgDownloadSomeAdd::OnLvnGetdispinfoListDownloadAddResult)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DOWNLOAD_ADD_RESULT, &CDlgDownloadSomeAdd::OnLvnColumnclickList)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DOWNLOAD_ADD_RESULT, &CDlgDownloadSomeAdd::OnNMClickList)
	ON_CBN_KILLFOCUS(IDC_COMBO_DOWNLOAD_RERSULTS, &CDlgDownloadSomeAdd::OnCbnKillfocusDownloadResults)
	ON_CBN_CLOSEUP(IDC_COMBO_DOWNLOAD_RERSULTS, &CDlgDownloadSomeAdd::OnCbnCloseupDownloadResults)
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()


namespace
{

	const stdex::tString ButtonText[] = 
	{
		_T("执行搜索"),
		_T("停止搜索")
	};

	// 是否正在搜索
	bool IsWorking = false;

	// 更新状态
	enum { Start = 0, Running, Stop };
	enum { NO_MATCH = 0, ACCURATE_MATCH, BEST_MATCH, REP_MATCH  };

	struct MatchItem
	{
		int				MatchType;	// 匹配类型 0: 未匹配  1：精确匹配  2: 最佳匹配 3：重复匹配
		int				macthSum;	// 匹配个数
		stdex::tString	dir;		// 匹配路径
		stdex::tString	exe;		// 用于不能匹配的游戏,记录用于选择的主程序.

		i8desk::data_helper::GameTraits::VectorType matchGames;	// 匹配中的游戏
		i8desk::data_helper::GameTraits::ValueType  game;		// 用于匹配重复的选择
	};

	typedef std::vector<MatchItem> MatchItemList;
	MatchItemList MatchItems;


	struct CheckFile
	{
		async::thread::AutoEvent &event_;
		size_t deep_;

		CheckFile(async::thread::AutoEvent &event, size_t deep)
			: event_(event)
			, deep_(deep)
		{}

		template < typename FindFileT >
		bool operator()(const FindFileT &find, size_t deep)
		{
			if( deep_ != deep )
				return false;

			if( ::WaitForSingleObject(event_, 0) == WAIT_OBJECT_0 )
				return false;

			if( find.IsSystem() || find.IsHidden() || !find.IsDirectory() )
				return false;	
			return true;
		}
	};

	struct SearchMatchItem
	{
		template < typename StringT >
		void operator()(const StringT &dir)
		{
			MatchItem item;
			item.macthSum = -1;
			item.MatchType = 0;
			item.dir = dir;

			MatchItems.push_back(item);
		}
	};


	i8desk::uint32 GetGameIDFromIdx(const stdex::tString &dir)
	{
		static const unsigned long MAX_IDX_FILE_SIZE = 1024 * 1024 * 4;

		const stdex::tString path = dir + _T("\\i8desk.idx");
		std::ifstream in(path.c_str());

		if( !in )
			return 0;
		in.seekg(24L);
		DWORD nXmlSize = 0;
		in >> nXmlSize;

		if( !in )
			return 0;
		if( nXmlSize >= MAX_IDX_FILE_SIZE || 
			nXmlSize == 0 )
			return 0;

		if( !in )
			return 0;
		in.seekg(30L);
		WORD nCrcBlock = 0;
		in >> nCrcBlock;

		if( !in )
			return 0;
		std::vector<char> buf(nXmlSize);
		in.seekg((nCrcBlock + 1) * 32L);
		in.read(&buf[0], nXmlSize);

		std::string IDXBuf = CA2A(&buf[0], CP_UTF8);

		MSXML2::IXMLDOMDocumentPtr xmlDoc;
		xmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));

		if( !xmlDoc->loadXML(IDXBuf.c_str()) ) 
			return 0;

		MSXML2::IXMLDOMElement *pRoot;
		if( FAILED(xmlDoc->get_documentElement(&pRoot)))
			return 0;

		MSXML2::IXMLDOMElementPtr root(pRoot);

		unsigned long gid = 0;

		try
		{
			gid = root->getAttribute(_T("g"));
		} 
		catch(...)
		{
			TRACE(_T("从索引文件查找游戏编号时没有发现相应属性.idx=%s\n"), path.c_str());
		}

		return gid;
	}


	template< typename MatchGamesT, typename AllGamnesT, typename GameT >
	void EraseMatchGame(MatchGamesT &MatchGames, AllGamnesT &games, MatchItem &item, const GameT game, long gid)
	{
		// 删除先前可能的最佳匹配
		typename MatchGamesT::iterator iter = MatchGames.lower_bound(gid);
		for(; iter != MatchGames.upper_bound(gid); ++iter)
		{
			MatchItem &tmp = MatchItems[iter->second];
			if( tmp.MatchType == 1 )
				continue;

			if( tmp.game && tmp.game->GID == gid )
				tmp.game.reset();

			for( i8desk::data_helper::GameTraits::VecIterator giit = tmp.matchGames.begin();
				giit != tmp.matchGames.end(); ++giit) 
			{
				if( (*giit)->GID == gid ) 
				{
					tmp.matchGames.erase(giit);
					tmp.macthSum--;
					break;
				}
			}
		}

		MatchGames.erase(gid);

		// 设置精确匹配参数
		item.macthSum = 1;
		item.MatchType = 1;
		item.game = game;
		item.matchGames.clear();
		item.matchGames.push_back(game);


	}

	template < typename MatchGamesT, typename MatchItemT, typename GameT >
	void SetMatchGame(MatchGamesT &MatchGames, MatchItemT &item, const GameT &game, int deep)
	{
		// 目录名与游戏名相同，如果匹配数目保持1则为最佳匹配
		assert(!item.isMatch);
		MatchGames.insert(std::make_pair<long, size_t>(game->GID, deep));
	}
}

////////////////////////////////////////////////////////////////////////////////////////

					/*201０版匹配规则*/
//////////////////////////////////////////////////////////////////////////////////////////

/*
1, 索引中得到ID则视为精确匹配, 同时要满足中心资源中存在此ID的游戏
2, 目录名与游戏名相同, 且执行文件或特征文件也相同则视为精确匹配

3, 其他匹配条件有：其中如果只有匹配到一个游戏成功则为最佳匹配，否则为重复匹配
目录名与游戏名相同, 
或EXE文件不空，
且EXE为rungame.exe或目录下存在游戏指定的EXE文件, 
且指定了特征文件并且特征文件存在目录中
4, 最佳匹配的游戏缺省为匹配，下列条件下除外
如果有多个目录最佳匹配到一个游戏，则只有第一个为缺省匹配，其他为未匹配
如果有目录精确匹配已经最佳匹配的游戏，则最佳匹配的游戏将失去此游戏匹配项，
失去匹配项的目录并可能变为未匹配
5, 游戏被精确匹配到目录后，其他目录不可能再以任何方式匹配此游戏
6, 重复匹配的目录在选择匹配的游戏时，如果该游戏已经被匹配（通过游戏名比较）则选择失败
*/



////////////////////////////////////////////////////////////////////////////////////////

						/*2011版匹配规则*/
//////////////////////////////////////////////////////////////////////////////////////////
//现在的匹配规则：
//1： 精确匹配
//文件名相同 || i8index.idx 文件中的gid 在 中心游戏中 存在
//
//2： 最佳匹配
//(目标目录 ＋ game表的 exe 存在) && (目标目录 ＋ game表的 traitefile 存在)  满足此条件的 情况只有一条
//
//3： 重复匹配
//(目标目录 ＋ game表的 exe 存在) && (目标目录 ＋ game表的 traitefile 存在)  满足此条件的 情况有多条
//
//4： 没有匹配
//不满足以上情况则为没有匹配

void CDlgDownloadSomeAdd::_Scan()
{
	::CoInitialize(NULL);


	MatchItems.clear();
	typedef win32::file::FileRangeSearch<CheckFile, SearchMatchItem> ScanDirectory;

	stdex::tString searchDir = (LPCTSTR)searchDir_;


	CheckFile checkFile(stop_, searchDeep_ - 1);
	SearchMatchItem searchItem;
	ScanDirectory scanDir(checkFile, searchItem, searchDeep_ - 1);

	scanDir.Run(searchDir);

	// 开始
	PostMessage(WM_UPDATE, Start, 0);

	std::multimap<long, size_t> MatchGames;

	std::set<long> MatchGID;

	using i8desk::data_helper::GameTraits;
	GameTraits::MapType games = i8desk::GetDataMgr().GetGames();

	std::vector<stdex::tString> paths;

	int Progress = 0;
	int nMacthCount = 0;
	for(size_t i = 0; i < MatchItems.size(); ++i) 
	{
		++Progress ;
		PostMessage(WM_UPDATE, Running, Progress);
		if( ::WaitForSingleObject(stop_, 0) == WAIT_OBJECT_0 )
			break;
		assert( i < MatchItems.size() );
		MatchItem &item = MatchItems[i];

		item.exe = _T("");
		item.game.reset();
		item.macthSum = 0;
		item.matchGames.clear();
		bool IsMatchAccurate = false;

		// 精确匹配	
		//文件名相同则视为精确匹配
		paths.clear();

		stdex::Split(paths, item.dir, _T('\\'));
		stdex::tString dirName = paths.back();
		for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
		{
			const GameTraits::ValueType &game = iter->second;

			if( game->GID < MIN_IDC_GID )
				continue;
			
			stdex::tString gamename = game->Name;
			stdex::ToUpper(dirName);
			stdex::ToUpper(gamename);
			if ( dirName  == gamename )
			{
				EraseMatchGame(MatchGames, games, item, game, game->GID);
				games.erase(iter);
				IsMatchAccurate = true;
				break;
			}
		}


		// 索引中得到ID且在中心游戏中存在则视为精确匹配
		unsigned long gid = GetGameIDFromIdx(item.dir);
		if( gid != 0 && gid > MIN_IDC_GID )
		{
		
			GameTraits::MapConstIterator iter = games.find(gid);
			if( iter != games.end() )
			{
				EraseMatchGame(MatchGames, games, item, iter->second, gid);
				// 精确匹配后，从游戏列表中删除以免重复匹配
				games.erase(iter);
				IsMatchAccurate = true;
				continue;
			}
		}

		if ( IsMatchAccurate == true )
			continue;
		
		// 执行文件或特征文件也相同  满足此条件的情况只有一条则视为最佳匹配
		// 执行文件或特征文件也相同  满足此条件的情况有多条则视为重复匹配
		for(GameTraits::MapConstIterator iter = games.begin(); iter != games.end(); ++iter)
		{
			const GameTraits::ValueType &game = iter->second;

			if( game->GID < MIN_IDC_GID )
				continue;

			stdex::tString file = item.dir + _T("\\") + game->Exe;
			stdex::tString traitFile = item.dir + _T("\\") + game->TraitFile;

			if( ( !utility::IsEmpty(game->Exe)
				&& ::PathFileExists(file.c_str()) )
				|| (!utility::IsEmpty(game->TraitFile) 
				&& ::PathFileExists(traitFile.c_str()))
				)
			{
				assert(!item.MatchType);
				MatchGames.insert(std::make_pair<long, size_t>(game->GID, i));
				MatchGID.insert(game->GID);
			}
		}
	}



	std::multimap<long, size_t>::iterator iterrsp;

	for(std::set<long>::iterator iterset = MatchGID.begin(); iterset != MatchGID.end(); ++iterset)
	{
		std::multimap<long, size_t> rspMatchGames;
		long sgid = *iterset;
		size_t rspnum = MatchGames.count(sgid);
		if( rspnum == 1)
		{
			iterrsp = MatchGames.find(sgid);
			if ( iterrsp != MatchGames.end() )
			{	
				MatchItem &itemtmp = MatchItems[iterrsp->second];
				itemtmp.MatchType = BEST_MATCH;
				itemtmp.macthSum = 1;
				itemtmp.matchGames.push_back(games[sgid]);
			}	
		}
		else
		{
			for (size_t n = 0; n<rspnum ; n++)
			{
				iterrsp = MatchGames.find(sgid);
				if ( iterrsp != MatchGames.end() )
				{
					MatchItem &itemtmp = MatchItems[iterrsp->second];
					itemtmp.MatchType = REP_MATCH;
					itemtmp.macthSum = rspnum;
					itemtmp.matchGames.push_back(games[sgid]);
					MatchGames.erase(iterrsp);
				}
			}
		}
	}

	if( WAIT_OBJECT_0 != ::WaitForSingleObject(stop_, 0) )
		PostMessage(WM_UPDATE, Stop, 0);

	::CoUninitialize();
}


// CDlgDownloadSomeAdd 消息处理程序


BOOL CDlgDownloadSomeAdd::OnInitDialog()
{
	CNonFrameChildDlg::OnInitDialog();
	
	SetTitle(_T("批量添加游戏"));
	// Combo
	i8desk::data_helper::ClassTraits::VectorType classes ; 

	i8desk::business::Class::GetAllSortedClasses(classes);

	for(i8desk::data_helper::ClassTraits::VecConstIterator iter= classes.begin(); 
		iter != classes.end(); ++iter )
	{
		int nItem = wndComboClass_.AddString((*iter)->Name);
		wndComboClass_.SetItemData(nItem, reinterpret_cast<DWORD_PTR>((*iter).get()));
	}

	wndComboClass_.SelectString(-1, curClass_.c_str());
	if( wndComboClass_.GetCurSel() == -1 )
		wndComboClass_.SetCurSel(0);

	wndComboDeep_.AddString(_T("1"));
	wndComboDeep_.AddString(_T("2"));
	wndComboDeep_.AddString(_T("3"));
	wndComboDeep_.SetCurSel(0);


	// Result 
	wndListResult_.InsertHiddenLabelColumn();

	wndListResult_.InsertColumnTrait(MASK_SOMEADD_PATH,		_T("路径"),		LVCFMT_LEFT, 203,MASK_SOMEADD_PATH,GetTrait());
	wndListResult_.InsertColumnTrait(MASK_SOMEADD_STATUS,	_T("状态"),		LVCFMT_LEFT, 70,MASK_SOMEADD_STATUS,GetTrait());
	wndListResult_.InsertColumnTrait(MASK_SOMEADD_RESULT,	_T("结果操作"),	LVCFMT_LEFT, 100,MASK_SOMEADD_RESULT,GetTrait());
	wndListResult_.InsertColumnTrait(MASK_SOMEADD_GID,		_T("匹配GID"),	LVCFMT_LEFT, 70,MASK_SOMEADD_GID,GetTrait());

	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;

	// Run Type
	wndListRuntype_.InsertHiddenLabelColumn();

	wndListRuntype_.InsertColumnTrait(1, _T("分区"), LVCFMT_LEFT, 173,1,GetTrait());
	wndListRuntype_.InsertColumnTrait(2, _T("运行方式"), LVCFMT_LEFT, 270, 2, pComboTrait);

	// 添加组合框的所有运行方式
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	int nIndex = 0;
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_UNKNOWN.c_str());
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_LOCAL.c_str());

	i8desk::business::VDisk::RunTypeDesc(std::tr1::bind(&CGridColumnTraitCombo::AddItem, pComboTrait, 
		nIndex++, std::tr1::placeholders::_1));
	pComboTrait->AddItem(nIndex++, i8desk::data_helper::RT_DIRECT.c_str());


	// 添加游戏在每个分区的运行方式
	stdex::tString szDefRunType = i8desk::GetDataMgr().GetOptVal(OPT_D_AREADEFRUNTYPE, _T(""));
	using i8desk::data_helper::AreaTraits;
	AreaTraits::MapType &areas = i8desk::GetDataMgr().GetAreas();
	i8desk::GetDataMgr().GetAllData(areas);

	int nItem = 0;
	for(AreaTraits::MapConstIterator iter = areas.begin(); iter != areas.end(); ++iter)
	{
		nItem = wndListRuntype_.InsertItem(nItem, _T(""));
		wndListRuntype_.SetItemText(nItem, 1, iter->second->Name);
		wndListRuntype_.SetItemText(nItem, 2, i8desk::business::Area::ParseRunType(iter->first, szDefRunType).c_str());
		nItem++;
	}

	// Out Line
	outboxLine_.Attach(CopyBitmap(i8desk::ui::SkinMgrInstance().GetSkin(_T("Common/ControlOutBox.png"))));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgDownloadSomeAdd::OnBnClickedButtonDownloadAddBrowseDir()
{
	CString selPath;
	if( theApp.GetShellManager()->BrowseForFolder(selPath, this, 0, _T("选择搜索目录:")) )
	{
		searchDir_ = selPath;
		UpdateData(FALSE);
	}
}

void CDlgDownloadSomeAdd::OnBnClickedButtonDownloadAddExecute()
{
	UpdateData(TRUE);

	if( !IsWorking )
	{
		if( searchDir_.IsEmpty() )
		{
			CMessageBox msgDlg(_T("提示"),_T("请输入搜索目录."));
			msgDlg.DoModal();

			wndEditSearch_.SetFocus();
			return ;
		}

		if( searchDir_.Right(1) == _T("\\") )
			searchDir_.Delete(searchDir_.GetLength() - 1);
		if (searchDeep_ > 3)
		{
			wndComboDeep_.SetCurSel(2);
			searchDeep_ = 3;
		}
		if (searchDeep_ < 1)
		{
			wndComboDeep_.SetCurSel(0);
			searchDeep_ = 1;
		}

		i8desk::io::GetIODispatcher().Post(std::tr1::bind(&CDlgDownloadSomeAdd::_Scan, this));
		wndProgress_.SetPos(0);

		IsWorking = true;
		wndBtnStart_.SetWindowText(ButtonText[1].c_str());
		wndEditTip_.SetWindowText(_T("正在搜索:"));

		GetDlgItem(IDOK)->EnableWindow(FALSE);
		wndBtnSearch_.EnableWindow(FALSE);
	}
	else
	{
		stop_.SetEvent();

		IsWorking = true;
		wndBtnStart_.SetWindowText(ButtonText[1].c_str());
		wndEditTip_.SetWindowText(_T("正在搜索:"));

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		wndBtnSearch_.EnableWindow(TRUE);
	}
}


LRESULT CDlgDownloadSomeAdd::OnMessage(WPARAM wParam, LPARAM lParam)
{
	if( wParam == Running ) 
	{
		wndProgress_.SetPos(lParam);
		wndListResult_.RedrawWindow();
	} 
	else if( wParam == Start ) 
	{
		wndListResult_.SetItemCount(MatchItems.size());
		wndProgress_.SetRange(0, MatchItems.size());
	} 
	else if( wParam == Stop )
	{
		int nMatch = 0, nBestMatch = 0, nRepMatch = 0, nUnMatch = 0;

		for(MatchItemList::const_iterator iter = MatchItems.begin(); 
			iter != MatchItems.end(); ++iter) 
		{
			switch( iter->MatchType ) 
			{
			case NO_MATCH:				nUnMatch++;		break; 
			case ACCURATE_MATCH:		nMatch++;		break; 
			case BEST_MATCH:			nBestMatch++;	break; 
			default:					nRepMatch++;	break; 
			}
		}

		bCanClick_ = true;
		IsWorking = false;
		wndBtnStart_.SetWindowText(ButtonText[0].c_str());

		stdex::tOstringstream os;
		os << _T("搜索结果: 精确匹配数(") << nMatch 
			<< _T("), 重复匹配数(") << nRepMatch
			<< _T("), 最佳匹配数(") << nBestMatch
			<< _T("), 不能匹配数(") << nUnMatch
			<< _T(")");
		wndEditTip_.SetWindowText(os.str().c_str());

		wndProgress_.SetPos(MatchItems.size());
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		wndBtnSearch_.EnableWindow(TRUE);

		wndListResult_.RedrawWindow();
	}

	return TRUE;
}

void CDlgDownloadSomeAdd::OnBnClickedOk()
{
	if( IsWorking )
		return;

	bool isDirectRun = false; //检测是否为不更新直接运行方式，以不替换客户端路径

	// 设置运行方式
	using i8desk::data_helper::RunTypeTraits;
	RunTypeTraits::VectorType runTypes;

	for( int nIdx= 0; nIdx != wndListRuntype_.GetItemCount(); ++nIdx )
	{
		RunTypeTraits::ValueType val(new RunTypeTraits::ElementType);

		val->Name = (LPCTSTR)wndListRuntype_.GetItemText(nIdx, 1);
		utility::Strcpy(val->AID, i8desk::business::Area::GetAIDByName(val->Name));
		val->GID = 0;

		if( i8desk::business::Area::ParseArea((LPCTSTR)wndListRuntype_.GetItemText(nIdx, 2), val) )
		{
			runTypes.push_back(val);
			if( val->Type == i8desk::data_helper::ERT_DIRECT )
				isDirectRun = true;
		}
	}


	i8desk::data_helper::SyncTaskTraits::VectorType syncTasks;
	i8desk::business::SyncTask::GetSyncTasks(syncTasks);


	struct Op
	{

		void Run(const MatchItemList &MatchItems, i8desk::data_helper::ClassTraits::ElementType *classVal, bool isDirectRun, 
			const i8desk::data_helper::RunTypeTraits::VectorType &runTypes,
			const i8desk::data_helper::SyncTaskTraits::VectorType &syncTasks)
		{
			// Class
			using i8desk::data_helper::ClassTraits;
			using i8desk::data_helper::GameTraits;

			const stdex::tString CID = classVal->CID;
			const stdex::tString drv = i8desk::GetDataMgr().GetOptVal(OPT_M_GAMEDRV, _T("E"));
			GameTraits::MapType &games = i8desk::GetDataMgr().GetGames();

			for(MatchItemList::const_iterator iter = MatchItems.begin(); iter != MatchItems.end(); ++iter)
			{
				GameTraits::ValueType game;
				if( iter->macthSum >= 1 ) 
					game = iter->game;
				else if( !iter->exe.empty() ) 
				{
					game.reset(new GameTraits::ElementType);
					game->GID = i8desk::business::Game::GetCustomGID();

					utility::Strcpy(game->Name, ::PathFindFileName(iter->dir.c_str()));
					utility::Strcpy(game->CID, CID);
					utility::Strcpy(game->Exe, ::PathFindFileName(iter->exe.c_str()));
					game->Priority = i8desk::data_helper::Lowest;
				}

				if( !game )
					continue;

				// Path
				utility::Strcpy(game->SvrPath, iter->dir);
				::PathAddBackslash(game->SvrPath);

				utility::Strcpy(game->CliPath, game->SvrPath);
				if( !isDirectRun && !drv.empty() )
					game->CliPath[0] = drv[0];

				if( game->GID >= MIN_IDC_GID )
					game->AutoUpt = 1;
				else
				{
					game->AutoUpt = 0;
					game->SvrVer = (i8desk::ulong)::time(0);
				}

				GameTraits::ValueType findGame;
				if( i8desk::business::Game::CheckByName(game->Name, findGame) )
				{
					// 不在本地且为中心游戏
					if( findGame->GID >= MIN_IDC_GID )
					{
						game->Status = 1;
						if( !i8desk::GetDataMgr().ModifyData(games, game->GID, game) ||
							!i8desk::business::Game::SetRunType(game, runTypes) )
							continue;
					}
				}
				else // 添加没有重复的游戏
				{
					game->Status = 1;
					if( !i8desk::GetDataMgr().AddData(games, game->GID, game) ||
						!i8desk::business::Game::SetRunType(game, runTypes) )
						continue;
				}

				// icon.
				stdex::tString exePath = game->SvrPath;
				exePath += game->Exe;

				HICON hIcon = LoadIconFromFile(exePath.c_str());
				if( hIcon != 0 )
					i8desk::business::Game::SetIcon(game, hIcon);
				else
					game->Icon = i8desk::ui::SkinMgrInstance().GetIcon(0);
			}
		}
	}op;

	if( !MatchItems.empty() )
	{
		using i8desk::data_helper::ClassTraits;
		ClassTraits::ElementType *val = reinterpret_cast<ClassTraits::ElementType *>(wndComboClass_.GetItemData(wndComboClass_.GetCurSel()));
		CDlgWaitMsg dlg;
		dlg.Exec(std::tr1::bind(&Op::Run, op, std::tr1::cref(MatchItems), val, isDirectRun, std::tr1::cref(runTypes), std::tr1::cref(syncTasks)));
	}
	

	OnOK();
}

void CDlgDownloadSomeAdd::OnBnClickedCancel()
{

	OnCancel();
}

void CDlgDownloadSomeAdd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( IsWorking && nID == SC_CLOSE )
		return;

	CNonFrameChildDlg::OnSysCommand(nID, lParam);
}

void CDlgDownloadSomeAdd::OnLvnGetdispinfoListDownloadAddResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	NMLVCUSTOMDRAW *pNMCD = reinterpret_cast<NMLVCUSTOMDRAW *>(pNMHDR);
	*pResult = 0;

	LV_ITEM* pItem = &pDispInfo->item;
	if( pItem->iItem >= (int)MatchItems.size() )
		return;

	static stdex::tString text; 
	if( pItem->mask & LVIF_TEXT )
	{
		MatchItem &item = MatchItems[pItem->iItem];
		switch(pItem->iSubItem)
		{
		case MASK_SOMEADD_PATH:
			text = item.dir;
			break;
		case MASK_SOMEADD_STATUS: 
			switch( item.MatchType ) 
			{
			case -1: text.clear();				break;
			case 0:	 text = _T("没有匹配");	break;
			case 1:	 text = _T("精确匹配");	break;
			case 2:	 text = _T("最佳匹配");	break;
			default: text = _T("重复匹配");	break;
			}
			break;
		case MASK_SOMEADD_RESULT:
			if( item.game.use_count() )
				text = item.game->Name;
			else 
				text = item.exe;
			break;
		case MASK_SOMEADD_GID:
			if( item.game.use_count() )
				text = stdex::ToString<stdex::tString>(item.game->GID);
			else 
				text.clear();
			break;
		}

		utility::Strcpy(pItem->pszText, pItem->cchTextMax, text.c_str());
	} 
}


void CDlgDownloadSomeAdd::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	if (!bCanClick_)
		return ;

	int nSelItem = wndListResult_.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1)	
		return ;
	ASSERT(nSelItem < (int)MatchItems.size());
	
	MatchItem &game = MatchItems[nSelItem];

	CPoint pt( ::GetMessagePos() );
	ScreenToClient(&pt);

	CRect rc;
	wndListResult_.GetSubItemRect(nSelItem, MASK_SOMEADD_RESULT, LVIR_BOUNDS, rc);
	wndListResult_.ClientToScreen(&rc);	
	ScreenToClient(&rc);
	if (!rc.PtInRect(pt))
		return ;
	if ( game.macthSum == 0 )
	{
		CFileDialog Open(TRUE, _T(""), _T(""), 4|2, _T("所有文件(*.*)|*.*||"));	
		Open.m_ofn.lpstrInitialDir = (LPCTSTR)game.dir.c_str();
		if (Open.DoModal() == IDOK)
		{
			CString file = Open.GetPathName();
			if (StrNCmpI(game.dir.c_str(), file, game.dir.length()) != 0)
			{
				CMessageBox msgDlg(_T("提示"),_T("执行文件和目录路径不一致."));
				msgDlg.DoModal();
			}
			game.exe = ::PathFindFileName(file);
			wndListResult_.RedrawItems(nSelItem, nSelItem);
		}
		else
		{
			game.exe = _T("");
			wndListResult_.RedrawItems(nSelItem, nSelItem);
		}
	}
	else
	{
		wndComboResulte_.ResetContent();
		wndComboResulte_.AddString(_T("不匹配"));
		for (size_t idx = 0; idx < game.matchGames.size(); idx++)
			wndComboResulte_.AddString( game.matchGames[idx]->Name );
		wndComboResulte_.SelectString(-1, wndListResult_.GetItemText(nSelItem, MASK_SOMEADD_RESULT));
		rc.OffsetRect(-1, -1);
		wndComboResulte_.MoveWindow(&rc);
		wndComboResulte_.ShowWindow(SW_SHOW);
		wndComboResulte_.BringWindowToTop();
		wndComboResulte_.SetFocus();
		wndComboResulte_.ShowDropDown(TRUE);
	}
}

void CDlgDownloadSomeAdd::OnCbnKillfocusDownloadResults()
{
	wndComboResulte_.ShowWindow(SW_HIDE);

	int nSelItem = wndListResult_.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1 || nSelItem >= (int)MatchItems.size())	
		return ;

	bool ok = false;
	MatchItem &game = MatchItems[nSelItem];

	CString Text;
	if (wndComboResulte_.GetCurSel() == -1) {
		goto Exit;
	}

	wndComboResulte_.GetLBText(wndComboResulte_.GetCurSel(), Text);

	//检查是否有目录已经匹配过选中的游戏
	for (int i = 0; i < wndListResult_.GetItemCount(); i++) {
		if (nSelItem == i) {
			continue;
		}
		if (wndListResult_.GetItemText(i, MASK_SOMEADD_RESULT) == Text) {
			stdex::tOstringstream msg;
			msg << _T("目录[") << wndListResult_.GetItemText(i, MASK_SOMEADD_PATH)
				<< _T("]已经匹配了游戏[") << Text << _T("],不允许重复匹配") ;
			CMessageBox msgDlg(_T("提示"),msg.str().c_str());
			msgDlg.DoModal();
			goto Exit;
		}
	}

	//匹配游戏
	for (size_t idx=0; idx < game.matchGames.size(); idx++)
	{
		if (Text == game.matchGames[idx]->Name)
		{
			game.game = game.matchGames[idx];
			ok = true;
			break;
		}
	}

Exit:
	if (!ok) {
		game.game.reset();
	}

	wndListResult_.RedrawItems(nSelItem, nSelItem);
	wndListResult_.SetFocus();
}

void CDlgDownloadSomeAdd::OnCbnCloseupDownloadResults()
{
	OnCbnKillfocusDownloadResults();
}

void CDlgDownloadSomeAdd::OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if( wndListResult_.GetItemCount() == 0 )
		return;


	struct Compare
	{
		int col_;
		bool ascSort_;

		Compare(int col, bool ascSort)
			: col_(col)
			, ascSort_(ascSort)
		{}
		bool operator()(const MatchItem &lhs, const MatchItem &rhs) const
		{
			switch(col_)
			{
			case MASK_SOMEADD_PATH:
				return ascSort_ ? utility::Strcmp(lhs.dir.c_str(), rhs.dir.c_str()) > 0 : utility::Strcmp(lhs.dir.c_str(), rhs.dir.c_str()) < 0;
			case MASK_SOMEADD_STATUS:
				return false;
			case MASK_SOMEADD_RESULT:
				return ascSort_ ?  (lhs.MatchType > rhs.MatchType)  : (lhs.MatchType < rhs.MatchType) ;
			case MASK_SOMEADD_GID:
				return false ;
				break;
			default:
				assert(0);
				return false;
			}
		}
	};

	CWaitCursor wc;
	std::stable_sort(MatchItems.begin(), MatchItems.end(), Compare(pNMLV->iSubItem, SomeAddCompare[pNMLV->iSubItem]));
	SomeAddCompare.set(pNMLV->iSubItem, !SomeAddCompare[pNMLV->iSubItem]);
	wndListResult_.RedrawItems(0, wndListResult_.GetItemCount() - 1);

}

BOOL CDlgDownloadSomeAdd::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	CRect rcMultiText;
	wndListResult_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);
	wndListRuntype_.GetWindowRect(rcMultiText);
	i8desk::ui::DrawFrame(this, *pDC, rcMultiText, &outboxLine_);

	return TRUE;
}

