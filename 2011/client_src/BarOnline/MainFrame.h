#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "GongGaoDlg.h"
#include "UptPrgDlg.h"
#include "HotGamePanel.h"
#include "DockDlg.h"
#include "FavPanel.h"
#include "RBPopAdvDlg.h"
#include "UI/I8SkinCtrl/Header.h"
#include "UI.h"
#include "DockBrowseBar.h"
#include "DockFinderBar.h"
#include "DockStatusBar.h"

static int WM_EXEC_START_PARAM		= RegisterWindowMessage(TEXT("_i8desk_exec_start_param"));

typedef struct tagMenuItemEx
{
	DWORD	dwId;
	int		dwGid;
	TCHAR	szText[MAX_PATH];
}tagMenuItemEx;

class CMainFrame : public CFrameWnd
{
	friend class CHotGamePanel;

	enum	FrameWndMode
	{
		FWM_UNKNOWN = 0,
		FWM_GAME	= 1,
		FWM_IE		= 2,
		FWM_PERSON	= 3
		//FWM_DOCK	= 4
	};

public:
	CMainFrame(LPCTSTR lpszWindowName = NULL);
	virtual ~CMainFrame();
public:
	//创建主界面上所有的控件
	void CreateControl();
	void SetToolbarGame();

	//显示主窗口的三种模式
	void SetFrameMode(FrameWndMode FrameMode);
	CRect GetWndDefRect();	//得到窗口默认位置
	void  ShowUrlFavorite(BOOL bIsLeft);
	void  ClickUrlFavorite(CString szUrl);
	void  HideUrlFavorite();
	void  RefreshUrlFavorite();
	BOOL m_bLeftUrlFav;
	BOOL m_bRightUrlFav;
	BOOL m_bShowDock;
	BOOL m_bDockMode;
	
	//托盘菜单处理
	void ShowTray(DWORD dwMessage);

	//显示公告,更新，右下角广告弹出窗口。
	CGongGaoDlg*	m_pDlgGongGao;
	CUptPrgDlg*		m_pDlgUptPrg;
	CRBPopAdvDlg*	m_pDlgRbPopAdv;
	CDockDlg*		m_pDlgDock;
	tagMenuItemEx*	m_pPopMenu;
	tagMenuItemEx*  m_pTrayMenu;
	void ShowGongGao();
	void ShowUptPrg(tagGameInfo* pGameInfo);
	void ShowRBPopAdv();
	void ShowDockWnd();

	//窗口控制
	BOOL         IsMaxWindow();
	void         ShowMaxWindow();
	void         ShowNormalWindow();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnThemeChanged();
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);

	//处理主界面上控件的事件
	afx_msg LRESULT HandleNavClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleBrowseClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleFinderClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleTabClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleStatusClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleTrayMessage(WPARAM wParam, LPARAM lParam);

	//处理命令行参数
	//wParam:1表示是第二个实例发送来的消息，0表示是自己内部发来的消息。
	//命令行的内容，己经写到共享内存中（在发送消息前）
	afx_msg LRESULT HandleCmdLine(WPARAM wParam, LPARAM lParam);

	//处理动态刷新离线数据
	afx_msg LRESULT HandleRefreshData(WPARAM wParam, LPARAM lParam);

	//处理网页打开失败情况
	afx_msg LRESULT HandleHttpError(WPARAM wParam, LPARAM lParam);

	//List消息处理
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDockList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickDockList(NMHDR* pNMHDR, LRESULT* pResult);

	//List右键菜单消息处理
	afx_msg void OnListPopRunGame();
	afx_msg void OnListPopOpenDir();
	afx_msg void OnListPopSearchGame();
	afx_msg void OnListPopAddGameFav();
	afx_msg void OnListPopSaveProgress();
	afx_msg void OnListPopLoadProgress();

	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMis);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDis);

	//托盘菜单消息处理
	afx_msg void OnTrayExit();
	afx_msg void OnTrayBrowse();
	afx_msg void OnTrayGameRoom();
	afx_msg void OnTrayChat();
	afx_msg void OnTraySoftware();
	afx_msg void OnTrayDotCard();
	afx_msg void OnTrayVDMusic();

	virtual void ActivateFrame(int nCmdShow = -1);
	
	DECLARE_MESSAGE_MAP()
private:
	void AddUrlToHistory(CString url);
	CString GetLastUrl();
	CString GetNextUrl();
	CString GetPrevUrl();

private:
    BOOL ReleaseResToFile(CString strFileName, CString strDir, DWORD dwResId, LPCWSTR lpType);

private:
	HICON	m_hIcon;
	HICON	m_hOnlineProt, m_hOnlineNProt, m_hNOnlineProt, m_hNOnlineNProt;
	FrameWndMode	m_FrameMode;

	std::vector<tagGameInfo*>	m_CurGameInfo;
	std::vector<CString>		m_UrlHistory;
	std::map<DWORD, DWORD>		m_GameClicks;

	CString			m_oldInputString;
	CString			m_strLastUrl;
	bool			m_bAddBlankPage;
	bool            m_bFirstShow;
	bool            m_bMaxWindow;
	int				m_nLastNav;
	int				m_nLastTab;
	
	I8SkinCtrl_ns::CI8HtmlView m_WebPage;
	CHotGamePanel   m_HotPanel;
	CFavPanel		m_FavPanel;
	CDockNavBar		m_DockNav;
	CDockFinderBar	m_DockFinder;
	CDockBrowseBar	m_DockBrowse;
	CI8TabBar       m_DockTab;
	CListCtrl		m_DockList;
	CImageList		m_ilLstCtrl;
	CDockStatusBar	m_DockStaBar;
    I8SkinCtrl_ns::CI8WndShadow m_Shadow;
	CBitmap*		m_pbmpLstBk;
	CBitmap*        m_pbmpLstTop;
	CBitmap         m_LstBmp;
};
