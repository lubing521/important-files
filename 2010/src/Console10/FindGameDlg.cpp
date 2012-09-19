// FindGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "FindGameDlg.h"
#include "ConsoleDlg.h"

#include <regex>

IMPLEMENT_DYNAMIC(CFindGameDlg, CDialog)

CFindGameDlg::CFindGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindGameDlg::IDD, pParent)
	, m_btnUp(0)
	, m_btnByGid(0)
{
	m_nCurPos = -1;
}

CFindGameDlg::~CFindGameDlg()
{
}

void CFindGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_DOWN, m_btnDwon);
	DDX_Control(pDX, IDC_RADIO_BYNAME, m_btnByName);
	DDX_Control(pDX, IDC_EDIT_FIND, m_edtFindStr);
}


BEGIN_MESSAGE_MAP(CFindGameDlg, CDialog)
	ON_BN_CLICKED(IDC_FIND_NEXT, &CFindGameDlg::OnBnClickedFindNext)
	ON_BN_CLICKED(IDC_SELECT_ALL, &CFindGameDlg::OnBnClickedSelectAll)
	ON_BN_CLICKED(IDOK, &CFindGameDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_DOWN, &CFindGameDlg::OnBnClickedRadioDown)
	ON_BN_CLICKED(IDC_RADIO_UP, &CFindGameDlg::OnBnClickedRadioDown)
	ON_BN_CLICKED(IDC_RADIO_BYNAME, &CFindGameDlg::OnBnClickedRadioDown)
	ON_BN_CLICKED(IDC_RADIO_BYGID, &CFindGameDlg::OnBnClickedRadioDown)
	ON_EN_SETFOCUS(IDC_EDIT_FIND, &CFindGameDlg::OnEnSetfocusEditFind)
	ON_EN_CHANGE(IDC_EDIT_FIND, &CFindGameDlg::OnEnChangedEditFind)
END_MESSAGE_MAP()

BOOL CFindGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnByName.SetCheck(1);
	m_btnDwon.SetCheck(1);

	m_edtFindStr.SetWindowText(m_InputChar);

	return TRUE;
}

BOOL CFindGameDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtFindStr.m_hWnd &&
		pMsg->message == WM_KEYDOWN &&
		pMsg->wParam  == VK_RETURN)
	{
		OnBnClickedFindNext();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CFindGameDlg::OnBnClickedFindNext()
{
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	CString strFind;
	m_edtFindStr.GetWindowText(strFind);
	if (strFind.IsEmpty())
	{
		AfxMessageBox("请输入要查找的内容.");
		return ;
	}
	if (m_nCurPos == -1)
		m_list.clear();
	m_nCurPos = pDlg->FindGame(m_nCurPos, m_btnByName.GetCheck() == BST_CHECKED, m_btnDwon.GetCheck() == BST_CHECKED, strFind);
	if (m_nCurPos == -1)
	{
		AfxMessageBox("己经查找到尾.");
	}
	else
	{
		m_list.insert(m_nCurPos);
	}
}

void CFindGameDlg::OnBnClickedSelectAll()
{
	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	pDlg->FindSelectall(m_list);
}

void CFindGameDlg::OnBnClickedOk()
{
}

void CFindGameDlg::OnBnClickedRadioDown()
{
	m_nCurPos = -1;
}

void CFindGameDlg::OnEnSetfocusEditFind()
{
	CString str;
	m_edtFindStr.GetWindowText(str);
	m_edtFindStr.SetSel(str.GetLength(), str.GetLength()+1);
}

void CFindGameDlg::OnEnChangedEditFind()
{
	CString str;
	m_edtFindStr.GetWindowText(str);
	
	//如果查找串只有数字（两边可以有空格），则自动识别为按GID查找
	std::tr1::regex regex("^\\s*\\d+\\s*$");
	m_btnByGid = std::tr1::regex_match((LPCTSTR)str, regex) ? 1 : 0;
	((CButton*)GetDlgItem(IDC_RADIO_BYGID))->SetCheck(m_btnByGid);
	m_btnByName.SetCheck(m_btnByGid == 0 ? 1 : 0);

	OnBnClickedRadioDown();
}
