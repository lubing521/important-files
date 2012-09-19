// WZServerBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZServerBoard.h"


// CWZServerBoard 对话框

IMPLEMENT_DYNAMIC(CWZServerBoard, CDialog)

CWZServerBoard::CWZServerBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZServerBoard::IDD, pParent)
{

}

CWZServerBoard::~CWZServerBoard()
{
}

void CWZServerBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZServerBoard::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CWZServerBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZServerBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZServerBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_RADIO1, &CWZServerBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZServerBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZServerBoard 消息处理程序
LRESULT CWZServerBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZServerBoard::OnBnClickedButton()
{
	GetConsoleDlg()->OnSysMirrorServer();
}

void CWZServerBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZServerBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
