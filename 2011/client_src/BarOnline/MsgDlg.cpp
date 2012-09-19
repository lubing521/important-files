// MsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarOnline.h"
#include "MsgDlg.h"

#include "UptPrgDlg.h"
#include "I8UpdateGameDll.h"

// CMsgDlg 对话框

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIP,		m_btnTip);
	DDX_Control(pDX, IDC_CLOSE,		m_btnClose);
	DDX_Control(pDX, IDOK,			m_btnOK);
	DDX_Control(pDX, IDC_CONTENT,	m_btnContent);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CMsgDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLOSE, &CMsgDlg::OnBnClickedClose)
END_MESSAGE_MAP()


BOOL CMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rcClient(0, 0, 463, 215);
	CRgn rgn;
	rgn.CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 5, 5);

	CRect rc = rcClient;
	rc.left = rc.right - 51;
	rc.right = rc.right - 10;
	rc.bottom = rc.top + 24;
	m_btnClose.MoveWindow(&rc);
	m_btnClose.SetImage(TEXT("Skin/Wnd/Msg/x.png"), TEXT("Skin/Wnd/Msg/x_a.png"));

	rc.left = 10; rc.top = 5;
	rc.bottom = rc.top + 24; rc.right = rc.left + 100;
	m_btnTip.MoveWindow(&rc);
	m_btnTip.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 255), RGB(255, 255, 255),
		RGB(255, 255, 255), RGB(255, 255, 255));

	rc.left = 200; rc.top = 175;
	rc.right = rc.left + 97; rc.bottom = rc.top + 25;
	m_btnOK.SetImage(TEXT("Skin/Wnd/Msg/button01.png"));
	m_btnOK.MoveWindow(&rc);

	m_btnContent.MoveWindow(10, 40, 440, 80);
	m_btnContent.SetWindowText(m_strContent);
	m_btnContent.SetTextStyle(DEFAULT_FONT_NAME, 12, RGB(255, 255, 0), RGB(255, 255, 0),
		RGB(255, 255, 0), RGB(255, 255, 0), I8SkinCtrl_ns::CI8OldButton::BTEXT_CENTER);

	MoveWindow(&rcClient, FALSE);
	CenterWindow();
	SetWindowRgn(rgn, TRUE);

	return TRUE;
}

BOOL CMsgDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	I8SkinCtrl_ns::I8_DrawImage(pDC, TEXT("Skin/Wnd/Msg/bg_b.png"), rc);
	return TRUE;
}

LRESULT CMsgDlg::OnNcHitTest(CPoint point)
{
	UINT nHitTest = CDialog::OnNcHitTest(point);
	if (nHitTest == HTCLIENT)
		return HTCAPTION;
	return nHitTest;
}

HBRUSH CMsgDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brush(RGB(255, 255, 255));
	pDC->SetBkMode(1);
	pDC->SetBkColor(RGB(255, 255, 255));

	return brush; //hbr;
}

void CMsgDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}

void CMsgDlg::OnBnClickedClose()
{
	CDialog::OnCancel();
}

IMPLEMENT_DYNAMIC(CInputMethodDlg, CMsgDlg)

CInputMethodDlg::CInputMethodDlg(CWnd* pParent /*=NULL*/)
: CMsgDlg(pParent)
{
    m_hUpdate = NULL;
	m_pGameInfo = NULL;
}

CInputMethodDlg::~CInputMethodDlg()
{
}

BEGIN_MESSAGE_MAP(CInputMethodDlg, CMsgDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CInputMethodDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CInputMethodDlg::OnInitDialog()
{
	CMsgDlg::OnInitDialog();
	m_btnClose.ShowWindow(SW_HIDE);
	m_btnOK.ShowWindow(SW_HIDE);
	m_pGameInfo = AfxGetDbMgr()->FindGameInfo(40724);
	if (m_pGameInfo == NULL)
	{
		return FALSE;
	}
	m_btnContent.SetWindowText(TEXT("正在更新输入法工具，请稍候。。。"));

	CString str = AfxGetDbMgr()->LoadIpAddr();
	str += TEXT("|");
	str += AfxGetDbMgr()->GetUptSvrList();
	//	_bstr_t ip = str
	m_hUpdate = CI8UpdateGameDll::Instance().UG_StartUpdate(m_pGameInfo->gid,
        NULL,
		_bstr_t(m_pGameInfo->SvrPath), 
		_bstr_t(m_pGameInfo->CliPath),		  
		UPDATE_FLAG_QUICK_COMPARE|UPDATE_FLAG_DELETE_MOREFILE|UPDATE_FLAG_DELETE_GAME,
		str,
        2);

	SetTimer(1, 1000, NULL);

	return TRUE;
}

void CInputMethodDlg::OnBnClickedOk()
{
}

void CInputMethodDlg::OnCancel()
{
}

void CInputMethodDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		if (m_hUpdate != NULL)
		{
			CString str;
			tagUpdateStatus stStatus;
			stStatus.cbSize = sizeof(tagUpdateStatus);
			CI8UpdateGameDll::Instance().UG_GetUpdateStatus(m_hUpdate, &stStatus);

			if (UPDATE_STATUS_FINISH == stStatus.Status)
			{
				CI8UpdateGameDll::Instance().UG_StopUpdate(m_hUpdate);
				m_hUpdate = NULL;
				KillTimer(1);
				CUptPrgDlg::RunGame(m_pGameInfo);
				CDialog::OnOK();				
			}
			else if (UPDATE_STATUS_ERROR == stStatus.Status)
			{
				CI8UpdateGameDll::Instance().UG_StopUpdate(m_hUpdate);
				m_hUpdate = NULL;
				KillTimer(1);
				CDialog::OnOK();
			}
			else if (UPDATE_STATUS_UPDATE == stStatus.Status)
			{
				CString strText;
				//strText.Format(TEXT("正在更新输入法工具，当前进度:%2ld%%"), (DWORD)(stStatus.m_qUpdatedBytes/10));
				m_btnContent.SetWindowText(strText);
				Invalidate();
				UpdateWindow();
			}
		}
	}

	CMsgDlg::OnTimer(nIDEvent);
}

void CInputMethodDlg::OnDestroy()
{
	CWaitCursor wc;
	if (m_hUpdate != NULL)
	{
		CI8UpdateGameDll::Instance().UG_StopUpdate(m_hUpdate);
		m_hUpdate = NULL;
	}
	CMsgDlg::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}

void AfxMessageBoxEx(LPCTSTR lpszText)
{
    CMsgDlg dlg;
    dlg.m_strContent = lpszText;
    dlg.DoModal();
}
