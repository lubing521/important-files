
// UDiskServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskServer.h"
#include "UDiskServerDlg.h"
#include "NetEvent.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  WM_NotifyIcon  WM_USER +0x0010
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#include "d:\\psdk\\include\\IPTypes.h"
typedef DWORD (WINAPI *PFNGETADAPTERSINFO)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);

class DllLoadLib
{
public:
	HMODULE m_hModule;
	void* m_pProg;

	DllLoadLib(LPCTSTR pName,LPCSTR pProgName)
	{
		m_hModule = LoadLibrary(pName);
		m_pProg = m_hModule ? (void*)GetProcAddress (m_hModule,pProgName) : NULL;
	}

	~DllLoadLib()
	{
		if(m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
};

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUDiskServerDlg 对话框




CUDiskServerDlg::CUDiskServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDiskServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pConfigInfo = theApp.m_pConfigInfo;
	m_pNetLayer = NULL;
	m_hExit = CreateEvent(NULL,FALSE,FALSE,NULL);
}

void CUDiskServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_RigstrCombox);
	DDX_Control(pDX, IDC_CHECK2, m_chkAutoStart);
	DDX_Control(pDX, IDC_CHECK3, m_ChkCommRes);
}

BEGIN_MESSAGE_MAP(CUDiskServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON3, &CUDiskServerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CUDiskServerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUDiskServerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CUDiskServerDlg::OnBnClickedOk)
	ON_MESSAGE(WM_NotifyIcon,OnNotifyIcon)
	ON_COMMAND(IDM_SHOW, &CUDiskServerDlg::OnShow)
	ON_COMMAND(IDM_EXIT, &CUDiskServerDlg::OnExit)
END_MESSAGE_MAP()


// CUDiskServerDlg 消息处理程序

BOOL CUDiskServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	theApp.SetMainHWnd(m_hWnd);
	ShowWindow(SW_SHOW);
	SetWindowText("个人U盘服务端");
	if(!InitNetLayer())
	{
		AfxMessageBox("加载网络失败");
		OnOK();
		return FALSE;
	}
	m_RigstrCombox.AddString("开放注册");
	m_RigstrCombox.AddString("停止注册");

	GetConfigFormFile();
	SetConfigInfo();
	m_hTread = (HANDLE)_beginthreadex(NULL,0,WorkThread,this,0,NULL);
	// TODO: 在此添加额外的初始化代码
	//OnClose();
	Sleep(2000);
	PostMessage(WM_CLOSE,0,0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUDiskServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUDiskServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUDiskServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CUDiskServerDlg::InitNetLayer(void)
{
		IMemPool* pMempool = NULL;
		HMODULE hmod = LoadLibrary("NetLayer.dll");
		PFNCREATEIMEMPOOL CreateMemPool  = (PFNCREATEIMEMPOOL)(GetProcAddress(hmod,(LPSTR)1));
		PFNCREATENETLAYER CreateNetLayer = (PFNCREATENETLAYER)(GetProcAddress(hmod,(LPSTR)2));

		if(CreateMemPool == NULL || CreateNetLayer == NULL)
		{
			return false;
		}
		CreateMemPool(&pMempool);
		pMempool->SetDefault(50,66*1024,20);

		CreateNetLayer(&m_pNetLayer);
		m_pNetLayer->SetIMemPool(pMempool);

		INetLayerEvent* pNetLayerEvent  = new CNetEvent;
		m_pNetLayer->SetINetLayerEvent(pNetLayerEvent);

		DWORD dwError = m_pNetLayer->Start(PORT);
		return (dwError ==0);

}
LRESULT CUDiskServerDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	POINT rpoint;   
	CMenu tmenu;   
	GetCursorPos(&rpoint); 
	if ( ( wParam == IDR_MAINFRAME ) && ( lParam == WM_LBUTTONDBLCLK ) )   
	{   
		ShowWindow(SW_SHOW); //显示窗体   
		Shell_NotifyIcon(NIM_DELETE, &m_nid); //删除系统托盘图标   
	}

	if ( ( wParam == IDR_MAINFRAME ) && ( lParam == WM_RBUTTONDOWN ) )   
	{         
		tmenu.LoadMenu(IDR_MENU1); //加载系统主菜单  
		SetForegroundWindow(); //将程序置于桌面顶部   
		tmenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN |   TPM_RIGHTBUTTON,rpoint.x, rpoint.y, this); //在点击图标处弹出系统菜单的第一项  
	}   
