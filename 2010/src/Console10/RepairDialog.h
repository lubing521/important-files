#pragma once
#include "explorer.h"

#include "UI.h"

// CRepairDialog 对话框

class CRepairDialog : public CDialog
{
	DECLARE_DYNAMIC(CRepairDialog)

public:
	CRepairDialog(const CString& nid, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRepairDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_REPAIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CString m_strNid;
	CDockHtmlCtrl m_WebPage;
};
