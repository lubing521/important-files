// TSGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "TSGameDlg.h"
#include "ConsoleDlg.h"
#include "AddGameDlg.h"

IMPLEMENT_DYNAMIC(CTSGameDlg, CDialog)

CTSGameDlg::CTSGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTSGameDlg::IDD, pParent)
{

}

CTSGameDlg::~CTSGameDlg()
{
}

void CTSGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GAME, m_lstGame);
}

BEGIN_MESSAGE_MAP(CTSGameDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTSGameDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD, &CTSGameDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &CTSGameDlg::OnBnClickedDel)
	ON_BN_CLICKED(IDC_UP, &CTSGameDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_DOWN, &CTSGameDlg::OnBnClickedDown)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CTSGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::string ErrInfo;
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());



	TCHAR szText[] = _T("后台更新用于在客户机开机时，后台限速更新指定游戏。\r\n注意：游戏在区域的运行方式为本地更新时，才会触发更新。\r\n例如：游戏设置A区为虚拟盘运行，B区为本地更新，则只有B区开机时才会后台更新此游戏。");
	SetDlgItemText(IDC_STATIC_TS_INFO, szText);
	SetDlgItemInt(IDC_EDIT_TSGAME_SPEED, pDlg->m_pDbMgr->GetOptInt(OPT_M_TSSPEED, 0));
	((CButton*)(GetDlgItem(IDC_RADIO_TSGAME_INDEX)))->SetCheck(TRUE);

	
	std::map<std::string, i8desk::tagBootTask*> BootTasks;
	if (!pDlg->m_pDbMgr->GetAllLocalGame(m_GameInfos, ErrInfo) || 
		!pDlg->m_pDbMgr->GetBootTask(brUpdateGame, BootTasks, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	m_lstGame.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstGame.InsertColumn(0, "GID",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(1, "游戏名",  LVCFMT_LEFT, 120);
	m_lstGame.InsertColumn(2, "大小(M)",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(3, "热门度",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(4, "等级",	LVCFMT_LEFT, 150);

	for (std::map<std::string, i8desk::tagBootTask*>::iterator it = BootTasks.begin();
		it != BootTasks.end(); it++)
	{
		CString Task;
		AfxExtractSubString(Task, it->second->Content, 1, '|');
		if(atoi(Task) != FC_COPY_VERSION)
		{
			((CButton*)(GetDlgItem(IDC_RADIO_TSGAME_INDEX)))->SetCheck(FALSE);
			((CButton*)(GetDlgItem(IDC_RADIO_TSGAME_FILE)))->SetCheck(TRUE);
		}

		for (int nIdx=2; ; nIdx++)
		{
			AfxExtractSubString(Task, it->second->Content, nIdx, '|');
			if (Task.IsEmpty())
				break;
						
			DWORD gid = atoi(Task);
			m_IDArray.insert(gid);

			i8desk::GameInfoMapItr Iter = m_GameInfos.find(gid);
			if (Iter != m_GameInfos.end())
			{
				CString msg;
				msg.Format("%05d", gid);
				LVFINDINFO info = {0};
				info.psz = msg;
				info.flags = LVFI_STRING;
				if (-1 == m_lstGame.FindItem(&info))
				{
					int nItem = m_lstGame.InsertItem(m_lstGame.GetItemCount(), msg);
					m_lstGame.SetItemText(nItem, 1, Iter->second->Name);
					msg.Format("%0.2fM", Iter->second->Size*1.0/1024);
					m_lstGame.SetItemText(nItem, 2, msg);
					msg.Format("%d", Iter->second->IdcClick);
					m_lstGame.SetItemText(nItem, 3, msg);
					m_lstGame.SetItemText(nItem, 4, Iter->second->GetPriority().c_str());
				}
			}
		}
		delete it->second;
	}
//	SetTimer(1, 200, NULL);

	return TRUE;
}

void CTSGameDlg::OnBnClickedAdd()
{
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	CAddGameDlg dlg;
	for (i8desk::GameInfoMapItr it = m_GameInfos.begin(); it != m_GameInfos.end(); it++)
	{
		if (m_IDArray.find(it->first) == m_IDArray.end())
		{
			dlg.m_GameInfos.insert(*it);
		}
	}
	if (dlg.DoModal() == IDOK && dlg.m_IDs.size())
	{
		// 开机推送游戏数量不能超过40个
		const int nCount = 40;
		if( dlg.m_IDs.size() >= nCount )
		{
			AfxMessageBox(_T("后台更新游戏不能超过40个"), MB_ICONWARNING | MB_OK);
		}
		CString Task;
		int n = 0;
		for (std::set<DWORD>::iterator it = dlg.m_IDs.begin(); it != dlg.m_IDs.end() && n != nCount; it++, ++n)
		{
			DWORD gid = *it;
			m_IDArray.insert(gid);
			CString msg;
			msg.Format("%05d", gid);
			int nItem = m_lstGame.InsertItem(m_lstGame.GetItemCount(), msg);
			i8desk::GameInfoMapItr Iter = m_GameInfos.find(gid);
			if (Iter != m_GameInfos.end())
			{
				m_lstGame.SetItemText(nItem, 1, Iter->second->Name);
				msg.Format("%0.2fM", Iter->second->Size*1.0/1024);
				m_lstGame.SetItemText(nItem, 2, msg);
				msg.Format("%d", Iter->second->IdcClick);
				m_lstGame.SetItemText(nItem, 3, msg);
				m_lstGame.SetItemText(nItem, 4, Iter->second->GetPriority().c_str());
			}
		}				
	}
}

void CTSGameDlg::OnBnClickedDel()
{
	int nSel = -1;
	std::vector<DWORD> Lines;
	while ((nSel = m_lstGame.GetNextItem(nSel, LVIS_SELECTED)) != -1)
	{
		Lines.push_back(nSel);
	}
	if (Lines.size() && MessageBox("确定要删除选定的后台更新游戏吗?", "提示:", MB_ICONINFORMATION|MB_OKCANCEL) ==IDOK)
	{
		for (std::vector<DWORD>::reverse_iterator it = Lines.rbegin(); it != Lines.rend(); it++)
		{
			DWORD nLine = *it;
			DWORD gid = atoi(m_lstGame.GetItemText(nLine, 0));
			m_IDArray.erase(gid);
			m_lstGame.DeleteItem(nLine);
		}
	}
}

void CTSGameDlg::OnBnClickedUp()
{
	int nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel <= 0)
		return ;
	CString szText;
	for (int nIdx=0; nIdx<m_lstGame.GetItemCount(); nIdx++)
	{
		szText = m_lstGame.GetItemText(nSel, nIdx);
		m_lstGame.SetItemText(nSel, nIdx, m_lstGame.GetItemText(nSel-1, nIdx));
		m_lstGame.SetItemText(nSel-1, nIdx, szText);
	}
}

void CTSGameDlg::OnBnClickedDown()
{
	int nSel = m_lstGame.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || m_lstGame.GetItemCount() == nSel + 1)
		return ;
	CString szText;
	for (int nIdx=0; nIdx<m_lstGame.GetItemCount(); nIdx++)
	{
		szText = m_lstGame.GetItemText(nSel, nIdx);
		m_lstGame.SetItemText(nSel, nIdx, m_lstGame.GetItemText(nSel+1, nIdx));
		m_lstGame.SetItemText(nSel+1, nIdx, szText);
	}
}

void CTSGameDlg::OnDestroy()
{
	m_GameInfos.clear();

	CDialog::OnDestroy();
}

void CTSGameDlg::OnBnClickedOk()
{
	static char cSep = '|';
	CString strInfo;
	
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());


	// 获取限制速度
	pDlg->m_pDbMgr->SetOption(OPT_M_TSSPEED, GetDlgItemInt(IDC_EDIT_TSGAME_SPEED));
	GetDlgItemText(IDC_EDIT_TSGAME_SPEED, strInfo);
	strInfo += cSep;

	// 获取对比方式
	FC_COPY_MODE nType;
	CString strType;
	if( ((CButton*)(GetDlgItem(IDC_RADIO_TSGAME_INDEX)))->GetCheck() == BST_CHECKED )
		nType = FC_COPY_VERSION;
	else
		nType = FC_COPY_QUICK;
	strType.Format(_T("%d"), nType);
	strInfo += strType + cSep;

	// 获取gid
	CString strGid;
	for (int nIdx=0; nIdx<m_lstGame.GetItemCount(); nIdx++)
	{
		CString Item;
		Item.Format("%d", atoi(m_lstGame.GetItemText(nIdx, 0)));
		if (strGid.GetLength())
			strGid += cSep;
		strGid += Item;
	}
	strInfo	+= strGid;
	

	// 更新开机任务
	std::string ErrInfo;
	if (!pDlg->m_pDbMgr->DelBootTask(brUpdateGame, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return ;
	}

	// 添加新的开机任务
	i8desk::tagBootTask Task;
	i8desk::SAFE_STRCPY(Task.UID, i8desk::CreateGUIDString().c_str());
	Task.AreaType = i8desk::TASK_AREA_ALL;
	CLEAR_STRING(Task.AreaParam);
	Task.Type = brUpdateGame;
	Task.Flag = 2;
	i8desk::SAFE_STRCPY(Task.Content, strInfo);
	if (!pDlg->m_pDbMgr->AddBootTask(&Task, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return ;
	}

	


	OnOK();
}

void CTSGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	if (IsWindowVisible())
	{
		CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
		std::set<DWORD> setGame;
		pDlg->GetAllSelectGame(setGame);
		if (setGame.size())
		{
			PostMessage(WM_COMMAND, IDC_ADD, (LPARAM)GetDlgItem(IDC_ADD)->m_hWnd);
		}
		KillTimer(1);
	}
}
