#ifndef __db_mgr_inc__
#define __db_mgr_inc__

#include <comutil.h>
#include <vector>
#include <map>
#include <algorithm>

#include "package.h"

#define CAP_URL_NAV				TEXT("网址导航")
#define CAP_DOT_CARD			TEXT("点卡商城")
#define URL_DOTCARD				TEXT("http://biz.i8cn.com/CybercafeClient/")
#define URL_URLNAVAGITOER		TEXT("http://www.zhaochina.com")
#define URL_PERSON				TEXT("http://desk.i8.com.cn/CybercafeClient/User/Main.aspx")
#define CAP_PERSON              TEXT("个人设置")

#define FAV_TYPE_WEB			TEXT("网站")
#define FAV_TYPE_VIDEO			TEXT("电影")
#define FAV_TYPE_MUSIC			TEXT("音乐")

typedef struct tagSTMultiChar
{
	WCHAR                        chChar;
	TCHAR                        szPy[3];
}STMultiChar;

//收藏夹信息
typedef struct tagFavInfo
{
	CString strType;			//类型
	CString strName;			//名字
	CString strUrl;				//网址
}FAVINFO;

//索引信息
typedef struct tagIdxHeahder
{
	DWORD  lowsize;
	DWORD  highsize;
	DWORD  resv3;
	DWORD  resv4;
	DWORD  resv5;
	DWORD  resv6;
	DWORD  xmlsize;
	WORD   resv8;
	WORD   blknum;
}tagIdxHeahder;

//游戏信息
typedef struct tagGameInfo
{
	HICON		hIcon;
	DWORD		gid;			//游戏id.
	DWORD		pid;			//游戏关联id.
	CString		Name;			//游戏名称
	CString		Py;				//游戏名称的拼音
	CString     MultiPy;        //多音字拼音
	int         nFindPos;       //匹配的位置
	CString		IdcClass;		//游戏类别
	CString		Exe;			//游戏运行的ＥＸＥ.(相对路径)
	CString		Param;			//运行的参数
	DWORD		Size;			//游戏的大小（以Ｋ为单位）
	bool		DeskLnk;		//是否显示到桌面快捷方式
	bool		Toolbar;		//是否显示到菜单工具栏
	CString		SvrPath;		//游戏服务端路径
	CString		CliPath;		//游戏客户端路径
	DWORD       Priority;		//游戏等级，用于删除点击率低的游戏，三层会需要
	DWORD		ShowPriority;	//游戏在菜单上的显示优先级
	CString		MatchFile;		//游戏自动搜索特征文件
	CString		SaveFilter;		//游戏存档信息
	DWORD		I8Play;			//增值活动游戏
	DWORD		IdcClick;		//游戏全网点率
	DWORD		SvrClick;		//游戏网吧点率
	CString		Url;			//游戏官网
	DWORD		RunType;		//游戏运行方式
	DWORD       dwNbVer;        //网吧服务器版本
	CString		VID;			//游戏所在虚拟盘ID
	CString     GameArea;       //专区URL
	CString     RegUrl;         //注册地址 
	CString     PayUrl;         //支付地址
	CString     VideoUrl;       //视频攻略
}GAMEINFO;

//虚拟盘信息
typedef struct tagVDiskInfo
{
	CString VID;				//虚拟盘GUID
	DWORD	IP;					//虚拟盘ip
	DWORD	Port;				//虚拟盘端口
	TCHAR	SvrDrv;				//服务端盘符
	TCHAR   CliDrv;				//客户端指定盘符
	DWORD	Serial;				//虚拟盘的序列号
	DWORD	Type;				//虚拟盘的类型
	DWORD	LoadType;			//虚拟盘的刷盘方式
	CString SID;				//镜像服务器的标识。
}VDISKINFO;

//多服务器列表配置
typedef struct tagSvrCfg
{
	CString Sid;
	CString SyncIP;
	CString VDiskIP;
	CString UptIP;
	DWORD	dwSyncType;
	DWORD   dwDestDrv;
	DWORD   dwBalanceType;
}SVRCFG;

