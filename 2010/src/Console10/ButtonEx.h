#pragma once
#include <afxbutton.h>
#include <functional>

// CButtonEx

class CPlugButtonEx : public CMFCButton
{
public:
	typedef std::tr1::function<void(UINT, int, int)> ClickCallback;

public:
	CPlugButtonEx(UINT id, int nItem, int nSubItem, const CRect &rect, const ClickCallback &callback);
	virtual ~CPlugButtonEx();

public:
	UINT uID_;		
	BOOL bEnable;
	int m_inItem;
	int m_inSubItem;
	CRect m_rect;
	ClickCallback clickCallback_;
	
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClicked();
	
};


