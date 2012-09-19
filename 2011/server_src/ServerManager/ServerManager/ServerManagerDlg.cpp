
// ServerManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include "VDiskConfigDlg.h"

#include "../../../include/AsyncIO/iocp/Dispatcher.hpp"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


enum 
{ 
	WM_USER_TRAY_ICON = WM_USER + 0x100,

	WM_USER_STOP_SERVICE,
	WM_USER_START_SERVICE,
	WM_USER_UPDATE_LIST
};


enum 
{
	MSG_OK,
	MSG_ERROR
};


template < typename Handler >
void AsyncRun(const Handler &handler)
{
	using namespace async::iocp;

	static IODispatcher io(1);

	io.Post(handler);
}



struct StopHelper
{
	mgr::ServiceMgr &svrMgr_;
	CWnd *wnd_;

	StopHelper(mgr::ServiceMgr &svrMgr, CWnd *wnd)
		: svrMgr_(svrMgr)
		, wnd_(wnd)
	{}

	void operator()(size_t, size_t)
	{
		if( svrMgr_.IsManService() )
		{
			if( !svrMgr_.StopMainService() )
			{
				wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_ERROR, (LPARAM)_T("停止I8DeskSvr失败"));
				return;
			}
			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(0, 1), (LPARAM)_T("未启动"));
			
			if( !svrMgr_.StopVDiskService() )
			{
				wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_ERROR, (LPARAM)_T("停止I8VDiskSvr失败"));
				return;
			}
			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(1, 1), (LPARAM)_T("未启动"));

			wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_OK);
		}
		else if( svrMgr_.IsVDiskService() )
		{
			if( !svrMgr_.StopVDiskService() )
			{
				wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_ERROR, (LPARAM)_T("停止I8VDiskSvr失败"));
				return;
			}

			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(0, 1), (LPARAM)_T("未启动"));

			wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_OK);
		}
		else
		{
			wnd_->PostMessage(WM_USER_STOP_SERVICE, MSG_ERROR, (LPARAM)_T("没有安装任何I8相关服务"));
		}
	}
};

struct StartHelper
{
	mgr::ServiceMgr &svrMgr_;
	CWnd *wnd_;

	StartHelper(mgr::ServiceMgr &svrMgr, CWnd *wnd)
		: svrMgr_(svrMgr)
		, wnd_(wnd)
	{}

	void operator()(size_t, size_t)
	{
		if( svrMgr_.IsManService() )
		{
			if( !svrMgr_.RunMainService() )
			{
				wnd_->PostMessage(WM_USER_START_SERVICE, MSG_ERROR, (LPARAM)_T("启动I8DeskSvr失败"));
				return;
			}
			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(0, 1), (LPARAM)_T("启动"));

			if( !svrMgr_.RunVDiskService() )
			{
				wnd_->PostMessage(WM_USER_START_SERVICE, MSG_ERROR, (LPARAM)_T("启动I8VDiskSvr失败"));
				return;
			}
			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(1, 1), (LPARAM)_T("启动"));

			wnd_->PostMessage(WM_USER_START_SERVICE, MSG_OK);
		}
		else if( svrMgr_.IsVDiskService() )
		{
			if( !svrMgr_.RunVDiskService() )
			{
				wnd_->PostMessage(WM_USER_START_SERVICE, MSG_ERROR, (LPARAM)_T("启动I8VDiskSvr失败"));
				return;
			}
			wnd_->PostMessage(WM_USER_UPDATE_LIST, MAKELONG(0, 1), (LPARAM)_T("启动"));

			wnd_->PostMessage(WM_USER_START_SERVICE, MSG_OK);
		}
		else
		{
			wnd_->PostMessage(WM_USER_START_SERVICE, MSG_ERROR, (LPARAM)_T("没有安装任何I8相关服务"));
		}

	}
};


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CServerManagerDlg 对话框




CServerManagerDlg::CServerManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerManagerDlg::IDD, pParent)
	, TrayIconImpl(WM_USER_TRAY_ICON)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVICE_STATUS, wndSvrs_);
}

BEGIN_MESSAGE_MAP(CServerManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_ALL, &CServerManagerDlg::OnBnClickedButtonStartAll)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL, &CServerManagerDlg::OnBnClickedButtonStopAll)
	ON_MESSAGE(WM_USER_TRAY_ICON, &CServerManagerDlg::OnTrayIconMessage)
	ON_COMMAND(ID_TRAYICON_START_ALL, &CServerManagerDlg::OnTrayiconStartAll)
	ON_COMMAND(ID_TRAYICON_STOP_ALL, &CServerManagerDlg::OnTrayiconStopAll)
	ON_COMMAND(ID_TRAYICON_CONFIG, &CServerManagerDlg::OnTrayiconConfig)
	ON_COMMAND(ID_TRAYICON_OPEN, &CServerManagerDlg::OnTrayiconOpen)
	ON_COMMAND(ID_TRAYICON_EXIT, &CServerManagerDlg::OnTrayiconExit)

	ON_MESSAGE(WM_USER_START_SERVICE, &CServerManagerDlg::OnStartSvr)
	ON_MESSAGE(WM_USER_STOP_SERVICE, &CServerManagerDlg::OnStopSvr)
	ON_MESSAGE(WM_USER_UPDATE_LIST, &CServerManagerDlg::OnUpdateList)
