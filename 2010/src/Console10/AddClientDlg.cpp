// AddClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "AddClientDlg.h"
#include "AreaMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CAddClientDlg, CDialog)

CAddClientDlg::CAddClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddClientDlg::IDD, pParent)
{
	m_bAdd = TRUE;
}

CAddClientDlg::~CAddClientDlg()
{
}

void CAddClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPFROM, m_ipFrom);
	DDX_Control(pDX, IDC_IPTO, m_ipTo);
	DDX_Control(pDX, IDC_NETGATE, m_ipNetGate);
	DDX_Control(pDX, IDC_MARK, m_ipMark);
	DDX_Control(pDX, IDC_DNS, m_ipDNS);
	DDX_Control(pDX, IDC_DNS2, m_ipDNS2);
	DDX_Control(pDX, IDC_COMBO_AREA, m_cboArea);
	DDX_Control(pDX, IDC_LIST_CLIGRP, m_lstCliGrp);
	DDX_Control(pDX, IDC_FROM, m_editStartNum);
	DDX_Control(pDX, IDC_MACHINE_NUM, m_editMachineNum);
	DDX_Control(pDX, IDC_ENCODE_SIZE, m_editEncodeSize);
}


BEGIN_MESSAGE_MAP(CAddClientDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddClientDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_TO, &CAddClientDlg::OnEnChangeTo)
	ON_EN_CHANGE(IDC_FROM, &CAddClientDlg::OnEnChangeTo)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPFROM, &CAddClientDlg::OnIpnFieldchangedIpfrom)
	ON_EN_CHANGE(IDC_NAME_PRE, &CAddClientDlg::OnEnChangeNamePre)
	ON_BN_CLICKED(IDC_ADD_CLI, &CAddClientDlg::OnBnClickedAddCli)
	ON_BN_CLICKED(IDC_MOD_CLI, &CAddClientDlg::OnBnClickedModCli)
	ON_BN_CLICKED(IDC_REMOVE_CLI, &CAddClientDlg::OnBnClickedRemoveCli)
//	ON_NOTIFY(HDN_ITEMCLICK, 0, &CAddClientDlg::OnHdnItemclickListCligrp)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_CLIGRP, &CAddClientDlg::OnLvnDeleteitemListCligrp)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_CLIGRP, &CAddClientDlg::OnLvnItemActivateListCligrp)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CLIGRP, &CAddClientDlg::OnNMClickListCligrp)
	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_CLIGRP, &CAddClientDlg::OnNMKillfocusListCligrp)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_INSERTITEM, IDC_LIST_CLIGRP, &CAddClientDlg::OnLvnInsertitemListCligrp)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CLIGRP, &CAddClientDlg::OnLvnItemchangedListCligrp)
END_MESSAGE_MAP()

BOOL CAddClientDlg::InitDefaultNetParam()
{
	IP_ADAPTER_INFO info[16] = {0};
	DWORD dwSize = sizeof(info);
	if (ERROR_SUCCESS != GetAdaptersInfo(info, &dwSize))
		return FALSE; //????

	PIP_ADAPTER_INFO pAdapter = info;
	if (pAdapter) {
		if (PIP_ADDR_STRING pIpAddress = &pAdapter->IpAddressList) {
			m_ipMark.SetAddress(::ntohl(::inet_addr(pIpAddress->IpMask.String)));
		}

		m_ipNetGate.SetAddress(::ntohl(::inet_addr(pAdapter->GatewayList.IpAddress.String)));
		m_ipDNS.SetAddress(::ntohl(::inet_addr(pAdapter->SecondaryWinsServer.IpAddress.String)));
		m_ipDNS2.SetAddress(::ntohl(::inet_addr(pAdapter->PrimaryWinsServer.IpAddress.String)));

		//缺省的客户机起始IP = SUBMASK & GATEWAY
		DWORD dwGatewayIp, dwMarkIp, dwFromIp;
		m_ipNetGate.GetAddress(dwGatewayIp);
		m_ipMark.GetAddress(dwMarkIp);
		dwFromIp = dwGatewayIp & dwMarkIp;
		m_ipFrom.SetAddress(dwFromIp);
	}
/*
	IP_ADAPTER_ADDRESSES addresses[16];
 	dwSize = sizeof(addresses);
	if (ERROR_SUCCESS != GetAdaptersAddresses(AF_INET,0,NULL, addresses, &dwSize))
		return FALSE;

	PIP_ADAPTER_ADDRESSES pAddresses = addresses;
	if (pAddresses) {
		//pAddresses->FirstDnsServerAddress->Address.lpSockaddr;
		char *ip = inet_ntoa(*(in_addr*)pAddresses->FirstDnsServerAddress->Address.lpSockaddr->sa_data);
		m_ipDNS.SetAddress(1);
		m_ipDNS2.SetAddress(2);
	}
//*/
	return TRUE;
}

