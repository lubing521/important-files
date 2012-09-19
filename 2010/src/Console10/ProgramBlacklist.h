#pragma once

#include "resource.h"

// CProgramBlacklist 对话框

class CProgramBlacklist : public CDialog
{
	DECLARE_DYNAMIC(CProgramBlacklist)

public:
	CProgramBlacklist(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgramBlacklist();

// 对话框数据
	enum { IDD = IDD_PROGRAM_BLACKLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strProgram;
	CString m_strDescription;
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedOk();
};
