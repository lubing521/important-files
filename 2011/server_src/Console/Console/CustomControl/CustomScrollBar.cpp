#include "stdafx.h"
#include "CustomScrollBar.h"

#include "../../../../include/UI/ImageHelpers.h"



CCustomVScrollbar::CCustomVScrollbar()
: parent_(0)
, bMouseDown_(false)
, bMouseDownArrowUp_(false)
, bMouseDownArrowDown_(false)
, bDragging_(false)
, nThumbTop_(0)
, dbThumbInterval_(0.0)
{

}


BEGIN_MESSAGE_MAP(CCustomVScrollbar, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CCustomVScrollbar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CCustomVScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CRect rcClient;
	GetClientRect(rcClient);

	CRect rectUpArrow(0, 0, img_[0].GetWidth(), img_[0].GetHeight());
	CRect rectDownArrow(CPoint(0, rcClient.bottom - img_[1].GetHeight()), CSize(img_[1].GetWidth(), img_[1].GetHeight()));
	CRect rectThumb(CPoint(0, nThumbTop_), CSize(img_[5].GetWidth(), nThumbTop_ + img_[5].GetHeight()));

	if( rectThumb.PtInRect(point) )
		bMouseDown_ = true;

	if( rectDownArrow.PtInRect(point) )
	{
		bMouseDownArrowDown_ = true;
		SetTimer(2, 250,NULL);
	}

	if( rectUpArrow.PtInRect(point) )
	{
		bMouseDownArrowUp_ = true;
		SetTimer(2, 250,NULL);
	}

	__super::OnLButtonDown(nFlags, point);
}

void CCustomVScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();

	bool bInChannel = true;

	CRect rcClient;
	GetClientRect(rcClient);

	CRect rectUpArrow(0, 0, img_[0].GetWidth(), img_[0].GetHeight());
	CRect rectDownArrow(CPoint(0, rcClient.bottom - img_[1].GetHeight()), CSize(img_[1].GetWidth(), img_[1].GetHeight()));
	CRect rectThumb(CPoint(0, nThumbTop_), CSize(img_[5].GetWidth(), nThumbTop_ + img_[5].GetHeight()));

	if( rectUpArrow.PtInRect(point) && bMouseDownArrowUp_ )
	{
		ScrollUp();	
		bInChannel = false;
	}

	if( rectDownArrow.PtInRect(point) && bMouseDownArrowDown_ )
	{
		ScrollDown();
		bInChannel = false;
	}

	if( rectThumb.PtInRect(point) )
	{
		bInChannel = false;
	}

	if( bInChannel == true )
	{
		if( point.y > nThumbTop_ )
		{
			PageDown();
		}
		else
		{
			PageUp();
		}
	}

	bMouseDown_ = false;
	bDragging_ = false;
	bMouseDownArrowUp_ = false;
	bMouseDownArrowDown_ = false;

	__super::OnLButtonUp(nFlags, point);
}

void CCustomVScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if( bMouseDown_ )
	{

		int nPreviousThumbTop = nThumbTop_;
		nThumbTop_ = point.y-13; //-13 so mouse is in middle of thumb

		double nMax = parent_->GetScrollLimit(SB_VERT);
		int nPos = parent_->GetScrollPos(SB_VERT);

		double nHeight = clientRect.Height()-98;
		double nVar = nMax;
		dbThumbInterval_ = nHeight/nVar;

		//figure out how many times to scroll total from top
		//then minus the current position from it

		int nScrollTimes = (int)((nThumbTop_-36)/dbThumbInterval_)-nPos;

		CSize size;
		size.cx = 0;
		size.cy = nScrollTimes*13; //13 is the height of each row at current font
		//I cant figure out how to grab this value
		//dynamically

		parent_->Scroll(size);


		_LimitThumbPosition();

		CClientDC dc(this);
		_Draw(dc);

	}
	__super::OnMouseMove(nFlags, point);
}

void CCustomVScrollbar::OnPaint() 
{
	CPaintDC dc(this); 

	_Draw(dc);
}

