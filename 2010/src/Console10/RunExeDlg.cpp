// RunExeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "RunExeDlg.h"


// CRunExeDlg 对话框

IMPLEMENT_DYNAMIC(CRunExeDlg, CDialog)

CRunExeDlg::CRunExeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRunExeDlg::IDD, pParent)
{

}

CRunExeDlg::~CRunExeDlg()
{
}

void CRunExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRunExeDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRunExeDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CRunExeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}
void CRunExeDlg::OnBnClickedOk()
{
	GetDlgItemText(IDC_EXEFILE, m_strExeFile);
	if (m_strExeFile.IsEmpty())
		return ;
	OnOK();
}
