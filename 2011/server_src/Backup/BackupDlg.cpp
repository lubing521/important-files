// BackupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Backup.h"
#include "BackupDlg.h"

#include <process.h>
#include <comdef.h>
#include <Shlwapi.h>
#include <WinSvc.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_PROGRESS WM_USER + 0x1000

enum e_Progress 
{
	E_START,
	E_STOP_SERVICE,
	E_EXE_BACKUP,
	E_EXE_RESTORE,
	E_START_SERVICE,
	E_FINISH
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


CBackupDlg::CBackupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBackupDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThread = NULL;
}

void CBackupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BACKUP, m_lstBackup);
	DDX_Control(pDX, IDC_BACKUP_DATE, m_dtBackup);
	DDX_Control(pDX, IDC_CHECK_BYDAY, m_btnByDay);
	DDX_Control(pDX, IDC_PROGRESS1, m_btnProgress);
}

BEGIN_MESSAGE_MAP(CBackupDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, &CBackupDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_CHECK_BYDAY, &CBackupDlg::OnBnClickedCheckByday)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXEC_BACKUP, &CBackupDlg::OnBnClickedExecBackup)
	ON_BN_CLICKED(IDC_EXEC_RESTORE, &CBackupDlg::OnBnClickedExecRestore)
	ON_BN_CLICKED(IDC_DELETE_BACKUP, &CBackupDlg::OnBnClickedDeleteBackup)
	ON_EN_CHANGE(IDC_BACKUP_DIR, &CBackupDlg::OnEnChangeBackupDir)
	ON_BN_CLICKED(IDC_SAVE_SET, &CBackupDlg::OnBnClickedSaveSet)
	ON_MESSAGE(WM_PROGRESS, &CBackupDlg::OnProgress)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CBackupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szExe[MAX_PATH] = {0};
	GetModuleFileName(NULL, szExe, MAX_PATH);
	PathRemoveFileSpec(szExe);
	PathAddBackslash(szExe);
	lstrcat(szExe, TEXT("I8DeskSvr.exe"));
	if (!PathFileExists(szExe))
	{
		AfxMessageBox(TEXT("备份工具只能在安装目录下，才能正确运行."));
		PostQuitMessage(0);
		return TRUE;
	}

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);


	((CEdit*)GetDlgItem(IDC_BACKUP_NUM))->LimitText(2);
	SetDlgItemText(IDC_BACKUP_DIR, GetBackupOption("backupdir"));
	SetDlgItemText(IDC_BACKUP_NUM, GetBackupOption("backupnum"));
	CString szDate = (LPCSTR)GetBackupOption("backupdate");
	if (szDate.IsEmpty())
	{
		m_dtBackup.EnableWindow(FALSE);
	}
	else
	{
		m_btnByDay.SetCheck(BST_CHECKED);
	}
	{
		int h = 0, m = 0;
		CString item;
		AfxExtractSubString(item, szDate, 0, ':');
		h = atoi(item);
		AfxExtractSubString(item, szDate, 1, ':');
		m = atoi(item);

		CTime tm(2009, 1, 1, h, m, 0);
		m_dtBackup.SetTime(&tm);		
	}

	m_lstBackup.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstBackup.InsertColumn(0, "文件名",	LVCFMT_LEFT, 240);
	m_lstBackup.InsertColumn(1, "备份时间", LVCFMT_LEFT, 180);

	RefreshBackListFile();

	return TRUE;
}

void CBackupDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBackupDlg::OnPaint()
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

HCURSOR CBackupDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static char s_buf[MAX_PATH] = {0};	//保存浏览后的目录.

static int   CALLBACK   SetSelect_CB(HWND   win,   UINT   msg,   LPARAM   param,   LPARAM   data)   
{   
	if (msg == BFFM_INITIALIZED)
	{
		::SendMessage(win, BFFM_SETSELECTION, TRUE, (LPARAM)s_buf);
	}
	return 0;
}   

