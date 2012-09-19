#include "stdafx.h"
#include "BarOnline.h"
#include "MainFrame.h"
#include "SysSetDlg.h"
#include "BarOnline.h"
#include "OptLgnDlg.h"
#include "MsgDlg.h"
#include <algorithm>

#ifdef _DEBUG
#include "vld.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DOCK_LIST_ID	0x3000
#define FAV_PANEL_WIDTH	230

static int WM_TRAY_MESSAGE			= RegisterWindowMessage(TEXT("_i8desk_tray_message"));
static int WM_REFRESH_DATA	          = RegisterWindowMessage(TEXT("_i8desk_refresh_data"));

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CMainFrame::CMainFrame(LPCTSTR lpszWindowName)
{
	m_pDlgGongGao	= NULL;
	m_pDlgUptPrg	= NULL;
	m_pDlgRbPopAdv	= NULL;
	m_pDlgDock		= NULL;
	m_bLeftUrlFav	= FALSE;
	m_bRightUrlFav	= FALSE;
	m_bShowDock     = FALSE;
	m_bFirstShow    = TRUE;
	m_bMaxWindow    = FALSE;

//#ifdef _OEM_MODE
	m_hIcon			= I8SkinCtrl_ns::I8_GetIconImage(TEXT("Skin/Icon/MainFrame.ico"));
	if(m_hIcon == NULL) m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hOnlineProt	= I8SkinCtrl_ns::I8_GetIconImage(TEXT("Skin/Icon/OnlineProt.ico"));
	if(m_hOnlineProt == NULL) m_hOnlineProt = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ONLINE_PROT));
	m_hOnlineNProt	= I8SkinCtrl_ns::I8_GetIconImage(TEXT("Skin/Icon/OnlineNoProt.ico"));
	if(m_hOnlineNProt == NULL) m_hOnlineNProt = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ONLINE_NOPROT));
	m_hNOnlineProt	= I8SkinCtrl_ns::I8_GetIconImage(TEXT("Skin/Icon/NoOnlineProt.ico")); 
	if(m_hNOnlineProt == NULL) m_hNOnlineProt = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_NOONLINE_PROT));
	m_hNOnlineNProt = I8SkinCtrl_ns::I8_GetIconImage(TEXT("Skin/Icon/NoOnlineNoProt.ico")); 
	if(m_hNOnlineNProt == NULL)	m_hNOnlineNProt = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_NOONLINE_NOPROT));
//#else
//    m_hIcon         = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_hOnlineProt   = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ONLINE_PROT));
//	m_hOnlineNProt  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_ONLINE_NOPROT));
//	m_hNOnlineProt  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_NOONLINE_PROT));
//	m_hNOnlineNProt = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDI_NOONLINE_NOPROT));
//#endif

	m_FrameMode		= FWM_UNKNOWN;
	m_bAddBlankPage = false;
	m_bDockMode     = FALSE;
	m_nLastNav		= m_nLastTab = -1;
	
	Create(NULL, lpszWindowName, WS_VISIBLE|WS_POPUP|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_SYSMENU);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);

	CMenu* pMenu = GetSystemMenu(FALSE);
	if (pMenu != NULL)
	{
		pMenu->DeleteMenu(SC_MOVE,	  MF_BYCOMMAND);
		pMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
		pMenu->DeleteMenu(SC_SIZE,	  MF_BYCOMMAND);
	}
}

CMainFrame::~CMainFrame()
{
	m_pDlgGongGao = NULL;
	m_pDlgUptPrg  = NULL;
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCHITTEST()
	ON_WM_SYSCOMMAND()
	ON_WM_THEMECHANGED()
	
	ON_NOTIFY(LVN_GETDISPINFO, DOCK_LIST_ID, OnGetdispinfoList)
	ON_NOTIFY(NM_DBLCLK, DOCK_LIST_ID, OnDblclkDockList)
	ON_NOTIFY(NM_RCLICK, DOCK_LIST_ID, OnRclickDockList)

	ON_MESSAGE(WM_DISPLAYCHANGE, &CMainFrame::OnDisplayChange)

	ON_COMMAND(ID_POP_RUNGAME, &CMainFrame::OnListPopRunGame)
	ON_COMMAND(ID_POP_OPENDIR, &CMainFrame::OnListPopOpenDir)
	ON_COMMAND(ID_POP_SEARCHGAME, &CMainFrame::OnListPopSearchGame)
	ON_COMMAND(ID_POP_ADD_GAME_FAV, &CMainFrame::OnListPopAddGameFav)
	ON_COMMAND(ID_POP_SAVE_PROGRESS, &CMainFrame::OnListPopSaveProgress)
	ON_COMMAND(ID_POP_LOAD_PROGRESS, &CMainFrame::OnListPopLoadProgress)

	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(WM_NAV_CLICK_MESSAGE,		HandleNavClick)
	ON_REGISTERED_MESSAGE(WM_BROWSE_CLICK_MESSAGE,	HandleBrowseClick)
	ON_REGISTERED_MESSAGE(WM_FINDER_CLICK_MESSAGE,	HandleFinderClick)
    ON_REGISTERED_MESSAGE(WM_TAB_CLICK_MESSAGE, HandleTabClick)
	ON_REGISTERED_MESSAGE(WM_STATUS_CLICK_MESSAGE,	HandleStatusClick)
	ON_REGISTERED_MESSAGE(WM_EXEC_START_PARAM,		HandleCmdLine)
	ON_REGISTERED_MESSAGE(WM_REFRESH_DATA,			HandleRefreshData)
	ON_REGISTERED_MESSAGE(WM_TRAY_MESSAGE,			HandleTrayMessage)
    ON_REGISTERED_MESSAGE(I8SkinCtrl_ns::WM_HTTP_ERROR_MESSAGE, HandleHttpError)
 	ON_WM_MEASUREITEM()
 	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_COMMAND(ID_TRAY_EXIT,		&CMainFrame::OnTrayExit)
	ON_COMMAND(ID_TRAY_BROWSE,		&CMainFrame::OnTrayBrowse)
	ON_COMMAND(ID_TRAY_GAMEROOM,	&CMainFrame::OnTrayGameRoom)
	ON_COMMAND(ID_TRAY_CHAT,		&CMainFrame::OnTrayChat)
	ON_COMMAND(ID_TRAY_SOFTWARE,	&CMainFrame::OnTraySoftware)
	ON_COMMAND(ID_TRAY_DOTCARD,		&CMainFrame::OnTrayDotCard)
	ON_COMMAND(ID_TRAY_VDMUSIC,		&CMainFrame::OnTrayVDMusic)
END_MESSAGE_MAP()

void CMainFrame::CreateControl()
{
	CRect rc(0, 0, 0, 0);
	m_DockNav.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_DockNav.SetDockMini(FALSE);
	CString szNbMemo = AfxGetDbMgr()->GetStrOpt(OPT_U_NBNAME) + TEXT("|") + 
		AfxGetDbMgr()->GetStrOpt(OPT_U_NBTITLE) + TEXT("|") + AfxGetDbMgr()->GetStrOpt(OPT_U_NBSUBTITLE);

	//CString szNbMemo = CString("时代亮点网吧") + TEXT("|") + 
	//CString("成都市金牛区西北桥路53#附1") + TEXT("|") + CString("028-855632588");
	m_DockNav.SetNetBarText(szNbMemo);

	m_DockBrowse.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_DockFinder.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_DockTab.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_DockList.Create(WS_VISIBLE|WS_CHILD|LVS_OWNERDATA|WS_CHILDWINDOW|WS_CLIPSIBLINGS, rc, this, DOCK_LIST_ID);
	m_HotPanel.Create(CHotGamePanel::IDD, this);
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, 0xffff);
	m_WebPage.SetSilent(TRUE);
	m_WebPage.Navigate2(TEXT("about:blank"));
	m_DockStaBar.Create(NULL, WS_VISIBLE|WS_CHILD, rc, this);
	m_FavPanel.Create(CFavPanel::IDD, this);

	//设置listctrl的属性
	{
		m_ilLstCtrl.Create(32, 32, ILC_COLOR32|ILC_MASK, 1, 1);
		m_DockList.InsertColumn(0, TEXT("name"), 80);
		m_DockList.SetImageList(&m_ilLstCtrl, LVSIL_NORMAL);
		m_DockList.SetBkColor(0xFAF7F1);
		m_DockList.SetTextBkColor(-1);

		m_pbmpLstBk = I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Wnd/GameList_bg.png"));
		m_pbmpLstTop = I8SkinCtrl_ns::I8_GetSkinImage(TEXT("Skin/Wnd/背景_图标上部间隔.png"));
		
		int x = GetSystemMetrics(SM_CXICONSPACING);
		int y = GetSystemMetrics(SM_CYICONSPACING);
		if (x < 90) x = 90;
		if (y < 75) y = 75;
		m_DockList.SetIconSpacing(x, y);
	}
}

void CMainFrame::SetToolbarGame()
{
	std::vector<tagGameInfo*>::iterator it = AfxGetDbMgr()->m_toolGameInfo.begin();
	for (int idx=0; idx<5; idx++)
	{
		if (it != AfxGetDbMgr()->m_toolGameInfo.end())
		{
			tagGameInfo* pGame = *it;
			HICON hIcon = AfxGetDbMgr()->FindIconOfGame(pGame->gid);
			if (hIcon != NULL)
			{
				m_DockStaBar.SetQuickRunGroup(idx, hIcon, pGame->Name);
			}
			it++;
		}
		else
		{
			m_DockStaBar.SetQuickRunGroup(idx, NULL, CString(""));
		}
	}
}

