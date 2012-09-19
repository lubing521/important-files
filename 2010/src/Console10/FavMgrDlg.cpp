// FavMgrDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "FavMgrDlg.h"

#ifdef max
#undef max
#endif 

#include <algorithm>

// CFavMgrDlg 对话框

IMPLEMENT_DYNAMIC(CFavMgrDlg, CDialog)

CFavMgrDlg::CFavMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFavMgrDlg::IDD, pParent)
	, m_dwFavMaxNo(0)
{
}

CFavMgrDlg::~CFavMgrDlg()
{
}

void CFavMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE, m_cboType);
	DDX_Control(pDX, IDC_LIST_FAV, m_lstFav);
}


BEGIN_MESSAGE_MAP(CFavMgrDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD, &CFavMgrDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CFavMgrDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DELETE, &CFavMgrDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDOK, &CFavMgrDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FAV, &CFavMgrDlg::OnLvnItemChangedListFav)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_MOVE_UP, &CFavMgrDlg::OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, &CFavMgrDlg::OnBnClickedMoveDown)
END_MESSAGE_MAP()

BOOL CFavMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_NAME))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_URL))->LimitText(255);

	m_cboType.AddString("电影");
	m_cboType.AddString("音乐");
	m_cboType.AddString("网站");
	m_cboType.SetCurSel(0);

	m_lstFav.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstFav.InsertColumn(0, "序号", LVCFMT_LEFT, 0);
	m_lstFav.InsertColumn(1, "名称", LVCFMT_LEFT, 80);
	m_lstFav.InsertColumn(2, "类型", LVCFMT_LEFT, 60);
	m_lstFav.InsertColumn(3, "网址", LVCFMT_LEFT, 200);

	std::string ErrInfo;
	i8desk::FavoriteVector FavInfos;

	pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	if (!pDlg->m_pDbMgr->GetAllFavorite(FavInfos, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		OnCancel();
		return TRUE;
	}

	TCHAR szFavNo[BUFSIZ];
	for (i8desk::FavoriteVectorItr it = FavInfos.begin();
		it != FavInfos.end(); it ++)
	{
		int nItem = m_lstFav.InsertItem(m_lstFav.GetItemCount(), 
			::_itot((*it)->Serial, szFavNo, 10));

		m_lstFav.SetItemText(nItem, 1, (*it)->Name);
		m_lstFav.SetItemText(nItem, 2, (*it)->Type);
		m_lstFav.SetItemText(nItem, 3, (*it)->URL);

		char* p = new char[strlen((*it)->UID) + 1];
		lstrcpy(p, (*it)->UID);
		m_lstFav.SetItemData(nItem, reinterpret_cast<DWORD>(p));

		if ((*it)->Serial > m_dwFavMaxNo)
			m_dwFavMaxNo = (*it)->Serial;
	}

	if (m_lstFav.GetItemCount())
	{
		m_lstFav.SetItemState(0, LVIS_SELECTED|LVFIS_FOCUSED, LVIS_SELECTED|LVFIS_FOCUSED);
	}
	
	CenterWindow();

	return TRUE;
}

int  CFavMgrDlg::FindListItem(int nSubItem, LPCTSTR text)
{
	int n = m_lstFav.GetItemCount();
	for (int i = 0; i < n; i++) {
		if (m_lstFav.GetItemText(i, nSubItem) == text)
			return i;
	}

	return -1;
}

