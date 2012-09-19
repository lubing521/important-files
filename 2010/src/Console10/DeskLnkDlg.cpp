// DeskLnkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Console.h"
#include "DeskLnkDlg.h"
#include "ConsoleDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CDeskLnkDlg, CDialog)

CDeskLnkDlg::CDeskLnkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeskLnkDlg::IDD, pParent)
	, m_bModify(FALSE)
{

}

CDeskLnkDlg::~CDeskLnkDlg()
{
}

void CDeskLnkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DESKLNK, m_lstDeskLnk);
	DDX_Control(pDX, IDC_LIST_DESKLNK_CLASS, m_lstDeskClassLnk);
}

BEGIN_MESSAGE_MAP(CDeskLnkDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDeskLnkDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDeskLnkDlg::OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(i8desk::g_nOptApplyMsg, CDeskLnkDlg::OnApplyMessage)

	ON_BN_CLICKED(IDC_ADD, &CDeskLnkDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_MODIFY, &CDeskLnkDlg::OnBnClickedModify)
	ON_BN_CLICKED(IDC_DEL, &CDeskLnkDlg::OnBnClickedDel)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DESKLNK, &CDeskLnkDlg::OnLvnItemchangedListDesklnk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DESKLNK_CLASS, &CDeskLnkDlg::OnLvnItemchangedListDesklnkClass)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DESKLNK_CLASS, &CDeskLnkDlg::OnNMClickListDesklnkClass)
END_MESSAGE_MAP()


void CDeskLnkDlg::_InitClassShortcuts()
{
	// 检查路径
	i8desk::mkpath(GetConsoleDlg()->GetI8DeskSvrPath() + _T("Data\\ClassIcon"));

	i8desk::std_string strShortcuts(GetConsoleDlg()->m_pDbMgr->GetOptString(_T("ClassShortcuts")));

	int nPos = 0;
	for(; ;)
	{
		i8desk::std_string::size_type nCount = strShortcuts.find_first_of(_T('|'), nPos); 

		// 没有找到
		if( nCount == i8desk::std_string::npos )
			break;

		m_vecClassShortcuts.push_back(strShortcuts.substr(nPos, nCount - nPos));
		nPos = nCount + 1;
	}

	//std::sort(m_vecClassShortcuts.begin(), m_vecClassShortcuts.end());
}


