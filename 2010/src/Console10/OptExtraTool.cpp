// OptExtraTool.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "OptExtraTool.h"
#include "ConsoleDlg.h"


// COptExtraTool 对话框

IMPLEMENT_DYNAMIC(COptExtraTool, CDialog)

COptExtraTool::COptExtraTool(CWnd* pParent /*=NULL*/)
	: CDialog(COptExtraTool::IDD, pParent)
	, m_bModify(FALSE)
	, m_bUseUdisk(FALSE)
{

}

COptExtraTool::~COptExtraTool()
{
}

void COptExtraTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_AUTO_START_IE_HISTORY, m_bAutoStartIeHistory);
	DDX_Check(pDX, IDC_USE_UDISK, m_bUseUdisk);
}


BEGIN_MESSAGE_MAP(COptExtraTool, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &COptExtraTool::OnApplyMessage)
	ON_BN_CLICKED(IDC_CHECK_AUTO_START_IE_HISTORY, &COptExtraTool::OnBnClickedCheckAutoStartIeHistory)
	ON_BN_CLICKED(IDC_USE_UDISK, &COptExtraTool::OnBnClickedUdisk)
END_MESSAGE_MAP()


BOOL COptExtraTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	m_bAutoStartIeHistory = pDbMgr->GetOptInt(OPT_M_IEHISTORY, 0);
	m_bUseUdisk = pDbMgr->GetOptInt(OPT_M_USEUDISK, 0);
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// COptExtraTool 消息处理程序
LRESULT COptExtraTool::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify) {
		CConsoleDlg* pMainWnd = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

		pMainWnd->m_pDbMgr->SetOption(OPT_M_IEHISTORY, m_bAutoStartIeHistory);
		pMainWnd->m_pDbMgr->SetOption(OPT_M_USEUDISK, m_bUseUdisk);
		
		if (m_bAutoStartIeHistory) {
			pMainWnd->OnIeHistory();
		}

		if (m_bUseUdisk) {
			pMainWnd->OnUDiskServer();
		}

		m_bModify = FALSE;
	}
	return TRUE;
}

void COptExtraTool::OnBnClickedCheckAutoStartIeHistory()
{
	m_bModify = TRUE;
	UpdateData();
}

void COptExtraTool::OnBnClickedUdisk()
{
	m_bModify = TRUE;
	UpdateData();
}