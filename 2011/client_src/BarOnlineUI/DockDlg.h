#pragma once
#include "afxwin.h"
#include "DockNavBar.h"

class CDockDlg : public CDialog
{
	DECLARE_DYNAMIC(CDockDlg)
public:
	CDockDlg(CWnd* pParent = NULL);
	virtual ~CDockDlg();

	enum { IDD = IDD_DLG_DOCK };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT  OnDisplayChange(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void         ResetDockPos();

private:
	CBitmap     *m_pbkImage;
	CDockNavBar  m_DockNav;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