return S_OK;
}
void   CUDiskServerDlg::autototray()   
{   
	m_nid.cbSize=sizeof(NOTIFYICONDATA); 
	m_nid.hWnd=this->m_hWnd; 
	m_nid.uID=IDR_MAINFRAME; 
	m_nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	m_nid.uCallbackMessage=WM_NotifyIcon;  
	m_nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME)); 
	lstrcpy(m_nid.szTip,"个人U盘服务端");     //图标提示 
	Shell_NotifyIcon(NIM_ADD,&m_nid);//向任务栏添加图标
	ShowWindow(SW_HIDE); 
}   

void CUDiskServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	autototray();
	
}

void CUDiskServerDlg::OnBnClickedButton3()
{
	GetConfigInfo();
	if(SaveConfig())
	{
		AfxMessageBox("保存成功！");
	}
	else
	{
		AfxMessageBox("保存失败！");
	}
}
bool CUDiskServerDlg::AutoStart(bool bStart)
{
	if(bStart)
	{
		char szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL,szPath,MAX_PATH);
		DWORD ret =	SHSetValue(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","i8uDisk"
								,REG_SZ,szPath,lstrlen(szPath));
	}
	else
	{
		SHDeleteValue(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run","i8uDisk");
	}

	return true;

}
bool  	CUDiskServerDlg::SaveConfig()
{
	char szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL,szPath,MAX_PATH);
	PathRemoveFileSpec(szPath);
	lstrcat(szPath,"\\Config.dat");
	DWORD ByteWrite;

	HANDLE hFile = CreateFile(szPath,GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	char * pData = new char[64*1024];
	CPackageHelper inpack(pData);
	inpack.pushString(m_pConfigInfo->sztitle.c_str(),m_pConfigInfo->sztitle.size());
	inpack.pushString(m_pConfigInfo->szSavePath.c_str(),m_pConfigInfo->szSavePath.size());
	inpack.pushDWORD(m_pConfigInfo->dwAllocSize);
	inpack.pushString(m_pConfigInfo->szCustomInfo.c_str(),m_pConfigInfo->szCustomInfo.size());
	inpack.pushDWORD(m_pConfigInfo->bAutoStart?1:0);
	inpack.pushDWORD(m_pConfigInfo->bRegist?1:0);
	inpack.pushString(m_pConfigInfo->szStopRigTitle.c_str(),m_pConfigInfo->szStopRigTitle.size());
	inpack.pushDWORD(m_pConfigInfo->dwDay);
	inpack.pushDWORD(m_pConfigInfo->dwSpace);
	inpack.pushString(m_pConfigInfo->szCommResPath.c_str(),m_pConfigInfo->szCommResPath.size());
	inpack.pushDWORD(m_pConfigInfo->bShowCommres?1:0);
	_packageheader header ={0};
	header.Length = inpack.GetOffset();
	inpack.pushPackageHeader(header);
	WriteFile(hFile ,pData,header.Length,&ByteWrite,NULL);

	AutoStart(m_pConfigInfo->bAutoStart);
	delete []pData;
return true;
}
void CUDiskServerDlg::GetConfigInfo(void)
{
	CString str;
	GetDlgItemText(EDT_TITLE,str);
	m_pConfigInfo->sztitle = string(str);
	GetDlgItemText(EDT_SAVEPATH,str);
	m_pConfigInfo->szSavePath = string(str);

	if(m_pConfigInfo->szSavePath.size() && *m_pConfigInfo->szSavePath.rbegin() != '\\')
		m_pConfigInfo->szSavePath+="\\";

	GetDlgItemText(IDC_EDIT3,str);
	m_pConfigInfo->dwAllocSize = atol(str);
	GetDlgItemText(IDC_EDIT8,str);
	m_pConfigInfo->szCustomInfo = string(str);
	m_pConfigInfo->bAutoStart = (m_chkAutoStart.GetCheck()==1);
	m_pConfigInfo->bRegist = (m_RigstrCombox.GetCurSel() ==0);
	GetDlgItemText(IDC_EDIT4,str);
	m_pConfigInfo->szStopRigTitle = str;

	GetDlgItemText(IDC_EDIT6,str);
	m_pConfigInfo->dwDay  = atol(str);
	
	GetDlgItemText(IDC_EDIT7,str);
	m_pConfigInfo->dwSpace = atol(str);
	GetDlgItemText(IDC_EDIT5,str);
	m_pConfigInfo->szCommResPath = string(str);
	if(m_pConfigInfo->szCommResPath.size() && *m_pConfigInfo->szCommResPath.rbegin() !='\\')
		m_pConfigInfo->szCommResPath +="\\";

	m_pConfigInfo->bShowCommres = (m_ChkCommRes.GetCheck() ==1);
}

