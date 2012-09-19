// DorpListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "UDiskClient.h"
#include "DropListCtrl.h"
#include "UDiskClientDlg.h"

// CDropListCtrl

IMPLEMENT_DYNAMIC(CDropListCtrl, CListCtrl)

CDropListCtrl::CDropListCtrl()
{
	ZeroMemory(m_szPath,MAX_PATH);
	m_bShowLst =false;
}

CDropListCtrl::~CDropListCtrl()
{

}
void CDropListCtrl::OnDropFiles(HDROP hDropInfo)
{
	m_vecFileLst.clear();
	UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0); //文件的个数
	for ( UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex )
	{
		ZeroMemory(m_szPath,MAX_PATH);
		DragQueryFile(hDropInfo, nIndex, m_szPath, _MAX_PATH);
		m_vecFileLst.push_back(string(m_szPath));
	}
	DragFinish(hDropInfo);
	LPARAM lParam = (LPARAM)(&m_vecFileLst);
	::PostThreadMessage(((CUDiskClientDlg*)AfxGetMainWnd())->m_threadID,
		WM_MSG_POSTFILE,0,lParam);

}

BEGIN_MESSAGE_MAP(CDropListCtrl, CListCtrl)
	ON_WM_DROPFILES() 
	ON_NOTIFY_REFLECT(NM_RCLICK, &CDropListCtrl::OnNMRClick)

	ON_COMMAND(ID_PostFile, &CDropListCtrl::OnPostfile)
	ON_COMMAND(ID_PostDir, &CDropListCtrl::OnPostdir)
	ON_COMMAND(ID_DowndFile, &CDropListCtrl::OnDowndfile)
	ON_COMMAND(ID_DeleteFile, &CDropListCtrl::OnDeletefile)
	//ON_COMMAND(ID_ShowLst, &CDropListCtrl::OnShowlst)
	ON_WM_CREATE()
	ON_COMMAND(ID_VLIST, &CDropListCtrl::OnVlist)
	ON_COMMAND(ID_VICON, &CDropListCtrl::OnVicon)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CDropListCtrl::OnNMDblclk)
END_MESSAGE_MAP()



// CDropListCtrl 消息处理程序



void CDropListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CPoint pt ( GetMessagePos() );
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	*pResult = 0;
}


void CDropListCtrl::OnPostfile()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE);
	if(dlg.DoModal()== IDOK)
	{
		m_vecFileLst.clear();
		CString str = dlg.GetFileName();
		m_vecFileLst.push_back(string(str));
		LPARAM lParam = (LPARAM)(&m_vecFileLst);
		::PostThreadMessage(((CUDiskClientDlg*)AfxGetMainWnd())->m_threadID,
			WM_MSG_POSTFILE,0,lParam);
	}

}

void CDropListCtrl::OnPostdir()
{
	BROWSEINFO bi;

//	TCHAR           szDisplayName[MAX_PATH]; 
	LPITEMIDLIST     pidl; 
	LPMALLOC     pMalloc = NULL;  
	ZeroMemory(&bi, sizeof(bi));      
	bi.hwndOwner = GetSafeHwnd(); 
	bi.pszDisplayName = m_szPath; 
	bi.lpszTitle = TEXT("请选择一个文件夹:"); 
	bi.ulFlags = BIF_RETURNONLYFSDIRS;  
	pidl = SHBrowseForFolder(&bi); 

	if (pidl) 
	{ 
		SHGetPathFromIDList(pidl, m_szPath); 
		m_vecFileLst.clear();
		m_vecFileLst.push_back(m_szPath);
		LPARAM lParam = (LPARAM)&m_vecFileLst;
		::PostThreadMessage(((CUDiskClientDlg*)AfxGetMainWnd())->m_threadID,
			WM_MSG_POSTFILE,0,lParam);
	}
	
	
}

void CDropListCtrl::OnDowndfile()
{

	CUDiskClientDlg* pthis =((CUDiskClientDlg*)AfxGetMainWnd());
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		m_vecFileLst.clear();
		while (pos)
		{
			int nItem = GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			CString str = GetItemText(nItem,0);
			m_vecFileLst.push_back(string(str));
		}
		LPARAM lParam = (LPARAM)(&m_vecFileLst);
		::PostThreadMessage(((CUDiskClientDlg*)AfxGetMainWnd())->m_threadID,
			WM_MSG_DOWNODFILE,0,lParam);
	}
	 	
}

void CDropListCtrl::OnDeletefile()
{

	CUDiskClientDlg* pthis =((CUDiskClientDlg*)AfxGetMainWnd());
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		while (pos)
		{
			int nItem = GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			CString str = GetItemText(nItem,0);
			char buf[MAX_PATH] ={0};
			sprintf_s(buf,"%s\\u盘\\%s",pthis->m_UserName.c_str(),str);
			pthis->m_pSocket->SendCmdToServer(NET_CMD_DELETEFILE,buf,lstrlen(buf));
		}
	}
	pthis->DrawFileList();
}


int CDropListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;


	
	return 0;
}

void CDropListCtrl::OnVlist()
{

	ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_REPORT|LVS_LIST,0);
	ModifyStyle(0,LVS_REPORT);
	int nColumnCount = GetHeaderCtrl()->GetItemCount();
	for (int i=0; i < nColumnCount; i++)
	{
		DeleteColumn(0);
	}
	InsertColumn(0,_T("文件名"),LVCFMT_LEFT,250);
	InsertColumn(1,_T("文件大小"),LVCFMT_LEFT,100);
	InsertColumn(1,_T("文件大小"),LVCFMT_LEFT,160);
	((CUDiskClientDlg*)AfxGetMainWnd())->DrawFileList();
}

void CDropListCtrl::OnVicon()
{
	ModifyStyle(LVS_ICON|LVS_SMALLICON|LVS_REPORT|LVS_LIST,0);
	ModifyStyle(0,LVS_SMALLICON);
}

void CDropListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		TRACE0("No items were selected!\n");
	else
	{
		m_vecFileLst.clear();
		while (pos)
		{
			int nItem = GetNextSelectedItem(pos);
			TRACE1("Item %d was selected!\n", nItem);
			CString str = GetItemText(nItem,0);
			m_vecFileLst.push_back(string(str));
		}
		LPARAM lParam = (LPARAM)(&m_vecFileLst);
		::PostThreadMessage(((CUDiskClientDlg*)AfxGetMainWnd())->m_threadID,
			WM_MSG_DOWNODFILE,1,lParam);
	}
	*pResult = 0;
}
