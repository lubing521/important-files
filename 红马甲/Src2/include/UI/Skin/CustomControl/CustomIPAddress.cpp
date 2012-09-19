#include "stdafx.h"
#include "CustomIPAddress.h"

#include "../../../../include/ui/ImageHelpers.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomIPAddress::CCustomIPAddress()
{

}

CCustomIPAddress::~CCustomIPAddress()
{

}

BEGIN_MESSAGE_MAP(CCustomIPAddress, CIPAddressCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	//ON_WM_NCPAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
    ON_WM_ENABLE()
END_MESSAGE_MAP()


void CCustomIPAddress::PreSubclassWindow()
{
	CIPAddressCtrl::PreSubclassWindow();

	_Init();
}


void CCustomIPAddress::_Init()
{
	ModifyStyle(WS_BORDER, 0);

	CWindow wnd(GetSafeHwnd());
	parent_ = wnd.GetTopLevelWindow();
}

void CCustomIPAddress::SetThemeParent(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	parent_ = hWnd;

	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();
}

void CCustomIPAddress::SetImage(HBITMAP hBitmap)
{
	HBITMAP tmp = CopyBitmap(hBitmap);
	if( !image_.IsNull() )
	{
		::DeleteObject(image_.Detach());
	}
	image_.Attach(tmp);

	if( ::IsWindow(GetSafeHwnd()) )
		Invalidate();
}


void CCustomIPAddress::UpdateBk()
{
	if( !backBrush_.IsNull() )
		backBrush_.DeleteObject();

	Invalidate();
}

int CCustomIPAddress::OnCreate(LPCREATESTRUCT)
{
	_Init();

	return 1;
}


LRESULT CCustomIPAddress::OnPrintClient(WPARAM, LPARAM)
{
	OnPaint();

	return TRUE;
}

BOOL CCustomIPAddress::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCustomIPAddress::OnMove(int x, int y)
{
	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomIPAddress::OnSize(UINT, int, int)
{
	if( !backBrush_.IsNull() ) 
		backBrush_.DeleteObject();

	Invalidate();
}

void CCustomIPAddress::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect(rcClient);

	CMemDC memDC(dc, rcClient);
	
	// Preserve background
	if( backBrush_.IsNull() ) 
		backBrush_ = AtlGetBackgroundBrush(m_hWnd, parent_);

	HBRUSH hOldBrush = (HBRUSH)memDC.GetDC().SelectObject(backBrush_);
	memDC.GetDC().PatBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), PATCOPY);
	memDC.GetDC().SelectObject(hOldBrush);

	// Draw Text
	CRect rcTmp = rcClient;
	for(int i = 1; i != 4; ++i)
	{
		int nWidth = (rcClient.Width() / 2) * i;
		rcTmp.right = rcTmp.left + nWidth;
		CString str = _T(".");
		memDC.GetDC().DrawText(str, rcTmp, DT_CENTER | DT_SINGLELINE);
	}
}

void CCustomIPAddress::OnNcPaint()
{
    if (IsWindowEnabled())
    {
	    WTL::CWindowDC dc(GetSafeHwnd());
	    CRect rcWindow;
	    GetWindowRect(rcWindow);

	    rcWindow.right = rcWindow.Width();
	    rcWindow.bottom = rcWindow.Height();
	    rcWindow.left = rcWindow.top = 0;

	    image_.Draw(dc, rcWindow);
    }
    else
    {
        __super::OnNcPaint();
    }
}

LRESULT CCustomIPAddress::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_PAINT )
	{
		OnPaint();
		return 0;
	}
	else if( message == WM_NCPAINT )
	{
		OnNcPaint();
		return 0;
	}

	return CIPAddressCtrl::WindowProc(message, wParam, lParam);
}

void CCustomIPAddress::OnEnable(BOOL bEnable)
{
    __super::OnEnable(bEnable);
    ShowWindow(SW_HIDE);
    ShowWindow(SW_SHOW);
}
