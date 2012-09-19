// DialogRealTSGame.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DialogRealTSGame.h"

#include "ConsoleDlg.h"

// CDialogRealTSGame 对话框

IMPLEMENT_DYNAMIC(CDialogRealTSGame, CDialog)

CDialogRealTSGame::CDialogRealTSGame(const std::vector<unsigned int> &vecGids, CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRealTSGame::IDD, pParent)
	, m_uLimitSpeed(0)
	, m_vecGids(vecGids)
{
	m_pParent = GetConsoleDlg();
}

CDialogRealTSGame::~CDialogRealTSGame()
{
}

void CDialogRealTSGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REAL_TSGAME_CLIENT, m_wndListClients);
	DDX_Control(pDX, IDC_LIST_REAL_TSGAME_GAME, m_wndListGames);
	DDX_Text(pDX, IDC_EDIT_REAL_TSGAME_SPEED, m_uLimitSpeed);
}


BEGIN_MESSAGE_MAP(CDialogRealTSGame, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CDialogRealTSGame::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogRealTSGame::OnBnClickedCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REAL_TSGAME_GAME, &CDialogRealTSGame::OnLvnItemchangedListRealTsgameGame)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REAL_TSGAME_CLIENT, &CDialogRealTSGame::OnLvnItemchangedListRealTsgameClient)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_CLIENT_SELECT_ALL, &CDialogRealTSGame::OnBnClickedCheckClientSelectAll)
	ON_BN_CLICKED(IDC_CHECK_GAME_SELECT_ALL, &CDialogRealTSGame::OnBnClickedCheckGameSelectAll)
END_MESSAGE_MAP()


// CDialogRealTSGame 消息处理程序


void CDialogRealTSGame::_InitListCtrl()
{
	_InitClientsList();
	_InitGamesList();
}

void CDialogRealTSGame::_InitClientsList()
{
	m_wndListClients.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	//m_wndListClients.InsertColumn(0, _T(""),		LVCFMT_LEFT, 25);
	m_wndListClients.InsertColumn(0, _T("机器名称"),	LVCFMT_LEFT, 120);
	m_wndListClients.InsertColumn(1, _T("IP"),		LVCFMT_LEFT, 100);

	int nIndex = 0;

	for(i8desk::MachineMap::iterator iter = m_OnlineMachines.begin();
		iter != m_OnlineMachines.end();
		++iter)
	{
		if( iter->second->online )
		{
			/*nIndex = m_wndListClients.InsertItem(nIndex, _T(""));
			m_wndListClients.SetItemText(nIndex, 1, CA2T(iter->second->Name));
			m_wndListClients.SetItemText(nIndex, 2, i8desk::MakeIpString(iter->second->IP).c_str());*/
			nIndex = m_wndListClients.InsertItem(nIndex, CA2T(iter->second->Name));
			m_wndListClients.SetItemText(nIndex, 1, i8desk::MakeIpString(iter->second->IP).c_str());
			m_wndListClients.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(iter->second.get()));
		}
	}
}

void CDialogRealTSGame::_InitGamesList()
{
	m_wndListGames.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_wndListGames.InsertColumn(0, _T("GID"),	LVCFMT_LEFT, 60);
	m_wndListGames.InsertColumn(1, _T("游戏名"), LVCFMT_LEFT, 120);
	m_wndListGames.InsertColumn(2, _T("大小(M)"),	LVCFMT_LEFT, 60);
	m_wndListGames.InsertColumn(3, _T("热门度"),	LVCFMT_LEFT, 60);
	m_wndListGames.InsertColumn(4, _T("等级"),	LVCFMT_LEFT, 60);

	int nIndex = 0, nFirst = -1;
	stdex::tString strGid;
	//stdex::tString strSize;
	stdex::tString strClick;
	CString strSize;

	m_wndListGames.SetFocus();
	for(i8desk::GameInfoMap::iterator iter = m_GameInfos.begin();
		iter != m_GameInfos.end();
		++iter)
	{
		stdex::ToString(iter->second->GID, strGid);
		//stdex::ToString(iter->second->Size, strSize);
		strSize.Format(_T("%0.2fM"), iter->second->Size * 1.0 / 1024);
		stdex::ToString(iter->second->IdcClick, strClick);

		int nCount = m_wndListGames.GetItemCount();
		nIndex = m_wndListGames.InsertItem(nCount, strGid.c_str());
		m_wndListGames.SetItemText(nIndex, 1, CA2T(iter->second->Name));
		m_wndListGames.SetItemText(nIndex, 2, strSize);
		m_wndListGames.SetItemText(nIndex, 3, strClick.c_str());
		m_wndListGames.SetItemText(nIndex, 4, iter->second->GetPriority().c_str());
		m_wndListGames.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(iter->second.get()));

		if( std::binary_search(m_vecGids.begin(), m_vecGids.end(),iter->second->GID) == true )
		{
			if( nFirst == -1 )
				nFirst = nIndex;

			m_wndListGames.SetCheck(nIndex, TRUE);
		}
	}
	
	BOOL bSuc = m_wndListGames.EnsureVisible(nFirst, FALSE);
}


