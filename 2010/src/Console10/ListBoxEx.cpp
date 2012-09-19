#include "stdafx.h"
#include "ListBoxEx.h"
#include "resource.h"

CListBoxEx::CListBoxEx()
{
	VERIFY(m_bmListBoxExDef.LoadBitmap(IDB_LISTBOXEXDEF));
	VERIFY(m_bmListBoxExSel.LoadBitmap(IDB_LISTBOXEXSEL));
	
	BITMAP   bm;   
	m_bmListBoxExSel.GetBitmap(&bm);   
	m_nItemHeight = bm.bmHeight;
}
CListBoxEx::~CListBoxEx()
{

}

BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_nItemHeight;
}

void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDS)
{
	CDC dc;
	dc.Attach(lpDS->hDC);
	dc.SetBkMode(1);
	CRect rcText(lpDS->rcItem);

	if (((lpDS->itemAction | ODA_SELECT) && (lpDS->itemState & ODS_SELECTED)) ||
		((lpDS->itemAction | ODA_FOCUS) && (lpDS->itemState & ODS_FOCUS)) )
	{
		dc.FillRect(&rcText, &CBrush(&m_bmListBoxExSel));
	}
	else
	{
		dc.FillRect(&rcText, &CBrush(&m_bmListBoxExDef));
	}

	rcText.DeflateRect(2, 2, 2, 2);
	rcText.left += 36;
	dc.DrawText(m_lstItemData[lpDS->itemID].szText, -1, &rcText, DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	dc.Detach();
}

void CListBoxEx::AddItem(UINT nIconID, LPCTSTR lpszText)
{
	tagItem Item;
	Item.IconID = nIconID;
	Item.szText = lpszText;
	m_lstItemData.push_back(Item);
	AddString("");
}