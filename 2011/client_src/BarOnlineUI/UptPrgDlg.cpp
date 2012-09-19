// UptPrgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "UptPrgDlg.h"
#include "MainFrame.h"
#include "UptOptDlg.h"
#include "MsgDlg.h"
#include "I8GameData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CUptPrgDlg, CDialog)

CUptPrgDlg::CUptPrgDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUptPrgDlg::IDD, pParent)
{
	m_hUpdate = NULL;
	m_enDownloadMode = E_UDM_NORMAL_DOWNLOAD;
	m_hPlugCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
}

CUptPrgDlg::~CUptPrgDlg()
{
}

void CUptPrgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTION, m_btnCaption);
	DDX_Control(pDX, IDC_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_GAMEPLUG, m_btnGamePlug);
	DDX_Control(pDX, IDC_PROGRESS, m_btnProgress);
	DDX_Control(pDX, IDC_STATUS, m_btnStatus);
	DDX_Control(pDX, IDC_TIME, m_btnTime);
	DDX_Control(pDX, IDC_RUN, m_btnRun);
	DDX_Control(pDX, IDC_SEARCH, m_btnSearch);
	DDX_Control(pDX, IDC_BUYCARD, m_btnBuyCard);
	DDX_Control(pDX, IDC_GAMESAVE, m_btnVidoSkill);
	DDX_Control(pDX, IDC_MODIFY, m_btnModify);
	DDX_Control(pDX, IDC_FAVORITE, m_btnFavorite);
	DDX_Control(pDX, IDC_PLUG,	   m_btnPlug);
	DDX_Control(pDX, IDC_LIST_GAME, m_lstGamePlug);
}

BEGIN_MESSAGE_MAP(CUptPrgDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_CLOSE,	&CUptPrgDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_RUN,		&CUptPrgDlg::OnBnClickedRun)
	ON_BN_CLICKED(IDC_SEARCH,	&CUptPrgDlg::OnBnClickedSearch)	
	ON_BN_CLICKED(IDC_BUYCARD,	&CUptPrgDlg::OnBnClickedBuycard)
	ON_BN_CLICKED(IDC_VIDEOSKILL, &CUptPrgDlg::OnBnClickedVideoSkill)
	ON_BN_CLICKED(IDC_MODIFY,	&CUptPrgDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_FAVORITE, &CUptPrgDlg::OnBnClickedFavorite)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_GAME, OnGetdispinfoList)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CLICK, IDC_LIST_GAME, &CUptPrgDlg::OnNMClickListGame)
END_MESSAGE_MAP()

