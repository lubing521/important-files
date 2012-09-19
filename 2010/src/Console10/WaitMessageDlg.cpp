// WaitMessageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "WaitMessageDlg.h"

//背景颜色与文本颜色
#define BK_COLOR	RGB(128, 128, 255)
#define TEXT_COLOR	RGB(255, 255, 128)

// CWaitMessageDlg 对话框

IMPLEMENT_DYNAMIC(CWaitMessageDlg, CDialog)

CWaitMessageDlg::CWaitMessageDlg(LPCTSTR lpstr, CWnd* pParent /*=NULL*/)
	: CDialog(CWaitMessageDlg::IDD, pParent)
	, m_strMsg(lpstr)
	, m_progress(0)
{
	m_brush.CreateSolidBrush(BK_COLOR);

	//限制提示消息的长度以有恰当的显示效果
	ASSERT(m_strMsg.GetLength() < 24);
	m_strMsg += ".";
}

CWaitMessageDlg::~CWaitMessageDlg()
{
}

void CWaitMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWaitMessageDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CWaitMessageDlg 消息处理程序

BOOL CWaitMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_MSG)->SetWindowText(m_strMsg);
	
	this->SetTimer(1, 1000, NULL);

	s_event.SetEvent();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CWaitMessageDlg::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);
	CRect rcClient;
	GetClientRect(&rcClient);
	pDC->FillRect(&rcClient, &CBrush( BK_COLOR ));

	return TRUE;
}

HBRUSH CWaitMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(1);
		pDC->SetBkColor( BK_COLOR );
		if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_MSG)->m_hWnd)
		{
			pDC->SetTextColor( TEXT_COLOR );
		}
		return m_brush;
	}

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CWaitMessageDlg::OnTimer(UINT_PTR nIDEvent)
{
	int n = ++m_progress % MAX_DOT_NUM;

	CString str = m_strMsg;
	for (int i = 0; i < n; i++)
		str += ".";

	GetDlgItem(IDC_STATIC_MSG)->SetWindowText(str);

	CDialog::OnTimer(nIDEvent);
}

void CWaitMessageDlg::OnDestroy()
{
	this->KillTimer(1);

	CDialog::OnDestroy();
}

CEvent CWaitMessageDlg::s_event;