void CBackupDlg::OnBnClickedBrowse()
{
	BROWSEINFO info = {0};
	info.hwndOwner = m_hWnd;
	info.lpszTitle = "设备备份目录:";

	CString xPath;
	GetDlgItemText(IDC_BACKUP_DIR, xPath);
	if (!xPath.IsEmpty())
		lstrcpy(s_buf, xPath);
	info.lpfn = SetSelect_CB;

	LPITEMIDLIST lst = SHBrowseForFolder(&info);
	if (lst != NULL)
	{
		SHGetPathFromIDList(lst, s_buf);
		LPMALLOC pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(lst);
		pMalloc->Release();
		PathAddBackslash(s_buf);
		SetDlgItemText(IDC_BACKUP_DIR, s_buf);
	}
}

void CBackupDlg::OnBnClickedCheckByday()
{
	GetDlgItem(IDC_BACKUP_DATE)->EnableWindow(m_btnByDay.GetCheck());
}

void CBackupDlg::OnClose()
{
	if (m_hThread != NULL)
	{
		AfxMessageBox("操作未完成，不能退出.");
		return ;
	}
	CDialog::OnClose();
}

CString GetConfigFileName()
{
	char file[MAX_PATH] = {0};
	GetModuleFileName(NULL, file, MAX_PATH);
	PathRemoveFileSpec(file);
	PathAddBackslash(file);
	lstrcat(file, "Data\\Backup.ini");
	return CString(file);
}

CString CBackupDlg::GetBackupOption(CString szKey)
{
	char value[MAX_PATH] = {0};
	GetPrivateProfileString("System", szKey, "", value, MAX_PATH, GetConfigFileName());	
	return CString(value);
}

void CBackupDlg::SetBackupOption(CString szKey, CString szValue)
{
	WritePrivateProfileString("System", szKey, szValue, GetConfigFileName());
}

void CBackupDlg::RefreshBackListFile()
{
	CString dir;
	GetDlgItemText(IDC_BACKUP_DIR, dir);
	if (dir.IsEmpty())
		return ;

	m_lstBackup.DeleteAllItems();

	if (dir.Right(1) != "\\")
		dir += "\\";
	dir += "*";
	CFileFind finder;
	BOOL bWorker = finder.FindFile(dir);
	while (bWorker)
	{
		bWorker = finder.FindNextFile();
		
		if (!finder.IsDots() && !finder.IsDirectory() && finder.GetFileName().GetLength() == 31)
		{
			CString file = finder.GetFileName();
			if (file.Right(4).CompareNoCase(".zip") != 0)
				continue;

			if (file.Left(13).CompareNoCase("I8DeskBackup-") != 0)
				continue;

			int nIdx = m_lstBackup.InsertItem(m_lstBackup.GetItemCount(), finder.GetFileName());
			file = file.Mid(13, 14);
			file.Insert(4, '-');	file.Insert(7, '-');
			file.Insert(10, ' ');
			file.Insert(13, ':');	file.Insert(16, ':');
			m_lstBackup.SetItemText(nIdx, 1, file);
		}
	}
	finder.Close();
}

bool CBackupDlg::SaveData()
{
	char buf[MAX_PATH] = {0};
	GetDlgItemText(IDC_BACKUP_DIR, buf, MAX_PATH);
	if (lstrlen(buf) == 0)
	{
		AfxMessageBox("请设置备份目录.");
		GetDlgItem(IDC_BACKUP_DIR)->SetFocus();
		return false;
	}
	if (GetDlgItemInt(IDC_BACKUP_NUM) == 0)
	{
		AfxMessageBox("备份保留个数不能设置为0.");
		((CEdit*)GetDlgItem(IDC_BACKUP_NUM))->SetSel(0, 1000);
		GetDlgItem(IDC_BACKUP_NUM)->SetFocus();
		return false;
	}
	CString szValue;
	szValue.Format("%d", GetDlgItemInt(IDC_BACKUP_NUM));
	SetBackupOption("backupdir", buf);
	SetBackupOption("backupnum", szValue);

	if (m_btnByDay.GetCheck())
	{
		CTime dt;
		m_dtBackup.GetTime(dt);
		sprintf_s(buf, "%02d:%02d", dt.GetHour(), dt.GetMinute());
	}
	else
	{
		buf[0] = 0;
	}
	SetBackupOption("backupdate", buf);
	return true;
}

