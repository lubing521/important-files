#include "stdafx.h"
#include "CGridRowTraitText.h"

#include "CGridRowTraitVisitor.h"
#include "CGridListCtrlEx.h"

CGridRowTraitText::CGridRowTraitText()
	:m_pOldFont(NULL)
	,m_TextColor((COLORREF)-1)
	,m_BackColor((COLORREF)-1)
	,m_AltTextColor((COLORREF)-1)
	,m_AltBackColor((COLORREF)-1)
{}

void CGridRowTraitText::SetRowColor(COLORREF textColor, COLORREF backColor)
{
	m_TextColor = textColor;
	m_BackColor = backColor;
}

void CGridRowTraitText::SetAltRowColor(COLORREF textColor, COLORREF backColor)
{
	m_AltTextColor = textColor;
	m_AltBackColor = backColor;
}

void CGridRowTraitText::Accept(CGridRowTraitVisitor& visitor)
{
	visitor.Visit(*this);
}

bool CGridRowTraitText::UpdateTextColor(int nRow, COLORREF& textColor)
{
	if (m_AltTextColor!=COLORREF(-1) && nRow % 2)
	{
		textColor = m_AltTextColor;
		return true;
	}

	if (m_TextColor!=COLORREF(-1))
	{
		textColor = m_TextColor;
		return true;
	}

	return false;
}

bool CGridRowTraitText::UpdateBackColor(int nRow, COLORREF& backColor)
{
	if (m_AltBackColor!=COLORREF(-1) && nRow % 2)
	{
		backColor = m_AltBackColor;
		return true;
	}

	if (m_BackColor!=COLORREF(-1))
	{
		backColor = m_BackColor;
		return true;
	}

	return false;
}

void CGridRowTraitText::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
	int nRow = (int)pLVCD->nmcd.dwItemSpec;

	switch (pLVCD->nmcd.dwDrawStage)
	{
		// Before painting a row
		case CDDS_ITEMPREPAINT:
		{
			if (!owner.IsRowSelected(nRow) && owner.GetHotItem()!=nRow)
			{
				// Only change row colors when not selected / in focus
				if (UpdateTextColor(nRow, pLVCD->clrText))
					*pResult |= CDRF_NEWFONT;

				if (UpdateBackColor(nRow, pLVCD->clrTextBk))
					*pResult |= CDRF_NEWFONT;

				if (owner.OnDisplayRowColor(nRow, pLVCD->clrText, pLVCD->clrTextBk))
					*pResult |= CDRF_NEWFONT;
			}

			LOGFONT newFont = {0};
			if (owner.OnDisplayRowFont(nRow, newFont))
			{
				// New font provided
				CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
				CFont* pNewFont = new CFont;
				VERIFY( pNewFont->CreateFontIndirect(&newFont) );
				m_pOldFont = pDC->SelectObject(pNewFont);
				*pResult |= CDRF_NOTIFYPOSTPAINT;	// We need to restore the original font
				*pResult |= CDRF_NEWFONT;
			}
			else
			{
				if (owner.GetFont()!=owner.GetCellFont())
				{
					// Using special cell font because of SetMargin()
					CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
					m_pOldFont = pDC->SelectObject(owner.GetCellFont());
					*pResult |= CDRF_NOTIFYPOSTPAINT;	// We need to restore the original font
					*pResult |= CDRF_NEWFONT;
				}
			}

			if (pLVCD->nmcd.uItemState & CDIS_FOCUS)
			{
				// If drawing focus row, then remove focus state and request to draw it later
				//	- Row paint request can come twice, with and without focus flag
				//	- Only respond to the one with focus flag, else DrawFocusRect XOR will cause solid or blank focus-rectangle
				if (owner.GetFocusRow()==nRow)
				{
					if (owner.GetFocusCell() >= 0)
					{
						// We want to draw a cell-focus-rectangle instead of row-focus-rectangle
						pLVCD->nmcd.uItemState &= ~CDIS_FOCUS;
						*pResult |= CDRF_NOTIFYPOSTPAINT;
					}
					else
					if (owner.GetExtendedStyle() & LVS_EX_GRIDLINES)
					{
						// Avoid bug where bottom of focus rectangle is missing when using grid-lines
						//	- Draw the focus-rectangle for the entire row (explicit)
						pLVCD->nmcd.uItemState &= ~CDIS_FOCUS;
						*pResult |= CDRF_NOTIFYPOSTPAINT;
					}
				}
			}
		} break;

		// After painting a row
		case CDDS_ITEMPOSTPAINT:
		{
			if (m_pOldFont!=NULL)
			{
				// Restore the original font
				CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
				CFont* pNewFont = pDC->SelectObject(m_pOldFont);
				if (pNewFont!=owner.GetCellFont())
				{
					delete pNewFont;
				}
			}

			if (owner.GetFocusRow()!=nRow)
				break;

			// Perform the drawing of the focus rectangle
			if (owner.GetFocusCell() >= 0)
			{
				// Draw the focus-rectangle for a single-cell
				CRect rcHighlight;
				CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

				VERIFY( owner.GetCellRect(nRow, owner.GetFocusCell(), LVIR_BOUNDS, rcHighlight) );

				// Adjust rectangle according to grid-lines
				if (owner.GetExtendedStyle() & LVS_EX_GRIDLINES)
				{
					int cxborder = ::GetSystemMetrics(SM_CXBORDER);
					rcHighlight.bottom -= cxborder;
					// First column doesn't have a left-grid border
					if (owner.GetFirstVisibleColumn()!=owner.GetFocusCell())
						rcHighlight.left += cxborder;
				}

				pDC->DrawFocusRect(rcHighlight);
			}
			else
			if (owner.GetExtendedStyle() & LVS_EX_GRIDLINES)
			{
				// Avoid bug where bottom of focus rectangle is missing when using grid-lines
				//	- Draw the focus-rectangle for the entire row (explicit)
				CRect rcHighlight;
				CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
				// Using LVIR_BOUNDS to get the entire row-rectangle
				VERIFY( owner.GetItemRect(nRow, rcHighlight, LVIR_BOUNDS) );
				
				// Adjust rectangle according to grid-lines
				int cxborder = ::GetSystemMetrics(SM_CXBORDER);
				rcHighlight.bottom -= cxborder;
				pDC->DrawFocusRect(rcHighlight);
			}
		} break;
	}
}