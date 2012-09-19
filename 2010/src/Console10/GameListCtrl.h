#pragma once


// CGameListCtrl

class CGameListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGameListCtrl)

public:
	CGameListCtrl();
	virtual ~CGameListCtrl();

	CImageList *m_pDragImage;
	HCURSOR m_hNoDropCursor;
	HCURSOR m_hOkDropCursor;
protected:
	//{{AFX_MSG(CGridListCtrlEx)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnBegindragList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


