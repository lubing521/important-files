#include "stdafx.h"
#include "CustomImage.h"

#include "../../../../include/ui/ImageHelpers.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomImage::CCustomImage()
{

}

CCustomImage::~CCustomImage()
{

}

BEGIN_MESSAGE_MAP(CCustomImage, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()


void CCustomImage::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	_Init();
}


void CCustomImage::_Init()
{
	CWindow wnd(GetSafeHwnd());
	parent_ = wnd.GetTopLevelWindow();
}

void CCustomImage::SetThemeParent(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();
}

void CCustomImage::SetImage(HBITMAP hBitmap, bool IsSizetoFit)
{
	HBITMAP tmp = CopyBitmap(hBitmap);
	if( !image_.IsNull() )
	{
		::DeleteObject(image_.Detach());
	}
	image_.Attach(tmp);

	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
	
	if ( IsSizetoFit )
		SizeToFit();
}

void CCustomImage::SetImage(HICON hIcon, bool IsSizetoFit/* = true*/)
{
	icon_ = hIcon;

	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();

	if ( IsSizetoFit )
		SizeToFit();
}

void CCustomImage::SizeToFit()
{
	size_t width = 0, height = 0;
	if( image_.IsNull() )
		width = height = 32;
	else
	{
		width	= image_.GetWidth();
		height	= image_.GetHeight();
	}

	
	CWindow wnd(GetSafeHwnd());
	wnd.ResizeClient(width, height);

	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
}

void CCustomImage::UpdateBk()
{
	if( !backBrush_.IsNull() )
		backBrush_.DeleteObject();

	Invalidate();
}

int CCustomImage::OnCreate(LPCREATESTRUCT)
{
	_Init();

	return 1;
}


LRESULT CCustomImage::OnPrintClient(WPARAM, LPARAM)
{
	OnPaint();

	return TRUE;
}

BOOL CCustomImage::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCustomImage::OnMove(int x, int y)
{
	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomImage::OnSize(UINT, int, int)
{
	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomImage::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);

	CMemDC memDC(dc, rcClient);
	
	// Preserve background
	if( backBrush_.IsNull() ) 
		backBrush_ = AtlGetBackgroundBrush(m_hWnd, parent_);

	HBRUSH hOldBrush = (HBRUSH)memDC.GetDC().SelectObject(backBrush_);
	memDC.GetDC().PatBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), PATCOPY);
	memDC.GetDC().SelectObject(hOldBrush);

	// Paint Image
	if( !image_.IsNull() )
		image_.Draw(memDC.GetDC(), rcClient);
	else if( !icon_.IsNull() )
		icon_.DrawIconEx(memDC.GetDC(), rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height());
	//else
		//assert(0);
}