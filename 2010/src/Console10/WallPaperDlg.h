#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CWallPaperDlg : public CDialog
{
	DECLARE_DYNAMIC(CWallPaperDlg)

public:
	CWallPaperDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWallPaperDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_WALLPAPER };
protected:
	BOOL m_bModify;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	CListCtrl m_lstWallPaper;
	afx_msg void OnNMClickListWallpaper(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_cboCell;
	afx_msg void OnCbnKillfocusCombo1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOpenDir();
};
