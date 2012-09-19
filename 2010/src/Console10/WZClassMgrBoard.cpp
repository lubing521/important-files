// WZClassMgrBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZClassMgrBoard.h"

#include "ConsoleDlg.h"

// CWZClassMgrBoard 对话框

IMPLEMENT_DYNAMIC(CWZClassMgrBoard, CDialog)

CWZClassMgrBoard::CWZClassMgrBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZClassMgrBoard::IDD, pParent)
{

}

CWZClassMgrBoard::~CWZClassMgrBoard()
{
}

void CWZClassMgrBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZClassMgrBoard::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CWZClassMgrBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZClassMgrBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZClassMgrBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_RADIO1, &CWZClassMgrBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZClassMgrBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZClassMgrBoard 消息处理程序
LRESULT CWZClassMgrBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZClassMgrBoard::OnBnClickedButton()
{
	GetConsoleDlg()->OnGameClassMgr();
}

void CWZClassMgrBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZClassMgrBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