void CMainFrame::SetFrameMode(FrameWndMode FrameMode)
{
	m_FrameMode = FrameMode;
	switch (m_FrameMode)
	{
	case FWM_UNKNOWN:
		{
			m_DockFinder.ShowWindow(SW_HIDE);
			m_DockBrowse.ShowWindow(SW_HIDE);
			m_DockTab.ShowUrlNav(SW_HIDE);
			m_DockTab.ShowWindow(SW_HIDE);
			m_HotPanel.ShowWindow(SW_HIDE);
			m_DockList.ShowWindow(SW_HIDE);
			m_WebPage.ShowWindow(SW_HIDE);
			m_FavPanel.ShowWindow(SW_HIDE);
			m_bLeftUrlFav = m_bRightUrlFav = FALSE;
		}
		break;
	case FWM_GAME:
		{
			m_DockFinder.ShowWindow(SW_SHOW);
			m_DockBrowse.ShowWindow(SW_HIDE);
			m_DockTab.ShowUrlNav(SW_SHOW);
			m_DockTab.ShowWindow(SW_SHOW);
			m_HotPanel.ShowWindow(SW_HIDE);
			m_DockList.ShowWindow(SW_SHOW);
			m_WebPage.ShowWindow(SW_HIDE);
			m_FavPanel.ShowWindow(SW_HIDE);
			m_bLeftUrlFav = m_bRightUrlFav = FALSE;
		}
		break;
	case FWM_IE:
		{
			m_DockFinder.ShowWindow(SW_HIDE);
			m_DockBrowse.ShowWindow(SW_SHOW);
			m_DockTab.ShowUrlNav(SW_HIDE);
			m_DockTab.ShowWindow(SW_SHOW);
			m_HotPanel.ShowWindow(SW_HIDE);
			m_DockList.ShowWindow(SW_HIDE);
			m_WebPage.ShowWindow(SW_SHOW);
			m_FavPanel.ShowWindow((m_bLeftUrlFav || m_bRightUrlFav) ? SW_SHOW : SW_HIDE);
		}
		break;
	case FWM_PERSON:
		{
			m_DockFinder.ShowWindow(SW_HIDE);
			m_DockBrowse.ShowWindow(SW_SHOW);
			m_DockTab.ShowUrlNav(SW_SHOW);
			m_DockTab.ShowWindow(SW_SHOW);
			m_HotPanel.ShowWindow(SW_HIDE);
			m_DockList.ShowWindow(SW_HIDE);
			m_WebPage.ShowWindow(SW_SHOW);
			m_FavPanel.ShowWindow(SW_HIDE);
			m_bLeftUrlFav = m_bRightUrlFav = FALSE;
		}
		break;
	}
	SendMessage(WM_SIZE);
}

CRect CMainFrame::GetWndDefRect()
{
	MONITORINFO info = {sizeof(info)};
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &info);
	CRect rcWorkArea = info.rcWork;
	CRect rc;
	if (rcWorkArea.Width() < FRAME_MINWIDTH)
	{
		rc.left = 0;
		rc.right = rc.left + rcWorkArea.Width();
	}
	else
	{
		rc.left = abs(FRAME_MINWIDTH - rcWorkArea.Width())/2;
		rc.right = rc.left + FRAME_MINWIDTH;
	}

	if (rcWorkArea.Height() < FRAME_MINHEIGHT)
	{
		rc.top = 0;
		rc.bottom = rc.top + rcWorkArea.Height();
	}
	else
	{
		rc.top  = abs(FRAME_MINHEIGHT - rcWorkArea.Height())/2;
		rc.bottom = rc.top + FRAME_MINHEIGHT;
	}

	if (rcWorkArea.Width() > FRAME_MINWIDTH)
		rc.right = rc.left + FRAME_MINWIDTH;
	else
		rc.right = rc.left + rcWorkArea.Width();

	if (rcWorkArea.Height() > FRAME_MINHEIGHT)
		rc.bottom = rc.top + FRAME_MINHEIGHT;
	else
		rc.bottom = rc.top + rcWorkArea.Height();

	int nWidth = rc.Width();
	int nHeight = rc.Height();
	return rc;
}

void CMainFrame::ShowTray(DWORD dwMessage)
{
	NOTIFYICONDATA nid = {0};
	nid.cbSize	= sizeof(NOTIFYICONDATA); 
	nid.hWnd	= m_hWnd;
	nid.uFlags	= NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAY_MESSAGE;
	lstrcpy(nid.szTip, AfxGetApp()->m_pszAppName);

	{
		bool bOnline = false;
		bool bProt   = false;
		i8desk_GetShareData(bOnline, bProt);
		if (bOnline)
		{
			nid.hIcon = bProt ? m_hOnlineProt : m_hOnlineNProt;
		}
		else
		{
			nid.hIcon = bProt ? m_hNOnlineProt : m_hNOnlineNProt;
		}
	}	
	Shell_NotifyIcon(dwMessage, &nid);
}

void CMainFrame::ShowGongGao()
{
	if (m_pDlgGongGao == NULL)
	{
		m_pDlgGongGao = new CGongGaoDlg;
		m_pDlgGongGao->Create(CGongGaoDlg::IDD);
		m_pDlgGongGao->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDlgGongGao->SetFocus();
	}
}

void CMainFrame::ShowUptPrg(tagGameInfo* pGameInfo)
{
	if (m_pDlgGongGao != NULL)
	{
		if (m_pDlgGongGao->m_hWnd)
			m_pDlgGongGao->DestroyWindow();
		delete m_pDlgGongGao;
	}
	m_pDlgGongGao = NULL;

	//////////////////////////////////////////////////////////////////////////
	//统计游戏点击数
	std::map<DWORD, DWORD>::iterator it = m_GameClicks.find(pGameInfo->gid);
	if (it != m_GameClicks.end())
	{
		it->second++;
	}
	else
	{
		m_GameClicks.insert(std::make_pair(pGameInfo->gid, 1));
	}
	//////////////////////////////////////////////////////////////////////////

	if (m_pDlgUptPrg == NULL)
	{
		m_pDlgUptPrg = new CUptPrgDlg(NULL);
		m_pDlgUptPrg->SetStartGame(pGameInfo);
		m_pDlgUptPrg->Create(CUptPrgDlg::IDD);
		m_pDlgUptPrg->ShowWindow(SW_SHOW);
	}
	else
	{
		CUptPrgDlg::RunGame(pGameInfo);
	}
}

void CMainFrame::ShowRBPopAdv()
{
	if (m_pDlgRbPopAdv == NULL)
	{
		m_pDlgRbPopAdv = new CRBPopAdvDlg;
		m_pDlgRbPopAdv->Create(CRBPopAdvDlg::IDD);
		m_pDlgRbPopAdv->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pDlgRbPopAdv->SetFocus();
	}
}

void CMainFrame::ShowDockWnd()
{
	ShowWindow(FALSE);
	m_bDockMode = TRUE;
	if (m_pDlgGongGao != NULL)
	{
		if (m_pDlgGongGao->m_hWnd)
			m_pDlgGongGao->DestroyWindow();
		delete m_pDlgGongGao;
	}
	m_pDlgGongGao = NULL;

	if (m_pDlgDock == NULL)
	{
		m_pDlgDock = new CDockDlg;
		m_pDlgDock->Create(CDockDlg::IDD, GetDesktopWindow());
		m_pDlgDock->ShowWindow(SW_SHOW);		
	}
	else
	{
		m_pDlgDock->ShowWindow(SW_SHOW);
		m_pDlgDock->SetFocus();
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	WNDCLASS wndClass;
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndClass);
	wndClass.lpszClassName = FRAME_CLASSNAME;
	AfxRegisterClass(&wndClass);
	cs.lpszClass = wndClass.lpszClassName;
	CRect rc = GetWndDefRect();
	cs.x = rc.left;
	cs.y = rc.top;
	cs.cx = rc.Width();
	cs.cy = rc.Height();
	//cs.dwExStyle |= WS_EX_TOOLWINDOW;
	
	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (-1 == CFrameWnd::OnCreate(lpCreateStruct))
		return -1;

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	LONG lStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	lStyle &= ~WS_EX_CLIENTEDGE;
	SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
	
	CreateControl();
	SetToolbarGame();
	m_Shadow.Create(GetSafeHwnd());

	SendMessage(WM_EXEC_START_PARAM, 0, 0);

	//if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_WINMODE))
	//{
	//	ShowWindow(SW_MAXIMIZE);
	//}

	SetTimer(1, 1000*60*5, NULL);
	SetTimer(2, 1000, NULL);

	ShowTray(NIM_ADD);

	m_pPopMenu = new tagMenuItemEx[8];

	m_pPopMenu[0].dwId = ID_POP_HOLD;
	m_pPopMenu[0].dwGid = 1;

	m_pPopMenu[1].dwId = ID_SEPARATOR;
	m_pPopMenu[1].dwGid = -1;
	lstrcpy(m_pPopMenu[1].szText, TEXT(""));

	m_pPopMenu[2].dwId = ID_POP_RUNGAME;
	m_pPopMenu[2].dwGid = 0;
	lstrcpy(m_pPopMenu[2].szText, TEXT("运行游戏"));

	m_pPopMenu[3].dwId = ID_POP_OPENDIR;
	m_pPopMenu[3].dwGid = 0;
	lstrcpy(m_pPopMenu[3].szText, TEXT("打开目录"));

	m_pPopMenu[4].dwId = ID_POP_SEARCHGAME;
	m_pPopMenu[4].dwGid = 0;
	lstrcpy(m_pPopMenu[4].szText, TEXT("搜索游戏相关内容"));

	m_pPopMenu[5].dwId = ID_POP_ADD_GAME_FAV;
	m_pPopMenu[5].dwGid = 0;
	lstrcpy(m_pPopMenu[5].szText, TEXT("收藏游戏"));

	m_pPopMenu[6].dwId = ID_POP_SAVE_PROGRESS;
	m_pPopMenu[6].dwGid = 0;
	lstrcpy(m_pPopMenu[6].szText, TEXT("保存进度"));

	m_pPopMenu[7].dwId = ID_POP_LOAD_PROGRESS;
	m_pPopMenu[7].dwGid = 0;
	lstrcpy(m_pPopMenu[7].szText, TEXT("读取进度"));
	/**************************************************/
	m_pTrayMenu = new tagMenuItemEx[8];

	m_pTrayMenu[0].dwId = ID_TRAY_BROWSE;
	m_pTrayMenu[0].dwGid = 0;
	lstrcpy(m_pTrayMenu[0].szText, TEXT("浏览器"));

	m_pTrayMenu[1].dwId = ID_TRAY_GAMEROOM;
	m_pTrayMenu[1].dwGid = 0;
	lstrcpy(m_pTrayMenu[1].szText, TEXT("游戏大厅"));

	m_pTrayMenu[2].dwId = ID_TRAY_CHAT;
	m_pTrayMenu[2].dwGid = 0;
	lstrcpy(m_pTrayMenu[2].szText, TEXT("聊天工具"));

	m_pTrayMenu[3].dwId = ID_TRAY_SOFTWARE;
	m_pTrayMenu[3].dwGid = 0;
	lstrcpy(m_pTrayMenu[3].szText, TEXT("常用软件"));

	m_pTrayMenu[4].dwId = ID_TRAY_DOTCARD;
	m_pTrayMenu[4].dwGid = 0;
	lstrcpy(m_pTrayMenu[4].szText, TEXT("点卡商城"));

	m_pTrayMenu[5].dwId = ID_TRAY_VDMUSIC;
	m_pTrayMenu[5].dwGid = 0;
	lstrcpy(m_pTrayMenu[5].szText, TEXT("影视/音乐"));

	m_pTrayMenu[6].dwId = 0;
	m_pTrayMenu[6].dwGid = -1;

	m_pTrayMenu[7].dwId = ID_TRAY_EXIT;
	m_pTrayMenu[7].dwGid = 0;
	lstrcpy(m_pTrayMenu[7].szText, TEXT("退出"));
	/**************************************************/

	AfxGetPersonMgr()->NoOperator();

	return 0;
}