BOOL CUptPrgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//界面布局处理
	{
		CString str, strUrl;
		CRect rcClient(0, 0, 514, 521), rc;
		CRgn rgn;
		rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 10, 10);

        TCHAR atGBKString[MAX_PATH];
        I8CD_GetGBKFromString(atGBKString, _countof(atGBKString), m_arGameInfo[0].atName);
		strUrl.Format(TEXT("http://ads.i8.com.cn/ad/client/adbar09_gx.html?ProductID=%lu&ProductName=%s&nid=%lu&oemid=%lu&cmac=%s"), m_arGameInfo[0].gid, atGBKString, I8CD_GetInternetBarID(), I8CD_GetChannelsID(),  LPCTSTR(I8CD_GetMacAddr()));
		m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, CRect(7, 31, 500+7, 250+31), this, AFX_IDW_PANE_FIRST);
		m_WebPage.SetSilent(TRUE);
		m_WebPage.Navigate2(strUrl);

		rc.left = 10; rc.top = 5;
		rc.right = 200; rc.bottom = 24;
		m_btnCaption.MoveWindow(&rc);
		//m_btnCaption.SetWindowText(m_arGameInfo[0].atName);
        I8CD_GameInfo_st* pstGameInfo = I8CD_GetGameInfo(m_arGameInfo[0].gid);
        if (pstGameInfo == NULL)
        {
            m_btnCaption.SetWindowText(m_arGameInfo[0].atName);
        }
        else
        {
            m_btnCaption.SetWindowText(pstGameInfo->stGameExInfo.atProductName);
            I8CD_ReleaseData(pstGameInfo);
        }
		m_btnCaption.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 255, 255), 
			RGB(255, 255, 255), RGB(255, 255, 255), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT);

		rc.right = rcClient.right - 15 ; rc.left = rc.right - 41;
		rc.top = 3; rc.bottom = rc.top + 24;
		m_btnClose.SetImage(TEXT("Skin/Wnd/GongGao/按钮_关闭_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_关闭_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_关闭_按下.png"));
		m_btnClose.MoveWindow(&rc);

		//关联游戏
		rc = CRect(10, 290, 110+10, 20+290);
		m_btnGamePlug.MoveWindow(&rc);

		m_iGamePlug.Create(32, 32, ILC_COLOR32|ILC_MASK, 1, 1);	
		m_lstGamePlug.SetImageList(&m_iGamePlug, LVSIL_NORMAL);
		m_lstGamePlug.SetBkColor(RGB(233, 236, 241));
		m_lstGamePlug.SetTextBkColor(-1);
		m_lstGamePlug.ModifyStyle(LVS_ALIGNLEFT, LVS_ALIGNTOP);
		m_lstGamePlug.InsertColumn(0, TEXT("Name"), 80);
		m_lstGamePlug.SetBkColor(RGB(255, 255, 255));

        m_nGameCount = 1;
        CI8ClientDataCoreReturnData<I8CD_GameIDArray_st, DWORD> clGameIDArray = I8CD_GetPertinentGame(m_arGameInfo[0].gid);
        for (int i = 0; i < clGameIDArray.GetCount() && m_nGameCount < _countof(m_arGameInfo); ++i)
        {
            m_arGameInfo[m_nGameCount] = *CI8GameData::Instance().GetGameInfo(clGameIDArray[i]);
            ++m_nGameCount;
        }
		m_nCurrentUptIdx = 0;
		
		m_iGamePlug.SetImageCount(m_nGameCount);
		m_lstGamePlug.SetItemCount(m_nGameCount);
		m_lstGamePlug.MoveWindow(10, 310, 494, 54);

		//中间状态
		rc = CRect(10, 374, 494+10, 26+374);	//长度是494.
		m_btnProgress.MoveWindow(&rc);
		m_btnProgress.SetProgress(0);
		m_btnProgress.SetImage(TEXT("Skin/Wnd/GongGao/背景_更新条.png"), TEXT("Skin/Wnd/GongGao/背景_更新条_1px.png"));

		rc.OffsetRect(0, 32); rc.bottom -= 2;
		rc.right = rc.left + 350;
		m_btnStatus.MoveWindow(&rc);
		m_btnStatus.SetImage(TEXT("Skin/Wnd/GongGao/bk.png"));
		m_btnStatus.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 255, 255), 
			RGB(255, 255, 255), RGB(255, 255, 255), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT);		

		rc.left = rc.right; rc.right = rcClient.right - 10;
		m_btnTime.MoveWindow(&rc);
		m_btnTime.SetImage(TEXT("Skin/Wnd/GongGao/bk.png"));
		m_btnTime.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 255, 255), 
			RGB(255, 255, 255), RGB(255, 255, 255), I8SkinCtrl_ns::CI8OldButton::BTEXT_LEFT);

		//下部按钮
		rc = CRect(22, 445, 70+22, 60+445);
		m_btnRun.MoveWindow(&rc);
		m_btnRun.SetImage(TEXT("Skin/Wnd/GongGao/按钮_运行游戏_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_运行游戏_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_运行游戏_按下.png"),
			NULL, TEXT("Skin/Wnd/GongGao/按钮_运行游戏_黑白状态.png"));		

		rc.OffsetRect(rc.Width() + 10, 0);
		m_btnSearch.MoveWindow(&rc);
		m_btnSearch.SetImage(TEXT("Skin/Wnd/GongGao/按钮_相关搜索_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_相关搜索_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_相关搜索_按下.png"));

		rc.OffsetRect(rc.Width() + 10, 0);
		m_btnBuyCard.MoveWindow(&rc);
		m_btnBuyCard.SetImage(TEXT("Skin/Wnd/GongGao/按钮_购买点卡_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_购买点卡_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_购买点卡_按下.png"));

		rc.OffsetRect(rc.Width() + 10, 0);
		m_btnVidoSkill.MoveWindow(&rc);
		m_btnVidoSkill.SetImage(TEXT("Skin/Wnd/GongGao/按钮_视频攻略_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_视频攻略_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_视频攻略_按下.png"));

		rc.OffsetRect(rc.Width() + 10, 0);
		m_btnModify.MoveWindow(&rc);
		m_btnModify.SetImage(TEXT("Skin/Wnd/GongGao/按钮_更新修复_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_更新修复_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_更新修复_按下.png"));		

		rc.OffsetRect(rc.Width() + 10, 0);
		m_btnFavorite.MoveWindow(&rc);
		m_btnFavorite.SetImage(TEXT("Skin/Wnd/GongGao/按钮_添加收藏_默认状态.png"), 
			TEXT("Skin/Wnd/GongGao/按钮_添加收藏_鼠标经过.png"), TEXT("Skin/Wnd/GongGao/按钮_添加收藏_按下.png"));		

		//m_btnPlug.SetImage(TEXT("Skin/Wnd/GongGao/背景_没插件.png"));
		//m_btnPlug.MoveWindow(&CRect(7, 312, 500+7, 55+312));


		MoveWindow(&rcClient, FALSE);
		SetWindowRgn(rgn, TRUE);

		CenterWindow();
	}

	m_bAutoRun	= true;
	m_bIsUpdate	= false;
	m_bNeedWait = true;

	SetTimer(2, 7000, NULL);	//停留N秒后才能运行游戏的计数。
	m_btnTime.SetWindowText(TEXT(""));

	for (m_nCurrentUptIdx = 0; m_nCurrentUptIdx < m_nGameCount; m_nCurrentUptIdx++)
	{
        if (UpdateGame(&m_arGameInfo[m_nCurrentUptIdx]))
        {
            m_btnRun.EnableWindow(FALSE);
            break;
        }
	}
	if (m_nCurrentUptIdx >= m_nGameCount)
	{
        m_btnStatus.SetWindowText(TEXT("正在启动游戏,单击停止."));
	}
	return TRUE;
}

