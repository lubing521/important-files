// LogDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskClient.h"
#include "LogDialog.h"
#include "RigDlg.h"

// CLogDialog 对话框

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)

CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{

}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CLogDialog::OnBnClickedOk)
	ON_BN_CLICKED(BTN_Register, &CLogDialog::OnBnClickedRegister)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CLogDialog 消息处理程序

void CLogDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
// 	SetDlgItemText(IDC_EDIT1,"dengzailin");
// 	SetDlgItemText(IDC_EDIT2,"123456");

	GetDlgItemText(IDC_EDIT1,m_strUsername);
	GetDlgItemText(IDC_EDIT2,m_strPassword);
	if(m_strPassword.IsEmpty())
	{
		AfxMessageBox("密码不能为空！");
		return ;
	}
	if(m_strUsername.IsEmpty())
	{
		AfxMessageBox("用户名不能为空！");
		return ;
	}
	OnOK();
}

void CLogDialog::OnBnClickedRegister()
{
	// TODO: 在此添加控件通知处理程序代码

	CRigDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString str = dlg.m_strName+"|"+dlg.m_strPassword;
		char* pData = NULL;
		DWORD nRecv = 0;
		if(theApp.m_pSocket->ExeCommand(NET_CMD_REGISTCLIENT,pData,nRecv,(LPSTR)(LPCSTR)str,str.GetLength()))
		{
			CPackageHelper outpack(pData);
			char buf[1024] = {0};
			outpack.popString(buf);
			AfxMessageBox(buf);
			SetDlgItemText(IDC_EDIT1,dlg.m_strName);
			SetDlgItemText(IDC_EDIT2,dlg.m_strPassword);
			delete []pData;
		}
	}

}

BOOL CLogDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_EDIT1)->SetFocus();   
	// TODO:  在此添加额外的初始化
	SetWindowText(theApp.m_pszAppName);
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CLogDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnEraseBkgnd(pDC);
	static CBrush brush(RGB(255,255,255));
	RECT rt;
	GetClientRect(&rt);
	rt.bottom-=40;
	pDC->FillRect(&rt,&brush);
	return TRUE;
}

HBRUSH CLogDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	static CBrush brush;
	if (brush.m_hObject == NULL)
	{
		brush.CreateSolidBrush(RGB(255, 255, 255));
	}
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (nCtlColor == CTLCOLOR_STATIC  )
	{
		pDC->SetBkMode(TRANSPARENT);
		return brush;
	}
	if(pWnd->GetDlgCtrlID() == BTN_Register)
	{
		pDC->SetBkMode(TRANSPARENT);
		return brush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
