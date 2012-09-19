// BrowseDirDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BrowseDirDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CBrowseDirDlg, CDialog)

CBrowseDirDlg::CBrowseDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowseDirDlg::IDD, pParent)
{
	m_bBrowseDir = TRUE;
}

CBrowseDirDlg::~CBrowseDirDlg()
{
}

void CBrowseDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_lstDir);
}


BEGIN_MESSAGE_MAP(CBrowseDirDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CBrowseDirDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBrowseDirDlg::OnBnClickedCancel)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, &CBrowseDirDlg::OnTvnItemexpandedTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CBrowseDirDlg::OnTvnSelchangedTree)
END_MESSAGE_MAP()

BOOL CBrowseDirDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	std::vector<std::string> lstDir;
	std::string ErrInfo;
	if (m_bBrowseDir)
	{	if (!pDlg->m_pDbMgr->GetFileOrDir("", GET_DRVLIST, true, lstDir, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
		}
		else
		{
			for (std::vector<std::string>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
			{
				HTREEITEM hItem = m_lstDir.InsertItem(it->c_str());
				m_lstDir.SetItemData(hItem, 0);//还没有展开
				m_lstDir.InsertItem("", hItem);
			}
 		}

		for (int nIdx=0; ;nIdx++)
		{
			CString szItem;
			AfxExtractSubString(szItem, m_strDirOrFile, nIdx, '\\');
			if (szItem.IsEmpty())
				break;
			SelectTreeNode(szItem, m_lstDir.GetRootItem());
		}
	}
	else
	{
		HTREEITEM hRoot = m_lstDir.InsertItem(m_strDirOrFile);
		if (!pDlg->m_pDbMgr->GetFileOrDir(std::string((LPCSTR)m_strDirOrFile), GET_DIRLIST, true, lstDir, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
		}
		else
		{
			for (std::vector<std::string>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
			{
				HTREEITEM hItem = m_lstDir.InsertItem(it->c_str(), hRoot);
				m_lstDir.SetItemData(hItem, 0);//还没有展开
				m_lstDir.InsertItem("", hItem);
			}
		}

		lstDir.clear();
		if (!pDlg->m_pDbMgr->GetFileOrDir(std::string((LPCSTR)m_strDirOrFile), GET_FILELIST, true, lstDir, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
		}
		else
		{
			m_lstDir.SetItemData(hRoot, 1);
			for (std::vector<std::string>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
			{
				HTREEITEM hItem = m_lstDir.InsertItem(it->c_str(), hRoot);
				m_lstDir.SetItemData(hItem, 1);
			}
			m_lstDir.Expand(hRoot, TVE_EXPAND);
		}
	}
	CenterWindow();
	return TRUE;
}

void CBrowseDirDlg::OnBnClickedOk()
{
	m_strDirOrFile = GetSelDir().c_str();
	if (m_strDirOrFile.IsEmpty())
	{
		AfxMessageBox("请选择目录!");
		return ;
	}
	OnOK();
}

void CBrowseDirDlg::OnBnClickedCancel()
{
	OnCancel();
}

std::string CBrowseDirDlg::GetSelDir(HTREEITEM hSel /* = NULL */)
{
	HTREEITEM hTemp = hSel;
	if (hSel == NULL)
		hTemp = m_lstDir.GetSelectedItem();
	std::string dir;
	while (hTemp != NULL)
	{
		CString szITem = m_lstDir.GetItemText(hTemp);
		hTemp = m_lstDir.GetParentItem(hTemp);
		if (szITem.Right(1) != "\\")
			dir = szITem + "\\" + dir.c_str();
		else
			dir = szITem + dir.c_str();
	}
	if (dir.size() && *dir.rbegin() == '\\')
		dir.resize(dir.size()-1);
	return dir;
}

bool CBrowseDirDlg::SelectTreeNode(LPCTSTR lpszText, HTREEITEM hItem)
{
	if (hItem != NULL)
	{
		if (m_lstDir.GetItemText(hItem) == lpszText)
		{
			m_lstDir.SelectItem(hItem);
			m_lstDir.Expand(hItem, TVE_EXPAND);
			m_lstDir.EnsureVisible(hItem);
			return true;
		}

		if (SelectTreeNode(lpszText, m_lstDir.GetChildItem(hItem)))
			return true;

		if (SelectTreeNode(lpszText, m_lstDir.GetNextSiblingItem(hItem)))
			return true;
	}
	return false;
}

void CBrowseDirDlg::OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	DWORD dwData = m_lstDir.GetItemData(hSelItem);
	if (1 == dwData || pNMTreeView->action == 1)
		return ;

	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	std::string dir = GetSelDir(hSelItem);
	std::vector<std::string> lstDir;
	std::string ErrInfo;

	if (!pDlg->m_pDbMgr->GetFileOrDir(dir, GET_DIRLIST, true, lstDir, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
	}
	else
	{
		HTREEITEM hChild = m_lstDir.GetChildItem(hSelItem);
		if (hChild != NULL)
			m_lstDir.DeleteItem(hChild);
		for (std::vector<std::string>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
		{
			HTREEITEM hItem = m_lstDir.InsertItem(it->c_str(), hSelItem);
			m_lstDir.SetItemData(hItem, 0);//还没有展开
			m_lstDir.InsertItem("", hItem);
		}
		m_lstDir.SetItemData(hSelItem, 1);//父己经展开了。
	}

	if (m_bBrowseDir)
		return ;

	lstDir.clear();
	if (!pDlg->m_pDbMgr->GetFileOrDir(dir, GET_FILELIST, true, lstDir, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
	}
	else
	{
		HTREEITEM hChild = m_lstDir.GetChildItem(hSelItem);
		if (hChild != NULL && m_lstDir.GetItemText(hChild).IsEmpty())
			m_lstDir.DeleteItem(hChild);
		for (std::vector<std::string>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
		{
			HTREEITEM hItem = m_lstDir.InsertItem(it->c_str(), hSelItem);
			m_lstDir.SetItemData(hItem, 1); //文件不能展开
		}
		m_lstDir.SetItemData(hSelItem, 1);//父己经展开了。
	}
}

BOOL CBrowseDirDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CBrowseDirDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	CString szCap = "选择文件:[";
	if (m_bBrowseDir)
		szCap = "选择目录:[";
	
	szCap += GetSelDir().c_str();
	szCap += "]";
	SetWindowText(szCap);
}
