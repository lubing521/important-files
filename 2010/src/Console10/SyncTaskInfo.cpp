// SyncTaskInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "SyncTaskInfo.h"

#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CSpeedEdit, CEdit)

BEGIN_MESSAGE_MAP(CSpeedEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CSpeedEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//8=backspace
	if ((nChar >= '0' && nChar <= '9') || nChar == 8)
		CEdit::OnChar(nChar, nRepCnt, nFlags);
}

static BOOL IsLocMachineIP(const CString& szSvrIp /* = */ )
{
	if (szSvrIp == _T("0.0.0.0"))
		return FALSE;

	if (szSvrIp == _T("127.0.0.1"))
		return TRUE;

	IP_ADAPTER_INFO info[16] = {0};
	DWORD dwSize = sizeof(info);
	if (ERROR_SUCCESS != GetAdaptersInfo(info, &dwSize))
		return FALSE; //????

	PIP_ADAPTER_INFO pAdapter = info;
	while (pAdapter != NULL)
	{
		PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
		while (pAddr != NULL)
		{
			if (szSvrIp == pAddr->IpAddress.String)
				return TRUE;
			pAddr = pAddr->Next;
		}
		pAdapter = pAdapter->Next;
	}
	return FALSE;
}

// CSyncTaskInfo 对话框

IMPLEMENT_DYNAMIC(CSyncTaskInfo, CDialog)

CSyncTaskInfo::CSyncTaskInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncTaskInfo::IDD, pParent)
	, m_bAdd(TRUE)
{

}

CSyncTaskInfo::~CSyncTaskInfo()
{
}

void CSyncTaskInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYNC_TYPE, m_cbSyncType);
	DDX_Control(pDX, IDC_TREE_CUSTOM_SYNCGAME, m_tcCustomSyncGame);
	DDX_Control(pDX, IDC_IPADDRESS_TARGET_IP, m_ipcTargetIP);
	DDX_Control(pDX, IDC_COMBO_TARGET_PARTITION, m_cbTargetPartition);
	DDX_Control(pDX, IDC_EDIT_MAX_SPEED, m_editMaxSpeed);
}


BEGIN_MESSAGE_MAP(CSyncTaskInfo, CDialog)
	ON_BN_CLICKED(IDOK, &CSyncTaskInfo::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_SYNC_TYPE, &CSyncTaskInfo::OnCbnSelchangeComboSyncType)
	ON_NOTIFY(NM_CLICK, IDC_TREE_CUSTOM_SYNCGAME, &CSyncTaskInfo::OnNMClickTreeCustomSyncgame)
	ON_BN_CLICKED(IDC_CHECK, &CSyncTaskInfo::OnBnClickedCheck)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_TARGET_IP, &CSyncTaskInfo::OnIpnFieldchangedIpaddressTargetIp)
END_MESSAGE_MAP()

static LPCTSTR GetSyncTypeDesc(int type)
{
	static const TCHAR *const desc[] = {
		_T("未设置"),
		_T("完全镜像服务器"),
		_T("仅同步虚拟盘资源"),
		_T("仅同步本地更新资源"),
		_T("同步自定义资源"),
		_T("同步全部资源"),
	};
	
	if (type < sizeof(desc)/sizeof(desc[0])) {
		return desc[type];
	}

	ASSERT(0);
	return _T("未设置");
}

