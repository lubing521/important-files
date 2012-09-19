// SysOptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "SysOptDlg.h"
#include "UserInfoDlg.h"
#include "MenuOptDlg.h"
#include "DirFileCleanDlg.h"
#include "DeskLnkDlg.h"
#include "WallPaperDlg.h"
#include "BootExec.h"
#include "GongGaoDlg.h"
#include "InUpdateDlg.h"
#include "ConsoleDlg.h"
#include "OptExtraTool.h"
#include "wizarddownloadboard.h"
#include "OptionPenetateFile.h"
#include "WZSafeCenterBoard.h"


IMPLEMENT_DYNAMIC(CSysOptDlg, CDialog)

CSysOptDlg::CSysOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysOptDlg::IDD, pParent)
	, m_nInitBoard(0)
{
	m_pLastDlg = NULL;
}

CSysOptDlg::~CSysOptDlg()
{
}

void CSysOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstBox);
	DDX_Control(pDX, IDC_OPTICON, m_btnIcon);
	DDX_Control(pDX, IDC_DESCRIPTION, m_btnDescription);
}

BEGIN_MESSAGE_MAP(CSysOptDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST, &CSysOptDlg::OnLbnSelchangeList)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDOK, &CSysOptDlg::OnBnClickedOk)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CSysOptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_OptList.push_back(tagItem(IDI_OPTION,	"用户帐号",		"设置和显示一些帐号相关信息"));
	m_OptList.push_back(tagItem(IDI_OPTION, "公告信息",		"设置菜单启动时弹出的公告信息"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"菜单设置",		"设置菜单操作的一些默认行为"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"三层设置",		"设置三层下载的基本属性"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"内网更新",		"设置内网多点更新的行为"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"磁盘清理",		"客户机开机时会执行清除的文件和目录"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"开机执行",		"客户机开机时会运行用户指定的文件"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"桌面墙纸",		"设置各个分区的桌面墙纸"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"桌面快捷方式",	"设置客户机开机后在桌面上创建的快捷方式"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"安全中心",		"设置安全中心的选项"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"辅助工具",		"设置辅助工具相关的选项"));
	m_OptList.push_back(tagItem(IDI_OPTION,	"定向升级",		"用于在客户机开机时，将服务器指定目录下的文件(含子目录)，穿透升级到客户机。"));
	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		m_lstBox.AddItem(m_OptList[idx].IconID,	m_OptList[idx].Caption);
	}
	
	m_lstBox.SetCurSel(m_nInitBoard);
	OnLbnSelchangeList();
	CenterWindow();
	return TRUE;
}

void CSysOptDlg::OnLbnSelchangeList()
{
	int nSel = m_lstBox.GetCurSel();
	if (nSel == -1)
		return ;

	CDialog* pDlg = m_OptList[nSel].pDlg;
	CWnd*    pWnd = GetDlgItem(IDC_AREA);
	if (pDlg == NULL)
	{
		switch (nSel)
		{
		case 0:
			{
				pDlg = new CUserInfoDlg;
				pDlg->Create(CUserInfoDlg::IDD, pWnd);			
			}
			break;
		case 1:
			{
				pDlg = new CGongGaoDlg;
				pDlg->Create(CGongGaoDlg::IDD, pWnd);
			}
			break;
		case 2:
			{
				pDlg = new CMenuOptDlg;
				pDlg->Create(CMenuOptDlg::IDD, pWnd);
			}
			break;
		case 3:
			{
				pDlg = new CWizardDownloadBoard;//CDwnSetDlg;
				pDlg->Create(CWizardDownloadBoard::IDD, pWnd);
			}
			break;
		case 4:
			{
 				pDlg = new CInUpdateDlg;
 				pDlg->Create(CInUpdateDlg::IDD, pWnd);
			}
			break;
		case 5:
			{
				pDlg = new CDirFileCleanDlg;
				pDlg->Create(CDirFileCleanDlg::IDD, pWnd);
			}
			break;
		case 6:
			{
				pDlg = new CBootExec;
				pDlg->Create(CBootExec::IDD, pWnd);	
			}
			break;
		case 7:
			{
				pDlg = new CWallPaperDlg;
				pDlg->Create(CWallPaperDlg::IDD, pWnd);	
			}
			break;
		case 8:
			{
				pDlg = new CDeskLnkDlg;
				pDlg->Create(CDeskLnkDlg::IDD, pWnd);	
			}
			break;
		case 9:
			{
				pDlg = new CWZSafeCenterBoard;
				pDlg->Create(CWZSafeCenterBoard::IDD, pWnd);	
			}
			break;
		case 10:
			{
				pDlg = new COptExtraTool;
				pDlg->Create(COptExtraTool::IDD, pWnd);	
			}
			break;
		case 11:
			{
				pDlg = new COptionPenetateFile;
				pDlg->Create(COptionPenetateFile::IDD, pWnd);	
			}
			break;
		}
	}

	if (m_pLastDlg != NULL && m_pLastDlg != pDlg)
	{
		m_pLastDlg->ShowWindow(SW_HIDE);
	}

	HICON hIcon = m_btnIcon.GetIcon();
	if (hIcon != NULL)
		DestroyIcon(hIcon);
	m_btnIcon.SetIcon(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_OptList[nSel].IconID)));
	m_btnDescription.SetWindowText(m_OptList[nSel].Description);

	if (pDlg != NULL && m_pLastDlg != pDlg)
	{
		CRect rc;
		GetDlgItem(IDC_AREA)->GetWindowRect(&rc);
		pWnd->ScreenToClient(&rc);
		rc.DeflateRect(1, 1, 1, 1);
		pDlg->MoveWindow(&rc);
		pDlg->ShowWindow(SW_SHOW);
	}
	m_OptList[nSel].pDlg = pDlg;
	m_pLastDlg = pDlg;
	m_lstBox.SetFocus();
}

BOOL CSysOptDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CSysOptDlg::OnDestroy()
{
	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		if (m_OptList[idx].pDlg != NULL)
		{
			if (m_OptList[idx].pDlg->m_hWnd != NULL)
				m_OptList[idx].pDlg->DestroyWindow();
			delete m_OptList[idx].pDlg;
		}
	}
	CDialog::OnDestroy();
}

void CSysOptDlg::OnBnClickedOk()
{
	CWaitCursor wc;
	for (size_t idx=0; idx<m_OptList.size(); idx++)
	{
		if (m_OptList[idx].pDlg != NULL && m_OptList[idx].pDlg->m_hWnd != NULL)
		{
			if (FALSE == ::SendMessage(m_OptList[idx].pDlg->m_hWnd, i8desk::g_nOptApplyMsg, 0, 0))
			{
				wc.Restore();
				m_lstBox.SetCurSel(idx);
				OnLbnSelchangeList();
				return ;
			}
		}
	}
	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	std::string ErrInfo;
	pDbMgr->NotifySvrCheck(ErrInfo);
	wc.Restore();
	OnOK();
}