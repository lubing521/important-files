// IdcUptToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IdcUptTool.h"
#include "IdcUptToolDlg.h"
#include "Markup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static DWORD Crc32Table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
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

CIdcUptToolDlg::CIdcUptToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIdcUptToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIdcUptToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
}

BEGIN_MESSAGE_MAP(CIdcUptToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CIdcUptToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_OPENDIR, &CIdcUptToolDlg::OnBnClickedOpendir)
	ON_BN_CLICKED(IDC_MAKEIDX, &CIdcUptToolDlg::OnBnClickedMakeidx)
	ON_BN_CLICKED(IDC_DELFILE, &CIdcUptToolDlg::OnBnClickedDelfile)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CIdcUptToolDlg::OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_MODIFYFILE, &CIdcUptToolDlg::OnBnClickedModifyfile)
END_MESSAGE_MAP()


// CIdcUptToolDlg 消息处理程序

BOOL CIdcUptToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	m_lstCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstCtrl.InsertColumn(0, TEXT("本地文件名"),	LVCFMT_LEFT, 140, 0);
	m_lstCtrl.InsertColumn(1, TEXT("网吧文件名"),	LVCFMT_LEFT, 140, 0);
	m_lstCtrl.InsertColumn(2, TEXT("下载URL地址"),	LVCFMT_LEFT, 180, 0);
	m_lstCtrl.InsertColumn(3, TEXT("文件CRC32"),	LVCFMT_LEFT, 80, 0);
	m_lstCtrl.InsertColumn(4, TEXT("修改时间"),		LVCFMT_LEFT, 80, 0);

	SetDlgItemText(IDC_URL, TEXT("http://www.i8desk.com"));

	return TRUE;
}

void CIdcUptToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CIdcUptToolDlg::OnPaint()
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

HCURSOR CIdcUptToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIdcUptToolDlg::OnBnClickedOk()
{

}

void CIdcUptToolDlg::ScanDir(CString szDir, int nBaseLen)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(szDir + TEXT("\\*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory())
		{
			if (!finder.IsDots())
			{
				CString newDir = szDir + finder.GetFileName() + TEXT("\\");
				ScanDir(newDir, nBaseLen);
			}
		}
		else
		{
			CString szUrl;
			GetDlgItemText(IDC_URL, szUrl);
			if (szUrl.GetLength() && szUrl.Right(1) != TEXT("/"))
				szUrl += TEXT("/");
			int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), finder.GetFilePath());

			CString szText = finder.GetFilePath().Mid(nBaseLen);
			m_lstCtrl.SetItemText(nIdx, 1, TEXT("%root%\\") + szText);

			szText.Replace(TEXT('\\'), TEXT('/'));
			m_lstCtrl.SetItemText(nIdx, 2, szUrl + szText);
			CString strTime;
			FILETIME ft = {0};
			finder.GetLastWriteTime(&ft);
			strTime.Format(TEXT("%d"), MakeTimeFromFileTime(ft));
			m_lstCtrl.SetItemText(nIdx, 4, strTime);
// 			CTime tMod;
// 			finder.GetLastWriteTime(tMod);
// 			szText.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), 
// 				tMod.GetYear(), tMod.GetMonth(), tMod.GetDay(),
// 				tMod.GetHour(), tMod.GetMinute(), tMod.GetSecond());
// 			m_lstCtrl.SetItemText(nIdx, 4, szText);
		}
	}
}

void CIdcUptToolDlg::OnBnClickedOpendir()
{
	CString szURL;
	GetDlgItemText(IDC_URL, szURL);
	if (szURL.IsEmpty())
	{
		AfxMessageBox(TEXT("请先输入URL地址."));
		return ;
	}

	TCHAR path[MAX_PATH] = {0};
	BROWSEINFO info;
	memset(&info, 0, sizeof(info));
	info.hwndOwner = m_hWnd;
	info.lpszTitle = TEXT("选择升级目录:");
	//info.lpfn = SetSelect_CB;

	LPITEMIDLIST lst = SHBrowseForFolder(&info);
	if (lst != NULL)
	{
		SHGetPathFromIDList(lst, path);
		LPMALLOC pMalloc = NULL;
		SHGetMalloc(&pMalloc);
		pMalloc->Free(lst);
		pMalloc->Release();
		CString dir(path);
		if (dir.Right(1) != TEXT("\\"))
			dir += TEXT("\\");
		m_lstCtrl.DeleteAllItems();
		CWaitCursor wc;
		ScanDir(dir, dir.GetLength());
		wc.Restore();
	}
}

