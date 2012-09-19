// CliSysInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "CliSysInfo.h"
#include "ConsoleDlg.h"
#include "Markup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CCliSysInfo, CDialog)

CCliSysInfo::CCliSysInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CCliSysInfo::IDD, pParent)
{
	m_InfoType = CLI_SYSINFO;
}

CCliSysInfo::~CCliSysInfo()
{
}

void CCliSysInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
	DDX_Control(pDX, IDC_SYSINFO, m_edtSysInfo);
}

BEGIN_MESSAGE_MAP(CCliSysInfo, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, &CCliSysInfo::OnNMRClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CCliSysInfo::OnNMDblclkList)
	ON_COMMAND(ID_KILL_REFRESH, &CCliSysInfo::OnKillRefresh)
	ON_COMMAND(ID_KILL_PROCSVR, &CCliSysInfo::OnKillProcsvr)
END_MESSAGE_MAP()

BOOL CCliSysInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcClient;
	GetClientRect(&rcClient);
	rcClient.DeflateRect(1, 1, 1, 1);
	m_lstCtrl.MoveWindow(&rcClient);
	m_edtSysInfo.MoveWindow(&rcClient);

	RefreshInfo();

	SetTimer(1, 500, NULL);
	return TRUE;
}

void CCliSysInfo::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	i8desk::CCliMgr* pCliMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pCliMgr;
	DWORD dwCmd = 0;
	std::string Info;
	if (pCliMgr->GetUdpInfo(dwCmd, Info))
	{
		if (dwCmd == Ctrl_ViewSysInfo)
		{
			SetDlgItemText(IDC_SYSINFO, Info.c_str());
		}
		else if (dwCmd ==  Ctrl_ViewProcInfo)
		{
			m_lstCtrl.DeleteAllItems();
			CMarkup xml;
			xml.SetDoc(Info);
			if (xml.GetError().size() == 0)
			{
				xml.ResetPos();
				if (xml.FindElem("ProcessLst"))
				{
					xml.IntoElem();
					while (xml.FindElem("Process"))
					{
						CString szItem;
						szItem.Format("%d", atoi(xml.GetAttrib("Memory").c_str()) / 1024);

						int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), xml.GetAttrib("Name").c_str());
						m_lstCtrl.SetItemText(nIdx, 1, xml.GetAttrib("Pid").c_str());						
						m_lstCtrl.SetItemText(nIdx, 2, szItem);
					}
				}				
			}
		}
		else if (dwCmd == Ctrl_ViewSviInfo)
		{
			m_lstCtrl.DeleteAllItems();
			CMarkup xml;
			xml.SetDoc(Info);
			if (xml.GetError().size() == 0)
			{
				xml.ResetPos();
				if (xml.FindElem("ServiceLst"))
				{
					xml.IntoElem();
					while (xml.FindElem("Service"))
					{
						int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), xml.GetAttrib("Name").c_str());
						m_lstCtrl.SetItemText(nIdx, 1, xml.GetAttrib("DisplayName").c_str());
						m_lstCtrl.SetItemText(nIdx, 2, xml.GetAttrib("Application").c_str());
					}
				}				
			}
		}
	}
}

BOOL CCliSysInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	if (pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == m_edtSysInfo.m_hWnd)
	{
		LRESULT result;
		OnNMDblclkList(NULL, &result);
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);	
}

void CCliSysInfo::OnDestroy()
{
	KillTimer(1);
	
	CDialog::OnDestroy();
}

void CCliSysInfo::OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	
	LRESULT result;
	OnNMDblclkList(NULL, &result);
}

void CCliSysInfo::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CPoint oPoint;
	GetCursorPos( &oPoint );
	CMenu menu;
	menu.LoadMenu(MAKEINTRESOURCE(IDR_MENU_KILL));
	menu.ModifyMenu(ID_KILL_PROCSVR, MF_BYCOMMAND|MF_STRING, ID_KILL_PROCSVR, 
		m_InfoType == CLI_PROCINFO ? "结束进程" : "停止服务");
	if (m_InfoType == CLI_SYSINFO)
		menu.EnableMenuItem(ID_KILL_PROCSVR, MF_GRAYED);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		oPoint.x, oPoint.y, this);
}

void  CCliSysInfo::RefreshInfo()
{
	CWaitCursor wc;
	m_edtSysInfo.SetWindowText("");
	m_lstCtrl.DeleteAllItems();

	std::string ErrInfo;
	std::vector<DWORD> list;
	list.push_back(m_dwIp);
	i8desk::CCliMgr* pCliMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pCliMgr;
	bool bFlag = false;

	if (m_InfoType == CLI_SYSINFO)
	{
		SetWindowText("计算机系统信息");
		m_edtSysInfo.ShowWindow(SW_SHOW);

		bFlag = pCliMgr->ViewSysInfo(list, ErrInfo);
	}
	else if (m_InfoType == CLI_PROCINFO)
	{
		SetWindowText("计算机进程信息");
		m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		m_lstCtrl.ShowWindow(SW_SHOW);
		m_lstCtrl.InsertColumn(0, "进程名称",	LVCFMT_LEFT, 450);
		m_lstCtrl.InsertColumn(1, "PID",		LVCFMT_RIGHT, 60);
		m_lstCtrl.InsertColumn(2, "内存使用(K)",LVCFMT_RIGHT, 100);

		bFlag = pCliMgr->ViewProcInfo(list, ErrInfo);
	}
	else 
	{
		SetWindowText("计算机服务信息");
		m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		m_lstCtrl.ShowWindow(SW_SHOW);
		m_lstCtrl.InsertColumn(0, "服务名",		LVCFMT_LEFT, 90);
		m_lstCtrl.InsertColumn(1, "显示名称",	LVCFMT_LEFT, 170);
		m_lstCtrl.InsertColumn(2, "服务进程名",	LVCFMT_LEFT, 350);

		bFlag = pCliMgr->ViewSviInfo(list, ErrInfo);
	}
	wc.Restore();
	if (!bFlag)
	{
		AfxMessageBox(ErrInfo.c_str());
	}
}

void  CCliSysInfo::KillProcSvr()
{
	if (m_InfoType == CLI_SYSINFO)
		return ;

	int nSel = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	CString file = m_lstCtrl.GetItemText(nSel, 0);
	if (file.IsEmpty())
		return ;

	std::string ErrInfo;
	std::vector<DWORD> list;
	list.push_back(m_dwIp);
	i8desk::CCliMgr* pCliMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pCliMgr;

	if (!pCliMgr->KillProcSvr((LPCSTR)file, m_dwIp, ErrInfo, m_InfoType == CLI_PROCINFO))
	{
		AfxMessageBox(ErrInfo.c_str());
	}
	RefreshInfo();
}

void CCliSysInfo::OnKillRefresh()
{
	RefreshInfo();
}

void CCliSysInfo::OnKillProcsvr()
{
	CWaitCursor wc;
	KillProcSvr();
	wc.Restore();
}
