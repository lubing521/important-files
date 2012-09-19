// HotGamePanel.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "HotGamePanel.h"
#include "MainFrame.h"

IMPLEMENT_DYNAMIC(CHotGamePanel, CDialog)

CHotGamePanel::CHotGamePanel(CWnd* pParent /*=NULL*/)
	: CDialog(CHotGamePanel::IDD, pParent)
{
}

CHotGamePanel::~CHotGamePanel()
{
	//if (m_pbmpLstBk != NULL)
	//{
	//	m_pbmpLstBk->DeleteObject();
	//	delete m_pbmpLstBk;
	//	m_pbmpLstBk = NULL;
	//}
}

void CHotGamePanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOP_LINE,	m_btnTopLine);
	DDX_Control(pDX, IDC_TITLE1,	m_btnTitle1);
	DDX_Control(pDX, IDC_COMMENT1,	m_btnComment1);
	DDX_Control(pDX, IDC_GAME1,		m_btnGame1);
	DDX_Control(pDX, IDC_NUMBER,    m_btnNumer1);
	DDX_Control(pDX, IDC_GAME2,		m_btnGame2);
	DDX_Control(pDX, IDC_LIST_GAME, m_lstGame);
}

BEGIN_MESSAGE_MAP(CHotGamePanel, CDialog)
	ON_BN_CLICKED(IDOK, &CHotGamePanel::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHotGamePanel::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_MEASUREITEM()
 	ON_WM_DRAWITEM()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_GAME, OnGetdispinfoList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GAME, OnDblclkDockList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GAME, &CHotGamePanel::OnNMRClickListGame)

	ON_COMMAND(ID_POP_RUNGAME, &CHotGamePanel::OnListPopRunGame)
	ON_COMMAND(ID_POP_OPENDIR, &CHotGamePanel::OnListPopOpenDir)
	ON_COMMAND(ID_POP_SEARCHGAME, &CHotGamePanel::OnListPopSearchGame)
	ON_COMMAND(ID_POP_ADD_GAME_FAV, &CHotGamePanel::OnListPopAddGameFav)
	ON_COMMAND(ID_POP_SAVE_PROGRESS, &CHotGamePanel::OnListPopSaveProgress)
	ON_COMMAND(ID_POP_LOAD_PROGRESS, &CHotGamePanel::OnListPopLoadProgress)
END_MESSAGE_MAP()

BOOL CHotGamePanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_pbmpLstBk = AfxGetSkinImage(TEXT("Skin/Wnd/GameList_bg.png"));
	m_btnTopLine.SetImage2(TEXT("Skin/Wnd/hotgame/bg_left.png"), 
		TEXT("Skin/Wnd/hotgame/bg_1px.png"), TEXT("Skin/Wnd/hotgame/bg_right.png"));
	m_btnTitle1.SetImage(TEXT("Skin/Wnd/hotgame/title_1.png"));
	m_btnTitle1.SetParent(&m_btnTopLine);

	m_btnComment1.SetParent(&m_btnTopLine);
	m_btnComment1.SetImage(TEXT("Skin/Wnd/hotgame/bg_1px.png"));

	m_btnGame1.SetParent(&m_btnTopLine);
	m_btnGame1.SetImage(TEXT("Skin/Wnd/hotgame/bg_1px.png"));

	m_btnNumer1.SetParent(&m_btnTopLine);
	m_btnNumer1.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 0, 0));
	m_btnNumer1.SetImage(TEXT("Skin/Wnd/hotgame/bg_1px.png"));

	m_btnGame2.SetParent(&m_btnTopLine);
	m_btnGame2.SetImage(TEXT("Skin/Wnd/hotgame/bg_1px.png"));

	CString strUrl;
	TCHAR szMacAddr[30] = {0};
	AfxGetDbMgr()->GetMacAddr(szMacAddr, sizeof(szMacAddr));
	strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar10_rc3.html?nid=%lu&oemid=%lu&cmac=%s&ProductID=&ProductName="), AfxGetDbMgr()->GetIntOpt(OPT_U_NID), AfxGetDbMgr()->GetIntOpt(OPT_U_OEMID), szMacAddr);
	m_WebPage.SetNoScroll();
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST);
	m_WebPage.SetSilent(TRUE);
	m_WebPage.Navigate2(strUrl);

	m_ilLstCtrl.Create(32, 32, ILC_COLOR32|ILC_MASK, 1, 1);	

	m_lstGame.SetImageList(&m_ilLstCtrl, LVSIL_NORMAL);
	m_lstGame.SetBkColor(RGB(233, 236, 241));
	m_lstGame.SetTextBkColor(-1);
	m_lstGame.ModifyStyle(LVS_ALIGNLEFT, LVS_ALIGNTOP);
	m_lstGame.InsertColumn(0, TEXT("Name"), 80);
	m_lstGame.SetBkColor(RGB(255, 255, 255));
	int x = GetSystemMetrics(SM_CXICONSPACING);
	int y = GetSystemMetrics(SM_CYICONSPACING);
	if (x < 80)
		x = 80;
	if (y < 75) 
		y = 75;
	m_lstGame.SetIconSpacing(x, y);

	Refresh();

	return TRUE;
}

