#include "stdafx.h"
#include "ServerUI.h"
#pragma warning(disable:4996)

#include "MainFrm.h"
#include "UiTreeView.h"
#include "VDiskInfoView.h"
#include "UpdateGameInfoView.h"
#include "SyncGameInfoView.h"
#include "LogView.h"
#include "SetDialog.h"
#include "FindGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define   VK_F   0X46 

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SET_MASTRIP, &CMainFrame::OnSetMastrip)
	ON_COMMAND(ID_VDISKINFO, &CMainFrame::OnVdiskinfo)
	ON_COMMAND(ID_UPDATEGAMEINFO, &CMainFrame::OnUpdategameinfo)
	ON_COMMAND(ID_SYNCINFO, &CMainFrame::OnSyncinfo)
	ON_COMMAND(ID_IMUPDATE, &CMainFrame::OnImupdate)
	ON_MESSAGE(MSG_INITVIEW,InitView)
	ON_MESSAGE(WM_DRAWBAR,DrawStatusBar)
	ON_MESSAGE(WM_HOTKEY,OnHotKey)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT nStatus[] = 
{
	ID_SEPARATOR,
	ID_STATUSTEXT
};

CMainFrame::CMainFrame():m_hread(NULL),m_bInitSyncView(false)
{
	m_hExit = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_pSocket =  new CSocketEx;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
		return -1;
	if (!m_wndStatusBar.Create(this))
		return -1;
	m_wndStatusBar.SetIndicators(nStatus, sizeof(nStatus)/sizeof(UINT));
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	PostMessage(MSG_INITVIEW,0,0);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}		

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if(!m_wndSplitter1.CreateStatic(this,1,2))
		return -1;
	CRect cr;
	GetClientRect(&cr);
	if(!m_wndSplitter1.CreateView(0,0,RUNTIME_CLASS(CUiTreeView),CSize(cr.Width()/4,cr.Height()),pContext))
		return -1;

	if(!m_wndSplitter2.CreateStatic(&m_wndSplitter1,2,1,WS_CHILD|WS_VISIBLE,m_wndSplitter1.IdFromRowCol(0,1)))
		return -1;
	
	if(!m_wndSplitter2.CreateView(0,0,RUNTIME_CLASS(CSyncGameInfoView),CSize(3*cr.Width()/4,cr.Height()*3/5),pContext))
		return -1;
	
	if(!m_wndSplitter2.CreateView(1,0,RUNTIME_CLASS(CLogView),CSize(3*cr.Width()/4,cr.Height()*2/5),pContext))
		return -1;

	CCreateContext Context ;
	Context.m_pCurrentFrame = this;
	Context.m_pCurrentDoc = NULL;
	Context.m_pNewViewClass = RUNTIME_CLASS(CUpdateGameInfoView);
	Context.m_pLastView = NULL;
	CWnd* pView = CreateView(&Context, 0x9300);
	pView->ShowWindow(SW_HIDE);
	pView->MoveWindow(0,0,cr.Width() * 3 / 4, cr.Height());
	m_mapView.insert(std::make_pair(STR_UPDATEGAMEINFO, pView));

	Context.m_pNewViewClass = RUNTIME_CLASS(CVDiskInfoView);
	pView = CreateView(&Context, 0x9301);
	pView->ShowWindow(SW_HIDE);
	pView->MoveWindow(0,0,cr.Width() * 3 / 4, cr.Height());
	m_mapView.insert(std::make_pair(STR_VDISKINFOVIEW, pView));
	m_mapView.insert(std::make_pair(STR_SYNCGAMEINFO, m_wndSplitter2.GetPane(0,0)));
	m_mapView.insert(std::make_pair(STR_LONGVIEW, m_wndSplitter2.GetPane(1,0)));

	SetWindowText(_T("节点监控"));
	CenterWindow();
	return true;
}

void CMainFrame::OnSetMastrip()
{
	CSetDialog dlg;
	if( dlg.DoModal() ==IDOK)
	{
		if(dlg.m_bSet)
		{
			WriteLog("正在停止服务.....");
			if(!StopServer(SrvName))
			{
				WriteLog("停止服务失败");
				return;
			}
			WriteLog("正在启动服务.....");
			if(!RunServer(SrvName))
			{
				WriteLog("启动服务失败");
				return ;
			}
			WriteLog("启动服务成功");
			m_bInitSyncView = false;
		}
	}
}