void CUDiskServerDlg::SetConfigInfo(void)
{
	if(m_pConfigInfo->szSavePath.size() ==0)
	{
		char szPath[MAX_PATH] = {0};
		GetModuleFileName(NULL,szPath,MAX_PATH);
		PathRemoveFileSpec(szPath);
		lstrcat(szPath,"\\u盘存档\\");
		MakeSureDirectoryPathExists(szPath);
		m_pConfigInfo->szSavePath = string(szPath);
	}


	SetDlgItemText(EDT_TITLE,m_pConfigInfo->sztitle.c_str());
	SetDlgItemText(EDT_SAVEPATH,m_pConfigInfo->szSavePath.c_str());
	CString strtmp ;
	strtmp.Format("%d",m_pConfigInfo->dwAllocSize);
	SetDlgItemText(IDC_EDIT3,strtmp);
	SetDlgItemText(IDC_EDIT8,m_pConfigInfo->szCustomInfo.c_str());
	m_chkAutoStart.SetCheck(m_pConfigInfo->bAutoStart);

	m_RigstrCombox.SetCurSel(m_pConfigInfo->bRegist?0:1);
	SetDlgItemText(IDC_EDIT4,m_pConfigInfo->szStopRigTitle.c_str());
	strtmp.Format("%d",m_pConfigInfo->dwDay);
	SetDlgItemText(IDC_EDIT6,strtmp);
	strtmp.Format("%d",m_pConfigInfo->dwSpace);
	SetDlgItemText(IDC_EDIT7,strtmp);
	SetDlgItemText(IDC_EDIT5,m_pConfigInfo->szCommResPath.c_str());
	m_ChkCommRes.SetCheck(m_pConfigInfo->bShowCommres);
}