LRESULT CMainFrame::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CFrameWnd::OnNcHitTest(point);
	ScreenToClient(&point);
	if (nHitTest == HTCLIENT && point.y < 59)
		return HTCAPTION;
 	return nHitTest;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{ 
	CFrameWnd::OnSize(nType, cx, cy);
	if (m_DockNav.GetSafeHwnd() == NULL)
		return ;

	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.DeflateRect(1, 1, 1, 1);
	CRect rx(rcClient);
	rx.bottom = rx.top + FRAME_DOCK_HEIGHT;
	m_DockNav.MoveWindow(&rx);

	switch (m_FrameMode)
	{
	case FWM_UNKNOWN:
		{

		}
		break;
	case FWM_GAME:
		{
			rx = rcClient;
			rx.top += FRAME_DOCK_HEIGHT;
			rx.bottom = rx.top + 37;
			m_DockFinder.MoveWindow(&rx);

			rx.OffsetRect(0, rx.Height());
			rx.bottom = rx.top + 32;
			m_DockTab.MoveWindow(&rx);

			rx.left = 1;
			rx.right -= 1;
			rx.top = rx.bottom/* + 5*/;
			rx.bottom = rcClient.bottom - 32;			
			m_HotPanel.MoveWindow(&rx);
			m_WebPage.MoveWindow(&rx);

			rx.top += 15;			
			m_DockList.MoveWindow(&rx);

			//创建并设置listctrl的背景图片
			if (m_pbmpLstBk != NULL)
			{
				if (m_LstBmp.m_hObject != NULL)
					m_LstBmp.DeleteObject();

				CRect rc;
				m_DockList.GetWindowRect(&rc);

				CDC dcScreen, dcMem;
				dcScreen.CreateDC(TEXT("Display"), NULL, NULL, NULL);
				dcMem.CreateCompatibleDC(&dcScreen);

				BITMAP bm;
				m_pbmpLstBk->GetBitmap(&bm);
				int nImgWidth  = bm.bmWidth;
				int nImgHeight = bm.bmHeight;
				float fPercent = (float)0;
                //首先以宽为基准计算缩放比例
				fPercent = (float)rc.Width()/(float)nImgWidth;
				int nTemp = (int)(nImgHeight*fPercent);
				if (nTemp < rc.Height())
				{
					//以高度为基准计算缩放比例
					fPercent = (float)rc.Height()/(float)nImgHeight;
					nTemp = (int)(nImgWidth*fPercent);

					nImgWidth  = nTemp;				
					nImgHeight = rc.Height();
				}
				else
				{
					nImgWidth = rc.Width();
					nImgHeight = nTemp;
				}
				m_LstBmp.CreateCompatibleBitmap(&dcScreen, nImgWidth, nImgHeight);
				CBitmap* pOldBmp = dcMem.SelectObject(&m_LstBmp);

				I8SkinCtrl_ns::I8_DrawImage(&dcMem, m_pbmpLstBk, CRect(0, 0, nImgWidth, nImgHeight));
				dcScreen.SelectObject(pOldBmp);
				m_DockList.SetBkImage(m_LstBmp);//, TRUE, 0, -2);
			}
		}
		break;
	case FWM_IE: case FWM_PERSON:
		{
			rx = rcClient;
			rx.top += FRAME_DOCK_HEIGHT;
			rx.bottom = rx.top + 37;
			m_DockBrowse.MoveWindow(&rx);

			rx.OffsetRect(0, rx.Height());
			rx.bottom = rx.top + 32;
			m_DockTab.MoveWindow(&rx);

			rx.top = rx.bottom /*+ 5*/;
			rx.bottom = rcClient.bottom - 32;
			rx.left = 1;
			if (m_bLeftUrlFav)
			{
				rx.right = rx.left + FAV_PANEL_WIDTH;
				m_FavPanel.MoveWindow(&rx);
				rx.left = rx.right;
			}
			rx.right = rcClient.right - 1 - (m_bRightUrlFav ? FAV_PANEL_WIDTH : 0);
			m_WebPage.MoveWindow(&rx);
			if (m_bRightUrlFav)
			{
				rx.right = rcClient.right - 1;
				rx.left = rx.right - FAV_PANEL_WIDTH;
				m_FavPanel.MoveWindow(&rx);
			}
		}
		break;
	//case FWM_PERSON:
	//	{
	//		rx = rcClient;
	//		rx.top = FRAME_DOCK_HEIGHT;
	//		rx.bottom = rx.top + 37;
	//		m_DockBrowse.MoveWindow(&rx);

	//		rx.OffsetRect(0, rx.Height());
	//		rx.bottom = rx.top + 32;
	//		m_DockTab.MoveWindow(&rx);

	//		rx.top = rx.bottom + 5;
	//		rx.bottom = rcClient.bottom - 32;
	//		rx.left = 1;
	//		rx.right -= 1;
	//		m_WebPage.MoveWindow(&rx);
	//	}
	//	break;
	}

	rx = rcClient;
	rx.top = rx.bottom - 32;
	m_DockStaBar.MoveWindow(&rx);
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	CClientDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CPoint ptFrom(0, 0);
	CPoint ptTo(0, rc.bottom);
	pDC->MoveTo(ptFrom); pDC->LineTo(ptTo); //左边框

	ptFrom.x = ptTo.x = rc.right - 1;
	pDC->MoveTo(ptFrom);pDC->LineTo(ptTo);  //右边框

	ptFrom.x = ptFrom.y = 0;
	ptTo.x = rc.right; ptTo.y = 0;           
	pDC->MoveTo(ptFrom); pDC->LineTo(ptTo); //上边框

	ptFrom.y = ptTo.y = rc.bottom - 1;   
	pDC->MoveTo(ptFrom); pDC->LineTo(ptTo); //下边框

	rc.top = FRAME_DOCK_HEIGHT+32+37+1;
	rc.bottom = rc.top + 15;
	rc.left = 1;
	rc.right -= 1;
//	pDC->FillRect(&rc, &CBrush(RGB(255, 255, 255)));//&CBrush(0xFFDE95));
	I8SkinCtrl_ns::I8_DrawImage(pDC, m_pbmpLstTop, rc);
	pDC->SelectObject(pOldPen);

	return TRUE;
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* pMMI)
{
	CFrameWnd::OnGetMinMaxInfo(pMMI);
	
	MONITORINFO info = {sizeof(info)};
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &info);

	RECT rcWorkArea = info.rcWork;
	RECT rcMonitorArea = info.rcMonitor;
	pMMI->ptMaxPosition.x = abs(rcWorkArea.left - rcMonitorArea.left);
	pMMI->ptMaxPosition.y = abs(rcWorkArea.top - rcMonitorArea.top);
	pMMI->ptMaxSize.x = abs(rcWorkArea.right - rcWorkArea.left);
	pMMI->ptMaxSize.y = abs(rcWorkArea.bottom - rcWorkArea.top);
}

