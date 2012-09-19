#include "stdafx.h"
#include "CustomComboBox.h"

#include "../../../../include/UI/ImageHelpers.h"
#include "../../../../include/ui/wtl/atlwinmisc.h"


CCustomComboBox::CCustomComboBox(const CSize &szArrow)
: isComboHover_(false)
, isArrowHover_(false)
, isArrowDown_(false)
, szArrow_(szArrow)
{
}

CCustomComboBox::~CCustomComboBox()
{

}


BEGIN_MESSAGE_MAP(CCustomComboBox, CComboBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, OnCbnSelchange)
END_MESSAGE_MAP()

void CCustomComboBox::SetBk(HBITMAP bk, HBITMAP editBmp)
{
	assert(bk != 0);
	assert(editBmp != 0);


    editBmp_ = editBmp;
    bk_.Attach(CopyBitmap(bk));
}

void CCustomComboBox::SetArrow(HBITMAP normalArrow, HBITMAP hoverArrow, HBITMAP downArrow)
{
	assert(normalArrow != 0);
	assert(hoverArrow != 0);
	assert(downArrow != 0);

	bmpArrow_[0].Attach(CopyBitmap(normalArrow));
	bmpArrow_[1].Attach(CopyBitmap(hoverArrow));
	bmpArrow_[2].Attach(CopyBitmap(downArrow));
}


void CCustomComboBox::_Init()
{
	ModifyStyle(0, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS);


	WTL::CClientRect rcClient(this->GetSafeHwnd());
    SetItemHeight(-1, szArrow_.cy - 6);
    font_.CreateFont(szArrow_.cy - 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));

	CWnd *wnd = GetWindow(GW_CHILD);

    if (wnd != NULL)
    {
        wnd->ModifyStyle(WS_BORDER, ES_MULTILINE | ES_AUTOHSCROLL);
        edit_.SubclassWindow(wnd->GetSafeHwnd());
        edit_.SetBkImg(editBmp_);
        edit_.SetImages(editBmp_, editBmp_, editBmp_);

        CRect rcEdit;
        rcEdit.left = 2;
        rcEdit.top	= 6;
        rcEdit.right= rcClient.Width() - szArrow_.cx - 4;
        rcEdit.bottom = rcClient.bottom - 2;

        wnd->MoveWindow(rcEdit);

    }

}

void CCustomComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	_Init();
}

void CCustomComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    assert(lpDrawItemStruct->CtlType == ODT_COMBOBOX);

    CDC *dc = CDC::FromHandle(lpDrawItemStruct->hDC);

    if ((GetStyle() & 0x0000003) == CBS_DROPDOWNLIST)
    {
        if( lpDrawItemStruct->itemAction & ODA_FOCUS )
        {
            dc->SetTextColor(RGB(255, 0, 0));
        }
    }
    else
    {
        if( (lpDrawItemStruct->itemAction & ODA_SELECT ) &&
            (lpDrawItemStruct->itemState & ODS_SELECTED) )
        {
            dc->SetTextColor(RGB(255, 0, 0));

        }
    }

    CString text;

	int index = lpDrawItemStruct->itemID;
	if( index != - 1 )
		GetLBText(index, text);

    CRect rcText(lpDrawItemStruct->rcItem);
    rcText.DeflateRect(5, 0, 0, 0);
    dc->DrawText(text, rcText, DT_SINGLELINE | DT_VCENTER);

}

