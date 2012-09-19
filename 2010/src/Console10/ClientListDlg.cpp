// ClientListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ClientListDlg.h"
#include "ConsoleDlg.h"

// CClientListDlg 对话框

IMPLEMENT_DYNAMIC(CClientListDlg, CDialog)

CClientListDlg::CClientListDlg(
		i8desk::AreaInfoMap& AreaInfos,
		i8desk::MachineMap& Machines,
		int selector, 							  
	   CWnd* pParent /*=NULL*/)
	: CDialog(CClientListDlg::IDD, pParent)
	, m_selector(selector)
	, m_AreaInfos(AreaInfos)
{
	if (m_selector == ONLINE) 
	{
		i8desk::MachineMapItr it = Machines.begin();
		for (; it != Machines.end(); ++it)
		{
			if (it->second->online) 
			{
				it->second->selected = true;
				m_Machines.push_back(it->second);
			}
		}
	}
	else if (m_selector == NORESTOREPROT) 
	{
		i8desk::MachineMapItr it = Machines.begin();
		for (; it != Machines.end(); ++it)
		{
			if (it->second->online && !it->second->ProtInstall)
			{
				it->second->selected = true;
				m_Machines.push_back(it->second);
			}
		}
	}
	else if (m_selector == NOSAFECENTER) 
	{
		i8desk::MachineMapItr it = Machines.begin();
		for (; it != Machines.end(); ++it)
		{
			if (it->second->online && !it->second->SCStatus)
			{
				it->second->selected = true;
				m_Machines.push_back(it->second);
			}
		}
	}
	else if (m_selector == NOIEPROT) 
	{
		i8desk::MachineMapItr it = Machines.begin();
		for (; it != Machines.end(); ++it)
		{
			if (it->second->online && !it->second->IEProt)
			{
				it->second->selected = true;
				m_Machines.push_back(it->second);
			}
		}
	}
	else if (m_selector == NODOGPROT) 
	{
		i8desk::MachineMapItr it = Machines.begin();
		for (; it != Machines.end(); ++it)
		{
			if (it->second->online && !it->second->FDogDriver)
			{
				it->second->selected = true;
				m_Machines.push_back(it->second);
			}
		}
	}
}

CClientListDlg::~CClientListDlg()
{
}

void CClientListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CClientListDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CClientListDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, &CClientListDlg::OnLvnGetdispinfoList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CClientListDlg::OnNMClickList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, &CClientListDlg::OnLvnKeydownList)
	ON_BN_CLICKED(IDC_ALL, &CClientListDlg::OnBnClickedAll)
	ON_BN_CLICKED(IDC_CLEAR, &CClientListDlg::OnBnClickedClear)
END_MESSAGE_MAP()



