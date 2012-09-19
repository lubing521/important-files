// DownGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DownGameDlg.h"
#include "ConsoleDlg.h"


IMPLEMENT_DYNAMIC(CDownGameDlg, CDialog)

CDownGameDlg::CDownGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownGameDlg::IDD, pParent)
{
	 m_bIsMultiSelect = FALSE;
}

CDownGameDlg::~CDownGameDlg()
{
}

void CDownGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUNTYPE, m_lstRunType);
	DDX_Control(pDX, IDC_LIST_ASYNC_TYPE, syncTypeList_);

	DDX_Control(pDX, IDC_COMBO_PRIORITY, m_cboPriority);
	DDX_Control(pDX, IDC_COMBO_AUTOUPT, m_cboAutoUpt);
	DDX_Control(pDX, IDC_COMBO_DOWN_PRIORITY, m_cb3upPriority);
}


BEGIN_MESSAGE_MAP(CDownGameDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDownGameDlg::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CString CDownGameDlg::GetAreaRunType(LPCSTR aid, const CString& szAreaRunType)
{
	for (int nIdx=0; ;nIdx++)
	{
		CString Item;
		AfxExtractSubString(Item, szAreaRunType, nIdx, '|');
		if (Item.IsEmpty())
			break;
		CString szAid, szRunType;
		AfxExtractSubString(szAid, Item, 0, ',');
		AfxExtractSubString(szRunType, Item, 1, ',');
		if (szAid == aid)
		{
			if (szRunType == "0")
				return RT_UNKNOWN;
			else if (szRunType == "1")
				return RT_LOCAL;
			else if (szRunType == "3")
				return RT_DIRECT;
			else
			{
				i8desk::VDiskInfoMapItr it = m_VDiskInfos.find((LPCSTR)szRunType);
				if (it ==  m_VDiskInfos.end())
					return RT_UNKNOWN;
				else
				{
					CString info;
					if (it->second->Type == VDISK_I8DESK)
					{
						info.Format("%s%c (%s:%C)", RT_VDISK, (char)it->second->CliDrv,
							i8desk::MakeIpString(it->second->IP).c_str(), (char)it->second->SvrDrv);
					}
					else
					{
						info.Format("%s%C", RT_VDISK, (char)it->second->CliDrv);
					}
					return info;
				}
			}
		}
	}
	return RT_UNKNOWN;
}

CString CDownGameDlg::ParaseAreaRunType(const CString& AreaName, const CString& RunType)
{
	CString aid;	
	for (i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
		it != m_AreaInfos.end(); it++)
	{
		if (AreaName == it->second->Name)
		{
			aid = it->first.c_str();
			aid += ",";
			break;
		}
	}

	if (RunType == RT_UNKNOWN)
		aid += "0";
	else if (RunType == RT_LOCAL)
		aid += "1";
	else if (RunType == RT_DIRECT)
		aid += "3";
	else
	{
		int dwPos = lstrlen(RT_VDISK);
		DWORD dwCliDrv = RunType.GetLength() > dwPos ? RunType.GetAt(dwPos) : 0;
		for (i8desk::VDiskInfoMapItr it = m_VDiskInfos.begin();
			it != m_VDiskInfos.end(); it++)
		{
			if (it->second->CliDrv== dwCliDrv)
			{
				aid += it->first.c_str();
				return aid;
			}
		}
		aid += "0";
	}
	return aid;
};

CString CDownGameDlg::GetAreaRTText(std::string& AreaName, i8desk::GameRunTypeList& RunType)
{
	i8desk::GameRunTypeListItr it = RunType.begin();
	for (; it != RunType.end(); it ++)
	{
		if (AreaName == it->AreaName)
		{
			switch (it->RunType)
			{
			case 0:
				return RT_UNKNOWN;
			case 1:
				return RT_LOCAL;
			case 3:
				return RT_DIRECT;
			case 2:
				{
					i8desk::VDiskInfoMapItr VIt = m_VDiskInfos.begin();
					for (; VIt != m_VDiskInfos.end(); VIt++)
					{
						if (strcmp(VIt->second->VID, it->VID) == 0)
						{
							CString info;
							if (VIt->second->Type == VDISK_I8DESK)
							{
								info.Format("%s%c (%s:%C)",
									RT_VDISK,
									(char)VIt->second->CliDrv,
									i8desk::MakeIpString(VIt->second->IP).c_str(),
									(char)VIt->second->SvrDrv);
							}
							else
							{
								info.Format("%s%C", RT_VDISK, (char)VIt->second->CliDrv);
							}
							return info;
						}
					}
				}
				break;
			}
		}
	}

	return RT_UNKNOWN;
}

std::string CDownGameDlg::GetAIDByName(std::string& Name)
{
	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); it ++)
	{
		if (Name == it->second->Name)
		{
			return it->first;
		}
	}
	return "";
}