void CMainFrame::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo == NULL)
		return ;
	LV_ITEM* pItem= &(pDispInfo->item);
	size_t itemid = pItem->iItem;
	if (itemid >= m_CurGameInfo.size())
	{
		*pResult = 0;
		return ;
	}
	tagGameInfo* pGameInfo = m_CurGameInfo[itemid];
	if (pItem->mask & LVIF_TEXT)
	{
		//Which column?
		if (pItem->iSubItem == 0)
		{
			lstrcpyn(pItem->pszText, pGameInfo->Name, pItem->cchTextMax);
		}
	}

	if( pItem->mask & LVIF_IMAGE) 
	{
		HICON hIcon = AfxGetDbMgr()->FindIconOfGame(pGameInfo->gid);
		m_ilLstCtrl.Replace(itemid, hIcon);
		pItem->iImage = itemid;
	}

	*pResult = 0;
}

void CMainFrame::OnDblclkDockList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;

	ShowUptPrg(m_CurGameInfo[nSel]);
}

void CMainFrame::OnRclickDockList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nItem = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nItem == -1 || nItem >= (int)(m_CurGameInfo.size()))
		return ;

	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_TRAY_MENU));
	CMenu* pContextMenu = menu.GetSubMenu(1);

	CPoint oPoint;
	GetCursorPos( &oPoint );
	SetForegroundWindow();

	for (UINT idx=0; idx<pContextMenu->GetMenuItemCount(); idx++)
	{
		UINT uFlags =  0;
		switch(m_pPopMenu[idx].dwId)
		{
		case ID_POP_SAVE_PROGRESS:
			{
              uFlags = MF_BYCOMMAND|MF_OWNERDRAW|MF_GRAYED;
			   if (m_CurGameInfo[nItem]->SaveFilter.IsEmpty())
			   {
				   lstrcpy(m_pPopMenu[idx].szText, TEXT("保存进度(未设置)"));
			   }
			   else
			   {
				   lstrcpy(m_pPopMenu[idx].szText, TEXT("保存进度"));
			   }
				break;
			}
		case ID_POP_LOAD_PROGRESS:
			{   
				uFlags = MF_BYCOMMAND|MF_OWNERDRAW|MF_GRAYED;
				if (m_CurGameInfo[nItem]->SaveFilter.IsEmpty())
				{
					lstrcpy(m_pPopMenu[idx].szText, TEXT("读取进度(未设置)"));
				}
				else
				{
					lstrcpy(m_pPopMenu[idx].szText, TEXT("读取进度"));
				}
				break;
			}
		default:
			{
				uFlags =  MF_BYCOMMAND|MF_OWNERDRAW;
				break;
			}
		}
		pContextMenu->ModifyMenu(m_pPopMenu[idx].dwId, uFlags, m_pPopMenu[idx].dwId, 
			(LPCTSTR)&m_pPopMenu[idx]);
	}
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		oPoint.x, oPoint.y, this);
}

void CMainFrame::OnListPopRunGame()
{
	LRESULT result;
	OnDblclkDockList(NULL, &result);
}

void CMainFrame::OnListPopOpenDir()
{
	if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_BROWSEDIR))
	{
		CString strNotify;
		strNotify.Format(TEXT("对不起，当前设置不允许浏览游戏目录。"));
		AfxMessageBoxEx(strNotify);
		return;
	}	

	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;
	tagGameInfo* pGame = m_CurGameInfo[nSel];

	CString szWorkDir;
	if (pGame->RunType == 2)	//虚拟盘运行的，需要转换路径
	{
		szWorkDir = pGame->SvrPath;
		szWorkDir.Trim();
		tagVDiskInfo* pVDisk = AfxGetDbMgr()->FindVDiskInfo(pGame->VID);
		if (szWorkDir.GetLength() > 3 && pVDisk != NULL)
		{
			szWorkDir.SetAt(0, pVDisk->CliDrv);
		}
	}
	else
	{
		szWorkDir = pGame->CliPath;
	}

	if (szWorkDir.GetLength() && szWorkDir.Right(1) != TEXT("\\"))
		szWorkDir += TEXT("\\");

	if (PathFileExists(szWorkDir))
	{
		ShellExecute(m_hWnd, TEXT("open"), szWorkDir, TEXT(""), TEXT(""), SW_SHOWNORMAL);
	}
	else
	{
		szWorkDir.Insert(0, TEXT("目录不存在:"));
		AfxMessageBoxEx(szWorkDir);
	}
}

void CMainFrame::OnListPopSearchGame()
{
	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;
	tagGameInfo* pGame = m_CurGameInfo[nSel];

	CWaitCursor wc;
	CString str;
	str.Format(TEXT("http://www.baidu.com/s?wd=%s&tn=licenseonline_pg"), (LPCTSTR)pGame->Name);
	ShellExecute(m_hWnd, TEXT("open"), str, TEXT(""), TEXT(""), SW_SHOWNORMAL);
}

void CMainFrame::OnListPopAddGameFav()
{
	return;
	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;
	tagGameInfo* pGame = m_CurGameInfo[nSel];
	CWaitCursor wc;
	CPersonMgr::RES_AddGame res = AfxGetPersonMgr()->AddGameToFavList(pGame->gid);
	if (res == CPersonMgr::RES_AddGame_Failed)
	{
		CString strError = TEXT("对不起，收藏游戏失败！");
		AfxMessageBoxEx(strError);
	}
	else if (res == CPersonMgr::RES_AddGame_NotLogin)
	{
		CString strError = TEXT("您还没有登陆，请登陆以后使用游戏收藏功能！");
		AfxMessageBoxEx(strError);
		HandleNavClick(9, 0);
	}
	else
	{
		CString strError = TEXT("收藏游戏成功！");
		AfxMessageBoxEx(strError);
	}
}

void CMainFrame::OnListPopSaveProgress()
{
	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;

	tagGameInfo* pGame = m_CurGameInfo[nSel];
	if (pGame->SaveFilter.IsEmpty()) return;

	CString szParam;
	szParam.Format(TEXT("%d|1"), pGame->gid);

	TCHAR szUDisk[MAX_PATH] = {0};
	GetModuleFileName(NULL, szUDisk, MAX_PATH);
	PathRemoveFileSpec(szUDisk);
	PathAddBackslash(szUDisk);
	lstrcat(szUDisk, TEXT("UDiskClient.exe"));
	int n = (int)ShellExecute(AfxGetMainWnd()->m_hWnd, TEXT("open"), szUDisk, szParam, TEXT(""), SW_SHOWNORMAL);
	if (n <= 32)
	{
		CString strError;
		strError.Format(TEXT("游戏[%s]存档失败！"), pGame->Name);
		AfxMessageBoxEx(strError);
	}
}

