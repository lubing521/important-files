// WallPaperDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "WallPaperDlg.h"
#include "ConsoleDlg.h"

IMPLEMENT_DYNAMIC(CWallPaperDlg, CDialog)

CWallPaperDlg::CWallPaperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWallPaperDlg::IDD, pParent)
{

}

CWallPaperDlg::~CWallPaperDlg()
{
}

static std::vector<std::string> m_lstPicFile;
#define WALLPAPER_DIR	"Data\\WallPaper"

void CWallPaperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WALLPAPER, m_lstWallPaper);
	DDX_Control(pDX, IDC_COMBO1, m_cboCell);
}

BEGIN_MESSAGE_MAP(CWallPaperDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CWallPaperDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWallPaperDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, &CWallPaperDlg::OnApplyMessage)
	ON_NOTIFY(NM_CLICK, IDC_LIST_WALLPAPER, &CWallPaperDlg::OnNMClickListWallpaper)
	ON_CBN_KILLFOCUS(IDC_COMBO1, &CWallPaperDlg::OnCbnKillfocusCombo1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_OPEN_DIR, &CWallPaperDlg::OnBnClickedOpenDir)
END_MESSAGE_MAP()

BOOL CWallPaperDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lstWallPaper.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_lstWallPaper.InsertColumn(0, "区域",		LVCFMT_LEFT, 90);
	m_lstWallPaper.InsertColumn(1, "图片文件",	LVCFMT_LEFT, 220);
	m_lstWallPaper.InsertColumn(2, "方式",		LVCFMT_LEFT, 60);

	CConsoleDlg* pDlg = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd());
	if (!pDlg->IsLocMachineIP())
	{
		GetDlgItem(IDC_OPEN_DIR)->EnableWindow(FALSE);
	}

	i8desk::CDbMgr* pDbMgr = pDlg->m_pDbMgr;
	i8desk::AreaInfoMap AreaInfos;
	std::map<std::string, i8desk::tagBootTask*> BootTasks;
	std::string ErrInfo;
	m_lstWallPaper.InsertItem(0, DEFAULT_AREA_NAME);
	char* pGuid = new char[lstrlen(DEFAULT_AREA_GUID) +1];
	lstrcpy(pGuid, DEFAULT_AREA_GUID);
	m_lstWallPaper.SetItemData(0, reinterpret_cast<DWORD>(pGuid));

	if (!pDbMgr->GetAllArea(AreaInfos, ErrInfo) || 
		!pDbMgr->GetBootTask(brDeskTop, BootTasks, ErrInfo))
	{
		CString str;
		str.Format(_T("初始化桌面墙纸设置窗口数据时:%s"), ErrInfo.c_str());
		AfxMessageBox(str);
	}
	for (i8desk::AreaInfoMapItr it = AreaInfos.begin();
		it != AreaInfos.end(); it++)
	{
		if (it->first != DEFAULT_AREA_GUID)
		{
			int nItem = m_lstWallPaper.InsertItem(
				m_lstWallPaper.GetItemCount(), it->second->Name);
			pGuid = new char[it->first.size() + 1];
			lstrcpy(pGuid, it->first.c_str());
			m_lstWallPaper.SetItemData(nItem, reinterpret_cast<DWORD>(pGuid));
		}
	}
	AreaInfos.clear();

	for (int idx=0; idx<m_lstWallPaper.GetItemCount(); idx++)
	{
		pGuid = reinterpret_cast<char*>(m_lstWallPaper.GetItemData(idx));
		std::map<std::string, i8desk::tagBootTask*>::iterator Iter = BootTasks.begin();
		for (; Iter != BootTasks.end(); Iter++)
		{
			if (Iter->second->AreaType == i8desk::TASK_AREA_AREA
				&& strcmp(pGuid, Iter->second->AreaParam) == 0)
			{
				CString szFile, szWallType;
				AfxExtractSubString(szFile,		Iter->second->Content, 0, '|');
				AfxExtractSubString(szWallType, Iter->second->Content, 1, '|');
				szFile.Delete(0, lstrlen(WALLPAPER_DIR)+1);
				m_lstWallPaper.SetItemText(idx, 1, szFile);
				DWORD nWallType = atoi(szWallType);
				if (nWallType == 0)
					m_lstWallPaper.SetItemText(idx, 2, "居中");
				else if (nWallType == 1)
					m_lstWallPaper.SetItemText(idx, 2, "平铺");
				else
					m_lstWallPaper.SetItemText(idx, 2, "拉伸");
				break;
			}
		}
	}

	for (std::map<std::string, i8desk::tagBootTask*>::iterator Iter = BootTasks.begin();
		Iter != BootTasks.end(); Iter++)
	{
		delete Iter->second;
	}
	m_bModify = FALSE;

	return TRUE;
}

void CWallPaperDlg::OnBnClickedOk()
{
}

void CWallPaperDlg::OnBnClickedCancel()
{

}

