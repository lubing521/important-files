// DlgInfoTip.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgInfoTip.h"
#include "ConsoleDlg.h"

// CDlgInfoTip 对话框

IMPLEMENT_DYNAMIC(CDlgInfoTip, CDialog)

CDlgInfoTip::CDlgInfoTip(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInfoTip::IDD, pParent)
	, m_bShowCancel(FALSE)
{

}

CDlgInfoTip::~CDlgInfoTip()
{
}

void CDlgInfoTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInfoTip, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_INFO_TIP_OK, &CDlgInfoTip::OnBnClickedButtonInfoTipOk)
	ON_BN_CLICKED(IDC_INFO_TIP_CANCEL, &CDlgInfoTip::OnBnClickedCancel)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()




void CDlgInfoTip::_InitCtrl()
{
	// 是否有取消按钮
	m_bShowCancel = GetConsoleDlg()->m_pDbMgr->GetOptInt(OPT_U_CONSHOWCANCEL);
	if( m_bShowCancel == FALSE )
	{
		CRect rcCancelRect;
		GetDlgItem(IDC_INFO_TIP_CANCEL)->GetWindowRect(rcCancelRect);
		GetDlgItem(IDC_INFO_TIP_CANCEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INFO_TIP_CANCEL)->DestroyWindow();
		
		ScreenToClient(rcCancelRect);
		GetDlgItem(IDC_BUTTON_INFO_TIP_OK)->MoveWindow(rcCancelRect);
	}
}

void CDlgInfoTip::_InitUrl()
{
	// 读取URL
	m_strUrl = GetConsoleDlg()->m_pDbMgr->GetOptString(OPT_U_CONOKURL);
}


void CDlgInfoTip::_InitContent()
{
	// 读取提示内容
	m_strContent = GetConsoleDlg()->m_pDbMgr->GetOptString(OPT_U_CONMESSAGE);
	SetDlgItemText(IDC_STATIC_INFO_TIP_MESSAGE, m_strContent.c_str());
}


// CDlgInfoTip 消息处理程序

BOOL CDlgInfoTip::OnInitDialog()
{
	CDialog::OnInitDialog();

	_InitCtrl();

	_InitContent();

	_InitUrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgInfoTip::OnBnClickedButtonInfoTipOk()
{
	if( m_strUrl.empty() == false )
	{
		ShellExecute(GetSafeHwnd(), _T("open"), m_strUrl.c_str(), _T(""), _T(""), SW_SHOWNORMAL);
	}

	OnCancel();
}

void CDlgInfoTip::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CDlgInfoTip::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}

int CDlgInfoTip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	/*CMenu *pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	VERIFY(pSysMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND));*/

	return CDialog::OnCreate(lpCreateStruct);
}

BOOL CDlgInfoTip::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )      
	{      
		if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )      
		{      
			return   TRUE;      
		}      
	}        

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgInfoTip::OnSysCommand(UINT nID, LPARAM lParam)
{
	////add the following code
	//if ( ( nID & 0xFFF0 ) == SC_CLOSE ) 
	//{
	//	//if user clicked the "X"
	//	//OnExit();
	//	//---end of code you have added
	//}
	//else 
	//{
	//	CDialog::OnSysCommand( nID, lParam );
	//}

	CDialog::OnSysCommand( nID, lParam );
}