END_MESSAGE_MAP()


// CServerManagerDlg 消息处理程序

BOOL CServerManagerDlg::OnInitDialog()
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

	statusIcon_[0] = AfxGetApp()->LoadIcon(IDI_ICON_RUNNING);
	statusIcon_[1] = AfxGetApp()->LoadIcon(IDI_ICON_NO_RUNNING);
	
	InstallIcon(_T("服务管理器"), m_hIcon, IDR_TRAYICON);


	// List 
	wndSvrs_.InsertColumn(0, _T("服务名称"), LVCFMT_LEFT, 80);
	wndSvrs_.InsertColumn(1, _T("状态"), LVCFMT_LEFT, 50);

	bool isRun = false;
	if( svrMgr_.IsManService() )
	{
		isRun = svrMgr_.IsRunMainService();
		int index = wndSvrs_.InsertItem(0, _T("I8DeskSvr"));
		wndSvrs_.SetItemText(index, 1, isRun ? _T("启动") : _T("未启动"));

		isRun = svrMgr_.IsRunVDiskService();
		index = wndSvrs_.InsertItem(0, _T("I8VDiskSvr"));
		wndSvrs_.SetItemText(index, 1, isRun ? _T("启动") : _T("未启动"));
	}
	else if( svrMgr_.IsVDiskService() )
	{
		isRun = svrMgr_.IsRunVDiskService();
		int index = wndSvrs_.InsertItem(0, _T("I8VDiskSvr"));
		wndSvrs_.SetItemText(index, 1, isRun ? _T("启动") : _T("未启动"));
	}

	ChangeIcon(isRun ? statusIcon_[0] : statusIcon_[1]);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerManagerDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}



LRESULT CServerManagerDlg::OnTrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	OnTrayIcon(WM_USER_TRAY_ICON, wParam, lParam);
	return TRUE;
}

void CServerManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerManagerDlg::OnPaint()
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
HCURSOR CServerManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerManagerDlg::OnBnClickedButtonStartAll()
{
	AsyncRun(StartHelper(svrMgr_, this));
}




void CServerManagerDlg::OnBnClickedButtonStopAll()
{
	AsyncRun(StopHelper(svrMgr_, this));
}

void CServerManagerDlg::OnTrayiconStartAll()
{
	OnBnClickedButtonStartAll();
}

void CServerManagerDlg::OnTrayiconStopAll()
{
	OnBnClickedButtonStopAll();
}

void CServerManagerDlg::OnTrayiconConfig()
{
	if( svrMgr_.IsManService() )
	{
		svrMgr_.RunMainConfig();
	}
	else if( svrMgr_.IsVDiskService() )
	{
		CVDiskConfigDlg vDiskDlg;
		svrMgr_.RunVDiskConfig(std::tr1::bind(
			&CVDiskConfigDlg::ConfigIP, &vDiskDlg, std::tr1::placeholders::_1));

	}
	else
	{
		AfxMessageBox(_T("没有安装任何I8相关服务"));
	}
}

void CServerManagerDlg::OnTrayiconOpen()
{
	ShowWindow(SW_SHOW);
}

void CServerManagerDlg::OnTrayiconExit()
{
	PostQuitMessage(0);
}



LRESULT CServerManagerDlg::OnStartSvr(WPARAM wParam, LPARAM lParam)
{
	if( wParam == MSG_OK )
	{
		ChangeIcon(statusIcon_[0]);
	}
	if( wParam == MSG_ERROR )
	{
		ChangeIcon(statusIcon_[1]);
		CString msg((TCHAR *)lParam);
		AfxMessageBox(msg);
	}

	return TRUE;
}

LRESULT CServerManagerDlg::OnStopSvr(WPARAM wParam, LPARAM lParam)
{
	if( wParam == MSG_OK )
	{
		ChangeIcon(statusIcon_[1]);
	}
	else if( wParam == MSG_ERROR )
	{
		CString msg((TCHAR *)lParam);
		AfxMessageBox(msg);
	}

	return TRUE;
}

LRESULT CServerManagerDlg::OnUpdateList(WPARAM wParam, LPARAM lParam)
{
	size_t row = LOWORD(wParam);
	size_t col = HIWORD(wParam);

	TCHAR *text = (TCHAR *)lParam;

	wndSvrs_.SetItemText(row, col, text);

	return TRUE;
}