// CSyncTaskInfo 消息处理程序
BOOL CSyncTaskInfo::GetData(i8desk::SyncTask *SyncTask)
{
	GetDlgItemText(IDC_EDIT_TASK_NAME, SyncTask->Name, sizeof(SyncTask->Name) - 1);
	if (STRING_ISEMPTY(SyncTask->Name)) 
	{
		AfxMessageBox(_T("没有设定任务名称"));
		return FALSE;
	}
	SyncTask->BalanceType = btUnknown;

	SyncTask->MaxSyncSpeed = GetDlgItemInt(IDC_EDIT_MAX_SPEED);

	int iSel = m_cbSyncType.GetCurSel();
	if (iSel == -1) {
		AfxMessageBox(_T("没有选择同步方式"));
		return FALSE;
	}
	SyncTask->SyncType = iSel + 2;

	GetDlgItemText(IDC_IPADDRESS_TARGET_IP, SyncTask->SyncIP, sizeof(SyncTask->SyncIP));
	if (STRING_ISEMPTY(SyncTask->SyncIP) || strcmp(SyncTask->SyncIP, "0.0.0.0") == 0)
	{
		AfxMessageBox(_T("没有设置同步目标IP地址"));
		return FALSE;
	}


	// 检测同步IP，不能与服务器集群有相同IP
	i8desk::SyncTaskMap mirrorServers;
	i8desk::std_string strError;
	if( !GetConsoleDlg()->m_pDbMgr->GetAllMirrorServer(mirrorServers, strError) )
	{
		AfxMessageBox(strError.c_str());
		return FALSE;
	}

	for(i8desk::SyncTaskMap::iterator iter = mirrorServers.begin();
		iter != mirrorServers.end();
		++iter)
	{
		if( _tcsicmp(iter->second->SyncIP, SyncTask->SyncIP) == 0 )
		{
			AfxMessageBox(_T("镜像服务器会自动与主服务器保持同步，请勿重复添加同步任务。\r\n请重新设置同步IP"));
			GetDlgItem(IDC_IPADDRESS_TARGET_IP)->SetFocus();
			return FALSE;
		}
	}



	if (((CButton*)GetDlgItem(IDC_CHECK))->GetCheck()) {
		SyncTask->DestDrv = 0;
	}
	else {
		GetDlgItemText(IDC_COMBO_TARGET_PARTITION, (LPSTR)&SyncTask->DestDrv, 4);
		if (SyncTask->DestDrv == 0) {
			AfxMessageBox(_T("没有设置同步目标分区"));
			return FALSE;
		}
	}
	
	if (IsLocMachineIP(SyncTask->SyncIP) && SyncTask->DestDrv == 0) {
		AfxMessageBox(
			_T("不允许有同步到本机相同位置的设置\n")
			_T("请修改目标IP地址或设置一个同步盘符\n")
			);
		return FALSE;
	}

	return TRUE;
}

