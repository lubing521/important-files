#pragma once



#include "./GridListCtrlEx/CGridListCtrlEx.h"
#include "./GridListCtrlEx/CGridColumnTraitText.h"


class CShortcutsListCtrl
	: public CGridListCtrlEx
{
public:
	CShortcutsListCtrl(void);
	~CShortcutsListCtrl(void);


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};
