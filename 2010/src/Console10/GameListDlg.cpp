// GameListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameListDlg.h"
#include "ConsoleDlg.h"

// CGameListDlg 对话框

IMPLEMENT_DYNAMIC(CGameListDlg, CDialog)

CGameListDlg::CGameListDlg(
		i8desk::DefClassMap& DefClasses,
		i8desk::GameInfoMap& Games,
		int selector, 			
		int partition,
	   CWnd* pParent /*=NULL*/)
	: CDialog(CGameListDlg::IDD, pParent)
	, m_selector(selector)
	, m_partition(partition)
	, m_DefClasses(DefClasses)
{
	if (m_selector == MATCH) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->GID >= MIN_IDC_GID 
				&& it->second->Status == 1)
			{
				it->second->selected = 1;
				m_Games.push_back(it->second);
			}
		}
	}
	else if (m_selector == TODAYUPDATE) 
	{
		CTime now = CTime::GetCurrentTime();
		int nowYear = now.GetYear();
		int nowMonth = now.GetMonth();
		int nowDay = now.GetDay();

		//得到本周开始的时刻，用于计算本周中心更新的游戏数
		int nowDayOfWeek = now.GetDayOfWeek();
		CTime ThisWeekStart = now - CTimeSpan(nowDayOfWeek);

		CTime ThisWeekStartDay(ThisWeekStart.GetYear(), 
			ThisWeekStart.GetMonth(), ThisWeekStart.GetDay(), 0, 0, 0);
		DWORD ThisWeekStartTime = (DWORD)ThisWeekStartDay.GetTime();

		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->GID >= MIN_IDC_GID) 
			{
				if (it->second->IdcVer > ThisWeekStartTime)
				{
					CTime IdcUpdateTime = it->second->IdcVer;
					if (IdcUpdateTime.GetDay() == nowDay
						&& IdcUpdateTime.GetMonth() == nowMonth
						&& IdcUpdateTime.GetYear() == nowYear)
					{
						it->second->selected = 1;
						m_Games.push_back(it->second);
					}
				}
			}
		}
	}
	else if (m_selector == VALUENODOWN) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->I8Play == 1 && it->second->Status == 0)
			{
				it->second->selected = 1;
				m_Games.push_back(it->second);
			}
		}
	}
	else if (m_selector == AUTOUPDATE) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->Status == 1 && it->second->AutoUpt == 1)
			{
				it->second->selected = 1;
				m_Games.push_back(it->second);
			}
		}
	}
	else if (m_selector == MAYDELETE) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->Priority < 3 
				&& ::toupper(it->second->SvrPath[0]) == ::toupper(m_partition))
			{
				it->second->selected = 1;
				m_Games.push_back(it->second);
			}
		}
	}
	else if (m_selector == IDCADDNODOWN) 
	{
		CTime now = CTime::GetCurrentTime();
		CTime start(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
		start -= CTimeSpan(30,0,0,0); //30天算近期
		DWORD StartTime = (DWORD)start.GetTime();

		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->GID >= MIN_IDC_GID 
				&& it->second->Status == 0 
				&& it->second->AddDate > StartTime)
			{
				it->second->selected = 1;
				m_Games.push_back(it->second);
			}
		}
	}
	else if (m_selector == RTLOCALUPDATE) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->Status == 0)
				continue;

			for (size_t i = 0; i < it->second->RunTypes.size(); i++)
			{
				if (it->second->RunTypes[i].RunType == ERT_LOCAL)
				{
					it->second->selected = 1;
					m_Games.push_back(it->second);
					break;
				}
			}

		}
	}
	else if (m_selector == RTVDISK) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->Status == 0)
				continue;

			for (size_t i = 0; i < it->second->RunTypes.size(); i++)
			{
				if (it->second->RunTypes[i].RunType == ERT_VDISK)
				{
					it->second->selected = 1;
					m_Games.push_back(it->second);
					break;
				}
			}

		}
	}
	else if (m_selector == RTDIRECTRUN) 
	{
		i8desk::GameInfoMapItr it = Games.begin();
		for (; it != Games.end(); ++it)
		{
			if (it->second->Status == 0)
				continue;

			for (size_t i = 0; i < it->second->RunTypes.size(); i++)
			{
				if (it->second->RunTypes[i].RunType == ERT_DIRECT)
				{
					it->second->selected = 1;
					m_Games.push_back(it->second);
					break;
				}
			}
		}
	}
}

CGameListDlg::~CGameListDlg()
{
}

void CGameListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CGameListDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGameListDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, &CGameListDlg::OnLvnGetdispinfoList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CGameListDlg::OnNMClickList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, &CGameListDlg::OnLvnKeydownList)
	ON_BN_CLICKED(IDC_ALL, &CGameListDlg::OnBnClickedAll)
	ON_BN_CLICKED(IDC_CLEAR, &CGameListDlg::OnBnClickedClear)
