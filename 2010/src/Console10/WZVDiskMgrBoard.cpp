// WZVDiskMgrBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "WZVDiskMgrBoard.h"

// CWZVDiskMgrBoard 对话框

IMPLEMENT_DYNAMIC(CWZVDiskMgrBoard, CDialog)

CWZVDiskMgrBoard::CWZVDiskMgrBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CWZVDiskMgrBoard::IDD, pParent)
{

}

CWZVDiskMgrBoard::~CWZVDiskMgrBoard()
{
}

void CWZVDiskMgrBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BOOL CWZVDiskMgrBoard::OnInitDialog()
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

BEGIN_MESSAGE_MAP(CWZVDiskMgrBoard, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWZVDiskMgrBoard::OnApplyMessage)
	ON_BN_CLICKED(IDC_BUTTON, &CWZVDiskMgrBoard::OnBnClickedButton)
	ON_BN_CLICKED(IDC_RADIO1, &CWZVDiskMgrBoard::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CWZVDiskMgrBoard::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CWZVDiskMgrBoard 消息处理程序
LRESULT CWZVDiskMgrBoard::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

void CWZVDiskMgrBoard::OnBnClickedButton()
{
	GetConsoleDlg()->OnSysVDisk();
}

void CWZVDiskMgrBoard::OnBnClickedRadio1()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(TRUE);
}

void CWZVDiskMgrBoard::OnBnClickedRadio2()
{
	GetDlgItem(IDC_BUTTON)->EnableWindow(FALSE);
}
