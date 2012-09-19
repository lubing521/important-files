// MirrorServerInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "MirrorServerInfo.h"
#include "ConsoleDlg.h"

// CMirrorServerInfo 对话框

IMPLEMENT_DYNAMIC(CMirrorServerInfo, CDialog)

CMirrorServerInfo::CMirrorServerInfo(const i8desk::std_string &strMainServerID, CWnd* pParent /*=NULL*/)
	: CDialog(CMirrorServerInfo::IDD, pParent)
	, m_bAdd(FALSE)
	, m_bServerMachine(false)
	, m_strMainServerID(strMainServerID)
	, m_strSyncIP(_T("0.0.0.0"))
	, m_strVirtualIP(_T("0.0.0.0"))
	, m_strNetworkIP(_T("0.0.0.0"))
{

}

CMirrorServerInfo::~CMirrorServerInfo()
{
}

void CMirrorServerInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PRIORITY_AREA, m_tcPriorityArea);
	DDX_CBString(pDX, IDC_COMBO_SYNC, m_strSyncIP);
	DDX_CBString(pDX, IDC_COMBO_VIRTUAL, m_strVirtualIP);
	DDX_CBString(pDX, IDC_COMBO_NETWORK, m_strNetworkIP);
}


BEGIN_MESSAGE_MAP(CMirrorServerInfo, CDialog)
	ON_BN_CLICKED(IDOK, &CMirrorServerInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_BALANCE_DYNAMIC, &CMirrorServerInfo::OnBnClickedRadioBalanceDynamic)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_AREA, &CMirrorServerInfo::OnBnClickedRadioPriorityArea)
	ON_CBN_EDITCHANGE(IDC_COMBO_SYNC, &CMirrorServerInfo::OnCbnEditchangeComboSync)
	ON_CBN_SELCHANGE(IDC_COMBO_SYNC, &CMirrorServerInfo::OnCbnSelchangeComboSync)
END_MESSAGE_MAP()

BOOL CMirrorServerInfo::GetData(i8desk::SyncTask *SyncTask)
{
	reinterpret_cast<CDialog *>(this)->UpdateData(TRUE);

	GetDlgItemText(IDC_EDIT_TASK_NAME, SyncTask->Name, sizeof(SyncTask->Name) - 1);
	if (STRING_ISEMPTY(SyncTask->Name)) 
	{
		AfxMessageBox(_T("没有设定任务名称"));
		return FALSE;
	}

	
	// 检查IP合法性
	static CString strValidIP = _T("0.0.0.0");
	if( i8desk::IsValidIP((LPTSTR)(LPCTSTR)m_strSyncIP) && 
		i8desk::IsValidIP((LPTSTR)(LPCTSTR)m_strVirtualIP) &&
		i8desk::IsValidIP((LPTSTR)(LPCTSTR)m_strNetworkIP) &&
		m_strSyncIP != strValidIP &&
		m_strVirtualIP != strValidIP &&
		m_strNetworkIP != strValidIP )
	{
		i8desk::SAFE_STRCPY(SyncTask->SyncIP, m_strSyncIP);
		i8desk::SAFE_STRCPY(SyncTask->VDiskIP, m_strVirtualIP);
		i8desk::SAFE_STRCPY(SyncTask->UpdateIP, m_strNetworkIP);
	}
	else
	{
		AfxMessageBox(_T("IP输入不正确"));
		return FALSE;
	}


	SyncTask->SyncType = stMirror;
	SyncTask->MaxSyncSpeed = 0;
	SyncTask->DestDrv = 0;


	if (((CButton *)GetDlgItem(IDC_RADIO_BALANCE_DYNAMIC))->GetCheck())
		SyncTask->BalanceType = btDynamic;
	else 
		SyncTask->BalanceType = btAreaPriority;

	return TRUE;
}

