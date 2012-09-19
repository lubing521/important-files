// DlgOptBrowseDir.cpp : 实现文件
//

#include "stdafx.h"
#include "../Console.h"
#include "DlgCfgBrowseDir.h"
#include "../ManagerInstance.h"
#include "../Data/DataHelper.h"
#include "../MessageBox.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgCfgBrowseDir, CDialog)

CDlgCfgBrowseDir::CDlgCfgBrowseDir(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCfgBrowseDir::IDD, pParent)
{
}

CDlgCfgBrowseDir::~CDlgCfgBrowseDir()
{
}

void CDlgCfgBrowseDir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BROWSEDIR, listDir_);
}


BEGIN_MESSAGE_MAP(CDlgCfgBrowseDir, CDialog)
	ON_BN_CLICKED(IDC_BROWSEDIR_OK, &CDlgCfgBrowseDir::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSEDIR_CANCEL, &CDlgCfgBrowseDir::OnBnClickedCancel)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_BROWSEDIR, &CDlgCfgBrowseDir::OnTvnItemexpandedTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_BROWSEDIR, &CDlgCfgBrowseDir::OnTvnSelchangedTree)
END_MESSAGE_MAP()

BOOL CDlgCfgBrowseDir::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::vector<stdex::tString> lstDir;
	stdex::tString ErrInfo;
	if (i8desk::GetControlMgr().GetFileOrDir(_T(""),i8desk::data_helper::GET_DRVLIST, true, lstDir))
	{
		for (std::vector<stdex::tString>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
		{
			HTREEITEM hItem = listDir_.InsertItem(it->c_str());
			listDir_.SetItemData(hItem, 0);//还没有展开
			listDir_.InsertItem(_T(""), hItem);
		}
	}

	for (int nIdx=0; ;nIdx++)
	{
		CString szItem;
		AfxExtractSubString(szItem, strDirOrFile_, nIdx, '\\');
		if (szItem.IsEmpty())
			break;
		SelectTreeNode(szItem, listDir_.GetRootItem());
		}

	CenterWindow();
	return TRUE;
}

void CDlgCfgBrowseDir::OnBnClickedOk()
{
	strDirOrFile_ = GetSelDir().c_str();
	if (strDirOrFile_.IsEmpty())
	{
		CMessageBox msgDlg(_T("提示"), _T("请选择目录!"));
		msgDlg.DoModal();
		return ;
	}
	OnOK();
}

void CDlgCfgBrowseDir::OnBnClickedCancel()
{
	OnCancel();
}

stdex::tString CDlgCfgBrowseDir::GetSelDir(HTREEITEM hSel /* = NULL */)
{
	HTREEITEM hTemp = hSel;
	if (hSel == NULL)
		hTemp = listDir_.GetSelectedItem();
	stdex::tString dir;
	while (hTemp != NULL)
	{
		CString szITem = listDir_.GetItemText(hTemp);
		hTemp = listDir_.GetParentItem(hTemp);
		if (szITem.Right(1) != _T("\\"))
			dir = szITem + _T("\\") + dir.c_str();
		else
			dir = szITem + dir.c_str();
	}
	if (dir.size() && *dir.rbegin() == '\\')
		dir.resize(dir.size()-1);
	return dir;
}

bool CDlgCfgBrowseDir::SelectTreeNode(LPCTSTR lpszText, HTREEITEM hItem)
{
	if (hItem != NULL)
	{
		if (listDir_.GetItemText(hItem) == lpszText)
		{
			listDir_.SelectItem(hItem);
			listDir_.Expand(hItem, TVE_EXPAND);
			listDir_.EnsureVisible(hItem);
			return true;
		}

		if (SelectTreeNode(lpszText, listDir_.GetChildItem(hItem)))
			return true;

		if (SelectTreeNode(lpszText, listDir_.GetNextSiblingItem(hItem)))
			return true;
	}
	return false;
}

void CDlgCfgBrowseDir::OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	DWORD dwData = listDir_.GetItemData(hSelItem);
	if (1 == dwData || pNMTreeView->action == 1)
		return ;

	stdex::tString dir = GetSelDir(hSelItem);
	std::vector<stdex::tString> lstDir;
	stdex::tString ErrInfo;
	if (i8desk::GetControlMgr().GetFileOrDir(dir, i8desk::data_helper::GET_DIRLIST, true, lstDir))
	{
		HTREEITEM hChild = listDir_.GetChildItem(hSelItem);
		if (hChild != NULL)
			listDir_.DeleteItem(hChild);
		for (std::vector<stdex::tString>::iterator it = lstDir.begin(); it != lstDir.end(); it++)
		{
			HTREEITEM hItem = listDir_.InsertItem(it->c_str(), hSelItem);
			listDir_.SetItemData(hItem, 0);//还没有展开
			listDir_.InsertItem(_T(""), hItem);
		}
		listDir_.SetItemData(hSelItem, 1);//父己经展开了。
	}
}

BOOL CDlgCfgBrowseDir::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgCfgBrowseDir::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;

	CString szCap = _T("选择目录:[");
	
	szCap += GetSelDir().c_str();
	szCap += _T("]");
	SetWindowText(szCap);
}
