// WizardDownloadBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "ConsoleDlg.h"
#include "WizardDownloadBoard.h"
#include "SetUptTimeDlg.h"
#include "BrowseDirDlg.h"

// CWizardDownloadBoard 对话框

IMPLEMENT_DYNAMIC(CWizardDownloadBoard, CDialog)

CWizardDownloadBoard::CWizardDownloadBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWizardDownloadBoard::IDD, pParent)
	, m_bModify(FALSE)
	, m_bModifyClass(FALSE)
	, m_lcClass(m_bModifyClass)
{

}

CWizardDownloadBoard::~CWizardDownloadBoard()
{
}

void CWizardDownloadBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUNTYPE, m_lstRunType);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
	DDX_Control(pDX, IDC_CHECK_TIMEUPT, m_btnTimeUpt);
	DDX_Control(pDX, IDC_LIST_CLASS, m_lcClass);
}

BEGIN_MESSAGE_MAP(CWizardDownloadBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWizardDownloadBoard::OnApplyMessage)
	ON_BN_CLICKED(IDOK, &CWizardDownloadBoard::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWizardDownloadBoard::OnBnClickedCancel)

	ON_EN_CHANGE(IDC_INITDIR, &CWizardDownloadBoard::OnEnChangeInitdir)
	ON_EN_CHANGE(IDC_DOWNTEMPDIR, &CWizardDownloadBoard::OnEnChangeInitdir)

	ON_EN_CHANGE(IDC_TASKNUM, &CWizardDownloadBoard::OnEnChangeInitdir)
	ON_CBN_SELCHANGE(IDC_DOWNSPEED, &CWizardDownloadBoard::OnEnChangeInitdir)
	ON_BN_CLICKED(IDC_BUTTON1, &CWizardDownloadBoard::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD, &CWizardDownloadBoard::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &CWizardDownloadBoard::OnBnClickedDel)
	ON_BN_CLICKED(IDC_CHECK_TIMEUPT, &CWizardDownloadBoard::OnBnClickedCheckTimeupt)
	ON_BN_CLICKED(IDC_SELECT_MSCTMPDIR, &CWizardDownloadBoard::OnBnClickedSelectMsctmpdir)
	ON_CBN_SELCHANGE(IDC_JOIN_TYPE, &CWizardDownloadBoard::OnCbnSelchangeJoinType)
	ON_CBN_SELCHANGE(IDC_TASKNUM, &CWizardDownloadBoard::OnCbnSelchangeTasknum)
END_MESSAGE_MAP()


CString CWizardDownloadBoard::GetAreaRunType(LPCSTR aid, const CString& szAreaRunType)
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

CString CWizardDownloadBoard::ParaseAreaRunType(const CString& AreaName, const CString& RunType)
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
			if (it->second->CliDrv == dwCliDrv)
			{
				aid += it->first.c_str();
				return aid;
			}
		}
		aid += "0";
	}
	return aid;
};

static const char *DownSpeeds[] = {
	"80",
	"150",
	"300",
	"550",
	"1100",
	"2000",
	"4000",
	"8200",
	"16500",
	"不限制"
};
static const size_t nDownSpeeds = sizeof(DownSpeeds)/sizeof(char *);

