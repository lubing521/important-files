#pragma once

#include "resource.h"
#include "afxcmn.h"

// COptionPenetateFile 对话框

class COptionPenetateFile : public CDialog
{
	DECLARE_DYNAMIC(COptionPenetateFile)

public:
	COptionPenetateFile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionPenetateFile();

// 对话框数据
	enum { IDD = IDD_OPTION_PENETRATEFILE };
	BOOL m_bModify;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListdir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListdir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
private:
	CListCtrl m_list;
public:
	afx_msg void OnBnClickedMkidx();
};
