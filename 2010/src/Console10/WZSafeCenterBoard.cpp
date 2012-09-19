// WZSafeCenterBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZSafeCenterBoard.h"

#include "consoledlg.h"

// CWZSafeCenterBoard 对话框

IMPLEMENT_DYNAMIC(CWZSafeCenterBoard, CDialog)

CWZSafeCenterBoard::CWZSafeCenterBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZSafeCenterBoard::IDD, pParent)
	, m_bModify(FALSE)
{

}

CWZSafeCenterBoard::~CWZSafeCenterBoard()
{
}

void CWZSafeCenterBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZSafeCenterBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT lf;
	this->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_font.CreateFontIndirect(&lf);

	GetDlgItem(IDC_RADIO1)->SetFont(&m_font);
	GetDlgItem(IDC_RADIO2)->SetFont(&m_font);

	i8desk::CDbMgr* pDbMgr = GetDbMgr();

	int iWhichSC = pDbMgr->GetOptInt(OPT_M_WHICHONE_SC, 1);
	((CButton*)GetDlgItem(IDC_RADIO_I8_SC))->SetCheck(iWhichSC == 1);
	((CButton*)GetDlgItem(IDC_RADIO_KANGHE_SC))->SetCheck(iWhichSC == 2);

	m_bAutoStartSafeCenter = pDbMgr->GetOptInt(OPT_M_SAFECENTER, 0);

	if (m_bAutoStartSafeCenter)
	{
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);	
		GetDlgItem(IDC_RADIO_I8_SC)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_KANGHE_SC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);	
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);	
		GetDlgItem(IDC_RADIO_I8_SC)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_KANGHE_SC)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
	}
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CWZSafeCenterBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZSafeCenterBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZSafeCenterBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_CHECK_AUTO_START_SAFECENTER, &CWZSafeCenterBoard::OnBnClickedCheckAutoStartSafecenter)
	ON_BN_CLICKED(IDC_RADIO_I8_SC, &CWZSafeCenterBoard::OnBnClickedRadioI8Sc)
	ON_BN_CLICKED(IDC_RADIO_KANGHE_SC, &CWZSafeCenterBoard::OnBnClickedRadioKangheSc)
	ON_BN_CLICKED(IDC_RADIO1, &CWZSafeCenterBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZSafeCenterBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZSafeCenterBoard 消息处理程序
LRESULT CWZSafeCenterBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		GetDbMgr()->SetOption(OPT_M_SAFECENTER, m_bAutoStartSafeCenter);
		
		if (((CButton*)GetDlgItem(IDC_RADIO_I8_SC))->GetCheck())
			GetDbMgr()->SetOption(OPT_M_WHICHONE_SC, 1L);

		if (((CButton*)GetDlgItem(IDC_RADIO_KANGHE_SC))->GetCheck())
			GetDbMgr()->SetOption(OPT_M_WHICHONE_SC, 2L);

		m_bModify = FALSE;
	}

	return TRUE;
}

void CWZSafeCenterBoard::OnBnClickedButton()
{
	OnApplyMessage(0, 0);
	GetConsoleDlg()->SafeCenterConfig();
}

void CWZSafeCenterBoard::OnBnClickedCheckAutoStartSafecenter()
{
	m_bModify = TRUE;
}

void CWZSafeCenterBoard::OnBnClickedRadioI8Sc()
{
	m_bModify = TRUE;
}

void CWZSafeCenterBoard::OnBnClickedRadioKangheSc()
{
	m_bModify = TRUE;
}

void CWZSafeCenterBoard::OnBnClickedRadio1()
{
	m_bModify = TRUE;
	m_bAutoStartSafeCenter = TRUE;

	GetDlgItem(IDC_RADIO_I8_SC)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_KANGHE_SC)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZSafeCenterBoard::OnBnClickedRadio2()
{
	m_bModify = TRUE;
	m_bAutoStartSafeCenter = FALSE;

	GetDlgItem(IDC_RADIO_I8_SC)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_KANGHE_SC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