void CMirrorServerInfo::UpdateData(const i8desk::SyncTask *SyncTask)
{
	if( m_bServerMachine == true )
		GetDlgItem(IDC_EDIT_TASK_NAME)->EnableWindow(FALSE);

	SetDlgItemText(IDC_EDIT_TASK_NAME, SyncTask->Name);

	CComboBox *pSyncComboBox	= reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SYNC));
	CComboBox *pVDiskComboBox	= reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_VIRTUAL));
	CComboBox *pUpdateComboBox	= reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_NETWORK));
	ASSERT(pSyncComboBox);
	ASSERT(pVDiskComboBox);
	ASSERT(pUpdateComboBox);

	
	m_strSyncIP		= SyncTask->SyncIP;
	m_strVirtualIP	= SyncTask->VDiskIP;
	m_strNetworkIP	= SyncTask->UpdateIP;

	pSyncComboBox->AddString(m_strSyncIP);
	pVDiskComboBox->AddString(m_strVirtualIP);
	pUpdateComboBox->AddString(m_strNetworkIP);


	// 如果是运行在服务器上，则插入本机IP,且点击为主服务器
	std::vector<CString> vecLocalIP;
	if( GetConsoleDlg()->IsRunOnServer() && 
		GetConsoleDlg()->GetLocalIP(vecLocalIP) &&
		m_bServerMachine == true)
	{
		std::for_each(vecLocalIP.begin(), vecLocalIP.end(),
			std::tr1::bind(&CComboBox::AddString, pSyncComboBox, std::tr1::placeholders::_1));
		std::for_each(vecLocalIP.begin(), vecLocalIP.end(),
			std::tr1::bind(&CComboBox::AddString, pVDiskComboBox, std::tr1::placeholders::_1));
		std::for_each(vecLocalIP.begin(), vecLocalIP.end(),
			std::tr1::bind(&CComboBox::AddString, pUpdateComboBox, std::tr1::placeholders::_1));
	}

	pSyncComboBox->SetCurSel(0);
	pVDiskComboBox->SetCurSel(0);
	pUpdateComboBox->SetCurSel(0);

	reinterpret_cast<CDialog *>(this)->UpdateData(FALSE);

	if (SyncTask->BalanceType == btDynamic) 
	{
		((CButton *)GetDlgItem(IDC_RADIO_BALANCE_DYNAMIC))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO_PRIORITY_AREA))->SetCheck(FALSE);
		GetDlgItem(IDC_TREE_PRIORITY_AREA)->EnableWindow(FALSE);
	}
	else 
	{
		((CButton *)GetDlgItem(IDC_RADIO_BALANCE_DYNAMIC))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO_PRIORITY_AREA))->SetCheck(TRUE);
		GetDlgItem(IDC_TREE_PRIORITY_AREA)->EnableWindow(TRUE);

		UpdatePriorityAreaData(SyncTask->SID);
	}
}

void CMirrorServerInfo::UpdatePriorityAreaData(const char *SID)
{
	for (HTREEITEM hItem = m_tcPriorityArea.GetRootItem(); 
		hItem != NULL; hItem = m_tcPriorityArea.GetNextSiblingItem(hItem)) 
	{
		const char *AID = (char *)m_tcPriorityArea.GetItemData(hItem);
		ASSERT(AID);

		i8desk::AreaInfo_Ptr AreaInfoPtr = m_AreaInfos[AID];
		ASSERT(AreaInfoPtr);

		m_tcPriorityArea.SetCheck(hItem, strcmp(AreaInfoPtr->SID, SID) == 0);
	}
}

void CMirrorServerInfo::ClearPriorityArea(const char *SID)
{
	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	std::string ErrInfo;

	for (i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
		it != m_AreaInfos.end(); ++it)
	{
		if (strcmp(it->second->SID, SID) == 0) 
		{
			i8desk::AreaInfo AreaInfo;
			i8desk::SAFE_STRCPY(AreaInfo.AID, it->second->AID);

			if (!pMainWnd->m_pDbMgr->ModifyArea(AreaInfo, ErrInfo, MASK_TAREA_SID))
			{
				AfxMessageBox(ErrInfo.c_str());
				return;
			}

			i8desk::SAFE_STRCPY(it->second->SID, "");
		}
	}
}