CString GetPY(CString& sChinese);
DWORD CalBufCRC32(BYTE* buffer, DWORD dwSize);


//游戏排序方式，但这两种排序，都是增值游戏一定会排在前面。
#define SORT_WINDOWS	0	//按windows资源管理器默认方式排序
#define SORT_CLICK		1	//按游戏点击率排序
#define SORT_FINDPOS    2   //匹配位置

template<int N = SORT_WINDOWS>
struct Pred
{
	bool operator()(tagGameInfo* p1, tagGameInfo* p2)
	{
		int nLen1 = p1->Name.GetLength();
		int nLen2 = p2->Name.GetLength();
		int nLen = nLen1 > nLen2 ? nLen1 : nLen2;

		//增值放最前
		if (p1->I8Play || p2->I8Play)
		{
			if (p1->I8Play && p2->I8Play)
			{
				if (p1->I8Play == p2->I8Play)
					return p1->SvrClick > p2->SvrClick;
				return p1->I8Play > p2->I8Play;
			}
			if (p1->I8Play)
				return true;
			if (p2->I8Play)
				return false;
		}
		//其它按资源管理器的排列方式来排。(升序)
		return StrNCmpI(p1->Name, p2->Name, nLen) < 0;
	}
};

template<>
struct Pred<SORT_CLICK>
{
	bool operator()(tagGameInfo* p1, tagGameInfo* p2)
	{
		//增值放最前
		if (p1->I8Play || p2->I8Play)
		{
			if (p1->I8Play && p2->I8Play)
			{
				if (p1->I8Play == p2->I8Play)
					return p1->SvrClick > p2->SvrClick;
				return p1->I8Play > p2->I8Play;
			}

			if (p1->I8Play)
				return true;
			if (p2->I8Play)
				return false;
		}

		//按点击数的升序来排
		return p1->SvrClick > p2->SvrClick;
	}
};

template<>
struct Pred<SORT_FINDPOS>
{
	bool operator()(tagGameInfo* p1, tagGameInfo* p2)
	{
		//按搜索位置升序来排
		if (p1->nFindPos != p2->nFindPos)
		{
			return p1->nFindPos < p2->nFindPos;
		}

		int nLen1 = p1->Name.GetLength();
		int nLen2 = p2->Name.GetLength();
		int nLen = nLen1 > nLen2 ? nLen1 : nLen2;
		return StrNCmpI(p1->Name, p2->Name, nLen) < 0;
	}
};

template <int N>
void SortGameList(std::vector<tagGameInfo*>& GameList)
{
	std::sort(GameList.begin(), GameList.end(), Pred<N>());

	//方便调试时查看排列是否正确
// 	for (size_t idx=0; idx<GameList.size(); idx++)
// 	{
// 		TCHAR szLog[MAX_PATH] = {0};
// 		_stprintf_s(szLog, TEXT("%s-%d-%d\r\n"), (LPCTSTR)GameList[idx]->Name, 
// 			GameList[idx]->SvrClick, GameList[idx]->I8Play);
// 		OutputDebugString(szLog);
// 	}
}

class CLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	CLock()			{ InitializeCriticalSection(&m_lock);	}
	~CLock()		{ DeleteCriticalSection(&m_lock);		}
	void Lock()		{ EnterCriticalSection(&m_lock);		}
	void UnLock()	{ LeaveCriticalSection(&m_lock);		}
};

void AfxWriteLog(LPCTSTR lpszLog, bool bAddDate = true);
void AfxWriteLog2(LPCTSTR lpszLog, ...);

class CDbMgr
{
public:
	~CDbMgr();
	static CDbMgr* getInstance();
	static void CheckCliService();
	static void CheckVDiskService();
	static void WriteI8deskProtocol();

public:
	std::map<CString, CString>			m_mapSysOpt;	//系统选项列表
	std::map<CString, tagVDiskInfo*>	m_mapVDisk;		//虚拟磁盘列表
	std::map<DWORD, tagGameInfo*>		m_mapGameInfo;	//游戏信息列表
	std::vector<tagGameInfo*>			m_toolGameInfo;	//工具栏上的游戏列表。
	std::vector<tagFavInfo*>			m_FavList;		//收藏夹列表