void CHotGamePanel::Refresh()
{
	m_GameInfo.clear();

	std::map<DWORD, tagGameInfo*>::iterator start = AfxGetDbMgr()->m_mapGameInfo.begin();
	std::map<DWORD, tagGameInfo*>::iterator end = AfxGetDbMgr()->m_mapGameInfo.end();
	for (; start != end; start ++)
	{
		m_GameInfo.push_back(start->second);
	}
	SortGameList<SORT_CLICK>(m_GameInfo);

	int nSize = 120;
	m_ilLstCtrl.SetImageCount(nSize);
	m_lstGame.SetItemCount(nSize);
	CString msg;
	msg.Format(TEXT("%d"), AfxGetDbMgr()->m_mapGameInfo.size());
	m_btnNumer1.SetWindowText(msg);
}

void CHotGamePanel::OnBnClickedOk()
{

}

void CHotGamePanel::OnBnClickedCancel()
{

}

HBRUSH CHotGamePanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	static CBrush brush(RGB(233, 236, 241));
	if (nCtlColor == CTLCOLOR_DLG)
	{
		return brush;
	}	
	return hbr;
}

void CHotGamePanel::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_btnTopLine.GetSafeHwnd() == NULL)
		return ;
	CRect rc;
	GetClientRect(&rc);
	rc.left = 5;
	rc.right -= 5;
	rc.top = 5;
	rc.bottom = rc.top + 31l;
	m_btnTopLine.MoveWindow(&rc);

	m_btnTopLine.GetClientRect(&rc);
	CRect rx = rc;
	rx.left = 2; rx.right = rx.left + 179;
	m_btnTitle1.MoveWindow(&rx);

	rx.left  = rc.right - 380;
	rx.right = rx.left + 200;
	m_btnComment1.MoveWindow(&rx);

	rx.OffsetRect(rx.Width(), 0);
	rx.right = rx.left + 95;
	m_btnGame1.MoveWindow(&rx);

	rx.OffsetRect(rx.Width(), 0);
	rx.right = rx.left + 40;
	m_btnNumer1.MoveWindow(&rx);

	rx.OffsetRect(rx.Width(), 0);
	rx.right = rx.left + 20;
	m_btnGame2.MoveWindow(&rx);

	GetClientRect(rc);
	rx = rc;
	rx.left = 10;
	rx.top = 40;
	rx.right = 405;
	rx.bottom -= 5;
	rx.bottom = rx.top + rx.Height();
	m_WebPage.MoveWindow(&rx);

	rx.top = 41;
	rx.left = 410;
	rx.right = rc.right;
	rx.bottom -= 5;
	m_lstGame.MoveWindow(&rx);

	//创建并设置listctrl的背景图片
	//{
	//	if (m_LstBmp.m_hObject != NULL)
	//		m_LstBmp.DeleteObject();

	//	CRect rc;
	//	m_lstGame.GetWindowRect(&rc);

	//	CDC dcScreen, dcMem;
	//	dcScreen.CreateDC(TEXT("Display"), NULL, NULL, NULL);
	//	dcMem.CreateCompatibleDC(&dcScreen);
	//	m_LstBmp.CreateCompatibleBitmap(&dcScreen, rc.Width(), rc.Height());
	//	CBitmap* pOldBmp = dcMem.SelectObject(&m_LstBmp);

	//	CRect rx(0, 0, rc.Width(), rc.Height());
	//	AfxDrawImage(&dcMem, m_pbmpLstBk, rx);

	//	dcScreen.SelectObject(pOldBmp);
	//	m_lstGame.SetBkImage(m_LstBmp);//, TRUE, 0, -2);
	//}
}

void CHotGamePanel::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo == NULL)
		return ;
	LV_ITEM* pItem= &(pDispInfo->item);
	size_t itemid = pItem->iItem;
	if (itemid >= m_GameInfo.size())
	{
		*pResult = 0;
		return ;
	}
	tagGameInfo* pGameInfo = m_GameInfo[itemid];
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

