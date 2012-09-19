// PiLiAddDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "PiLiAddDlg.h"
#include "ConsoleDlg.h"


#include <process.h>
#include <algorithm>
#include <map>

#define MAX_IDX_FILESIZE (1024 * 1024 * 4)

#define  WM_MSG		WM_USER + 1



template<typename T>
class FMatchResult {
public:
	FMatchResult(bool b) : m_b(b) {}
	bool greater(const T& left, const T& right)
	{
		if (left.pGame) {
			if (right.pGame)
				return i8desk::stricmp(left.pGame->Name, right.pGame->Name) > 0;
			else 
				return true;
		} else {
			if (right.pGame)
				return false;
			else {
				return i8desk::stricmp(left.exe, right.exe) > 0;
			}
		}
	}
	bool less(const T& left, const T& right)
	{
		if (left.pGame) {
			if (right.pGame)
				return i8desk::stricmp(left.pGame->Name, right.pGame->Name) < 0;
			else 
				return false;
		} else  {
			if (right.pGame)
				return true;
			else {
				return i8desk::stricmp(left.exe, right.exe) < 0;
			}
		}
	}
	bool operator()(const T& left, const T& right)
	{
		if (m_b)
			return this->greater(left, right);
		return this->less(left, right);
	}
private:
	bool m_b;
};


template<typename T>
class FGid {
public:
	FGid(bool b) : m_b(b) {}
	bool greater(const T& left, const T& right)
	{
		if (left.pGame) {
			if (right.pGame)
				return left.pGame->GID > right.pGame->GID;
			else 
				return true;
		} else {
			if (right.pGame)
				return false;
			else 
				return false;
		}
	}
	bool less(const T& left, const T& right)
	{
		if (left.pGame) {
			if (right.pGame)
				return left.pGame->GID < right.pGame->GID;
			else 
				return false;
		} else {
			if (right.pGame)
				return true;
			else 
				return false;
		}
	}
	bool operator()(const T& left, const T& right)
	{
		if (m_b)
			return this->greater(left, right);
		return this->less(left, right);
	}
private:
	bool m_b;
};


template<typename T>
class FMatch {
public:
	FMatch(bool b) : m_b(b) {}
	bool greater(const T& left, const T& right)
	{
		if (left.bMatchType) {
			if (right.bMatchType)
				return false;
			else 
				return true;
		} else {
			if (right.bMatchType)
				return false;
			else 
				return left.nMacthCount > right.nMacthCount;
		}
	}
	bool less(const T& left, const T& right)
	{
		if (left.bMatchType) {
			if (right.bMatchType)
				return false;
			else 
				return false;
		} else {
			if (right.bMatchType)
				return true;
			else 
				return left.nMacthCount < right.nMacthCount;
		}
	}
	bool operator()(const T& left, const T& right)
	{
		if (m_b)
			return this->greater(left, right);
		return this->less(left, right);
	}
private:
	bool m_b;
};


IMPLEMENT_DYNAMIC(CPiLiAddDlg, CDialog)

CPiLiAddDlg::CPiLiAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPiLiAddDlg::IDD, pParent)
	, m_bDirSortDesc(false)
	, m_bMatchSortDesc(false)
	, m_bResultSortDesc(false)
	, m_bGidSortDesc(false)
{
	m_pbuf = new char[MAX_IDX_FILESIZE];
	m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThread = NULL;
	m_bCanClick = FALSE;
}

CPiLiAddDlg::~CPiLiAddDlg()
{
	CloseHandle(m_hExited);
	delete[] m_pbuf;
}

void CPiLiAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
	DDX_Control(pDX, IDC_PROGRESS, m_bProgress);
	DDX_Control(pDX, IDC_LIST1, m_lstRunType);
	DDX_Control(pDX, IDC_COMBO1, m_cboGame);
	DDX_Control(pDX, IDC_COMBO_CLASS, m_cboClass);
}

BEGIN_MESSAGE_MAP(CPiLiAddDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPiLiAddDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSE_DIR, &CPiLiAddDlg::OnBnClickedBrowseDir)
	ON_BN_CLICKED(IDC_EXECUTE, &CPiLiAddDlg::OnBnClickedExecute)
	ON_EN_CHANGE(IDC_DEEP, &CPiLiAddDlg::OnEnChangeDeep)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MSG, OnMessage)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CPiLiAddDlg::OnNMClickList)
	ON_CBN_KILLFOCUS(IDC_COMBO1, &CPiLiAddDlg::OnCbnKillfocusCombo1)
	ON_CBN_CLOSEUP(IDC_COMBO1, &CPiLiAddDlg::OnCbnCloseupCombo1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, &CPiLiAddDlg::OnNMCustomdrawList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST, &CPiLiAddDlg::OnLvnColumnclickList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, &CPiLiAddDlg::OnLvnGetdispinfoList)