void CMainFrame::OnVdiskinfo()
{
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_VDISKINFOVIEW);
	if(it != m_mapView.end())
	{
		m_wndSplitter2.ChangeView(it->second);
	}
}

void CMainFrame::OnUpdategameinfo()
{
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_UPDATEGAMEINFO);
	if(it != m_mapView.end())
	{
		m_wndSplitter2.ChangeView(it->second);
	}
}

void CMainFrame::OnSyncinfo()
{
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_SYNCGAMEINFO);
	if(it != m_mapView.end())
	{
		m_wndSplitter2.ChangeView(it->second);
	}
}

bool CMainFrame::IsExistExe(LPCSTR Exename)
{
	DWORD ProcessID[1024];
	DWORD dwneed;
	EnumProcesses(ProcessID,sizeof(ProcessID),&dwneed);
	DWORD dwProcess = dwneed/sizeof(DWORD);
	HANDLE hSnopshort = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,ProcessID[0]);
	if(hSnopshort != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe = {0};
		pe.dwSize = sizeof(PROCESSENTRY32);
		BOOL bret =Process32First(hSnopshort,&pe);
		while(bret)
		{
			if(StrCmp(pe.szExeFile,Exename) == 0)
			{
				CloseHandle(hSnopshort);
				return true;
			}
			bret = 	Process32Next(hSnopshort,&pe);
		}
	}
	CloseHandle(hSnopshort);
	return false;
}

LRESULT CMainFrame::InitView(WPARAM wparam,LPARAM lparam)
{
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SendMessage(WM_SETICON, TRUE, (LPARAM)hIcon);

	if(IsExistServer(SrvName))
	{
		if(RunServer(SrvName))
		{
			WriteLog("启动服务成功");
		}
		else
		{
			WriteLog("启动服务失败");
		}
	}
	else
	{
		if(!IsExistExe("I8VDiskSvr.exe"))
		{
			char szPath[MAX_PATH] = {0};
			GetModuleFileName(NULL,szPath, MAX_PATH);
			PathRemoveFileSpec(szPath);
			lstrcat(szPath,"\\i8VDiskSvr.exe");
			if ((int)ShellExecute(NULL,"open",szPath, "-d", NULL, SW_HIDE) > 32)
				WriteLog("服务启动成功");
			else 
				WriteLog("服务启动失败");
		}
		else { WriteLog("服务已经启动"); }
	}
	UINT fsModifiers = 0; 
	fsModifiers |= MOD_CONTROL; 
	RegisterHotKey(GetSafeHwnd(), 1, fsModifiers, VK_F);
	SetTimer(1, 2000, NULL);
	m_hread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
	return 0;
}
UINT CMainFrame::WorkThread(LPVOID lparam)
{
	CMainFrame* pMainDlg = reinterpret_cast<CMainFrame*>(lparam);
	while(1)
	{
		char* pData = NULL;
		DWORD nRecv = 0;
		pMainDlg->m_pSocket->RecvDataFromServer(pData, nRecv);
		if(pData && nRecv)
		{
			switch(((_packageheader*)pData)->Cmd)
			{
			case NET_CMD_INTERACT_GUI:
				pMainDlg->DrawView(pData);
				break;
			case NET_CMD_SYNCLOG:
				pMainDlg->DrawLog(pData);
				break;
			case NET_CMD_ENDSYNCGAME:
				pMainDlg->NotiSyncEnd(pData);
				break;
			case NET_CMD_GETSYNCGAMEINFO:
				pMainDlg->DrawSyncView(pData);
				break;
			case NET_CMD_GETCONNECTINFO:
				{
					char buf[MAX_PATH] = {0};
					char szip[20] = {0};
					CPackageHelper outpack(pData);
					outpack.popString(szip);
					DWORD bconnect  = outpack.popDWORD();
					sprintf_s(buf,"连接服务器:%s:%s",szip, bconnect ? "成功": "失败");
					LPARAM lparam = (LPARAM)buf;
					::SendMessage(pMainDlg->m_hWnd,WM_DRAWBAR,0,lparam);
				}
				break;
			case  NET_CMD_REFRESHGUI:
				pMainDlg->m_bInitSyncView = false;
				break;
			default:break;
			}
			delete []pData;
		}

		if(WaitForSingleObject(pMainDlg->m_hExit,0) == WAIT_OBJECT_0)
			break;
	}
	return 0;
}

