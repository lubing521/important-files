// SyncGameInfoView.cpp : 实现文件
#include "stdafx.h"
#include "ServerUI.h"
#include "SyncGameInfoView.h"
#include "FindGameDlg.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CSyncGameInfoView, CListView)

CSyncGameInfoView::CSyncGameInfoView()
{

}

BEGIN_MESSAGE_MAP(CSyncGameInfoView, CListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CSyncGameInfoView::OnNMRClick)
	ON_COMMAND(ID_FORCEUPDATE, &CSyncGameInfoView::OnForceupdate)
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_FINDGAME, &CSyncGameInfoView::OnFindgame)
	ON_MESSAGE(WM_MYSELECTITEM,SelectItem)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CSyncGameInfoView::OnLvnColumnclick)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CSyncGameInfoView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CSyncGameInfoView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

int CSyncGameInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CListCtrl& pListCtrl = this->GetListCtrl();
	pListCtrl.ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	pListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


	pListCtrl.InsertColumn(0,_T("游戏编号"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(1,_T("游戏名称"),LVCFMT_LEFT,120);
	pListCtrl.InsertColumn(2,_T("当前状态"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(3,_T("当前进度"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(4,_T("更新量(M)"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(5,_T("剩余量(M)"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(6,_T("速度(K/S)"),LVCFMT_LEFT,80);

	return 0;
}

void CSyncGameInfoView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	POINT pt;
	 GetCursorPos(&pt);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN ,pt.x,pt.y,this);
	*pResult = 0;
}

bool CSyncGameInfoView::ForceUpdate(DWORD gid)
{
	SOCKET sock = ((CMainFrame*)AfxGetMainWnd())->m_pSocket->GetSocket();
	char buf[MAX_PATH] = {0};
	CPackageHelper inpack(buf);
	inpack.pushDWORD(gid);
	inpack.pushDWORD(1);

	_packageheader header ={0};
	header.Cmd = 1;
	header.StartFlag = 0x5e7d;
	header.Length = inpack.GetOffset();
	inpack.pushPackageHeader(header);

	DWORD total = header.Length;
	int left = total;
	SOCKADDR_IN Addr = {0}; 
	int AddrLen = sizeof(SOCKADDR_IN);

	Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Addr.sin_port = htons(5969);
	Addr.sin_family = AF_INET;

	while (left)
	{
		int len = sendto(sock, (char*) &buf[total-left], left, 0,(PSOCKADDR)&Addr,AddrLen);
		if (len <=0)
		{
			return false;
		}
		left -= len;
	}
	return true;
}
void CSyncGameInfoView::OnForceupdate()
{
	if(MessageBox("确定要强制修复所选中的游戏吗?","提示:", MB_OKCANCEL) == IDCANCEL)
		return ;

	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	while (pos)
	{
		int nItem = GetListCtrl().GetNextSelectedItem(pos);
		CString strgid = GetListCtrl().GetItemText(nItem,0);
		ForceUpdate(atol(strgid));
	}
}

void CSyncGameInfoView::OnFindgame()
{
	CFindGameDlg dlg;
	dlg.DoModal();
}
LRESULT CSyncGameInfoView::SelectItem(WPARAM wparam,LPARAM lparam)
{
	GetListCtrl().SetItemState(wparam, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	GetListCtrl().EnsureVisible(wparam, TRUE);

	return S_OK;
}
void CSyncGameInfoView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

//bool g_sort[] = {false, false, false, false, false, false, false};
bool b_sort = false;
int CALLBACK ListHeader_Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSyncGameInfoView* pListView = (CSyncGameInfoView*) lParamSort;
	CString item1= pListView->GetListCtrl().GetItemText(lParam1, 2);
	CString item2 = pListView->GetListCtrl().GetItemText(lParam2, 2);
	if(b_sort)
		return item1.Compare(item2) > 0;
	else
		return item1.Compare(item2) < 0;
}

void CSyncGameInfoView::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	for (int idx=0; idx<this->GetListCtrl().GetItemCount(); idx++)
	{
		this->GetListCtrl().SetItemData(idx, idx);
	}
	this->GetListCtrl().SortItems(ListHeader_Compare, (DWORD)this);
	b_sort = !b_sort;
}