BOOL CClientListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	switch (m_selector)
	{
	case ONLINE:
		SetWindowText(_T("在线客户机列表"));
		GetDlgItem(IDOK)->SetWindowText(_T("确定"));
		GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CLEAR)->ShowWindow(SW_HIDE);
		break;
	case NORESTOREPROT:
		SetWindowText(_T("未安装还原保护的客户机列表"));
		GetDlgItem(IDOK)->SetWindowText(_T("启用还原保护"));
		break;
	case NOSAFECENTER:
		SetWindowText(_T("未启动安全中心的客户机列表"));
		GetDlgItem(IDOK)->SetWindowText(_T("启用安全中心"));
		break;
	case NOIEPROT:
		SetWindowText(_T("未启动IE保护的客户机列表"));
		GetDlgItem(IDOK)->SetWindowText(_T("启用IE保护"));
		break;
	case NODOGPROT:
		SetWindowText(_T("未启动机器狗保护的客户机列表"));
		GetDlgItem(IDOK)->SetWindowText(_T("启用防狗驱动"));
		break;
	default:
		ASSERT(0);
		break;
	}

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	m_list.InsertColumn(MACHINE_NAME,	TEXT("机器名称"),		LVCFMT_LEFT, 80);
	m_list.InsertColumn(MACHINE_STATUS,	TEXT("状态"),			LVCFMT_LEFT, 40);
	m_list.InsertColumn(MACHINE_AREA,	TEXT("区域"),			LVCFMT_LEFT, 60);
	m_list.InsertColumn(MACHINE_IP,		TEXT("IP"),				LVCFMT_LEFT, 110);
	m_list.InsertColumn(MACHINE_MAC,	TEXT("MAC"),			LVCFMT_LEFT, 110);
	m_list.InsertColumn(MACHINE_MARK,	TEXT("子网掩码"),		LVCFMT_LEFT, 120);
	m_list.InsertColumn(MACHINE_NETGATE,TEXT("网关"),			LVCFMT_LEFT, 110);
	m_list.InsertColumn(MACHINE_DNS,	TEXT("DNS"),			LVCFMT_LEFT, 110);
	m_list.InsertColumn(MACHINE_PROTINSTALL,TEXT("还原状态"),		LVCFMT_LEFT, 70);
	m_list.InsertColumn(MACHINE_ALLAREA,	TEXT("所有分区"),		LVCFMT_LEFT, 70);
	m_list.InsertColumn(MACHINE_PROTAREA,	TEXT("保护分区"),		LVCFMT_LEFT, 70);
	m_list.InsertColumn(MACHINE_PROTVER,	TEXT("还原版本"),		LVCFMT_RIGHT,110);
	m_list.InsertColumn(MACHINE_MENUVER,	TEXT("菜单版本"),		LVCFMT_LEFT, 80);
	m_list.InsertColumn(MACHINE_CLISVRVER,	TEXT("服务版本"),		LVCFMT_LEFT, 80);

	m_list.SetItemCount(m_Machines.size());

	GetDlgItem(IDOK)->EnableWindow(m_Machines.size() != 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// CClientListDlg 消息处理程序

size_t CClientListDlg::GetSelectedClients(std::vector<DWORD>& clients)
{
	for (size_t i = 0; i < m_Machines.size(); i++)
	{
		i8desk::Machine_Ptr pMachine = m_Machines[i];
		if (pMachine->selected) 
		{
			clients.push_back(pMachine->IP);
		}
	}

	return clients.size();
}

void CClientListDlg::OnBnClickedOk()
{
	std::vector<DWORD> clients;
	if (!GetSelectedClients(clients))
		return;

	CConsoleDlg *pMainWnd = static_cast<CConsoleDlg *>(AfxGetMainWnd());
	i8desk::CCliMgr *pCliMgr = pMainWnd->m_pCliMgr;
	std::string ErrInfo;

	bool ok = true;

	switch (m_selector)
	{
	case ONLINE:
		break;
	case NORESTOREPROT:
		ok = pCliMgr->InstallProt(clients, true, ErrInfo);
		break;
	case NOSAFECENTER:
		ok = pCliMgr->EnableSafeCenter(clients, ErrInfo);
		break;
	case NOIEPROT:
		ok = pCliMgr->EnableIEProt(clients, ErrInfo);
		break;
	case NODOGPROT:
		ok = pCliMgr->EnableDogProt(clients, ErrInfo);
		break;
	default:
		break;
	}

	if (!ok)
	{
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	OnOK();
}

CString CClientListDlg::GetMachineArea(const i8desk::Machine& Machine)
{
	i8desk::AreaInfoMapItr it = m_AreaInfos.find(Machine.AID);
	if (it != m_AreaInfos.end())
		return CString(it->second->Name);
	return CString("");
}

void CClientListDlg::OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* pItem= &(pDispInfo)->item;
	int itemid = pItem->iItem;

	if (itemid >= (int)m_Machines.size())
		return ;

	i8desk::Machine_Ptr pMachine = m_Machines[itemid];
	ASSERT(pMachine);

	if (pItem->mask & LVIF_TEXT)
	{
		CString szText;
		switch(pItem->iSubItem)
		{
		case MACHINE_NAME:	szText = pMachine->Name;					break;
		case MACHINE_STATUS: szText = pMachine->online ? _T("在线"):_T("离线");	break;
		case MACHINE_AREA:	szText = GetMachineArea(*pMachine);					break;
		case MACHINE_IP: szText = i8desk::MakeIpString(pMachine->IP).c_str();	break;				
		case MACHINE_MAC: szText = pMachine->MAC;						break;
		case MACHINE_MARK: szText = pMachine->Mark;						break;
		case MACHINE_NETGATE: szText = pMachine->NetGate;				break;
		case MACHINE_DNS: 
			{
				if (!STRING_ISEMPTY(pMachine->DNS2))
				{
					szText = (std::string() + pMachine->DNS + ":" + pMachine->DNS2).c_str();
				}
				else
				{
					szText = pMachine->DNS;
				}
			}
			break;
		case MACHINE_PROTINSTALL: szText = pMachine->ProtInstall ? "安装":"未安装";	break;
		case MACHINE_ALLAREA: szText = pMachine->AllArea;					break;
		case MACHINE_PROTAREA: szText = pMachine->ProtArea;					break;
		case MACHINE_PROTVER: szText = pMachine->ProtVer;					break;
		case MACHINE_MENUVER: szText = pMachine->MenuVer;					break;
		case MACHINE_CLISVRVER: szText = pMachine->CliSvrVer;				break;
		}
		lstrcpyn(pItem->pszText, szText, pItem->cchTextMax);
	}

	pItem->mask |= LVIF_STATE;
	pItem->stateMask = LVIS_STATEIMAGEMASK;

	pItem->state = INDEXTOSTATEIMAGEMASK(pMachine->selected ? 2 : 1);
}

void CClientListDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
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
			i8desk::Machine_Ptr pMachine = m_Machines[hitinfo.iItem];
			pMachine->selected = !pMachine->selected;
			m_list.RedrawItems(hitinfo.iItem, hitinfo.iItem);
        }
    }
    
	std::vector<DWORD> clients;
	GetDlgItem(IDOK)->EnableWindow(GetSelectedClients(clients));

	*pResult = 0;
}

void CClientListDlg::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    if( pLVKeyDown->wVKey == VK_SPACE )
    {
	    int item = m_list.GetSelectionMark();
        if(item != -1)
		{
			i8desk::Machine_Ptr pMachine = m_Machines[item];
			pMachine->selected = !pMachine->selected;
			m_list.RedrawItems(item, item);
		}
    }

	std::vector<DWORD> clients;
	GetDlgItem(IDOK)->EnableWindow(GetSelectedClients(clients));

    *pResult = 0;
}

void CClientListDlg::OnBnClickedAll()
{
	for (size_t i = 0; i < m_Machines.size(); i++)
	{
		i8desk::Machine_Ptr pMachine = m_Machines[i];
		pMachine->selected = true;
	}
	m_list.RedrawWindow();
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CClientListDlg::OnBnClickedClear()
{
	for (size_t i = 0; i < m_Machines.size(); i++)
	{
		i8desk::Machine_Ptr pMachine = m_Machines[i];
		pMachine->selected = false;
	}
	m_list.RedrawWindow();
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}
