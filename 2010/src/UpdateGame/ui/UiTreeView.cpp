#include "stdafx.h"
#include "ServerUI.h"
#include "UiTreeView.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CUiTreeView, CTreeView)

CUiTreeView::CUiTreeView()
{

}

CUiTreeView::~CUiTreeView()
{
}

BEGIN_MESSAGE_MAP(CUiTreeView, CTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CUiTreeView::OnTvnSelchanged)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CUiTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CUiTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG

int CUiTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_ImagList.Create(16, 16, ILC_COLOR32|ILC_MASK, 1, 1);

	int Icon[4] ={IDI_ICON1,IDI_ICON2,IDI_ICON3,IDI_ICON4}; 
	for(int i =0; i<4; i++)
	{
		HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(Icon[i]));
		m_ImagList.Add(hIcon);
		DestroyIcon(hIcon);	
	}

	CTreeCtrl &treeCtrl=this->GetTreeCtrl();
	treeCtrl.ModifyStyle(0,TVS_HASLINES|TVS_LINESATROOT|TVS_SHOWSELALWAYS|
		TVS_HASBUTTONS);

	treeCtrl.SetImageList(&m_ImagList,TVSIL_NORMAL);
	char HostName[MAX_PATH] = {0};
	gethostname(HostName,MAX_PATH);
	CString str;
	str.Format("节点监控[%s]",HostName);
	HTREEITEM hPA =  treeCtrl.InsertItem(str,TVI_ROOT);
	treeCtrl.SetItemData(treeCtrl.InsertItem("虚拟盘",2,2,hPA),NODE_VDISK);
	treeCtrl.SetItemData(treeCtrl.InsertItem("内网更新",1,1,hPA),NODE_UPDATEGAME);
	HTREEITEM  hSyncPA = treeCtrl.InsertItem("同步列表",3,3,hPA);
	treeCtrl.SetItemData(hSyncPA,NODE_SYNCGAME);
	treeCtrl.Expand(hPA,TVE_EXPAND  ); 
	treeCtrl.SelectItem(hSyncPA);

	return 0;
}

void CUiTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CTreeCtrl &treeCtrl=this->GetTreeCtrl();
	HTREEITEM hItem = treeCtrl.GetSelectedItem();
	if(hItem ==  NULL)
		return ;

	switch((TUserData)treeCtrl.GetItemData(hItem))
	{
	case NODE_VDISK:
		{
			((CMainFrame*)AfxGetMainWnd())->OnVdiskinfo();
			break;
		}
	case NODE_UPDATEGAME:
		{
			((CMainFrame*)AfxGetMainWnd())->OnUpdategameinfo();
			break;
		}
	case NODE_SYNCGAME:
		{
			((CMainFrame*)AfxGetMainWnd())->OnSyncinfo();
			break;
		}
	default:break;
	}
}

void CUiTreeView::SeleteItem(LPCSTR ItemName)
{

	HTREEITEM RootItem = GetTreeCtrl().GetRootItem();
	while(RootItem)
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(RootItem);

		while (hChildItem != NULL)
		{
			hNextItem = GetTreeCtrl().GetNextItem(hChildItem, TVGN_NEXT);
			CString szName = GetTreeCtrl().GetItemText(hNextItem);
			if(szName.CompareNoCase(ItemName) == 0)
			{
				GetTreeCtrl().SelectItem(hNextItem);
				break;
			}
			hChildItem = hNextItem;
		}

	}
}
