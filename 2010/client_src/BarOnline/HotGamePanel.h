#pragma once
#include "afxwin.h"


// CHotGamePanel 对话框

class CHotGamePanel : public CDialog
{
	DECLARE_DYNAMIC(CHotGamePanel)

public:
	CHotGamePanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHotGamePanel();

// 对话框数据
	enum { IDD = IDD_DLG_HOTGAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnNMRClickListGame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMis);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDis);

	afx_msg void OnListPopRunGame();
	afx_msg void OnListPopOpenDir();
	afx_msg void OnListPopSearchGame();
	afx_msg void OnListPopAddGameFav();
	afx_msg void OnListPopSaveProgress();
	afx_msg void OnListPopLoadProgress();

	DECLARE_MESSAGE_MAP()
public:
	void  Refresh();
	void  RefreshWebpage();
	void  DeleteAllGame()
	{
		m_lstGame.DeleteAllItems();
	}
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDockList(NMHDR* pNMHDR, LRESULT* pResult);

	std::vector<tagGameInfo*> m_GameInfo;
	CImageList		m_ilLstCtrl;

	CButtonEx m_btnTopLine;	
	CButtonEx m_btnTitle1;
	CButtonEx m_btnComment1;
	CButtonEx m_btnGame1;
	CButtonEx m_btnNumer1;
	CButtonEx m_btnGame2;

	CDockHtmlCtrl m_WebPage;
	CListCtrl     m_lstGame;
	//CBitmap*	  m_pbmpLstBk;
	//CBitmap       m_LstBmp;
};
