#pragma once


// CSyncGameInfoView 视图

class CSyncGameInfoView : public CListView
{
	DECLARE_DYNCREATE(CSyncGameInfoView)

protected:
	CSyncGameInfoView();           // 动态创建所使用的受保护的构造函数
//	virtual ~CSyncGameInfoView();
	bool SetItemColor(int nItem, int nSubItem, COLORREF clrText, COLORREF clrBkgnd);
	bool ForceUpdate(DWORD gid);
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnForceupdate();
//	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnFindgame();
	afx_msg LRESULT SelectItem(WPARAM wparam,LPARAM lparam);
	virtual void OnInitialUpdate();
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);


};