END_MESSAGE_MAP()



BOOL CGameListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	switch (m_selector)
	{
	case MATCH:
		SetWindowText(_T("本地已匹配到中心资源的游戏"));
		GetDlgItem(IDOK)->SetWindowText(_T("确定"));
		GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEAR)->ShowWindow(SW_HIDE);
		break;
	case RTLOCALUPDATE:
		SetWindowText(_T("本地更新资源"));
		GetDlgItem(IDOK)->SetWindowText(_T("确定"));
		GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEAR)->ShowWindow(SW_HIDE);
		break;
	case RTVDISK:
		SetWindowText(_T("虚拟盘资源"));
		GetDlgItem(IDOK)->SetWindowText(_T("确定"));
		GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEAR)->ShowWindow(SW_HIDE);
		break;
	case RTDIRECTRUN:
		SetWindowText(_T("不更新直接运行资源"));
		GetDlgItem(IDOK)->SetWindowText(_T("确定"));
		GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEAR)->ShowWindow(SW_HIDE);
		break;
	case TODAYUPDATE:
		SetWindowText(_T("中心今日更新游戏"));
		GetDlgItem(IDOK)->SetWindowText(_T("下载"));
		break;
	case VALUENODOWN:
		SetWindowText(_T("本地未下载增值游戏"));
		GetDlgItem(IDOK)->SetWindowText(_T("下载"));
		break;
	case AUTOUPDATE:
		SetWindowText(_T("本地自动更新的游戏"));
		GetDlgItem(IDOK)->SetWindowText(_T("更新"));
		break;
	case MAYDELETE: {
		CString strCaption;
		strCaption.Format(_T("清理分区[%C:]上可删除的游戏"), m_partition);
		SetWindowText(strCaption);
		GetDlgItem(IDOK)->SetWindowText(_T("删除"));	 
		break; }
	case IDCADDNODOWN: 
		SetWindowText(_T("近期中心新增资源(未下载)"));
		GetDlgItem(IDOK)->SetWindowText(_T("下载"));
		break;
	default:
		ASSERT(0);
		break;
	}

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	m_list.InsertColumn(NETBAR_COL_NAME,			TEXT("名称"),			LVCFMT_LEFT, 120);
	m_list.InsertColumn(NETBAR_COL_GID,			TEXT("GID"),			LVCFMT_LEFT, 60);
	m_list.InsertColumn(NETBAR_COL_STATUS,		TEXT("当前状态"),		LVCFMT_LEFT, 70);
	m_list.InsertColumn(NETBAR_COL_AUTOUPT,		TEXT("更新方式"),		LVCFMT_LEFT, 60);
	m_list.InsertColumn(NETBAR_COL_PRIORITY,		TEXT("等级"),			LVCFMT_LEFT, 80);
	m_list.InsertColumn(NETBAR_COL_RUNTYPE,		TEXT("运行方式"),		LVCFMT_LEFT, 0);
	m_list.InsertColumn(NETBAR_COL_UPDATETIME,	TEXT("本地更新时间"),	LVCFMT_LEFT, 135);
	m_list.InsertColumn(NETBAR_COL_SVRCLICK,		TEXT("点击数"),			LVCFMT_RIGHT,50);
	m_list.InsertColumn(NETBAR_COL_GAMECLASS,	TEXT("类别"),			LVCFMT_LEFT, 70);
	m_list.InsertColumn(NETBAR_COL_SIZE,			TEXT("大小(M)"),		LVCFMT_RIGHT,70);
	m_list.InsertColumn(NETBAR_COL_GAMEEXE,		TEXT("执行程序"),		LVCFMT_LEFT, 120);
	m_list.InsertColumn(NETBAR_COL_SVRPATH,		TEXT("服务端路径"),		LVCFMT_LEFT, 100);
	m_list.InsertColumn(NETBAR_COL_DOWNPRIORITY,	TEXT("优先级别"),		LVCFMT_LEFT, 0);
	m_list.InsertColumn(NETBAR_COL_GAMESOURCE,	TEXT("游戏来源"),		LVCFMT_LEFT, 100);

	m_list.SetItemCount(m_Games.size());

	GetDlgItem(IDOK)->EnableWindow(m_Games.size() != 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CGameListDlg 消息处理程序

size_t CGameListDlg::GetSelectedGames(std::vector<DWORD>& games)
{
	for (size_t i = 0; i < m_Games.size(); i++)
	{
		i8desk::GameInfo_Ptr pGameInfo = m_Games[i];
		if (pGameInfo->selected) 
		{
			games.push_back(pGameInfo->GID);
		}
	}

	return games.size();
}

void CGameListDlg::OnBnClickedOk()
{
	i8desk::GameInfoVector GameInfos;
	for (size_t i = 0; i < m_Games.size(); i++)
	{
		i8desk::GameInfo_Ptr pGameInfo = m_Games[i];
		if (pGameInfo->selected) 
		{
			GameInfos.push_back(pGameInfo);
		}
	}

	CConsoleDlg *pMainWnd = static_cast<CConsoleDlg *>(AfxGetMainWnd());
	
	switch (m_selector)
	{
	case MAYDELETE:
		if (!pMainWnd->DelGames(GameInfos))
			return;
		break;
	case MATCH:
		break;
	case RTLOCALUPDATE:
		break;
	case RTVDISK:
		break;
	case RTDIRECTRUN:
		break;
	default:
		pMainWnd->DownGames(GameInfos);
		break;
	}

	OnOK();
}

CString CGameListDlg::GetGameClass(const i8desk::GameInfo& GameInfo)
{
	i8desk::DefClassMapItr it = m_DefClasses.find(GameInfo.DefClass);
	if (it != m_DefClasses.end())
		return CString(it->second->Name);
	return CString("");
}

void CGameListDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* pItem= &(pDispInfo)->item;
	int itemid = pItem->iItem;

	if (itemid >= (int)m_Games.size())
		return ;

	i8desk::GameInfo_Ptr pGameInfo = m_Games[itemid];
	ASSERT(pGameInfo);

	if (pItem->mask & LVIF_TEXT)
	{
		CString szText;
		switch(pItem->iSubItem)
		{			
		case NETBAR_COL_NAME:		szText = pGameInfo->Name;					break;
		case NETBAR_COL_GID:		szText.Format("%05d", pGameInfo->GID);				break;
		case NETBAR_COL_STATUS:		szText = pGameInfo->GetStatusText().c_str();		break;
		case NETBAR_COL_AUTOUPT:	szText = pGameInfo->AutoUpt ? "自动" : "手动";		break;
		case NETBAR_COL_PRIORITY:	szText = pGameInfo->GetPriority().c_str();			break;
		case NETBAR_COL_UPDATETIME: 
			szText = i8desk::MakeTimeString(pGameInfo->SvrVer).c_str();	
			break;
		case NETBAR_COL_SVRCLICK:	szText.Format("%d", pGameInfo->SvrClick);			break;				
		case NETBAR_COL_GAMECLASS:	szText = GetGameClass(*pGameInfo);					break;
		case NETBAR_COL_SIZE:		szText.Format("%0.2f", pGameInfo->Size*1.0/1024);	break;
		case NETBAR_COL_GAMEEXE:	szText = pGameInfo->GameExe;				break;
		case NETBAR_COL_SVRPATH:		szText = pGameInfo->SvrPath;				break;
		}
		lstrcpyn(pItem->pszText, szText, pItem->cchTextMax);
	}

	pItem->mask |= LVIF_STATE;
	pItem->stateMask = LVIS_STATEIMAGEMASK;

	pItem->state = INDEXTOSTATEIMAGEMASK(pGameInfo->selected ? 2 : 1);
}

void CGameListDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    LVHITTESTINFO hitinfo;
    hitinfo.pt = pNMItemActivate->ptAction;

    m_list.SubItemHitTest(&hitinfo);
	if(hitinfo.iItem != -1 && hitinfo.iSubItem == 0)
    {
        //看看鼠标是否单击在 check box上面了?
        if(hitinfo.flags == LVHT_ONITEMSTATEICON)
        {
			i8desk::GameInfo_Ptr pGameInfo = m_Games[hitinfo.iItem];
			pGameInfo->selected = !pGameInfo->selected;
			m_list.RedrawItems(hitinfo.iItem, hitinfo.iItem);
        }
    }
    
	std::vector<DWORD> clients;
	GetDlgItem(IDOK)->EnableWindow(GetSelectedGames(clients));

	*pResult = 0;
}

void CGameListDlg::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    if( pLVKeyDown->wVKey == VK_SPACE )
    {
	    int item = m_list.GetSelectionMark();
        if(item != -1)
		{
			i8desk::GameInfo_Ptr pGameInfo = m_Games[item];
			pGameInfo->selected = !pGameInfo->selected;
			m_list.RedrawItems(item, item);
		}
    }

	std::vector<DWORD> games;
	GetDlgItem(IDOK)->EnableWindow(GetSelectedGames(games));

    *pResult = 0;
}

void CGameListDlg::OnBnClickedAll()
{
	for (size_t i = 0; i < m_Games.size(); i++)
	{
		i8desk::GameInfo_Ptr pGameInfo = m_Games[i];
		pGameInfo->selected = true;
	}
	m_list.RedrawWindow();
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CGameListDlg::OnBnClickedClear()
{
	for (size_t i = 0; i < m_Games.size(); i++)
	{
		i8desk::GameInfo_Ptr pGameInfo = m_Games[i];
		pGameInfo->selected = false;
	}
	m_list.RedrawWindow();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}