BOOL CWizardDownloadBoard::OnInitDialog()
{
	CDialog::OnInitDialog();
	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	i8desk::CDbMgr* pDbMgr = pFrame->m_pDbMgr;
	std::string	    ErrInfo;

	((CComboBox*)GetDlgItem(IDC_JOIN_TYPE))->AddString("ADSL");
	((CComboBox*)GetDlgItem(IDC_JOIN_TYPE))->AddString("光纤");
	((CComboBox*)GetDlgItem(IDC_JOIN_TYPE))->SetCurSel(pDbMgr->GetOptInt(OPT_D_JOINTYPE, 0));

	((CEdit*)GetDlgItem(IDC_TASKNUM))->LimitText(2);
	((CEdit*)GetDlgItem(IDC_DOWNSPEED))->LimitText(4);

	SetDlgItemText(IDC_INITDIR, pDbMgr->GetOptString(OPT_D_INITDIR, "E:\\").c_str());
	SetDlgItemText(IDC_DOWNTEMPDIR,	  pDbMgr->GetOptString(OPT_D_TEMPDIR, "E:\\msctmp\\").c_str());

	CComboBox *pTaskNumCtrl = (CComboBox *)GetDlgItem(IDC_TASKNUM);
	pTaskNumCtrl->AddString("1");
	pTaskNumCtrl->AddString("2");
	pTaskNumCtrl->AddString("3");

	int nTaskNum = GetDbMgr()->GetOptInt(OPT_D_TASKNUM);
	if (nTaskNum < 1 || nTaskNum > 3)
	{
		nTaskNum = 3;
		GetDbMgr()->SetOption(OPT_D_TASKNUM, nTaskNum);
	}
	pTaskNumCtrl->SetCurSel(nTaskNum - 1);

	//限速
	CComboBox *dwonspeed = (CComboBox *)GetDlgItem(IDC_DOWNSPEED);
	for (size_t i = 0; i < nDownSpeeds; i++)
		dwonspeed->AddString(DownSpeeds[i]);

	int nValue = pDbMgr->GetOptInt(OPT_D_SPEED);
	if (nValue) {
		for (size_t i = 0; i < nDownSpeeds; i++) {
			if (nValue <= atoi(DownSpeeds[i])) {
				dwonspeed->SetCurSel(i);
				break;
			}
		}
	}

	if (dwonspeed->GetCurSel() == -1) {
		dwonspeed->SelectString(0, "不限制");
	}

	m_lstCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstCtrl.InsertColumn(0, "开始时间", LVCFMT_LEFT, 80);
	m_lstCtrl.InsertColumn(1, "停止时间", LVCFMT_LEFT, 80);

	m_btnTimeUpt.SetCheck(pDbMgr->GetOptInt(OPT_D_TIMEUPT, 0));
	CString Value = pDbMgr->GetOptString(OPT_D_TIMEUPTVALUE, "").c_str();
	for (int nIdx=0; ; nIdx++)
	{
		CString szItem, szStart, szEnd;
		AfxExtractSubString(szItem, Value, nIdx, '|');
		if (szItem.IsEmpty())
			break;

		AfxExtractSubString(szStart, szItem, 0, ',');
		AfxExtractSubString(szEnd, szItem, 1, ',');
		if (!szStart.IsEmpty() && !szEnd.IsEmpty())
		{
			int nItem = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), szStart);
			m_lstCtrl.SetItemText(nItem, 1, szEnd);
		}
	}

	CWaitCursor wc;
	if (!pFrame->m_pDbMgr->GetAllVDisk(m_VDiskInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllArea(m_AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
	}

	m_lstRunType.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstRunType.InsertColumn(0, "分区", LVCFMT_LEFT, 80);

	UpdateClassList();

	UpdateRunTypeList();

	OnBnClickedCheckTimeupt();

	m_bModify = FALSE;

	return TRUE;
}

BOOL CWizardDownloadBoard::UpdateRunTypeList()
{
	CWaitCursor wc;

	std::string ErrInfo;
	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	if (!pFrame->m_pDbMgr->GetAllVDisk(m_VDiskInfos, ErrInfo) ||
		!pFrame->m_pDbMgr->GetAllArea(m_AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return FALSE;
	}
	m_lstRunType.DeleteAllItems();

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

	m_lstRunType.DeleteColumn(1);
	m_lstRunType.InsertColumnTrait(1, "运行方式", LVCFMT_LEFT, 150, 1, pComboTrait);

	std::string DefRunType = pFrame->m_pDbMgr->GetOptString(OPT_D_AREADEFRUNTYPE, "");

	//添加游戏在每个分区的运行方式
	m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 0, DEFAULT_AREA_NAME);
	m_lstRunType.SetItemText(0, 1, GetAreaRunType(DEFAULT_AREA_GUID, DefRunType.c_str()));
	i8desk::AreaInfoMapItr it = m_AreaInfos.begin();
	for (; it != m_AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nIdx = m_lstRunType.InsertItem(m_lstRunType.GetItemCount(), it->second->Name);
			m_lstRunType.SetItemText(nIdx, 0, it->second->Name);
			m_lstRunType.SetItemText(nIdx, 1, GetAreaRunType(it->first.c_str(), DefRunType.c_str()));
		}
	}

	return TRUE;
}


