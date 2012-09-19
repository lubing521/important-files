#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDirFileCleanDlg 对话框

class CDirFileCleanDlg : public CDialog
{
	DECLARE_DYNAMIC(CDirFileCleanDlg)

public:
	CDirFileCleanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDirFileCleanDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_FILECLEAN };

protected:
	BOOL m_bModify;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstDir;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedAddDirFile();
	afx_msg void OnBnClickedDel();
	afx_msg void OnDestroy();
	CListCtrl m_lstDrv;
	afx_msg void OnNMClickListdrv(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_edtSize;
	afx_msg void OnEnKillfocusDrvSize();
	afx_msg void OnEnChangeExcludeDir();
};
