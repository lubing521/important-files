// AreaMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "AreaMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG

IMPLEMENT_DYNAMIC(CAreaMgrDlg, CDialog)

CAreaMgrDlg::CAreaMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAreaMgrDlg::IDD, pParent)
{
	m_bModified = false;
}

CAreaMgrDlg::~CAreaMgrDlg()
{
}

void CAreaMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_lstArea);
}


BEGIN_MESSAGE_MAP(CAreaMgrDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, &CAreaMgrDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CAreaMgrDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CAreaMgrDlg::OnBnClickedDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CAreaMgrDlg::OnLvnItemchangedList)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CAreaMgrDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CAreaMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	((CEdit*)GetDlgItem(IDC_NAME))->LimitText(64);

	pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	m_lstArea.InsertColumn(0, TEXT("区域名称"), LVCFMT_LEFT, 320);
	m_lstArea.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);


	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	i8desk::AreaInfoMap AreaInfos;
	std::string ErrInfo;
	if (!pDbMgr->GetAllArea(AreaInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	for (i8desk::AreaInfoMapItr it = AreaInfos.begin();
		it != AreaInfos.end(); it ++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nItem = m_lstArea.InsertItem(m_lstArea.GetItemCount(), it->second->Name);
			char *p = new char[it->first.size() + 1];
			lstrcpy(p, it->first.c_str());
			m_lstArea.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}
	}
	AreaInfos.clear();

	if (m_lstArea.GetItemCount())
		m_lstArea.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	CenterWindow();
	return TRUE;
}

void CAreaMgrDlg::OnBnClickedAdd()
{
	CString szText;
	GetDlgItemText(IDC_NAME, szText);
	if (szText.IsEmpty() || szText == DEFAULT_AREA_NAME)
		return ;
	LVFINDINFO info = {0};
	info.psz = szText;
	if (-1 == m_lstArea.FindItem(&info))
	{
		i8desk::tagAreaInfo tagAreaInfo;
		i8desk::SAFE_STRCPY(tagAreaInfo.AID, i8desk::CreateGUIDString().c_str());
		i8desk::SAFE_STRCPY(tagAreaInfo.Name, szText);
		std::string ErrInfo;
		if (!pDlg->m_pDbMgr->AddArea(tagAreaInfo, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}

		int nItem = m_lstArea.InsertItem(m_lstArea.GetItemCount(), szText);
		char* p = new char[strlen(tagAreaInfo.AID) + 1];
		lstrcpy(p, tagAreaInfo.AID);
		m_lstArea.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		m_bModified = true;
	}
}

void CAreaMgrDlg::OnBnClickedModify()
{
	int nSel = m_lstArea.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	CString szText;
	GetDlgItemText(IDC_NAME, szText);

	if (szText.IsEmpty() || szText == DEFAULT_AREA_NAME)
		return ;

	LVFINDINFO info = {0};
	info.psz = szText;
	if (-1 == m_lstArea.FindItem(&info))
	{
		std::string ErrInfo;
		i8desk::tagAreaInfo tagAreaInfo;
		i8desk::SAFE_STRCPY(tagAreaInfo.AID, reinterpret_cast<const char*>(m_lstArea.GetItemData(nSel)));
		i8desk::SAFE_STRCPY(tagAreaInfo.Name, szText);
		if (pDlg->m_pDbMgr->ModifyArea(tagAreaInfo, ErrInfo))
		{
			m_lstArea.SetItemText(nSel, 0, szText);
			m_bModified = true;
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CAreaMgrDlg::OnBnClickedDelete()
{
	int nSel = m_lstArea.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		char* pGuid = reinterpret_cast<char*>(m_lstArea.GetItemData(nSel));
		std::string ErrInfo;
		if (pDlg->m_pDbMgr->DelArea(pGuid, ErrInfo))
		{
			i8desk::std_string strArea(pGuid);

			delete []pGuid;
			m_lstArea.DeleteItem(nSel);
			m_bModified = true;

			
			pDlg->m_pDbMgr->DelGameArea(strArea, ErrInfo);

			for(int i = 0 ; i !=pDlg->m_CurGameInfos.size(); ++i)
			{
				pDlg->m_CurGameInfos[i]->RunTypeDesc = _T("");
			}
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CAreaMgrDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;
	if (m_lstArea.GetSelectedCount() == 1)
	{
		SetDlgItemText(IDC_NAME, m_lstArea.GetItemText(m_lstArea.GetNextItem(-1, LVIS_SELECTED), 0));
	}
}

void CAreaMgrDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstArea.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstArea.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

void CAreaMgrDlg::OnBnClickedOk()
{
}