BOOL CWizardDownloadBoard::UpdateClassList()
{
	CWaitCursor wc;

	std::string ErrInfo;
	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	if (!pFrame->m_pDbMgr->GetAllClass(m_DefClasses, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return FALSE;
	}

	m_lcClass.DeleteAllItems();
	m_lcClass.DeleteColumn(0);
	m_lcClass.DeleteColumn(1);
	m_lcClass.DeleteColumn(2);

	m_lcClass.InsertColumn(0, _T(""), LVCFMT_LEFT, 0); //ID
	m_lcClass.InsertColumn(1, _T("类别名称"), LVCFMT_LEFT, 180);

	//添加组合框的所有运行方式
	CGridColumnTraitEdit* pTrait = new CGridColumnTraitEdit;
	m_lcClass.InsertColumnTrait(2, _T("默认下载路径"),
		LVCFMT_LEFT, 280, 1, pTrait);

	CString strDefRootPath;
	GetDlgItem(IDC_INITDIR)->GetWindowText(strDefRootPath);

	for (i8desk::DefClassMapItr it = m_DefClasses.begin();
		it != m_DefClasses.end(); it ++)
	{
		int nIdx = m_lcClass.InsertItem(m_lcClass.GetItemCount(), it->first.c_str());
		m_lcClass.SetItemText(nIdx, 1, it->second->Name);
		
		if (it->second->Path[0] == 0) {
			CString strDefPath;
			strDefPath.Format(_T("(%s%s)"), (LPCTSTR)strDefRootPath, it->second->Name);
			m_lcClass.SetItemText(nIdx, 2, strDefPath);
		}
		else 
			m_lcClass.SetItemText(nIdx, 2, it->second->Path);
	}

	return TRUE;
}

void CWizardDownloadBoard::OnBnClickedOk()
{
	
}

void CWizardDownloadBoard::OnBnClickedCancel()
{
	
}

void CWizardDownloadBoard::WriteP2PConfig()
{
	CConsoleDlg *pMainWnd = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	CString strSvrPath = pMainWnd->GetI8DeskSvrPath();
	CString P2PConfig = strSvrPath + _T("DNAService\\AppData\\conf\\P2PConfig.xml");	

	CMarkup xml;
	if (!xml.Load((LPCTSTR)P2PConfig)) {
		AfxMessageBox(_T("修改三层下载配置文件时无法读入文件."));
		return;
	}
	xml.ResetMainPos();

	if (xml.FindElem("P2PAgent")) {
		CString str;
		str.Format(_T("%d"), GetDlgItemInt(IDC_DOWNSPEED)*1024);
		xml.SetAttrib("bandwidth_download", (LPCTSTR)str);
		xml.SetAttrib("bandwidth_upload", (LPCTSTR)str);

		str.Format(_T("%d"), GetDlgItemInt(IDC_CONNNUM));
		xml.SetAttrib("connections_download", (LPCTSTR)str);
		xml.SetAttrib("connections_upload", (LPCTSTR)str);
	}

	if (!xml.Save((LPCTSTR)P2PConfig)) {
		AfxMessageBox(_T("修改三层下载配置文件时无法写入文件."));
		return;
	}
}

LRESULT CWizardDownloadBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	CConsoleDlg *pMainWnd = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	i8desk::CDbMgr* pDbMgr = pMainWnd->m_pDbMgr;

	if (m_bModifyClass)
	{
		for (int nItem = 0; nItem < m_lcClass.GetItemCount(); nItem++) 
		{
			CString strDefPath = m_lcClass.GetItemText(nItem, 2);
			ASSERT(!strDefPath.IsEmpty());

			//由设置的下载根路径和类别名字合成的缺省位置，不需要写入
			if (strDefPath[0] == _T('(')) 
				continue; 

			i8desk::DefClass DefClass;
			i8desk::SAFE_STRCPY(DefClass.DefClass, (LPCTSTR)m_lcClass.GetItemText(nItem, 0));
			i8desk::SAFE_STRCPY(DefClass.Path, (LPCTSTR)strDefPath);
		
			std::string ErrInfo;
			if (!pDbMgr->ModifyClass(DefClass, ErrInfo, MASK_TCLASS_PATH)) 
			{
				AfxMessageBox(ErrInfo.c_str());
				return FALSE;
			}
		}

		m_bModifyClass = FALSE;
	}

	if (m_bModify)
	{
		CString strInitDir;
		GetDlgItemText(IDC_INITDIR,		strInitDir);
		strInitDir.Trim();

		if (!i8desk::IsValidDirName((LPCSTR)strInitDir))
		{
			AfxMessageBox("下载初始目录路径不是有效的路径名.");
			return FALSE;
		}

		if (strInitDir[strInitDir.GetLength() - 1] != _T('\\')) {
			strInitDir += _T("\\");
		}

		CString strTempDir;
		GetDlgItemText(IDC_DOWNTEMPDIR,	strTempDir);
		strTempDir.Trim();

		if (!i8desk::IsValidDirName((LPCSTR)strTempDir))
		{
			AfxMessageBox("下载临时目录路径不是有效的路径名.");
			return FALSE;
		}

		if (strTempDir[strTempDir.GetLength() - 1] != _T('\\')) {
			strTempDir += _T("\\");
		}

		int nValue = GetDlgItemInt(IDC_TASKNUM);
		if (nValue > 3 || nValue < 1)
		{
			AfxMessageBox("任务数只能设置为1-3.");
			return FALSE;
		}

		int nSpeedValue = GetDlgItemInt(IDC_DOWNSPEED);
		if (nSpeedValue != 0 && nSpeedValue < 60)
		{
			AfxMessageBox("最高下载速度设置值不能低于60KB/s.");
			return FALSE;
		}

		pDbMgr->SetOption(OPT_D_INITDIR, (LPCSTR)strInitDir);
		pDbMgr->SetOption(OPT_D_TEMPDIR, (LPCSTR)strTempDir);
		pDbMgr->SetOption(OPT_D_JOINTYPE, ((CComboBox*)GetDlgItem(IDC_JOIN_TYPE))->GetCurSel());
		
		pDbMgr->SetOption(OPT_D_TASKNUM, GetDlgItemInt(IDC_TASKNUM));

		pDbMgr->SetOption(OPT_D_SPEED, GetDlgItemInt(IDC_DOWNSPEED));
		pDbMgr->SetOption(OPT_D_TIMEUPT, m_btnTimeUpt.GetCheck());
		CString szValue;
		for (int nIdx=0; nIdx<m_lstCtrl.GetItemCount(); nIdx++)
		{
			if (!szValue.IsEmpty())
				szValue += "|";
			szValue += m_lstCtrl.GetItemText(nIdx, 0);
			szValue += ",";
			szValue += m_lstCtrl.GetItemText(nIdx, 1);
		}
		pDbMgr->SetOption(OPT_D_TIMEUPTVALUE, (LPCSTR)szValue);

		if (pMainWnd->IsRunOnServer()) {
//			WriteP2PConfig();
		}

		m_bModify = FALSE;
	}

	CString szRunType;
	for (int nIdx=0; nIdx<m_lstRunType.GetItemCount(); nIdx++)
	{
		if (szRunType.GetLength())
			szRunType += "|";
		szRunType += ParaseAreaRunType(m_lstRunType.GetItemText(nIdx, 0), m_lstRunType.GetItemText(nIdx, 1));
	}

	pDbMgr->SetOption(OPT_D_AREADEFRUNTYPE, (LPCSTR)szRunType);


	return TRUE;
}

