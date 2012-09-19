// VDiskMgr.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "VDiskMgr.h"
#include "WaitMessageDlg.h"

IMPLEMENT_DYNAMIC(CVDiskMgr, CDialog)

CVDiskMgr::CVDiskMgr(CWnd* pParent /*=NULL*/)
	: CDialog(CVDiskMgr::IDD, pParent)
	, m_bChanged(false)
	, m_msg(_T(""))
{

}

CVDiskMgr::~CVDiskMgr()
{
}

void CVDiskMgr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VDISK, m_lstVDisk);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cboVdType);
	DDX_Control(pDX, IDC_SVR_DRV, m_cboSvrDrv);
	DDX_Control(pDX, IDC_CLI_DRV, m_cboCliDrv);
	DDX_Control(pDX, IDC_IPADDRESS, m_IP);
	DDX_Control(pDX, IDC_LOADTYPE, m_cboLoadType);
	DDX_Text(pDX, IDC_EDIT_MSG, m_msg);
	DDX_Control(pDX, IDC_COMBO_VDISK_MODE, m_cbVDiskMode);
}

BEGIN_MESSAGE_MAP(CVDiskMgr, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD, &CVDiskMgr::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CVDiskMgr::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CVDiskMgr::OnBnClickedDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VDISK, &CVDiskMgr::OnLvnItemchangedListVdisk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CVDiskMgr::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_CHECK, &CVDiskMgr::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_VDISK_REFLESH, &CVDiskMgr::OnBnClickedReflesh)
	ON_BN_CLICKED(IDC_VDISK_REFLESH_ALL, &CVDiskMgr::OnBnClickedVdiskRefleshAll)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_LIST_VDISK, &CVDiskMgr::OnNMClickListVdisk)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_VDISK, &CVDiskMgr::OnLvnDeleteitemListVdisk)
END_MESSAGE_MAP()

void CVDiskMgr::ShowVDiskToList(int nIdx, const i8desk::VDiskInfo& vdisk)
{
	int nItem = nIdx;
	if (nIdx == -1)
	{
		nItem = m_lstVDisk.InsertItem(m_lstVDisk.GetItemCount(), 
			vdisk.Type == 0 ? VDISK_I8DESK_TEXT : VDISK_OTHER_TEXT);
	}
	else
	{
		//m_lstVDisk.SetItemText(nItem, 0, vdisk.Type == 0 ? VDISK_I8DESK_TEXT : VDISK_OTHER_TEXT);
	}

	if (nIdx == -1)		//添加的需要设置Item的数据(VID)。
	{
		char *p = new char[strlen(vdisk.VID) + 1];
		lstrcpy(p, vdisk.VID);
		m_lstVDisk.SetItemData(nItem, reinterpret_cast<DWORD>(p));
	}

	CString szItem;
	if (vdisk.Type == 0)
	{
		m_lstVDisk.SetItemText(nItem, 1, i8desk::MakeIpString(vdisk.IP).c_str());
		szItem.Format("%C", (char)vdisk.SvrDrv);
		m_lstVDisk.SetItemText(nItem, 2, szItem);
		m_lstVDisk.SetItemText(nItem, 4, vdisk.LoadType == vdBoot ? RT_COMP_BOOT : 
			((vdisk.LoadType == vdStartMenu) ? RT_MENU_RUN : RT_GAME_RUN));
	}
	else
	{
		m_lstVDisk.SetItemText(nItem, 1, "--");
		m_lstVDisk.SetItemText(nItem, 2, "--");
		m_lstVDisk.SetItemText(nItem, 4, "--");
	}

	szItem.Format("%C", (char)vdisk.CliDrv);
	m_lstVDisk.SetItemText(nItem, 3, szItem);
	if (nIdx != -1)
		m_lstVDisk.SetItemText(nItem, 0, vdisk.Type == 0 ? VDISK_I8DESK_TEXT : VDISK_OTHER_TEXT);

	if (strcmp(MAIN_SERVER_SID, vdisk.SID) == 0)
		m_lstVDisk.SetItemText(nItem, 5, VDISK_SERVER_MODE_GROUP);
	else 
		m_lstVDisk.SetItemText(nItem, 5, VDISK_SERVER_MODE_09);
}

