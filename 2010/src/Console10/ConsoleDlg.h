// ConsoleDlg.h : 头文件
//

#pragma once

#include "afxcmn.h"

#include <vector>
#include <set>
#include <fstream>

#include "explorer.h"
#include "GameListCtrl.h"
#include "DragDropListCtrl.h"
#include "SvrMonitorDailog.h"
#include "MonitorPage.h"

inline void SetDlgItemFont(CDialog *pDlg, UINT nCtrlID, const LOGFONT *lf)
{
	CFont font;
	font.CreateFontIndirect(lf);
	pDlg->GetDlgItem(nCtrlID)->SetFont(&font);
}

typedef enum _Tree_Node_
{
	TN_UNKNOWN,
	TN_CENTER,
	TN_NETBAR,
	TN_MACHINE,
	TN_DOWNLOAD,
	TN_NETWORKUPDATE,
	TN_CHILD = 100,
	TN_VALUE,
	TN_NODOWN,
	TN_ADDNEARLY,
	TN_NEEDUPDATE,
}_Tree_Node_;



#ifdef USE_VNCVIEWER_DLL
typedef int (WINAPI* PFNVNCVIEWER)(LPCSTR, LPCSTR, int);
#endif 

class CConsoleDlg : public CDialog
{
public:
	CConsoleDlg(CWnd* pParent = NULL);
	~CConsoleDlg();

	enum { IDD = IDD_CONSOLE_DIALOG };

	friend class CGameInfoDlg;
	friend class CAddClientDlg;
	friend class CPiLiAddDlg;

public:
	i8desk::CSimpleSocket	*m_pSocket;
	i8desk::CDbMgr			*m_pDbMgr;
	i8desk::CCliMgr			*m_pCliMgr;

	i8desk::CSimpleSocket	*m_pGameMgrSocket;
	i8desk::CDownloadMgr	*m_pDownloadMgr;


#ifdef USE_VNCVIEWER_DLL
	HMODULE m_hVNCviewer;
	PFNVNCVIEWER m_pfnVNCviewer;
#endif 
	std::set<DWORD>			m_IDArray;	
	i8desk::GameInfoMap		m_GameInfos;
	i8desk::GameInfoVector	m_CurGameInfos;

	//保存要排除显示的类别ID
	std::set<std::string>	m_ClassFilter;
	
	i8desk::MachineMap		m_Machines;
	i8desk::MachineVector	m_CurMachines;

	i8desk::VDiskInfoMap    m_VDiskInfos; 

	i8desk::DefClassMap		m_ClassInfos;
	i8desk::AreaInfoMap		m_AreaInfos;

private:
	int						m_nLoginFreeTimeout;
	int						m_bShowUploadTotalRate;

	CMarkup					m_xmlDoc;
	CMarkup					m_xmlOnline;
	std::string				m_GameVersion;
	std::string				m_MachineVersion;
	std::string				m_OnlineVersion;
	i8desk::std_string		m_strMainServerName;
	std::vector<CString>    m_vecServerIPs;

	CString					m_SvrIpAddr;
	DWORD					m_dwPort;
	std::vector<bool>		m_vecSort;

	HANDLE					m_hThread;
	HANDLE					m_hThreadNotify;
	HANDLE					m_hEixted;

	HICON					m_hIcon;

	CImageList				m_ilToolbar;
	CToolBar				m_wndToolbar;

	CImageList				m_ilToolbar2;
	CToolBar				m_wndToolbar2;

	CStatusBar				m_wndStatusbar;

	CImageList				m_ilTreeCtrl;	
	CTreeCtrl				m_treCtrl;

	CListCtrl				m_lstLog;
	CListCtrl				m_lstRepairGameLog;

	HACCEL					m_hAccel;
	_Tree_Node_				m_LastNodeType;

	CSvrMonitorDailog		m_wndSvrMonitor;

	CDockHtmlCtrl			m_wndSystemNews;

	CExplorer				m_WebPage;
	CGameListCtrl			m_lstCtrl;
	CListCtrl				m_lst3up;
//	CDragDropListCtrl		m_lst3up;
	CMonitorPage			m_wndMonitorPage;

	std::ofstream			m_logf;

	DWORD					m_DownloadTotalRate;
	DWORD					m_UploadTotalRate;

	i8desk::ulonglong		m_ullNoDownSize;	// 未下载游戏
	i8desk::uint64			m_ullIdcI8PlaySize; // 增值游戏数目
	i8desk::uint64			m_ullNeedUptSize;	// 有更新游戏数目
	i8desk::uint64			m_ullIdcAddInNearlySize; // 中心近期添加游戏 

	std::map<int, long>		m_map3DownGid;		// 三层下载项与游戏gid相关联

	bool					m_bShowAds;			// 是否显示公告

public:
	void		SelectPage(LPCTSTR lpszGroup, LPCTSTR lpszItem);
	void		SelectPage(_Tree_Node_ nodeType);
	void		DownGames(i8desk::GameInfoVector& GameInfos);
	BOOL		DelGames(i8desk::GameInfoVector& GameInfos);
	
	// 获取主服务器名称
	const i8desk::std_string &GetMainSevrerName() const 
	{ return m_strMainServerName; }

	// 获取主服务器IP
	const std::vector<CString> &GetMainServerIP() const
	{ return m_vecServerIPs; }

	BOOL		IsRunOnServer() { return IsLocMachineIP(m_SvrIpAddr); }
	bool		GetLocalIP(std::vector<CString> &vecLocalIP);
	BOOL		IsLocMachineIP(const CString& szSvrIp = "");
	CString		GetI8DeskSvrPath();
	bool		ProcessAddModifyGame(i8desk::GameInfo *pGame, bool bAdd, bool bDrop = false);
	void		FormatGameRuntype(const i8desk::GameInfo& ginfo, std::string& desc);
	void		SafeCenterConfig();