void CFavMgrDlg::OnBnClickedAdd()
{
	CString szName;
	GetDlgItemText(IDC_NAME, szName);	
	if (szName.IsEmpty())
		return ;

	CString szURL, szType;
	GetDlgItemText(IDC_URL,  szURL);
	m_cboType.GetLBText(m_cboType.GetCurSel(), szType);

	if (-1 == FindListItem(1, szName))
	{
		i8desk::tagFavorite tagFav;
		i8desk::SAFE_STRCPY(tagFav.UID, i8desk::CreateGUIDString().c_str());
		i8desk::SAFE_STRCPY(tagFav.Name, szName);
		i8desk::SAFE_STRCPY(tagFav.Type, szType);
		i8desk::SAFE_STRCPY(tagFav.URL, szURL);
		tagFav.Serial = m_dwFavMaxNo + 1;
		
		std::string ErrInfo;
		if (!pDlg->m_pDbMgr->AddFavorite(&tagFav, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return ;
		}
		m_dwFavMaxNo++;

		TCHAR szFavNo[BUFSIZ];
		int nItem = m_lstFav.InsertItem(m_lstFav.GetItemCount(), 
			::_itot(tagFav.Serial, szFavNo, 10));

		m_lstFav.SetItemText(nItem, 1, szName);
		m_lstFav.SetItemText(nItem, 2, szType);
		m_lstFav.SetItemText(nItem, 3, szURL);

		char* pGuid = new char[strlen(tagFav.UID) + 1];
		lstrcpy(pGuid, tagFav.UID);
		m_lstFav.SetItemData(nItem, reinterpret_cast<DWORD>(pGuid));
	}	
}

void CFavMgrDlg::OnBnClickedModify()
{
	int nSel = m_lstFav.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	CString szText;
	GetDlgItemText(IDC_NAME, szText);
	if (szText.IsEmpty())
		return ;

	CString szFavNo, szType, szURL;
	GetDlgItemText(IDC_FAV_NO, szFavNo);
	GetDlgItemText(IDC_URL, szURL);
	m_cboType.GetLBText(m_cboType.GetCurSel(), szType);

	int nFind = FindListItem(1, szText);
	if (-1 == nFind || nFind == nSel)
	{
		std::string ErrInfo;
		i8desk::tagFavorite tagFav;

		//预防数据库中不存在"网吧影音"这一特殊条目
		char *pGuid = reinterpret_cast<char*>(m_lstFav.GetItemData(nSel));
		ASSERT(pGuid);

		i8desk::SAFE_STRCPY(tagFav.UID, pGuid);
		i8desk::SAFE_STRCPY(tagFav.Name, szText);
		i8desk::SAFE_STRCPY(tagFav.Type, szType);
		i8desk::SAFE_STRCPY(tagFav.URL, szURL);

		if (pDlg->m_pDbMgr->ModifyFavorite(&tagFav, ErrInfo))
		{			
			m_lstFav.SetItemText(nSel, 1, tagFav.Name);
			m_lstFav.SetItemText(nSel, 2, tagFav.Type);
			m_lstFav.SetItemText(nSel, 3, tagFav.URL);
		}
		else
		{
			AfxMessageBox(ErrInfo.c_str());
		}
	}
}

void CFavMgrDlg::OnBnClickedDelete()
{
	int nSel = m_lstFav.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return;

	if (::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", 
		MB_ICONQUESTION|MB_OKCANCEL) != IDOK)
		return;
	
	char* pGuid = reinterpret_cast<char*>(m_lstFav.GetItemData(nSel));
	ASSERT(pGuid);
	
	std::string ErrInfo;
	if (!pDlg->m_pDbMgr->DelFavorite(pGuid, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	delete []pGuid;
	m_lstFav.DeleteItem(nSel);
}

void CFavMgrDlg::OnLvnItemChangedListFav(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = m_lstFav.GetNextItem(-1, LVIS_SELECTED);

	if (nSel == -1)
		return ;

	SetDlgItemText(IDC_FAV_NO, m_lstFav.GetItemText(nSel, 0));
	SetDlgItemText(IDC_NAME, m_lstFav.GetItemText(nSel, 1));
	SetDlgItemText(IDC_URL,  m_lstFav.GetItemText(nSel, 3));
	m_cboType.SelectString(-1, m_lstFav.GetItemText(nSel, 2));

	GetDlgItem(IDC_DELETE)->EnableWindow(TRUE);
	GetDlgItem(IDC_MOVE_UP)->EnableWindow(nSel != 0);
	GetDlgItem(IDC_MOVE_DOWN)->EnableWindow(nSel != m_lstFav.GetItemCount() - 1);
}

void CFavMgrDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstFav.GetItemCount(); idx++)
	{
		if (char* pGuid = reinterpret_cast<char*>(m_lstFav.GetItemData(idx)))
		{
			delete[] pGuid;
		}
	}
	CDialog::OnDestroy();
}

