// WZSyncTaskBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZSyncTaskBoard.h"


// CWZSyncTaskBoard 对话框

IMPLEMENT_DYNAMIC(CWZSyncTaskBoard, CDialog)

CWZSyncTaskBoard::CWZSyncTaskBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZSyncTaskBoard::IDD, pParent)
{

}

CWZSyncTaskBoard::~CWZSyncTaskBoard()
{
}

void CWZSyncTaskBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZSyncTaskBoard::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CWZSyncTaskBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZSyncTaskBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZSyncTaskBoard::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RADIO1, &CWZSyncTaskBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZSyncTaskBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZSyncTaskBoard 消息处理程序
LRESULT CWZSyncTaskBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZSyncTaskBoard::OnBnClickedButton1()
{
	GetConsoleDlg()->OnSysSyncTask();
}

void CWZSyncTaskBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZSyncTaskBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
