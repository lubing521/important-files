// WZAreaMgrBorad.cpp : 实现文件
//

#include "stdafx.h"
#include "WZAreaMgrBorad.h"


// CWZAreaMgrBoard 对话框

IMPLEMENT_DYNAMIC(CWZAreaMgrBoard, CDialog)

CWZAreaMgrBoard::CWZAreaMgrBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZAreaMgrBoard::IDD, pParent)
{

}

CWZAreaMgrBoard::~CWZAreaMgrBoard()
{
}

void CWZAreaMgrBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZAreaMgrBoard::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CWZAreaMgrBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZAreaMgrBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZAreaMgrBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_RADIO1, &CWZAreaMgrBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZAreaMgrBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZAreaMgrBoard 消息处理程序
LRESULT CWZAreaMgrBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZAreaMgrBoard::OnBnClickedButton()
{
	GetConsoleDlg()->OnGameAreaMgr();
}

void CWZAreaMgrBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZAreaMgrBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