void CFavMgrDlg::OnBnClickedOk()
{
}

void CFavMgrDlg::OnBnClickedMoveUp()
{
	if (POSITION pos = m_lstFav.GetFirstSelectedItemPosition())
	{
		int nItem = m_lstFav.GetNextSelectedItem(pos);
		ASSERT(nItem != 0);
		SwapListItem(nItem, nItem - 1);
		m_lstFav.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFavMgrDlg::OnBnClickedMoveDown()
{
	if (POSITION pos = m_lstFav.GetFirstSelectedItemPosition())
	{
		int nItem = m_lstFav.GetNextSelectedItem(pos);
		ASSERT(nItem != m_lstFav.GetItemCount() - 1);
		SwapListItem(nItem, nItem + 1);
		m_lstFav.SetItemState(nItem + 1, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFavMgrDlg::SwapListItem(int first, int second)
{
	ASSERT(first >= 0 && first < m_lstFav.GetItemCount()
		&& second >= 0  && second < m_lstFav.GetItemCount()
		&& first != second);

	i8desk::Favorite fav_first, fav_second;

	fav_first.Serial	= ::_ttol(m_lstFav.GetItemText(first, 0));
	i8desk::SAFE_STRCPY(fav_first.Name, m_lstFav.GetItemText(first, 1));
	i8desk::SAFE_STRCPY(fav_first.Type, m_lstFav.GetItemText(first, 2));
	i8desk::SAFE_STRCPY(fav_first.URL, m_lstFav.GetItemText(first, 3));
	i8desk::SAFE_STRCPY(fav_first.UID, (char *)m_lstFav.GetItemData(first));

	fav_second.Serial	= ::_ttol(m_lstFav.GetItemText(second, 0));
	i8desk::SAFE_STRCPY(fav_second.Name, m_lstFav.GetItemText(second, 1));
	i8desk::SAFE_STRCPY(fav_second.Type, m_lstFav.GetItemText(second, 2));
	i8desk::SAFE_STRCPY(fav_second.URL, m_lstFav.GetItemText(second, 3));
	i8desk::SAFE_STRCPY(fav_second.UID, (char *)m_lstFav.GetItemData(second));

	std::swap(fav_first.Serial, fav_second.Serial);

	std::string ErrInfo;
	
	//交换序号到数据库
	if (!pDlg->m_pDbMgr->ModifyFavorite(&fav_first, ErrInfo)
		|| !pDlg->m_pDbMgr->ModifyFavorite(&fav_second, ErrInfo))
	{
		AfxMessageBox(ErrInfo.c_str());
		return;
	}

	CString strTmp;
	strTmp = m_lstFav.GetItemText(first, 1);
	m_lstFav.SetItemText(first, 1, m_lstFav.GetItemText(second, 1));
	m_lstFav.SetItemText(second, 1, strTmp);

	strTmp = m_lstFav.GetItemText(first, 2);
	m_lstFav.SetItemText(first, 2, m_lstFav.GetItemText(second, 2));
	m_lstFav.SetItemText(second, 2, strTmp);

	strTmp = m_lstFav.GetItemText(first, 3);
	m_lstFav.SetItemText(first, 3, m_lstFav.GetItemText(second, 3));
	m_lstFav.SetItemText(second, 3, strTmp);


	DWORD_PTR pTmp = m_lstFav.GetItemData(first);
	m_lstFav.SetItemData(first, m_lstFav.GetItemData(second));
	m_lstFav.SetItemData(second, pTmp);
}