void CDeskLnkDlg::_InitListCtrl()
{
	m_lstDeskLnk.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_lstDeskLnk.InsertColumn(0, "快捷名称", LVCFMT_LEFT, 80);
	m_lstDeskLnk.InsertColumn(1, "执行文件", LVCFMT_LEFT, 160);
	m_lstDeskLnk.InsertColumn(2, "执行参数", LVCFMT_LEFT, 120);
	m_lstDeskLnk.InsertColumn(3, "图标文件", LVCFMT_LEFT, 120);


	m_images.Create(16, 16, ILC_COLOR16 | ILC_MASK, 1, 0);
	
	m_lstDeskClassLnk.SetExtendedStyle(m_lstDeskClassLnk.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_lstDeskClassLnk.SetImageList(&m_images, LVSIL_SMALL);
	
	m_lstDeskClassLnk.SetCellMargin(1.0);


	//CGridColumnTrait *pTrait = new CGridColumnTraitText;
	//m_lstDeskClassLnk.SetDefaultRowTrait(new CGridRowTraitXP);
	//m_lstDeskClassLnk.InsertHiddenLabelColumn();	// Requires one never uses column 0


	
	m_lstDeskClassLnk.InsertColumnTrait(0, _T(""), LVCFMT_LEFT, 18, -1);
	m_lstDeskClassLnk.InsertColumnTrait(1, _T("    图标"), LVCFMT_LEFT, 55, -1);
	m_lstDeskClassLnk.InsertColumnTrait(2, _T("      类别"), LVCFMT_LEFT, 300, -1);
	

	// 插入已有类别
	// 
	for(i8desk::DefClassMapItr iter = GetConsoleDlg()->m_ClassInfos.begin();
		iter != GetConsoleDlg()->m_ClassInfos.end(); iter++)
	{
		int nItem = m_lstDeskClassLnk.InsertItem(m_lstDeskClassLnk.GetItemCount(), _T(""));

		/*if( std::binary_search(m_vecClassShortcuts.begin(), m_vecClassShortcuts.end(), iter->second->Name) )*/
		if( std::find(m_vecClassShortcuts.begin(), m_vecClassShortcuts.end(), iter->second->Name) != m_vecClassShortcuts.end() )
			m_lstDeskClassLnk.SetCheck(nItem, TRUE);

		// 加载对应图标
		CString strIconPath = GetConsoleDlg()->GetI8DeskSvrPath() + _T("Data\\ClassIcon\\") + iter->second->Name + _T(".ico");
		HICON hIcon = static_cast<HICON>(::LoadImage(AfxGetInstanceHandle(), strIconPath, IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE));
		
		// 如果失败，加载默认图标
		if( hIcon == NULL )
		{
			m_images.Add(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
			m_lstDeskClassLnk.SetItemText(nItem, 1, _T("图标"));
		}
		else
		{
			int nIndex = m_images.Add(hIcon);
			m_lstDeskClassLnk.SetCellImage(nItem, 1, nIndex);
		}
	
		
		m_lstDeskClassLnk.SetItemText(nItem, 2, iter->second->Name);
	}
}


void CDeskLnkDlg::_InsertBootTask()
{
	i8desk::CDbMgr* pDbMgr = GetConsoleDlg()->m_pDbMgr;
	std::map<std::string, i8desk::tagBootTask*> BootTasks;
	std::string ErrInfo;

	if(!pDbMgr->GetBootTask(brShortcut, BootTasks, ErrInfo))
	{
		CString str;
		str.Format(_T("初始化桌面快捷方式设置窗口数据时:%s"), ErrInfo.c_str());
		AfxMessageBox(str);
	}

	for(std::map<std::string, i8desk::tagBootTask*>::iterator it = BootTasks.begin();
		it != BootTasks.end(); it++)
	{
		if (it->second->AreaType == i8desk::TASK_AREA_ALL)
		{
			CString Name, ExeFile, IconFile, ExeParam;
			AfxExtractSubString(Name, it->second->Content, 0, '|');
			AfxExtractSubString(ExeFile, it->second->Content, 1, '|');
			AfxExtractSubString(IconFile, it->second->Content, 2, '|');
			AfxExtractSubString(ExeParam, it->second->Content, 3, '|');


			int nItem = m_lstDeskLnk.InsertItem(m_lstDeskLnk.GetItemCount(), Name);
			m_lstDeskLnk.SetItemText(nItem, 1, ExeFile);
			m_lstDeskLnk.SetItemText(nItem, 2, ExeParam);
			m_lstDeskLnk.SetItemText(nItem, 3, IconFile);


			char* p = new char[strlen(it->second->UID) + 1];
			lstrcpy(p, it->second->UID);
			m_lstDeskLnk.SetItemData(nItem, reinterpret_cast<DWORD>(p));
		}

		delete it->second;
	}

	if (m_lstDeskLnk.GetItemCount())
		m_lstDeskLnk.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

}

BOOL CDeskLnkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初始化本地资源类别
	_InitClassShortcuts();

	// 初始化List  Ctrl
	_InitListCtrl();

	// 插入开机任务
	_InsertBootTask();
	
	return TRUE;
}

void CDeskLnkDlg::OnBnClickedOk()
{
}

void CDeskLnkDlg::OnBnClickedCancel()
{

}

void CDeskLnkDlg::OnBnClickedAdd()
{
	CString strName, strExeFile, strIconFile, strExeParam;
	GetDlgItemText(IDC_NAME,	strName);
	GetDlgItemText(IDC_EXEFILE, strExeFile);
	GetDlgItemText(IDC_EXEPARAM,strExeParam);
	GetDlgItemText(IDC_ICONFILE,strIconFile);

	if (strName.IsEmpty())
	{
		AfxMessageBox("快捷方式名称不能为空!");
		GetDlgItem(IDC_NAME)->SetFocus();
		return ;
	}

	if (strExeFile.IsEmpty())
	{
		AfxMessageBox("执行文件不能为空!");
		GetDlgItem(IDC_EXEFILE)->SetFocus();
		return ;
	}

	LVFINDINFO info = {0};
	info.psz = strName;
	if (-1 != m_lstDeskLnk.FindItem(&info))
	{
		AfxMessageBox("快捷方式名称己经存在!");
		GetDlgItem(IDC_NAME)->SetFocus();
		return ;
	}

	int nItem = m_lstDeskLnk.InsertItem(m_lstDeskLnk.GetItemCount(), strName);
	m_lstDeskLnk.SetItemText(nItem, 1, strExeFile);
	m_lstDeskLnk.SetItemText(nItem, 2, strExeParam);
	m_lstDeskLnk.SetItemText(nItem, 3, strIconFile);

	std::string sGuid = i8desk::CreateGUIDString();
	char* p = new char[sGuid.size()+1];

	lstrcpy(p, sGuid.c_str());
	m_lstDeskLnk.SetItemData(nItem, reinterpret_cast<DWORD>(p));

	m_bModify = TRUE;
}

void CDeskLnkDlg::OnBnClickedModify()
{
	int nSel = m_lstDeskLnk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1) return ;

	CString strName, strExeFile, strIconFile, strExeParam;
	GetDlgItemText(IDC_NAME,	strName);
	GetDlgItemText(IDC_EXEFILE, strExeFile);
	GetDlgItemText(IDC_EXEPARAM,strExeParam);
	GetDlgItemText(IDC_ICONFILE,strIconFile);

	if (strName.IsEmpty())
	{
		AfxMessageBox("快捷方式名称不能为空!");
		GetDlgItem(IDC_NAME)->SetFocus();
		return ;
	}

	if (strExeFile.IsEmpty())
	{
		AfxMessageBox("执行文件不能为空!");
		GetDlgItem(IDC_EXEFILE)->SetFocus();
		return ;
	}

	LVFINDINFO info = {0};
	info.psz = strName;
	int nFind = m_lstDeskLnk.FindItem(&info);
	if (nFind != -1 && nFind != nSel)
	{
		AfxMessageBox("快捷方式名称己经存在!");
		GetDlgItem(IDC_NAME)->SetFocus();
		return ;
	}

	m_lstDeskLnk.SetItemText(nSel, 0, strName);
	m_lstDeskLnk.SetItemText(nSel, 1, strExeFile);
	m_lstDeskLnk.SetItemText(nSel, 2, strExeParam);
	m_lstDeskLnk.SetItemText(nSel, 3, strIconFile);

	m_bModify = TRUE;
}

