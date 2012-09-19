#include "stdafx.h"
#include "ServerUI.h"
#include "VDiskInfoView.h"

IMPLEMENT_DYNCREATE(CVDiskInfoView, CListView)

CVDiskInfoView::CVDiskInfoView()
{

}

CVDiskInfoView::~CVDiskInfoView()
{
}

BEGIN_MESSAGE_MAP(CVDiskInfoView, CListView)
	ON_WM_CREATE()
END_MESSAGE_MAP()

#ifdef _DEBUG
void CVDiskInfoView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CVDiskInfoView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

int CVDiskInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ImageLst.Create(16, 16, ILC_COLOR32|ILC_MASK, 1, 1);

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	m_ImageLst.Add(hIcon);
	DestroyIcon(hIcon);	

	CListCtrl& pListCtrl = this->GetListCtrl();
	pListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	pListCtrl.ModifyStyle(0,LVS_REPORT|LVS_SINGLESEL);
	pListCtrl.SetImageList(&m_ImageLst,LVSIL_NORMAL);
	//pListCtrl.InsertColumn(0,_T("服务器IP"),LVCFMT_LEFT,100);
	pListCtrl.InsertColumn(0,_T("服务器盘符"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(1,_T("客户机盘符"),LVCFMT_LEFT,80);
	pListCtrl.InsertColumn(2,_T("连接数"),LVCFMT_LEFT,60);
	pListCtrl.InsertColumn(3,_T("端口"),LVCFMT_LEFT,80);
	//pListCtrl.InsertColumn(5,_T("客户机回写目录"),LVCFMT_LEFT,120);
	pListCtrl.InsertColumn(4,_T("最后一次刷新时间"),LVCFMT_LEFT,120);
	return 0;
}
