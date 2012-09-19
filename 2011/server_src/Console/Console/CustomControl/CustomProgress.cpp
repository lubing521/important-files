#include "stdafx.h"
#include "CustomProgress.h"

#include "../../../../include/ui/ImageHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCustomProgress::CCustomProgress()
: pos_(0)
, range_(0, 0)
{

}

CCustomProgress::~CCustomProgress()
{

}

BEGIN_MESSAGE_MAP(CCustomProgress, CStatic)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()


void CCustomProgress::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	_Init();
}


void CCustomProgress::_Init()
{
	CWindow wnd(GetSafeHwnd());
	parent_ = wnd.GetTopLevelWindow();
}

void CCustomProgress::SetImages(HBITMAP gray[3], HBITMAP normal[3])
{
	for(size_t i = 0; i != 3; ++i)
	{
		assert(normal[i] != 0);
		normal_[i].reset(new ATL::CImage);
		normal_[i]->Attach(CopyBitmap(normal[i]));
	}

	for(size_t i = 0; i != 3; ++i)
	{
		assert(gray[i] != 0);
		gray_[i].reset(new ATL::CImage);
		gray_[i]->Attach(CopyBitmap(gray[i]));
	}
}

void CCustomProgress::SetThemeParent(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !bkBrush_.IsNull() ) 
		bkBrush_.DeleteObject();
}

void CCustomProgress::SetRange(size_t start, size_t stop)
{
	assert(start <= stop);
	range_ = std::make_pair(start, stop);
}

void CCustomProgress::SetPos(size_t pos)
{
	assert(pos <= range_.second);

	pos_ = pos;
	Invalidate();
}


int CCustomProgress::OnCreate(LPCREATESTRUCT)
{
	_Init();

	return 1;
}

LRESULT CCustomProgress::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

LRESULT CCustomProgress::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT res = DefWindowProc(WM_SETTEXT, wParam, lParam);
	Invalidate();
	RedrawWindow();

	return res;
}

LRESULT CCustomProgress::OnPrintClient(WPARAM, LPARAM)
{
	OnPaint();

	return TRUE;
}

BOOL CCustomProgress::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCustomProgress::OnMove(int x, int y)
{
	if( !bkBrush_.IsNull() ) 
		bkBrush_.DeleteObject();

	Invalidate();
}

void CCustomProgress::OnSize(UINT, int, int)
{
	if( !bkBrush_.IsNull() ) 
		bkBrush_.DeleteObject();

	Invalidate();
}

void CCustomProgress::OnPaint()
{
	WTL::CPaintDC dc(GetSafeHwnd());
	CRect rcClient;
	GetClientRect(rcClient);
	WTL::CMemoryDC memDC(dc, rcClient);


	// Preserve background
	if( bkBrush_.IsNull() ) 
		bkBrush_ = AtlGetBackgroundBrush(m_hWnd, parent_);

	HBRUSH hOldBrush = memDC.SelectBrush(bkBrush_);
	memDC.PatBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), PATCOPY);
	memDC.SelectBrush(hOldBrush);

	// Draw Progress BK

	// Left
	CRect rcLeftBK(CPoint(rcClient.left, rcClient.top), CSize(gray_[0]->GetWidth(), gray_[0]->GetHeight()));
	gray_[0]->Draw(memDC, rcLeftBK);
	
	// Right
	CRect rcRightBK(CPoint(rcClient.right - gray_[2]->GetWidth(), rcClient.top), CSize(gray_[2]->GetWidth(), gray_[2]->GetHeight()));
	gray_[2]->Draw(memDC, rcRightBK);

	// Middle
	CRect rcMidBK(CPoint(rcLeftBK.right, rcClient.top), CSize(rcRightBK.left - rcLeftBK.right, gray_[1]->GetHeight()));
	gray_[1]->Draw(memDC, rcMidBK);

	
	// Draw Progress
	if( pos_ == 0 )
		return;

	CRect rcLeft(CPoint(rcClient.left, rcClient.top), CSize(normal_[0]->GetWidth(), normal_[0]->GetHeight()));
	normal_[0]->Draw(memDC, rcLeft);

	CRect rcMid(CPoint(rcLeft.right, rcLeft.top), CSize(pos_ * (rcRightBK.left - rcLeftBK.right) / (range_.second - range_.first), normal_[1]->GetHeight()));
	if( rcMid.Width() != 0 )
		normal_[1]->Draw(memDC, rcMid);
	
	CRect rcRight(CPoint(rcMid.right, rcClient.top), CSize(normal_[2]->GetWidth(), normal_[2]->GetHeight()));
	normal_[2]->Draw(memDC, rcRight);

}