void CSyncTaskInfo::OnBnClickedOk()
{
	if (!GetData(&m_SyncTask))
		return;

	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	
	BOOL ok;
	std::string ErrInfo;

	CWaitCursor wc;
	if (m_bAdd)
		ok = pMainWnd->m_pDbMgr->AddSyncTask(m_SyncTask, ErrInfo);
	else
		ok = pMainWnd->m_pDbMgr->ModifySyncTask(m_SyncTask, ErrInfo);

	if (!ok) 
	{
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	//自定义同步资源
	if (m_SyncTask.SyncType == stCustom) 
	{
		ModifyCustomSyncGame(m_SyncTask.SID);
	}

	OnOK();
}

void CSyncTaskInfo::UpdateData(const i8desk::SyncTask *SyncTask)
{
	SetDlgItemText(IDC_EDIT_TASK_NAME, SyncTask->Name);
	m_cbSyncType.SetCurSel(SyncTask->SyncType - 2);
	if (SyncTask->MaxSyncSpeed)
		SetDlgItemInt(IDC_EDIT_MAX_SPEED, SyncTask->MaxSyncSpeed);

	UpdateTargetData(SyncTask);
	
	if (SyncTask->SyncType == stCustom) {
		GetDlgItem(IDC_TREE_CUSTOM_SYNCGAME)->EnableWindow(TRUE);
		UpdateCustomData(SyncTask);
	}
	else 
	{
		GetDlgItem(IDC_TREE_CUSTOM_SYNCGAME)->EnableWindow(FALSE);
	}

}

void CSyncTaskInfo::UpdateTargetData(const i8desk::SyncTask *SyncTask)
{
	m_ipcTargetIP.SetAddress( ntohl(inet_addr(SyncTask->SyncIP)) );
	if (SyncTask->DestDrv == 0) {
		GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(FALSE);
		m_cbTargetPartition.EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(TRUE);
	}
	else {
		((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(FALSE);
		GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(TRUE);
		m_cbTargetPartition.EnableWindow(TRUE);
		TCHAR buf[] = { (TCHAR)SyncTask->DestDrv, 0 };
		m_cbTargetPartition.SelectString(0, buf);
	}
}

void CSyncTaskInfo::UpdateCustomData(const i8desk::SyncTask *SyncTask)
{
	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	std::string ErrInfo;

	//取自定义同步游戏
	i8desk::SyncGameMap SyncGames;
	if (!pMainWnd->m_pDbMgr->GetTaskSyncGames(SyncGames, SyncTask->SID, ErrInfo)) {
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	for (HTREEITEM hClassItem = m_tcCustomSyncGame.GetRootItem(); 
		hClassItem != NULL; hClassItem = m_tcCustomSyncGame.GetNextSiblingItem(hClassItem)) 
	{
		BOOL bHasCheckedGame = FALSE;
		BOOL bHasUncheckedGame = FALSE;
		for (HTREEITEM hGameItem = m_tcCustomSyncGame.GetChildItem(hClassItem); 
			hGameItem != NULL; hGameItem = m_tcCustomSyncGame.GetNextSiblingItem(hGameItem)) 
		{
			DWORD gid = (DWORD)m_tcCustomSyncGame.GetItemData(hGameItem);
			BOOL bChecked = SyncGames.find(gid) != SyncGames.end();
			m_tcCustomSyncGame.SetCheck(hGameItem, bChecked);
			if (bChecked)
				bHasCheckedGame = TRUE;
			else 
				bHasUncheckedGame = TRUE;
		}

		if (bHasCheckedGame)
			m_tcCustomSyncGame.Expand(hClassItem, TVE_EXPAND);

		m_tcCustomSyncGame.SetCheck(hClassItem, !bHasUncheckedGame);
	}	

}

void CSyncTaskInfo::ModifyCustomSyncGame(const char *SID)
{
	CConsoleDlg *pMainWnd = (CConsoleDlg *)::AfxGetMainWnd();
	std::string ErrInfo;

	//删除全部
	if (!pMainWnd->m_pDbMgr->DelSyncGames(SID, ErrInfo)) {
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	//重新添加
	for (HTREEITEM hClassItem = m_tcCustomSyncGame.GetRootItem(); 
		hClassItem != NULL; hClassItem = m_tcCustomSyncGame.GetNextSiblingItem(hClassItem)) 
	{
		for (HTREEITEM hGameItem = m_tcCustomSyncGame.GetChildItem(hClassItem); 
			hGameItem != NULL; hGameItem = m_tcCustomSyncGame.GetNextSiblingItem(hGameItem)) 
		{
			if (m_tcCustomSyncGame.GetCheck(hGameItem)) 
			{
				i8desk::uint32 gid = (i8desk::uint32)m_tcCustomSyncGame.GetItemData(hGameItem);
				i8desk::GameInfoMapItr it = m_GameInfos.find(gid);
				
				if (it == m_GameInfos.end()) continue;
				 
				it->second->AutoSync = 1;	
				if (!pMainWnd->m_pDbMgr->AddSyncGame(SID, gid, ErrInfo)
					||!pMainWnd->m_pDbMgr->ModifyGame(*it->second, ErrInfo, MASK_TGAME_AUTOSYNC)) 
				{
					AfxMessageBox(ErrInfo.c_str());
					return;
				}
			}
			else
			{
				i8desk::uint32 gid = (i8desk::uint32)m_tcCustomSyncGame.GetItemData(hGameItem);
				i8desk::GameInfoMapItr it = m_GameInfos.find(gid);

				if (it == m_GameInfos.end()) continue;

				it->second->AutoSync = 0;	
				if (!pMainWnd->m_pDbMgr->ModifyGame(*it->second, ErrInfo, MASK_TGAME_AUTOSYNC)) 
				{
					AfxMessageBox(ErrInfo.c_str());
					return;
				}

			}
		}
	}

}

void CSyncTaskInfo::OnNMClickTreeCustomSyncgame(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CPoint pt;
	GetCursorPos(&pt);
	m_tcCustomSyncGame.ScreenToClient(&pt);

	UINT flags;
	HTREEITEM hItem = m_tcCustomSyncGame.HitTest(pt, &flags);
	if (!hItem || flags != TVHT_ONITEMSTATEICON)
		return;

	BOOL bChecked = !m_tcCustomSyncGame.GetCheck(hItem);
	if (HTREEITEM hClassItem = m_tcCustomSyncGame.GetParentItem(hItem)) {
		if (!bChecked)
			m_tcCustomSyncGame.SetCheck(hClassItem, FALSE);
	}
	else {
		for (HTREEITEM hGameItem = m_tcCustomSyncGame.GetChildItem(hItem);
			hGameItem != NULL; hGameItem = m_tcCustomSyncGame.GetNextSiblingItem(hGameItem))
		{
			m_tcCustomSyncGame.SetCheck(hGameItem, bChecked);
		}
	}
}

BOOL CSyncTaskInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ImageList.Create(IDB_TREE_CHECKBOX, 16, 3, RGB(255, 255, 255));
	m_tcCustomSyncGame.SetImageList(&m_ImageList, TVSIL_STATE);

	CWaitCursor wc;

	TCHAR buf[BUFSIZ];
	std::map<std::string, HTREEITEM> ClassItems;
	
	for (i8desk::GameInfoMapItr it = m_GameInfos.begin();
		it != m_GameInfos.end(); ++it) 
	{
		if (it->second->Status == 0)
			continue;

		i8desk::DefClass_Ptr DefClass = m_ClassInfos[it->second->DefClass];
		if (!DefClass) {
			continue;
		}

		HTREEITEM hClassItem = ClassItems[it->second->DefClass];
		if (!hClassItem) {
			hClassItem = m_tcCustomSyncGame.InsertItem(DefClass->Name);
			ClassItems[it->second->DefClass] = hClassItem;
		}

		::_stprintf(buf, _T("%u:%s"), it->second->GID, it->second->Name);
		HTREEITEM hGameItem = m_tcCustomSyncGame.InsertItem(buf, hClassItem);
		m_tcCustomSyncGame.SetItemData(hGameItem, it->second->GID);
	}

	m_cbSyncType.AddString(GetSyncTypeDesc(2));
	m_cbSyncType.AddString(GetSyncTypeDesc(3));
	m_cbSyncType.AddString(GetSyncTypeDesc(4));
	m_cbSyncType.AddString(GetSyncTypeDesc(5));

	for (char drv = 'D'; drv <= 'Z'; drv++) {
		char szDrv[] = {drv, 0};
		m_cbTargetPartition.AddString(szDrv);
	}
	
	if (m_bAdd) {
		i8desk::SAFE_STRCPY(m_SyncTask.SID, i8desk::CreateGuidString().c_str());
		SetWindowText(_T("添加同步任务"));
		SetDlgItemText(IDOK, _T("添加"));
	}
	else {
		CString caption = _T("修改同步任务: ");
		caption += m_SyncTask.Name;
		SetWindowText(caption);
		SetDlgItemText(IDOK, _T("修改"));
	}

	UpdateData(&m_SyncTask);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSyncTaskInfo::OnCbnSelchangeComboSyncType()
{
	int SyncType = m_cbSyncType.GetCurSel() + 2;
	if (SyncType == stCustom) {
		GetDlgItem(IDC_TREE_CUSTOM_SYNCGAME)->EnableWindow(TRUE);
		UpdateCustomData(&m_SyncTask);
	}
	else 
	{
		GetDlgItem(IDC_TREE_CUSTOM_SYNCGAME)->EnableWindow(FALSE);
	}
}


void CSyncTaskInfo::OnBnClickedCheck()
{
	if (((CButton*)GetDlgItem(IDC_CHECK))->GetCheck()) {
		CString TargetIP;
		GetDlgItem(IDC_IPADDRESS_TARGET_IP)->GetWindowText(TargetIP);
		if (IsLocMachineIP(TargetIP)) {
			AfxMessageBox(_T("不允许有同步到本机相同位置的设置\n"));
			((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(FALSE);
			GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(TRUE);
			m_cbTargetPartition.EnableWindow(TRUE);
			return;
		}
		GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(FALSE);
		m_cbTargetPartition.EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(TRUE);
		m_cbTargetPartition.EnableWindow(TRUE);
	}
}

void CSyncTaskInfo::OnIpnFieldchangedIpaddressTargetIp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);

	if (!((CButton*)GetDlgItem(IDC_CHECK))->GetCheck()) {
		return;
	}

	CString TargetIP;
	GetDlgItem(IDC_IPADDRESS_TARGET_IP)->GetWindowText(TargetIP);
	if (IsLocMachineIP(TargetIP)) {
		AfxMessageBox(
			_T("不允许有同步到本机相同位置的设置\n")
			_T("与源路径一致的选项已被自动取消\n")
			);
		((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(FALSE);
		GetDlgItem(IDC_STATIC_TARGET_PARTITION)->EnableWindow(TRUE);
		m_cbTargetPartition.EnableWindow(TRUE);
		return;
	}

	*pResult = 0;
}