void CMainFrame::OnListPopLoadProgress()
{
	size_t nSel = m_DockList.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_CurGameInfo.size())
		return ;

	tagGameInfo* pGame = m_CurGameInfo[nSel];
	if (pGame->SaveFilter.IsEmpty()) return;

	CString szParam;
	szParam.Format(TEXT("%d|0"), pGame->gid);

	TCHAR szUDisk[MAX_PATH] = {0};
	GetModuleFileName(NULL, szUDisk, MAX_PATH);
	PathRemoveFileSpec(szUDisk);
	PathAddBackslash(szUDisk);
	lstrcat(szUDisk, TEXT("UDiskClient.exe"));
	int n = (int)ShellExecute(AfxGetMainWnd()->m_hWnd, TEXT("open"), szUDisk, szParam, TEXT(""), SW_SHOWNORMAL);
	if (n <= 32)
	{
		CString strError;
		strError.Format(TEXT("游戏[%s]读档失败！"), pGame->Name);
		AfxMessageBoxEx(strError);
	}
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMis)
{
	if (lpMis->CtlType != ODT_MENU)
	{
		CFrameWnd::OnMeasureItem(nIDCtl, lpMis);
		return ;
	}	
	lpMis->itemWidth = 190;

	tagMenuItemEx* pItem = (tagMenuItemEx*)lpMis->itemData;
	if (pItem->dwGid == -1)
		lpMis->itemHeight = 4;
	else if (pItem->dwGid > 0)
		lpMis->itemHeight = 42;
	else if (pItem->dwGid == 0)
		lpMis->itemHeight = 28;
}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDis)
{
	if (lpDis->CtlType != ODT_MENU)
	{
		CFrameWnd::OnDrawItem(nIDCtl, lpDis);
		return ;
	}
	
	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	pDC->SetBkMode(1);

	CRect rc(lpDis->rcItem);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Menu/背景_1px.png"), rc);

	tagMenuItemEx* pItem = (tagMenuItemEx*)lpDis->itemData;
	if (pItem->dwGid < 0)
	{
		CRect rx = rc;
		rx.DeflateRect(0, 1, 0, 1);
		I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Menu/背景_分割线.png"), rx);
	}
	else if (pItem->dwGid > 0)
	{
		int nItem = m_DockList.GetNextItem(-1, LVIS_SELECTED);
		if (nItem != -1)
		{
			tagGameInfo* pGame = m_CurGameInfo[nItem];

			CRect rx = rc;
			rx.top = rc.top + (rc.Height() - 32)/2;
			rx.bottom = rx.top + 32;
			rx.left = 4; rx.right = rx.left + 32;
			m_ilLstCtrl.Draw(pDC, nItem, rx.TopLeft(), ILD_TRANSPARENT);

			LOGFONT lf = {0};
			GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
			lf.lfWeight = FW_BOLD;
			CFont font;
			font.CreateFontIndirect(&lf);

			rx = rc;
			rx.left = 48;
			pDC->SetTextColor(0xB56226);
			CFont* pOldFont = pDC->SelectObject(&font);
			pDC->DrawText(pGame->Name, -1, &rx, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
			pDC->SelectObject(pOldFont);
		}
	}
	else
	{
		if (lpDis->itemState & ODS_SELECTED)
			pDC->SetTextColor(RGB(255, 255, 255));
		else
			pDC->SetTextColor(RGB(0, 0, 0));

		CRect rcImg = rc;
		rcImg.DeflateRect(4, 1, 4, 1);
		
		if (lpDis->itemState & ODS_SELECTED)
			I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Menu/背景_按钮_鼠标经过.png"), rcImg);
		else
			I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Menu/背景_按钮_默认状态.png"), rcImg);

		CRect rx = rc;
		rx.left = 40;
		rx.right = rc.right - 10;
		pDC->DrawText(pItem->szText, &rx, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	}
}

void CMainFrame::OnDestroy()
{
	CWaitCursor wc;
	KillTimer(1);
	KillTimer(2);

	CRptData Rpt;
	Rpt.ReprotDataToServer(&m_DockNav, m_GameClicks, false);
	AfxGetDbMgr()->WaitExit();

	if (m_pDlgGongGao != NULL)
	{
		if (m_pDlgGongGao->m_hWnd != NULL)
			m_pDlgGongGao->DestroyWindow();
		m_pDlgGongGao = NULL;
	}
	
	if (m_pDlgUptPrg != NULL)
	{
		if (m_pDlgUptPrg->m_hWnd != NULL)
			m_pDlgUptPrg->DestroyWindow();
		m_pDlgUptPrg = NULL;
	}

	if (m_pDlgRbPopAdv != NULL)
	{
		if (m_pDlgRbPopAdv->m_hWnd != NULL)
			m_pDlgRbPopAdv->DestroyWindow();
		m_pDlgRbPopAdv = NULL;
	}
	
	if (m_pDlgDock != NULL)
	{
		if (m_pDlgDock->m_hWnd != NULL)
			m_pDlgDock->DestroyWindow();
		m_pDlgDock = NULL;
	}

	ShowTray(NIM_DELETE);

	delete []m_pPopMenu;
	delete []m_pTrayMenu;
	if (m_pbmpLstBk != NULL)
	{
		m_pbmpLstBk->DeleteObject();
		delete m_pbmpLstBk;
		m_pbmpLstBk = NULL;
	}
	if (m_pbmpLstTop != NULL)
	{
		m_pbmpLstTop->DeleteObject();
		delete m_pbmpLstTop;
		m_pbmpLstTop = NULL;
	}
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	CFrameWnd::OnTimer(nIDEvent);
	
	if (nIDEvent == 1)
	{
		CRptData Rpt;
		Rpt.ReprotDataToServer(&m_DockNav, m_GameClicks, true);
	}
	else if (nIDEvent == 2)
	{
		ShowTray(NIM_MODIFY);
	}
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	CFrameWnd::OnSysCommand(nID, lParam);	
	if (nID == SC_RESTORE)
	{
		SendMessage(WM_DISPLAYCHANGE);
	}
}

LRESULT CMainFrame::OnThemeChanged()
{
	//CenterWindow();
	//return 0;
	if (IsMaxWindow())
	{
		ShowMaxWindow();
	}
	else
	{
		CenterWindow();
	}
	return 0;
}

LRESULT CMainFrame::HandleNavClick(WPARAM wParam, LPARAM lParam)
{
	//处理导航栏及个人设置按钮
	if ((wParam >= 0 && wParam <= 5)
		|| wParam == 9)
	{
		if (m_pDlgDock != NULL && m_pDlgDock->m_hWnd != NULL)
		{
			m_pDlgDock->ShowWindow(SW_HIDE);
		}
		ShowWindow(SW_SHOW);
		m_bDockMode = FALSE;

		if (m_nLastNav == wParam && m_nLastTab == lParam+1 /*&& m_FrameMode != FWM_PERSON*/)
			return 0;

		m_nLastNav = wParam;
		m_bAddBlankPage = false;

		m_DockNav.SetActive(wParam);
		m_DockTab.RemoveAllTab();

		if (wParam == 0 || wParam == 4 || wParam == 5)
			SetFrameMode(FWM_IE);
		else
			SetFrameMode(FWM_GAME);

		m_nLastTab = -1;
		CDbMgr* pDbMgr = AfxGetDbMgr();
		switch (wParam)
		{
		case 0:
			{
				m_DockTab.AddTab(pDbMgr->GetNavigateCap(), NULL, NULL, FALSE);
				for (std::vector<CString>::iterator it = pDbMgr->m_BrowseTab.begin(); 
					it != pDbMgr->m_BrowseTab.end(); it++)
				{
					if (it != pDbMgr->m_BrowseTab.begin())
						m_DockTab.AddTab(*it, NULL, NULL, TRUE);
				}
			}
			break;
		case 1:
			{
				for (size_t idx=0; idx<pDbMgr->m_GameRoomTab.size(); idx++)
				{
					if (idx == 0)
					{
						m_DockTab.AddTab(pDbMgr->m_GameRoomTab[idx], 
							TEXT("Skin/TabBar/图标_未选中标签_热.png"), 
							TEXT("Skin/TabBar/图标_选中标签_热.png"), TRUE);
					}
					else
					{
						m_DockTab.AddTab(pDbMgr->m_GameRoomTab[idx], NULL, NULL, TRUE);
					}
				}
			}
			break;
		case 2:
			{
				for (std::vector<CString>::iterator it = pDbMgr->m_ChatRoomTab.begin(); 
					it != pDbMgr->m_ChatRoomTab.end(); it++)
				{
					m_DockTab.AddTab(*it, NULL, NULL, TRUE);
				}
			}
			break;
		case 3:
			{
				for (std::vector<CString>::iterator it = pDbMgr->m_SoftwareTab.begin(); 
					it != pDbMgr->m_SoftwareTab.end(); it++)
				{
					m_DockTab.AddTab(*it, NULL, NULL, TRUE);
				}
			}
			break;
		case 4:
			{
				for (std::vector<CString>::iterator it = pDbMgr->m_DotCardTab.begin(); 
					it != pDbMgr->m_DotCardTab.end(); it++)
				{
					m_DockTab.AddTab(*it, NULL, NULL, TRUE);
				}
			}
			break;
		case 5:
			{
				for (std::vector<CString>::iterator it = pDbMgr->m_VideoTab.begin(); 
					it != pDbMgr->m_VideoTab.end(); it++)
				{
					m_DockTab.AddTab(*it, NULL, NULL, TRUE);
				}
			}
			break;
		case 9:
			{
				SetFrameMode(FWM_PERSON);
				m_DockNav.SetAllUnActive();
				m_DockTab.AddTab(AfxGetDbMgr()->GetPersonTabText(), NULL, NULL, TRUE);
			}
			break;
		}
		if (lParam == 0)
			lParam = 1;
		HandleTabClick(lParam, 0);
	}
	else //处理工具栏按钮
	{
		switch (wParam)
		{
		case 6:
			ShowDockWnd();
			break;
		case 7:
			{
				if (IsMaxWindow())
				{
					ShowNormalWindow();
				}
				else
				{
					ShowMaxWindow();
				}
				//if (IsZoomed())
				//	ShowWindow(SW_RESTORE);
				//else
				//	ShowWindow(SW_MAXIMIZE);
			}
			break;
		case 8:
			{
 				//SetFrameMode(FALSE, FALSE);
				//PostQuitMessage(0);
				ShowDockWnd();
 			}
			break;
		case 10:
			{
				TCHAR szUDisk[MAX_PATH] = {0};
				GetModuleFileName(NULL, szUDisk, MAX_PATH);
				PathRemoveFileSpec(szUDisk);
				PathAddBackslash(szUDisk);
				lstrcat(szUDisk, TEXT("UDiskClient.exe"));
				int n = (int)ShellExecute(AfxGetMainWnd()->m_hWnd, TEXT("open"), szUDisk, TEXT(""), TEXT(""), SW_SHOWNORMAL);
				if (n <= 32)
				{
					CString strError;
					strError.Format(TEXT("个人U盘启动失败！"));
					AfxMessageBoxEx(strError);
				}
			}
			break;
		case 11:
			{
				m_nLastTab = m_nLastNav = -1;
				HandleNavClick(0, 1);
				ShowUrlFavorite(FALSE);
			}
			break;
		case 12:
			{
				CString strURL = AfxGetDbMgr()->GetStrOpt(OPT_U_NBTITLEURL);
				if (strURL.IsEmpty())
				{
					ShowGongGao();
				}
				else
			   	{
			    	ShellExecute(m_hWnd, TEXT("open"), strURL, TEXT(""), TEXT(""), SW_SHOWNORMAL);
				}
			}
			break;
		}
	}
	return 0;
}

void  CMainFrame::ShowUrlFavorite(BOOL bIsLeft)
{
	m_bLeftUrlFav  = bIsLeft;
	m_bRightUrlFav = !bIsLeft;
	m_FavPanel.MoveWindow(CRect(0, 0, 0, 0)); //防止网址收藏栏在菜单顶部闪烁
	m_FavPanel.ShowWindow(SW_SHOW);
	m_FavPanel.Refresh();
	m_FavPanel.RedrawWindow();
	m_WebPage.Refresh();
	SendMessage(WM_SIZE);
}

void  CMainFrame::ClickUrlFavorite(CString szUrl)
{
	m_DockBrowse.SetText(szUrl);
	HandleBrowseClick(10, 0);
}

void  CMainFrame::HideUrlFavorite()
{
	m_bLeftUrlFav = m_bRightUrlFav = FALSE;
	if (m_FavPanel.GetSafeHwnd() != NULL)
		m_FavPanel.ShowWindow(SW_HIDE);
	SendMessage(WM_SIZE);
}

void  CMainFrame::RefreshUrlFavorite()
{
	if (m_FavPanel.GetSafeHwnd() != NULL)
	{
		m_FavPanel.Refresh();
	}
}

LRESULT CMainFrame::HandleBrowseClick(WPARAM wParam, LPARAM lParam)
{
	CWaitCursor wc;
	switch (wParam)
	{
	case 1:	//Favorite.
		ShowUrlFavorite(TRUE);
		break;
	case 8:	//refresh.
		m_WebPage.Refresh();
		break;
	case 9: //stop
		m_WebPage.Stop();
		break;
	case 2: case 3: case 4: case 10:	//back, forward, home. go.
		{
			//只有显示了空白页才能有前进和后退。
			if ((wParam == 2 || wParam == 3) && !m_bAddBlankPage)
				return 0;

			if (!m_bAddBlankPage)
			{
				m_DockTab.AddTab(TEXT("当前页"), NULL, NULL, TRUE);	
				m_bAddBlankPage = true;
			}
			CString str;
			if (wParam == 2)
				str = GetPrevUrl();
			else if (wParam == 3)
				str = GetNextUrl();
			else if (wParam == 4)
				str = TEXT("http://www.i8desk.com");
			else
				str = m_DockBrowse.GetText();
			
			if (str.IsEmpty())
				return 0;

			//添加取历名记录
			if (wParam == 4 || wParam == 10)
			{
				AddUrlToHistory(str);
				m_DockBrowse.SetListText(m_UrlHistory);
			}

			m_strLastUrl = str;
			m_nLastTab = -1;
			HandleTabClick(m_DockTab.GetTabCount(), 0);
		}
		break;
	case 11:	//url navagate.
		{
			if (m_DockNav.GetActive() != 0)
			{
				m_nLastNav = m_nLastTab = -1;
				HandleNavClick(0, 1);
			}
			else
			{
				m_nLastTab = -1;
				HandleTabClick(1, 0);
			}
		}
		break;
	}
	return 0;
}

LRESULT CMainFrame::HandleFinderClick(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 2)	//Go.
	{
		CString str = m_DockTab.GetTabText(m_DockTab.GetActive());
		if (str == CLASS_RM_NAME)
		{
			HandleTabClick(2, 0);
		}

		str = m_DockFinder.GetText();
		str.MakeLower();str.Trim();
		if (str == m_oldInputString)
			return 0;
		m_oldInputString = str;
		if (str.IsEmpty())
		{
			m_nLastTab = -1;
			HandleTabClick(m_DockTab.GetActive()+1, 0);
		}
		else
		{
			m_CurGameInfo.clear();
			m_ilLstCtrl.SetImageCount(0);
			m_DockList.DeleteAllItems();
			std::map<DWORD, tagGameInfo*>::iterator it = AfxGetDbMgr()->m_mapGameInfo.begin();
			for (; it != AfxGetDbMgr()->m_mapGameInfo.end(); it++)
			{
				tagGameInfo* pGame = it->second;
				if (lParam == 0)//处理手动输入的查找
				{
                    CString strName = pGame->Name;strName.MakeLower();					
					if ((pGame->nFindPos = strName.Find(str)) != -1)
					{
						if(pGame->nFindPos != 0) pGame->nFindPos = 1;
						else pGame->nFindPos = -1; //在名字中查找到的优先级高过拼音中查找到的
						m_CurGameInfo.push_back(pGame);
					}
					else if ((pGame->nFindPos = pGame->Py.Find(str)) != -1
						|| (pGame->nFindPos = pGame->MultiPy.Find(str)) != -1)
					{
						if(pGame->nFindPos != 0) pGame->nFindPos = 1;
						m_CurGameInfo.push_back(pGame);
					}
				}
				else	//特别处理字线表的查找
				{
					if ((pGame->Name.GetLength() && pGame->Name.GetAt(0) == str) || 
						(pGame->Py.GetLength() && pGame->Py.GetAt(0) == str) || 
						(pGame->MultiPy.GetLength() && pGame->MultiPy.GetAt(0) == str))
					{
						m_CurGameInfo.push_back(pGame);
					}
				}
			}
            SortGameList<SORT_FINDPOS>(m_CurGameInfo);
			int nSize = m_CurGameInfo.size();
			m_ilLstCtrl.SetImageCount(nSize);
			m_DockList.SetItemCount(nSize);

			m_WebPage.Stop();
			m_WebPage.Navigate2(TEXT("about:blank"));
			m_WebPage.ShowWindow(SW_HIDE);
			m_DockList.ShowWindow(SW_SHOW);
		}
		
		return 0;
	}
	
	return 0;
}

LRESULT CMainFrame::HandleTabClick(WPARAM wParam, LPARAM lParam)
{
	m_WebPage.Stop();
	m_WebPage.Navigate2(TEXT("about:blank"));
	m_DockTab.SetActive(wParam-1);	
	int nSel = (m_FrameMode == FWM_PERSON ? 9 : m_DockNav.GetActive());
	if (m_nLastNav == nSel && m_nLastTab == wParam)
		return 0;
	m_nLastTab = wParam;
	
	m_oldInputString = TEXT("");
	m_DockList.DeleteAllItems();
	m_ilLstCtrl.SetImageCount(0);
	m_CurGameInfo.clear();

	switch (nSel)
	{
	//处理网页类ＴＡＢ.
	case 0: case 4: case 5: case 9:
		{
			CDbMgr* pDbMgr = AfxGetDbMgr();
			CString szIdcClass = m_DockTab.GetTabText(wParam-1);
			if (nSel == 9)
			{
				if (szIdcClass == pDbMgr->GetPersonTabText())
				{	
					m_WebPage.Stop();
				    //m_WebPage.Navigate2(AfxGetDbMgr()->GetPersonUrl());
					m_WebPage.LoadFromResource(IDR_HTTPERROR);
				    m_DockBrowse.SetText(CString(DEFAULT_TIP_TEXT2));
				}
				else
				{
					m_WebPage.Stop();
					m_WebPage.Navigate2(m_strLastUrl);
					m_DockBrowse.SetText(m_strLastUrl);
				}
			}
			else if (szIdcClass == pDbMgr->GetNavigateCap())
			{
				m_WebPage.Stop();
				m_WebPage.Navigate2(AfxGetDbMgr()->GetNavigateUrl());
				m_DockBrowse.SetText(CString(DEFAULT_TIP_TEXT2));
			}
			else if (szIdcClass == pDbMgr->GetDotCardCap())
			{
				m_WebPage.Stop();
				m_WebPage.Navigate2(AfxGetDbMgr()->GetDotCardUrl());
				m_DockBrowse.SetText(CString(DEFAULT_TIP_TEXT2));
			}
			else
			{
				CDbMgr* pDbMgr = AfxGetDbMgr();
				tagFavInfo* pFav = pDbMgr->FindFavInfo(m_DockTab.GetTabText(wParam-1));
				if (pFav != NULL)
				{
					m_WebPage.Stop();
					m_WebPage.Navigate2(pFav->strUrl);
					m_DockBrowse.SetText(pFav->strUrl);
				}
				else
				{
					m_WebPage.Stop();
					m_WebPage.Navigate2(m_strLastUrl);
					m_DockBrowse.SetText(m_strLastUrl);
				}
			}
		}
		break;
	//处理游戏类ＴＡＢ.
	case 1: case 2: case 3:
		{
			CString szIdcClass = m_DockTab.GetTabText(wParam-1);
			if (szIdcClass != CLASS_RM_NAME)
			{
				//先看是不是中心要求显示的游戏后置于网址
				CString url;
				if (AfxGetDbMgr()->GetPreGameUrl(szIdcClass, url))
				{
					m_HotPanel.ShowWindow(SW_HIDE);
					m_DockList.ShowWindow(SW_HIDE);
					m_WebPage.Stop();
					m_WebPage.Navigate2(url);
					m_WebPage.ShowWindow(SW_SHOW);
				}
				else
				{
					std::map<DWORD, tagGameInfo*>::iterator it = AfxGetDbMgr()->m_mapGameInfo.begin();
					for (; it != AfxGetDbMgr()->m_mapGameInfo.end(); it++)
					{
						tagGameInfo* pGame = it->second;
						if (pGame->IdcClass.CompareNoCase(szIdcClass) == 0)
						{
							m_CurGameInfo.push_back(pGame);
						}
					}
					m_WebPage.ShowWindow(SW_HIDE);
					m_HotPanel.ShowWindow(SW_HIDE);
					m_DockList.ShowWindow(SW_SHOW);
				}
			}
			else
			{
				m_WebPage.ShowWindow(SW_HIDE);
				m_DockList.ShowWindow(SW_HIDE);
				m_HotPanel.ShowWindow(SW_SHOW);
			}
			int nSize = m_CurGameInfo.size();
			SortGameList<SORT_WINDOWS>(m_CurGameInfo);
			m_ilLstCtrl.SetImageCount(nSize);
			m_DockList.SetItemCount(nSize);
		}
		break;
	}
	return 0;
}

LRESULT CMainFrame::HandleStatusClick(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0: case 1: case 2: case 3: case 4:
		{
			CDbMgr* pDbMgr = AfxGetDbMgr();
			if (pDbMgr->m_toolGameInfo.size() > wParam)
			{
				ShowUptPrg(pDbMgr->m_toolGameInfo[wParam]);
			}
		}
		break;
	case 5:
		ShowGongGao();
		break;
	case 11:
		{
			TCHAR szFile[MAX_PATH] = {0};
			GetModuleFileName(NULL, szFile, MAX_PATH);
			PathRemoveFileSpec(szFile);
			PathAddBackslash(szFile);
			lstrcat(szFile, TEXT("wxSLock.exe"));
			ShellExecute(m_hWnd, TEXT("open"), szFile, TEXT(""), TEXT(""), SW_SHOWNORMAL);
		}
		break;
	case 12:
		{
			COptLgnDlg login;
			if (login.DoModal() == IDOK)
			{
				CSysSetDlg dlg;
 				dlg.DoModal();
			}
		}
		break;
	case 14:
		{
			tagGameInfo* pGameInfo = AfxGetDbMgr()->FindGameInfo(40724);
			if (pGameInfo != NULL)
			{
				//虚拟盘运行
				if (pGameInfo->RunType != 1)
				{
					CUptPrgDlg::RunGame(pGameInfo);
				}
				else
				{
					TCHAR szIdxFile[MAX_PATH] = {0};
					_tcscat_s(szIdxFile, _countof(szIdxFile), pGameInfo->CliPath);
					PathRemoveFileSpec(szIdxFile);
					PathAddBackslash(szIdxFile);
					_tcscat_s(szIdxFile, _countof(szIdxFile), TEXT("i8desk.idx"));
					if (PathFileExists(szIdxFile) && AfxGetDbMgr()->Idxfile_GetVersion(szIdxFile) == pGameInfo->dwNbVer)
					{
						CUptPrgDlg::RunGame(pGameInfo);
					}
					else if (m_pDlgUptPrg != NULL)
					{
						AfxMessageBoxEx(TEXT("您正在更新游戏，请完成游戏更新后再更新输入法！"));
					}
					else
					{
						CInputMethodDlg dlg;
						dlg.DoModal();
					}
				}
			}
			else
			{
				AfxMessageBoxEx(TEXT("您启动的输入法工具还未下载，请提示网管从三层下载！"));
			}
		}
		break;
	}
	return 0;
}