bool CMirrorServerInfo::_JudgeArea(LPCTSTR AID)
{
	for(i8desk::SyncTaskMapItr iter = m_MirrorServers.begin();
		iter != m_MirrorServers.end(); ++iter)
	{
		if( _tcsicmp(m_AreaInfos[AID]->SID, iter->second->SID) == 0 )
			return true;
	}
	return false;
}

bool CMirrorServerInfo::ModifyPriorityArea(const char *SID)
{
	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	std::string ErrInfo;

	for(HTREEITEM hItem = m_tcPriorityArea.GetRootItem(); 
		hItem != NULL; hItem = m_tcPriorityArea.GetNextSiblingItem(hItem)) 
	{
		const char *AID = (char *)m_tcPriorityArea.GetItemData(hItem);
		i8desk::AreaInfo_Ptr AreaInfoPtr = m_AreaInfos[AID];
		if (m_tcPriorityArea.GetCheck(hItem)) 
		{
			if( strcmp(AreaInfoPtr->SID, SID) != 0 )
			{
				// 检测该区域是否已经分配到其他服务器
				if( _JudgeArea(AID) == true )
				{
					CString strInfo;
					strInfo.Format(_T("%s区域已被分配到“%s服务器[%s]”，确认需要重新分配到此服务器吗？"), 
						m_AreaInfos[AID]->Name, m_AreaInfos[AID]->SID == m_strMainServerID ? _T("主") : _T("镜像"), m_MirrorServers[m_AreaInfos[AID]->SID]->Name);
					if( MessageBox(strInfo, _T("注意"), MB_ICONINFORMATION | MB_YESNO) != IDYES )
						return false;
				}

				// 取消被覆盖服务器的区域，保证区域所属唯一性
				/*i8desk::AreaInfo AreaInfoDel;
				i8desk::SAFE_STRCPY(AreaInfo.AID, AID);
				if (!pMainWnd->m_pDbMgr->ModifyArea(AreaInfo, ErrInfo, MASK_TAREA_SID))
				{
					AfxMessageBox(ErrInfo.c_str());
					return false;
				}
				i8desk::SAFE_STRCPY(AreaInfoPtr->SID, _T(""));*/



				// 选中的区域被设置为本服务优先
				i8desk::AreaInfo AreaInfoAdd;
				i8desk::SAFE_STRCPY(AreaInfoAdd.AID, AID);
				i8desk::SAFE_STRCPY(AreaInfoAdd.SID, SID);
				if (!pMainWnd->m_pDbMgr->ModifyArea(AreaInfoAdd, ErrInfo, MASK_TAREA_SID))
				{
					AfxMessageBox(ErrInfo.c_str());
					return false;
				}
				i8desk::SAFE_STRCPY(AreaInfoPtr->SID, SID);

				

			}
		}
		else
		{
			if (strcmp(AreaInfoPtr->SID, SID) == 0)
			{
				//没选中的如果原先的设置为本服务，则清除
				i8desk::AreaInfo AreaInfo;
				i8desk::SAFE_STRCPY(AreaInfo.AID, AID);
				if (!pMainWnd->m_pDbMgr->ModifyArea(AreaInfo, ErrInfo, MASK_TAREA_SID))
				{
					AfxMessageBox(ErrInfo.c_str());
					return false;
				}
				i8desk::SAFE_STRCPY(AreaInfoPtr->SID, "");
			}
		}
	}

	return true;
}