END_MESSAGE_MAP()

BOOL CPiLiAddDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWaitCursor wc;

	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	{
	extern i8desk::CLock g_Lock;
	i8desk::CAutoLock<i8desk::CLock> lock(&g_Lock);
	GameInfos = pFrame->m_GameInfos;
	m_IDArray = pFrame->m_IDArray; 
	}

	std::string	    ErrInfo;

	//if (!pFrame->m_pDbMgr->GetAllGame(GameInfos, m_IDArray, ErrInfo) ||
	if (!pFrame->m_pDbMgr->GetAllVDisk(VDiskInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllArea(AreaInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllClass(ClassInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	LPCSTR defGuid[] = {
		CLASS_WL_GUID, 
		CLASS_DJ_GUID, 
		CLASS_XX_GUID, 
		CLASS_WY_GUID, 
		CLASS_DZ_GUID, 
		CLASS_QP_GUID, 
		CLASS_PL_GUID, 
		CLASS_LT_GUID, 
		CLASS_CY_GUID, 
		CLASS_YY_GUID,
		CLASS_GP_GUID,
	};

	for (int idx=0; idx<_countof(defGuid); idx++)
	{		
		i8desk::DefClassMapItr it = ClassInfos.find(defGuid[idx]);
		if (it != ClassInfos.end())
		{
			int nItem = m_cboClass.AddString(it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_cboClass.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}
	}

	for (i8desk::DefClassMapItr it = ClassInfos.begin();
		it != ClassInfos.end(); it++)
	{
		if (!i8desk::IsI8DeskClassGUID(it->first))
		{
			int nItem = m_cboClass.AddString(it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_cboClass.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}
	}
	ClassInfos.clear();

	m_cboClass.SelectString(-1, pFrame->GetTreeSelNodeText());
	if (m_cboClass.GetCurSel() == -1)
		m_cboClass.SetCurSel(0);

	m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstCtrl.InsertColumn(0, "路径",		LVCFMT_LEFT, 170);
	m_lstCtrl.InsertColumn(1, "状态",		LVCFMT_LEFT, 70);
	m_lstCtrl.InsertColumn(2, "结果操作",	LVCFMT_LEFT, 100);
	m_lstCtrl.InsertColumn(3, "匹配GID",	LVCFMT_LEFT, 60);

	SetDlgItemInt(IDC_DEEP, 1);
	m_bProgress.ShowWindow(SW_HIDE);

	m_lstRunType.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstRunType.InsertColumn(0, "分区", LVCFMT_LEFT, 100);

	//添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	pComboTrait->AddItem(0, RT_UNKNOWN);
	pComboTrait->AddItem(1, RT_LOCAL);
	int loop = 2;
	for (i8desk::VDiskInfoMapItr it = VDiskInfos.begin();
		it != VDiskInfos.end(); it ++)
	{
		CString info;
		if (it->second->Type == VDISK_I8DESK)
		{
			info.Format("%s%c (%s:%C)",
				RT_VDISK,
				(char)it->second->CliDrv,
				i8desk::MakeIpString(it->second->IP).c_str(),
				(char)it->second->SvrDrv);
		}
		else
		{
			info.Format("%s%C", RT_VDISK, (char)it->second->CliDrv);
		}
		pComboTrait->AddItem(loop, info);
		loop ++;
	}
	pComboTrait->AddItem(loop, RT_DIRECT);

 	m_lstRunType.InsertColumnTrait(1, "运行方式", LVCFMT_LEFT, 260, 1, pComboTrait);
	
	m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 1 , RT_LOCAL);
	for (i8desk::AreaInfoMapItr it = AreaInfos.begin(); 
		it != AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nIdx = m_lstRunType.InsertItem(
				m_lstRunType.GetItemCount(), it->second->Name);
			m_lstRunType.SetItemText(nIdx, 1 , RT_LOCAL);
		}
	}
	wc.Restore();
	return TRUE;
}

void CPiLiAddDlg::OnBnClickedBrowseDir()
{
	BROWSEINFO info;
	memset(&info, 0, sizeof(info));
	info.hwndOwner = m_hWnd;
	info.lpszTitle = "选择搜索目录:";
	LPITEMIDLIST lst = SHBrowseForFolder(&info);
	if (lst != NULL)
	{
		char buf[MAX_PATH] = {0};
		SHGetPathFromIDList(lst, buf);
		LPMALLOC pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(lst);
		pMalloc->Release();
		SetDlgItemText(IDC_PATH, buf);
	}
}

void CPiLiAddDlg::ScanDir(const CString& dir, std::vector<tagItem>& lstDir, int nDeep, int nCurDeep)
{
	if (nCurDeep == nDeep)
		return ;

	CFileFind finder;
	CString path(dir+"\\*.*");
	BOOL bWorder = finder.FindFile(path);
	while (bWorder)
	{
		if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
			break;
		bWorder = finder.FindNextFile();
		if (finder.IsDirectory() && !finder.IsDots() && !finder.IsSystem() && !finder.IsHidden())
		{
			if (nCurDeep == nDeep -1)
			{
				tagItem Item;
				Item.nMacthCount = -1;
				Item.bMatchType = false;
				Item.dir = finder.GetFilePath();
				lstDir.push_back(Item);
			}
			ScanDir(finder.GetFilePath(), lstDir, nDeep, nCurDeep+1);
		}
	}
}

void CPiLiAddDlg::OnBnClickedExecute()
{
	CString cmd;
	GetDlgItemText(IDC_EXECUTE, cmd);
	if (cmd == "执行搜索")
	{
		GetDlgItemText(IDC_PATH, m_strPath);
		if (m_strPath.IsEmpty())
		{
			AfxMessageBox("请输入搜索目录.");
			GetDlgItem(IDC_PATH)->SetFocus();
			return ;
		}
		if (m_strPath.Right(1) == "\\")
			m_strPath.Delete(m_strPath.GetLength()-1);
		m_nDeep	  =  GetDlgItemInt(IDC_DEEP);
		ResetEvent(m_hExited);
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
		if (m_hThread == NULL)
		{
			AfxMessageBox("创建线程失败");
			return ;
		}
		else
		{
			m_bProgress.SetPos(0);
			SetDlgItemText(IDC_EXECUTE, "停止搜索");
			SetDlgItemText(IDC_TIP, "正在搜索:");
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			GetDlgItem(IDC_BROWSE_DIR)->EnableWindow(FALSE);
			m_bCanClick = FALSE;
		}
	}
	else
	{
		if (m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		SetDlgItemText(IDC_EXECUTE, "执行搜索");
		m_bProgress.ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_TIP, "搜索结果:");
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE_DIR)->EnableWindow(TRUE);
	}
}

void CPiLiAddDlg::OnEnChangeDeep()
{
	int nDeep = GetDlgItemInt(IDC_DEEP);
	if (nDeep > 3)
		SetDlgItemInt(IDC_DEEP, 3);
	if (nDeep < 1)
		SetDlgItemInt(IDC_DEEP, 1);
}

//从目录下的i8desk.idx取得游戏ID，返回0无效
long CPiLiAddDlg::GetGameIdFromIdx(const CString& dir)
{
	i8desk::FILE_Ptr f = i8desk::fopen((LPCTSTR)(dir + _T("\\i8desk.idx")), _T("rb"));
	if (!f) 
		return 0;
	
	DWORD nXmlSize;
	i8desk::fseek(f, 24L, SEEK_SET);
	if (i8desk::fread(&nXmlSize, sizeof(nXmlSize), 1, f) != 1 
		|| nXmlSize > MAX_IDX_FILESIZE) 
		return 0;

	WORD nCrcBlock;
	i8desk::fseek(f, 30L, SEEK_SET);
	if (i8desk::fread(&nCrcBlock, sizeof(nCrcBlock), 1, f) != 1)
		return 0;

	i8desk::fseek(f, (nCrcBlock + 1) * 32L, SEEK_SET);
	if (i8desk::fread(m_pbuf, nXmlSize, 1, f) != 1)
		return 0;
	i8desk::fclose(f);

	std::string xmlFile = CMarkup::UTF8ToA(std::string(m_pbuf, nXmlSize));


	MSXML2::IXMLDOMDocumentPtr XmlDoc;
	XmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (!XmlDoc->loadXML(xmlFile.c_str())) 
		return 0;

	MSXML2::IXMLDOMElement *pRoot = 0;
	if (FAILED(XmlDoc->get_documentElement(&pRoot)))
		return 0;

	long gid = 0;
	I8_TRY {
		gid = pRoot->getAttribute(_T("g"));
	} I8_CATCHALL {
		TRACE(_T("从索引文件查找游戏编号时没有发现相应属性.idx=%s\n"),
			(LPCTSTR)(dir + _T("\\i8desk.idx")));
	} I8_ENDTRY;

	pRoot->Release();
	
	return gid;
}

/*
1, 索引中得到ID则视为精确匹配, 同时要满足条件PL中存在此ID的游戏
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
UINT __stdcall CPiLiAddDlg::WorkThread(LPVOID lpVoid)
{
	CoInitialize(NULL);
	CPiLiAddDlg* pDlg   = reinterpret_cast<CPiLiAddDlg*>(lpVoid);

	pDlg->m_lstDir.clear();
	pDlg->ScanDir(pDlg->m_strPath, pDlg->m_lstDir, pDlg->m_nDeep, 0);

	::PostMessage(pDlg->m_hWnd, WM_MSG, 0, 0);

	i8desk::GameInfoMap GameInfos = pDlg->GameInfos;
	
	int nMacthCount = 0;
	std::multimap<long, size_t> BestMatchGame;

	std::vector<tagItem>& dirs = pDlg->m_lstDir;
	for (size_t nDir = 0; nDir < dirs.size(); nDir++) 
	{
		::PostMessage(pDlg->m_hWnd, WM_MSG, 1, nMacthCount++);

		if (WaitForSingleObject(pDlg->m_hExited, 0) == WAIT_OBJECT_0)
			break;

		tagItem& item = dirs[nDir];

		item.exe = "";
		item.pGame.reset();
		item.nMacthCount = 0;
		item.MatchGame.clear();

		if (long gid = pDlg->GetGameIdFromIdx(item.dir)) 
		{
			//索引中得到ID则视为精确匹配
			i8desk::GameInfoMapItr Iter = GameInfos.find(gid);
			if (Iter != GameInfos.end()) 
			{
				//删除先前可能的最佳匹配
				std::multimap<long, size_t>::iterator it = BestMatchGame.lower_bound(gid);
				for (; it != BestMatchGame.upper_bound(gid); ++it) {
					tagItem& theItem = dirs[it->second];
					if (theItem.bMatchType)
						continue;
					if (theItem.pGame && theItem.pGame->GID == gid) {
						theItem.pGame.reset();
					}
					i8desk::GameInfoVectorItr giit = theItem.MatchGame.begin();
					for (; giit != theItem.MatchGame.end(); ++giit) {
						if ((*giit)->GID == gid) {
							theItem.MatchGame.erase(giit);
							theItem.nMacthCount--;
							break;
						}
					}
				}
				BestMatchGame.erase(gid);

				//设置精确匹配参数
				item.nMacthCount = 1;
				item.bMatchType = true;
				item.pGame = Iter->second;
				item.MatchGame.clear();
				item.MatchGame.push_back(Iter->second);

				//精确匹配后，从游戏列表中删除以免重复匹配
				GameInfos.erase(Iter);

				continue;
			}
		}
 
		CString Name = item.dir.Mid(item.dir.ReverseFind('\\')+1);

		for (i8desk::GameInfoMapItr Iter = GameInfos.begin(); 
			Iter != GameInfos.end(); Iter++)
		{
			i8desk::GameInfo_Ptr pGame = Iter->second;
			
			if (pGame->GID < MIN_IDC_GID) {
				continue;
			}

			if (lstrcmpi(pGame->Name, Name) == 0)
			{
				if ((!STRING_ISEMPTY(pGame->GameExe)
					  && PathFileExists(item.dir + "\\" + pGame->GameExe)
					  )
					|| (!STRING_ISEMPTY(pGame->MatchFile) 
					     && (item.dir + "\\" + pGame->MatchFile)
						 )
					)
				{
					//目录名与游戏名相同且执行文件或特征文件也相同则视为精确匹配

					//删除先前可能的最佳匹配
					std::multimap<long, size_t>::iterator it = BestMatchGame.lower_bound(pGame->GID);
					for (; it != BestMatchGame.upper_bound(pGame->GID); ++it) {
						tagItem& theItem = dirs[it->second];
						if (theItem.bMatchType)
							continue;
						if (theItem.pGame && theItem.pGame->GID == pGame->GID) {
							theItem.pGame.reset();
						}
						i8desk::GameInfoVectorItr giit = theItem.MatchGame.begin();
						for (; giit != theItem.MatchGame.end(); ++giit) {
							if ((*giit)->GID == pGame->GID) {
								theItem.MatchGame.erase(giit);
								theItem.nMacthCount--;
								break;
							}
						}
					}
					BestMatchGame.erase(pGame->GID);

					//设置精确匹配参数
					item.nMacthCount = 1;
					item.bMatchType = true;
					item.pGame = pGame;
					item.MatchGame.clear();
					item.MatchGame.push_back(pGame);

					//精确匹配后，从游戏列表中删除以免重复匹配
					GameInfos.erase(Iter);
					break;
				} 

				//目录名与游戏名相同，如果匹配数目保持1则为最佳匹配
				ASSERT(!item.bMatchType);
				item.nMacthCount++;
				item.MatchGame.push_back(pGame);
				BestMatchGame.insert(std::make_pair<long, size_t>(pGame->GID, nDir));

				if (item.nMacthCount == 1) {
					//第一次匹配
					bool bMatched = false;
					for (size_t n = 0; n < dirs.size(); n++) {
						if (dirs[n].pGame && dirs[n].pGame->GID == pGame->GID) {
							bMatched = true;
							break;
						}
					}
					//没有匹配项则可以缺省为匹配
					if (!bMatched) {
						item.pGame = pGame;
					}
				} else {
					//重复匹配，缺省未匹配
					item.pGame.reset();
				} 
			} else {
				//exe文件匹配
				if (lstrcmpi(pGame->GameExe, "rungame.exe") == 0
					|| (!STRING_ISEMPTY(pGame->GameExe) 
							&& PathFileExists(item.dir + "\\" + pGame->GameExe)
						)
					)
				{
					//可执行文件为rungame.exe或目录下存在游戏指定的EXE文件
					if (!STRING_ISEMPTY(pGame->MatchFile)
						&& PathFileExists(item.dir + "\\" + pGame->MatchFile))
					{
						//没有指定特征文件或目录下存在指定的特征文件

						//最佳匹配或重复匹配，如果匹配数目保持1则为最佳匹配
						ASSERT(!item.bMatchType);
						item.nMacthCount++;
						item.MatchGame.push_back(pGame);
						BestMatchGame.insert(std::make_pair<long, size_t>(pGame->GID, nDir));

						if (item.nMacthCount == 1) {
							//第一次匹配
							bool bMatched = false;
							for (size_t n = 0; n < dirs.size(); n++) {
								if (dirs[n].pGame && dirs[n].pGame->GID == pGame->GID) {
									bMatched = true;
									break;
								}
							}
							//没有匹配项则可以缺省为匹配
							if (!bMatched) {
								item.pGame = pGame;
							}
						} else {
							//重复匹配，缺省未匹配
							item.pGame.reset();
						} 
					}
				}
			}
		}
	}
	
	if (WAIT_OBJECT_0 != WaitForSingleObject(pDlg->m_hExited, 0))
		::PostMessage(pDlg->m_hWnd, WM_MSG, 2, 0);

	CoUninitialize();

	return 0;
}

void CPiLiAddDlg::OnDestroy()
{
	SetEvent(m_hExited);
	if (m_hThread != NULL)
	{
		SetEvent(m_hExited);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	AreaInfos.clear();
	VDiskInfos.clear();

	for (int nIdx=0; nIdx<m_cboClass.GetCount(); nIdx++)
	{
		char* pGuid = reinterpret_cast<char*>(m_cboClass.GetItemData(nIdx));
		delete []pGuid;
	}

	CDialog::OnDestroy();
}

LRESULT CPiLiAddDlg::OnMessage(WPARAM wParam, LPARAM lParam)
{
	std::vector<tagItem>::iterator it =	m_lstDir.begin();
	if (wParam == 1) {
		m_bProgress.SetPos(lParam);		
	} else if (wParam == 0) {
		m_lstCtrl.SetItemCount(m_lstDir.size());
		m_bProgress.ShowWindow(SW_SHOW);
		m_lstCtrl.RedrawWindow();
		m_bProgress.SetRange32(0, m_lstDir.size());
	} else if (wParam == 2) {
		m_lstCtrl.RedrawWindow();
		int nMatch = 0, nBestMatch = 0, nRepMatch = 0, nUnMatch = 0;
		for (; it != m_lstDir.end(); it++) {
			if (it->bMatchType) {
				nMatch++;
			} else {
				switch (it->nMacthCount) {
				case 0:		nUnMatch++;		break; 
				case 1:		nBestMatch++;	break; 
				default:	nRepMatch++;	break; 
				}
			}
		}

		m_bProgress.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EXECUTE)->SetWindowText("执行搜索");

		CString szMsg;
		szMsg.Format("搜索结果:精确匹配数:%d, 重复匹配数:%d,最佳匹配数:%d,不能匹配数:%d",
			nMatch, nRepMatch, nBestMatch, nUnMatch);
		SetDlgItemText(IDC_TIP, szMsg);

		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDC_BROWSE_DIR)->EnableWindow(TRUE);

		m_bCanClick = TRUE;


		m_bMatchSortDesc = !m_bMatchSortDesc;
		std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
			FMatch<tagItem>(m_bMatchSortDesc));
		
		m_lstCtrl.RedrawWindow();
	}

	return 0;
}

std::string CPiLiAddDlg::GetAIDByName(std::string& Name)
{
	i8desk::AreaInfoMapItr it = AreaInfos.begin();
	for (; it != AreaInfos.end(); it ++)
	{
		if (Name == it->second->Name)
		{
			return it->first;
		}
	}
	return "";
}

BOOL CPiLiAddDlg::ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType)
{
	CLEAR_CHAR_ARRAY(RtType.VID);

	int nLen = lstrlen(RT_VDISK);
	RtType.RunType = 0;
	if (szText == RT_UNKNOWN)
	{
		RtType.RunType = 0;
		return TRUE;
	}
	else if (szText == RT_LOCAL) {
		RtType.RunType = 1;
		return TRUE;
	} 
	else if (szText == RT_DIRECT) {
		RtType.RunType = 3;
		return TRUE;
	} 
	else if (szText.Left(nLen) == RT_VDISK && szText.GetLength() > nLen) {
		RtType.RunType = 2;
		char CliDrv = szText.GetAt(nLen);
		i8desk::VDiskInfoMapItr it = VDiskInfos.begin();
		for (; it != VDiskInfos.end(); it++) {
			if (it->second->CliDrv == CliDrv) {
				strcpy(RtType.VID, it->second->VID);
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CPiLiAddDlg::OnBnClickedOk()
{
	std::string ErrInfo;
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	BOOL bDirectRun = FALSE; //检测是否为不更新直接运行方式，以不替换客户端路径
	i8desk::GameRunTypeList m_lstRT;
	for (int nIdx=0;nIdx<m_lstRunType.GetItemCount(); nIdx++)
	{
		i8desk::tagGameRunType RtType;
		RtType.AreaName = m_lstRunType.GetItemText(nIdx, 0);
		strcpy(RtType.AID, GetAIDByName(RtType.AreaName).c_str());
		RtType.GID = 0;
		if (ParaseAreaRTTexT(m_lstRunType.GetItemText(nIdx, 1), RtType)) {
			m_lstRT.push_back(RtType);
			if (RtType.RunType == ERT_DIRECT)
				bDirectRun = TRUE;
		}
	}

	CWaitCursor wc;
	std::vector<tagItem>::iterator it =	m_lstDir.begin();
	CString svrPath = pDlg->GetI8DeskSvrPath();
	DWORD gid = 0; 
	BYTE buf[10240] = {0};
	DWORD dwSize = 0;

	std::string guid = reinterpret_cast<char*>(m_cboClass.GetItemData(m_cboClass.GetCurSel()));
	std::string drv = pDlg->m_pDbMgr->GetOptString(OPT_M_GAMEDRV, "E");
	for (; it != m_lstDir.end(); it++)
	{
		i8desk::GameInfo_Ptr pGame;
		if (it->nMacthCount >= 1) {
			pGame = it->pGame;
		} else if (!it->exe.IsEmpty()) {
			pGame = i8desk::GameInfo_Ptr(new i8desk::GameInfo);
			pGame->GID = GetNextID();
			m_IDArray.insert(pGame->GID);
			i8desk::SAFE_STRCPY(pGame->Name, it->dir.Mid(it->dir.ReverseFind('\\')+1));
			i8desk::SAFE_STRCPY(pGame->DefClass, CLASS_WL_GUID);
			i8desk::SAFE_STRCPY(pGame->GameExe, it->exe.Mid(it->dir.GetLength() + 1));
			pGame->DownPriority = 1;
		}

		if (pGame)
		{
			if (pGame->GID >= MIN_IDC_GID)
			{
				pGame->AutoUpt = 1;
			}
			else
			{
				i8desk::SAFE_STRCPY(pGame->DefClass, guid.c_str());
				pGame->Priority = 3;
			}
			i8desk::SAFE_STRCPY(pGame->SvrPath, it->dir);
			if (!STRING_ISEMPTY(pGame->SvrPath) && LAST_CHAR(pGame->SvrPath) != '\\')
				i8desk::SAFE_STRCAT(pGame->SvrPath, "\\");
			pGame->AddDate = _time32(NULL);
			pGame->Status = TRUE;

			i8desk::SAFE_STRCPY(pGame->CliPath, pGame->SvrPath);
			if (!bDirectRun && drv.size() && pDlg->IsRunOnServer())
				pGame->CliPath[0] = drv[0];

			DWORD rgid = 0;
			bool  bIsLocal = false;
			if (!pDlg->m_pDbMgr->CheckGameNameCheck(pGame->Name, 0, ErrInfo, rgid, bIsLocal))
			{
				wc.Restore();
				AfxMessageBox(ErrInfo.c_str());
				return ;
			}

			if (rgid >= MIN_IDC_GID && !bIsLocal)
			{
				if (!pDlg->m_pDbMgr->ModifyGame(*pGame, ErrInfo) ||
					!pDlg->m_pDbMgr->SetGameRunType(pGame->GID, m_lstRT, ErrInfo))
				{
					continue ;
				}
			}
			else if (rgid == 0)	//添加没有重复的游戏
			{
				if (!pDlg->m_pDbMgr->AddGame(*pGame, ErrInfo) ||
					!pDlg->m_pDbMgr->SetGameRunType(pGame->GID, m_lstRT, ErrInfo))
				{
					continue;
				}
			}
			//save icon.
			std::string file = pGame->SvrPath;
			if (*file.rbegin() != '\\')
				file += "\\";
			file += pGame->GameExe;
 
			HICON hIcon =  i8desk::LoadIconFromFile(file.c_str());
			if (hIcon == NULL) {
				hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LOCRES));
			}
			
			i8desk::SaveIconToBuffer(hIcon, buf, dwSize);
			DestroyIcon(hIcon);

			pDlg->m_pDbMgr->AddIcon(pGame->GID, reinterpret_cast<char *>(buf), dwSize, ErrInfo);
		}
	}
	wc.Restore();
	OnOK();
}

void CPiLiAddDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	if (!m_bCanClick)
		return ;

	int nSelItem = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1)	
		return ;
	ASSERT(nSelItem < (int)m_lstDir.size());

	tagItem* pGame = &m_lstDir[nSelItem];
	if (pGame->bMatchType)
		return;

	CPoint pt( ::GetMessagePos() );
	ScreenToClient(&pt);

	CRect rc;
	m_lstCtrl.GetSubItemRect(nSelItem, 2, LVIR_BOUNDS, rc);
	m_lstCtrl.ClientToScreen(&rc);	
	ScreenToClient(&rc);
	if (!rc.PtInRect(pt))
		return ;
	if (pGame->nMacthCount == 0)
	{
		CFileDialog Open(TRUE, "", "", 4|2, "所有文件(*.*)|*.*||");	
		Open.m_ofn.lpstrInitialDir = pGame->dir;
		if (Open.DoModal() == IDOK)
		{
			CString file = Open.GetPathName();
			if (StrNCmpI(pGame->dir, file, pGame->dir.GetLength()) != 0)
			{
				AfxMessageBox("执行文件和目录路径不一致.");
			}
			pGame->exe = file;
			m_lstCtrl.RedrawItems(nSelItem, nSelItem);
		}
		else
		{
			pGame->exe = "";
			m_lstCtrl.RedrawItems(nSelItem, nSelItem);
		}
	}
	else
	{
		m_cboGame.ResetContent();
		m_cboGame.AddString("不匹配");
		for (size_t idx=0; idx<pGame->MatchGame.size(); idx++)
			m_cboGame.AddString(pGame->MatchGame[idx]->Name);
		m_cboGame.SelectString(-1, m_lstCtrl.GetItemText(nSelItem, 2));
		rc.OffsetRect(-1, -1);
		m_cboGame.MoveWindow(&rc);
		m_cboGame.ShowWindow(SW_SHOW);
		m_cboGame.BringWindowToTop();
		m_cboGame.SetFocus();
		m_cboGame.ShowDropDown(TRUE);
	}
}

void CPiLiAddDlg::OnCbnKillfocusCombo1()
{
	m_cboGame.ShowWindow(SW_HIDE);

	int nSelItem = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1 || nSelItem >= (int)m_lstDir.size())	
		return ;

	bool ok = false;
	tagItem& Game = m_lstDir[nSelItem];

	CString Text;
	if (m_cboGame.GetCurSel() == -1) {
		goto Exit;
	}
	
	m_cboGame.GetLBText(m_cboGame.GetCurSel(), Text);

	//检查是否有目录已经匹配过选中的游戏
	for (int i = 0; i < m_lstCtrl.GetItemCount(); i++) {
		if (nSelItem == i) {
			continue;
		}
		if (m_lstCtrl.GetItemText(i, 2) == Text) {
			CString msg;
			msg.Format(_T("目录[%s]已经匹配了游戏[%s],不允许重复匹配"), 
				(LPCTSTR)m_lstCtrl.GetItemText(i, 0), (LPCTSTR)Text);
			AfxMessageBox(msg);
			goto Exit;
		}
	}

	//匹配游戏
	for (size_t idx=0; idx < Game.MatchGame.size(); idx++)
	{
		if (Text == Game.MatchGame[idx]->Name)
		{
			Game.pGame = Game.MatchGame[idx];
			ok = true;
			break;
		}
	}
	
Exit:
	if (!ok) {
		Game.pGame.reset();
	}

	m_lstCtrl.RedrawItems(nSelItem, nSelItem);
	m_lstCtrl.SetFocus();
}

void CPiLiAddDlg::OnCbnCloseupCombo1()
{
	OnCbnKillfocusCombo1();
}

void CPiLiAddDlg::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	*pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{

#define OK_MATCH_COLOR		RGB(0, 0, 0)
#define NO_MATCH_COLOR		RGB(255, 0, 0)
#define BEST_MATCH_COLOR	RGB(0, 127, 127)
#define MULTI_MATCH_COLOR	RGB(127, 127, 0)

		int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
		if (m_lstDir[nItem].bMatchType) {
			pLVCD->clrText = OK_MATCH_COLOR;	
		} else {
			switch (m_lstDir[nItem].nMacthCount) {
			case -1:	 
				//pLVCD->clrText = OK_MATCH_COLOR;	
				break;
			case 1:	 
				pLVCD->clrText = BEST_MATCH_COLOR;	
				break;
			case 0:	 
				pLVCD->clrText = NO_MATCH_COLOR;	
				break;
			default:	
				pLVCD->clrText = MULTI_MATCH_COLOR;	
				break;
			}
		}

		*pResult = CDRF_DODEFAULT;
	}
}



void CPiLiAddDlg::OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (!m_bCanClick) {
		return;
	}

	using std::tr1::placeholders::_1;
	using std::tr1::placeholders::_2;

	switch (pNMLV->iSubItem) {
	case 0:
		if (m_bDirSortDesc)
			std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
				std::tr1::bind(std::greater<CString>(), 
					std::tr1::bind(&tagItem::dir, _1),
					std::tr1::bind(&tagItem::dir, _2)));
		else 
			std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
				std::tr1::bind(std::less<CString>(), 
					std::tr1::bind(&tagItem::dir, _1),
					std::tr1::bind(&tagItem::dir, _2)));
		m_bDirSortDesc = !m_bDirSortDesc;
		m_lstCtrl.RedrawWindow();
		break;
	case 1:
		std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
			FMatch<tagItem>(m_bMatchSortDesc));
		m_bMatchSortDesc = !m_bMatchSortDesc;
		m_lstCtrl.RedrawWindow();
		break;
	case 2:
		std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
			FMatchResult<tagItem>(m_bResultSortDesc));
		m_bResultSortDesc = !m_bResultSortDesc;
		m_lstCtrl.RedrawWindow();
		break;
	case 3:
		std::stable_sort(m_lstDir.begin(), m_lstDir.end(),
			FGid<tagItem>(m_bGidSortDesc));
		m_bGidSortDesc = !m_bGidSortDesc;
		m_lstCtrl.RedrawWindow();
		break;
	default:
		ASSERT(0);
	}

}

void CPiLiAddDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LV_ITEM* pItem = &pDispInfo->item;
	ASSERT(pItem->iItem < (int)m_lstDir.size());

	if (pItem->mask & LVIF_TEXT)
	{
		tagItem& Game = m_lstDir[pItem->iItem];
		switch (pItem->iSubItem)
		{
		case 0:
			lstrcpyn(pItem->pszText, (LPCTSTR)Game.dir, pItem->cchTextMax);
			break;
		case 1: {
			if (Game.bMatchType) {
				lstrcpyn(pItem->pszText, _T("精确匹配"), pItem->cchTextMax); 
			} else {
				switch (Game.nMacthCount) {
				case -1: pItem->pszText[0] = 0;										  break;
				case 0:	 lstrcpyn(pItem->pszText, _T("没有匹配"), pItem->cchTextMax); break;
				case 1:	 lstrcpyn(pItem->pszText, _T("最佳匹配"), pItem->cchTextMax); break;
				default: lstrcpyn(pItem->pszText, _T("重复匹配"), pItem->cchTextMax); break;
				}
			}
			break; }
		case 2:
			if (Game.pGame)
				lstrcpyn(pItem->pszText, Game.pGame->Name, pItem->cchTextMax);
			else 
				lstrcpyn(pItem->pszText, (LPCTSTR)Game.exe.Mid(Game.exe.ReverseFind('\\')+1), pItem->cchTextMax);
			break;
		case 3:
			if (Game.pGame)
				::_stprintf_s(pItem->pszText, pItem->cchTextMax, _T("%d"), Game.pGame->GID);
			else 
				pItem->pszText[0] = 0;
			break;
		}
	} 

	*pResult = 0;
}