BOOL CAddClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_bAdd)
	{
		SetDlgItemText(IDC_NAME_PRE,"A-");
		SetDlgItemText(IDC_FROM,	"1");
		SetDlgItemText(IDC_TO,		"1");
		SetDlgItemText(IDC_MACHINE_NUM,		"1");
		SetDlgItemText(IDC_ENCODE_SIZE,		"3");
		m_ipFrom.SetAddress(192, 168, 1, 1);
		m_ipTo.SetAddress(192, 168, 1, 1);
		m_ipNetGate.SetAddress(192, 168, 1, 254);
		m_ipMark.SetAddress(255, 255, 255, 0);
		m_ipDNS.SetAddress(61, 139, 2, 69);
		m_ipDNS2.SetAddress(61, 139, 2, 68);

		InitDefaultNetParam();
	}

	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	i8desk::CDbMgr* pDbMgr = pDlg->m_pDbMgr;
	i8desk::AreaInfoMap AreaInfos;
	std::string ErrInfo;
	if (!pDbMgr->GetAllArea(AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	int nItem = m_cboArea.AddString(DEFAULT_AREA_NAME);
	char *p = new char[lstrlen(DEFAULT_AREA_GUID) + 1];
	lstrcpy(p, DEFAULT_AREA_GUID);
	m_cboArea.SetItemData(nItem, reinterpret_cast<DWORD>(p));

	for (i8desk::AreaInfoMapItr it = AreaInfos.begin();
		it != AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			nItem = m_cboArea.AddString(it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_cboArea.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}
	}
	AreaInfos.clear();

	if (m_bAdd)
	{
		CString szText = pDlg->GetTreeSelNodeText();
		m_cboArea.SelectString(-1, szText);
		if (m_cboArea.GetCurSel() == -1 && m_cboArea.GetCount())
			m_cboArea.SetCurSel(0);
	}

	m_ipTo.EnableWindow(FALSE);

	//初始化客户分组控件
	static const struct {
		LPCTSTR name;
		int width;
	} colums[] = {
		{ _T("名字前缀"), 60 },
		{ _T("名字后缀"), 60 },
		{ _T("所属区域"), 80 },
		{ _T("起始编号"), 60 },
		{ _T("机器总数"), 60 },
		{ _T("编号长度"), 60 },
		{ _T("起始IP地址"), 100 },
		{ _T("子网掩码"), 100 },
		{ _T("网关"), 100 },
		{ _T("首选DNS"), 100 },
		{ _T("备用DNS"), 100 },
	};

	for (int i = 0; i < _countof(colums); i++) {
		m_lstCliGrp.InsertColumn(i, colums[i].name, LVCFMT_CENTER, colums[i].width);
	}
	m_lstCliGrp.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_TRACKSELECT);

	this->GetDlgItem(IDC_MOD_CLI)->EnableWindow( FALSE );
	this->GetDlgItem(IDC_REMOVE_CLI)->EnableWindow( FALSE );
	this->GetDlgItem(IDOK)->EnableWindow( FALSE );

	m_editStartNum.SetLimitText(4);
	m_editMachineNum.SetLimitText(3);
	m_editEncodeSize.SetLimitText(1);

	CenterWindow();
	
	return TRUE;
}

