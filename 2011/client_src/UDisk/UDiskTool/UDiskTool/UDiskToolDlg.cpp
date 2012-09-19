
// UDiskToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskTool.h"
#include "UDiskToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#define ERR_OPENDATABASE	TEXT("数据库连接己经断开")
#define  WM_PROGRESS     WM_USER +0x0100

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


// CUDiskToolDlg 对话框




CUDiskToolDlg::CUDiskToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDiskToolDlg::IDD, pParent),m_hread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pConn.CreateInstance(__uuidof(Connection));
	m_hExit= CreateEvent(NULL,TRUE,FALSE,NULL);
}

void CUDiskToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}

BEGIN_MESSAGE_MAP(CUDiskToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CUDiskToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CUDiskToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUDiskToolDlg::OnBnClickedButton3)
	ON_MESSAGE(WM_PROGRESS,OnProgress)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUDiskToolDlg 消息处理程序

BOOL CUDiskToolDlg::OnInitDialog()
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

	ShowWindow(SW_SHOW);

	// TODO: 在此添加额外的初始化代码
	char szpath[MAX_PATH] = {0};
	GetModuleFileName(NULL,szpath,MAX_PATH);
	PathRemoveFileSpec(szpath);
	PathRemoveFileSpec(szpath);
	CString strDB = CString(szpath)+CString("\\data\\i8desk.mdb");
	SetDlgItemText(IDC_EDIT3,strDB);
	CString strDest = CString(szpath)+CString("\\u盘存档\\");
	SetDlgItemText(IDC_EDIT1,strDest);

	m_Progress.SetRange(0,100);
	m_Progress.SetStep(10);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUDiskToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUDiskToolDlg::OnPaint()
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
HCURSOR CUDiskToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUDiskToolDlg::OnBnClickedButton1()
{
	GetDlgItemText(IDC_EDIT1,m_szDestPath);
	if(m_szDestPath.IsEmpty())
	{
		AfxMessageBox("目的目录未设置！");
		return ;
	}
	CString szpath;
	GetDlgItemText(IDC_EDIT3,szpath);

// 	GetModuleFileName(NULL,szpath,MAX_PATH);
// 	PathRemoveFileSpec(szpath);
// 	PathRemoveFileSpec(szpath);
// 	lstrcat(szpath,"\\data\\i8desk.mdb");
	if(!PathFileExists(szpath))
	{
		AfxMessageBox("没有找到数据库文件，请确认此工具放在I8安装目录的TOOLS目录下！");
		return ;
	}
	CString strDb;
	strDb.Format("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s",szpath);
	if(!Open(strDb,"",""))
	{
		AfxMessageBox(m_szErrInfo);
		return;
	}

	CString szSql("select OptValue from tSysOpt where OptName =\'muserdatadir\'");

	_RecordsetPtr prcd;
	if (ExecSql(szSql, prcd))
	{
		if (prcd->adoEOF != VARIANT_TRUE)
		{
			_variant_t var = prcd->GetCollect(0L);
			prcd->Close();

			if (var.vt != VT_NULL)
			{
				m_szSorcePath  = (LPCSTR)(_bstr_t)(var);
			}
			prcd.Release();
		}
	}
	
	if(m_szSorcePath.GetAt(m_szSorcePath.GetLength())!='\\')
	{
		m_szSorcePath+="\\";
	}
	m_hread = (HANDLE)_beginthreadex(NULL,0,WorkThread,this,0,NULL);


}
bool CUDiskToolDlg::Open(LPCTSTR lpszConnString, LPCTSTR lpszUser, LPCTSTR lpszPassword)
{

	try
	{
		if(m_pConn->State == adStateOpen)
		{
			m_pConn->Close();
		}
		m_pConn->CursorLocation = adUseClient;
		HRESULT hr = m_pConn->Open(_bstr_t(lpszConnString), _bstr_t(lpszUser), 
			_bstr_t(lpszPassword), adModeUnknown);
		if(SUCCEEDED(hr))
		{
			if (m_pConn->State == adStateOpen)
				return true;
		}
	
		return false;
	}
	catch (_com_error& e)
	{
		if (m_pConn->Errors->GetItem(0)->GetNativeError())
			m_hErrCode = m_pConn->Errors->GetItem(0)->GetNativeError();
		else
			m_hErrCode = GetLastError();
		lstrcpy(m_szErrInfo,  (LPCTSTR)e.Description());
		return false;
	}
	return true;
}
 bool CUDiskToolDlg::ExecSql(LPCTSTR lpszSql, _RecordsetPtr& prcd)
{
	
	if (m_pConn == NULL || m_pConn->State != adStateOpen)
	{		
		lstrcpy(m_szErrInfo, ERR_OPENDATABASE);
		return false;
	}

	try
	{
		prcd.CreateInstance(__uuidof(Recordset));
		HRESULT hr = prcd->Open(_bstr_t(lpszSql), m_pConn.GetInterfacePtr(), 
			adOpenForwardOnly, adLockReadOnly, adCmdText);
		return SUCCEEDED(hr);
	}
	catch (_com_error& e)
	{
		if (m_pConn->Errors->GetItem(0)->GetNativeError())
			m_hErrCode = m_pConn->Errors->GetItem(0)->GetNativeError();
		else
			m_hErrCode = GetLastError();
		lstrcpy(m_szErrInfo,  e.Description());
		try
		{
			if (prcd != NULL && prcd->State == adStateOpen)
				prcd->Close();
			if (prcd != NULL)
				prcd.Release();
		}
		catch (...) {	}
		return false;
	}

	return true;
}
 void CUDiskToolDlg::OnBnClickedButton2()
 {
	 BROWSEINFO bi;
	 char szDisplayname[MAX_PATH] = {0};
	 LPITEMIDLIST     pidl; 
	 LPMALLOC     pMalloc = NULL;  
	 ZeroMemory(&bi, sizeof(bi));      
	 bi.hwndOwner =GetSafeHwnd(); 
	 bi.pszDisplayName = szDisplayname; 
	 bi.lpszTitle = TEXT("请选择一个文件夹:"); 
	 bi.ulFlags = BIF_RETURNONLYFSDIRS;  
	 pidl = SHBrowseForFolder(&bi); 
	 if (!pidl) 
	 { 
		 return;
	 }
	 SHGetPathFromIDList(pidl, szDisplayname); 
	 lstrcat(szDisplayname,"\\");
	 SetDlgItemText(IDC_EDIT1,szDisplayname);
 }
 bool CUDiskToolDlg::ZipFile(LPCSTR szSrc,LPCSTR szDest,LPCSTR zipName)
 {

	 char fileName[_MAX_FNAME];
	 _splitpath_s(szSrc, NULL, 0, NULL, 0, fileName, _MAX_FNAME, NULL, 0);// 得到文件名
	 char buf[MAX_PATH] = {0};
	 sprintf_s(buf,"%s%s.zip",szDest,(zipName == NULL?fileName:zipName));
	 HZIP hz=  CreateZip(buf,0,ZIP_FILENAME);
	ZRESULT ok ;

	 if(PathIsDirectory(szSrc))
	 {
		 vector<CString> vecLst;
		 ScanFile(szSrc,string(szSrc).size(),vecLst);
		 for(size_t i = 0; i<vecLst.size(); i++)
		 {
			 string str = szSrc + vecLst[i];
			 if( ZipAdd(hz,vecLst[i],(LPVOID)str.c_str(),0,ZIP_FILENAME) != ZR_OK)
			 {
				 CloseZip(hz);
				 return false;
			 }
		 }
	 }
	 else
	 {
		ok = ZipAdd(hz,PathFindFileName(szSrc),(LPVOID)szSrc,0,ZIP_FILENAME);
	 }
	 CloseZip(hz);
	 return ok == ZR_OK;
 }
 bool CUDiskToolDlg::ScanDirtory(LPCSTR szDir,int nDir,vector<CString>&veclst)
 {
	 string szDest(szDir);
	 if(*szDest.rbegin() !='\\')szDest+="\\";
	 szDest+= "*.*";
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
				 veclst.push_back(wfd.cFileName);
			 }
		 }
		 else
		 {
		 }
	 } while(FindNextFile(hFile,&wfd));
	 FindClose(hFile);
	 return true;
 }
 bool CUDiskToolDlg::ScanFile(LPCSTR szfile,int nDir,vector<CString>& vecLst)
 {
	 string szDest(szfile);
	 if(*szDest.rbegin() !='\\')szDest+="\\";
	 szDest+= "*.*";
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
				 string sztmp = szfile + string(wfd.cFileName)+"\\";
				 ScanFile(sztmp.c_str(),nDir,vecLst);
			 }

		 }
		 else
		 {
			 string strtmp = szfile +string(wfd.cFileName);
			 vecLst.push_back(strtmp.substr(nDir).c_str());

		 }
	 } while(FindNextFile(hFile,&wfd));
	 FindClose(hFile);
	 return true;

 }
 void CUDiskToolDlg::ConvertFileToDst(const _variant_t& name, const _variant_t& password, const _variant_t& logTime)
{
	CString UserName = (LPCSTR)(_bstr_t)name;
	CString szpassword = (LPCSTR)(_bstr_t)password;
	CString dwTime = (LPCSTR)(_bstr_t)(logTime);

	CString szDest = m_szDestPath +UserName +"\\";
	char Inifile[MAX_PATH] = {0};
	sprintf_s(Inifile,"%s%s.ini",szDest,UserName);
	MakeSureDirectoryPathExists(szDest);
	WritePrivateProfileString("system","password",szpassword,Inifile);
	WritePrivateProfileString("system","LogTime",dwTime,Inifile);
	szDest +="u盘\\";

	MakeSureDirectoryPathExists(szDest);
	vector<CString>veclst;
	CString sztmp = m_szSorcePath+UserName+"\\file"+CString("\\");
	ScanFile(sztmp,sztmp.GetLength(),veclst);
	for(size_t i =0; i<veclst.size();i++)
	{
		CString file = sztmp+ veclst[i];
		ZipFile(file,szDest);
	}



}
 void CUDiskToolDlg::ConvertGameToDst(const _variant_t& name, const _variant_t& password, const _variant_t& logTime)
 {
	 CString UserName = (LPCSTR)(_bstr_t)name;
	 CString szpassword = (LPCSTR)(_bstr_t)password;
	 CString dwTime = (LPCSTR)(_bstr_t)(logTime);

	 CString szDest = m_szDestPath +UserName +"\\";
	 char Inifile[MAX_PATH] = {0};
	 sprintf_s(Inifile,"%s%s.ini",szDest,UserName);
	 MakeSureDirectoryPathExists(szDest);
	 WritePrivateProfileString("system","password",szpassword,Inifile);
	 WritePrivateProfileString("system","LogTime",dwTime,Inifile);
	 szDest +="game\\";
	 MakeSureDirectoryPathExists(szDest);
	 vector<CString>veclst;
	 string sztmp = m_szSorcePath+UserName+"\\game"+CString("\\");
	 ScanDirtory(sztmp.c_str(),sztmp.size(),veclst);
	for(size_t i =0; i<veclst.size();i++)
	{
		CString szGid;
		_RecordsetPtr prcod;
		char szSql[MAX_PATH ] = {0};
		sprintf_s(szSql,_T("select GID from tGame where Name = \'%s\'"),veclst[i]);
		sztmp+=string(veclst[i])+"\\";
		if(ExecSql(szSql,prcod))
		{
			while(prcod->adoEOF != VARIANT_TRUE)
			{
				szGid = (LPCSTR)(_bstr_t)prcod->GetCollect(0L);
				szDest=szDest+szGid+"\\";
				MakeSureDirectoryPathExists(szDest);
				ZipFile(sztmp.c_str(),szDest,"path");
				prcod->MoveNext();
			}
			prcod.Release();
		}

	}
	
 }
 UINT CUDiskToolDlg:: WorkThread(LPVOID lparam)
 {
	 CUDiskToolDlg* pthis = reinterpret_cast<CUDiskToolDlg*>(lparam);
	
	 _RecordsetPtr prcd;
	 if (pthis->ExecSql("select * from tUser", prcd))
	 {
		DWORD dwCount = prcd->GetRecordCount();
		DWORD i =0;
		while(prcd->adoEOF != VARIANT_TRUE)
		 {
			 _variant_t varName = prcd->GetCollect(0L);
			_variant_t Password = prcd->GetCollect(1L);
			_variant_t LastLogTime = prcd->GetCollect(4L);
			prcd->MoveNext();
			pthis->ConvertFileToDst(varName,Password,LastLogTime);
			pthis->ConvertGameToDst(varName,Password,LastLogTime);
			pthis->PostMessage(WM_PROGRESS,(++i)*(100.0/dwCount),0);
			if(WaitForSingleObject(pthis->m_hExit,0) == WAIT_OBJECT_0)
				break;
		 }
		 prcd.Release();
		 pthis->PostMessage(WM_PROGRESS,0,0);
		 pthis->SetDlgItemText(IDC_STATIC1,"转换完成");
	 }
		return 0;

 }

 void CUDiskToolDlg::OnBnClickedButton3()
 {
	 TCHAR szFilters[]= _T("DataBase Files (*.mdb)|*.mdb|All Files (*.*)|*.*||");
	 CFileDialog dlg(TRUE,"i8desk.mdb","*.mdb", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if(IDOK ==  dlg.DoModal())
	{
		SetDlgItemText(IDC_EDIT3,dlg.GetPathName());
	}
 }
LRESULT CUDiskToolDlg::OnProgress(WPARAM wparam,LPARAM lparam)
{
	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
	m_Progress.ShowWindow(SW_SHOW);
	m_Progress.SetPos(wparam);
return 0;
}
void CUDiskToolDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	SetEvent(m_hExit);
	if(m_hread != NULL)
	{
		WaitForSingleObject(m_hread,INFINITE);
		CloseHandle(m_hread);
		m_hread = NULL;
	}
	if(m_pConn!=NULL)   
	{   
		if(m_pConn->State!=adStateClosed)   
		{   
			m_pConn->Close();     
		}   

		m_pConn.Release();   
	}
	if(m_hExit)
	{
		CloseHandle(m_hExit);
		m_hExit = NULL;
	}

	CDialog::OnClose();
}