void CCustomComboBox::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    WTL::CClientRect rcClient(*this);

    WTL::CClientRect rcCombo(rcClient);
    rcCombo.left	= rcClient.Width() - szArrow_.cx;
    rcCombo.top	= rcClient.Height() - szArrow_.cy; 

    CMemDC memDC(dc, rcClient);
	
    // Draw Bk
    CRect rcBK(0, 0, bk_.GetWidth(), bk_.GetHeight());
    bk_.Draw(memDC.GetDC().GetSafeHdc(), rcClient);

	CRect rcBKLeft(0, 0, 2, bk_.GetHeight());
	CRect rcLeft(0, 0, 2, rcClient.Height());
	bk_.Draw(memDC.GetDC().GetSafeHdc(), rcLeft, rcBKLeft);

	CRect rcBKRight(bk_.GetWidth() - 2, 0, bk_.GetWidth(), bk_.GetHeight());
	CRect rcRight(rcClient.Width() - 2, 0, rcClient.Width(), rcClient.Height());
	bk_.Draw(memDC.GetDC().GetSafeHdc(), rcRight, rcBKRight);
	
    // Draw Combo
    CRect rcImg(0, 0, bmpArrow_[0].GetWidth(), bmpArrow_[0].GetHeight());
    if( !isArrowDown_ && ! isArrowHover_ )
    {
	    bmpArrow_[0].Draw(memDC.GetDC().GetSafeHdc(), rcCombo, rcImg);
    }
    else if( isArrowHover_ && !isArrowDown_ )
    {
	    bmpArrow_[1].Draw(memDC.GetDC().GetSafeHdc(), rcCombo, rcImg);
    }
    else
    {
	    bmpArrow_[2].Draw(memDC.GetDC().GetSafeHdc(), rcCombo, rcImg);
    }


    if((GetStyle() & 0x0000003) == CBS_DROPDOWNLIST)
    {
        memDC.GetDC().SelectObject(GetFont());
        CString sText;
        __super::GetWindowText(sText);
        memDC.GetDC().SetBkMode(TRANSPARENT);

        COMBOBOXINFO   comboBoxInfo;
        comboBoxInfo.cbSize = sizeof(comboBoxInfo);
        CComboBox::GetComboBoxInfo(&comboBoxInfo);

        CRect rcText(rcClient);
        rcText.DeflateRect(5,3);
        rcText.right -= szArrow_.cx;
        memDC.GetDC().DrawText(sText, rcText, DT_SINGLELINE | DT_VCENTER);
    }
}

void CCustomComboBox::OnSetFocus(CWnd* pNewWnd)
{
    if((GetStyle() & 0x0000003) == CBS_DROPDOWNLIST)
    {
        SetRedraw(FALSE);
        __super::OnSetFocus(pNewWnd);
        SetRedraw(TRUE);
        Invalidate(FALSE);
    }
    else __super::OnKillFocus(pNewWnd);
}

void CCustomComboBox::OnKillFocus(CWnd* pNewWnd)
{
    if((GetStyle() & 0x0000003) == CBS_DROPDOWNLIST)
    {
        SetRedraw(FALSE);
        __super::OnKillFocus(pNewWnd);
        SetRedraw(TRUE);
        InvalidateRect(FALSE);
    }
    else __super::OnKillFocus(pNewWnd);
}


BOOL CCustomComboBox::OnCbnSelchange()
{
    if((GetStyle() & 0x0000003) == CBS_DROPDOWNLIST)
    {
        Invalidate(FALSE);
    }

    return FALSE;
}

LRESULT CCustomComboBox::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case CB_SETCURSEL:
    case CB_RESETCONTENT:
        SetRedraw(FALSE);
        LRESULT lResult = __super::DefWindowProc(message, wParam, lParam);
        SetRedraw(TRUE);
        Invalidate(FALSE);
        return lResult;
    }
    return __super::DefWindowProc(message, wParam, lParam);
}

BOOL CCustomComboBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

HBRUSH CCustomComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH brush = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if( nCtlColor == CTLCOLOR_EDIT )
	{
		if( !::IsWindow(edit_.GetSafeHwnd()) )
		{
			/*pWnd->ModifyStyle(WS_BORDER, ES_MULTILINE | ES_AUTOHSCROLL);
			edit_.SubclassWindow(pWnd->GetSafeHwnd());

			edit_.SetBkImg(editBmp_);
			edit_.SetImages(editBmp_, editBmp_, editBmp_);*/
		}
	}

	return brush;
}

void CCustomComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT ET;
	ET.cbSize		= sizeof(TRACKMOUSEEVENT);   
	ET.dwFlags		= TME_LEAVE;   
	ET.dwHoverTime	= NULL;   
	ET.hwndTrack	= m_hWnd;  
	_TrackMouseEvent(&ET);

	WTL::CClientRect rcArrow(GetSafeHwnd());
	rcArrow.left = rcArrow.Width() - szArrow_.cx;
	isComboHover_ = true;

	if( rcArrow.PtInRect(point) )
		isArrowHover_ = true;
	
	__super::OnMouseMove(nFlags, point);
}

void CCustomComboBox::OnMouseLeave()
{
	isComboHover_ = false;
	isArrowHover_ = false;

	Invalidate();
	__super::OnMouseLeave();
}

void CCustomComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	isArrowDown_ = true;
	Invalidate();
	__super::OnLButtonDown(nFlags, point);
}

void CCustomComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	isArrowDown_ = false;
	Invalidate();
	__super::OnLButtonUp(nFlags, point);
}