void CUptPrgDlg::OnDestroy()
{
	CWaitCursor wc;
	if (m_hUpdate != NULL)
	{
		I8CD_StopUpdate(m_hUpdate);
		m_hUpdate = NULL;
	}
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_pDlgUptPrg = NULL;
	CDialog::OnDestroy();
}

void CUptPrgDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}

BOOL CUptPrgDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		if (pMsg->hwnd == m_btnStatus.m_hWnd ||
			pMsg->hwnd == m_btnProgress.m_hWnd ||
			pMsg->hwnd == m_btnTime.m_hWnd)
		{
			if (!m_bIsUpdate && m_bNeedWait && m_bAutoRun)
			{
				m_bAutoRun = false;
				m_btnStatus.SetWindowText(TEXT("己暂停，请手动运行游戏."));
				m_btnRun.EnableWindow(TRUE);
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CUptPrgDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

void CUptPrgDlg::OnOK()
{
	OnBnClickedClose();
}

void CUptPrgDlg::OnCancel()
{
	OnBnClickedClose();
}

void CUptPrgDlg::OnBnClickedClose()
{
	CWaitCursor wc;
	if (m_hUpdate != NULL)
	{
		I8CD_StopUpdate(m_hUpdate);
		m_hUpdate = NULL;
	}
	DestroyWindow();
}

CString CUptPrgDlg::RunGame(const I8CD_GameBaseInfo_st* pGame)
{
    CWaitCursor wc;
    I8CD_RunGameResult_en eResult = I8CD_RunGame(pGame->gid);
    switch (eResult)
    {
    case E_RGR_SUCESS:
        return CString(TEXT(""));
    case E_RGR_FAIL_VDISK:
        AfxMessageBoxEx(TEXT("虚拟盘信息设置有误，运行游戏失败！"));
        return CString(TEXT(""));
    case E_RGR_FILE_INEXIST:
        return TEXT("游戏的可执行文件不存在");
    default:
        return TEXT("运行游戏失败");
    }
}

void CUptPrgDlg::OnBnClickedRun()
{
	if (m_bIsUpdate )
		return ;

	m_bAutoRun = false;

	CString szStatus = RunGame(&m_arGameInfo[0]);
	if (!szStatus.IsEmpty())
	{
		m_btnStatus.SetWindowText(szStatus);
		m_btnRun.EnableWindow(TRUE);
	}
	else if (m_nGameCount > 1)
	{
		m_btnStatus.SetWindowText(TEXT("游戏启动成功，您可以点击游戏插件启动"));
		m_btnProgress.SetProgress(0.0f);
		m_btnModify.EnableWindow(FALSE);
	}
	else
	{
		m_btnModify.EnableWindow(FALSE);
		if (m_nGameCount == 1)
		{	
            I8CD_OperatorAfterRunGame_en eAct = I8CD_GetOperatorAfterRunGame();
			CWnd* pWnd = AfxGetMainWnd();
            if (eAct != E_OARG_NOTHING && pWnd != NULL && pWnd->m_hWnd != NULL)
			{
                if (eAct == E_OARG_MIN)
				{
					::PostMessage(pWnd->m_hWnd, WM_NAV_CLICK_MESSAGE, 6, 0);
				}
				else
				{
					::PostMessage(pWnd->m_hWnd, WM_CLOSE, 0, 0);
				}			
			}
			OnBnClickedClose();
		}
	}
}

void CUptPrgDlg::OnBnClickedSearch()
{
	CWaitCursor wc;
	CString str;
	str.Format(TEXT("http://www.baidu.com/s?wd=%s&tn=licenseonline_pg"), (LPCTSTR)m_arGameInfo[0].atName);
	ShellExecute(m_hWnd, TEXT("open"), str, TEXT(""), TEXT(""), SW_SHOWNORMAL);
}

void CUptPrgDlg::OnBnClickedBuycard()
{
	CWaitCursor wc;
	CString str;
	str.Format(TEXT("http://desk.i8.com.cn/CybercafeClient/Shop/ProductInfo.aspx?ProductID=%d"), m_arGameInfo[0].gid);
	ShellExecute(m_hWnd, TEXT("open"), str, TEXT(""), TEXT(""), SW_SHOWNORMAL);
}

void CUptPrgDlg::OnBnClickedVideoSkill()
{
	//视频攻略，弹出网址
}

void CUptPrgDlg::OnBnClickedModify() //XXXX
{
	if (m_hUpdate != NULL)
	{
		CWaitCursor wc;
		I8CD_StopUpdate(m_hUpdate);
		m_hUpdate = NULL;
		m_btnStatus.SetWindowText(TEXT("更新暂停..."));
	}

	m_bAutoRun = false;
	m_btnTime.SetWindowText(TEXT("剩余时间:"));
	try
	{
		CUptOptDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
            m_enDownloadMode = dlg.m_bIsQuick ? E_UDM_FAST_DOWNLOAD : E_UDM_RE_DOWNLOAD;
		}

		for (m_nCurrentUptIdx = 0; m_nCurrentUptIdx < m_nGameCount; m_nCurrentUptIdx++)
		{
            if (UpdateGame(&m_arGameInfo[m_nCurrentUptIdx]))
            {
                m_btnRun.EnableWindow(FALSE);
                break;
            }
		}
		m_bAutoRun = true;
	}
	catch (...) {}
}

void CUptPrgDlg::OnBnClickedFavorite()
{
	return;
}
//更新界面显示信息
void CUptPrgDlg::UpdateInfo(const I8CD_UpdateStatus_st& stStatus)
{
    if (m_nCurrentUptIdx < m_nGameCount)
    {
        if (m_bIsUpdate && m_hUpdate)
        {
            CString str;

            switch (stStatus.enStatus)
            {
            case E_US_STATUS_FINISH:
                m_hUpdate = NULL;
                for (m_nCurrentUptIdx++; m_nCurrentUptIdx < m_nGameCount; m_nCurrentUptIdx++)
                {
                    if (UpdateGame(&m_arGameInfo[m_nCurrentUptIdx]))
                    {
                        break;
                    }
                }
                if (m_nCurrentUptIdx >= m_nGameCount)
                {	
                    m_btnProgress.SetProgress(100);
                    m_btnStatus.SetWindowText(TEXT("正在启动游戏, 单击停止..."));
                    m_btnTime.SetWindowText(TEXT(""));
                    m_hUpdate = NULL;
                    m_bIsUpdate = false;
                    m_btnRun.EnableWindow(TRUE);
                }
                break;
            case E_US_STATUS_ERROR:
                m_btnProgress.SetProgress(0);
                str.Format(TEXT("%s"), stStatus.atInfo);
                m_btnStatus.SetWindowText(str);
                m_btnTime.SetWindowText(TEXT(""));
                m_hUpdate = NULL;
                m_bIsUpdate = false;
                m_bAutoRun = false;
                m_btnRun.EnableWindow(TRUE);
                break;
            case E_US_STATUS_COMPARE:
                m_btnProgress.SetProgress(0);
                m_btnStatus.SetWindowText(TEXT("正在对比文件..."));
                m_btnTime.SetWindowText(TEXT(""));
                break;
            case E_US_STATUS_UPDATE:
                m_btnProgress.SetProgress(float(stStatus.lUpdatedBytes*100/stStatus.lNeedsUpdateBytes));

                str.Format(TEXT("新组件正在更新:[%s][%dMB/%dMB, %dK/S]"), (LPCTSTR)m_arGameInfo[m_nCurrentUptIdx].atName, 
                    (DWORD)((stStatus.lNeedsUpdateBytes - stStatus.lUpdatedBytes)/1024/1024), 
                    (DWORD)(stStatus.lNeedsUpdateBytes/1024/1024),
                    stStatus.dwSpeed);
                m_btnStatus.SetWindowText(str);

                str.Format(TEXT("剩余时间:%02d:%02d:%02d"), 
                    stStatus.dwLeftTime / 3600, 
                    (stStatus.dwLeftTime % 3600) / 60, 
                    stStatus.dwLeftTime % 60);
                m_btnTime.SetWindowText(str);
                break;
            case E_US_STATUS_DELETE_MOREFILE:
                m_btnProgress.SetProgress(0);
                m_btnStatus.SetWindowText(TEXT("删除多余文件..."));
                m_btnTime.SetWindowText(TEXT(""));
                break;
            case E_US_STATUS_DELETE_GAME:
                m_btnProgress.SetProgress(0);
                str.Format(TEXT("%s"), (LPCTSTR)_bstr_t(stStatus.atInfo));
                m_btnStatus.SetWindowText(str);
                m_btnTime.SetWindowText(TEXT(""));
                break;
            default:
                break;
            }
        }
    }
}
void CUptPrgDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

    switch (nIDEvent)
    {
    case 1:
        if (!m_bNeedWait)
        {
            KillTimer(1);
        }
        UpdateInfo(m_stUpdateStatus);
        if (!m_bIsUpdate && !m_bNeedWait && m_bAutoRun)
        {
            OnBnClickedRun();
        }
        break;
    case 2:
        m_bNeedWait = false;
        KillTimer(2);
        break;
    default:
        break;
    }
}