void CBackupDlg::OnBnClickedExecBackup()
{
	if (!SaveData())
		return ;

	m_bIsBackup = TRUE;
	GetDlgItemText(IDC_BACKUP_DIR, m_strBackDir);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, this, 0, NULL);
	if (m_hThread == NULL)
	{
		AfxMessageBox("创建线程失败.");
	}
}

void CBackupDlg::OnBnClickedExecRestore()
{
	if (!SaveData())
		return ;

	int nSel = m_lstBackup.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox("请选择要恢复的备份文件.");
		return ;
	}
	GetDlgItemText(IDC_BACKUP_DIR, m_strBackFile);
	if (m_strBackFile.IsEmpty())
	{
		AfxMessageBox("备份目录不能为空.");
		return ;
	}
	if (m_strBackFile.Right(1) != "\\")
		m_strBackFile += "\\";
	m_strBackFile += m_lstBackup.GetItemText(nSel, 0);
	
	m_bIsBackup = FALSE;	
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, this, 0, NULL);
	if (m_hThread == NULL)
	{
		AfxMessageBox("创建线程失败.");
	}
}

void CBackupDlg::OnBnClickedDeleteBackup()
{
	int nSel = m_lstBackup.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox("请选择要删除的备份文件.");
		return ;
	}

	char file[MAX_PATH] = {0};
	GetDlgItemText(IDC_BACKUP_DIR, file, MAX_PATH);
	PathAddBackslash(file);
	lstrcat(file, m_lstBackup.GetItemText(nSel, 0));
	if (!DeleteFile(file))
	{
		AfxMessageBox("删除备份文件失败.");
		return ;
	}
	m_lstBackup.DeleteItem(nSel);
}

void CBackupDlg::OnEnChangeBackupDir()
{
	RefreshBackListFile();
}

void CBackupDlg::OnBnClickedSaveSet()
{
	SaveData();
}

LRESULT CBackupDlg::OnProgress(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case E_START:
		{
			GetDlgItem(IDC_EXEC_BACKUP)->EnableWindow(FALSE);
			GetDlgItem(IDC_EXEC_RESTORE)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_BACKUP)->EnableWindow(FALSE);
		}
		break;
	case E_STOP_SERVICE:
		{
			SetDlgItemText(IDC_STATUS, "开始停止服务:");
			SetTimer(1, 1000, NULL);
			m_btnProgress.SetRange32(0, 100);
			m_btnProgress.SetPos(0);
		}
		break;
	case E_EXE_BACKUP:
		{
			KillTimer(1);
			m_btnProgress.SetPos(0);
			SetDlgItemText(IDC_STATUS, "开始执行备份:");
			SetTimer(1, 1000, NULL);
		}
		break;
	case E_EXE_RESTORE:
		{
			KillTimer(1);
			m_btnProgress.SetPos(0);
			SetDlgItemText(IDC_STATUS, "开始恢复备份:");
			SetTimer(1, 1000, NULL);
		}
		break;
	case E_START_SERVICE:
		{
			KillTimer(1);
			m_btnProgress.SetPos(0);
			SetDlgItemText(IDC_STATUS, "重启服务:");
			SetTimer(1, 1000, NULL);
		}
		break;
	case E_FINISH:
		{
			KillTimer(1);
			GetDlgItem(IDC_EXEC_BACKUP)->EnableWindow(TRUE);
			GetDlgItem(IDC_EXEC_RESTORE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_BACKUP)->EnableWindow(TRUE);
			SetDlgItemText(IDC_STATUS, "操作完成:");
			CloseHandle(m_hThread);
			m_hThread = NULL;
			RefreshBackListFile();
		}
		break;
	}	
	return 0;
}