LRESULT CMainFrame::HandleTrayMessage(WPARAM wParam, LPARAM lParam)
{
	switch ( lParam )
	{
	case WM_LBUTTONDBLCLK:
		{
			//ShowWindow(SW_RESTORE);
			int nLastNav = m_nLastNav;
			int nLastTab = m_nLastTab;
			m_nLastNav = m_nLastTab = -1;
			//处理开机启动DOCK没有选中任何一栏
			nLastNav = (nLastNav == -1 ? 0 : nLastNav);
			nLastTab = (nLastTab == -1 ? 0 : nLastTab);
			HandleNavClick(nLastNav, nLastTab);
			SetForegroundWindow();
		}
		break;
	case WM_RBUTTONDOWN:
		{
			CMenu menu;
			menu.LoadMenu(MAKEINTRESOURCE(IDR_TRAY_MENU));
			CMenu* pContextMenu = menu.GetSubMenu(0);

			CPoint oPoint;
			GetCursorPos( &oPoint );
			SetForegroundWindow();

			for (UINT idx=0; idx<pContextMenu->GetMenuItemCount(); idx++)
			{
		    	pContextMenu->ModifyMenu(m_pTrayMenu[idx].dwId, MF_BYCOMMAND|MF_OWNERDRAW, m_pTrayMenu[idx].dwId, 
				    (LPCTSTR)&m_pTrayMenu[idx]);
			}

			pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				oPoint.x, oPoint.y, this);
		}		
		break;
	}	

	return 0;
}

