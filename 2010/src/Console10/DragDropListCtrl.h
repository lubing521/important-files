#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDragDropListCtrl window



class CDragDropListCtrl 
	: public CListCtrl
{
protected:
	CDWordArray			m_anDragIndexes;
	int					m_nDropIndex;
	CImageList*			m_pDragImage;
	int					m_nPrevDropIndex;
	UINT				m_uPrevDropState;
	DWORD				m_dwStyle;

	enum EScrollDirection
	{
		scrollNull,
		scrollUp,
		scrollDown
	};
	EScrollDirection	m_ScrollDirection;
	UINT				m_uScrollTimer;


	typedef std::tr1::function<bool(long , CDWordArray &)> CALLBACKFUNC; 

	CALLBACKFUNC m_callbackFunc;

// Construction
public:
	CDragDropListCtrl();
	bool IsDragging() const { return m_pDragImage != NULL; }

// Attributes
public:

// Operations
public:
	void RegisterCallBack(CALLBACKFUNC callbackFunc);

// Implementation
public:
	virtual ~CDragDropListCtrl();

protected:
	void DropItem();
	void RestorePrevDropItemState();
	void UpdateSelection(int nDropIndex);
	void SetScrollTimer(EScrollDirection ScrollDirection);
	void KillScrollTimer();
	CImageList* CreateDragImageEx(LPPOINT lpPoint);
	
// Generated message map functions
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};