void CWizardDownloadBoard::OnEnChangeInitdir()
{
	CString strDefRootPath;
	GetDlgItem(IDC_INITDIR)->GetWindowText(strDefRootPath);

	strDefRootPath.Trim();
	if (!i8desk::IsValidDirName((LPCTSTR)strDefRootPath)) {
		return;
	}

	if (strDefRootPath[strDefRootPath.GetLength() - 1] != _T('\\')) {
		strDefRootPath += _T("\\");
	}

	for (int nItem = 0; nItem < m_lcClass.GetItemCount(); nItem++) {
		if (m_lcClass.GetItemText(nItem, 2)[0] == _T('(')) {
			CString strDefPath;
			CString strName = m_lcClass.GetItemText(nItem, 1);
			strDefPath.Format(_T("(%s%s)"), (LPCTSTR)strDefRootPath, (LPCTSTR)strName);
			m_lcClass.SetItemText(nItem, 2, strDefPath);
			m_bModifyClass = TRUE;
		}
	}

	m_bModify = TRUE;	
}

static char buf[MAX_PATH] = {0};
static int CALLBACK SetSelect_CB(HWND   win,   UINT   msg,   LPARAM   param,   LPARAM   data)   
{   
	if (msg == BFFM_INITIALIZED)
	{
		::SendMessage(win, BFFM_SETSELECTION, TRUE, (LPARAM)buf);
	}
	return 0;
}