void CMainFrame::OnClose()
{
	SetEvent(m_hExit);
	if(m_pSocket)
	{
		m_pSocket->Close();
	}
	if(m_hread)
	{
		WaitForSingleObject(m_hread,INFINITE);
		CloseHandle(m_hread);
		m_hread = NULL;
	}

	if(m_pSocket)
	{
		m_pSocket->Release();
		m_pSocket = NULL;
	}

	CloseHandle(m_hExit);
	m_hExit = NULL;
	
	CFrameWnd::OnClose();
}

void CMainFrame::DrawSyncView(char* pData)
{
	CSyncGameInfoView* pView = NULL;
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_SYNCGAMEINFO);
	if(it != m_mapView.end())
	{
		pView = (CSyncGameInfoView*)it->second;
	}
	CPackageHelper outpack(pData);
	DWORD ncount  = outpack.popDWORD();
	DWORD dwNeedSyncCount = 0;
	if(pView)
	{
		pView->GetListCtrl().DeleteAllItems();
		char szgame[MAX_PATH] = {0};
		for(DWORD i = 0; i< ncount; i++)
		{
			char szgid[10] = {0};
			sprintf_s(szgid,"%d",outpack.popDWORD());
			outpack.popString(szgame);
			int nItem  = pView->GetListCtrl().GetItemCount();
			pView->GetListCtrl().InsertItem(nItem,szgid);
			pView->GetListCtrl().SetItemText(nItem,1,szgame);
			bool bNoNeedSync = outpack.popDWORD() == 0;
			pView->GetListCtrl().SetItemText(nItem,2, bNoNeedSync ? "己最新" : "正在排队");
			if (!bNoNeedSync)
				dwNeedSyncCount++;
		}
	}
	char szLog[MAX_PATH] = {0};
	sprintf(szLog, "总共游戏:%d个, 需要同步的游戏数:%d个", ncount, dwNeedSyncCount);
	WriteLog(szLog);
	m_bInitSyncView = true;
}