LRESULT CMainFrame::HandleCmdLine(WPARAM wParam, LPARAM lParam)
{
	//wParam=1表示第二个实例发来的消息，0表示自己发送来的消息。
	CBarOnlineApp* pApp = (CBarOnlineApp*)AfxGetApp();
	
	CString szCmdLine = pApp->GetCmdLine();
	szCmdLine.Trim(); szCmdLine.Trim(TEXT("\""));
	szCmdLine.MakeLower();

	int  nNav	  = 1;		//游戏大厅
	int  nTab     = 2;		//网络游戏

	//开机启动Dock
	if (StrNCmpI(szCmdLine, TEXT("/SHOWDOCK"), lstrlen(TEXT("/SHOWDOCK"))) == 0)
	{
		m_bShowDock = TRUE;
		return 0;
	}

	//查找运行游戏的gid.
	CString gid, szSub;

	//支持个人用户的东西。
	if (StrNCmpI(szCmdLine, TEXT("i8desk://personal="), lstrlen(TEXT("i8desk://personal="))) == 0)
	{
		//"i8desk://personal=getuserstate,getpersonalization,getfavoriteswebsite,getfavoritesproduct/"
		int nStart = lstrlen(TEXT("i8desk://personal="));
		CString cmd = szCmdLine.Mid(nStart);
		if (cmd.Right(1) == TEXT("/"))
			cmd.Delete(cmd.GetLength() - 1, 1);
		//先判断是否是启动个人U盘
		if (cmd.CompareNoCase(TEXT("FlashDrive")) == 0)
		{
			TCHAR szUDisk[MAX_PATH] = {0};
			GetModuleFileName(NULL, szUDisk, MAX_PATH);
			PathRemoveFileSpec(szUDisk);
			PathAddBackslash(szUDisk);
			lstrcat(szUDisk, TEXT("UDiskClient.exe"));
			int n = (int)ShellExecute(AfxGetMainWnd()->m_hWnd, TEXT("open"), szUDisk, TEXT(""), TEXT(""), SW_SHOWNORMAL);
			if (n <= 32)
			{
				CString strError;
				strError.Format(TEXT("个人U盘启动失败！"));
				AfxMessageBoxEx(strError);
			}
			m_bShowDock = TRUE;
			return 0;
		}
		//再判断是否是个人设置
		CString szItem;
		for (int idx=0;;idx++)
		{
			AfxExtractSubString(szItem, cmd, idx, TEXT(','));
			if (szItem.IsEmpty())
				break;

			if (szItem.CompareNoCase(TEXT("getuserstate")) == 0)
			{
				AfxGetPersonMgr()->GetPersonState();
			}
			else if (szItem.CompareNoCase(TEXT("getpersonalization")) == 0)
			{
				AfxGetPersonMgr()->GetPersonalizationSet();
				RefreshUrlFavorite();
			}
			else if (szItem.CompareNoCase(TEXT("getfavoriteswebsite")) == 0)
			{
				AfxGetPersonMgr()->GetPersonFavorite();
			}
			else if (szItem.CompareNoCase(TEXT("getfavoritesproduct")) == 0)
			{
				//reserved.
			}
		}

		//第二个实例发来的参数，不应该修改默认导航栏的选中。
		//if (wParam == 1)
			return 0;
	}
	//支持i8desk://gid=gid/主运行方式
	else if (StrNCmpI(szCmdLine, TEXT("i8desk://gid="), lstrlen(TEXT("i8desk://gid="))) == 0)
	{
		gid = szCmdLine.Mid(szCmdLine.Find(TEXT('='))+1);
		if (gid.Right(1) == TEXT("/"))
			gid.Delete(gid.GetLength() -1);
	}
	//支持/start gid参数方式
	else if (StrNCmpI(szCmdLine, TEXT("/start"), lstrlen(TEXT("/start"))) == 0)
	{
		gid = szCmdLine.Mid(6);

		//////////////////////////////////////////////////////////////////////////
		//兼容09版本的参数
		/*： BarOnline.exe /start Game:网络游戏
		聊天工具快捷方式的设置方法： BarOnline.exe /start Game:聊天工具
		单机游戏快捷方式的设置方法： BarOnline.exe /start Game:单机游戏
		对战游戏快捷方式的设置方法： BarOnline.exe /start Game:对战游戏
		影音工具快捷方式的设置方法： BarOnline.exe /start tools:影音工具
		点卡商城快捷方式的设置办法： BarOnline.exe /start card
		*/
		gid.Trim();
		if (gid.CompareNoCase(TEXT("card")) == 0)
		{
			szSub = CAP_DOT_CARD;
			gid = TEXT("");
		}
		else
		{
			int nPos = gid.Find(TEXT(':'));
			if (nPos != -1)
			{
				CString szClass = gid.Mid(nPos+1);
				szClass.Trim();
				if (!szClass.IsEmpty())
				{
					szSub = szClass;
					gid = TEXT("");
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	else	//支持定位首页
	{
		if (szCmdLine.IsEmpty())
		{
			//如果没有参数,而且是菜单内部发来消息，则检查中心是否强制定位首页。
			if (wParam == 0)
			{
				long nPage = AfxGetDbMgr()->GetDefaultPage();
				if (nPage == 1001)
					szSub = CLASS_RM_NAME;
				else if (nPage == 1002)
					szSub = CLASS_WL_NAME;
				else if (nPage == 1003)
					szSub = CLASS_DJ_NAME;
				else if (nPage == 1004)
					szSub = CLASS_XX_NAME;
				else if (nPage == 1005)
					szSub = CLASS_DZ_NAME;
				else if (nPage == 1006)
					szSub = CLASS_QP_NAME;
				else if (nPage == 2001)
					szSub = CLASS_LT_NAME;
				else if (nPage == 3001)
					szSub = CAP_URL_NAV;
			}
		}
		else
		{
			//支持 class:类别名称 这种带参数的运行方式(目前只要有:就行了。因为没有检测前面是否是class,就支持09了。
			szSub = szCmdLine.Mid(szCmdLine.Find(TEXT(':')) +1);
		}
	}

	gid.Trim(); szSub.Trim();
	m_nLastNav = m_nLastTab = -1;
	if (!szSub.IsEmpty())
	{
		CDbMgr* pDbMgr = AfxGetDbMgr();
		std::vector<CString>* NavList[] = //反顺的,是保证聊天工具定位到另个聊天工具导航栏，而不是游戏大厅
		{
			&pDbMgr->m_VideoTab,
			&pDbMgr->m_DotCardTab,
			&pDbMgr->m_SoftwareTab,
			&pDbMgr->m_ChatRoomTab,
			&pDbMgr->m_GameRoomTab,
			&pDbMgr->m_BrowseTab
		};
		for (int idx=0; idx<_countof(NavList); idx++)
		{
			for (size_t loop=0; loop<NavList[idx]->size(); loop++)
			{
				if ((*NavList[idx])[loop] == szSub)
				{
					nNav = _countof(NavList) - idx-1;
					nTab = loop+1;
					goto _do_next;
				}
			}
		}
	}
_do_next:
	if (!gid.IsEmpty())
	{
		tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(_ttoi(gid));
		if (pGame != NULL)
		{
			ShowUptPrg(pGame);
		}
		m_bShowDock = TRUE;
		return 0;
	}
	m_nLastNav = m_nLastTab = -1;
	if (m_bFirstShow && !(AfxGetDbMgr()->GetBoolOpt(OPT_M_WINMODE)))
	{
		ShowMaxWindow();
		m_bFirstShow = false;
	}
	else
	{
	    ShowWindow(SW_SHOW);
	}
	SetForegroundWindow();
	SetFocus();
	HandleNavClick(nNav, nTab);
	return 0;
}

LRESULT CMainFrame::HandleRefreshData(WPARAM wParam, LPARAM lParam)
{
	AfxWriteLog(TEXT("Recv refresh offline data message."));

	m_DockList.DeleteAllItems();
	if (m_HotPanel.GetSafeHwnd() != NULL)
		m_HotPanel.DeleteAllGame();

	AfxGetDbMgr()->RefreshOfflineData();
	if (m_HotPanel.m_hWnd != NULL)
		m_HotPanel.Refresh();

	if (m_nLastNav > 0 && m_nLastNav < 4 && m_bDockMode == FALSE)
	{
		int nLastNav = m_nLastNav;
		int nLastTab = m_nLastTab;
		m_nLastNav = m_nLastTab = -1;
		HandleNavClick(nLastNav, nLastTab);
	}
	SetToolbarGame();

	AfxWriteLog(TEXT("finish to handle refresh offline data message."));
	return 0;
}

void CMainFrame::AddUrlToHistory(CString url)
{
	std::vector<CString>::iterator it = std::find(m_UrlHistory.begin(), m_UrlHistory.end(), url);
	if (it != m_UrlHistory.end())
	{
		m_UrlHistory.erase(it);
	}
	m_UrlHistory.push_back(url);
}

CString CMainFrame::GetLastUrl()
{
	if (m_UrlHistory.size() == 0)
		return CString();
	return *m_UrlHistory.rbegin();
}

CString CMainFrame::GetNextUrl()
{
	int nIndex = -1;
	for (size_t idx=0; idx<m_UrlHistory.size(); idx++)
	{
		if (m_UrlHistory[idx] == m_strLastUrl)
		{
			nIndex = idx;
			break;
		}
	}
	if (nIndex != -1 && nIndex + 1 < (int)m_UrlHistory.size() && m_UrlHistory.size())
	{
		nIndex ++;
	}
	else
	{
		nIndex = -1;
	}
	if (nIndex >= 0 && nIndex < (int)m_UrlHistory.size())
	{
		return m_UrlHistory[nIndex];
	}
	return CString("");
}

CString CMainFrame::GetPrevUrl()
{
	int nIndex = -1;
	for (size_t idx=0; idx<m_UrlHistory.size(); idx++)
	{
		if (m_UrlHistory[idx] == m_strLastUrl)
		{
			nIndex = idx;
			break;
		}
	}
	if (nIndex != -1 && nIndex - 1 >= 0 && m_UrlHistory.size())
	{
		nIndex --;
	}
	else 
	{
		nIndex = -1;
	}
	if (nIndex >=0 && nIndex < (int)m_UrlHistory.size())
	{
		return m_UrlHistory[nIndex];
	}
	return CString("");
}

void CMainFrame::OnTrayExit()
{
	PostMessage(WM_CLOSE);
}


void CMainFrame::OnTrayBrowse()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(0, 1);
}

void CMainFrame::OnTrayGameRoom()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(1, 1);
}

void CMainFrame::OnTrayChat()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(2, 1);
}

void CMainFrame::OnTraySoftware()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(3, 1);
}

