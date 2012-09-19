#pragma once


// COptExtraTool 对话框

class COptExtraTool : public CDialog
{
	DECLARE_DYNAMIC(COptExtraTool)

public:
	COptExtraTool(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptExtraTool();

// 对话框数据
	enum { IDD = IDD_OPTION_EXTRATOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL m_bAutoStartIeHistory;
	BOOL m_bUseUdisk;
	BOOL m_bModify;
	afx_msg void OnBnClickedCheckAutoStartIeHistory();
	afx_msg void OnBnClickedUdisk();

};