// CMirrorServerInfo 消息处理程序
BOOL CMirrorServerInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ImageList.Create(IDB_TREE_CHECKBOX, 16, 3, RGB(255, 255, 255));
	m_tcPriorityArea.SetImageList(&m_ImageList, TVSIL_STATE);

	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); ++it) 
	{
		HTREEITEM hItem = m_tcPriorityArea.InsertItem(it->second->Name);
		m_tcPriorityArea.SetItemData(hItem, (DWORD_PTR)it->second->AID);
	}

	UpdateData(&m_SyncTask);


	if (m_bAdd) 
	{
		i8desk::SAFE_STRCPY(m_SyncTask.SID, i8desk::CreateGuidString().c_str());
		SetWindowText(_T("添加镜像服务器"));
		SetDlgItemText(IDOK, _T("添加"));

		m_strSyncIP		= _T("0.0.0.0");
		m_strVirtualIP	= _T("0.0.0.0");
		m_strNetworkIP	= _T("0.0.0.0");
		
		reinterpret_cast<CDialog *>(this)->UpdateData(FALSE);
	}
	else 
	{
		// 判断是否为主服务器
		CString caption;
		if( m_bServerMachine == true )
			caption = _T("修改主服务器: ");
		else
			caption = _T("修改镜像服务器: ");

		caption += m_SyncTask.Name;
		SetWindowText(caption);
		SetDlgItemText(IDOK, _T("修改"));
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CMirrorServerInfo::OnBnClickedOk()
{
	if (!GetData(&m_SyncTask))
		return;

	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	
	BOOL ok;
	std::string ErrInfo;

	CWaitCursor wc;
	
	//区域优先负载均衡
	if (m_SyncTask.BalanceType == btAreaPriority) 
	{
		if( ModifyPriorityArea(m_SyncTask.SID) == false )
			return;
	}
	else 
	{
		ClearPriorityArea(m_SyncTask.SID);
	}

	OnOK();
}

void CMirrorServerInfo::OnBnClickedRadioBalanceDynamic()
{
	GetDlgItem(IDC_TREE_PRIORITY_AREA)->EnableWindow(FALSE);
}

void CMirrorServerInfo::OnBnClickedRadioPriorityArea()
{
	GetDlgItem(IDC_TREE_PRIORITY_AREA)->EnableWindow(TRUE);
}

void CMirrorServerInfo::OnCbnEditchangeComboSync()
{
	//CComboBox *pSyncComboBox = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SYNC));
	//int nCurSel = pSyncComboBox->GetEditSel();
	//if( nCurSel ==  CB_ERR )
	//	return;

	//reinterpret_cast<CDialog *>(this)->UpdateData(TRUE);

	//// 只有当主服务器虚拟盘地址或同步地址不为0时才同步更新
	//if( m_strNetworkIP == _T("0.0.0.0") )
	//	m_strNetworkIP = m_strSyncIP;
	//if( m_strVirtualIP == _T("0.0.0.0") )
	//	m_strVirtualIP = m_strSyncIP;

	////reinterpret_cast<CDialog *>(this)->UpdateData(FALSE);
	////reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SYNC))->Set;
	//SetDlgItemText(IDC_COMBO_VIRTUAL, m_strVirtualIP);
	//SetDlgItemText(IDC_COMBO_NETWORK, m_strVirtualIP);

	//pSyncComboBox->SetEditSel(HIWORD(nCurSel), HIWORD(nCurSel));
}

void CMirrorServerInfo::OnCbnSelchangeComboSync()
{
	CComboBox *pSyncComboBox = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_SYNC));
	pSyncComboBox->GetLBText(pSyncComboBox->GetCurSel(), m_strSyncIP);
	
	// 只有当主服务器虚拟盘地址或同步地址不为0时才同步更新
	if( m_strNetworkIP == _T("0.0.0.0") )
		m_strNetworkIP = m_strSyncIP;
	if( m_strVirtualIP == _T("0.0.0.0") )
		m_strVirtualIP = m_strSyncIP;


	reinterpret_cast<CDialog *>(this)->UpdateData(FALSE);	
}
