// SCBaseConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SCBaseConfigDlg.h"


// CSCBaseConfigDlg 对话框

IMPLEMENT_DYNAMIC(CSCBaseConfigDlg, CDialog)

CSCBaseConfigDlg::CSCBaseConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCBaseConfigDlg::IDD, pParent)
{

}

CSCBaseConfigDlg::~CSCBaseConfigDlg()
{
}

void CSCBaseConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSCBaseConfigDlg, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CSCBaseConfigDlg::OnApplyMessage)
END_MESSAGE_MAP()

// CSCBaseConfigDlg 消息处理程序

LRESULT CSCBaseConfigDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	return 1;
}


