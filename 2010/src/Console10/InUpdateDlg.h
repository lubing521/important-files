#pragma once
#include "afxwin.h"


// CInUpdateDlg 对话框

class CInUpdateDlg : public CDialog
{
	DECLARE_DYNAMIC(CInUpdateDlg)

public:
	CInUpdateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInUpdateDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_INUPDATE };

	BOOL m_bModify;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_cboDrv;
	CComboBox m_cboInProt;
	afx_msg void OnCbnSelchangeInprot();
	afx_msg void OnCbnSelchangeComboDrv();
};
