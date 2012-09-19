// ViewGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "ViewGameDlg.h"
#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CViewGameDlg, CDialog)

CViewGameDlg::CViewGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewGameDlg::IDD, pParent)
{
	m_bViewCmpAllGame = true;
}

CViewGameDlg::~CViewGameDlg()
{
}

void CViewGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstCtrl);
}


BEGIN_MESSAGE_MAP(CViewGameDlg, CDialog)
END_MESSAGE_MAP()

BOOL CViewGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_lstCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	if (m_bViewCmpAllGame)
	{
		m_lstCtrl.InsertColumn(0, "GID",		LVCFMT_LEFT, 80);
		m_lstCtrl.InsertColumn(1, "游戏名",		LVCFMT_LEFT, 140);
		m_lstCtrl.InsertColumn(2, "更新时间",   LVCFMT_LEFT, 150);
	}
	else
	{
		m_lstCtrl.InsertColumn(0, "计算机",		LVCFMT_LEFT, 150);
		m_lstCtrl.InsertColumn(1, "更新时间",   LVCFMT_LEFT, 180);
	}

	std::string ErrInfo;
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());	
	if (!pDlg->m_pDbMgr->ViewGameInfo(gid, m_bViewCmpAllGame, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	CMarkup xml;
	xml.SetDoc(ErrInfo);
	if (m_bViewCmpAllGame)
	{
		if (i8desk::IntoXmlNode(xml, "/status/gameLst/"))
		{
			while (xml.FindElem("Task"))
			{
				std::string gid = xml.GetAttrib("gid");
				int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), gid.c_str());
				DWORD ver = atoi(xml.GetAttrib("version").c_str());
				m_lstCtrl.SetItemText(nIdx, 1, pDlg->GetNameFromGID(atoi(gid.c_str())).c_str());
				m_lstCtrl.SetItemText(nIdx, 2, i8desk::MakeTimeString(ver).c_str());
			}
		}
	}
	else
	{
		if (i8desk::IntoXmlNode(xml, "/status/ipLst/"))
		{
			while (xml.FindElem("Task"))
			{
				DWORD ip = atoi(xml.GetAttrib("ip").c_str());
				int nIdx = m_lstCtrl.InsertItem(m_lstCtrl.GetItemCount(), i8desk::MakeIpString(ip).c_str());
				DWORD ver = atoi(xml.GetAttrib("version").c_str());
				m_lstCtrl.SetItemText(nIdx, 1, i8desk::MakeTimeString(ver).c_str());
			}
		}
	}

	this->CenterWindow();

	return TRUE;
}