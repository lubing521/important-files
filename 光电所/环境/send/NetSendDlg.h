// NetSendDlg.h : header file
//

#if !defined(AFX_NETSENDDLG_H__ECE85C39_1305_4B57_AE0D_F03B27326737__INCLUDED_)
#define AFX_NETSENDDLG_H__ECE85C39_1305_4B57_AE0D_F03B27326737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNetSendDlg dialog

class CNetSendDlg : public CDialog
{
// Construction
public:
	
//	char * fileName;
	CNetSendDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNetSendDlg)
	enum { IDD = IDD_NETSEND_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetSendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	HANDLE   hFind1;
    int notFirstFile;
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNetSendDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSend();
	afx_msg void OnBtnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
DWORD FindFile(char * dataPath);
void SendData(char * dataPath);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETSENDDLG_H__ECE85C39_1305_4B57_AE0D_F03B27326737__INCLUDED_)
