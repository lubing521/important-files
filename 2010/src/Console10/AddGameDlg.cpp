// AddGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "AddGameDlg.h"
#include "ConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CAddGameDlg, CDialog)

CAddGameDlg::CAddGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddGameDlg::IDD, pParent)
{

}

CAddGameDlg::~CAddGameDlg()
{
}

void CAddGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstGame);
}


BEGIN_MESSAGE_MAP(CAddGameDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddGameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT, &CAddGameDlg::OnBnClickedSelect)
	ON_BN_CLICKED(IDC_UNSELECT, &CAddGameDlg::OnBnClickedUnselect)
END_MESSAGE_MAP()

BOOL CAddGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	m_lstGame.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_lstGame.InsertColumn(0, "GID",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(1, "游戏名", LVCFMT_LEFT, 120);
	m_lstGame.InsertColumn(2, "大小",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(3, "热门度",	LVCFMT_LEFT, 60);
	m_lstGame.InsertColumn(4, "等级",	LVCFMT_LEFT, 60);

	std::set<DWORD> setGame;
	pDlg->GetAllSelectGame(setGame);
	i8desk::GameInfoMapItr Iter = m_GameInfos.begin();
	//i8desk::GameInfoMapItr Iter = m_GameInfos.begin();
	CString msg;
	for (; Iter != m_GameInfos.end(); Iter++)
	{
		msg.Format("%05d", Iter->second->GID);
		int nItem = m_lstGame.InsertItem(m_lstGame.GetItemCount(), msg);
		m_lstGame.SetItemText(nItem, 1, Iter->second->Name);
		msg.Format("%0.2fM", Iter->second->Size*1.0/1024);
		m_lstGame.SetItemText(nItem, 2, msg);
		msg.Format("%d", Iter->second->IdcClick);
		m_lstGame.SetItemText(nItem, 3, msg);
		m_lstGame.SetItemText(nItem, 4, Iter->second->GetPriority().c_str());
		if (setGame.find(Iter->second->GID) != setGame.end())
		{
			m_lstGame.SetCheck(nItem);
		}
	}

	return TRUE;
}
void CAddGameDlg::OnBnClickedOk()
{
	m_IDs.clear();
	for (int nIDx=0; nIDx<m_lstGame.GetItemCount(); nIDx++)
	{
		if (m_lstGame.GetCheck(nIDx))
			m_IDs.insert(atoi(m_lstGame.GetItemText(nIDx, 0)));
	}
	OnOK();
}

void CAddGameDlg::OnBnClickedSelect()
{
	for (int nIdx=0; nIdx<m_lstGame.GetItemCount(); nIdx++)
		m_lstGame.SetCheck(nIdx, TRUE);
}

void CAddGameDlg::OnBnClickedUnselect()
{
	for (int nIdx=0; nIdx<m_lstGame.GetItemCount(); nIdx++)
		m_lstGame.SetCheck(nIdx, FALSE);
}
