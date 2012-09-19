#pragma once

#include "resource.h"

// CPenetateFile 对话框

class CPenetateFile : public CDialog
{
	DECLARE_DYNAMIC(CPenetateFile)

public:
	CPenetateFile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPenetateFile();

// 对话框数据
	enum { IDD = IDD_PENETATE_FILE };
	
	CString m_strSrcDir;
	CString m_strDestDir;
	BOOL m_bMakeIdx;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeSrcDir();

private:
	std::string m_strGameDriver;
public:
	afx_msg void OnEnChangeDestDir();
};
