#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "..\UpdateGame.h"

#import "msxml3.dll"
#import "c:\\Program Files\\Common Files\\system\\ado\\msadox.dll"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
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


CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &CTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK_SPEED, &CTestDlg::OnBnClickedCheckSpeed)
END_MESSAGE_MAP()

HANDLE		g_hUpdate		= NULL;
ISvrPlug*	g_pUpdatePlug	= NULL;
ISvrPlug*	g_pSyncPlug		= NULL;

#define TITLE_TEXT	L"内网更新-同步测试"

_bstr_t GetNodeAttr(const MSXML2::IXMLDOMNodePtr& node, LPCSTR name)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attr = node->Getattributes();
	if (!attr)
		return _bstr_t("");
	MSXML2::IXMLDOMNodePtr node2 = attr->getNamedItem(_bstr_t(name));
	if (!node2)
		return _bstr_t("");
	return node2->text;
}

//得到xml节点的文本
inline _bstr_t GetNodeAttrValue(const MSXML2::IXMLDOMNodePtr& node, LPCSTR name)
{
	MSXML2::IXMLDOMNamedNodeMapPtr attr = node->Getattributes();
	if (!attr)
		return _bstr_t("");
	MSXML2::IXMLDOMNodePtr node2 = attr->getNamedItem(_bstr_t(name));
	if (!node2)
		return _bstr_t("");
	return node2->text;
}

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	/*
	CoInitialize(NULL);
	{
		bool flag = true;
		MSXML2::IXMLDOMDocumentPtr xmlDoc(__uuidof(MSXML2::DOMDocument));
		MSXML2::IXMLDOMDocumentPtr pl(__uuidof(MSXML2::DOMDocument));
		BOOL b = pl->loadXML(TEXT("<?xml version='1.0' encoding='utf-8'?><document v='time_t' t='url' n='url'><games/></document>"));
		MSXML2::IXMLDOMElementPtr NodeGame = pl->selectSingleNode(TEXT("/document/games"));
		if (VARIANT_TRUE == xmlDoc->load(TEXT("E:\\I8Desk2010\\Server\\bin\\Tmp\\pl.xml")))
		{
			MSXML2::IXMLDOMNodeListPtr list = xmlDoc->selectNodes(TEXT("/main/products/product"));
			int nCount = list->Getlength();
			for (long idx=0; idx<nCount; idx++)
			{
				MSXML2::IXMLDOMNodePtr child  = list->Getitem(idx);
				MSXML2::IXMLDOMNodePtr base   = child->selectSingleNode(TEXT("base"));
				MSXML2::IXMLDOMNodePtr match  = child->selectSingleNode(TEXT("match"));
				MSXML2::IXMLDOMNodePtr update = child->selectSingleNode(TEXT("update"));

				MSXML2::IXMLDOMElementPtr game = pl->createElement(TEXT("game"));

				_bstr_t str = GetNodeAttrValue(child, "gid");
				game->setAttribute(TEXT("i"),  str);

				str = GetNodeAttrValue(child, "fid");
				if (!flag)
					game->setAttribute(TEXT("a"), str);
				else if (str.length() && str != _bstr_t(TEXT("0")))
					game->setAttribute(TEXT("a"), str);

				game->setAttribute(TEXT("t"), TEXT("a"));
				
				str = GetNodeAttrValue(child, "play");
				if (!flag)
					game->setAttribute(TEXT("v"),  str);
				else if (str.length() && str != _bstr_t(TEXT("0")))
					game->setAttribute(TEXT("v"),  str);
				
				str = GetNodeAttrValue(child, "lock");
				if (!flag)
					game->setAttribute(TEXT("d"), str);
				else if (str.length() && str != _bstr_t(TEXT("0")))
					game->setAttribute(TEXT("d"), str);

				{
					MSXML2::IXMLDOMElementPtr node = pl->createElement(TEXT("base"));
					node->setAttribute(TEXT("n"),	GetNodeAttrValue(base,	"name"));
					node->setAttribute(TEXT("e"),	GetNodeAttrValue(base,	"exe"));
					node->setAttribute(TEXT("t"),	GetNodeAttrValue(match, "miniorfile"));
					node->setAttribute(TEXT("s"),	GetNodeAttrValue(base,	"size"));
					node->setAttribute(TEXT("c"),   GetNodeAttrValue(update,"idcclick"));
					node->setAttribute(TEXT("m"),	GetNodeAttrValue(base,	"memo"));					
					node->setAttribute(TEXT("f"),	GetNodeAttrValue(base,	"SourceTypeName"));
					node->setAttribute(TEXT("d"),	GetNodeAttrValue(base,	"adddate"));

					game->appendChild(node);
				}
				{
					MSXML2::IXMLDOMElementPtr node = pl->createElement(TEXT("property"));

					str = GetNodeAttrValue(base, "syncName");
					if (!flag)
						node->setAttribute(TEXT("r"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("r"),  str);

					str = GetNodeAttrValue(base, "toolbar");
					if (!flag)
						node->setAttribute(TEXT("t"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("t"),  str);

					str = GetNodeAttrValue(base, "desklnk");
					if (!flag)
						node->setAttribute(TEXT("l"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("l"),  str);

					if (!flag)
						node->setAttribute(TEXT("d"),  TEXT("0"));

					str = GetNodeAttrValue(base, "param");
					if (!flag)
						node->setAttribute(TEXT("p"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("p"),  str);

					str = GetNodeAttrValue(base, "savefilter");
					if (!flag)
						node->setAttribute(TEXT("f"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("f"),  str);

					if (node->xml != _bstr_t(TEXT("<property/>")))
						game->appendChild(node);
				}
				{
					MSXML2::IXMLDOMElementPtr node = pl->createElement(TEXT("update"));
					str = GetNodeAttrValue(update, "idcver");
					if (!flag)
						node->setAttribute(TEXT("v"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("v"),  str);

					str = GetNodeAttrValue(update, "auto");
					if (!flag)
						node->setAttribute(TEXT("f"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("f"),  str);

					str = GetNodeAttrValue(update, "LocalPath");
					if (!flag)
						node->setAttribute(TEXT("d"),  str);
					else if (str.length() && str != _bstr_t(TEXT("0")))
						node->setAttribute(TEXT("d"),  str);

					if (node->xml != _bstr_t(TEXT("<update/>")))
						game->appendChild(node);
				}
				NodeGame->appendChild(game);
			}
		}

		DeleteFile(TEXT("C:\\1.xml"));
		_bstr_t str = pl->xml;
		pl->save(TEXT("C:\\1.xml"));			
	}
	{
		using namespace ADOX;
		_CatalogPtr p(__uuidof(Catalog));
		p->Create(_bstr_t(TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=c:\\1.mdb")));

	}
	CoUninitialize();
	*/

	::SetWindowTextW(m_hWnd, TITLE_TEXT);
	SetDlgItemInt(IDC_GID, 10255);
	GetDlgItem(IDC_SVR_IP)->SetWindowText(TEXT("10.34.43.45"));
	GetDlgItem(IDC_SVR_PATH)->SetWindowText(TEXT("Y:\\网络游戏\\刀剑Online\\"));
	GetDlgItem(IDC_CLI_PATH)->SetWindowText(TEXT("E:\\网络游戏\\刀剑Online\\"));

	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);

	((CButton*)GetDlgItem(IDC_CHECK_SPEED))->SetCheck(BST_UNCHECKED);
	OnBnClickedCheckSpeed();

	return TRUE;
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestDlg::OnDestroy()
{
	if (g_hUpdate != NULL)
	{
		UG_StopUpdate(g_hUpdate);
		g_hUpdate = NULL;
	}

	if (g_pUpdatePlug != NULL)
	{
		g_pUpdatePlug->UnInitialize();
		Release_Interface(g_pUpdatePlug);
	}
	if (g_pSyncPlug != NULL)
	{
		g_pSyncPlug->UnInitialize();
		Release_Interface(g_pSyncPlug);
	}
	CDialog::OnDestroy();
}

