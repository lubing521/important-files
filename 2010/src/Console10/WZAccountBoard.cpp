// WZAccountBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZAccountBoard.h"


// CWZAccountBoard 对话框

IMPLEMENT_DYNAMIC(CWZAccountBoard, CDialog)

CWZAccountBoard::CWZAccountBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZAccountBoard::IDD, pParent)
	, m_strAccount(_T(""))
	, m_strPassword(_T(""))
	, m_strPassword2(_T(""))
	, m_strCliPassword(_T(""))
{

}

CWZAccountBoard::~CWZAccountBoard()
{
}

void CWZAccountBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CWZAccountBoard::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	i8desk::CDbMgr* pDbMgr = GetDbMgr();

	((CEdit*)GetDlgItem(IDC_USERNAME))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD2))->LimitText(8);

	SetDlgItemText(IDC_USERNAME, pDbMgr->GetOptString(OPT_U_USERNAME).c_str());
	SetDlgItemText(IDC_PASSWORD, pDbMgr->GetOptString(OPT_U_PASSWORD).c_str());
	SetDlgItemText(IDC_PASSWORD2, pDbMgr->GetOptString(OPT_U_CTLPWD, "1234567").c_str());
	SetDlgItemText(IDC_CLIPASSWORD, pDbMgr->GetOptString(OPT_M_CLIPWD, "1234567").c_str());
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CWZAccountBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZAccountBoard::OnApplyMessage)
	ON_EN_CHANGE(IDC_USERNAME, &CWZAccountBoard::OnEnChangeUsername)
	ON_EN_CHANGE(IDC_PASSWORD, &CWZAccountBoard::OnEnChangePassword)
	ON_EN_CHANGE(IDC_PASSWORD2, &CWZAccountBoard::OnEnChangePassword2)
	ON_EN_CHANGE(IDC_CLIPASSWORD, &CWZAccountBoard::OnEnChangeClipassword)
END_MESSAGE_MAP()


// CWZAccountBoard 消息处理程序
LRESULT CWZAccountBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	i8desk::CDbMgr* pDbMgr = GetDbMgr();

	if (!m_strAccount.IsEmpty())
	{
		pDbMgr->SetOption(OPT_U_USERNAME, (LPCSTR)m_strAccount);
		m_strAccount = _T("");
	}

	if (!m_strPassword.IsEmpty())
	{
		pDbMgr->SetOption(OPT_U_PASSWORD, (LPCSTR)m_strPassword);
		m_strPassword = _T("");
	}

	if (!m_strPassword2.IsEmpty())
	{
		pDbMgr->SetOption(OPT_U_CTLPWD, (LPCSTR)m_strPassword2);
		m_strPassword2 = _T("");
	}
	
	if (!m_strCliPassword.IsEmpty())
	{
		pDbMgr->SetOption(OPT_M_CLIPWD, i8desk::GetBufCrc32((BYTE *)(LPCTSTR)m_strCliPassword, m_strCliPassword.GetLength()));
		m_strCliPassword = _T("");
	}
	
	return TRUE;
}

void CWZAccountBoard::OnEnChangeUsername()
{
	GetDlgItem(IDC_USERNAME)->GetWindowText(m_strAccount);
}

void CWZAccountBoard::OnEnChangePassword()
{
	GetDlgItem(IDC_PASSWORD)->GetWindowText(m_strPassword);
}

void CWZAccountBoard::OnEnChangePassword2()
{
	GetDlgItem(IDC_PASSWORD2)->GetWindowText(m_strPassword2);
}

void CWZAccountBoard::OnEnChangeClipassword()
{
	GetDlgItem(IDC_CLIPASSWORD)->GetWindowText(m_strCliPassword);
}
