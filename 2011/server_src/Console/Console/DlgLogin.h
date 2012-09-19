#pragma once

#include <functional>

#include "CustomControl/CustomLabel.h"
#include "CustomControl/CustomImage.h"
#include "CustomControl/CustomButton.h"
#include "CustomControl/CustomEdit.h"


// CDlgLogin 对话框

typedef std::tr1::function<bool(const stdex::tString &)> LoginCallback;

class CDlgLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(const LoginCallback &connect, const LoginCallback &password, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogin();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOGIN };

private:
	LoginCallback connect_;
	LoginCallback password_;

	CString serverIP_;
	CString serverPass_;

	CCustomEdit wndEditName_;
	CEdit wndEditPassword_;

	CCustomImage bk_;
	CCustomLabel serverName_;
	CCustomLabel pwd_;
	CCustomLabel tip_;

	CCustomButton ok_;
	CCustomButton cancel_;

	ATL::CImage title_;
	ATL::CImage content_;
	ATL::CImage outBox_;

	CCustomButton wndBtnClose_;
	
	CString titleText_;
	WTL::CFontHandle font_;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	
};
