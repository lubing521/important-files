#include "stdafx.h"
#include "CustomLabel.h"

#include "../../../include/ui/ImageHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCustomLabel::CCustomLabel()
{

}

CCustomLabel::~CCustomLabel()
{

}

BEGIN_MESSAGE_MAP(CCustomLabel, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()


void CCustomLabel::PreSubclassWindow()
{
	//ASSERT(m_hWnd == NULL);
	//ASSERT(::IsWindow(m_hWnd));

	CStatic::PreSubclassWindow();

	_Init();
}


void CCustomLabel::_Init()
{
	HWND wnd(GetSafeHwnd());
	parent_ = GetParent()->GetSafeHwnd();

	textStyle_	= DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK;
	textClr_	= RGB(0, 0, 0);
	bkClr_		= RGB(255, 255, 255);

	HGDIOBJ hFont = ::GetStockObject(DEFAULT_GUI_FONT); 
	font_.Attach(hFont);
}

void CCustomLabel::SetThemeParent(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !backBrush_.GetSafeHandle() ) 
		backBrush_.DeleteObject();
}

void CCustomLabel::SetTextColor(COLORREF clr)
{
	textClr_ = clr;
	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
}

void CCustomLabel::SetBkColor(COLORREF clr)
{

}

void CCustomLabel::SetTextStyle(UINT style)
{
	textStyle_ = style;
	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
}

int CCustomLabel::OnCreate(LPCREATESTRUCT)
{
	_Init();

	return 1;
}

LRESULT CCustomLabel::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	font_.Attach((HFONT)wParam);
	return FALSE;
}

LRESULT CCustomLabel::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT res = DefWindowProc(WM_SETTEXT, wParam, lParam);
	Invalidate();
	RedrawWindow();

	return res;
}

LRESULT CCustomLabel::OnPrintClient(WPARAM, LPARAM)
{
	OnPaint();

	return TRUE;
}

BOOL CCustomLabel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCustomLabel::OnMove(int x, int y)
{
	if( !backBrush_.GetSafeHandle() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomLabel::OnSize(UINT, int, int)
{
	if( !backBrush_.GetSafeHandle() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomLabel::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	CMemDC memDC(dc, rcClient);
	

	// Preserve background
	//if( backBrush_.GetSafeHandle() ) 
	{
		backBrush_.Attach(ui::draw::AtlGetBackgroundBrush(m_hWnd, parent_));
	}

	HGDIOBJ hOldBrush = memDC.GetDC().SelectObject(backBrush_.GetSafeHandle());
	memDC.GetDC().PatBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), PATCOPY);
	memDC.GetDC().SelectObject(hOldBrush);

	// Draw label text
	TCHAR szText[600] = { 0 };
	GetWindowText(szText, (sizeof(szText) / sizeof(TCHAR)) - 1);
	HGDIOBJ hOldFont = memDC.GetDC().SelectObject(font_);

	memDC.GetDC().SetBkMode(TRANSPARENT); 
	memDC.GetDC().SetTextColor(textClr_);
	memDC.GetDC().DrawText(szText, _tcslen(szText), &rcClient, textStyle_);
	memDC.GetDC().SelectObject(hOldFont);
}