BOOL CUptPrgDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Wnd/GongGao/背景_更新.png"), rc);
	return TRUE;
}

void CUptPrgDlg::OnGetdispinfoList( NMHDR* pNMHDR, LRESULT* pResult )
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo == NULL)
		return ;
	LV_ITEM* pItem= &(pDispInfo->item);
	size_t itemid = pItem->iItem;
	if ((int)itemid >= m_nGameCount - 1)
	{
		*pResult = 0;
		return ;
	}
	I8CD_GameBaseInfo_st* pGameInfo = &m_arGameInfo[(int)itemid + 1];
	if (pItem->mask & LVIF_TEXT)
	{
		//Which column?
		if (pItem->iSubItem == 0)
		{
			lstrcpyn(pItem->pszText, pGameInfo->atName, pItem->cchTextMax);
		}
	}

	if( pItem->mask & LVIF_IMAGE) 
	{
        HICON hIcon = CI8GameData::Instance().FindIconOfGame(pGameInfo->gid);
		m_iGamePlug.Replace(itemid, hIcon);
		pItem->iImage = itemid;
	}
	*pResult = 0;
}

BOOL CUptPrgDlg::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	if (m_lstGamePlug.m_hWnd && pWnd->m_hWnd == m_lstGamePlug.m_hWnd)
	{
		SetCursor(m_hPlugCursor);
		return TRUE;
	}    
	return FALSE;
}

