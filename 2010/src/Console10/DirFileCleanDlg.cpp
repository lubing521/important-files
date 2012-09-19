// DirFileCleanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DirFileCleanDlg.h"
#include "ConsoleDlg.h"
#include "AddDirFileDlg.h"

IMPLEMENT_DYNAMIC(CDirFileCleanDlg, CDialog)

CDirFileCleanDlg::CDirFileCleanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDirFileCleanDlg::IDD, pParent)
{

}

CDirFileCleanDlg::~CDirFileCleanDlg()
{
}

void CDirFileCleanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTDIR, m_lstDir);
	DDX_Control(pDX, IDC_LISTDRV, m_lstDrv);
	DDX_Control(pDX, IDC_DRV_SIZE, m_edtSize);
}

BEGIN_MESSAGE_MAP(CDirFileCleanDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDirFileCleanDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDirFileCleanDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CDirFileCleanDlg::OnApplyMessage)
	ON_BN_CLICKED(IDC_ADD, &CDirFileCleanDlg::OnBnClickedAddDirFile)
	ON_BN_CLICKED(IDC_DEL, &CDirFileCleanDlg::OnBnClickedDel)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LISTDRV, &CDirFileCleanDlg::OnNMClickListdrv)
	ON_EN_KILLFOCUS(IDC_DRV_SIZE, &CDirFileCleanDlg::OnEnKillfocusDrvSize)
	ON_EN_CHANGE(IDC_EXCLUDE_DIR, &CDirFileCleanDlg::OnEnChangeExcludeDir)
END_MESSAGE_MAP()

BOOL CDirFileCleanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_edtSize.LimitText(5);
	m_lstDir.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstDir.InsertColumn(0, "类型", LVCFMT_LEFT, 60);
	m_lstDir.InsertColumn(1, "内容", LVCFMT_LEFT, 240);

	i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
	std::map<std::string, i8desk::tagBootTask*> BootTasks;
	std::string ErrInfo;
	if (!pDbMgr->GetBootTask(brDelFile, BootTasks, ErrInfo))
	{
		CString str;
		str.Format(_T("取开机清理文件任务时:%s"), ErrInfo.c_str());
		AfxMessageBox(str);
	}
	for (std::map<std::string, i8desk::tagBootTask*>::iterator it = BootTasks.begin();
		it != BootTasks.end(); it++)
	{
		if (it->second->AreaType == i8desk::TASK_AREA_ALL)
		{
			int nItem = m_lstDir.GetItemCount();
			if (!STRING_ISEMPTY(it->second->Content) && LAST_CHAR(it->second->Content) == '\\')
				m_lstDir.InsertItem(nItem, "目录");
			else
				m_lstDir.InsertItem(nItem, "文件");
			m_lstDir.SetItemText(nItem, 1, it->second->Content);
			char* p = new char[strlen(it->second->UID) + 1];
			lstrcpy(p, it->second->UID);
			m_lstDir.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}
		delete it->second;
	}
	BootTasks.clear();

	SetDlgItemText(IDC_EXCLUDE_DIR, 
		pDbMgr->GetOptString(OPT_M_EXCLUDEDIR, _T("")).c_str());

	CString szItem;
	m_lstDrv.InsertColumn(0, "盘符",		LVCFMT_LEFT, 60);
	m_lstDrv.InsertColumn(1, "保留空间(M)", LVCFMT_LEFT, 120);
	m_lstDrv.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	for (char chDrv = 'D'; chDrv <= 'Z'; chDrv++)
	{
		szItem.Format("%C", chDrv);
		m_lstDrv.InsertItem(chDrv - 'D', szItem);
	}
	if (!pDbMgr->GetBootTask(brDelGames, BootTasks, ErrInfo))
	{
		CString str;
		str.Format(_T("取开机清理游戏任务时:%s"), ErrInfo.c_str());
		AfxMessageBox(str);
	}
	for (std::map<std::string, i8desk::tagBootTask*>::iterator it = BootTasks.begin();
		it != BootTasks.end(); it++)
	{
		if (it->second->AreaType == i8desk::TASK_AREA_ALL)
		{
			CString szDrv, szSize;
			AfxExtractSubString(szDrv,  it->second->Content, 0, '|');
			AfxExtractSubString(szSize, it->second->Content, 1, '|');
			LVFINDINFO info = {0};
			szDrv.MakeUpper();
			info.psz = szDrv;
			int nFind = m_lstDrv.FindItem(&info);
			if (-1 != nFind)
			{
				m_lstDrv.SetItemText(nFind, 1, szSize);
			}
		}
		delete it->second;
	}

	if (m_lstDir.GetItemCount())
		m_lstDir.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_bModify = FALSE;

	return TRUE;
}

void CDirFileCleanDlg::OnBnClickedAddDirFile()
{
	CAddDirFileDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		LVFINDINFO info = {0};
		info.psz = dlg.m_strDirFile;
		if (-1 == m_lstDir.FindItem(&info))
		{
			int nItem = m_lstDir.InsertItem(m_lstDir.GetItemCount(), dlg.m_bIsDir ? "目录" : "文件");
			if (dlg.m_bIsDir && dlg.m_strDirFile.Right(1) != "\\")
				dlg.m_strDirFile += "\\";
			m_lstDir.SetItemText(nItem, 1, dlg.m_strDirFile);
			std::string sGuid = i8desk::CreateGUIDString();
			char *pGuid = new char[sGuid.size() + 1];
			lstrcpy(pGuid, sGuid.c_str());
			m_lstDir.SetItemData(nItem, reinterpret_cast<DWORD>(pGuid));
			m_bModify = TRUE;
		}
	}
}

