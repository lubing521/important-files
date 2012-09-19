
// UDiskToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CUDiskToolDlg 对话框
class CUDiskToolDlg : public CDialog
{
// 构造
public:
	CUDiskToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UDISKTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	bool Open(LPCTSTR lpszConnString, LPCTSTR lpszUser, LPCTSTR lpszPassword);
	 bool ExecSql(LPCTSTR lpszSql, _RecordsetPtr& prcd);
	 static UINT _stdcall WorkThread(LPVOID lparam);
// 实现
	  bool ZipFile(LPCSTR szSrc,LPCSTR szDest,LPCSTR zipName = NULL);
	 bool ScanFile(LPCSTR szfile,int nDir,vector<CString>& vecLst);
	 bool ScanDirtory(LPCSTR szDir,int nDir,vector<CString>&veclst);
	void ConvertFileToDst( const _variant_t& name, const _variant_t& password, const _variant_t& logTime);
	void ConvertGameToDst( const _variant_t& name, const _variant_t& password, const _variant_t& logTime);
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnProgress(WPARAM wparam,LPARAM lparam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
private:
	_ConnectionPtr m_pConn;
	long m_hErrCode ;
	char m_szErrInfo[MAX_PATH] ;
	HANDLE m_hread,m_hExit;
public:
	CString m_szSorcePath,m_szDestPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CProgressCtrl m_Progress;
	afx_msg void OnClose();
};