void CCustomVScrollbar::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		if( bMouseDownArrowDown_ )
			ScrollDown();

		if( bMouseDownArrowUp_ )
			ScrollUp();
	}
	else if( nIDEvent == 2 )
	{
		if( bMouseDownArrowDown_ )
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}

		if( bMouseDownArrowUp_ )
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
	}
	
	__super::OnTimer(nIDEvent);
}

BOOL CCustomVScrollbar::Create(CListCtrl *list, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd)
{
	parent_ = list;
	return __super::Create(NULL, dwStyle, rect, pParentWnd);
}

void CCustomVScrollbar::PageDown()
{
	parent_->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CCustomVScrollbar::PageUp()
{
	parent_->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CCustomVScrollbar::ScrollUp()
{
	parent_->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CCustomVScrollbar::ScrollDown()
{
	parent_->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CCustomVScrollbar::SetImages(HBITMAP img[6])
{
	for(size_t i = 0; i != _countof(img_); ++i)
	{
		assert(img[i] != 0);
		HBITMAP tmp = CopyBitmap(img[i]);

		img_[i].Attach(tmp);
	}

	nThumbTop_ = img_[0].GetHeight() + img_[2].GetHeight();
}

void CCustomVScrollbar::UpdateThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	double nPos = parent_->GetScrollPos(SB_VERT);
	double nMax = parent_->GetScrollLimit(SB_VERT);
	double nHeight = (clientRect.Height()-98);
	double nVar = nMax;

	dbThumbInterval_ = nHeight/nVar;

	double nNewdbValue = (dbThumbInterval_ * nPos);
	int nNewValue = (int)nNewdbValue;


	nThumbTop_ = img_[0].GetHeight() + img_[2].GetHeight() + nNewValue;

	_LimitThumbPosition();

	CClientDC dc(this);
	_Draw(dc);
}


void CCustomVScrollbar::_Draw(CDC &dc)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CMemDC memDC(dc, clientRect);
	memDC.GetDC().FillSolidRect(clientRect,  RGB(255, 255, 255));

	// Top
	CRect top(CPoint(clientRect.left, clientRect.top), CSize(img_[0].GetWidth(), img_[0].GetHeight()));
	img_[0].Draw(memDC.GetDC().GetSafeHdc(), top);
	
	// Bottom
	CRect bottom(CPoint(clientRect.left, clientRect.bottom - img_[1].GetHeight()), CSize(img_[1].GetWidth(), img_[1].GetHeight()));
	img_[1].Draw(memDC.GetDC().GetSafeHdc(), bottom);


	// Bar Top
	CRect barTop(CPoint(clientRect.left, clientRect.top + top.Height()), CSize(img_[2].GetWidth(), img_[2].GetHeight()));
	img_[2].Draw(memDC.GetDC().GetSafeHdc(), barTop);

	// Bar Bottom
	CRect barBottom(CPoint(clientRect.left, bottom.top - img_[4].GetHeight()), CSize(img_[4].GetWidth(), img_[4].GetHeight()));
	img_[4].Draw(memDC.GetDC().GetSafeHdc(), barBottom);


	// Bar BK
	for(int i = barTop.bottom; i <= barBottom.top; i += img_[3].GetHeight())
	{
		CRect barBK(CPoint(clientRect.left, i), CSize(img_[3].GetWidth(), img_[3].GetHeight()));

		img_[3].Draw(memDC.GetDC().GetSafeHdc(), barBK);
	}

	// Thumb
	CRect thumb(CPoint(clientRect.left + 1, clientRect.top + nThumbTop_), CSize(img_[5].GetWidth() - 1, img_[5].GetHeight()));
	img_[5].Draw(memDC.GetDC().GetSafeHdc(), thumb);

}

void CCustomVScrollbar::_LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(clientRect);

	if(nThumbTop_+26 > (clientRect.Height()- img_[0].GetHeight() + img_[2].GetHeight()))
	{
		nThumbTop_ = clientRect.Height()-62;
	}

	if(nThumbTop_ < (clientRect.top+img_[0].GetHeight() + img_[2].GetHeight()))
	{
		nThumbTop_ = clientRect.top+img_[0].GetHeight() + img_[2].GetHeight();
	}
}
