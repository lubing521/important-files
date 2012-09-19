#pragma once

#include "resource.h"
#include "afxcmn.h"

// CSCProgramBlacklistDlg 对话框

class CSCProgramBlacklistDlg : public CDialog
{
	DECLARE_DYNAMIC(CSCProgramBlacklistDlg)

public:
	CSCProgramBlacklistDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSCProgramBlacklistDlg();

// 对话框数据
	enum { IDD = IDD_SC_PROGRAM_BLACKLIST };
	void InsertItem(LPCTSTR lpAddTime, LPCTSTR lpName, LPCTSTR lpDesc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_list;
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult);
};
