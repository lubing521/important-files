#include "stdafx.h"
#include "I8HtmlView.h"

using namespace I8SkinCtrl_ns;


IMPLEMENT_DYNAMIC(CI8HtmlView, CHtmlView)

BEGIN_MESSAGE_MAP(CI8HtmlView, CHtmlView)
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

CI8HtmlView::CI8HtmlView()
{
	m_bScroll = TRUE;
}

CI8HtmlView::~CI8HtmlView()
{

}

void CI8HtmlView::SetNoScroll()
{
	m_bScroll = FALSE;
}

void CI8HtmlView::OnTitleChange(LPCTSTR lpszText)
{
	// 	OutputDebugString(lpszText);
	// 	OutputDebugString(TEXT("\r\n"));
}

HRESULT CI8HtmlView::OnGetHostInfo(DOCHOSTUIINFO *info)
{
	//控件IE控件的边框，滚动条有无，以及扁平滚动条
	info->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_FLAT_SCROLLBAR;
	if (!m_bScroll)
	{
		info->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
	}
	return S_OK;
}

void CI8HtmlView::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage(WM_HTTP_ERROR_MESSAGE);
	}
	*pbCancel = TRUE;
}

HRESULT CI8HtmlView::OnShowContextMenu(DWORD dwID, LPPOINT ppt,
										 LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
{
	return S_OK;
}

int  CI8HtmlView::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CI8HtmlView::OnDestroy()
{
	CWnd::OnDestroy();
}
