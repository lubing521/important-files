#pragma once
#include "afxwin.h"


// CFindGameDlg 对话框

class CFindGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindGameDlg)

public:
	CFindGameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindGameDlg();
	int m_nItem;
bool FindGame(CString& str,int nItem);
// 对话框数据
	enum { IDD = ID_FINDGAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFindgame();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedSelectall();
	CButton m_Radio;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CButton m_Radio2;
	BOOL m_bRaido1;
	BOOL m_bRaido2;
	char m_InputChar;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CEdit m_Edit;
	afx_msg void OnEnSetfocusEdit1();
};
