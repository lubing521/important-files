// ReSetPasswordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskClient.h"
#include "ReSetPasswordDlg.h"


// CReSetPasswordDlg 对话框

IMPLEMENT_DYNAMIC(CReSetPasswordDlg, CDialog)

CReSetPasswordDlg::CReSetPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReSetPasswordDlg::IDD, pParent)
{

}

CReSetPasswordDlg::~CReSetPasswordDlg()
{
}

void CReSetPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReSetPasswordDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CReSetPasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CReSetPasswordDlg 消息处理程序

void CReSetPasswordDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItemText(IDC_EDIT1,m_strOldPassword);
	GetDlgItemText(IDC_EDIT2,m_strNewPassword);
	CString str;
	GetDlgItemText(IDC_EDIT3,str);
	if(str.Compare(m_strNewPassword) != 0)
	{
		AfxMessageBox("两次输入的密码不一致");
		return;
	}
	OnOK();
}
