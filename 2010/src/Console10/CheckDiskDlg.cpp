// CheckDiskDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckDiskDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

// CCheckDiskDlg 对话框

IMPLEMENT_DYNAMIC(CCheckDiskDlg, CDialog)

CCheckDiskDlg::CCheckDiskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckDiskDlg::IDD, pParent)
{

}

CCheckDiskDlg::~CCheckDiskDlg()
{
}

void CCheckDiskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_UNCHECKPROT, m_lstUnProt);
	DDX_Control(pDX, IDC_LIST_CHECKPROT, m_lstProt);
}


BEGIN_MESSAGE_MAP(CCheckDiskDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PORT, &CCheckDiskDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON_ADDALL_PORT, &CCheckDiskDlg::OnBnClickedAddAll)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PORT, &CCheckDiskDlg::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEALL_PORT, &CCheckDiskDlg::OnBnClickedRemoveAll)
	ON_BN_CLICKED(IDOK, &CCheckDiskDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCheckDiskDlg 消息处理程序


BOOL CCheckDiskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(char chDrv = 'C'; chDrv <= 'Z'; chDrv++)
	{
		CString szItem;
		szItem = chDrv;

		if( m_strCheckArea.Find(szItem) == -1 )
			m_lstUnProt.AddString(szItem);
	}
	for(int nIdx=0; nIdx<m_strCheckArea.GetLength(); nIdx++)
	{
		CString szSub = m_strCheckArea.Mid(nIdx, 1);
		if( -1 == m_lstProt.FindString(-1, szSub) )
			m_lstProt.AddString(szSub);
	}

	if (m_lstUnProt.GetCount())
		m_lstUnProt.SetCurSel(0);

	if (m_lstProt.GetCount())
		m_lstProt.SetCurSel(0);

	return TRUE;
}

void CCheckDiskDlg::OnBnClickedAdd()
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

void CCheckDiskDlg::OnBnClickedAddAll()
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

void CCheckDiskDlg::OnBnClickedRemove()
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

void CCheckDiskDlg::OnBnClickedRemoveAll()
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

void CCheckDiskDlg::OnBnClickedOk()
{
	for (int idx=0; idx<m_lstUnProt.GetCount(); idx++)
	{
		char buf[2] = {0};
		m_lstUnProt.GetText(idx, buf);
		m_strUnCheckArea += buf;
	}

	for (int idx=0; idx<m_lstProt.GetCount(); idx++)
	{
		char buf[2] = {0};
		m_lstProt.GetText(idx, buf);
		m_strCheckArea += buf;
	}

	OnOK();
}