void CDeskLnkDlg::OnBnClickedDel()
{
	int nSel = m_lstDeskLnk.GetNextItem(-1, LVIS_SELECTED);

	if (nSel != -1 && ::MessageBox(m_hWnd, "确定要删除选定的项吗?", "提示:", 
		MB_ICONQUESTION|MB_OKCANCEL) == IDOK)
	{
		char*p = reinterpret_cast<char*>(m_lstDeskLnk.GetItemData(nSel));
		delete []p;
		m_lstDeskLnk.DeleteItem(nSel);
		m_bModify = TRUE;
	}
}

void CDeskLnkDlg::OnDestroy()
{
	for (int idx=0; idx<m_lstDeskLnk.GetItemCount(); idx++)
	{
		char*p = reinterpret_cast<char*>(m_lstDeskLnk.GetItemData(idx));
		delete []p;
	}

	m_images.DeleteImageList();

	CDialog::OnDestroy();
}

LRESULT CDeskLnkDlg::OnApplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_bModify)
	{
		i8desk::CDbMgr* pDbMgr = reinterpret_cast<CConsoleDlg*>(AfxGetMainWnd())->m_pDbMgr;
		std::string ErrInfo;
		if (!pDbMgr->DelBootTask(brShortcut, ErrInfo))
		{
			AfxMessageBox(ErrInfo.c_str());
			return FALSE;
		}

		// 写入开机任务网址快捷方式
		for (int idx=0; idx<m_lstDeskLnk.GetItemCount(); idx++)
		{
			i8desk::tagBootTask Task;
			char* pGuid = reinterpret_cast<char*>(m_lstDeskLnk.GetItemData(idx));
			i8desk::SAFE_STRCPY(Task.UID, pGuid);
			Task.AreaType = i8desk::TASK_AREA_ALL;
			CLEAR_STRING(Task.AreaParam);
			Task.Type = brShortcut;
			Task.Flag = 2;
			i8desk::SAFE_STRCPY(Task.Content, m_lstDeskLnk.GetItemText(idx, 0) 
				+ "|" + m_lstDeskLnk.GetItemText(idx, 1) 
				+ "|" + m_lstDeskLnk.GetItemText(idx, 3)
				+ "|" + m_lstDeskLnk.GetItemText(idx, 2));			
			if (!pDbMgr->AddBootTask(&Task, ErrInfo))
			{
				AfxMessageBox(ErrInfo.c_str());
				return FALSE;
			}
		}


		// 写入开机任务类别快捷方式
		i8desk::std_string strName;
		for(int idx = 0; idx < m_lstDeskClassLnk.GetItemCount(); idx++)
		{
			BOOL bChecked = m_lstDeskClassLnk.GetCheck(idx);

			if( bChecked == TRUE )
			{
				strName += m_lstDeskClassLnk.GetItemText(idx, 2) + _T("|");
			}
		}

		GetConsoleDlg()->m_pDbMgr->SetOption(_T("ClassShortcuts"), strName);

		m_bModify = FALSE;
	}
	return TRUE;
}

