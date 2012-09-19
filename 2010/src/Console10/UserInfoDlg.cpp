// UserInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "UserInfoDlg.h"
#include "ConsoleDlg.h"

// CUserInfoDlg 对话框

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialog)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserInfoDlg::IDD, pParent)
{
	m_bModify = FALSE;
}

CUserInfoDlg::~CUserInfoDlg()
{
}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUserInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUserInfoDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CUserInfoDlg::OnApplyMessage)

	ON_EN_CHANGE(IDC_USERNAME, &CUserInfoDlg::OnEnChangeUsername)
	ON_EN_CHANGE(IDC_PASSWORD, &CUserInfoDlg::OnEnChangeUsername)
	ON_EN_CHANGE(IDC_PASSWORD2, &CUserInfoDlg::OnEnChangeUsername)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_USER_INFO_MPDIFY, &CUserInfoDlg::OnBnClickedButtonUserInfoMpdify)
END_MESSAGE_MAP()

BOOL CUserInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;

	((CEdit*)GetDlgItem(IDC_USERNAME))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD2))->LimitText(8);


	i8desk::std_string strProvinceCity = pDbMgr->GetOptString(OPT_U_PROVINCE) + _T(" ") +
		pDbMgr->GetOptString(OPT_U_CITY);
	SetDlgItemText(IDC_STATIC_USER_INFO_ADDR,		strProvinceCity.c_str());
	SetDlgItemText(IDC_STATIC_USER_INFO_ADDRESS,	pDbMgr->GetOptString(OPT_U_NBADDRESS).c_str());
	SetDlgItemText(IDC_STATIC_USER_INFO_TEL,		pDbMgr->GetOptString(OPT_U_NBPHONE).c_str());
	SetDlgItemText(IDC_STATIC_USER_INFO_MOBILE,		pDbMgr->GetOptString(OPT_U_NBMOBILE).c_str());

	SetDlgItemText(IDC_USERNAME, pDbMgr->GetOptString(OPT_U_USERNAME).c_str());
	SetDlgItemText(IDC_PASSWORD, pDbMgr->GetOptString(OPT_U_PASSWORD).c_str());
	SetDlgItemText(IDC_NID,		 pDbMgr->GetOptString(OPT_U_NID).c_str());
	SetDlgItemText(IDC_NBNAME,   pDbMgr->GetOptString(OPT_U_NBNAME).c_str());
	SetDlgItemText(IDC_USERNAME, pDbMgr->GetOptString(OPT_U_USERNAME).c_str());
	SetDlgItemText(IDC_BINDID,   pDbMgr->GetOptString(OPT_U_OEMID).c_str());
	SetDlgItemText(IDC_BINDNAME, pDbMgr->GetOptString(OPT_U_OEMNAME).c_str());
	SetDlgItemText(IDC_PASSWORD2, pDbMgr->GetOptString(OPT_U_CTLPWD, "1234567").c_str());
	
	std::string date_end = pDbMgr->GetOptString(OPT_U_DATEEND) 
		+ " " + pDbMgr->GetOptString(OPT_U_USERTYPE);
	SetDlgItemText(IDC_DATEEND, date_end.c_str());


	m_bModify = FALSE;

	return TRUE;
}

void CUserInfoDlg::OnBnClickedOk()
{
	
}

void CUserInfoDlg::OnBnClickedCancel()
{
	
}

LRESULT CUserInfoDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		CString strUser, strPassword, strPassword2;
		GetDlgItemText(IDC_USERNAME, strUser);
		GetDlgItemText(IDC_PASSWORD, strPassword);
		GetDlgItemText(IDC_PASSWORD2, strPassword2);
		if (strUser.IsEmpty())
		{
			AfxMessageBox("请输入用户名!");
			GetDlgItem(IDC_USERNAME)->SetFocus();
			return FALSE;
		}

		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		pDbMgr->SetOption(OPT_U_USERNAME, (LPCSTR)strUser);
		pDbMgr->SetOption(OPT_U_PASSWORD, (LPCSTR)strPassword);
		pDbMgr->SetOption(OPT_U_CTLPWD, (LPCSTR)strPassword2);
		m_bModify = FALSE;
	}

	return TRUE;
}
void CUserInfoDlg::OnEnChangeUsername()
{
	m_bModify = TRUE;
}

HBRUSH CUserInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static UINT uIDs[] =
	{
		IDC_STATIC1,
		IDC_STATIC2,
		IDC_STATIC3,
		IDC_STATIC4,
		IDC_STATIC_USER_INFO_RED
	};

	if( nCtlColor == CTLCOLOR_STATIC )
	{
		for(int i = 0; i != _countof(uIDs); ++i )
		{
			if( pWnd->GetDlgCtrlID() == uIDs[i] )
				pDC->SetTextColor(RGB(255, 0, 0));
		}
		
		
	}



	return hbr;
}

void CUserInfoDlg::OnBnClickedButtonUserInfoMpdify()
{
	i8desk::std_string strModifyUrl = GetConsoleDlg()->m_pDbMgr->GetOptString(OPT_U_REALITYURL);

	ShellExecute(
		GetSafeHwnd(),
		_T("open"), 
		strModifyUrl.c_str(), 
		_T(""), 
		_T(""),
		SW_SHOWNORMAL);
}
