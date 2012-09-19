#pragma once

#include <map>
#include <memory>

#include "GridListCtrlEx/CGridListCtrlEx.h"
#include "ButtonEx.h"


class CPlugListCtrlEx 
	: public CGridListCtrlEx
{
public:
	typedef std::map<UINT, std::tr1::shared_ptr<CPlugButtonEx>> Buttons;
	typedef std::map<UINT, std::tr1::shared_ptr<CProgressCtrl>> Progresses;
	typedef CPlugButtonEx::ClickCallback	ClickCallback;

public:
	CPlugListCtrlEx();
	virtual ~CPlugListCtrlEx();

public:
	void CreateCtrls(UINT lID, UINT rID, int nItem, int nSubItem, LPCTSTR lpszLeftText, LPCTSTR lpszText, const ClickCallback &callback);
	
	void EnableButton(UINT id, bool enable);
	void ShowButton(UINT id, bool enable);
	void SetButtonText(UINT id, LPCTSTR lpszText);
	stdex::tString GetButtonText(UINT id) const;

	void ShowProgress(int nItem, bool show);
	void SetProgress(int nItem, size_t progress);

private:
	void _CreateRButton(UINT uID, int nItem, int nSubItem, DWORD dwStyle, const CRect &rect, LPCTSTR lpszText, const ClickCallback &callback);
	void _CreateLButton(UINT uID, int nItem, int nSubItem, DWORD dwStyle, const CRect &rect, LPCTSTR lpszText, const ClickCallback &callback);
	void _CreateProgress(int nItem, int nSubItem, const CRect &rect);
	void _UpdateButtonPos();

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);


private:
	Buttons rButtons_;
	Buttons lButtons_;
	Progresses progresses_;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDrawItem(int, LPDRAWITEMSTRUCT);
	afx_msg void OnNcDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLvnEndScroll(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
};


