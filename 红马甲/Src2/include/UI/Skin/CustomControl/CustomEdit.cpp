#include "stdafx.h"
#include "CustomEdit.h"

#include "../../ImageHelpers.h"


CCustomEdit::CCustomEdit()
{

}

CCustomEdit::~CCustomEdit()
{

}

BEGIN_MESSAGE_MAP(CCustomEdit, CEdit)
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT_EX(EN_CHANGE, &CCustomEdit::OnChange)
END_MESSAGE_MAP()


void CCustomEdit::SetBkImg(HBITMAP hBitmap)
{
	assert(hBitmap != 0);
	CBitmap *bmp = CBitmap::FromHandle(hBitmap);
	brush_.CreatePatternBrush(bmp);

	bkImage_.reset(new ATL::CImage);
	bkImage_->Attach(ui::draw::CopyBitmap(hBitmap));
	Invalidate();
}

void  CCustomEdit::SetImages(HBITMAP left, HBITMAP mid, HBITMAP right)
{
	assert(left != 0);
	assert(mid != 0);
	assert(right != 0);

	HBITMAP leftTmp		= ui::draw::CopyBitmap(left);
	HBITMAP midTmp		= ui::draw::CopyBitmap(mid);
	HBITMAP rightTmp	= ui::draw::CopyBitmap(right);

	bmps_[0].Attach(leftTmp);
	bmps_[1].Attach(midTmp);
	bmps_[2].Attach(rightTmp);
}

void CCustomEdit::PreSubclassWindow()
{
	ModifyStyle(WS_BORDER, ES_MULTILINE | ES_AUTOHSCROLL);

	CRect rc ;  
	GetClientRect(&rc) ;  
	CDC* pDC = GetDC();  
	TEXTMETRIC tm;  
	pDC->GetTextMetrics(&tm);  
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;  
	int nMargin = (rc.Height() - nFontHeight) / 2;  

	rc.DeflateRect(0, nMargin + 2, 5, 0);  
	SetRectNP(&rc);  
	ReleaseDC(pDC); 

	SetMargins(8, 5);
}


void CCustomEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnChar(nChar, nRepCnt, nFlags); 
}

void CCustomEdit::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpncsp)
{
	/*int height = lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top;  
	int margin = 0;  

	CDC* pDC = GetDC();  
	TEXTMETRIC tm;  
	pDC->GetTextMetrics(&tm);  
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;  
	int nMargin = (height - nFontHeight) / 2;  

	lpncsp->rgrc[0].left += 8;  
	lpncsp->rgrc[0].top += nMargin;  
	lpncsp->rgrc[0].right -= 8;  
	lpncsp->rgrc[0].bottom -= (nMargin-1);  */

	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp); 
}


BOOL CCustomEdit::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	
	CRect rcImg(0, 0, bkImage_->GetWidth(), bkImage_->GetHeight());
	WTL::CMemoryDC memDC(pDC->GetSafeHdc(), rcClient);
	bkImage_->Draw(memDC, rcClient, rcImg);

	if( bmps_[0].IsNull() )
		return FALSE;

	// Left
	CRect rcLeft(CPoint(rcClient.left, rcClient.top), CSize(bmps_[0].GetWidth(), rcClient.Height()));
	bmps_[0].Draw(memDC, rcLeft);
	
	
	// Right
	CRect rcRight(CPoint(rcClient.right - bmps_[2].GetWidth(), rcClient.top), CSize(bmps_[2].GetWidth(), rcClient.Height()));
	bmps_[2].Draw(memDC, rcRight);
	

	// Mid
	//CRect rcMid(CPoint(rcLeft.right, rcLeft.top), CSize(rcRight.left - rcLeft.right, rcClient.Height()));
	//bmps_[1].Draw(memDC, rcMid);


	return TRUE;//CEdit::OnEraseBkgnd(pDC);
}


HBRUSH CCustomEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT); 

	return brush_;
}

BOOL CCustomEdit::OnChange()
{
	Invalidate();
	
	return FALSE;
}


void CCustomEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Invalidate();
	return CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}