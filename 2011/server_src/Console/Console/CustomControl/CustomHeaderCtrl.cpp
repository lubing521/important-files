#include "stdafx.h"
#include "CustomHeaderCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl

CCustomHeaderCtrl::CCustomHeaderCtrl()
: imageBk_(new ATL::CImage)
, imageLine_(new ATL::CImage)
{}

CCustomHeaderCtrl::~CCustomHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()


void CCustomHeaderCtrl::SetImage(LPCTSTR bgImg, LPCTSTR lineImg)
{
	imageBk_->Load(bgImg);
	imageLine_->Load(lineImg);
}

void CCustomHeaderCtrl::SetImage(HBITMAP bgImg, HBITMAP lineImg)
{
	HBITMAP bgTmp = CopyBitmap(bgImg);
	HBITMAP lineTmp = CopyBitmap(lineImg);

	imageBk_->Attach(bgTmp);
	imageLine_->Attach(lineTmp);
}

/////////////////////////////////////////////////////////////////////////////
// CSkinHeaderCtrl message handlers

void CCustomHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

LRESULT CCustomHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 

	HDLAYOUT &hdl = *( HDLAYOUT * ) lParam; 

	RECT *prc = hdl.prc; 

	WINDOWPOS *pwpos = hdl.pwpos; 

	int nHeight = 30;

	pwpos->cy = nHeight; 

	prc->top = nHeight; 

	return TRUE;
}

void CCustomHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( imageBk_->IsNull() )
		return;

	CRect rect, rectItem;
	GetClientRect(&rect);
	CMemDC memDC(dc, rect);
	memDC.GetDC().FillSolidRect(&rect, RGB(255,255,255));

	imageBk_->Draw(memDC.GetDC().GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, imageBk_->GetWidth(), imageBk_->GetHeight());
	
	int nItems = GetItemCount();

	for(int i = 0; i <nItems; i++)
	{
		
		TCHAR buf1[256] = {0};
		HD_ITEM hditem = {0};
		
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem.pszText = buf1;
		hditem.cchTextMax = 255;
		GetItem(i, &hditem );
		
		GetItemRect(i, &rectItem);
		
		//make sure we draw the start piece
		//on the first item so it has a left border

		//For the following items we will just use the
		//right border of the previous items as the left
		//border
		
		imageLine_->Draw(memDC.GetDC().GetSafeHdc(), rectItem.left - 1, rectItem.top, 2, rectItem.Height(), 0, 0, imageLine_->GetWidth(), imageLine_->GetHeight());
		
		//span the bitmap for the width of the column header item
		int nWidth = rect.Width() - 4;
		
	

		//Get all the info for the current
		//item so we can draw the text to it
		//in the desired font and style
		DRAWITEMSTRUCT	DrawItemStruct = {0};
		
		DrawItemStruct.CtlType		= 100;
		DrawItemStruct.hDC			= dc.GetSafeHdc();
		DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
		DrawItemStruct.hwndItem 	= GetSafeHwnd(); 
		DrawItemStruct.rcItem		= rectItem;
		DrawItemStruct.itemID		= i;
		DrawItem(&DrawItemStruct);
		
		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP | DT_END_ELLIPSIS ;
		
		
		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 14;
		lf.lfWeight = 2;
		lstrcpy(lf.lfFaceName, _T("Microsoft Sans Serif"));
		font.CreateFontIndirect(&lf);
		CFont* def_font = memDC.GetDC().SelectObject(&font);
		
		memDC.GetDC().SetBkMode(TRANSPARENT);
		rectItem.DeflateRect(10, 10, 5, 5);
		
		TCHAR buf[256] = {0};
		HD_ITEM hditemText = {0};
		
		hditemText.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditemText.pszText = buf;
		hditemText.cchTextMax = 255;
		GetItem( DrawItemStruct.itemID, &hditemText );

		memDC.GetDC().DrawText(buf, &rectItem, uFormat);
		memDC.GetDC().SelectObject(def_font);
		font.DeleteObject();
	}		
}

BOOL CCustomHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	
}

void CCustomHeaderCtrl::OnDestroy()
{
	__super::OnDestroy();
}