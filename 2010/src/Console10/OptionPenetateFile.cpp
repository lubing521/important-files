// OptionPenetateFile.cpp : 实现文件
//

#include "stdafx.h"
#include "OptionPenetateFile.h"
#include "PenetateFile.h"
#include "DirMakeIdxDlg.h"

// COptionPenetateFile 对话框

IMPLEMENT_DYNAMIC(COptionPenetateFile, CDialog)

COptionPenetateFile::COptionPenetateFile(CWnd* pParent /*=NULL*/)
	: CDialog(COptionPenetateFile::IDD, pParent)
	, m_bModify(FALSE)
{

}

COptionPenetateFile::~COptionPenetateFile()
{
}

void COptionPenetateFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTDIR, m_list);
}


BEGIN_MESSAGE_MAP(COptionPenetateFile, CDialog)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &COptionPenetateFile::OnApplyMessage)
	ON_BN_CLICKED(IDC_ADD, &COptionPenetateFile::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &COptionPenetateFile::OnBnClickedDel)
	ON_NOTIFY(NM_CLICK, IDC_LISTDIR, &COptionPenetateFile::OnNMClickListdir)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LISTDIR, &COptionPenetateFile::OnLvnDeleteitemListdir)
	ON_BN_CLICKED(IDC_MKIDX, &COptionPenetateFile::OnBnClickedMkidx)
END_MESSAGE_MAP()


BOOL COptionPenetateFile::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_MKIDX)->EnableWindow(FALSE);

	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, "选定服务器上的源目录", LVCFMT_LEFT, 220);
	m_list.InsertColumn(1, "推送到客户机的指定位置", LVCFMT_LEFT, 220);

	std::string val = GetDbMgr()->GetOptString(OPT_M_PENETATEFILE, "");
	std::string::size_type pos = val.find_first_of("|", 0);
	while (pos != std::string::npos)
	{
		std::string str(val.substr(0, pos));
		if (str.size() < 5)
			break;

		std::string::size_type posx = str.find_first_of("?", 0);
		if (posx == std::string::npos || posx == str.size() - 1)
			break;

		std::string src(str.substr(0, posx));
		std::string dest(str.substr(posx + 1, str.size()));
		
		int nItem = m_list.InsertItem(m_list.GetItemCount(), src.c_str());
		m_list.SetItemText(nItem, 1, dest.c_str());

		if (pos == val.size() - 1)
			break;

		val = val.substr(pos + 1, val.size());
		pos = val.find_first_of("|", 0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// COptionPenetateFile 消息处理程序

void COptionPenetateFile::OnBnClickedAdd()
{
	CPenetateFile dlg;
	if (dlg.DoModal() == IDOK)
	{
		int nItem = m_list.InsertItem(m_list.GetItemCount(), dlg.m_strSrcDir);
		m_list.SetItemText(nItem, 1, dlg.m_strDestDir);
		m_bModify = TRUE;
	}
}

void COptionPenetateFile::OnBnClickedDel()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	ASSERT(pos != NULL);

	int nItem = m_list.GetNextSelectedItem(pos);
	ASSERT(nItem != -1);

	m_list.DeleteItem(nItem);
	m_bModify = TRUE;
}


void COptionPenetateFile::OnNMClickListdir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	
	GetDlgItem(IDC_DEL)->EnableWindow(pos != NULL);
	GetDlgItem(IDC_MKIDX)->EnableWindow(pos != NULL);

	*pResult = 0;
}

void COptionPenetateFile::OnLvnDeleteitemListdir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	
	GetDlgItem(IDC_DEL)->EnableWindow(pos != NULL);
	GetDlgItem(IDC_MKIDX)->EnableWindow(pos != NULL);

	*pResult = 0;
}

LRESULT COptionPenetateFile::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		std::string val;
		for (int nItem = 0; nItem < m_list.GetItemCount(); nItem++)
		{
			val += (LPCTSTR)m_list.GetItemText(nItem, 0);
			val += "?";
			val += (LPCTSTR)m_list.GetItemText(nItem, 1);
			val += "|";
		}

		GetDbMgr()->SetOption(OPT_M_PENETATEFILE, val);
	}

	return TRUE;
}

void COptionPenetateFile::OnBnClickedMkidx()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	ASSERT(pos != NULL);

	int nItem = m_list.GetNextSelectedItem(pos);
	ASSERT(nItem != -1);

	if (GetConsoleDlg()->IsRunOnServer())
	{
		CDirMakeIdxDlg dlg(m_list.GetItemText(nItem, 0));
		dlg.DoModal();
	}
	else 
	{
		AfxMessageBox(_T("远程连接服务端时不能制作索引"));
	}
}