void CWizardDownloadBoard::OnBnClickedButton1()
{
	if (GetConsoleDlg()->IsRunOnServer())
	{
		BROWSEINFO info;
		memset(&info, 0, sizeof(info));
		info.hwndOwner = m_hWnd;
		info.lpszTitle = "设置默认下载目录:";
		GetDlgItemText(IDC_INITDIR, buf, MAX_PATH);
		info.lpfn = SetSelect_CB;

		LPITEMIDLIST lst = SHBrowseForFolder(&info);
		if (lst != NULL)
		{
			SHGetPathFromIDList(lst, buf);
			LPMALLOC pMalloc = NULL;
			SHGetMalloc(&pMalloc);
			pMalloc->Free(lst);
			pMalloc->Release();
			SetDlgItemText(IDC_INITDIR, buf);
		}
	}
	else
	{
		CBrowseDirDlg dlg;
		dlg.m_bBrowseDir = TRUE;
		GetDlgItemText(IDC_INITDIR, dlg.m_strDirOrFile);
		if (dlg.DoModal() != IDOK)
			return;
		SetDlgItemText(IDC_INITDIR, dlg.m_strDirOrFile);
	}

}

void CWizardDownloadBoard::OnDestroy()
{
	m_AreaInfos.clear();
	m_VDiskInfos.clear();

	CDialog::OnDestroy();
}

void CWizardDownloadBoard::OnBnClickedAdd()
{
	CSetUptTimeDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString szStart, szEnd;
		szStart.Format("%02d:%02d", dlg.tmStart.GetHour(), dlg.tmStart.GetMinute());
		szEnd.Format("%02d:%02d", dlg.tmEnd.GetHour(), dlg.tmEnd.GetMinute());
		int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), szStart);
		m_lstCtrl.SetItemText(nIdx, 1, szEnd);

		m_bModify = TRUE;
	}
}

void CWizardDownloadBoard::OnBnClickedDel()
{
	int nSel = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;
	m_lstCtrl.DeleteItem(nSel);
	m_bModify = TRUE;
}

void CWizardDownloadBoard::OnBnClickedCheckTimeupt()
{
	BOOL bEnable = (m_btnTimeUpt.GetCheck() == BST_CHECKED);
	m_lstCtrl.EnableWindow(bEnable);
	GetDlgItem(IDC_ADD)->EnableWindow(bEnable);
	GetDlgItem(IDC_DEL)->EnableWindow(bEnable);
	m_bModify = TRUE;
}

void CWizardDownloadBoard::OnBnClickedSelectMsctmpdir()
{
	if (GetConsoleDlg()->IsRunOnServer())
	{
		BROWSEINFO info;
		memset(&info, 0, sizeof(info));
		info.hwndOwner = m_hWnd;
		info.lpszTitle = "设置下载临时目录:";
		GetDlgItemText(IDC_DOWNTEMPDIR, buf, MAX_PATH);
		info.lpfn = SetSelect_CB;

		LPITEMIDLIST lst = SHBrowseForFolder(&info);
		if (lst != NULL)
		{
			SHGetPathFromIDList(lst, buf);
			LPMALLOC pMalloc = NULL;
			SHGetMalloc(&pMalloc);
			pMalloc->Free(lst);
			pMalloc->Release();
			SetDlgItemText(IDC_DOWNTEMPDIR, buf);
		}
	}
	else
	{
		CBrowseDirDlg dlg;
		dlg.m_bBrowseDir = TRUE;
		GetDlgItemText(IDC_DOWNTEMPDIR, dlg.m_strDirOrFile);
		if (dlg.DoModal() != IDOK)
			return;
		SetDlgItemText(IDC_DOWNTEMPDIR, dlg.m_strDirOrFile);
	}
}

void CWizardDownloadBoard::OnCbnSelchangeJoinType()
{
	m_bModify = TRUE;
}

void CWizardDownloadBoard::OnCbnSelchangeTasknum()
{
	m_bModify = TRUE;
}
