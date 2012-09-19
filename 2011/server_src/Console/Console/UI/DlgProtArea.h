#pragma once
#include "afxwin.h"

#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomComboBox.h"
#include "../CustomControl/CustomEdit.h"
#include "../CustomControl/CustomLabel.h"
#include "../CustomControl/CustomButton.h"


// CDlgProtArea 对话框

class CDlgProtArea : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgProtArea)

public:
	CDlgProtArea(const CString &unProtAreas, const CString &protAreas, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProtArea();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIENT_PROTAREA };

private:
	CListBox wndListBoxProtAreas_;
	CListBox wndListBoxUnProtAreas_;
	CString unProtAreas_;
	CString protAreas_;
	CString allAreas_;

	CCustomLabel wndunProtAreas_;
	CCustomLabel wndprotAreas_;

	CCustomButton wndBtnAddProt_;
	CCustomButton wndBtnAddAllProt_;
	CCustomButton wndBtnRemoveProt_;
	CCustomButton wndBtnRemoveAll_;
	CCustomButton wndBtnOk_;
	CCustomButton wndBtnCancel_;

	ATL::CImage			outboxLine_;

public:
	const CString &UnProtAreas() const
	{ return unProtAreas_; }
	const CString &ProtAreas() const
	{ return protAreas_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonClientControlAddPort();
	afx_msg void OnBnClickedButtonClientControlAddallPort();
	afx_msg void OnBnClickedButtonClientControlRemovePort();
	afx_msg void OnBnClickedButtonClientControlRemoveallPort();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
};
