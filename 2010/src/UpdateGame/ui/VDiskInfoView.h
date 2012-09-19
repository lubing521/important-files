#pragma once


// CVDiskInfoView 视图

class CVDiskInfoView : public CListView
{
	DECLARE_DYNCREATE(CVDiskInfoView)

protected:
	CVDiskInfoView();           // 动态创建所使用的受保护的构造函数
	virtual ~CVDiskInfoView();

	CImageList m_ImageLst;
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
};


