// BackupDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "XUnzip.h"
#include "XZip.h"

// CBackupDlg 对话框
class CBackupDlg : public CDialog
{
// 构造
public:
	CBackupDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BACKUP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CListCtrl m_lstBackup;
	CDateTimeCtrl m_dtBackup;
	CButton m_btnByDay;

	CString GetBackupOption(CString szKey);
	void    SetBackupOption(CString szKey, CString szValue);
	void	RefreshBackListFile();
	bool	SaveData();

	HANDLE  m_hThread;
	HANDLE  m_hExited;
	BOOL    m_bIsBackup;
	static UINT __stdcall WorkThreadProc(LPVOID lpVoid);

	bool StopI8DeskService();
	bool StartI8DeskService();

	CString m_strBackDir;
	CString m_strBackFile;
	bool _ExecBackup();
	bool _ExecRestore();
	bool _ZipAllData(HZIP hZip, LPCSTR szDir, int nBase);
	CString GetDataDir();

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedCheckByday();
	afx_msg void OnClose();
	afx_msg void OnBnClickedExecBackup();
	afx_msg void OnBnClickedExecRestore();
	afx_msg void OnBnClickedDeleteBackup();
	afx_msg void OnEnChangeBackupDir();
	afx_msg void OnBnClickedSaveSet();
	afx_msg LRESULT OnProgress(WPARAM wParam, LPARAM lParam);
	CProgressCtrl m_btnProgress;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
