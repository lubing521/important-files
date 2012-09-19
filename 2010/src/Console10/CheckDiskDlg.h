#pragma once

#include <vector>
// CCheckDiskDlg 对话框

class CCheckDiskDlg : public CDialog
{

private:
	CListBox m_lstUnProt;
	CListBox m_lstProt;	

	CString m_strCheckArea;					// 需要修复的分区
	CString m_strUnCheckArea;				// 待修复的分区

	DECLARE_DYNAMIC(CCheckDiskDlg)

public:
	CCheckDiskDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCheckDiskDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHECKDISK };


public:
	const CString &GetCheckArea() const		// 获取需要修复的分区
	{
		return m_strCheckArea;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	DECLARE_MESSAGE_MAP()

	
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedAddAll();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedRemoveAll();
	afx_msg void OnBnClickedOk();
};