bool CUDiskServerDlg::GetConfigFormFile(void)
{
	char szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL,szPath,MAX_PATH);
	PathRemoveFileSpec(szPath);
	lstrcat(szPath,"\\Config.dat");
	DWORD ByteRead;
	
	HANDLE hFile = CreateFile(szPath,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwSize = GetFileSize(hFile,NULL);
	char* pData = new char[dwSize+20];
	ReadFile(hFile,pData,dwSize,&ByteRead,NULL);
	CloseHandle(hFile);

	CPackageHelper outpack(pData);
	char buf[4096] = {0};
	try
	{

		outpack.popString(buf);
		m_pConfigInfo->sztitle = string(buf);
		outpack.popString(buf);
		m_pConfigInfo->szSavePath = string(buf);
		m_pConfigInfo->dwAllocSize = outpack.popDWORD();
		outpack.popString(buf);
		m_pConfigInfo->szCustomInfo = string(buf);
		m_pConfigInfo->bAutoStart = (outpack.popDWORD()==1);
		m_pConfigInfo->bRegist = (outpack.popDWORD() ==1);
		outpack.popString(buf);
		m_pConfigInfo->szStopRigTitle = string(buf);
		m_pConfigInfo->dwDay = outpack.popDWORD();
		m_pConfigInfo->dwSpace = outpack.popDWORD();
		outpack.popString(buf);
		m_pConfigInfo->szCommResPath = string(buf);
		m_pConfigInfo->bShowCommres = (outpack.popDWORD()==1);
	}
	catch(...)
	{

	}
	delete []pData;
	/*
	inpack.pushString(m_pConfigInfo->sztitle.c_str(),m_pConfigInfo->sztitle.size());
	inpack.pushString(m_pConfigInfo->szSavePath.c_str(),m_pConfigInfo->szSavePath.size());
	inpack.pushDWORD(m_pConfigInfo->dwAllocSize);
	inpack.pushString(m_pConfigInfo->szCustomInfo.c_str(),m_pConfigInfo->szCustomInfo.size());
	inpack.pushDWORD(m_pConfigInfo->bAutoStart?1:0);
	inpack.pushDWORD(m_pConfigInfo->bRegist?1:0);
	inpack.pushString(m_pConfigInfo->szStopRigTitle.c_str(),m_pConfigInfo->szStopRigTitle.size());
	inpack.pushDWORD(m_pConfigInfo->dwDay);
	inpack.pushDWORD(m_pConfigInfo->dwSpace);
*/
	return true;
}

void CUDiskServerDlg::OnBnClickedButton1()
{
	BROWSEINFO bi;
	char szpath[MAX_PATH] = {0};
	//	TCHAR           szDisplayName[MAX_PATH]; 
	LPITEMIDLIST     pidl; 
	LPMALLOC     pMalloc = NULL;  
	ZeroMemory(&bi, sizeof(bi));      
	bi.hwndOwner = GetSafeHwnd(); 
	bi.pszDisplayName = szpath; 
	bi.lpszTitle = TEXT("请选择一个文件夹:"); 
	bi.ulFlags = BIF_RETURNONLYFSDIRS;  
	pidl = SHBrowseForFolder(&bi); 

	if (pidl) 
	{ 
		SHGetPathFromIDList(pidl, szpath); 
		SetDlgItemText(EDT_SAVEPATH,szpath);
		m_pConfigInfo->szSavePath = string(szpath);
	}
	
}

void CUDiskServerDlg::OnBnClickedButton2()
{
	BROWSEINFO bi;
	char szpath[MAX_PATH] = {0};
	//	TCHAR           szDisplayName[MAX_PATH]; 
	LPITEMIDLIST     pidl; 
	LPMALLOC     pMalloc = NULL;  
	ZeroMemory(&bi, sizeof(bi));      
	bi.hwndOwner = GetSafeHwnd(); 
	bi.pszDisplayName = szpath; 
	bi.lpszTitle = TEXT("请选择一个文件夹:"); 
	bi.ulFlags = BIF_RETURNONLYFSDIRS;  
	pidl = SHBrowseForFolder(&bi); 

	if (pidl) 
	{ 
		SHGetPathFromIDList(pidl, szpath);  
		SetDlgItemText(IDC_EDIT5,szpath);
		m_pConfigInfo->szCommResPath = string(szpath);
	}
}

