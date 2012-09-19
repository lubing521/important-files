#include "stdafx.h"
#include "CustomRadio.h"


#include "../../ImageHelpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomRadio::CCustomRadio()
: isHover_(false)
, isChecked_(false)
{

}

CCustomRadio::~CCustomRadio()
{

}

BEGIN_MESSAGE_MAP(CCustomRadio, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



void CCustomRadio::_Init()
{
	CWindow wnd(GetSafeHwnd());
	parent_ = wnd.GetTopLevelWindow();
	font_.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
}


void CCustomRadio::SetImages(HBITMAP normal, HBITMAP hover, HBITMAP down)
{
	assert(normal != 0);
	assert(hover != 0);
	assert(down != 0);

	bmps_[0].Attach(normal);
	bmps_[1].Attach(hover);
	bmps_[2].Attach(down);
}

void CCustomRadio::SetThemeParent(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !brBack_.IsNull() )
		brBack_.DeleteObject();
}

void CCustomRadio::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	_Init();
}

void CCustomRadio::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	WTL::CMemoryDC memDC(dc.GetSafeHdc(), rcClient);

	if( brBack_.IsNull() )
		brBack_ = ui::draw::AtlGetBackgroundBrush(m_hWnd, parent_);
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(RGB(0, 0, 0));

	if( !IsWindowEnabled() )
		memDC.SetTextColor(RGB(40, 40, 40));

	memDC.FillRect(rcClient, brBack_);

	CRect rcImg(0, 0, bmps_[0].GetWidth(), bmps_[0].GetHeight());
	CRect rcDest(CPoint(rcClient.left, rcClient.top), CSize(rcImg.Width(), rcImg.Height()));

	int checked = GetCheck();
	if( checked != BST_CHECKED && isHover_ )
		bmps_[1].Draw(memDC, rcDest, rcImg);
	else
	{
		if( checked == BST_CHECKED )
			checked = 2;
		else
			checked = 0;
		bmps_[checked].Draw(memDC, rcDest, rcImg);
	}

	CRect rcText(CPoint(rcDest.right + 3, rcDest.top), CSize(rcClient.Width() - rcDest.Width(), rcClient.Height()));
	CString text;
	GetWindowText(text);
	HFONT oldFont = memDC.SelectFont(font_);
	memDC.DrawText(text, text.GetLength(), rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	memDC.SelectFont(oldFont);
	
}

BOOL CCustomRadio::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}



int CCustomRadio::OnCreate(LPCREATESTRUCT)
{
	_Init();
	return TRUE;
}

void CCustomRadio::OnDestroy()
{
	bmps_[0].Detach();
	bmps_[1].Detach();
	bmps_[2].Detach();
}

void CCustomRadio::OnMove(int, int)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	Invalidate();
}

void CCustomRadio::OnSize(UINT, int, int)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	Invalidate();
}

void CCustomRadio::OnMouseMove(UINT, CPoint)
{
	if( !isHover_ ) 
	{
		TRACKMOUSEEVENT tme = { 0 };
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		_TrackMouseEvent(&tme);

		isHover_ = true;
		Invalidate();
	}

}

void CCustomRadio::OnMouseLeave()
{
	if( isHover_ ) 
	{
		isHover_ = false;
		Invalidate();
	}
}

void CCustomRadio::OnLButtonDown(UINT id, CPoint)
{
	isChecked_ = !isChecked_;
}