void CTestDlg::OnBnClickedOk()
{
	
}

void CTestDlg::OnBnClickedButton1()
{
	wchar_t ip[MAX_PATH]  = {0};
	wchar_t src[MAX_PATH] = {0};
	wchar_t cli[MAX_PATH] = {0};
	DWORD gid = GetDlgItemInt(IDC_GID);
	DWORD speed = GetDlgItemInt(IDC_SPEED) * 1000;
	::GetWindowTextW(GetDlgItem(IDC_SVR_IP)->m_hWnd, ip, sizeof(ip));
	::GetWindowTextW(GetDlgItem(IDC_SVR_PATH)->m_hWnd, src, sizeof(src));
	::GetWindowTextW(GetDlgItem(IDC_CLI_PATH)->m_hWnd, cli, sizeof(src));
	bool bForce = ((CButton*)GetDlgItem(IDC_FORCE))->GetCheck() == BST_CHECKED;
	if (wcslen(ip) == 0 || wcslen(src) == 0 || wcslen(cli) == 0)
	{
		AfxMessageBox(TEXT("请输入要同步游戏的所有参数。"));
		return ;
	}
	DWORD flag = (bForce ? UPDATE_FLAG_FORCE_UPDATE : 0)|UPDATE_FLAG_DELETE_MOREFILE;
	
	g_hUpdate = UG_StartUpdate(gid, NULL, src, cli, flag, ip, NULL, speed);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	SetTimer(1, 1000, NULL);
}