UINT CUDiskServerDlg::WorkThread(LPVOID lparam)
{
	CUDiskServerDlg* pthis = reinterpret_cast<CUDiskServerDlg*>(lparam);
	vector<string> vecLst;
	pthis->	ScanDeleteUser(pthis->m_pConfigInfo->szSavePath,vecLst);
	char path[MAX_PATH] = {0};
	for(size_t i = 0 ;i<vecLst.size();i++)
	{
		ZeroMemory(path,MAX_PATH);
		lstrcpy(path,vecLst[i].c_str());
		SHFILEOPSTRUCT FileOp; 
		ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));
		FileOp.fFlags = FOF_SILENT |/* FOF_NOERRORUI |*/ FOF_NOCONFIRMATION; 
		FileOp.hNameMappings = NULL; 
		FileOp.hwnd = pthis->m_hWnd; 
		FileOp.lpszProgressTitle = NULL; 
		FileOp.pFrom =  path;
		FileOp.pTo =NULL; 
		FileOp.wFunc = FO_DELETE; 
		int ret = SHFileOperation(&FileOp);
		Sleep(1);
	}
	pthis->Listen();

	return 0;
}
bool CUDiskServerDlg::ScanDeleteUser(string& str,vector<string>&veclst)
{
	string szDest(str);
	if(szDest.size() ==0)
		return false;
	if( *szDest.rbegin() != '\\' )
		szDest +="\\";
	szDest+="*.*";
	WIN32_FIND_DATA wfd ={0};
	HANDLE hFile = FindFirstFile(szDest.c_str(),&wfd);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;
	do 
	{
		if(wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wfd.cFileName[0] != '.')
			{
				string sztmp = str + string(wfd.cFileName);
				if(CompareUserLogTime(sztmp,m_pConfigInfo->dwDay))
					veclst.push_back(sztmp);
			}
		}
		else
		{
		}
	} while(FindNextFile(hFile,&wfd));
	FindClose(hFile);
	return true;
}

bool CUDiskServerDlg::CompareUserLogTime(const string &sztmp,DWORD dwDay)
{
	 double dbSceond = dwDay*24*60*60;
	char* filename = PathFindFileName(sztmp.c_str());
	char filepath[MAX_PATH] = {0};
	sprintf_s(filepath,"%s\\%s.ini",sztmp.c_str(),filename);
	char szTime[64] = {0};
	GetPrivateProfileString("system","LogTime","",szTime,64,filepath);
	CTime t = CTime::GetCurrentTime();
	double  dftime = difftime(t.GetTime(),_atoi64(szTime));
	if(dftime )
	{
		return dftime > dbSceond;
	}

	return false;
}

void CUDiskServerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

BOOL CUDiskServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(   pMsg->message   ==   WM_KEYDOWN   &&   pMsg->wParam   ==   VK_ESCAPE   )   
		return   TRUE;  
	return CDialog::PreTranslateMessage(pMsg);
}

bool CUDiskServerDlg::Listen(void)
{

	SOCKET udpSock = socket(AF_INET,SOCK_DGRAM,0);
	if(udpSock == INVALID_SOCKET)
		return false;

	SOCKADDR_IN SerAddr = {0};
	SerAddr.sin_family = AF_INET;
	SerAddr.sin_port = htons(PORT);
	SerAddr.sin_addr.s_addr = ADDR_ANY;
	if(SOCKET_ERROR == bind(udpSock,(PSOCKADDR)&SerAddr,sizeof(SerAddr)))
	{
		closesocket(udpSock);
		return false;
	}

	WSAEVENT hNetEvent = WSACreateEvent();
	HANDLE hEvents[2] = {m_hExit,hNetEvent};

	int result = WSAEventSelect(udpSock,hNetEvent,FD_READ);
	if(result == SOCKET_ERROR)
	{
		WSACloseEvent(hNetEvent);
		return false;
	}
		WSANETWORKEVENTS NetworkEvents;	
	while(1)
	{

		DWORD EventCaused = WSAWaitForMultipleEvents(2,hEvents, FALSE,WSA_INFINITE,FALSE);
		if(EventCaused == WSA_WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			WSACloseEvent(hNetEvent);
			break;
		}
		result = WSAEnumNetworkEvents(udpSock,hNetEvent,&NetworkEvents);
		if(NetworkEvents.lNetworkEvents == FD_READ)
		{
			SOCKADDR_IN AddrPeer ={0};
			int len = sizeof(SOCKADDR_IN);
			char buf[1024] = {0};
			recvfrom(udpSock,buf,sizeof(buf),0,(PSOCKADDR)&AddrPeer,&len);
			if(((_packageheader*)buf)->Cmd ==0x500 && ((_packageheader*)buf)->StartFlag == 0x5e7d)
			{
				vector<string> veclst;
				GetIpAddr(veclst);
				char Data[2048] = {0};
				CPackageHelper inpack(Data);
				inpack.pushDWORD(veclst.size());
				for(size_t i =0; i<veclst.size();i++)
				{
					inpack.pushString(veclst[i].c_str(),veclst[i].size());
				}
				_packageheader header ={0};
				header.Cmd = 0x0500;
				header.StartFlag = 0x5e7d;
				header.Length = inpack.GetOffset();
				inpack.pushPackageHeader(header);
				sendto(udpSock,Data,header.Length,0,(PSOCKADDR)&AddrPeer,sizeof(SOCKADDR_IN));
			
			}

		}
	}
	closesocket(udpSock);

	return false;
}
static DllLoadLib DllLib("Iphlpapi.dll", "GetAdaptersInfo");
static PFNGETADAPTERSINFO pfnGetAdaptersInfo = (PFNGETADAPTERSINFO)DllLib.m_pProg;

