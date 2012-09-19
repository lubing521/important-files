// WZAddGameBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZAddGameBoard.h"


// CWZAddGameBoard 对话框

IMPLEMENT_DYNAMIC(CWZAddGameBoard, CDialog)

CWZAddGameBoard::CWZAddGameBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZAddGameBoard::IDD, pParent)
{

}

CWZAddGameBoard::~CWZAddGameBoard()
{
}

void CWZAddGameBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZAddGameBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	LOGFONT lf;
	this->GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_font.CreateFontIndirect(&lf);

	GetDlgItem(IDC_RADIO1)->SetFont(&m_font);
	GetDlgItem(IDC_RADIO2)->SetFont(&m_font);

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);	

	return TRUE;
}

BEGIN_MESSAGE_MAP(CWZAddGameBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZAddGameBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZAddGameBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_RADIO1, &CWZAddGameBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZAddGameBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZAddGameBoard 消息处理程序
LRESULT CWZAddGameBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZAddGameBoard::OnBnClickedButton()
{
	GetConsoleDlg()->OnGamePlAdd();
}

void CWZAddGameBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZAddGameBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