	CString		GetTreeSelNodeText();
	int			FindGame(size_t nCur, bool bByName, bool bByDown, const CString& strFind);
	void		FindSelectall(const std::set<int>& list);
	i8desk::std_string GetNameFromGID(DWORD gid);
	void		GetAllSelectGame(std::set<DWORD>& setGame);


	void		InsertLog(LPCTSTR szLog, ...);

private:
	void		Update3upStatus(void);
	void		UpdateVDiskStatus(void);
	void		UpdateSvrStatus(void);
	void		UpdateCliStatus(void);
	void		UpdateServerStatus(void);
	void		UpdateDiskStatus(void);
	void		UpdatePlugList();

	CString		GetGameClass(const i8desk::GameInfo& pGameInfo);
	CString		GetMachineArea(const i8desk::Machine& pMachine);

	_Tree_Node_ GetParentNodeType();
	_Tree_Node_ GetParentNodeType(HTREEITEM hItem);

	DWORD		GetNextID();
	void		InitLayout();
	void		AddTreeLayout();
	void		SetListCtrlHeader(_Tree_Node_ nodeType);
	bool		SelectTreeNode(_Tree_Node_ nodeType, HTREEITEM hItem);
	void		SetPanleText();
	void		FreeObjectMemory();
	void		SaveLog();

	//第一个参数表示需要刷新的数据库表的数据,第二个参数表示是否是后台线程调用
	void		UpdateData(WPARAM wParam, bool bIsBackground = false, bool realtime = false);
	bool		UpdateStatus(DWORD& flag);
	void		UpdateConnectStatus(void);


	// 调整三层更新排队任务优先级
	bool		AdjustTask(long lDestIndex, const CDWordArray &arr);

	// 调整下载菜单
	int			_JustDownMenu(int nCurItem, CMenu *pMenu);

	static		UINT __stdcall WorkThread(LPVOID lpVoid);

	void		ShowView(CWnd *pView);

	// 设置游戏更新方式
	void		_SetGameAutoUpdate(i8desk::GameInfo_Ptr ptr, int nAuto);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	
	
public:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	
	afx_msg void OnGameAreaMgr();
	afx_msg void OnGameClassMgr();
	afx_msg void OnGameAddGame();
	afx_msg void OnGameModGame();
	afx_msg void OnGameDelGame();
	afx_msg void OnGameSyncGame();

	afx_msg void OnCliOpen();
	afx_msg void OnCliBoot();
	afx_msg void OnCliShutdown();
	afx_msg void OnCliRunExe();
	afx_msg void OnCliChkdsk();
	afx_msg void OnCliAddClient();
	afx_msg void OnCliDelcli();
	afx_msg void OnCliModArea();
	afx_msg void OnCliInstall();
	afx_msg void OnCliInstallProt();
	afx_msg void OnCliProtArea();
	afx_msg void OnCliRemove();
	afx_msg void OnCliViewSysInfo();
	afx_msg void OnCliViewProcInfo();
	afx_msg void OnCliViewSviInfo();
	afx_msg void OnCliRefresh();
	afx_msg void OnFileExit();

	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMonitorPage();
	afx_msg void OnSysMirrorServer();
	afx_msg void OnSysSyncTask();
	afx_msg void OnSysUserMgr();
	afx_msg void OnSysVDisk();
	afx_msg void OnSysFavorite();
	afx_msg void OnSysBootTask();
	afx_msg void OnAntiIndulgence();
	afx_msg void OnConfigWizard();
	afx_msg void OnSysOption();
	afx_msg void OnSysPlug();
	afx_msg void OnHelpAbout();
	
	afx_msg void OnGameOpenDir();
	afx_msg void OnGamePlAdd();
	afx_msg void OnGameMkIdx();
	afx_msg void OnGameCheckIdx();
	afx_msg void OnTSGame();
	afx_msg void OnGanmeRealts();
	afx_msg void OnSafeCenter();
	afx_msg void OnUpdateClientIndex();
	afx_msg void OnUpdateIconlistVersion();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDownDownGame();
	afx_msg void OnGameRefresh();
	afx_msg void OnDownSetManual();
	afx_msg void OnGameFindGame();
	afx_msg void OnSysBackup();
	afx_msg void OnDownStop();
	afx_msg void OnDownSuspendGame();
	afx_msg void OnDownRestartGame();	
	
	afx_msg void OnHelpContent();
	afx_msg void OnVedioHelp();
	afx_msg void OnExtraForum();
	afx_msg void OnHelpOnline();
	afx_msg void OnExtraRepair();
	afx_msg void OnDownFore();

	afx_msg void OnCliRemStart();
	afx_msg void OnCliRemStop();
	afx_msg void OnCliRemMonitor();
	afx_msg void OnViewCmpGame();
	afx_msg void OnGameViewinfo();
	afx_msg void OnDownRefreshpl();
	afx_msg void OnSvrLog();
	afx_msg void OnIeHistory();
	afx_msg void OnUDiskServer();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFilteClass(UINT id);

	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnDragMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDragEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTaskNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOfflineNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnServerLog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowGameInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatGame(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDownTop();
	afx_msg void OnDownBottom();
	afx_msg void OnSysPluginTools();
};


inline CConsoleDlg* GetConsoleDlg(void)
{
	return reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
}

inline i8desk::CDbMgr* GetDbMgr(void)
{
	return GetConsoleDlg()->m_pDbMgr;
}

inline i8desk::CDownloadMgr *GetDownloadMgr()
{
	return GetConsoleDlg()->m_pDownloadMgr;
}
