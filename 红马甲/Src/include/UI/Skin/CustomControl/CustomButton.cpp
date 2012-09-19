#include "stdafx.h"
#include "CustomButton.h"

#include "../../wtl/atlgdi.h"
#include "../../ImageHelpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CCustomButton, CButton)

CCustomButton::CCustomButton()
: normal_(new ATL::CImage)
, hover_(new ATL::CImage)
, press_(new ATL::CImage)
, disable_(new ATL::CImage)
, isHover_(false)
{}

CCustomButton::~CCustomButton()
{

}

BEGIN_MESSAGE_MAP(CCustomButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CCustomButton::SetThememParent(HWND wnd)
{
	ATLASSERT(::IsWindow(wnd));

	parent_ = wnd;
	if( !brBack_.IsNull() ) 
		brBack_.DeleteObject();
}

void CCustomButton::SetTextClr(COLORREF normal, COLORREF clrHover, COLORREF clrPress, COLORREF clrDisable)
{
	clrText_[0] = normal;
	clrText_[1] = clrHover;
	clrText_[2] = clrPress;
	clrText_[3] = clrDisable;

	if( IsWindow(GetSafeHwnd()) )
		Invalidate();
}

void CCustomButton::SetImages(HBITMAP normal, HBITMAP hover, HBITMAP press, HBITMAP disable)
{
	if( !normal_->IsNull() )
		normal_->Detach();
	if( !hover_->IsNull() )
		hover_->Detach();
	if( !press_->IsNull() )
		press_->Detach();

	normal_->Attach(ui::draw::CopyBitmap(normal));
	hover_->Attach(ui::draw::CopyBitmap(hover));
	press_->Attach(ui::draw::CopyBitmap(press));
	if( disable == 0 )
	{
		if( !disable_->IsNull() )
			disable_->Detach();
		disable_->Attach(ui::draw::CopyBitmap(normal));
	}
	else
	{
		if( !disable_->IsNull() )
			disable_->Detach();
		disable_->Attach(ui::draw::CopyBitmap(disable));
	}

	if( IsWindow(GetSafeHwnd()) )
		Invalidate();
}

void CCustomButton::_Init()
{
	CWindow self(GetSafeHwnd());
	parent_ = self.GetTopLevelWindow();

	clrText_[0] = RGB(0, 0, 0);
	clrText_[1] = RGB(0, 0, 0);
	clrText_[2] = RGB(0, 0, 0);
	clrText_[3] = RGB(128, 128, 128);
	
	textStyle_ = DT_SINGLELINE | DT_VCENTER | DT_CENTER;

	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	if( font_.m_hObject != 0 )
		font_.DeleteObject();

	font_.CreateFontIndirect(&lf);
}


void CCustomButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	_Init();
}


void CCustomButton::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	LPNMCUSTOMDRAW *custom = reinterpret_cast<LPNMCUSTOMDRAW *>(lpDis);

	WTL::CDCHandle tmpDC = lpDis->hDC;
	CRect rcClient(lpDis->rcItem);

	WTL::CMemoryDC dc(tmpDC, rcClient);

	bool isHot = isHover_;//((lpDis->itemState & CDIS_HOT) != 0);
	bool isDisable = ((lpDis->itemState & CDIS_DISABLED) != 0);
	bool isPress = ((lpDis->itemState & CDIS_SELECTED) != 0);

	// Parent Bk
	if( brBack_.IsNull() )
		brBack_ = ui::draw::AtlGetBackgroundBrush(GetSafeHwnd(), parent_);
	HBRUSH hOldBrush = dc.SelectBrush(brBack_);
	dc.PatBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), PATCOPY);
	dc.SelectBrush(hOldBrush);

	// Button
	ATL::CImage *curImg = 0;
	
	if( isDisable )
		curImg = disable_.get();
	else if( isPress )
		curImg = press_.get();
	else if( isHot )
		curImg = hover_.get();
	else
		curImg = normal_.get();
	CRect rcImg(0, 0, curImg->GetWidth(), curImg->GetHeight());
	curImg->Draw(dc, rcClient, rcImg);

	// Image Right
	CRect rcClientRight(rcClient.right - 2, rcClient.top, rcClient.right, rcClient.bottom);
	CRect rcImgRight(curImg->GetWidth() - 2, 0, curImg->GetWidth(), curImg->GetHeight());
	curImg->Draw(dc, rcClientRight, rcImgRight);

	// Text
	CString text;
	GetWindowText(text);

	COLORREF clrText = clrText_[0];
	if( isHot ) 
		clrText = clrText_[1];
	if( isPress ) 
		clrText = clrText_[2];
	if( isDisable )
		clrText = clrText_[3];

	HFONT hOldFont = dc.SelectFont(font_);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrText);
	dc.DrawText(text, text.GetLength(), &rcClient, textStyle_);
	dc.SelectFont(hOldFont);
}


int CCustomButton::OnCreate(LPCREATESTRUCT lp)
{
	_Init();
	return __super::OnCreate(lp);
}

BOOL CCustomButton::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CCustomButton::OnSize(UINT flag, int x, int y)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	return __super::OnSize(flag, x, y);
}

void CCustomButton::OnMove(int x, int y)
{
	if( !brBack_.IsNull() )
		brBack_.DeleteObject();

	return __super::OnMove(x, y);
}


void CCustomButton::OnMouseMove(UINT, CPoint)
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

void CCustomButton::OnMouseLeave()
{
	if( isHover_ ) 
	{
		isHover_ = false;
		Invalidate();
	}
}