bool CAddClientDlg::CheckValid()
{
	int nFrom = GetDlgItemInt(IDC_FROM);
	int nTo   = GetDlgItemInt(IDC_TO);
	CString Name;
	GetDlgItemText(IDC_NAME_PRE, Name);
	Name.Trim();
	if (Name.IsEmpty())
	{
		AfxMessageBox("没有设置名称前缀");
		((CEdit*)GetDlgItem(IDC_NAME_PRE))->SetFocus();
		return false;
	}
	for (int nIdx=0; nIdx<Name.GetLength(); nIdx++)
	{
		char ch = Name[nIdx];
		if (ch >=0 && ch <= 127)
		{
			if ( !(isalnum(ch) || ch == '%' || ch == '-' || ch == '_') )
			{
				AfxMessageBox("前缀有非法字符");
				GetDlgItem(IDC_NAME_PRE)->SetFocus();
				return false;
			}
		}
	}
	if (nTo < nFrom)
	{
		AfxMessageBox("序号设置错！");
		((CEdit*)GetDlgItem(IDC_TO))->SetFocus();
		return false;
	}
	if (m_cboArea.GetCurSel() == -1)
	{
		AfxMessageBox("请选择区域.");
		return false;
	}
	return true;
}

void CAddClientDlg::OnBnClickedOk()
{
	CString Prefix, Suffix, Area;
	CString StartNum, MachineNum, EncodeSize;
	CString Ip, NetGate, Mark, DNS, DNS2;
	CString NameFormat, MachineName;

	i8desk::tagMachine Machine;
	std::string ErrInfo;
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	BYTE nField0, nField1, nField2, nField3;
	USHORT nField23;

	for (int i = 0, nSubItem = 0; i < m_lstCliGrp.GetItemCount(); i++, nSubItem = 0)
	{
		Prefix		= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		Suffix		= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		Area		= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		StartNum	= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		MachineNum	= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		EncodeSize	= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		Ip			= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		Mark		= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		NetGate		= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		DNS			= this->m_lstCliGrp.GetItemText(i, nSubItem++);
		DNS2		= this->m_lstCliGrp.GetItemText(i, nSubItem++);

		int nStartNum = atoi(StartNum);
		int nMachineNum = atoi(MachineNum);

		int sel = this->m_cboArea.FindString(0, Area);
		ASSERT(sel != -1);
		std::string AID = (char *)m_cboArea.GetItemData(sel);

		//机器名格式
		NameFormat.Format(_T("%s%%0%sd%s"), Prefix, EncodeSize, Suffix);

		//起始IP
		m_ipTo.SetAddress(ntohl(inet_addr(Ip)));
		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		nField23 = ((USHORT)nField2 << 8) + nField3;

		for (int n = nStartNum; n < nStartNum + nMachineNum; n++) 
		{
			//生成机器名
			MachineName.Format(NameFormat, n);
			i8desk::SAFE_STRCPY(Machine.Name, MachineName);
			
			//生成IP地址
			m_ipTo.SetAddress(nField0, nField1, nField23 >> 8, nField23 & 0xff);
			m_ipTo.GetAddress(Machine.IP);
			Machine.IP = ntohl(Machine.IP);
			nField23++;

			i8desk::SAFE_STRCPY(Machine.NetGate	, NetGate);
			i8desk::SAFE_STRCPY(Machine.Mark	, Mark);
			i8desk::SAFE_STRCPY(Machine.DNS		, DNS);
			i8desk::SAFE_STRCPY(Machine.DNS2	, DNS2);
			i8desk::SAFE_STRCPY(Machine.AID		, AID.c_str());

			//发送命令
			if (!pDlg->m_pDbMgr->AddMachine(Machine, ErrInfo)) 
			{
				CString msg;
				msg.Format(_T("添加客户机[name=%s]时:%s"), MachineName, ErrInfo.c_str());
				AfxMessageBox(msg);
				return;
			}
		}
	}

	OnOK();
}