BOOL CDownGameDlg::ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType)
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
		i8desk::VDiskInfoMapItr it = m_VDiskInfos.begin();
		for (; it != m_VDiskInfos.end(); it++) {
			if (it->second->CliDrv == CliDrv) {
				strcpy(RtType.VID, it->second->VID);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CDownGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	std::string	    ErrInfo;

	i8desk::AreaInfoMap AreaInfos;
	if (!pFrame->m_pDbMgr->GetAllArea(AreaInfos, ErrInfo)) {
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}		

	CWaitCursor wc;
	pFrame->m_pDbMgr->GetGameRunType(m_GameInfo.GID, m_RunType, ErrInfo, AreaInfos);
	if (!pFrame->m_pDbMgr->GetAllClass(m_ClassInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllVDisk(m_VDiskInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllArea(m_AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	if (!m_bIsMultiSelect)
	{
		char info[300] = {0};
		sprintf(info, "[gid=%05d][name=%s]", m_GameInfo.GID, m_GameInfo.Name);
		SetDlgItemText(IDC_GAMENAME, m_GameInfo.Name);

		if (!STRING_ISEMPTY(m_GameInfo.SvrPath))
		{
			SetDlgItemText(IDC_SVRPATH, m_GameInfo.SvrPath);
			if (STRING_ISEMPTY(m_GameInfo.CliPath))
			{
				i8desk::SAFE_STRCPY(m_GameInfo.CliPath, m_GameInfo.SvrPath);
				std::string Drv = pFrame->m_pDbMgr->GetOptString(OPT_M_GAMEDRV, "E");
				m_GameInfo.CliPath[0] = Drv[0];
			}
			SetDlgItemText(IDC_CLIPATH, m_GameInfo.CliPath);
		}
		else
		{
			for (i8desk::DefClassMapItr it = m_ClassInfos.begin();
				it != m_ClassInfos.end(); it ++)
			{
				if (it->first == m_GameInfo.DefClass)
				{
					if (!STRING_ISEMPTY(it->second->Path))
					{
						std::string path = it->second->Path;
						if (*path.rbegin() != '\\')
							path += "\\";
						path = path + m_GameInfo.Name + "\\";
						SetDlgItemText(IDC_SVRPATH, path.c_str());
						std::string Drv = pFrame->m_pDbMgr->GetOptString(OPT_M_GAMEDRV, "E");
						path[0] = Drv[0];
						SetDlgItemText(IDC_CLIPATH, path.c_str());
					}
					else
					{
						std::string path = pFrame->m_pDbMgr->GetOptString(OPT_D_INITDIR, "");
						if (path.size())
						{
							if (*path.rbegin() != '\\')
								path += "\\";
							path = path + it->second->Name + "\\" + m_GameInfo.Name + "\\";
							SetDlgItemText(IDC_SVRPATH, path.c_str());
							std::string Drv = pFrame->m_pDbMgr->GetOptString(OPT_M_GAMEDRV, "E");
							path[0] = Drv[0];
							SetDlgItemText(IDC_CLIPATH, path.c_str());
						}
					}
					break;
				}
			}
		}
	}
	else
	{
		SetDlgItemText(IDC_GAMENAME, "(默认)");
		SetDlgItemText(IDC_SVRPATH, "(默认)");
		SetDlgItemText(IDC_CLIPATH, "(默认)");
	}

	m_cboAutoUpt.AddString("自动更新");
	m_cboAutoUpt.AddString("手动更新");
	
	m_cboPriority.AddString("低:必删");
	m_cboPriority.AddString("中:可删");
	m_cboPriority.AddString("较高:暂不删");
	m_cboPriority.AddString("高:不删");

	m_cb3upPriority.AddString(_T("最低"));
	m_cb3upPriority.AddString(_T("较低"));
	m_cb3upPriority.AddString(_T("一般"));
	m_cb3upPriority.AddString(_T("较高"));
	m_cb3upPriority.AddString(_T("最高"));	
	
	if (m_bIsMultiSelect || !m_GameInfo.Status) {
		m_cboAutoUpt.SetCurSel(0);
		m_cboPriority.SetCurSel(2);
		m_cb3upPriority.SetCurSel(0);
	} else {
		m_cboAutoUpt.SetCurSel(m_GameInfo.AutoUpt ? 0 : 1);
		m_cboPriority.SetCurSel(m_GameInfo.Priority - 1);
		m_cb3upPriority.SetCurSel(m_GameInfo.DownPriority - 1);
	}

	m_lstRunType.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstRunType.InsertColumn(0, "分区", LVCFMT_LEFT, 80);

	//添加组合框的所有运行方式
	CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
	pComboTrait->SetStyle( pComboTrait->GetStyle() | CBS_DROPDOWNLIST);
	pComboTrait->AddItem(0, RT_UNKNOWN);
	pComboTrait->AddItem(1, RT_LOCAL);
	int loop = 2;
	for (i8desk::VDiskInfoMapItr it = m_VDiskInfos.begin();
		it != m_VDiskInfos.end(); it ++)
	{
		CString info;
		if (it->second->Type == VDISK_I8DESK)
		{
			info.Format("%s%c (%s:%C)", RT_VDISK, (char)it->second->CliDrv,
				i8desk::MakeIpString(it->second->IP).c_str(), (char)it->second->SvrDrv);
		}
		else
		{
			info.Format("%s%C", RT_VDISK, (char)it->second->CliDrv);
		}
		pComboTrait->AddItem(loop, info);
		loop ++;
	}
	pComboTrait->AddItem(loop, RT_DIRECT);

	m_lstRunType.InsertColumnTrait(1, "运行方式", LVCFMT_LEFT, 200, 1, pComboTrait);

	//添加游戏在每个分区的运行方式
	CString szDefRunType = pFrame->m_pDbMgr->GetOptString(OPT_D_AREADEFRUNTYPE, "").c_str();
	m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 0, DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 1, GetAreaRunType(DEFAULT_AREA_GUID, szDefRunType));

	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nIdx = m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), it->second->Name);
			m_lstRunType.SetItemText(nIdx, 0, it->second->Name);
			m_lstRunType.SetItemText(nIdx, 1, GetAreaRunType(it->first.c_str(), szDefRunType));
		}
	}


	// 同步任务
	

	if (!pFrame->m_pDbMgr->GetAllSyncTask(m_SyncTasks, ErrInfo)) 
	{
		AfxMessageBox(ErrInfo.c_str());
		return FALSE;
	}

	syncTypeList_.SetExtendedStyle(syncTypeList_.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	syncTypeList_.InsertColumn(0, "任务名称", LVCFMT_LEFT, 80);

	int index = 0;
	for(i8desk::SyncTaskMapItr iter = m_SyncTasks.begin(); iter != m_SyncTasks.end(); ++iter, ++index)
	{
		syncTypeList_.InsertItem(index, iter->second->Name);
		syncTypeList_.SetItemData(index, (DWORD_PTR)iter->second.get());
	}

	return TRUE;
}

void CDownGameDlg::OnBnClickedOk()
{
	if (!m_bIsMultiSelect)
	{
		CString path;
		GetDlgItemText(IDC_SVRPATH, path);
		path.Trim();
		if (!i8desk::IsValidDirName((LPCSTR)path))
		{
			AfxMessageBox("服务端路径不合法.");
			return ;
		}
		
		i8desk::SAFE_STRCPY(m_GameInfo.SvrPath, path);

		GetDlgItemText(IDC_CLIPATH, path);
		path.Trim();
		if (!i8desk::IsValidDirName((LPCSTR)path))
		{
			AfxMessageBox("客户端路径不合法.");
			return ;
		}
		i8desk::SAFE_STRCPY(m_GameInfo.CliPath, path);
	}

	m_GameInfo.AutoUpt = m_cboAutoUpt.GetCurSel() == 0;
	m_GameInfo.Priority = m_cboPriority.GetCurSel() + 1;
	m_GameInfo.DownPriority = m_cb3upPriority.GetCurSel() + 1;

	m_RunType.clear();
	for (int nIdx=0;nIdx<m_lstRunType.GetItemCount(); nIdx++)
	{
		i8desk::tagGameRunType RtType;
		RtType.AreaName = m_lstRunType.GetItemText(nIdx, 0);
		strcpy(RtType.AID, GetAIDByName(RtType.AreaName).c_str());
		RtType.GID = m_GameInfo.GID;
		if (ParaseAreaRTTexT(m_lstRunType.GetItemText(nIdx, 1), RtType))
			m_RunType.push_back(RtType);
	}
	
	
	// 添加游戏到同步任务
	typedef std::vector<i8desk::SyncTask *> SyncTaskVector;
	SyncTaskVector syncType;
	for(size_t i = 0; i != syncTypeList_.GetItemCount(); ++i)
	{
		if( syncTypeList_.GetCheck(i) )
		{
			syncType.push_back((i8desk::SyncTask *)syncTypeList_.GetItemData(i));
		}
	}


	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	std::string ErrInfo;

	// 如果已有同步游戏记录，则略过

	for (SyncTaskVector::iterator it = syncType.begin(); it != syncType.end(); ++it) 
	{
		i8desk::SyncGameMap syncGames;
		if( !pFrame->m_pDbMgr->GetTaskSyncGames(syncGames, (*it)->SID, ErrInfo) )
		{
			MessageBox(ErrInfo.c_str());
			return;
		}

		// 单选
		if( m_bIsMultiSelect == FALSE )
		{
			if( syncGames.find(m_GameInfo.GID) != syncGames.end() )
				continue;

			if (!pFrame->m_pDbMgr->AddSyncGame((*it)->SID, m_GameInfo.GID, ErrInfo))
			{
				AfxMessageBox(ErrInfo.c_str());
				return;
			}
		}
		else // 多选
		{
			for(i8desk::GameInfoVector::iterator iter = m_GamesInfo.begin();
				iter != m_GamesInfo.end();
				++iter)
			{
				if( syncGames.find((*iter)->GID) != syncGames.end() )
					continue;

				if (!pFrame->m_pDbMgr->AddSyncGame((*it)->SID, (*iter)->GID, ErrInfo))
				{
					AfxMessageBox(ErrInfo.c_str());
					return;
				}
			}
		}
	}	

	
	OnOK();
}

void CDownGameDlg::OnDestroy()
{
	m_ClassInfos.clear();
	m_AreaInfos.clear();
	m_VDiskInfos.clear();

	CDialog::OnDestroy();
}