BOOL CVDiskMgr::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cbVDiskMode.AddString(VDISK_SERVER_MODE_09);
	m_cbVDiskMode.AddString(VDISK_SERVER_MODE_GROUP);
	m_cbVDiskMode.SetCurSel(0);

	m_lstVDisk.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_cboVdType.AddString(VDISK_I8DESK_TEXT);
	m_cboVdType.AddString(VDISK_OTHER_TEXT);
	m_cboVdType.SetCurSel(0);

	for (char chDrv='C'; chDrv<='Z'; chDrv++)
	{
		CString Text;
		Text += chDrv;
		m_cboSvrDrv.AddString(Text);
		m_cboCliDrv.AddString(Text);		
	}
	m_cboSvrDrv.SetCurSel(0);
	m_cboCliDrv.SetCurSel(0);

	m_cboLoadType.AddString(RT_COMP_BOOT);
	m_cboLoadType.AddString(RT_MENU_RUN);
	m_cboLoadType.AddString(RT_GAME_RUN);
	m_cboLoadType.SetCurSel(0);

	m_lstVDisk.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstVDisk.InsertColumn(0, "类型",		 LVCFMT_LEFT, 80);
	m_lstVDisk.InsertColumn(1, "服务器IP",	 LVCFMT_LEFT, 95);
	m_lstVDisk.InsertColumn(2, "服务器盘符", LVCFMT_LEFT, 73);
	m_lstVDisk.InsertColumn(3, "客户机盘符", LVCFMT_LEFT, 73);
	m_lstVDisk.InsertColumn(4, "刷盘时机",	 LVCFMT_LEFT, 66);
	m_lstVDisk.InsertColumn(5, "服务器模式", LVCFMT_LEFT, 86);

	pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	i8desk::VDiskInfoMap VDiskInfos;
	std::string ErrInfo;
	if (!pDlg->m_pDbMgr->GetAllVDisk(VDiskInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	else
	{
		for (i8desk::VDiskInfoMapItr it = VDiskInfos.begin();
			it != VDiskInfos.end(); it ++)
		{
			ShowVDiskToList(-1, *it->second);
		}
		VDiskInfos.clear();
	}

	if (m_lstVDisk.GetItemCount())
		m_lstVDisk.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	
	AppendMsg(_T("兼容09模式："));
	AppendMsg(_T("兼容09模式的虚拟盘，客户端"));
	AppendMsg(_T("将按配置的IP地址进行刷盘。"));
	AppendMsg(_T(""));
	AppendMsg(_T("群组模式："));
	AppendMsg(_T("群组模式的虚拟盘，客户端将"));
	AppendMsg(_T("根据所连接到的镜像服务器所"));
	AppendMsg(_T("配置的虚拟盘IP进行刷盘"));

	CenterWindow();

	return TRUE;
}

void CVDiskMgr::OnBnClickedAdd()
{
 	i8desk::tagVDiskInfo VDisk;
	strcpy(VDisk.VID, i8desk::CreateGUIDString().c_str());
	VDisk.Type = m_cboVdType.GetCurSel();
	m_IP.GetAddress(VDisk.IP);
	VDisk.IP = htonl(VDisk.IP);
	VDisk.SvrDrv = m_cboSvrDrv.GetCurSel() + 'C';
	VDisk.Port = 17918 + VDisk.SvrDrv - 'A';
	VDisk.CliDrv = m_cboCliDrv.GetCurSel() + 'C';
	VDisk.LoadType = (VDStartType)m_cboLoadType.GetCurSel();

	if (VDisk.Type == 0 && (VDisk.IP == 0 || VDisk.IP == -1) )
	{
		AfxMessageBox("ip地址不正确!");
		m_IP.SetFocus();
		return ;
	}

	for (int idx=0; idx<m_lstVDisk.GetItemCount(); idx++)
	{
		if (VDisk.Type == 0 && 
			inet_addr(m_lstVDisk.GetItemText(idx, 1)) == VDisk.IP &&
			m_lstVDisk.GetItemText(idx, 2).Left(1) == (char)VDisk.SvrDrv)
		{
			AfxMessageBox("己经共享了服务器的该盘符.");
			m_cboSvrDrv.SetFocus();
			return ;
		}
		if (m_lstVDisk.GetItemText(idx, 3).Left(1) == (char)VDisk.CliDrv)
		{
			AfxMessageBox("己经使用了客户机的盘符.");
			m_cboCliDrv.SetFocus();
			return ;
		}
	}

	if (m_cbVDiskMode.GetCurSel() == 1)
		i8desk::SAFE_STRCPY(VDisk.SID, MAIN_SERVER_SID);
	else
		i8desk::SAFE_STRCPY(VDisk.SID, "");

	std::string ErrInfo;
	if (pDlg->m_pDbMgr->AddVDisk(VDisk, ErrInfo))
	{
		ShowVDiskToList(-1, VDisk);
		m_bChanged = true;
	}
	else
	{
		AfxMessageBox(ErrInfo.c_str());
	}
}

void CVDiskMgr::OnBnClickedModify()
{
	int nSel = m_lstVDisk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	i8desk::tagVDiskInfo VDisk;
	strcpy(VDisk.VID, reinterpret_cast<const char *>(m_lstVDisk.GetItemData(nSel)));
	VDisk.Type = m_cboVdType.GetCurSel();
	m_IP.GetAddress(VDisk.IP);
	VDisk.IP = htonl(VDisk.IP);
	VDisk.SvrDrv = m_cboSvrDrv.GetCurSel() + 'C';
	VDisk.Port = 17918 + VDisk.SvrDrv - 'A';
	VDisk.CliDrv = m_cboCliDrv.GetCurSel() + 'C';
	VDisk.LoadType = (VDStartType)m_cboLoadType.GetCurSel();

	if (VDisk.Type == 0 && (VDisk.IP == 0 || VDisk.IP == -1) )
	{
		AfxMessageBox("ip地址不正确!");
		return ;
	}

	for (int idx=0; idx<m_lstVDisk.GetItemCount(); idx++)
	{
		if (nSel == idx) continue;

		if (VDisk.Type == 0 && 
			inet_addr(m_lstVDisk.GetItemText(idx, 1)) == VDisk.IP &&
			m_lstVDisk.GetItemText(idx, 2).Left(1) == (char)VDisk.SvrDrv)
		{
			AfxMessageBox("己经共享了服务器的该盘符.");
			return ;
		}
		if (m_lstVDisk.GetItemText(idx, 3).Left(1) == (char)VDisk.CliDrv)
		{
			AfxMessageBox("己经使用了客户机的盘符.");
			return ;
		}
	}

	if (m_cbVDiskMode.GetCurSel() == 1)
		i8desk::SAFE_STRCPY(VDisk.SID, MAIN_SERVER_SID);
	else
		i8desk::SAFE_STRCPY(VDisk.SID, "");

	std::string ErrInfo;
	if (pDlg->m_pDbMgr->ModifyVDisk(VDisk, ErrInfo))
	{
		ShowVDiskToList(nSel, VDisk);
		m_bChanged = true;
	}
	else
	{
		AfxMessageBox(ErrInfo.c_str());
	}
}

void CVDiskMgr::OnBnClickedDelete()
{
	int nSel = m_lstVDisk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		char* pGuid = reinterpret_cast<char*>(m_lstVDisk.GetItemData(nSel));
		std::string ID(pGuid);
		std::string ErrInfo;
		if (pDlg->m_pDbMgr->DelVDisk(ID, ErrInfo))
		{
			delete []pGuid;
			m_lstVDisk.DeleteItem(nSel);
			m_bChanged = true;
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CVDiskMgr::OnDestroy()
{
	for (int idx=0; idx<m_lstVDisk.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstVDisk.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

void CVDiskMgr::OnLvnItemchangedListVdisk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nItem = m_lstVDisk.GetNextItem(-1, LVIS_SELECTED);
	if (nItem == -1) return ;

	CString szItem = m_lstVDisk.GetItemText(nItem, 0);
	bool bI8Desk = (szItem == VDISK_I8DESK_TEXT);
	m_cboVdType.SetCurSel(bI8Desk ? 0 : 1);

	if (bI8Desk)
	{
		m_IP.SetAddress(ntohl(inet_addr(m_lstVDisk.GetItemText(nItem, 1))));
		m_cboSvrDrv.SelectString(-1, m_lstVDisk.GetItemText(nItem, 2));
		CString szText = m_lstVDisk.GetItemText(nItem, 4);
		if (szText == RT_COMP_BOOT)
			m_cboLoadType.SetCurSel(0);
		else if (szText == RT_MENU_RUN)
			m_cboLoadType.SetCurSel(1);
		else
			m_cboLoadType.SetCurSel(2);
	}
	else
	{
		m_IP.SetAddress(0);
		m_cboSvrDrv.SetCurSel(0);
		m_cboLoadType.SetCurSel(0);
	}
	m_cboCliDrv.SelectString(-1, m_lstVDisk.GetItemText(nItem, 3));	
	OnCbnSelchangeComboType();

	if (m_lstVDisk.GetItemText(nItem, 5) == VDISK_SERVER_MODE_GROUP)
		m_cbVDiskMode.SetCurSel(1);
	else
		m_cbVDiskMode.SetCurSel(0);
}

void CVDiskMgr::OnCbnSelchangeComboType()
{
	m_IP.EnableWindow(m_cboVdType.GetCurSel() == 0);
	m_cboSvrDrv.EnableWindow(m_cboVdType.GetCurSel() == 0);
	m_cboLoadType.EnableWindow(m_cboVdType.GetCurSel() == 0);
}

namespace vdisk
{
	#pragma pack(push,8)
	#pragma warning(disable:4200)

	enum {
		DRP_MJ_REFRESH	 = 0x01,		//刷新虚拟为盘，上行只有一个命令，应答的数据空。只是成功。
		DRP_MJ_READ		 = 0X02,
		DRP_MJ_WRITE	 = 0x03,
		DRP_MJ_QUERY	 = 0x04,
	};

	typedef struct tagDRP
	{
		BYTE			 MajorFunction;//1: Get Size ;2: Read ; 3: Write; 4:get Info
		union
		{
			struct 
			{
				LARGE_INTEGER	Offset;
				ULONG			Length;
			}Read;
			struct 
			{
				LARGE_INTEGER   Offset;
				ULONG			Length;
			}Write;
			struct 
			{
				ULONG	DiskSize;
			}Query;
		};
		char buffer[0];
	} DRP, *LPDRP;

	//应答头
	typedef struct tagDSTATUS
	{
		WORD	Status;		//STATUS_
		DWORD	Information;
		DWORD	Length;
		char	buffer[0];
	} DSTATUS, *LPDSTATUS; //*/

	enum {
		STATUS_SUCCESS			= 0x0000,
		STATUS_INVALID_CALL 	= 0x2001,
		STATUS_READ_ERROR		= 0x2002,
		STATUS_WRITE_ERROR  	= 0x2003,
	};

	#pragma pack(pop)
}

void CVDiskMgr::OnBnClickedCheck()
{
	int nSel = m_lstVDisk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || m_lstVDisk.GetItemText(nSel, 0) != VDISK_I8DESK_TEXT)
	{
		AfxMessageBox("请选中要检测的虚拟盘.");
		return ;
	}

	m_msg = _T("");
	UpdateData(FALSE);

	CString msg;

	CString ip = m_lstVDisk.GetItemText(nSel, 1);
	DWORD port = 17918 + m_lstVDisk.GetItemText(nSel, 2).GetAt(0)- 'A';
	if (m_lstVDisk.GetItemText(nSel, 5) != VDISK_SERVER_MODE_GROUP)
	{
		msg.Format(_T("测试[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
		AppendMsg(msg);

		CWaitMessageDlg wmd(msg);
		wmd.exec(std::tr1::bind(&CVDiskMgr::CheckVDisk, this, (LPCTSTR)ip, port, &msg));

		AppendMsg(msg);
	}
	else 
	{
		std::string ErrInfo;
		i8desk::SyncTaskMap MirrorServers;
		if (!pDlg->m_pDbMgr->GetAllMirrorServer(MirrorServers, ErrInfo))
		{
			AfxMessageBox("获取镜像服务器组失败.");
			return;
		}
		
		AppendMsg(_T("群组:主服务器:"));

		msg.Format(_T("测试[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
		AppendMsg(msg);

		CWaitMessageDlg wmd(msg);
		wmd.exec(std::tr1::bind(&CVDiskMgr::CheckVDisk, this, (LPCTSTR)ip, port, &msg));

		AppendMsg(msg);

		i8desk::SyncTaskMapItr it = MirrorServers.begin();
		for (; it != MirrorServers.end(); ++it)
		{
			msg.Format(_T("群组:镜像服务器,%s:"), it->second->Name);
			AppendMsg(msg);

			msg.Format(_T("测试[%s:%C]"), it->second->VDiskIP, port - 17918 + 'A');
			AppendMsg(msg);

			CWaitMessageDlg wmd(msg);
			wmd.exec(std::tr1::bind(&CVDiskMgr::CheckVDisk, this, it->second->VDiskIP, port, &msg));

			AppendMsg(msg);
		}
	}
}

void CVDiskMgr::CheckVDisk(LPCTSTR lpIPAddress, DWORD port, CString *result)
{
	ASSERT(result);

	using namespace vdisk;
	char buf[MAX_PATH] = {0};

	DRP drp = {0};
	drp.MajorFunction = DRP_MJ_QUERY;
	drp.Read.Offset.QuadPart = 0;
	drp.Read.Length = 512;

	i8desk::CTcpSocket sck;

	CWaitCursor wc;
	if (!sck.ConnectServer(lpIPAddress, port, 3))
	{
		*result = _T("连接服务器失败");
		return;
	}

	sck.Send((const char*)&drp, sizeof(drp)); 

	*result = _T("连接服务器成功");
}

void CVDiskMgr::OnBnClickedReflesh()
{
	char buf[MAX_PATH] = {0};

	int nSel = m_lstVDisk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1 || m_lstVDisk.GetItemText(nSel, 0) != VDISK_I8DESK_TEXT)
	{
		AfxMessageBox("请选中要检测的虚拟盘.");
		return ;
	}

	m_msg = _T("");
	UpdateData(FALSE);

	CString msg;
	CString ip = m_lstVDisk.GetItemText(nSel, 1);
	DWORD port = 17918 + m_lstVDisk.GetItemText(nSel, 2).GetAt(0)- 'A';

	if (m_lstVDisk.GetItemText(nSel, 5) != VDISK_SERVER_MODE_GROUP)
	{
		msg.Format(_T("刷新[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
		AppendMsg(msg);

		CWaitMessageDlg wmd(msg);
		wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, (LPCTSTR)ip, port, &msg));

		AppendMsg(msg);
	}
	else 
	{
		std::string ErrInfo;
		i8desk::SyncTaskMap MirrorServers;
		if (!pDlg->m_pDbMgr->GetAllMirrorServer(MirrorServers, ErrInfo))
		{
			AfxMessageBox("获取镜像服务器组失败.");
			return;
		}
		
		AppendMsg(_T("群组:主服务器:"));

		msg.Format(_T("刷新[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
		AppendMsg(msg);

		CWaitMessageDlg wmd(msg);
		wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, (LPCTSTR)ip, port, &msg));

		AppendMsg(msg);

		i8desk::SyncTaskMapItr it = MirrorServers.begin();
		for (; it != MirrorServers.end(); ++it)
		{
			msg.Format(_T("群组:镜像服务器,%s:"), it->second->Name);
			AppendMsg(msg);

			msg.Format(_T("刷新[%s:%C]"), it->second->VDiskIP, port - 17918 + 'A');
			AppendMsg(msg);

			CWaitMessageDlg wmd(msg);
			wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, it->second->VDiskIP, port, &msg));

			AppendMsg(msg);
		}
	}
}

void CVDiskMgr::AppendMsg(LPCTSTR msg)
{
	m_msg += msg;
	m_msg += _T("\r\n");
	UpdateData(FALSE);
}

void CVDiskMgr::RefleshVDisk(LPCTSTR lpIPAddress, DWORD port, CString *result)
{
	vdisk::DRP drp = {0};
	drp.MajorFunction = vdisk::DRP_MJ_REFRESH;

	i8desk::CTcpSocket sck;

	CWaitCursor wc;
	if (!sck.ConnectServer(lpIPAddress, port, 3))
	{
		*result = _T("连接服务器失败");
		return;
	}

	if (sck.Send((const char*)&drp, sizeof(drp)) != sizeof(drp)) 
	{
		*result = _T("发送刷盘命令失败");
		return;
	}

	vdisk::DSTATUS dstatus = { vdisk::STATUS_INVALID_CALL };
	if (sck.Recv((char *)&dstatus, sizeof(dstatus)) != sizeof(dstatus))
	{
		*result = _T("接收响应报文失败");
		return;
	}

	if (dstatus.Status != vdisk::STATUS_SUCCESS) 
	{
		*result = _T("服务端操作失败");
		return;
	}

	*result = _T("刷盘成功");
}

void CVDiskMgr::OnBnClickedVdiskRefleshAll()
{
	std::string ErrInfo;
	i8desk::SyncTaskMap MirrorServers;
	if (!pDlg->m_pDbMgr->GetAllMirrorServer(MirrorServers, ErrInfo))
	{
		AfxMessageBox("获取镜像服务器组失败.");
		return;
	}

	m_msg = _T("");
	UpdateData(FALSE);

	CString msg;

	char buf[MAX_PATH] = {0};

	for (int nSel = 0; nSel < m_lstVDisk.GetItemCount(); nSel++) 
	{
		if (m_lstVDisk.GetItemText(nSel, 0) != VDISK_I8DESK_TEXT)
		{
			continue;
		}

		CString ip = m_lstVDisk.GetItemText(nSel, 1);
		DWORD port = 17918 + m_lstVDisk.GetItemText(nSel, 2).GetAt(0)- 'A';
		if (m_lstVDisk.GetItemText(nSel, 5) != VDISK_SERVER_MODE_GROUP)
		{
			msg.Format(_T("刷新[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
			AppendMsg(msg);

			CWaitMessageDlg wmd(msg);
			wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, (LPCTSTR)ip, port, &msg));

			AppendMsg(msg);
		}
		else 
		{
			AppendMsg(_T("群组:主服务器:"));

			msg.Format(_T("刷新[%s:%C]"), (LPCTSTR)ip, port - 17918 + 'A');
			AppendMsg(msg);

			CWaitMessageDlg wmd(msg);
			wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, (LPCTSTR)ip, port, &msg));

			AppendMsg(msg);

			i8desk::SyncTaskMapItr it = MirrorServers.begin();
			for (; it != MirrorServers.end(); ++it)
			{
				msg.Format(_T("群组:镜像服务器,%s:"), it->second->Name);
				AppendMsg(msg);

				msg.Format(_T("刷新[%s:%C]"), it->second->VDiskIP, port - 17918 + 'A');
				AppendMsg(msg);

				CWaitMessageDlg wmd(msg);
				wmd.exec(std::tr1::bind(&CVDiskMgr::RefleshVDisk, this, it->second->VDiskIP, port, &msg));

				AppendMsg(msg);
			}
		}
	}
}

HBRUSH CVDiskMgr::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == GetDlgItem(IDC_EDIT_MSG)->m_hWnd)
	{
		//pDC->SetBkColor(RGB(255, 255, 255));
		//pDC->SetTextColor(RGB(0, 0, 0));
	}
	
	return hbr;
}

void CVDiskMgr::OnNMClickListVdisk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION pos = m_lstVDisk.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_MODIFY)->EnableWindow(b);
	this->GetDlgItem(IDC_DELETE)->EnableWindow(b);
	this->GetDlgItem(IDC_CHECK)->EnableWindow(b);
	this->GetDlgItem(IDC_VDISK_REFLESH)->EnableWindow(b);

	*pResult = 0;
}

void CVDiskMgr::OnLvnDeleteitemListVdisk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION pos = m_lstVDisk.GetFirstSelectedItemPosition();
	bool b = pos != NULL;

	this->GetDlgItem(IDC_MODIFY)->EnableWindow(b);
	this->GetDlgItem(IDC_DELETE)->EnableWindow(b);
	this->GetDlgItem(IDC_CHECK)->EnableWindow(b);
	this->GetDlgItem(IDC_VDISK_REFLESH)->EnableWindow(b);
	*pResult = 0;
}