void CAddClientDlg::OnDestroy()
{
	for (int idx=0; idx<m_cboArea.GetCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_cboArea.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

void CAddClientDlg::OnEnChangeTo()
{
	int nFrom = GetDlgItemInt(IDC_FROM);
	int nTo = GetDlgItemInt(IDC_TO);
	if (nTo < nFrom)
		return ;

	BYTE a, b, c, d;
	m_ipFrom.GetAddress(a, b, c, d);
	c += (d + nTo - nFrom) / 255;
	d  = (d + nTo - nFrom) % 255;
	m_ipTo.SetAddress(a, b, c, d);
}

void CAddClientDlg::OnIpnFieldchangedIpfrom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	*pResult = 0;
	OnEnChangeTo();
}

void CAddClientDlg::OnEnChangeNamePre()
{
	
}

void CAddClientDlg::OnBnClickedAddCli()
{
	//if (!CheckValid())
	//	return ;

	CString Prefix, Suffix, Area;
	
	GetDlgItemText(IDC_NAME_PRE, Prefix);
	GetDlgItemText(IDC_NAME_SUFFIX, Suffix);
	m_cboArea.GetWindowText(Area);

	CString StartNum, MachineNum, EncodeSize;

	GetDlgItemText(IDC_FROM, StartNum);
	GetDlgItemText(IDC_MACHINE_NUM, MachineNum);
	GetDlgItemText(IDC_ENCODE_SIZE, EncodeSize);

	in_addr addr;
	CString Ip, NetGate, Mark, DNS, DNS2;

	m_ipNetGate.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	NetGate = inet_ntoa(addr);

	m_ipMark.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	Mark = inet_ntoa(addr);

	m_ipDNS.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	DNS = inet_ntoa(addr);

	m_ipDNS2.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	DNS2 = inet_ntoa(addr);

	m_ipFrom.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	Ip = inet_ntoa(addr);

	//当前设置的命名范围
	CString currNameFormat, currFirstName, currLastName;
	currNameFormat.Format(_T("%s%%0%sd%s"), Prefix, EncodeSize, Suffix);
	currFirstName.Format(currNameFormat, atoi(StartNum));
	currLastName.Format(currNameFormat, atoi(StartNum) + atoi(MachineNum) - 1);

	//当前的IP地址范围
	CString currFirstIp, currLastIp;
	BYTE nField0, nField1, nField2, nField3;

	m_ipFrom.GetAddress(nField0, nField1, nField2, nField3);
	currFirstIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

	USHORT nField23 = ((USHORT)nField2 << 8) +  nField3;
	nField23 += atoi(MachineNum) - 1;
	m_ipTo.SetAddress(nField0, nField1, nField23 >> 8, nField23 & 0xff);

	m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
	currLastIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

	//是否与列表中的客户机冲突
	for (int i = 0; i < m_lstCliGrp.GetItemCount(); i++) {
		//检查机器名
		CString NameFormat, firstName, lastName;

		NameFormat.Format(_T("%s%%0%sd%s"), m_lstCliGrp.GetItemText(i, 0),
			m_lstCliGrp.GetItemText(i, 5), m_lstCliGrp.GetItemText(i, 1));

		firstName.Format(NameFormat, atoi(m_lstCliGrp.GetItemText(i, 3)));
		lastName.Format(NameFormat, atoi(m_lstCliGrp.GetItemText(i, 3))	
			+ atoi(m_lstCliGrp.GetItemText(i, 4)) - 1);

		if ((currFirstName >= firstName && currFirstName <= lastName)
			|| (currLastName >= firstName && currLastName <= lastName)) 
		{
			AfxMessageBox(
				_T("设置的客户机名字与分组列表中存在重复\n\n")
				_T("请检查命名规范与编号设置后再试!\n")
				);
			return;
		}

		//检查IP地址
		CString firstIp, lastIp;
		m_ipTo.SetAddress(ntohl(inet_addr(m_lstCliGrp.GetItemText(i, 6))));
		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		firstIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

		nField23 = ((USHORT)nField2 << 8) +  nField3;
		nField23 += atoi(m_lstCliGrp.GetItemText(i, 4)) - 1;
		m_ipTo.SetAddress(nField0, nField1, nField23 >> 8, nField23 & 0xff);

		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		lastIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

		if ((currFirstIp >= firstIp && currFirstIp <= lastIp)
			|| (currLastIp >= firstIp && currLastIp <= lastIp)) 
		{
			AfxMessageBox(
				_T("设置的客户机IP地址与分组列表中存在重复\n\n")
				_T("请检查起始IP地址与机器数量设置后再试!\n")
				);
			return;
		}
	}

	//是否与已有的客户机冲突
	{
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	extern i8desk::CLock g_Lock;
	i8desk::CAutoLock<i8desk::CLock> lock(&g_Lock);

	i8desk::MachineMapItr it = pDlg->m_Machines.begin();
	for (; it != pDlg->m_Machines.end(); ++it) {
		if (currFirstName <= it->second->Name && currLastName >= it->second->Name) {
			AfxMessageBox(
				_T("设置的客户机名字与已有的客户机存在重复\n\n")
				_T("请检查命名规范与编号设置后再试!\n")
				);
			return;
		}
		m_ipTo.SetAddress(ntohl(it->second->IP));
		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		CString CliIP;
		CliIP.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);
		if (CliIP >= currFirstIp && CliIP <= currLastIp) {
			AfxMessageBox(
				_T("设置的客户机IP地址与已有的客户机存在重复\n\n")
				_T("请检查起始IP地址与机器数量设置后再试!\n")
				);
			return;
		}
	}
	}

	//检查完成,添加到分组列表中准备添加
	int nItem = m_lstCliGrp.GetItemCount();
	this->m_lstCliGrp.InsertItem(nItem, Prefix);

	int nSubItem = 1;
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, Suffix);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, Area);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, StartNum);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, MachineNum);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, EncodeSize);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, Ip);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, Mark);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, NetGate);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, DNS);
	this->m_lstCliGrp.SetItemText(nItem, nSubItem++, DNS2);
}