LRESULT CWallPaperDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		std::string ErrInfo;
		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		if (!pDbMgr->DelBootTask(brDeskTop, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return FALSE;
		}
		for (int idx=0;idx<m_lstWallPaper.GetItemCount();idx++)
		{
			CString szFile = m_lstWallPaper.GetItemText(idx, 1);
			CString szType = m_lstWallPaper.GetItemText(idx, 2);
			if (!szFile.IsEmpty())
			{
				szFile = CString(WALLPAPER_DIR) + "\\" + szFile + "|";
				if (szType == "平铺")
					szFile += "1";
				else if (szType == "拉伸")
					szFile += "2";
				else 
					szFile += "0";
				i8desk::tagBootTask Task;
				i8desk::SAFE_STRCPY(Task.UID, i8desk::CreateGUIDString().c_str());
				Task.AreaType = i8desk::TASK_AREA_AREA;
				i8desk::SAFE_STRCPY(Task.AreaParam, reinterpret_cast<const char*>(m_lstWallPaper.GetItemData(idx)));
				Task.Type = brDeskTop;
				Task.Flag = 2;
				i8desk::SAFE_STRCPY(Task.Content, szFile);
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

static int g_nLastSelItem = 0;
static int g_nLastCol = 0;

void CWallPaperDlg::OnNMClickListWallpaper(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	*pResult = 0;

	int nSelItem = m_lstWallPaper.GetNextItem(-1, LVIS_SELECTED);
	if (nSelItem == -1) return ;

	CPoint pt( ::GetMessagePos() ); 
	ScreenToClient(&pt);

	CRect rc;
	m_lstWallPaper.GetSubItemRect(nSelItem, 2, LVIR_BOUNDS, rc);
	m_lstWallPaper.ClientToScreen(&rc);
	ScreenToClient(&rc);
	if (rc.PtInRect(pt))
	{
		g_nLastSelItem = nSelItem;
		g_nLastCol     = 2;
		CString szText = m_lstWallPaper.GetItemText(nSelItem, 2);

		rc.DeflateRect(0, 0, 1, 1);
		m_cboCell.ResetContent();
		m_cboCell.AddString("居中");
		m_cboCell.AddString("平铺");
		m_cboCell.AddString("拉伸");

		m_cboCell.MoveWindow(&rc);
		m_cboCell.BringWindowToTop();
		m_cboCell.ShowWindow(TRUE);
		m_cboCell.SelectString(-1, szText);
 		m_cboCell.SetFocus();
	}

	m_lstWallPaper.GetSubItemRect(nSelItem, 1, LVIR_BOUNDS, rc);
	m_lstWallPaper.ClientToScreen(&rc);
	ScreenToClient(&rc);
	if (rc.PtInRect(pt))
	{
		g_nLastSelItem = nSelItem;
		g_nLastCol     = 1;
		CString szText = m_lstWallPaper.GetItemText(nSelItem, 1);

		rc.DeflateRect(0, 0, 1, 1);
		m_cboCell.ResetContent();
		m_cboCell.AddString("不设置");

		std::string ErrInfo;
		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		m_lstPicFile.clear();
		if (!pDbMgr->GetFileOrDir(WALLPAPER_DIR, GET_FILELIST, false, m_lstPicFile, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
		}

		for (size_t idx=0; idx<m_lstPicFile.size(); idx++)
		{
			std::string file = m_lstPicFile[idx];
			std::string sub = (file.size() > 4) ? file.substr(file.size()-4) : "";
			if (lstrcmpi(sub.c_str(), ".jpg") == 0 || 
				lstrcmpi(sub.c_str(), ".bmp") == 0 || 
				lstrcmpi(sub.c_str(), ".png") == 0 ||
				lstrcmpi(sub.c_str(), ".gif") == 0)
				m_cboCell.AddString(file.c_str());
		}

		m_cboCell.MoveWindow(&rc);
		m_cboCell.BringWindowToTop();
		m_cboCell.ShowWindow(TRUE);
		if (-1 == m_cboCell.SelectString(-1, szText))
			m_cboCell.SetCurSel(0);
		m_cboCell.SetFocus();
	}
}

void CWallPaperDlg::OnCbnKillfocusCombo1()
{
	CString szText;
	m_cboCell.GetWindowText(szText);
	if (g_nLastCol == 2)
	{
		if (!m_lstWallPaper.GetItemText(g_nLastSelItem, 1).IsEmpty())
		{
			if (m_lstWallPaper.GetItemText(g_nLastSelItem, 2) != szText)
			{
				m_lstWallPaper.SetItemText(g_nLastSelItem, 2, szText);
				m_bModify = TRUE;
			}
		}
	}
	else if (g_nLastCol == 1)
	{
		if (szText == "不设置")
		{
			szText = "";
		}
		if (m_lstWallPaper.GetItemText(g_nLastSelItem, 1) != szText)
		{
			m_lstWallPaper.SetItemText(g_nLastSelItem, 1, szText);
			if (m_lstWallPaper.GetItemText(g_nLastSelItem, 2).IsEmpty())
				m_lstWallPaper.SetItemText(g_nLastSelItem, 2, "居中");
			m_bModify = TRUE;
		}
	}
	m_cboCell.ShowWindow(FALSE);
}

BOOL CWallPaperDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->hwnd == m_cboCell.m_hWnd 
		&& pMsg->message == WM_KEYDOWN 
		&& pMsg->wParam == VK_RETURN)
	{
		m_lstWallPaper.SetFocus();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CWallPaperDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstWallPaper.GetItemCount(); idx++)
	{
		char *pGuid = reinterpret_cast<char*>(m_lstWallPaper.GetItemData(idx));
		delete []pGuid;
	}
	CDialog::OnDestroy();
}

void CWallPaperDlg::OnBnClickedOpenDir()
{
	CString szSvrPath = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->GetI8DeskSvrPath();
	if (szSvrPath.IsEmpty())
		return ;

	szSvrPath += WALLPAPER_DIR;
	CreateDirectory(szSvrPath, NULL);
	ShellExecute(m_hWnd, "open", szSvrPath, "", NULL, SW_SHOWNORMAL);
}
