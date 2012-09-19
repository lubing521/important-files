#include "stdafx.h"
#include "ServerUI.h"
#include "LogView.h"

IMPLEMENT_DYNCREATE(CLogView, CListView)

CLogView::CLogView()
{

}

CLogView::~CLogView()
{
}

BEGIN_MESSAGE_MAP(CLogView, CListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, &CLogView::OnNMRClick)
	ON_COMMAND(ID_CLEARLOG, &CLogView::OnClearlog)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

int CLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CListCtrl& pListCtrl = this->GetListCtrl();
	pListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	pListCtrl.ModifyStyle(0,LVS_REPORT|LVS_SINGLESEL);
	pListCtrl.InsertColumn(0,_T("时间"),LVCFMT_LEFT,200);
	pListCtrl.InsertColumn(1,_T("日志内容"),LVCFMT_LEFT,480);
	return 0;
}

void CLogView::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	POINT pt;
	GetCursorPos(&pt);
	menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN ,pt.x,pt.y,this);
	*pResult = 0;
}

void CLogView::OnClearlog()
{
	this->GetListCtrl().DeleteAllItems();
}
