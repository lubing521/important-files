#include "stdafx.h"
#include "ServerUI.h"
#include "UiSplitterWnd.h"


// CUiSplitterWnd

IMPLEMENT_DYNAMIC(CUiSplitterWnd, CSplitterWnd)

CUiSplitterWnd::CUiSplitterWnd()
{

}

CUiSplitterWnd::~CUiSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CUiSplitterWnd, CSplitterWnd)
END_MESSAGE_MAP()


void CUiSplitterWnd::ChangeView(CWnd* pView)
{
	CWnd * pOldView = GetPane(0, 0);
	pOldView->SetDlgCtrlID(1000);   // 1000是一个任意的id，只要大于256即可          
	pOldView->ShowWindow(SW_HIDE);

	pView->SetDlgCtrlID(IdFromRowCol(0, 0));
	pView->SetParent(this);
	pView->ShowWindow(SW_SHOW);
	pView->SetActiveWindow();
	RecalcLayout();
}