// IdcUptToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CIdcUptToolDlg 对话框
class CIdcUptToolDlg : public CDialog
{
// 构造
public:
	CIdcUptToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAINFRAME };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	void  ScanDir(CString szDir, int nBaseLen);
	DWORD MakeTimeFromFileTime(FILETIME& ft);
	CString GetFileCrc32(const CString& szFileName);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl m_lstCtrl;
	afx_msg void OnBnClickedOpendir();
	afx_msg void OnBnClickedMakeidx();
	afx_msg void OnBnClickedDelfile();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedModifyfile();
};