void CHotGamePanel::OnDblclkDockList(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->ShowUptPrg(m_GameInfo[nSel]);
}
void CHotGamePanel::OnNMRClickListGame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nItem == -1 || nItem >= (int)(m_GameInfo.size()))
		return ;

	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_TRAY_MENU));
	CMenu* pContextMenu = menu.GetSubMenu(1);

	CPoint oPoint;
	GetCursorPos( &oPoint );
	SetForegroundWindow();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	for (UINT idx=0; idx<pContextMenu->GetMenuItemCount(); idx++)
	{
		UINT uFlags =  0;
		switch(pFrame->m_pPopMenu[idx].dwId)
		{
		case ID_POP_SAVE_PROGRESS:
			{
				uFlags = MF_BYCOMMAND|MF_OWNERDRAW|MF_GRAYED;
				if (m_GameInfo[nItem]->SaveFilter.IsEmpty())
				{
					lstrcpy(pFrame->m_pPopMenu[idx].szText, TEXT("保存进度(未设置)"));
				}
				else
				{
					lstrcpy(pFrame->m_pPopMenu[idx].szText, TEXT("保存进度"));
				}
				break;
			}
		case ID_POP_LOAD_PROGRESS:
			{   
				uFlags = MF_BYCOMMAND|MF_OWNERDRAW|MF_GRAYED;
				if (m_GameInfo[nItem]->SaveFilter.IsEmpty())
				{
					lstrcpy(pFrame->m_pPopMenu[idx].szText, TEXT("读取进度(未设置)"));
				}
				else
				{
					lstrcpy(pFrame->m_pPopMenu[idx].szText, TEXT("读取进度"));
				}
				break;
			}
		default:
			{
				uFlags =  MF_BYCOMMAND|MF_OWNERDRAW;
				break;
			}
		}
		pContextMenu->ModifyMenu(pFrame->m_pPopMenu[idx].dwId, uFlags, 
			pFrame->m_pPopMenu[idx].dwId, (LPCTSTR)&pFrame->m_pPopMenu[idx]);
	}
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		oPoint.x, oPoint.y, this);
}


void CHotGamePanel::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMis)
{
	if (lpMis->CtlType != ODT_MENU)
	{
		CDialog::OnMeasureItem(nIDCtl, lpMis);
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

void CHotGamePanel::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDis)
{
	if (lpDis->CtlType != ODT_MENU)
	{
		CDialog::OnDrawItem(nIDCtl, lpDis);
		return ;
	}

	CDC* pDC = CDC::FromHandle(lpDis->hDC);
	pDC->SetBkMode(1);

	CRect rc(lpDis->rcItem);
	AfxDrawImage(pDC, TEXT("Skin/Menu/背景_1px.png"), rc);

	tagMenuItemEx* pItem = (tagMenuItemEx*)lpDis->itemData;
	if (pItem->dwGid < 0)
	{
		CRect rx = rc;
		rx.DeflateRect(0, 1, 0, 1);
		AfxDrawImage(pDC, TEXT("Skin/Menu/背景_分割线.png"), rx);
	}
	else if (pItem->dwGid > 0)
	{
		int nItem = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
		if (nItem != -1)
		{
			tagGameInfo* pGame = m_GameInfo[nItem];

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
			AfxDrawImage(pDC, TEXT("Skin/Menu/背景_按钮_鼠标经过.png"), rcImg);
		else
			AfxDrawImage(pDC, TEXT("Skin/Menu/背景_按钮_默认状态.png"), rcImg);

		CRect rx = rc;
		rx.left = 40;
		rx.right = rc.right - 10;
		pDC->DrawText(pItem->szText, &rx, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
	}
}

void CHotGamePanel::OnListPopRunGame()
{
	LRESULT result;
	OnDblclkDockList(NULL, &result);
}

void CHotGamePanel::OnListPopOpenDir()
{
	if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_BROWSEDIR))
	{
		CString strNotify;
		strNotify.Format(TEXT("对不起，当前设置不允许浏览游戏目录。"));
		AfxMessageBoxEx(strNotify);
		return;
	}	

	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;
	tagGameInfo* pGame = m_GameInfo[nSel];

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

void CHotGamePanel::OnListPopSearchGame()
{
	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;
	tagGameInfo* pGame = m_GameInfo[nSel];

	CWaitCursor wc;
	CString str;
	str.Format(TEXT("http://www.baidu.com/s?wd=%s&tn=licenseonline_pg"), (LPCTSTR)pGame->Name);
	ShellExecute(m_hWnd, TEXT("open"), str, TEXT(""), TEXT(""), SW_SHOWNORMAL);
}

void CHotGamePanel::OnListPopAddGameFav()
{
	return;
	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;
	tagGameInfo* pGame = m_GameInfo[nSel];
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
		CWnd* pParent = AfxGetMainWnd();
		if (pParent != NULL && pParent->m_hWnd != NULL)
		{
			pParent->SendMessage(WM_NAV_CLICK_MESSAGE, 9, 0);
		}
	}
	else
	{
		CString strError = TEXT("收藏游戏成功！");
		AfxMessageBoxEx(strError);
	}
}

void CHotGamePanel::OnListPopSaveProgress()
{
	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;
	tagGameInfo* pGame = m_GameInfo[nSel];
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

void CHotGamePanel::OnListPopLoadProgress()
{
	size_t nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || nSel >= m_GameInfo.size())
		return ;
	tagGameInfo* pGame = m_GameInfo[nSel];
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

void CHotGamePanel::RefreshWebpage()
{
    m_WebPage.Refresh();
}