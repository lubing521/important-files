#pragma once
#include "afxcmn.h"

#include "makeidxfile.h"
#include "afxwin.h"
class CMakeIdxDlg : public CDialog
{
	DECLARE_DYNAMIC(CMakeIdxDlg)

public:
	CMakeIdxDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMakeIdxDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MAKEIDX };
	bool m_bIsBackgroudWork;
	bool m_bOnlyMakeIdx;
	i8desk::GameInfoVector m_GameInfos;
protected:
	bool   m_bFinish;
	i8desk::CMakeIdx MakeIdx;
	int	   m_nIndex;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstCtrl;
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_btnProgress;
	CEdit m_edtLog;
};