void CTestDlg::OnBnClickedButton2()
{
	if (g_hUpdate != NULL)
	{
		UG_StopUpdate(g_hUpdate);
		g_hUpdate = NULL;
	}
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
}

void CTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	tagUpdateStatus Status = {sizeof(tagUpdateStatus)};	
	if (g_hUpdate != NULL)
	{
		if (!UG_GetUpdateStatus(g_hUpdate, &Status) ||
			Status.Status == UPDATE_STATUS_FINISH || 
			Status.Status == UPDATE_STATUS_ERROR)
		{
			KillTimer(1);
			if (Status.Status == UPDATE_STATUS_ERROR)
			{
				wchar_t szTitle[1024] = {0};
				swprintf(szTitle, L"%s:%s", TITLE_TEXT, Status.Info);
				::SetWindowTextW(m_hWnd, szTitle);
			}
			else
			{
				wchar_t szTitle[1024] = {0};
				swprintf(szTitle, L"%s:完成", TITLE_TEXT);
				::SetWindowTextW(m_hWnd, szTitle);
			}
			OnBnClickedButton2();
		}
		else if (Status.Status == UPDATE_STAUTS_COMPARE)
		{
			wchar_t szTitle[1024] = {0};
			swprintf(szTitle, L"%s:对比索引:%s", TITLE_TEXT, Status.Info);
			::SetWindowTextW(m_hWnd, szTitle);
		}
		else if (Status.Status == UPDATE_STATUS_UPDATE)
		{
			wchar_t szTitle[1024] = {0};
			swprintf(szTitle, L"%s:更新块:%.2f,剩余时间:%d:%s", 
				TITLE_TEXT, Status.m_dwSpeed/1024.0f, Status.m_dwLeftTime, Status.Info);
			::SetWindowTextW(m_hWnd, szTitle);
		}
		else if (Status.Status == UPDATE_STATUS_DELETE_MOREFILE)
		{
			wchar_t szTitle[1024] = {0};
			swprintf(szTitle, L"%s:删除多余文件:%s", TITLE_TEXT, Status.Info);
			::SetWindowTextW(m_hWnd, szTitle);
		}
	}
}

void CTestDlg::OnBnClickedButton3()
{
	g_pSyncPlug = CreateSvrPlug((DWORD)PLUG_SYNCDISK_NAME);
	if (g_pSyncPlug == NULL || !g_pSyncPlug->Initialize(NULL))
	{
		AfxMessageBox(TEXT("启动同步组件出错."));
		return ;
	}
}

void CTestDlg::OnBnClickedButton4()
{
	if (g_pSyncPlug != NULL)
	{
		g_pSyncPlug->UnInitialize();
		Release_Interface(g_pSyncPlug);
	}
}

void CTestDlg::OnBnClickedCheckSpeed()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SPEED))->GetCheck();
	GetDlgItem(IDC_SPEED)->EnableWindow(bCheck);
	if (!bCheck)
		SetDlgItemInt(IDC_SPEED, 0);
}




