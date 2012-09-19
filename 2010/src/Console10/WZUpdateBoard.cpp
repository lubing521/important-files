// WZUpdateBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZUpdateBoard.h"


// CWZUpdateBoard 对话框

IMPLEMENT_DYNAMIC(CWZUpdateBoard, CDialog)

CWZUpdateBoard::CWZUpdateBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZUpdateBoard::IDD, pParent)
	, m_strCliDrv(_T(""))
	, m_nInProt(0)
	, m_nP2P(-1)
{

}

CWZUpdateBoard::~CWZUpdateBoard()
{
}

void CWZUpdateBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZUpdateBoard::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	i8desk::CDbMgr* pDbMgr = GetDbMgr();

	CComboBox *pDrvCtrl = (CComboBox *)GetDlgItem(IDC_COMBO_DRV);
	for (char chDrv = 'D'; chDrv <= 'Z'; chDrv++)
	{
		char szDrv[2] = { chDrv };
		pDrvCtrl->AddString(szDrv);
	}
	
	CString strDrv = pDbMgr->GetOptString(OPT_M_GAMEDRV, "E").c_str();
	strDrv.MakeUpper();

	pDrvCtrl->SelectString(-1, strDrv);

	((CComboBox *)GetDlgItem(IDC_INPROT))->AddString("立即穿透");
	((CComboBox *)GetDlgItem(IDC_INPROT))->SetCurSel(0);
	((CButton*)GetDlgItem(IDC_P2P))->SetCheck(pDbMgr->GetOptInt(OPT_M_P2P, 0));

	return TRUE;
}

BEGIN_MESSAGE_MAP(CWZUpdateBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZUpdateBoard::OnApplyMessage)
	ON_CBN_SELCHANGE(IDC_COMBO_DRV, &CWZUpdateBoard::OnCbnSelchangeComboDrv)
	ON_CBN_SELCHANGE(IDC_INPROT, &CWZUpdateBoard::OnCbnSelchangeInprot)
	ON_BN_CLICKED(IDC_P2P, &CWZUpdateBoard::OnBnClickedP2p)
END_MESSAGE_MAP()


// CWZUpdateBoard 消息处理程序
LRESULT CWZUpdateBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (!this->m_strCliDrv.IsEmpty())
	{
		GetDbMgr()->SetOption(OPT_M_GAMEDRV, (LPCTSTR)m_strCliDrv);
		m_strCliDrv = _T("");
	}

	if (this->m_nInProt != -1)
	{
		GetDbMgr()->SetOption(OPT_M_INPROTTYPE, m_nInProt);
		m_nInProt = -1;
	}

	if (this->m_nP2P != -1)
	{
		GetDbMgr()->SetOption(OPT_M_P2P, m_nP2P);
		m_nP2P = -1;
	}

	return TRUE;
}

void CWZUpdateBoard::OnCbnSelchangeComboDrv()
{
	GetDlgItem(IDC_COMBO_DRV)->GetWindowText(m_strCliDrv);
}

void CWZUpdateBoard::OnCbnSelchangeInprot()
{
	m_nInProt = ((CComboBox*)GetDlgItem(IDC_INPROT))->GetCurSel();
}

void CWZUpdateBoard::OnBnClickedP2p()
{
	m_nP2P = ((CButton*)GetDlgItem(IDC_P2P))->GetCheck();
}
