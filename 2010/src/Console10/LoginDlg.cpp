// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "Console.h"
#include "LoginDlg.h"
#include "ConsoleDlg.h"

#include "oem.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	m_brush.CreateSolidBrush(RGB(255, 255, 255));
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()




BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CConsoleApp *app = (CConsoleApp *)AfxGetApp();
	this->SetWindowText(app->m_pszAppName);

	CString szText = AfxGetApp()->GetProfileString("Option", "SvrIP", "127.0.0.1");
	SetDlgItemText(IDC_IP, szText);
	szText = AfxGetApp()->GetProfileString("Option", "Password", "");
	//SetDlgItemText(IDC_PASSWORD, szText);
	
	//设置oem的logo图片
	HBITMAP hBitmap = 0;
	if (I8DESK_OEM->LoadLogo(hBitmap)) {
		ASSERT (hBitmap);
		CStatic *logo = (CStatic *)this->GetDlgItem(IDC_STATIC);
		HBITMAP oldBitmap = logo->SetBitmap(hBitmap);
		::DeleteObject(oldBitmap);
	}

	return TRUE;
}

BOOL CLoginDlg::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);

	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.bottom -= 35;
	pDC->FillRect(&rcClient, &CBrush(RGB(255, 255, 255)));

	return TRUE;
}

HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(1);
		return m_brush;
	}

	return hbr;
}

void CLoginDlg::OnPaint()
{
	CPaintDC dc(this);
}

void CLoginDlg::OnBnClickedOk()
{
	CString szFull, szIP, szPort, szPassword;
	GetDlgItemText(IDC_IP,	 szFull);
	GetDlgItemText(IDC_PASSWORD, szPassword);

	AfxExtractSubString(szIP, szFull, 0, ':');
	AfxExtractSubString(szPort, szFull, 1, ':');
	szIP.Trim(); szPort.Trim();
	if (szPort.IsEmpty())
		szPort.Format("%d", RTDATASVR_PORT);

	m_strSvrIP = szIP;
	m_dwPort   = atoi(szPort);

	CString Item1, Item2, Item3, Item4;
	AfxExtractSubString(Item1, szIP, 0, '.');
	AfxExtractSubString(Item2, szIP, 1, '.');
	AfxExtractSubString(Item3, szIP, 2, '.');
	AfxExtractSubString(Item4, szIP, 3, '.');

	if (Item1.IsEmpty() || Item2.IsEmpty() || Item3.IsEmpty() || Item4.IsEmpty() ||
		atoi(Item1)> 255 || atoi(Item2)>255 || atoi(Item3) > 255 || atoi(Item4) > 255)
	{
		AfxMessageBox("服务器ip地址设置错误.");
		GetDlgItem(IDC_IP)->SetFocus();
		((CEdit*)GetDlgItem(IDC_IP))->SetSel(0, 999);
		return ;
	}
	CWaitCursor wc;
	CConsoleDlg* pDlg = (CConsoleDlg*)AfxGetMainWnd();
	const int timeout = 15; //秒
	if (!pDlg->m_pSocket->ConnectServer(m_strSvrIP, m_dwPort, timeout)) {
		AfxMessageBox("连接实时数据服务超时!");
		return ;
	}

	//连接到下载管理服务
	if (!pDlg->m_pGameMgrSocket->ConnectServer(m_strSvrIP, GAMEMGR_PORT, timeout)) {
		AfxMessageBox("连接下载服务超时!");
		return ;
	}
	
	i8desk::std_string strPassword = pDlg->m_pDbMgr->GetOptString(OPT_U_CTLPWD, "1234567");
	if (!pDlg->IsLocMachineIP(m_strSvrIP) &&  strPassword != (LPCTSTR)szPassword)
	{
		pDlg->m_pSocket->CloseServer();
		AfxMessageBox("控制台密码不正确.请重新输入密码");
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		((CEdit*)GetDlgItem(IDC_PASSWORD))->SetSel(0, 9999);
		return ;
	}
	
	AfxGetApp()->WriteProfileString("Option", "SvrIP", szFull);
	//AfxGetApp()->WriteProfileString("Option", "Password", szPassword);

	std::string ErrInfo = pDlg->m_pDbMgr->GetOptString(OPT_U_ERRINFO);
	if (ErrInfo == "您暂时不能使用i8desk 2009版")
	{
		wc.Restore();
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return ;
	}
	wc.Restore();
	OnOK();
}
