#pragma once
#include "afxcmn.h"

class CFavPanel : public CDialog
{
	DECLARE_DYNAMIC(CFavPanel)
public:
	CFavPanel(CWnd* pParent = NULL);
	virtual ~CFavPanel();
	enum { IDD = IDD_DLG_FAVORITE };
public:
	void Refresh();
protected:
	void FreeData(HTREEITEM hItem);
	void GetAllFavUrl(LPCTSTR szDir, HTREEITEM hParentItem = NULL);
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnManage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
private:
	I8SkinCtrl_ns::CI8OldButton m_btnFav;
	I8SkinCtrl_ns::CI8OldButton m_btnLine;
	I8SkinCtrl_ns::CI8OldButton m_btnClose;
	I8SkinCtrl_ns::CI8OldButton m_btnManage;
	CTreeCtrl m_treCtrl;
	CImageList m_itreCtrl;
	CFont     m_treFont;
	CBitmap   *m_pImgTab;
    HCURSOR   m_hCursor;
public:
	afx_msg void OnPaint();
//	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