bool CBackupDlg::StopI8DeskService()
{
	//stop service.
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCM != NULL)
	{
		SC_HANDLE hService = OpenService(hSCM, "I8DeskSvr", SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
		if (hService != NULL)
		{
			SERVICE_STATUS ss;
			ss.dwWaitHint = 1000;
			ControlService( hService,	SERVICE_CONTROL_STOP, &ss);
			while( ::QueryServiceStatus( hService, &ss) == TRUE)
			{
				::Sleep( ss.dwWaitHint);
				if( ss.dwCurrentState == SERVICE_STOPPED)
					break;
			}
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hSCM);
	}

	//kill process.
	DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
	char path[MAX_PATH] = {0};
	if (EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
	{
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
			if (hProcess != NULL)
			{
				GetModuleBaseName(hProcess, NULL, path, sizeof(path));
				if (lstrcmpi(path, "I8DeskSvr.exe") == 0)
				{
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
					break;
				}
				CloseHandle(hProcess);
			}
		}
	}
	Sleep(2000);
	return true;
}

bool CBackupDlg::StartI8DeskService()
{
	//启动服务
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCM != NULL)
	{
		SC_HANDLE hService = OpenService(hSCM, "I8DeskSvr", SERVICE_START);
		if (hService != NULL)
		{
			SERVICE_STATUS ss;
			ss.dwWaitHint = 1000;
			StartService(hService, 0, NULL);
			while( ::QueryServiceStatus( hService, &ss) == TRUE)
			{
				::Sleep( ss.dwWaitHint);
				if( ss.dwCurrentState == SERVICE_RUNNING)
					break;
			}
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hSCM);
	}

	//启动进程.
	bool bRunning = false;
	DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
	char path[MAX_PATH] = {0};
	if (EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
	{
		cProcesses = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < cProcesses; i++ )
		{
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, aProcesses[i]);
			if (hProcess != NULL)
			{
				GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
				if (lstrcmpi(path, "I8DeskSvr.exe") == 0)
				{
					CloseHandle(hProcess);
					bRunning = true;
					break;
				}
				CloseHandle(hProcess);
			}
		}
	}
	if (!bRunning)
	{
		TCHAR szExe[MAX_PATH] = {0};
		szExe[0] = '\"';
		GetModuleFileName(NULL, &szExe[1], MAX_PATH);
		PathRemoveFileSpec(szExe);
		PathAddBackslash(szExe);
		lstrcat(szExe, "I8DeskSvr.exe\" -debug");
		STARTUPINFO			si = {sizeof(si)};
		PROCESS_INFORMATION pi = {0};
		if (CreateProcess(NULL, szExe, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}
	}
	return true;
}

UINT __stdcall CBackupDlg::WorkThreadProc(LPVOID lpVoid)
{
	CBackupDlg* pThis = reinterpret_cast<CBackupDlg*>(lpVoid);

	::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_START, 0);
	{
		::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_STOP_SERVICE, 0);
		if (pThis->StopI8DeskService())
		{			
			if (pThis->m_bIsBackup)
			{
				::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_EXE_BACKUP, 0);
				if (pThis->_ExecBackup())
				{
					CString szLastBackupDate;
					szLastBackupDate.Format(TEXT("%d"), _time32(NULL));
					pThis->SetBackupOption(TEXT("LastBackupDate"), szLastBackupDate);
				}
			}
			else
			{
				::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_EXE_RESTORE, 0);
				pThis->_ExecRestore();
			}
			::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_START_SERVICE, 0);
			pThis->StartI8DeskService();
		}
	}
	::PostMessage(pThis->m_hWnd, WM_PROGRESS, E_FINISH, 0);
	return 0;
}

void CBackupDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_btnProgress.SetPos(m_btnProgress.GetPos()+1);

	CDialog::OnTimer(nIDEvent);
}

bool CBackupDlg::_ExecBackup()
{
	if (m_strBackDir.Right(1) != "\\")
		m_strBackDir += "\\";
	SHCreateDirectory(NULL, _bstr_t((LPCTSTR)m_strBackDir));

	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	char backupfile[MAX_PATH] = {0};
	sprintf_s(backupfile, "%sI8DeskBackup-%04d%02d%02d%02d%02d%02d.zip", 
		m_strBackDir, st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);

	CString strDatadir = GetDataDir();
	HZIP hZip = CreateZip(backupfile, 0, ZIP_FILENAME);
	if (hZip == 0)
		return false;

	if (!_ZipAllData(hZip, strDatadir, strDatadir.GetLength()))
	{
		CloseZip(hZip);
		return false;
	}
	CloseZip(hZip);

	return true;
}

bool CBackupDlg::_ExecRestore()
{
	CString szDataDir = GetDataDir();
	SetCurrentDirectory(szDataDir);

	HZIP hZip = OpenZip((void*)(LPCSTR)m_strBackFile, 0, ZIP_FILENAME);
	if (hZip == 0)
	{
		AfxMessageBox("打开压缩包文件出错.");
		return false;
	}

	bool result = true;
	ZIPENTRY ze; 
	GetZipItem(hZip, -1, &ze); 
	int numitems = ze.index;
	for (int i=0; i<numitems; i++)
	{
		GetZipItem(hZip,i,&ze);
		ZRESULT zr = UnzipItem(hZip,i,ze.name,0,ZIP_FILENAME);
		if (zr != ZR_OK)
		{
			CString msg;
			msg = "解压文件出错:";
			msg += ze.name;
			AfxMessageBox(msg);
			result = false;
		}
	}
	CloseZip(hZip);
	return result;
}

bool CBackupDlg::_ZipAllData(HZIP hZip, LPCSTR szDir, int nBase)
{
	bool ret = true;
	char path[MAX_PATH] = {0};
	sprintf_s(path, "%s*", szDir);
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(path, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return false;

	while (FindNextFile(hFinder, &wfd) != 0)
	{
		if (lstrcmp(wfd.cFileName, ".") == 0 || lstrcmp(wfd.cFileName, "..") == 0)
			continue ;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			sprintf_s(path, "%s%s", szDir, wfd.cFileName);
			ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FOLDER);
			if (zr != ZR_OK)
			{
				CString msg = "压缩文件出错:";
				msg += &path[nBase];
				msg += ":";
				char log[1024] = {0};
				FormatZipMessage(zr, log, sizeof(log));
				msg += log;

				AfxMessageBox(msg);
				return false;
			}
			lstrcat(path, "\\");
			ret = _ZipAllData(hZip, path, nBase);
		}
		else
		{
			sprintf_s(path, "%s%s", szDir, wfd.cFileName);
			ZRESULT zr = ZipAdd(hZip, &path[nBase], path, 0, ZIP_FILENAME);
			if (zr != ZR_OK)
			{
				CString msg = "压缩文件出错:";
				msg += &path[nBase];
				msg += ":";
				char log[1024] = {0};
				FormatZipMessage(zr, log, sizeof(log));
				msg += log;
				AfxMessageBox(msg);
				return false;
			}
		}
	}
	FindClose(hFinder);
	return ret;
}

CString CBackupDlg::GetDataDir()
{
	TCHAR szDir[MAX_PATH] = {0};
	GetModuleFileName(NULL, szDir, MAX_PATH);
	PathRemoveFileSpec(szDir);
	PathAddBackslash(szDir);
	lstrcat(szDir, TEXT("Data\\"));
	return CString(szDir);
}