	std::vector<CString>				m_GameRoomTab;	//游戏大厅的TAB列表
	std::vector<CString>				m_ChatRoomTab;	//聊天工具的TAB列表
	std::vector<CString>				m_SoftwareTab;	//常用软件的TAB列表

	std::vector<CString>				m_BrowseTab;	//浏览器的TAB列表
	std::vector<CString>				m_DotCardTab;	//点卡商城的TAB列表
	std::vector<CString>				m_VideoTab;		//影视/音乐的TAB列表

public:
	void            WaitExit();
	void            RefreshOfflineData();
	void            LoadMultiChar();
	DWORD           Idxfile_GetVersion(LPCWSTR file);

	//查询系统选项
	bool	        GetBoolOpt(LPCTSTR lpOptName, bool nOptDef = false);
	long	        GetIntOpt(LPCTSTR lpOptName, long nOptDef = 0);
	__int64         GetInt64Opt(LPCTSTR lpOptName, __int64 nOptDef = 0);
	CString         GetStrOpt(LPCTSTR lpOptName, LPCTSTR lpOptDef = TEXT(""), bool bEmptyIsDef = false);

	bool	        IsRefVDiskFinish();
	CString         TransChToGBK(LPCTSTR lpszSouce);

	//系统配置文件
	CString         GetIniFile();
	CString         LoadIpAddr();
	void            SaveIpAddr(CString str);

	CString	        GetUptSvrList();
	void            GetMacAddr(LPSTR buf, int nSize);
	void            GetMacAddr(LPTSTR buf, int nSize);
	void            GetGameNamePy(tagGameInfo* pGameInfo);

	//刷指定虚拟盘
	bool            RefreshVDisk(TCHAR chCltDrv);

	//对象查找函数
	tagGameInfo*	FindGameInfo(DWORD gid);
	tagFavInfo*		FindFavInfo(CString name);
	tagVDiskInfo*	FindVDiskInfo(CString vid);
	//Dno't DestroyIcon return hIcon.(if hIcon != NULL)
	HICON			FindIconOfGame(DWORD gid);

	//增值或广告的控制辅助函数
	CString			GetDotCardCap()		{ return m_strDotCardCap;	}
	CString			GetDotCardUrl()		{ return m_strDotCardUrl;	}
	CString			GetNavigateCap()	{ return m_strNavUrlCap;	}
	CString			GetNavigateUrl()	{ return m_strNavUrl;		}
	CString			GetPersonUrl()		{ return m_strPersonUrl;	}
	CString         GetPersonTabText()  { return m_strPersonTabText;}
	long			GetDefaultPage()	{ return m_nDefaultPage;	}
	bool			GetIsShowGongGao()	{ return m_bShowGongGao;	}
	long			GetRBPopAdv()		{ return m_nRBPopAdv;		}
	bool			GetPreGameUrl(CString Name, CString& url)	
	{
		url = TEXT("");
		std::vector<std::pair<CString, CString>>::iterator it = m_GamePreUrlList.begin();
		for (; it != m_GamePreUrlList.end(); it++)
		{
			if (Name.CompareNoCase(it->first) == 0)
			{
				url = it->second;
				return true;
			}
		}
		return false;
	}

private:
	HANDLE m_hThread;
	HANDLE m_hExit;
	bool CheckConnect(DWORD ip, WORD port);
	static UINT __stdcall ThreadProc(LPVOID lpVoid);

private:
	CDbMgr();
	char* m_pIconInfo;
	std::map<TCHAR, WCHAR*> m_mapMultiChar;
	std::map<DWORD, char*> m_mapIconInfo;
	std::vector<tagSvrCfg*> m_SvrCfg;
	std::vector<std::pair<CString, CString>> m_GamePreUrlList;
	CString  m_strDotCardCap;
	CString  m_strDotCardUrl;
	CString  m_strNavUrlCap;
	CString  m_strNavUrl;
	CString	 m_strPersonUrl;
	CString  m_strPersonTabText;
	long	 m_nDefaultPage;
	bool	 m_bShowGongGao;
	long	 m_nRBPopAdv;