void CAddClientDlg::OnBnClickedModCli()
{
	//if (!CheckValid())
	//	return ;

	CString Prefix, Suffix, Area;
	
	GetDlgItemText(IDC_NAME_PRE, Prefix);
	GetDlgItemText(IDC_NAME_SUFFIX, Suffix);
	m_cboArea.GetWindowText(Area);

	CString StartNum, MachineNum, EncodeSize;

	GetDlgItemText(IDC_FROM, StartNum);
	GetDlgItemText(IDC_MACHINE_NUM, MachineNum);
	GetDlgItemText(IDC_ENCODE_SIZE, EncodeSize);

	in_addr addr;
	CString Ip, NetGate, Mark, DNS, DNS2;

	m_ipNetGate.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	NetGate = inet_ntoa(addr);

	m_ipMark.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	Mark = inet_ntoa(addr);

	m_ipDNS.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	DNS = inet_ntoa(addr);

	m_ipDNS2.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	DNS2 = inet_ntoa(addr);

	m_ipFrom.GetAddress(addr.s_addr);
	addr.s_addr = ntohl(addr.s_addr);
	Ip = inet_ntoa(addr);

	//当前设置的命名范围
	CString currNameFormat, currFirstName, currLastName;
	currNameFormat.Format(_T("%s%%0%sd%s"), Prefix, EncodeSize, Suffix);
	currFirstName.Format(currNameFormat, atoi(StartNum));
	currLastName.Format(currNameFormat, atoi(StartNum) + atoi(MachineNum) - 1);

	//当前的IP地址范围
	CString currFirstIp, currLastIp;
	BYTE nField0, nField1, nField2, nField3;

	m_ipFrom.GetAddress(nField0, nField1, nField2, nField3);
	currFirstIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

	USHORT nField23 = ((USHORT)nField2 << 8) +  nField3;
	nField23 += atoi(MachineNum) - 1;
	m_ipTo.SetAddress(nField0, nField1, nField23 >> 8, nField23 & 0xff);

	m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
	currLastIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

	int SelItemIndex = -1;

	//是否与分组列表中的客户机设置冲突
	for (int i = 0; i < m_lstCliGrp.GetItemCount(); i++) {
		//
		if (m_lstCliGrp.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED) {
			SelItemIndex = i;
			continue;
		}

		//检查机器名
		CString NameFormat, firstName, lastName;

		NameFormat.Format(_T("%s%%0%sd%s"), m_lstCliGrp.GetItemText(i, 0),
			m_lstCliGrp.GetItemText(i, 5), m_lstCliGrp.GetItemText(i, 1));

		firstName.Format(NameFormat, atoi(m_lstCliGrp.GetItemText(i, 3)));
		lastName.Format(NameFormat, atoi(m_lstCliGrp.GetItemText(i, 3))	
			+ atoi(m_lstCliGrp.GetItemText(i, 4)) - 1);

		if ((currFirstName >= firstName && currFirstName <= lastName)
			|| (currLastName >= firstName && currLastName <= lastName)) 
		{
			AfxMessageBox(
				_T("设置的客户机名字与分组列表中存在重复\n\n")
				_T("请检查命名规范与编号设置后再试!\n")
				);
			return;
		}

		//检查IP地址
		CString firstIp, lastIp;
		m_ipTo.SetAddress(ntohl(inet_addr(m_lstCliGrp.GetItemText(i, 6))));
		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		firstIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

		nField23 = ((USHORT)nField2 << 8) +  nField3;
		nField23 += atoi(m_lstCliGrp.GetItemText(i, 4)) - 1;
		m_ipTo.SetAddress(nField0, nField1, nField23 >> 8, nField23 & 0xff);

		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		lastIp.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);

		if ((currFirstIp >= firstIp && currFirstIp <= lastIp)
			|| (currLastIp >= firstIp && currLastIp <= lastIp)) 
		{
			AfxMessageBox(
				_T("设置的客户机IP地址与分组列表中存在重复\n\n")
				_T("请检查起始IP地址与机器数量设置后再试!\n")
				);
			return;
		}
	}

	//是否与已有的客户机冲突
	{
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	extern i8desk::CLock g_Lock;
	i8desk::CAutoLock<i8desk::CLock> lock(&g_Lock);

	i8desk::MachineMapItr it = pDlg->m_Machines.begin();
	for (; it != pDlg->m_Machines.end(); ++it) {
		if (currFirstName <= it->second->Name && currLastName >= it->second->Name) {
			AfxMessageBox(
				_T("设置的客户机名字与已有的客户机存在重复\n\n")
				_T("请检查命名规范与编号设置后再试!\n")
				);
			return;
		}
		m_ipTo.SetAddress(ntohl(it->second->IP));
		m_ipTo.GetAddress(nField0, nField1, nField2, nField3);
		CString CliIP;
		CliIP.Format(_T("%03u.%03u.%03u.%03u"), nField0, nField1, nField2, nField3);
		if (CliIP >= currFirstIp && CliIP <= currLastIp) {
			AfxMessageBox(
				_T("设置的客户机IP地址与已有的客户机存在重复\n\n")
				_T("请检查起始IP地址与机器数量设置后再试!\n")
				);
			return;
		}
	}
	}

	ASSERT(SelItemIndex != -1);

	int nSubItem = 0;
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, Prefix);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, Suffix);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, Area);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, StartNum);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, MachineNum);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, EncodeSize);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, Ip);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, Mark);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, NetGate);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, DNS);
	this->m_lstCliGrp.SetItemText(SelItemIndex, nSubItem++, DNS2);
}

