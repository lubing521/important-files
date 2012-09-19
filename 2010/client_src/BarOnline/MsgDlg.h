#pragma once
#include "resource.h"
#include "dbmgr.h"

class CMsgDlg : public CDialog
{
	friend class CInputMethodDlg;
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgDlg();

// 对话框数据
	enum { IDD = IDD_DLG_MESSAGE };

	CString m_strContent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedClose();
	DECLARE_MESSAGE_MAP()
private:
	CButtonEx m_btnTip;
	CButtonEx m_btnClose;
	CButtonEx m_btnOK;
	CButtonEx m_btnContent;
};

class CInputMethodDlg : public CMsgDlg
{
	DECLARE_DYNAMIC(CInputMethodDlg)

public:
	CInputMethodDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputMethodDlg();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();

private:
	tagGameInfo* m_pGameInfo;
	DWORD        m_hUpdate;
	
};