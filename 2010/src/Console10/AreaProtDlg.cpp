// AreaProtDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "AreaProtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CAreaProtDlg, CDialog)

CAreaProtDlg::CAreaProtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAreaProtDlg::IDD, pParent)
{

}

CAreaProtDlg::~CAreaProtDlg()
{
}

void CAreaProtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UNPROT, m_lstUnProt);
	DDX_Control(pDX, IDC_LIST_PROT, m_lstProt);
}


BEGIN_MESSAGE_MAP(CAreaProtDlg, CDialog)	
	ON_BN_CLICKED(IDC_ADD, &CAreaProtDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_ADDALL, &CAreaProtDlg::OnBnClickedAddAll)
	ON_BN_CLICKED(IDC_REMOVE, &CAreaProtDlg::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_REMOVEALL, &CAreaProtDlg::OnBnClickedRemoveAll)
	ON_BN_CLICKED(IDOK, &CAreaProtDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CAreaProtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (char chDrv = 'C'; chDrv <= 'Z'; chDrv++)
	{
		CString szItem;
		szItem = chDrv;
		if (m_strProtArea.Find(szItem) == -1)
			m_lstUnProt.AddString(szItem);
	}
	for (int nIdx=0; nIdx<m_strProtArea.GetLength(); nIdx++)
	{
		CString szSub = m_strProtArea.Mid(nIdx, 1);
		if (-1 == m_lstProt.FindString(-1, szSub))
			m_lstProt.AddString(szSub);
	}
	if (m_lstUnProt.GetCount())
		m_lstUnProt.SetCurSel(0);
	if (m_lstProt.GetCount())
		m_lstProt.SetCurSel(0);
	
	return TRUE;
}

void CAreaProtDlg::OnBnClickedAdd()
{
	for (int idx=0; idx<m_lstUnProt.GetCount(); idx++)
	{
		if (m_lstUnProt.GetSel(idx))
		{
			CString Text;
			m_lstUnProt.GetText(idx, Text);
			m_lstUnProt.DeleteString(idx);
			m_lstProt.AddString(Text);
		}
	}
	if (m_lstUnProt.GetCount())
		m_lstUnProt.SetCurSel(0);
}

void CAreaProtDlg::OnBnClickedAddAll()
{
	m_lstUnProt.ResetContent();
	m_lstProt.ResetContent();
	for (char chDrv = 'C'; chDrv <= 'Z'; chDrv++)
	{
		CString szItem;
		szItem = chDrv;
		m_lstProt.AddString(szItem);
	}
}

void CAreaProtDlg::OnBnClickedRemove()
{
	for (int idx=0; idx<m_lstProt.GetCount(); idx++)
	{
		if (m_lstProt.GetSel(idx))
		{
			CString Text;
			m_lstProt.GetText(idx, Text);
			m_lstProt.DeleteString(idx);
			m_lstUnProt.AddString(Text);
		}
	}
	if (m_lstProt.GetCount())
		m_lstProt.SetCurSel(0);
}

void CAreaProtDlg::OnBnClickedRemoveAll()
{
	m_lstUnProt.ResetContent();
	m_lstProt.ResetContent();
	for (char chDrv = 'C'; chDrv <= 'Z'; chDrv++)
	{
		CString szItem;
		szItem = chDrv;
		m_lstUnProt.AddString(szItem);
	}
}

void CAreaProtDlg::OnBnClickedOk()
{
	m_strUnProtArea = ""; m_strProtArea = "";
	for (int idx=0; idx<m_lstUnProt.GetCount(); idx++)
	{
		char buf[2] = {0};
		m_lstUnProt.GetText(idx, buf);
		m_strUnProtArea += buf;
	}

	for (int idx=0; idx<m_lstProt.GetCount(); idx++)
	{
		char buf[2] = {0};
		m_lstProt.GetText(idx, buf);
		m_strProtArea += buf;
	}

	OnOK();
}