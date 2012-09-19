// ListCtrlEx.cpp : 实现文件
//

#include "stdafx.h"
#include "ListCtrlEx.h"

using namespace std;

IMPLEMENT_DYNAMIC(CHeaderCtrlEx, CHeaderCtrl)

CHeaderCtrlEx::CHeaderCtrlEx()
{

}

CHeaderCtrlEx::~CHeaderCtrlEx()
{

}

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CHeaderCtrlEx::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(1);
	CRect rect;

	GetClientRect(&rect);
	dc.FillRect(&rect, &CBrush(RGB(255, 255, 255)));

	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	CFont font;
	font.CreateFontIndirect(&lf);
	dc.SelectObject(&font);
	for(int i = 0; i<GetItemCount(); i++)
	{
		TCHAR buf[256];
		HD_ITEM hditem;

		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER| DT_END_ELLIPSIS ;
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER | HDI_FORMAT;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( i, &hditem );
		GetItemRect(i, &rect);
		rect.left += 3;
		if (hditem.fmt & HDF_LEFT)
			uFormat |= DT_LEFT;
		else if (hditem.fmt & HDF_CENTER)
			uFormat |= DT_CENTER;
		else if (hditem.fmt & HDF_RIGHT)
			uFormat |= DT_RIGHT;		
	
		dc.DrawText(buf, &rect, uFormat);
	}
}

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
	m_headFont.CreateFontIndirect(&lf);
	m_clrHyperLink = RGB(0, 128, 0);
}

CListCtrlEx::~CListCtrlEx()
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.begin();
	for (; it != m_CellColor.end(); it++)
	{
		delete it->second;
	}
	m_CellColor.clear();
	if (m_headFont.m_hObject != NULL)
	{
		m_headFont.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
    ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDrawList)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CListCtrlEx::SetCellColor(int nRow, int nCol, COLORREF clrText, COLORREF clrBkCol, 
	COLORREF clrSelText, COLORREF clrSelBkCol)
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.find(MAKELONG(nRow, nCol));
	tagCellColor* pCell = NULL;
	if (it == m_CellColor.end())
	{
		pCell = new tagCellColor;
		m_CellColor.insert(make_pair(MAKELONG(nRow, nCol), pCell));
	}
	else
	{
		pCell = it->second;
	}
	pCell->clrText = clrText;
	pCell->clrBkText = clrBkCol;
	pCell->clrSelText = clrSelText;
	pCell->clrSelBkText = clrSelBkCol;
}

void CListCtrlEx::GetCellColr(int nRow, int nCol, COLORREF& clrText, COLORREF& clrBkCol, 
	COLORREF& clrSelText, COLORREF& clrSelBkCol)
{
	std::map<DWORD, tagCellColor*>::iterator it = m_CellColor.find(MAKELONG(nRow, nCol));
	
	if (it == m_CellColor.end())
	{
		//return default color;
		clrText = GetTextColor();
		clrBkCol = GetBkColor();
		clrSelText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		clrSelBkCol = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		tagCellColor* pCell = it->second;
		clrText = pCell->clrText;
		clrBkCol = pCell->clrBkText;
		clrSelText = pCell->clrSelText;
		clrSelBkCol = pCell->clrSelBkText;
	}
}

void CListCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	LVHITTESTINFO hti;
	hti.pt = point;
	hti.flags = LVHT_ONITEM;
	
	SubItemHitTest(&hti);

	if (m_CellHyperlinks.find(hti.iItem) != m_CellHyperlinks.end()
		&& m_CellHyperlinks[hti.iItem].find(hti.iSubItem) != m_CellHyperlinks[hti.iItem].end())
	{
		HCURSOR hCur = LoadCursor(NULL, IDC_HAND); 
        SetCursor(hCur);  
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CListCtrlEx::OnCustomDrawList ( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW*   pLVCD   =   reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);   
	*pResult = CDRF_DODEFAULT;   
	int rowIndex = static_cast<int>(pLVCD->nmcd.dwItemSpec);
	if(pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)   
	{
		if (m_headCtrl.GetSafeHwnd() == NULL && GetHeaderCtrl() != NULL && GetHeaderCtrl()->m_hWnd != NULL)
		{
			m_headCtrl.SubclassWindow(GetHeaderCtrl()->m_hWnd);
			//m_headCtrl.ModifyStyle(0, HDF_OWNERDRAW);
			m_headCtrl.SetFont(&m_headFont, TRUE);
		}
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{     
		*pResult = CDRF_NOTIFYSUBITEMDRAW;   
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))   
	{
		CDC* pDC	 = CDC::FromHandle(pLVCD->nmcd.hdc);
		int nItem	 = static_cast<int>(pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;
		CString str  = GetItemText(nItem, nSubItem);
		COLORREF clrText = 0, clrBkText = 0, clrSelText = 0, clrSelBkText = 0;
		GetCellColr(nItem, nSubItem, clrText, clrBkText, clrSelText, clrSelBkText);

		pDC->SetBkColor(1);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
		rect.DeflateRect(0, 0, 0, 1);

		//绘制图标
		if (nSubItem == 0)
		{
			if (CImageList *pImageList = GetImageList(LVSIL_SMALL))
			{
				LVITEM lvItem = {0};
				lvItem.iItem = nItem;
				lvItem.mask = LVIF_IMAGE;
				if (GetItem(&lvItem))
				{
					//图标绘制位置
					CPoint pt = rect.TopLeft();
					//文本位置调整
					rect.DeflateRect(rect.Height() + 4, 0, 0, 0); 
					//绘制图标
					pImageList->Draw(pDC, lvItem.iImage, pt, ILD_NORMAL);
				}
			}
		}

		//绘制文本
		if(0)//LVIS_SELECTED == GetItemState(rowIndex, LVIS_SELECTED))
		{
			pDC->SetTextColor(clrSelText);
			
			pDC->FillSolidRect(&rect, clrSelBkText);
		}
		else
		{
			pDC->SetTextColor(clrText);
			pDC->FillSolidRect(&rect, clrBkText);
		}
		
		if (nSubItem > 0)
			rect.left += 3;
	
		LV_COLUMN lv = {0};
		lv.mask = LVCF_FMT;
		
		BOOL b = GetColumn(nSubItem, &lv);

		DWORD nStyle = DT_SINGLELINE;
		if (lv.fmt & LVCFMT_LEFT)
			nStyle |= DT_LEFT;
		else if (lv.fmt & LVCFMT_CENTER)
			nStyle |= DT_CENTER;
		else if (lv.fmt & LVCFMT_RIGHT)
			nStyle |= DT_RIGHT;
		else 
			nStyle = DT_LEFT;

		if (m_CellHyperlinks.find(nItem) != m_CellHyperlinks.end()
			&& m_CellHyperlinks[nItem].find(nSubItem) != m_CellHyperlinks[nItem].end())
		{
			CFont *font = this->GetFont();
			LOGFONT lf;
			font->GetLogFont(&lf);
			lf.lfUnderline = TRUE;

			CFont f;
			f.CreateFontIndirect(&lf);
			CFont *oldf = pDC->SelectObject(&f);

			pDC->SetTextColor(m_clrHyperLink);
			pDC->DrawText(str, &rect, nStyle);

			pDC->SelectObject(oldf);
		}
		else 
		{
			pDC->DrawText(str, &rect, nStyle);
		}

		*pResult = CDRF_SKIPDEFAULT;
	}
}

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListCtrl::OnLButtonDown(nFlags, point);

	LVHITTESTINFO hti = {0};
	hti.pt = point;
	hti.flags = LVHT_ONITEM;
	
	SubItemHitTest(&hti);

	if (m_CellHyperlinks.find(hti.iItem) != m_CellHyperlinks.end()
		&& m_CellHyperlinks[hti.iItem].find(hti.iSubItem) != m_CellHyperlinks[hti.iItem].end())
	{
		m_CellHyperlinks[hti.iItem][hti.iSubItem]();
	}
}
