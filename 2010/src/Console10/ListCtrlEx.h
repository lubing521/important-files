#pragma once
#include <map>

#include <functional>

class CHeaderCtrlEx : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CHeaderCtrlEx)
public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

class CListCtrlEx : public CListCtrl
{
	typedef struct tagCellColor
	{
		COLORREF clrText;
		COLORREF clrBkText;
		COLORREF clrSelText;
		COLORREF clrSelBkText;
	}tagCellColor;
	DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

	void SetHyperLindColor(COLORREF clr)
	{
		m_clrHyperLink = clr;
	}
	//text:链接单元的文本串,如果为NULL则不改变此单元的文本
	template<typename Func> //std::tr1::function<void (void)>
	void SetCellHyperLink(int row, int col, LPCTSTR text, Func func)
	{
		if (text && GetItemText(row, col) != text)
			SetItemText(row, col, text);
		m_CellHyperlinks[row][col] = func;
	}
	void ClearCellHyperLink(void)
	{
		m_CellHyperlinks.clear();
	}
	void ClearCellHyperLink(int row, int col, bool bClearText = true)
	{
		m_CellHyperlinks[row].erase(col);
		if (bClearText)
			SetItemText(row, col, _T(""));
	}
public:
	void SetCellColor(int nRow, int nCol, COLORREF clrText, COLORREF clrBkCol, 
		COLORREF clrSelText, COLORREF clrSelBkCol);
	void GetCellColr(int nRow, int nCol, COLORREF& clrText, COLORREF& clrBkCol, 
		COLORREF& clrSelText, COLORREF& clrSelBkCol);
protected:
	afx_msg void OnCustomDrawList (NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_clrHyperLink;
	std::map<DWORD, tagCellColor*> m_CellColor;
	CHeaderCtrlEx m_headCtrl;
	CFont m_headFont;

	typedef std::tr1::function<void ()> HyperlinkHrefFunc;
	std::map<int, std::map<int, HyperlinkHrefFunc> > m_CellHyperlinks;
};