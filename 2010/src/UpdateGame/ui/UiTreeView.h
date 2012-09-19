#pragma once


// CUiTreeView 视图

class CUiTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CUiTreeView)

protected:
	CUiTreeView();           // 动态创建所使用的受保护的构造函数
	virtual ~CUiTreeView();
	CImageList m_ImagList;
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
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	void SeleteItem(LPCSTR ItemName);
};


