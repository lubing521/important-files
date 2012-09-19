// ClassMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "ClassMgrDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CClassMgrDlg, CDialog)

CClassMgrDlg::CClassMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClassMgrDlg::IDD, pParent)
{
	m_bModified = false;
}

CClassMgrDlg::~CClassMgrDlg()
{
}

void CClassMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstClass);
}

BEGIN_MESSAGE_MAP(CClassMgrDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, &CClassMgrDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CClassMgrDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CClassMgrDlg::OnBnClickedDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CClassMgrDlg::OnLvnItemchangedList)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CClassMgrDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CClassMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());

	((CEdit*)GetDlgItem(IDC_NAME))->LimitText(64);
	((CEdit*)GetDlgItem(IDC_PATH))->LimitText(255);

	m_lstClass.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_lstClass.InsertColumn(0, TEXT("类别名称"), LVCFMT_LEFT, 90);
	m_lstClass.InsertColumn(1, TEXT("游戏默认路径"), LVCFMT_LEFT, 240);

	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	i8desk::DefClassMap ClassInfos;
	std::string ErrInfo;

	if (!pDbMgr->GetAllClass(ClassInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	LPCSTR defGuid[] = {
		CLASS_WL_GUID, 
		CLASS_DJ_GUID, 
		CLASS_XX_GUID, 
		CLASS_WY_GUID, 
		CLASS_DZ_GUID, 
		CLASS_QP_GUID, 
		CLASS_PL_GUID, 
		CLASS_LT_GUID, 
		CLASS_CY_GUID, 
		CLASS_YY_GUID,
		CLASS_GP_GUID,
	};

	for (int idx=0; idx<_countof(defGuid); idx++)
	{		
		i8desk::DefClassMapItr it = ClassInfos.find(defGuid[idx]);
		if (it != ClassInfos.end())
		{
			int nItem = m_lstClass.InsertItem(m_lstClass.GetItemCount(), it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_lstClass.SetItemData(nItem, reinterpret_cast<DWORD>(p));
			m_lstClass.SetItemText(nItem, 1, it->second->Path);
		}
	}

	for (i8desk::DefClassMapItr it = ClassInfos.begin();
		it != ClassInfos.end(); it ++)
	{
		if (!i8desk::IsI8DeskClassGUID(it->first))
		{
			int nItem = m_lstClass.InsertItem(m_lstClass.GetItemCount(), it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_lstClass.SetItemData(nItem, reinterpret_cast<DWORD>(p));
			m_lstClass.SetItemText(nItem, 1, it->second->Path);
		}
	}
	ClassInfos.clear();

	if (m_lstClass.GetItemCount())
		m_lstClass.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	CenterWindow();
	return TRUE;
}

void CClassMgrDlg::OnBnClickedAdd()
{
	CString szText, szPath;
	GetDlgItemText(IDC_NAME, szText);
	GetDlgItemText(IDC_PATH, szPath);
	if (szText.IsEmpty() || i8desk::IsI8DeskClassName(szText))
		return ;
	if (szPath.GetLength() && !i8desk::IsValidDirName((LPCSTR)szPath))
	{
		AfxMessageBox("游戏默认下载路径不是有效的路径.");
		return ;
	}
	if (szPath.GetLength() && szPath.Right(1) != "\\")
		szPath += "\\";

	LVFINDINFO info = {0};
	info.psz = szText;
	if (-1 == m_lstClass.FindItem(&info))
	{
		i8desk::tagDefClass tagClass;
		i8desk::SAFE_STRCPY(tagClass.DefClass, i8desk::CreateGUIDString().c_str());
		i8desk::SAFE_STRCPY(tagClass.Name, szText);
		i8desk::SAFE_STRCPY(tagClass.Path, szPath);
		tagClass.IsIdc = FALSE;
		std::string ErrInfo;
		if (!pDlg->m_pDbMgr->AddClass(tagClass, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}
			
		int nItem = m_lstClass.InsertItem(m_lstClass.GetItemCount(), szText);
		char* p = new char[strlen(tagClass.DefClass) + 1];
		lstrcpy(p, tagClass.DefClass);
		m_lstClass.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		m_lstClass.SetItemText(nItem, 1, szPath);
		m_bModified = true;
	}
}

void CClassMgrDlg::OnBnClickedModify()
{
	int nSel = m_lstClass.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	CString szText, szPath;
	GetDlgItemText(IDC_NAME, szText);
	GetDlgItemText(IDC_PATH, szPath);
	CString x = m_lstClass.GetItemText(nSel, 0);
	bool IsIdcClass = i8desk::IsI8DeskClassName(x);
	if (!IsIdcClass && (szText.IsEmpty() || i8desk::IsI8DeskClassName(szText)))
		return ;

	if (szPath.GetLength() && !i8desk::IsValidDirName((LPCSTR)szPath))
	{
		AfxMessageBox("游戏默认下载路径不是有效的路径.");
		return ;
	}
	if (szPath.GetLength() && szPath.Right(1) != "\\")
		szPath += "\\";

	LVFINDINFO info = {0};
	info.psz = szText;

	int nFind = m_lstClass.FindItem(&info); 
	if (!IsIdcClass && (nFind != -1 &&  nSel != nFind))
		return ;

	{
		std::string ErrInfo;
		i8desk::tagDefClass tagClass;
		i8desk::SAFE_STRCPY(tagClass.DefClass, reinterpret_cast<const char*>(m_lstClass.GetItemData(nSel)));
		if (!IsIdcClass)
			i8desk::SAFE_STRCPY(tagClass.Name, szText);
		else
			i8desk::SAFE_STRCPY(tagClass.Name, x);
		i8desk::SAFE_STRCPY(tagClass.Path, szPath);
		tagClass.IsIdc = IsIdcClass;
		if (pDlg->m_pDbMgr->ModifyClass(tagClass, ErrInfo))
		{
			if (!IsIdcClass)
				m_lstClass.SetItemText(nSel, 0, szText);
			m_lstClass.SetItemText(nSel, 1, szPath);
			m_bModified = true;
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CClassMgrDlg::OnBnClickedDelete()
{
	int nSel = m_lstClass.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1)
	{
		CString szText = m_lstClass.GetItemText(nSel, 0);
		if (i8desk::IsI8DeskClassName(szText))
			return ;
	}
	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		char* pGuid = reinterpret_cast<char*>(m_lstClass.GetItemData(nSel));
		std::string ErrInfo;
		if (pDlg->m_pDbMgr->DelClass(pGuid, ErrInfo))
		{
			delete []pGuid;
			m_lstClass.DeleteItem(nSel);
			m_bModified = true;
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CClassMgrDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;
	int nSel = m_lstClass.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1)
	{
		SetDlgItemText(IDC_NAME, m_lstClass.GetItemText(nSel, 0));
		SetDlgItemText(IDC_PATH, m_lstClass.GetItemText(nSel, 1));
	}
}

void CClassMgrDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstClass.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstClass.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

void CClassMgrDlg::OnBnClickedOk()
{

}