void CDialogRealTSGame::_InitArguments()
{
	SetDlgItemInt(IDC_EDIT_REAL_TSGAME_SPEED, 0);

	//((CEdit*)(GetDlgItem(IDC_EDIT_REAL_TSGAME_SPEED)))->LimitText(5);
	((CButton*)(GetDlgItem(IDC_RADIO_REAL_TSGAME_INDEX)))->SetCheck(TRUE);

	// 设置提示信息
	_SetPanelInfo();
} 

void CDialogRealTSGame::_SetPanelInfo()
{
	// 设置游戏提示信息
	double fSize = 0.0;
	int nGameSelected = 0;
	for(int n = 0; n != m_wndListGames.GetItemCount(); ++n)
	{
		if( m_wndListGames.GetCheck(n) == TRUE )
		{
			i8desk::tagGameInfo *pGame = reinterpret_cast<i8desk::tagGameInfo *>(m_wndListGames.GetItemData(n));
			fSize += pGame->Size * 1.0 / 1024;

			nGameSelected += 1;
		}
	}

	
	// 设置客户机提示信息
	int nCount = 0;
	for(int n = 0; n != m_wndListClients.GetItemCount(); ++n)
	{
		if( m_wndListClients.GetCheck(n) == TRUE )
		{
			nCount += 1;
		}
	}

	_SetStaticInfo(nCount, nGameSelected, fSize);
}


void CDialogRealTSGame::_SetStaticInfo(int nClients, int nGames, double fSize)
{
	CString strClientsInfo, strGamesInfo;
	strClientsInfo.Format(_T("共选择客户机:%d台"), nClients);
	strGamesInfo.Format(_T("共选择游戏:%ld个,总大小为:%0.2fMB"), nGames, fSize);
	SetDlgItemText(IDC_STATIC_CLIENTS, strClientsInfo);
	SetDlgItemText(IDC_STATIC_GAMES, strGamesInfo);
}


void CDialogRealTSGame::_UpdateClients()
{
	LVFINDINFO info = {0};
	info.flags	= LVFI_STRING;


	for(i8desk::MachineMap::iterator iter = m_OnlineMachines.begin();
		iter != m_OnlineMachines.end();
		++iter)
	{
		info.psz = iter->second->Name;

		int nItem = m_wndListClients.FindItem(&info);
		// Insert
		if( nItem == -1 && iter->second->online == true )
		{
			int nIndex = m_wndListClients.InsertItem(m_wndListClients.GetItemCount(), CA2T(iter->second->Name));
			m_wndListClients.SetItemText(nIndex, 1, i8desk::MakeIpString(iter->second->IP).c_str());
			m_wndListClients.SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(iter->second.get()));
		}
		// Delete
		else if( nItem != -1 && iter->second->online == false )
		{
			m_wndListClients.DeleteItem(nItem);
		}
	
	}
}

