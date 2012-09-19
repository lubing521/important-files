#pragma once


// CUpdateGameInfoView 视图

class CUpdateGameInfoView : public CListView
{
	DECLARE_DYNCREATE(CUpdateGameInfoView)

protected:
	CUpdateGameInfoView();           // 动态创建所使用的受保护的构造函数
	virtual ~CUpdateGameInfoView();

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