void CAddClientDlg::OnBnClickedRemoveCli()
{
	if (POSITION pos = m_lstCliGrp.GetFirstSelectedItemPosition()) {
		m_lstCliGrp.DeleteItem( m_lstCliGrp.GetNextSelectedItem(pos) );
	}
}

//void CAddClientDlg::OnHdnItemclickListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//
//	bool b = m_lstCliGrp.GetFirstSelectedItemPosition() != NULL;
//	this->GetDlgItem(IDC_MOD_CLI)->EnableWindow(b);
//	this->GetDlgItem(IDC_REMOVE_CLI)->EnableWindow(b);
//}

void CAddClientDlg::OnLvnDeleteitemListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	bool b = m_lstCliGrp.GetFirstSelectedItemPosition() != NULL;
	this->GetDlgItem(IDC_MOD_CLI)->EnableWindow(b);
	this->GetDlgItem(IDC_REMOVE_CLI)->EnableWindow(b);
	this->GetDlgItem(IDOK)->EnableWindow(m_lstCliGrp.GetItemCount() > 1);

	*pResult = 0;
}

void CAddClientDlg::OnLvnItemActivateListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	bool b = m_lstCliGrp.GetFirstSelectedItemPosition() != NULL;
	this->GetDlgItem(IDC_MOD_CLI)->EnableWindow(b);
	this->GetDlgItem(IDC_REMOVE_CLI)->EnableWindow(b);

	*pResult = 0;
}