void CDirFileCleanDlg::OnBnClickedDel()
{
	int nSel = m_lstDir.GetNextItem(-1, LVIS_SELECTED);
	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		char* pGuid = reinterpret_cast<char*>(m_lstDir.GetItemData(nSel));
		delete []pGuid;
		m_lstDir.DeleteItem(nSel);
		m_bModify = TRUE;
	}
}

void CDirFileCleanDlg::OnBnClickedOk()
{
	
}

void CDirFileCleanDlg::OnBnClickedCancel()
{
	
}

LRESULT CDirFileCleanDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		std::string ErrInfo;
		
		CString strExcludeDir;
		GetDlgItemText(IDC_EXCLUDE_DIR, strExcludeDir);
		if (!pDbMgr->SetOption(OPT_M_EXCLUDEDIR, (LPCTSTR)strExcludeDir))
		{
			AfxMessageBox(_T("设置排除目录失败"));
			return FALSE;
		}

		if (!pDbMgr->DelBootTask(brDelFile, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return FALSE;
		}
		for (int idx=0; idx<m_lstDir.GetItemCount(); idx++)
		{
			i8desk::tagBootTask Task;
			char* pGuid = reinterpret_cast<char*>(m_lstDir.GetItemData(idx));
			i8desk::SAFE_STRCPY(Task.UID, pGuid);
			Task.AreaType = i8desk::TASK_AREA_ALL;
			CLEAR_STRING(Task.AreaParam);
			Task.Type = brDelFile;
			Task.Flag = 2;
			i8desk::SAFE_STRCPY(Task.Content, m_lstDir.GetItemText(idx, 1));
			if (!pDbMgr->AddBootTask(&Task, ErrInfo))
			{
				AfxMessageBox(ErrInfo.c_str());
				return FALSE;
			}
		}


		if (!pDbMgr->DelBootTask(brDelGames, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return FALSE;
		}
		for (int idx=0; idx<m_lstDrv.GetItemCount(); idx++)
		{
			CString szDrv  = m_lstDrv.GetItemText(idx, 0);
			CString szSize = m_lstDrv.GetItemText(idx, 1);
			if (atoi(szSize) > 0)
			{
				i8desk::tagBootTask Task;
				i8desk::SAFE_STRCPY(Task.UID, i8desk::CreateGUIDString().c_str());
				Task.AreaType = i8desk::TASK_AREA_ALL;
				CLEAR_STRING(Task.AreaParam);
				Task.Type = brDelGames;
				Task.Flag = 2;
				i8desk::SAFE_STRCPY(Task.Content, szDrv + "|" + szSize);
				if (!pDbMgr->AddBootTask(&Task, ErrInfo))
				{
					AfxMessageBox(ErrInfo.c_str());
					return FALSE;
				}
			}
		}
		m_bModify = FALSE;
	}	
	return TRUE;
}

void CDirFileCleanDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstDir.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstDir.GetItemData(idx));
		delete []p;
	}
	CDialog::OnDestroy();
}

static int g_nLastSelItem = 0;

void CDirFileCleanDlg::OnNMClickListdrv(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nSelItem = m_lstDrv.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1) return ;

	CPoint pt( ::GetMessagePos() ); 
	ScreenToClient(&pt);

	CRect rc;
	m_lstDrv.GetSubItemRect(nSelItem, 1, LVIR_BOUNDS, rc);
	m_lstDrv.ClientToScreen(&rc);
	ScreenToClient(&rc);
	if (rc.PtInRect(pt))
	{
		g_nLastSelItem = nSelItem;
		CString szItemText = m_lstDrv.GetItemText(nSelItem, 1);
		CString szSize;
		szSize.Format("%d", atoi(szItemText));
		m_edtSize.SetWindowText(szSize);
		
		rc.DeflateRect(0, 0, 1, 1);
		m_edtSize.MoveWindow(&rc);
		m_edtSize.BringWindowToTop();
		m_edtSize.ShowWindow(TRUE);
		m_edtSize.SetSel(0, 9999);
		m_edtSize.SetFocus();
	}
}

void CDirFileCleanDlg::OnEnKillfocusDrvSize()
{
	CString szText;
	m_edtSize.GetWindowText(szText);
	int nSize = atoi(szText);
	if (atoi(m_lstDrv.GetItemText(g_nLastSelItem, 1)) != nSize)
	{
		if (nSize)
		{
			szText.Format("%d", (nSize > 20*1024) ? 20480 : nSize);
			m_lstDrv.SetItemText(g_nLastSelItem, 1, szText);
		}
		else
		{
			m_lstDrv.SetItemText(g_nLastSelItem, 1, "");
		}
		m_bModify = TRUE;
	}
	m_edtSize.ShowWindow(FALSE);
}

BOOL CDirFileCleanDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->hwnd == m_edtSize.m_hWnd && pMsg->message == WM_KEYDOWN 
		&& pMsg->wParam == VK_RETURN)
	{
		m_lstDrv.SetFocus();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDirFileCleanDlg::OnEnChangeExcludeDir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	m_bModify = TRUE;
}
