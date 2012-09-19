// RigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskClient.h"
#include "RigDlg.h"


// CRigDlg 对话框

IMPLEMENT_DYNAMIC(CRigDlg, CDialog)

CRigDlg::CRigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRigDlg::IDD, pParent)
{

}

CRigDlg::~CRigDlg()
{
}

void CRigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRigDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CRigDlg 消息处理程序

void CRigDlg::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1,m_strName);
	GetDlgItemText(IDC_EDIT2,m_strPassword);
	CString strtmp;
	GetDlgItemText(IDC_EDIT3,strtmp);
	if(strtmp.Compare(m_strPassword) != 0)
	{
		AfxMessageBox("两次输入密码不一样");
		return ;
	}
	if(m_strName.IsEmpty())
	{
		AfxMessageBox("用户名不能为空");
		return ;
	}
	if(m_strPassword.IsEmpty())
	{
		AfxMessageBox("密码不能为空");
		return ;
	}
	OnOK();
}

HBRUSH CRigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	static CBrush brush;
	if (brush.m_hObject == NULL)
	{
		brush.CreateSolidBrush(RGB(255, 255, 255));
	}
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (nCtlColor == CTLCOLOR_STATIC )
	{
		pDC->SetBkMode(1);
		return brush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

BOOL CRigDlg::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);
	static CBrush brush(RGB(255,255,255));
	RECT rt;
	GetClientRect(&rt);
	rt.bottom-=40;
	pDC->FillRect(&rt,&brush);
	return TRUE;
}