void CDeskLnkDlg::OnLvnItemchangedListDesklnk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uNewState & LVIS_SELECTED) == 0)
		return ;

	int nSel = m_lstDeskLnk.GetNextItem(-1, LVIS_SELECTED);
	if (nSel == -1)
		return ;

	SetDlgItemText(IDC_NAME,	m_lstDeskLnk.GetItemText(nSel, 0));
	SetDlgItemText(IDC_EXEFILE, m_lstDeskLnk.GetItemText(nSel, 1));
	SetDlgItemText(IDC_EXEPARAM,m_lstDeskLnk.GetItemText(nSel, 2));
	SetDlgItemText(IDC_ICONFILE,m_lstDeskLnk.GetItemText(nSel, 3));
}

void CDeskLnkDlg::OnLvnItemchangedListDesklnkClass(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMLV->uOldState == 0 && pNMLV->uNewState == 0)
		return;    // No change

	// Old check box state
	BOOL bPrevState = (BOOL)(((pNMLV->uOldState & LVIS_STATEIMAGEMASK)>>12)-1);  
	if (bPrevState < 0)		// On startup there's no previous state 
		bPrevState = 0;		// so assign as false (unchecked)

	// New check box state
	BOOL bChecked = 
		(BOOL)(((pNMLV->uNewState &LVIS_STATEIMAGEMASK)>>12)-1);   
	if (bChecked < 0) // On non-checkbox notifications assume false
		bChecked = 0; 

	if (bPrevState == bChecked) // No change in check box
		return;    // Now bChecked holds the new check box state

	m_bModify = TRUE;


}


void CDeskLnkDlg::OnNMClickListDesklnkClass(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString strIconPath = GetConsoleDlg()->GetI8DeskSvrPath() + _T("Data\\ClassIcon\\*.ico");
	if( pNMItemActivate->iSubItem == 1 )
	{
		// 选择所需图标，并保存
		CFileDialog dlg(TRUE, "*.ico",strIconPath,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  "Icon files(*.ico)|*.ico|Exe files(*.exe)|*.exe||");

		if( dlg.DoModal() == IDOK )
		{
			CString strFilePath = dlg.GetPathName();
	
			HICON hIcon = i8desk::LoadIconFromFile(strFilePath);

			int nID = m_images.Replace(pNMItemActivate->iItem, hIcon);
			m_lstDeskClassLnk.SetCellImage(pNMItemActivate->iItem, 1, nID);
			m_lstDeskClassLnk.SetItemText(pNMItemActivate->iItem, 1, _T(""));
			m_lstDeskClassLnk.SetCheck(pNMItemActivate->iItem, TRUE);

			CString strIconPath = GetConsoleDlg()->GetI8DeskSvrPath() + _T("Data\\ClassIcon\\") + m_lstDeskClassLnk.GetItemText(pNMItemActivate->iItem, 2) + _T(".ico");
			if( i8desk::SaveIconToFile(hIcon, strIconPath) == false )
				GetConsoleDlg()->InsertLog(_T("保存 %s 失败!"), strIconPath);

			::DestroyIcon(hIcon);
		}
	}

	*pResult = 0;
}