DWORD CIdcUptToolDlg::MakeTimeFromFileTime(FILETIME& ft)
{
	FILETIME localFileTime = {0};
	FileTimeToLocalFileTime(&ft, &localFileTime);

	SYSTEMTIME st = {0};		
	FileTimeToSystemTime(&localFileTime, &st);
	struct tm stm = {0};
	stm.tm_year = st.wYear - 1900;
	stm.tm_mon  = st.wMonth - 1;
	stm.tm_mday = st.wDay;
	stm.tm_hour = st.wHour;
	stm.tm_min  = st.wMinute;
	stm.tm_sec  = st.wSecond;
	stm.tm_isdst = -1;

	return _mktime32(&stm);
}

static DWORD CalBufCRC32(BYTE* buffer, DWORD dwSize)
{
	ULONG  crc(0xffffffff);
	int len;
	len = dwSize;
	while(len--)
		crc = (crc >> 8) ^ Crc32Table[(crc & 0xFF) ^ *buffer++];
	return crc^0xffffffff;
}

CString CIdcUptToolDlg::GetFileCrc32(const CString& szFileName)
{
	CString strCrc32;
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pData = new BYTE[dwSize+1];
		DWORD dwReadBytes = 0;
		if (ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) && dwSize == dwReadBytes)
		{
			DWORD dwCrc = CalBufCRC32(pData, dwSize);
			strCrc32.Format(TEXT("%X"), dwCrc);
		}
		CloseHandle(hFile);
		delete[] pData;
	}
	return strCrc32;
}

void CIdcUptToolDlg::OnBnClickedMakeidx()
{
	CWaitCursor wc;
	CMarkup xml;
	xml.SetDoc(TEXT("<?xml version=\"1.0\" encoding=\"gb2312\" ?>\r\n"));
	xml.AddElem(TEXT("filelist"));
	xml.IntoElem();
	for (int idx=0; idx<m_lstCtrl.GetItemCount(); idx++)
	{
		xml.AddElem(TEXT("file"));
		CString szFileName = m_lstCtrl.GetItemText(idx, 0);
		xml.SetAttrib(TEXT("name"), m_lstCtrl.GetItemText(idx, 1));
		xml.SetAttrib(TEXT("url"),  m_lstCtrl.GetItemText(idx, 2));
		CString strCRC = GetFileCrc32(m_lstCtrl.GetItemText(idx, 0));
		xml.SetAttrib(TEXT("crc"),  strCRC);
		m_lstCtrl.SetItemText(idx, 3, strCRC);
		xml.SetAttrib(TEXT("modifytime"), m_lstCtrl.GetItemText(idx, 4));
	}
	xml.Save(TEXT("./update.xml"));
	ShellExecute(m_hWnd, TEXT("open"), TEXT("./update.xml"), NULL, NULL, SW_SHOWNORMAL);
	wc.Restore();
}

void CIdcUptToolDlg::OnBnClickedDelfile()
{
	for (int idx=m_lstCtrl.GetItemCount(); idx>0; idx--)
	{
		if (m_lstCtrl.GetItemState(idx-1, LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_lstCtrl.DeleteItem(idx-1);
		}
	}
}

void CIdcUptToolDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (m_lstCtrl.GetSelectedCount() == 0)
	{
		SetDlgItemText(IDC_FILENAME, TEXT(""));
		SetDlgItemText(IDC_DOWN_URL, TEXT(""));
		return ;
	}
	else
	{
		int nSel = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
		SetDlgItemText(IDC_FILENAME, m_lstCtrl.GetItemText(nSel, 1));
		SetDlgItemText(IDC_DOWN_URL, m_lstCtrl.GetItemText(nSel, 2));
	}
}

void CIdcUptToolDlg::OnBnClickedModifyfile()
{
	int nSel = m_lstCtrl.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox(TEXT("请选择要修改的文件项."));
		return ;
	}
	CString szFileName, szURL;
	GetDlgItemText(IDC_FILENAME, szFileName);
	GetDlgItemText(IDC_DOWN_URL, szURL);
	szFileName.Trim();
	szURL.Trim();
	if (szFileName.IsEmpty())
	{
		AfxMessageBox(TEXT("请输入文件名."));
		GetDlgItem(IDC_FILENAME)->SetFocus();
		return ;
	}
	if (szURL.IsEmpty())
	{
		AfxMessageBox(TEXT("请输入下载地址."));
		GetDlgItem(IDC_DOWN_URL)->SetFocus();
		return ;
	}

	m_lstCtrl.SetItemText(nSel, 1, szFileName);
	m_lstCtrl.SetItemText(nSel, 2, szURL);
}