bool CMainFrame::DrawView(char* pData)
{
	static bool bDrawlog = false;
	CVDiskInfoView* pVDiskView = NULL;
	CUpdateGameInfoView* pUpdateView = NULL;
	CSyncGameInfoView* pSyncView = NULL;
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_VDISKINFOVIEW);
	if(it != m_mapView.end())
	{
		pVDiskView = (CVDiskInfoView*)it->second;
	}
	
	it  = m_mapView.find(STR_UPDATEGAMEINFO);
	if(it != m_mapView.end())
	{
		pUpdateView = (CUpdateGameInfoView*)it->second;
	}

	it  = m_mapView.find(STR_SYNCGAMEINFO);
	if(it != m_mapView.end())
	{
		pSyncView = (CSyncGameInfoView*)it->second;
	}

	CString strPort,strConnect,strTime;
	CPackageHelper outpack(pData);
	DWORD ncount  = outpack.popDWORD();	
	pVDiskView->GetListCtrl().DeleteAllItems();
	for(DWORD i = 0 ;i <ncount ; i++)
	{
		char buf[sizeof(tagVDInfo)+1] = {0};
		tagVDInfo* pVdInfo =(tagVDInfo*)buf;
		outpack.popString(buf);

		strPort.Format("%d",pVdInfo->nPort);
		strConnect.Format("%d",pVdInfo->dwConnect);
		tm* localtm = _localtime32((__time32_t*)&pVdInfo->dwVersion);
		strTime.Format("%d/%d/%d %02d:%02d:%02d",localtm->tm_year+1900,localtm->tm_mon+1,localtm->tm_mday,
			localtm->tm_hour,localtm->tm_min,localtm->tm_sec);
		
		if(pVDiskView)
		{
			pVDiskView->GetListCtrl().InsertItem(i,CString(pVdInfo->szSvrDrv));
			pVDiskView->GetListCtrl().SetItemText(i,1,CString(pVdInfo->szCliDrv));
			pVDiskView->GetListCtrl().SetItemText(i,2,strConnect);
			pVDiskView->GetListCtrl().SetItemText(i,3,strPort);
			pVDiskView->GetListCtrl().SetItemText(i,4,strTime);
		}
	}
	char buf[sizeof(FastCopyStatusEx)+1] = {0};
	outpack.popString(buf);
	FastCopyStatusEx* pStatus = reinterpret_cast<FastCopyStatusEx*>(buf);
	if(pSyncView)
	{
		char tmp[32] = {0};
		sprintf_s(tmp,"%d",pStatus->gid);
		LVFINDINFO info;
		info.flags = LVFI_PARTIAL|LVFI_STRING;
		info.psz = tmp;	
		int nIndex = pSyncView->GetListCtrl().FindItem(&info);
		if(nIndex != -1)
		{
			pSyncView->GetListCtrl().SetItemText(nIndex,2,_T("正在同步"));
			sprintf_s(tmp,"%d%%",pStatus->TotalProgress/10);
			pSyncView->GetListCtrl().SetItemText(nIndex,3,tmp);
			sprintf_s(tmp,"%d M",(pStatus->TotalFileSize/(1024*1024)));
			pSyncView->GetListCtrl().SetItemText(nIndex,4,tmp);
			sprintf_s(tmp,"%d M",((pStatus->TotalFileSize-pStatus->UpdateFizeSize)/(1024*1024)));
			pSyncView->GetListCtrl().SetItemText(nIndex,5,tmp);
			sprintf_s(tmp,"%d K/S",pStatus->AverageSpeed);
			pSyncView->GetListCtrl().SetItemText(nIndex,6,tmp);
			pSyncView->GetListCtrl().EnsureVisible(nIndex, TRUE);
		}
	}
	if(pUpdateView)
	{
		pUpdateView->GetListCtrl().DeleteAllItems();
		size_t count  = outpack.popDWORD();
		char buftmp[20] = {0};
		char UpInfo[sizeof(UpdataInfo)+1] = {0};
		for(size_t i= 0; i<count ; i++)
		{
			outpack.popString(UpInfo);
			UpdataInfo* p =(UpdataInfo*)UpInfo;
			pUpdateView->GetListCtrl().InsertItem(i,inet_ntoa(*(in_addr*)&p->dwip));
			sprintf_s(buftmp,"%d",p->gid);
			pUpdateView->GetListCtrl().SetItemText(i,1,buftmp);
			pUpdateView->GetListCtrl().SetItemText(i,2,p->Name);
			pUpdateView->GetListCtrl().SetItemText(i,3,"下载");

			sprintf_s(buftmp,"%d%%",p->progress/10);
			pUpdateView->GetListCtrl().SetItemText(i,4,buftmp);

			sprintf_s(buftmp,"%d M",p->uptsize/(1024));
			pUpdateView->GetListCtrl().SetItemText(i,5,buftmp);

			sprintf_s(buftmp,"%d M",p->leftsize/(1024));
			pUpdateView->GetListCtrl().SetItemText(i,6,buftmp);

			sprintf_s(buftmp,"%d K/S",p->speed);
			pUpdateView->GetListCtrl().SetItemText(i,7,buftmp);
			ZeroMemory(UpInfo,sizeof(UpdataInfo)+1);
		}
	}
	return true;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bInitSyncView)
	{
		m_pSocket->SendCmdToServer(NET_CMD_GETCONNECTINFO);
		m_pSocket->SendCmdToServer(NET_CMD_GETSYNCGAMEINFO);
		OnImupdate();
		m_bInitSyncView = true;
	}
	m_pSocket->SendCmdToServer(NET_CMD_INTERACT_GUI);
	CFrameWnd::OnTimer(nIDEvent);
}

