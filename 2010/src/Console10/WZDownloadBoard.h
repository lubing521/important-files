#pragma once

#include "resource.h"
#include "afxwin.h"
// CWZDownloadBoard 对话框

class CWZDownloadBoard : public CDialog
{
	DECLARE_DYNAMIC(CWZDownloadBoard)

public:
	CWZDownloadBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWZDownloadBoard();

// 对话框数据
	enum { IDD = IDD_WZ_DOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeTasknum();
	afx_msg void OnCbnSelchangeDownspeed();
	afx_msg void OnEnChangeInitdir();
	afx_msg void OnEnChangeDowntempdir();
	afx_msg void OnCbnSelchangeRuntype();
	afx_msg void OnBnClickedSelectInitdir();
	afx_msg void OnBnClickedSelectMsctmpdir();
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
private:
	CComboBox m_cbRuntype;
	CString m_strTaskNum;
	CString m_strDownSpeed;
	CString m_strInitDir;
	CString m_strTempDir;
public:
	afx_msg void OnCbnSelchangeTasknum();
};
