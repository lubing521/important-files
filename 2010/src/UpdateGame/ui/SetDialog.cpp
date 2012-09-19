#include "stdafx.h"
#include "ServerUI.h"
#include "SetDialog.h"

IMPLEMENT_DYNAMIC(CSetDialog, CDialog)

CSetDialog::CSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDialog::IDD, pParent)
{
m_bSet = false;
}

CSetDialog::~CSetDialog()
{
}

void CSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CSetDialog::OnBnClickedOk)
END_MESSAGE_MAP()

void CSetDialog::OnBnClickedOk()
{
	CString szip;
	GetDlgItemText(IDC_IPADDRESS1,szip);
	if(!szip.IsEmpty() && (m_srvip.Compare(szip) !=0))
	{
		char szpath[MAX_PATH] = {0};
		GetModuleFileName(NULL,szpath,MAX_PATH);
		PathRemoveFileSpec(szpath);
		lstrcat(szpath,"\\data\\I8SyncSvr.ini");
		MakeSureDirectoryPathExists(szpath);
		WritePrivateProfileString("System","Svrip",szip,szpath);
		m_bSet = true;
	}
	OnOK();
}

BOOL CSetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	char szip[20] = {0};
	char szpath[MAX_PATH] = {0};
	GetModuleFileName(NULL,szpath,MAX_PATH);
	PathRemoveFileSpec(szpath);
	lstrcat(szpath,"\\data\\I8SyncSvr.ini");
	GetPrivateProfileString("System","Svrip","",szip,20,szpath);
	SetDlgItemText(IDC_IPADDRESS1,szip);
	m_srvip = CString(szip);
	return TRUE;
}