string CUDiskServerDlg::GetIpAddr(vector<string>&veclst )
{
	string result("0.0.0.0");

	IP_ADAPTER_INFO adapter[16] = {0};
	DWORD dwSize = sizeof(adapter);

	if (pfnGetAdaptersInfo != NULL && 
		ERROR_SUCCESS != pfnGetAdaptersInfo(adapter, &dwSize))
	{
		return result;
	}

	PIP_ADAPTER_INFO pAdapter = adapter;
	while (pAdapter!= NULL)
	{
		int nMacZeroCount = 0;
		int idx=0;
		for (idx=0; idx<6; idx++)
		{
			if (0 == pAdapter->Address[idx])
				nMacZeroCount++;
		}
		sockaddr_in addr;
		addr.sin_addr.s_addr = inet_addr(pAdapter->IpAddressList.IpAddress.String);
		int nIpZeroCount = 0;
		if (addr.sin_addr.S_un.S_un_b.s_b1 == 0) nIpZeroCount++;
		if (addr.sin_addr.S_un.S_un_b.s_b2 == 0) nIpZeroCount++;
		if (addr.sin_addr.S_un.S_un_b.s_b3 == 0) nIpZeroCount++;
		if (addr.sin_addr.S_un.S_un_b.s_b4 == 0) nIpZeroCount++;

		if (nMacZeroCount <=3 && nIpZeroCount <= 2 && addr.sin_addr.S_un.S_un_b.s_b1 != 0)
		{
#define PRIVATE_IP1	"169.254"
			if (StrNCmp(pAdapter->IpAddressList.IpAddress.String, PRIVATE_IP1, lstrlen(PRIVATE_IP1)) != 0)
			{
				result = pAdapter->IpAddressList.IpAddress.String;
				//break;
				veclst.push_back(result);
			}
		}
		pAdapter = pAdapter->Next;
	}
	return result;
}
void CUDiskServerDlg::OnShow()
{
	ShowWindow(SW_SHOW); //显示窗体   
	Shell_NotifyIcon(NIM_DELETE, &m_nid); //删除系统托盘图标   
}

void CUDiskServerDlg::OnExit()
{
	if(MessageBox("是否退出程序","退出程序",MB_OKCANCEL) == IDCANCEL)
	{
		return ;
	}
	if(MessageBox("是否保存配置","退出程序",MB_OKCANCEL) == IDCANCEL)
	{

	}
	else
	{
		SaveConfig();
	}
	SetEvent(m_hExit);
	if(m_pNetLayer)
	{
		IMemPool* pIMemPool = m_pNetLayer->GetIMemPool();
		INetLayerEvent* pNetLayerEvent = m_pNetLayer->GetINetLayerEvent();
		if(m_pNetLayer)
			m_pNetLayer->Stop();
		if(pIMemPool)
			pIMemPool->Release();
		if(pNetLayerEvent)
			pNetLayerEvent->Release();
	}
	WaitForSingleObject(m_hTread,3000);
	if(m_hExit)
	{
		CloseHandle(m_hExit);
		m_hExit = NULL;
	}
	Shell_NotifyIcon(NIM_DELETE, &m_nid); 
	PostQuitMessage(0);
}
