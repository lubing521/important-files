#include "stdafx.h"
#include "CustomCheckBox.h"

#include "../../ImageHelpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomCheckBox::CCustomCheckBox()
: state_(Normal)
{

}

CCustomCheckBox::~CCustomCheckBox()
{

}

BEGIN_MESSAGE_MAP(CCustomCheckBox, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CCustomCheckBox::_Init()
{
	parent_ = GetTopLevelParent()->GetSafeHwnd();
	font_.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
}

void CCustomCheckBox::SetImages(HBITMAP normal, HBITMAP hover, HBITMAP down)
{
	assert(normal != 0);
	assert(hover != 0);
	assert(down != 0);

	bmps_[0].Attach(ui::draw::CopyBitmap(normal));
	bmps_[1].Attach(ui::draw::CopyBitmap(hover));
	bmps_[2].Attach(ui::draw::CopyBitmap(down));
}


void CCustomCheckBox::SetThemeParent(HWND hWnd)
{
	assert(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !brBack_.IsNull() )
		brBack_.DeleteObject();
}

void CCustomCheckBox::PreSubclassWindow()
{
	_Init();
}

void CCustomCheckBox::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	CMemDC memDC(dc, rcClient);

	if( brBack_.IsNull() )
		brBack_ = ui::draw::AtlGetBackgroundBrush(m_hWnd, parent_);
	memDC.GetDC().SetBkMode(TRANSPARENT);
	memDC.GetDC().SetTextColor(RGB(0, 0, 0));

	if( !IsWindowEnabled() )
		memDC.GetDC().SetTextColor(RGB(40, 40, 40));

	memDC.GetDC().FillRect(rcClient, CBrush::FromHandle(brBack_));

	CRect rcImg(0, 0, bmps_[0].GetWidth(), bmps_[0].GetHeight());
	CRect rcDest(CPoint(rcClient.left, rcClient.top), CSize(rcImg.Width(), rcImg.Height()));

	int checked = GetCheck();
	bmps_[checked + 1].Draw(memDC.GetDC().GetSafeHdc(), rcDest, rcImg);

	CRect rcText(CPoint(rcDest.right + 3, rcDest.top), CSize(rcClient.Width() - rcDest.Width(), rcClient.Height()));
	CString text;
	GetWindowText(text);
	HGDIOBJ oldFont = memDC.GetDC().SelectObject(font_);
	memDC.GetDC().DrawText(text, rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	memDC.GetDC().SelectObject(oldFont);
}

BOOL CCustomCheckBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


int CCustomCheckBox::OnCreate(LPCREATESTRUCT)
{
	_Init();
	return TRUE;
}

void CCustomCheckBox::OnMove(int, int)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	Invalidate();
}

void CCustomCheckBox::OnSize(UINT, int, int)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	Invalidate();
}


void CCustomCheckBox::OnMouseHover(UINT nFlags, CPoint point)
{
	if( state_ == Down )
	{
		__super::OnMouseHover(nFlags, point);
	}
	else
	{
		state_ = Hover;
		Invalidate();
	}
}

void CCustomCheckBox::OnMouseLeave()
{
	if( state_ == Down )
	{
		__super::OnMouseLeave();
	}
	else
	{
		state_ = Normal;
		Invalidate();
	}
}

void CCustomCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( state_ == Down )
	{
		state_ = Normal;
	}
	else
	{
		state_ = Down;
	}

	Invalidate();
	__super::OnLButtonDown(nFlags, point);
}