void CUptPrgDlg::OnNMClickListGame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	if (m_bIsUpdate)
	{
		AfxMessageBoxEx(TEXT("请在更新完成后再启用插件！"));
		return;
	}

	size_t nSel = m_lstGamePlug.GetNextItem(-1, LVIS_SELECTED);
	if ((int)nSel == -1 || (int)nSel >= m_nGameCount - 1)
		return ;

	RunGame(&m_arGameInfo[(int)nSel + 1]);
}

void CUptPrgDlg::SetStartGame( const I8CD_GameBaseInfo_st* pGame )
{
	if (pGame) m_arGameInfo[0] = *pGame;
	m_enDownloadMode = E_UDM_NORMAL_DOWNLOAD;
}

BOOL CUptPrgDlg::UpdateGame( I8CD_GameBaseInfo_st* pGameInfo)
{
    m_bIsUpdate = FALSE;
    m_hUpdate = I8CD_UpdateGame(pGameInfo->gid, m_enDownloadMode, &CUptPrgDlg::OnUpdate, this);
    OutputDebugString(_T("'"));
    if (m_hUpdate == NULL)
    {
        return FALSE;
    }


    CString str;
	str.Format(TEXT("正在更新[%s]"), pGameInfo->atName);
    m_btnStatus.SetWindowText(str);

    m_bIsUpdate = TRUE;
	return TRUE;
}

void CUptPrgDlg::OnUpdate(HANDLE hUpdate, const I8CD_UpdateStatus_st* pstStatus, void* pCallBackParam)
{
    CUptPrgDlg *pDlg = reinterpret_cast<CUptPrgDlg*>(pCallBackParam);
    assert(pDlg != NULL);
    while (!pDlg->m_bIsUpdate);
    assert(hUpdate != NULL && pstStatus != NULL && pDlg->m_hUpdate == hUpdate);
    pDlg->m_stUpdateStatus = *pstStatus;
    pDlg->SetTimer(1, 0, NULL);
}