#pragma once
#include "afxwin.h"


// CGongGaoDlg 对话框

class CGongGaoDlg : public CDialog
{
	DECLARE_DYNAMIC(CGongGaoDlg)

public:
	CGongGaoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGongGaoDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_GONGGAO };

protected:
	CString		m_szFontString;
	LOGFONT		m_lf;
	CFont		m_font;
	COLORREF	m_fontColor;

	BOOL m_bModify;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedFont1();
	afx_msg void OnBnClickedFont2();
	afx_msg void OnEnChangeEdit1();
	CButton m_btnShowGongGao;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
