#pragma once


// CBrowseDirDlg 对话框

class CBrowseDirDlg : public CDialog
{
	DECLARE_DYNAMIC(CBrowseDirDlg)

public:
	CBrowseDirDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBrowseDirDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_BROWSEDIR };

	BOOL m_bBrowseDir;
	CString m_strDirOrFile;
	CTreeCtrl m_lstDir;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	std::string GetSelDir(HTREEITEM hSel = NULL);
	bool SelectTreeNode(LPCTSTR lpszText, HTREEITEM hItem);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();	
	afx_msg void OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
};
