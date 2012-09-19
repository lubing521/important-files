// UserMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "UserMgrDlg.h"


// CUserMgrDlg 对话框

IMPLEMENT_DYNAMIC(CUserMgrDlg, CDialog)

CUserMgrDlg::CUserMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserMgrDlg::IDD, pParent)
{

}

CUserMgrDlg::~CUserMgrDlg()
{
}

void CUserMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER, m_lstUser);
}

BEGIN_MESSAGE_MAP(CUserMgrDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, &CUserMgrDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CUserMgrDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CUserMgrDlg::OnBnClickedDelete)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CUserMgrDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USER, &CUserMgrDlg::OnLvnItemchangedListUser)
END_MESSAGE_MAP()


BOOL CUserMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CEdit*)GetDlgItem(IDC_USER))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD))->LimitText(32);

	m_lstUser.ModifyStyle(0, LVS_SINGLESEL | LVS_SHOWSELALWAYS);
	m_lstUser.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_lstUser.InsertColumn(0, "用户名",			LVCFMT_LEFT, 50);
	m_lstUser.InsertColumn(1, "空间(M)",		LVCFMT_LEFT, 60);
	m_lstUser.InsertColumn(2, "创建时间",		LVCFMT_LEFT, 120);
	m_lstUser.InsertColumn(3, "最后登陆时间",	LVCFMT_LEFT, 120);
	m_lstUser.InsertColumn(4, "最后登陆IP",		LVCFMT_LEFT, 120);

	SetDlgItemInt(IDC_ROOM, 50);
	pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	std::string ErrInfo;
	std::map<std::string, i8desk::tagUser*> UserInfos;
	if (!pDlg->m_pDbMgr->GetAllUser(UserInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}
	for (std::map<std::string, i8desk::tagUser*>::iterator it = UserInfos.begin();
		it != UserInfos.end(); it ++)
	{
		int nItem = m_lstUser.InsertItem(m_lstUser.GetItemCount(), it->second->Name);
		CString szItem;
		szItem.Format("%d", it->second->RoomSize);
		m_lstUser.SetItemText(nItem, 1, szItem);
		m_lstUser.SetItemText(nItem, 2, i8desk::MakeTimeString(it->second->CreateDate).c_str());
		m_lstUser.SetItemText(nItem, 3, i8desk::MakeTimeString(it->second->LastLoginDate).c_str());
		m_lstUser.SetItemText(nItem, 4, i8desk::MakeIpString(it->second->LastLoginIp).c_str());
		char* p = new char[strlen(it->second->Pwd) + 1];
		lstrcpy(p, it->second->Pwd);
		m_lstUser.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		delete it->second;
	}
	if (m_lstUser.GetItemCount())
	{
		m_lstUser.SetItemState(0, LVIS_SELECTED|LVFIS_FOCUSED, LVIS_SELECTED|LVFIS_FOCUSED);
	}
	
	CenterWindow();
	return TRUE;
}
void CUserMgrDlg::OnBnClickedAdd()
{
	CString szUser, szPassword;
	GetDlgItemText(IDC_USER, szUser);	
	GetDlgItemText(IDC_PASSWORD, szPassword);
	if (szUser.IsEmpty())
		return ;
	LVFINDINFO info = {0};
	info.psz = szUser;
	if (-1 == m_lstUser.FindItem(&info))
	{
		i8desk::tagUser tagUser;
		i8desk::SAFE_STRCPY(tagUser.Name, szUser);
		i8desk::SAFE_STRCPY(tagUser.Pwd, szPassword);
		tagUser.RoomSize = GetDlgItemInt(IDC_ROOM);
		tagUser.LastLoginIp = 0;
		tagUser.LastLoginDate = 0;
		tagUser.CreateDate = _time32(NULL);
		std::string ErrInfo;
		if (!pDlg->m_pDbMgr->AddUser(&tagUser, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}
		int nItem = m_lstUser.InsertItem(m_lstUser.GetItemCount(), szUser);
		CString szItem;
		szItem.Format("%d", tagUser.RoomSize);
		m_lstUser.SetItemText(nItem, 1, szItem);
		m_lstUser.SetItemText(nItem, 2, i8desk::MakeTimeString(tagUser.CreateDate).c_str());
		m_lstUser.SetItemText(nItem, 3, i8desk::MakeTimeString(tagUser.LastLoginDate).c_str());
		m_lstUser.SetItemText(nItem, 4, i8desk::MakeIpString(tagUser.LastLoginIp).c_str());
	}
}

void CUserMgrDlg::OnBnClickedModify()
{
	int nSel = m_lstUser.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	CString szUser, szPassword;
	GetDlgItemText(IDC_USER, szUser);	
	GetDlgItemText(IDC_PASSWORD, szPassword);
	if (szUser.IsEmpty())
		return ;
	LVFINDINFO info = {0};
	info.psz = szUser;
	if (nSel == m_lstUser.FindItem(&info))
	{
		std::string ErrInfo;
		i8desk::tagUser tagUser;
		i8desk::SAFE_STRCPY(tagUser.Name, szUser);
		i8desk::SAFE_STRCPY(tagUser.Pwd, szPassword);
		tagUser.RoomSize = GetDlgItemInt(IDC_ROOM);
		if (!pDlg->m_pDbMgr->ModifyUser(&tagUser, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}
		CString szItem;
		szItem.Format("%d", tagUser.RoomSize);
		m_lstUser.SetItemText(nSel, 1, szItem);

		char*p = reinterpret_cast<char*>(m_lstUser.GetItemData(nSel));
		delete []p;
		p = new char[szPassword.GetLength() + 1];
		lstrcpy(p, szPassword);
		m_lstUser.SetItemData(nSel, reinterpret_cast<DWORD>(p));
	}
}

void CUserMgrDlg::OnBnClickedDelete()
{
	int nSel = m_lstUser.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		CString name = m_lstUser.GetItemText(nSel, 0);
		std::string ErrInfo;
		if (pDlg->m_pDbMgr->DelUser(std::string((LPCSTR)name), ErrInfo))
		{
			char* p = reinterpret_cast<char*>(m_lstUser.GetItemData(nSel));
			delete []p;
			m_lstUser.DeleteItem(nSel);
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CUserMgrDlg::OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;
	int nSel = m_lstUser.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1) return ;

	SetDlgItemText(IDC_USER, m_lstUser.GetItemText(nSel, 0));
	SetDlgItemText(IDC_ROOM, m_lstUser.GetItemText(nSel, 1));
	char*p = reinterpret_cast<char*>(m_lstUser.GetItemData(nSel));
	SetDlgItemText(IDC_PASSWORD, p);
}

void CUserMgrDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstUser.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstUser.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

void CUserMgrDlg::OnBnClickedOk()
{
}