BOOL CDialogRealTSGame::OnInitDialog()
{
	CDialog::OnInitDialog();


	i8desk::std_string strError;
	if( !m_pParent->m_pDbMgr->GetAllLocalGame(m_GameInfos, strError) )
	{
		AfxMessageBox(strError.c_str());
		return FALSE;
	}

	m_OnlineMachines = GetConsoleDlg()->m_Machines;
	

	_InitListCtrl();
	_InitArguments();

	// 更新客户机在线信息
	SetTimer(IDT_UPDATE_CLIENT, 2000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDialogRealTSGame::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CDialogRealTSGame::OnBnClickedOk()
{
	std::vector<DWORD> vecIP;
	i8desk::std_string strGids; 

	// 获取所选择的客户机
	for(int nIndex = m_wndListClients.GetItemCount() - 1; nIndex >= 0; --nIndex)
	{
		BOOL bChecked = m_wndListClients.GetCheck(nIndex);
		if( bChecked == TRUE )
		{
			i8desk::Machine *pMachine = reinterpret_cast<i8desk::Machine *>(m_wndListClients.GetItemData(nIndex));
			vecIP.push_back(pMachine->IP);
		}
	}
	
	// 获取所选择的游戏
	for(int nIndex = m_wndListGames.GetItemCount() - 1; nIndex >= 0; --nIndex)
	{
		BOOL bChecked = m_wndListGames.GetCheck(nIndex);
		if( bChecked == TRUE )
		{
			i8desk::GameInfo *pGame = reinterpret_cast<i8desk::GameInfo *>(m_wndListGames.GetItemData(nIndex));
			
			stdex::tString strGid;
			stdex::ToString(pGame->GID, strGid);
			strGids += strGid + _T("|");
		}
	}
	
	// 没有选择客户机或需要推送游戏
	if( vecIP.empty() )
	{
		AfxMessageBox(_T("请选择需要推送的客户机"));
		return;
	}
	if( strGids.empty() )
	{
		AfxMessageBox(_T("请选择需要推送的游戏"));
		return;
	}

	// 读取界面限速信息和对比方式
	UpdateData(TRUE);
	
	int nCheck = ((CButton*)(GetDlgItem(IDC_RADIO_REAL_TSGAME_FILE)))->GetCheck();
	
	DWORD dwConstructWay = 0;
	// 选择文件对比方式
	if( nCheck == BST_CHECKED )
		dwConstructWay = FC_COPY_QUICK;
	// 选择索引对比方式
	else
		dwConstructWay = FC_COPY_VERSION;

	i8desk::std_string strError;
	if( !m_pParent->m_pCliMgr->RealPush(vecIP, strGids, m_uLimitSpeed, dwConstructWay, strError) )
	{
		AfxMessageBox(strError.c_str());
		return;
	}
	
	if( MessageBox(_T("是否需要查看游戏推送详细信息?"), _T("提示"), MB_YESNO | MB_ICONINFORMATION) == IDYES )
		OnOK();
	else
		OnCancel();
}

void CDialogRealTSGame::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CDialogRealTSGame::OnLvnItemchangedListRealTsgameGame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMLV->uOldState == 0 && pNMLV->uNewState == 0)
		return;    // No change

	// Old check box state
	BOOL bPrevState = (BOOL)(((pNMLV->uOldState & LVIS_STATEIMAGEMASK)>>12)-1);  
	if (bPrevState < 0)		// On startup there's no previous state 
		bPrevState = 0;		// so assign as false (unchecked)

	// New check box state
	BOOL bChecked = 
		(BOOL)(((pNMLV->uNewState &LVIS_STATEIMAGEMASK)>>12)-1);   
	if (bChecked < 0) // On non-checkbox notifications assume false
		bChecked = 0; 

	if (bPrevState == bChecked) // No change in check box
		return;    // Now bChecked holds the new check box state

	_SetPanelInfo();
}

void CDialogRealTSGame::OnLvnItemchangedListRealTsgameClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMLV->uOldState == 0 && pNMLV->uNewState == 0)
		return;    // No change

	// Old check box state
	BOOL bPrevState = (BOOL)(((pNMLV->uOldState & LVIS_STATEIMAGEMASK)>>12)-1);  
	if (bPrevState < 0)		// On startup there's no previous state 
		bPrevState = 0;		// so assign as false (unchecked)

	// New check box state
	BOOL bChecked = 
		(BOOL)(((pNMLV->uNewState &LVIS_STATEIMAGEMASK)>>12)-1);   
	if (bChecked < 0) // On non-checkbox notifications assume false
		bChecked = 0; 

	if (bPrevState == bChecked) // No change in check box
		return;    // Now bChecked holds the new check box state

	_SetPanelInfo();
}

void CDialogRealTSGame::OnTimer(UINT_PTR nIDEvent)
{
	m_OnlineMachines = GetConsoleDlg()->m_Machines;

	_UpdateClients();

	CDialog::OnTimer(nIDEvent);
}

void CDialogRealTSGame::OnBnClickedCheckClientSelectAll()
{
	BOOL bChecked = reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_CLIENT_SELECT_ALL))->GetCheck();

	for(int i = 0; i != m_wndListClients.GetItemCount(); ++i)
	{
		if( bChecked == TRUE )
			m_wndListClients.SetCheck(i, TRUE);
		else
			m_wndListClients.SetCheck(i, FALSE);
	}
}

void CDialogRealTSGame::OnBnClickedCheckGameSelectAll()
{
	BOOL bChecked = reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_GAME_SELECT_ALL))->GetCheck();

	for(int i = 0; i != m_wndListGames.GetItemCount(); ++i)
	{
		if( bChecked == TRUE )
			m_wndListGames.SetCheck(i, TRUE);
		else
			m_wndListGames.SetCheck(i, FALSE);
	}
}
