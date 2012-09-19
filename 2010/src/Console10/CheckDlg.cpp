// CheckDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "CheckDlg.h"
#include "ConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCheckDlg, CDialog)

CCheckDlg::CCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckDlg::IDD, pParent)
{

}

CCheckDlg::~CCheckDlg()
{
}

void CCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCheckDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCheckDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCheckDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(g_nCheckCloseMsg, &CCheckDlg::OnCheck)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;

	SetDlgItemText(IDC_USERNAME, pDbMgr->GetOptString(OPT_U_USERNAME).c_str());
	SetDlgItemText(IDC_PASSWORD, pDbMgr->GetOptString(OPT_U_PASSWORD).c_str());

	CString szErr = "验证失败:";
	szErr += pDbMgr->GetOptString(OPT_U_ERRINFO).c_str();
	SetDlgItemText(IDC_ERRINFO, szErr);

	return TRUE;
}

void CCheckDlg::OnBnClickedOk()
{
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	char szUser[MAX_PATH] = {0};
	char szPassword[MAX_PATH] = {0};
	GetDlgItemText(IDC_USERNAME, szUser, MAX_PATH);
	GetDlgItemText(IDC_PASSWORD, szPassword, MAX_PATH);
	
	if (lstrlen(szUser) == 0)
	{
		AfxMessageBox("用户名不能名空");
		GetDlgItem(IDC_USERNAME)->SetFocus();
		return ;
	}
	if (lstrlen(szPassword) == 0)
	{
		AfxMessageBox("密码不能为空");
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return ;
	}
	
	pDbMgr->SetOption(OPT_U_USERNAME, szUser);
	pDbMgr->SetOption(OPT_U_PASSWORD, szPassword);

	CWaitCursor wc;
	std::string ErrInfo;
	if (!pDbMgr->NotifySvrCheck(ErrInfo))
	{
		wc.Restore();
		AfxMessageBox(ErrInfo.c_str());
		return ;
	}
	SetTimer(1, 10000, NULL);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	wc.Restore();
}

void CCheckDlg::OnBnClickedCancel()
{
	OnCancel();
}


LRESULT CCheckDlg::OnCheck(WPARAM wParam, LPARAM lParam)
{
	CDialog::OnOK();
	return 0;
}

void CCheckDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	CString szErr = "验证失败:";
	szErr += pDbMgr->GetOptString(OPT_U_ERRINFO).c_str();
	SetDlgItemText(IDC_ERRINFO, szErr);
	KillTimer(1);
}

HBRUSH CCheckDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->m_hWnd == GetDlgItem(IDC_ERRINFO)->m_hWnd)
	{
		pDC->SetBkMode(1);
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	
	return hbr;
}
