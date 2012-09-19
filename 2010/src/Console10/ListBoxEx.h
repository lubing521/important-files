#ifndef _listboxexe_inc_
#define _listboxexe_inc_

#include <vector>

class CListBoxEx : public CListBox
{
	// Construction
public:
	CListBoxEx();
public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
	int CompareItem(LPCOMPAREITEMSTRUCT) { return 0; }
public:
	void AddItem(UINT nIconID, LPCTSTR lpszText);
	void SetItemHeight (int nHeight);
	virtual ~CListBoxEx();
protected:

	CBitmap m_bmListBoxExDef;
	CBitmap m_bmListBoxExSel;
	int m_nItemHeight;

	COLORREF m_crBkColor;
	COLORREF m_crTextColor;

	typedef struct tagItem
	{
		DWORD IconID;
		CString szText;
	}tagItem;

	std::vector<tagItem> m_lstItemData;
	//{{AFX_MSG(CListBoxEx)
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif