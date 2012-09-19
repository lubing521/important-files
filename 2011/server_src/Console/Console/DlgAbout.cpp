// DlgAbout.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DlgAbout.h"
#include "Misc.h"
#include "../../../include/Utility/utility.h"
#include "../../../include/define.h"

#include "../../../include/ui/ImageHelpers.h"
#include "ui/UIHelper.h"

// CDlgAbout 对话框

IMPLEMENT_DYNAMIC(CDlgAbout, CDialog)

CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{

}

CDlgAbout::~CDlgAbout()
{
}

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ABOUT_PIC, bk_);

}


BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_ABOUT_OK, &CDlgAbout::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAbout 消息处理程序
BOOL CDlgAbout::OnInitDialog()
{
	CDialog::OnInitDialog();


	CString strVersion(_T("版本信息\t\t产品版本号\t\t文件版本\r\nConsole.exe,\t\t"));	
	TCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path, MAX_PATH);
	strVersion += GetProductVersionString(path);
	strVersion += _T(",\t\t");
	strVersion += GetFileVersionString(path);

	if (i8desk::IsRunOnServer())
	{
		strVersion += _T("\r\n");
		CString svrPath = utility::GetAppPath().c_str();
		strVersion += _T("DeskSvr.exe,\t\t");
		strVersion += GetProductVersionString(svrPath + _T("I8DeskSvr.exe"));
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + _T("I8DeskSvr.exe"));

		strVersion += _T("\r\n");
		strVersion += PLUG_RTDATASVR_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_RTDATASVR_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_RTDATASVR_MODULE);

		strVersion += _T("\r\n");
		strVersion += PLUG_FRAME_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_FRAME_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_FRAME_MODULE);

		strVersion += _T("\r\n");
		svrPath = utility::GetAppPath().c_str();
		strVersion += _T("CustomFileDLL.dll,\t");
		strVersion += GetProductVersionString(svrPath + _T("CustomFileDLL.dll"));
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + _T("CustomFileDLL.dll"));

		strVersion += _T("\r\n");
		strVersion += PLUG_LOGRPT_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_LOGRPT_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_LOGRPT_MODULE);

		strVersion += _T("\r\n");
		strVersion += PLUG_GAMEMGR_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_GAMEMGR_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_GAMEMGR_MODULE);

		strVersion += _T("\r\n");
		strVersion += PLUG_PLUGTOOL_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_PLUGTOOL_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_PLUGTOOL_MODULE);

		strVersion += _T("\r\n");
		strVersion += PLUG_BUSINESS_MODULE;
		strVersion += _T(",\t\t");
		strVersion += GetProductVersionString(svrPath + PLUG_BUSINESS_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_BUSINESS_MODULE);

		strVersion += _T("\r\n");
		strVersion += PLUG_GAMEUPDATE_MODULE;
		strVersion += _T(",\t");
		strVersion += GetProductVersionString(svrPath + PLUG_GAMEUPDATE_MODULE);
		strVersion += _T(",\t\t");
		strVersion += GetFileVersionString(svrPath + PLUG_GAMEUPDATE_MODULE);
	}
	SetDlgItemText(IDC_STATIC_ABOUT_VERSION, strVersion);

	bk_.SetImage(AtlLoadGdiplusImage(IDB_PNG_BK, _T("PNG")));

	return TRUE;
}

CString CDlgAbout::GetProductVersionString(CString file)
{
	CString ver(_T("0.0.0.0"));

	DWORD dwSize = GetFileVersionInfoSize( file, 0);
	if (dwSize)
	{
		BYTE *pInfo = new BYTE[dwSize+1];
		GetFileVersionInfo( file, 0, dwSize, pInfo);

		UINT nQuerySize;
		DWORD* pTransTable = NULL;
		VerQueryValue(pInfo, _T("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
		LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		TCHAR SubBlock[MAX_PATH] = {0};
		_stprintf(SubBlock, _T("\\StringFileInfo\\%08lx\\ProductVersion"), m_dwLangCharset);	
		LPTSTR lpData;
		VerQueryValue(pInfo, SubBlock, (PVOID*)&lpData, &nQuerySize);
		ver = lpData;
		delete []pInfo;
	}
	ver.Replace(_T(","),_T(".") );
	ver.Replace(_T(" "),_T( ""));
	return ver;
}

CString CDlgAbout::GetFileVersionString(CString file)
{
	CString ver(_T("0.0.0.0"));

	DWORD dwSize = GetFileVersionInfoSize( file, 0);
	if (dwSize)
	{
		BYTE *pInfo = new BYTE[dwSize+1];
		GetFileVersionInfo( file, 0, dwSize, pInfo);

		UINT nQuerySize;
		DWORD* pTransTable = NULL;
		VerQueryValue(pInfo, _T("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
		LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		TCHAR SubBlock[MAX_PATH] = {0};
		_stprintf(SubBlock, _T("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
		LPTSTR lpData;
		VerQueryValue(pInfo, SubBlock, (PVOID*)&lpData, &nQuerySize);
		ver = lpData;
		delete []pInfo;
	}
	ver.Replace(_T(","),_T(".") );
	ver.Replace(_T(" "),_T( ""));
	return ver;
}

BOOL CDlgAbout::OnEraseBkgnd(CDC* pDC)
{
	__super::OnEraseBkgnd(pDC);

	return TRUE;
}

void CDlgAbout::OnBnClickedOk()
{
	OnOK();
}