void CAddClientDlg::OnNMClickListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_lstCliGrp.GetFirstSelectedItemPosition();
	bool b = pos != NULL;
	this->GetDlgItem(IDC_MOD_CLI)->EnableWindow(b);
	this->GetDlgItem(IDC_REMOVE_CLI)->EnableWindow(b);

	if (pos) {
		UpdateDataFromSelectedItem(m_lstCliGrp.GetNextSelectedItem(pos));
	}

	*pResult = 0;
}

void CAddClientDlg::UpdateDataFromSelectedItem(int SelItemIndex)
{
	ASSERT(SelItemIndex >= 0 && SelItemIndex <= m_lstCliGrp.GetItemCount());

	int nSubItem = 0;
	CString Prefix		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString Suffix		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString Area		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString StartNum	= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString MachineNum	= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString EncodeSize	= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString Ip			= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString Mark		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString NetGate		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString DNS			= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);
	CString DNS2		= this->m_lstCliGrp.GetItemText(SelItemIndex, nSubItem++);

	this->GetDlgItem(IDC_NAME_PRE)->SetWindowText(Prefix);
	this->GetDlgItem(IDC_NAME_SUFFIX)->SetWindowText(Suffix);

	int sel = m_cboArea.FindString(0, Area);
	ASSERT(sel != -1);
	this->m_cboArea.SetCurSel(sel);

	this->GetDlgItem(IDC_FROM)->SetWindowText(StartNum);
	this->GetDlgItem(IDC_MACHINE_NUM)->SetWindowText(MachineNum);
	this->GetDlgItem(IDC_ENCODE_SIZE)->SetWindowText(EncodeSize);

	this->m_ipFrom.SetAddress(ntohl(inet_addr(Ip)));
	this->m_ipMark.SetAddress(ntohl(inet_addr(Mark)));
	this->m_ipNetGate.SetAddress(ntohl(inet_addr(NetGate)));
	this->m_ipDNS.SetAddress(ntohl(inet_addr(DNS)));
	this->m_ipDNS2.SetAddress(ntohl(inet_addr(DNS2)));
}
void CAddClientDlg::OnNMKillfocusListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CAddClientDlg::OnClose()
{
	if (m_lstCliGrp.GetItemCount() > 0
		&& AfxMessageBox(_T("设置的客户机尚未生效,确认退出吗?"), MB_YESNO) == IDNO)
	{
		return;
	}

	CDialog::OnClose();
}

void CAddClientDlg::OnLvnInsertitemListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	this->GetDlgItem(IDOK)->EnableWindow(TRUE);

	*pResult = 0;
}

//void CAddClientDlg::OnLvnItemchangedListCligrp(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//
//	this->GetDlgItem(IDOK)->EnableWindow(m_lstCliGrp.GetItemCount() > 0);
//	
//	*pResult = 0;
//}

BOOL CAddClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetDlgItem(IDC_NAME_PRE)->GetSafeHwnd()
		|| pMsg->hwnd == GetDlgItem(IDC_NAME_SUFFIX)->GetSafeHwnd())   
	{     
		if (pMsg->message == WM_CHAR) {   
			switch (pMsg->wParam) {   
			case '%':   
			case '\\':   
				return   TRUE;           
			}   
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
