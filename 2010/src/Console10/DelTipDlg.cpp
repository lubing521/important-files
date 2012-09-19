// DelTipDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DelTipDlg.h"
#include "ConsoleDlg.h"


// CDelTipDlg 对话框

IMPLEMENT_DYNAMIC(CDelTipDlg, CDialog)

CDelTipDlg::CDelTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelTipDlg::IDD, pParent)
{
	m_bDelClient = m_bDelLocal = m_bDelSync = FALSE;
}

CDelTipDlg::~CDelTipDlg()
{
}

void CDelTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DELLOCAL, m_btnDelLocal);
	DDX_Control(pDX, IDC_CHECK_DELCLIENT, m_btnDelClient);
	DDX_Control(pDX, IDC_CHECK_DELSYNC, m_btnDelSync);
}

BEGIN_MESSAGE_MAP(CDelTipDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDelTipDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDelTipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CConsoleDlg* pFrame = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	if (!pFrame->IsLocMachineIP())
		m_btnDelLocal.EnableWindow(FALSE);

	return TRUE;
}

void CDelTipDlg::OnBnClickedOk()
{
	m_bDelLocal = m_btnDelLocal.GetCheck() == BST_CHECKED;
	m_bDelClient = m_btnDelClient.GetCheck() == BST_CHECKED;
	m_bDelSync = m_btnDelSync.GetCheck() == BST_CHECKED;
	OnOK();
}
