// InUpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "InUpdateDlg.h"
#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CInUpdateDlg, CDialog)

CInUpdateDlg::CInUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInUpdateDlg::IDD, pParent)
	, m_bModify(FALSE)
{

}

CInUpdateDlg::~CInUpdateDlg()
{
}

void CInUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DRV, m_cboDrv);
	DDX_Control(pDX, IDC_INPROT, m_cboInProt);
}


BEGIN_MESSAGE_MAP(CInUpdateDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CInUpdateDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInUpdateDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CInUpdateDlg::OnApplyMessage)
	ON_CBN_SELCHANGE(IDC_INPROT, &CInUpdateDlg::OnCbnSelchangeInprot)
	ON_CBN_SELCHANGE(IDC_COMBO_DRV, &CInUpdateDlg::OnCbnSelchangeComboDrv)
	ON_BN_CLICKED(IDC_P2P, &CInUpdateDlg::OnCbnSelchangeComboDrv)
	ON_EN_CHANGE(IDC_SPEED, &CInUpdateDlg::OnCbnSelchangeComboDrv)
END_MESSAGE_MAP()

BOOL CInUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString szDrv;
	for (char chDrv='D'; chDrv<='Z'; chDrv++)
	{
		szDrv.Format("%C", chDrv);
		m_cboDrv.AddString(szDrv);
	}
	
// 	m_cboInProt.AddString("后台延时穿透(推荐)");
	m_cboInProt.AddString("立即穿透");
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	szDrv= pDbMgr->GetOptString(OPT_M_GAMEDRV, "E").c_str();
	szDrv.MakeUpper();
	m_cboDrv.SelectString(-1, szDrv);

	m_cboInProt.SetCurSel(0);
// 	int nSpeed = pDbMgr->GetOptInt(OPT_M_INPROTTYPE, 0);
// 	SetDlgItemInt(IDC_SPEED, nSpeed);
// 	((CEdit*)GetDlgItem(IDC_SPEED))->LimitText(4);
// 	SetDlgItemInt(IDC_SPEED, nSpeed);
// 	GetDlgItem(IDC_SPEED)->EnableWindow(nSpeed);
// 	m_cboInProt.SetCurSel(nSpeed ? 0 : 1);
 	((CButton*)GetDlgItem(IDC_P2P))->SetCheck(pDbMgr->GetOptInt(OPT_M_P2P, 1));

	return TRUE;
}

void CInUpdateDlg::OnBnClickedOk()
{

}

void CInUpdateDlg::OnBnClickedCancel()
{
	
}

LRESULT CInUpdateDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		int nSel = m_cboDrv.GetCurSel();
		if (nSel == -1) nSel = 1;
		char buf[10] = {0};
		m_cboDrv.GetLBText(nSel, buf);
		pDbMgr->SetOption(OPT_M_GAMEDRV, buf);
		pDbMgr->SetOption(OPT_M_INPROTTYPE, 0);//GetDlgItemInt(IDC_SPEED));
		pDbMgr->SetOption(OPT_M_P2P, ((CButton*)GetDlgItem(IDC_P2P))->GetCheck() ? 1 : 0);
		m_bModify = FALSE;
	}
	return TRUE;
}

void CInUpdateDlg::OnCbnSelchangeInprot()
{
// 	int nSel = m_cboInProt.GetCurSel();
// 	if (nSel == 0)
// 	{
// 		SetDlgItemInt(IDC_SPEED, 1000);
// 		GetDlgItem(IDC_SPEED)->EnableWindow(TRUE);
// 	}
// 	else
// 	{
// 		SetDlgItemInt(IDC_SPEED, 0);
// 		GetDlgItem(IDC_SPEED)->EnableWindow(FALSE);
// 	}
	m_bModify = TRUE;
}

void CInUpdateDlg::OnCbnSelchangeComboDrv()
{
	m_bModify = TRUE;
}