int CMainFrame::WriteLog(CString strlog)
{
	CTime t = CTime::GetCurrentTime();
	CString   strTemp   =   t.Format("%Y-%m-%d  %H:%M:%S"); 
	CLogView * pLogView = NULL;
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_LONGVIEW);
	if(it != m_mapView.end())
	{
		pLogView = (CLogView*)it->second;

		int nIndex = pLogView->GetListCtrl().GetItemCount();
		if(nIndex>2048)
		{
			pLogView->GetListCtrl().DeleteAllItems();
		}
		pLogView->GetListCtrl().InsertItem(nIndex,strTemp);
		pLogView->GetListCtrl().SetItemText(nIndex,1,strlog);
		pLogView->GetListCtrl().EnsureVisible(nIndex,FALSE);
	}
	return 0;
}

int CMainFrame::DrawLog(char* pData)
{
	CPackageHelper outpack(pData);
	char log[4096] = {0};
	outpack.popString(log);
	WriteLog(CString(log));
	return 0;
}

int CMainFrame::NotiSyncEnd(char* pData)
{
	CPackageHelper outpack(pData);
	DWORD gid = outpack.popDWORD();
	bool  bfinish = (outpack.popDWORD() ==1);

	CSyncGameInfoView* pSyncView = NULL;
	std::map<std::string,CWnd*>::iterator it  = m_mapView.find(STR_SYNCGAMEINFO);
	if(it != m_mapView.end())
	{
		pSyncView = (CSyncGameInfoView*)it->second;
		char tmp[32] = {0};
		sprintf_s(tmp,"%d",gid);
		LVFINDINFO info;
		info.flags = LVFI_PARTIAL|LVFI_STRING;
		info.psz = tmp;	
		int nIndex = pSyncView->GetListCtrl().FindItem(&info);
		if(nIndex == -1 )
			return 0;
		pSyncView->GetListCtrl().SetItemText(nIndex,2,bfinish?_T("已最新"):_T("同步失败"));
		pSyncView->GetListCtrl().SetItemText(nIndex,3,"");
		pSyncView->GetListCtrl().SetItemText(nIndex,4,"");
		pSyncView->GetListCtrl().SetItemText(nIndex,5,"");
		pSyncView->GetListCtrl().SetItemText(nIndex,6,"");
	
	}
	return 0;
}

bool CMainFrame::InstallServer(LPCSTR lpszSvrName,LPCSTR  lpszSvrDispName)
{
	SC_HANDLE   schService;
	SC_HANDLE   schSCManager;

	TCHAR szPath[512] = {0};
	GetModuleFileName(NULL,szPath,MAX_PATH);
	PathRemoveFileSpec(szPath);
	lstrcat(szPath,"\\");
	lstrcat(szPath,lpszSvrName);

	schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
		);
	if (schSCManager == NULL)
		return false;

	schService = CreateService(
		schSCManager,               // SCManager database
		lpszSvrName,				// name of service
		lpszSvrDispName,			// name to display
		SERVICE_ALL_ACCESS,         // desired access
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,  // service type
		SERVICE_AUTO_START,			// start type
		SERVICE_ERROR_NORMAL,       // error control type
		szPath,                     // service's binary
		NULL,                       // no load ordering group
		NULL,                       // no tag identifier
		NULL,						// dependencies
		NULL,                       // LocalSystem account
		NULL);                      // no password

	if (schService == NULL)
	{
		DWORD dwErr = GetLastError();
		CloseServiceHandle(schSCManager);
		return false;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return true;
}

LRESULT CMainFrame::DrawStatusBar(WPARAM wparam,LPARAM lparam)
{
	char * pdata = (char*)lparam;
	m_wndStatusBar.SetPaneText(1,pdata);

	return S_OK;
}

void CMainFrame::OnImupdate()
{
	m_pSocket->SendCmdToServer(NET_CMD_IMMUPDATEALLGAME);
}

LRESULT   CMainFrame::OnHotKey(WPARAM   wParam,LPARAM   lParam)   
{   
	UINT fuModifiers = (UINT)LOWORD(lParam);
	UINT f = HIWORD(lParam);
	if (fuModifiers == MOD_CONTROL && f == VK_F)
	{   
		CFindGameDlg dlg;
		dlg.DoModal();
	}   
	return S_OK;
}   

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_CHAR)
	{
		CFindGameDlg dlg;
		dlg.m_InputChar = (char)pMsg->wParam;
		dlg.DoModal();
		return TRUE;
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}
