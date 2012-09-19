// RepairDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "RepairDialog.h"


// CRepairDialog 对话框

IMPLEMENT_DYNAMIC(CRepairDialog, CDialog)

CRepairDialog::CRepairDialog(const CString& nid, CWnd* pParent /*=NULL*/)
	: CDialog(CRepairDialog::IDD, pParent)
	, m_strNid(nid)
{
}

CRepairDialog::~CRepairDialog()
{
}

void CRepairDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRepairDialog, CDialog)
END_MESSAGE_MAP()


// CRepairDialog 消息处理程序

BOOL CRepairDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nCaption = ::GetSystemMetrics(SM_CYCAPTION);
	int nXBorder = ::GetSystemMetrics(SM_CXBORDER);
	int nYBorder = ::GetSystemMetrics(SM_CYBORDER);

	int nWidth = 550 + nXBorder * 3 * 2;
	int nHeight = 445 + nCaption + nYBorder * 3 * 2;

	this->MoveWindow(0, 0, nWidth, nHeight);
	this->CenterWindow();             

	RECT rc;
	this->GetClientRect(&rc);
	m_WebPage.Create(NULL, NULL, WS_VISIBLE|WS_CHILD, rc, this, AFX_IDW_PANE_FIRST);
	m_WebPage.SetNoScroll();
	m_WebPage.SetSilent(TRUE);

	CString url(_T("http://desk.i8cn.com/ComplainManage/SMSRepair.aspx?AgentID="));
	url += m_strNid;

	m_WebPage.Navigate2(url);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
