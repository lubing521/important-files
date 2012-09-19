#pragma once
#include "afxcmn.h"

#include "ShortcutsListCtrl.h"


// CDeskLnkDlg 对话框

class CDeskLnkDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeskLnkDlg)

public:
	CDeskLnkDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeskLnkDlg();

// 对话框数据
	enum { IDD = IDD_OPTION_DESKLNK };

private:
	BOOL m_bModify;
	CListCtrl m_lstDeskLnk;
	CShortcutsListCtrl m_lstDeskClassLnk;		// 类别快捷方式List Ctrl
	CImageList	m_images;						// 类别快捷方式Image

	std::vector<i8desk::std_string> m_vecClassShortcuts;	// 类别快捷方式集合

public:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	void _InitClassShortcuts();
	void _InitListCtrl();
	void _InsertBootTask();

public:
	DECLARE_MESSAGE_MAP()

	LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedDel();
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedListDesklnk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListDesklnkClass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListDesklnkClass(NMHDR *pNMHDR, LRESULT *pResult);
};
