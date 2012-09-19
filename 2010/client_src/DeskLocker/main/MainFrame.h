#pragma once

#include "../../BarOnline/UI.h"
#include "afxwin.h"

#define WM_AUTO_UNLOCK	WM_USER + 0x1000

class CMainFrame : public CDialog
{
public:
	CMainFrame(CWnd* pParent = NULL);

	enum { IDD = IDD_MAINFRAME };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;
	CEdit     m_edtPwd;
	CButtonEx m_btnUnLocker;
	CButtonEx m_btnPlus;
	CButtonEx m_btnDecrease;
	BYTE      m_nSreed;
	
	void SetWndParent(BYTE value);
	void PaintTime(CDC* pDC);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedPlus();
	afx_msg void OnBnClickedDecrease();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedUnlocker();
	afx_msg LRESULT OnAtuoUnlock(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPwd;
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
