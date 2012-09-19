#pragma once


// CUiSplitterWnd

class CUiSplitterWnd : public CSplitterWnd
{
	DECLARE_DYNAMIC(CUiSplitterWnd)

public:
	CUiSplitterWnd();
	virtual ~CUiSplitterWnd();
	void ChangeView(CWnd* pView);
protected:
	DECLARE_MESSAGE_MAP()
};