void CMainFrame::OnTrayDotCard()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(4, 1);
}

void CMainFrame::OnTrayVDMusic()
{
	m_nLastNav = m_nLastTab = -1;
//	ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	HandleNavClick(5, 1);
}

void CMainFrame::ActivateFrame( int nCmdShow /*= -1*/ )
{
	nCmdShow = m_bShowDock ? SW_HIDE : nCmdShow;
	CFrameWnd::ActivateFrame(nCmdShow);
}

LRESULT CMainFrame::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
	if (IsMaxWindow())
	{
		ShowMaxWindow();
	}
	else
	{
		CenterWindow();
	}
	return 0;
}

BOOL CMainFrame::IsMaxWindow()
{
	return m_bMaxWindow ? TRUE : FALSE;
}

void CMainFrame::ShowMaxWindow()
{
	MONITORINFO info = {sizeof(info)};
	GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &info);

	RECT rcWorkArea = info.rcWork;
	RECT rcMonitorArea = info.rcMonitor;

	CRect rtWindow;
	rtWindow.left   = abs(rcWorkArea.left - rcMonitorArea.left);
	rtWindow.top    = abs(rcWorkArea.top - rcMonitorArea.top);
	rtWindow.right  = rtWindow.left + abs(rcWorkArea.right - rcWorkArea.left);
	rtWindow.bottom = rtWindow.top + abs(rcWorkArea.bottom - rcWorkArea.top); 
	MoveWindow(&rtWindow);

	m_bMaxWindow = true;
}

void CMainFrame::ShowNormalWindow()
{
    MoveWindow(&GetWndDefRect());
	m_bMaxWindow = false;
}

LRESULT CMainFrame::HandleHttpError( WPARAM wParam, LPARAM lParam )
{
	ReleaseResToFile(TEXT("404_btm.jpg"), TEXT("images"), IDR_JPG_404BTM, TEXT("JPG"));
	ReleaseResToFile(TEXT("404_mid.jpg"), TEXT("images"), IDR_JPG_404MID, TEXT("JPG"));
	ReleaseResToFile(TEXT("404_pic.jpg"), TEXT("images"), IDR_JPG_404PIC, TEXT("JPG"));
	ReleaseResToFile(TEXT("404_top.jpg"), TEXT("images"), IDR_JPG_404TOP, TEXT("JPG"));
    ReleaseResToFile(TEXT("HttpError.html"),    TEXT(""), IDR_HTTPERROR, RT_HTML);

	m_WebPage.Stop();

	TCHAR szHttpErrorPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szHttpErrorPath, MAX_PATH);
	PathRemoveFileSpec(szHttpErrorPath);
	PathAddBackslash(szHttpErrorPath);
	_tcscat_s(szHttpErrorPath, _countof(szHttpErrorPath), TEXT("HttpError.html"));

	if (!PathFileExists(szHttpErrorPath))
	{
		m_WebPage.LoadFromResource(IDR_HTTPERROR);
	}
	else
	{
		m_WebPage.Navigate(szHttpErrorPath);
	}	

	return 1;
}

BOOL CMainFrame::ReleaseResToFile( CString strFileName, CString strDir, DWORD dwResId, LPCWSTR lpType)
{
	TCHAR szExePath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szExePath, MAX_PATH);
	PathRemoveFileSpec(szExePath);
	PathAddBackslash(szExePath);
	if (!strDir.IsEmpty())
	{
		strDir += TEXT("\\");
    	_tcscat_s(szExePath, _countof(szExePath), strDir);
		SHCreateDirectory(NULL, _bstr_t(szExePath));
	}	
    _tcscat_s(szExePath, _countof(szExePath), strFileName);

	if (!PathFileExists(szExePath))
	{
		HMODULE hMod = GetModuleHandle(NULL);
		HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(dwResId), lpType);
		if (hRes == NULL)
		{
			return FALSE;
		}

		DWORD dwSize  = SizeofResource(hMod, hRes);
		HGLOBAL hGlobal = LoadResource(hMod, hRes);
		if (hGlobal == NULL)
		{
			return FALSE;
		}

		LPVOID lpData = LockResource(hGlobal);
		if (lpData == NULL)
		{
			DWORD dwError = GetLastError();
			FreeResource(hRes);
			return FALSE;
		}
		HANDLE hFile = CreateFile(szExePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return FALSE;
		}
		DWORD dwWriteBytes = 0;
		if (!WriteFile(hFile, lpData, dwSize, &dwWriteBytes, NULL))
		{
			DWORD dwError = GetLastError();		
			CloseHandle(hFile);
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return FALSE;
		}
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		GlobalUnlock(hGlobal);
		FreeResource(hRes);
	}
	return TRUE;    
}