	//加载数据
	CString GetDataDir();
	void LoadSysOpt();
	void LoadVDisk();
	void LoadFavorite();
	void LoadGameInfo();
	void LoadIconInfo();
	void ParseInValue();	//解析增值控制数据
	void ParseSvrList();

	HICON FindIconOfGame_(DWORD gid);   

	//释放数据所占的内存
	void FreeSysOpt();
	void FreeVDisk();
	void FreeFavorite();
	void FreeGameInfo();
	void FreeIconInfo();
	void FreeSvrList();
};

class CRptData
{
public:
	CRptData();
	~CRptData();
public:
	void ReprotDataToServer(CDockNavBar* pNav, std::map<DWORD, DWORD>& GameClick, bool bAck = true);
private:
	bool ConnectToServer();
	bool SendDataToServer(char* pData, int len, bool bAck = false);
	void CloseServer();
	
private:
	SOCKET m_sck;
};

inline void i8desk_GetShareData(bool& bOnline, bool& bProt)
{
	bOnline = bProt = false;
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("I8MENU_SHARE"));
	if (hFileMap != NULL)
	{
		DWORD* pShareData = (DWORD*)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, MAX_PATH);
		if (pShareData != NULL)
		{
			bOnline = (pShareData[0] == 1);
			bProt   = (pShareData[1] == 1);
			UnmapViewOfFile(pShareData);
		}
		CloseHandle(hFileMap);
	}
};


inline CDbMgr* AfxGetDbMgr()
{
	return CDbMgr::getInstance();
}

class CPersonMgr
{
public:
	~CPersonMgr() 
	{
		if (m_hThread != NULL && m_hExited != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hExited);
			CloseHandle(m_hThread);
		}
		DeleteFavOfAdded();
//		ChangeDisplaySetting(m_nDefWidth, m_nDefHeight);
		CoUninitialize();
	}
public:
	static CPersonMgr* getInstance()
	{
		static CPersonMgr self;
		return &self;
	}
	enum RES_AddGame
	{
		RES_AddGame_OK,
		RES_AddGame_NotLogin,
		RES_AddGame_Failed
	};
	void NoOperator();
	void GetPersonState();
	RES_AddGame AddGameToFavList(DWORD gid);
	void GetPersonFavorite(DWORD dwNum = -1, DWORD dwDefault = 1);
	void GetPersonalizationSet();
private:
	CPersonMgr() : m_dwUserId(-1), m_hThread(NULL), m_hExited(NULL)
	{
		CoInitialize(NULL);
		m_nDefWidth = GetSystemMetrics(SM_CXSCREEN);
		m_nDefHeight = GetSystemMetrics(SM_CYSCREEN);
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, NULL);
	}
	LPCTSTR GetCookie();
	void ChangeDisplaySetting(int nWidth, int nHeight);
	void DeleteFavOfAdded();
	HRESULT CreateShortcutToURL(LPCTSTR pszURL, LPCTSTR pszLinkFile);
	void ChangeMouseSpeed(int nSpeed);
	void InstallInput(CString str);

	static UINT __stdcall ThreadProc(LPVOID lpVoid);
private:
	DWORD m_nDefWidth, m_nDefHeight;
	DWORD m_dwUserId;
	HANDLE m_hThread;
	HANDLE m_hExited;
	CString m_strUserName;
	CString m_strLgnIp;
	CString m_strLgnDate;
	std::vector<CString> m_favlst;
};

inline CPersonMgr* AfxGetPersonMgr()
{
	return CPersonMgr::getInstance();
}

#endif