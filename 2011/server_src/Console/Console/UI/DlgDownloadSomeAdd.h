#pragma once


#include "afxwin.h"
#include "afxcmn.h"

#include "../Network/NetworkImpl.h"
#include "../../../../include/MultiThread/Lock.hpp"
#include "../GridListCtrlEx/CGridColumnTraitCombo.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinCheckBox.h"
#include "Skin/SkinProgress.h"
#include "Skin/SkinEdit.h"

// CDlgDownloadSomeAdd 对话框

class CDlgDownloadSomeAdd : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadSomeAdd)

public:
	CDlgDownloadSomeAdd(const stdex::tString &classes, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadSomeAdd();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNLOAD_SOME_ADD };

private:
	stdex::tString curClass_;
	async::thread::AutoEvent stop_;

	UINT searchDeep_;
	CString searchDir_;

	bool bCanClick_;

	i8desk::ui::SkinEdit wndEditSearch_;

	i8desk::ui::SkinButton wndBtnSearch_;
	i8desk::ui::SkinButton wndBtnStart_;
	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;

	i8desk::ui::SkinComboBox wndComboClass_;
	i8desk::ui::SkinComboBox wndComboResulte_;
	i8desk::ui::SkinComboBox wndComboDeep_;
	i8desk::ui::SkinProgress wndProgress_;
	i8desk::ui::SkinListCtrl wndListResult_;
	i8desk::ui::SkinListCtrl wndListRuntype_;

	i8desk::ui::SkinLabel wndEditTip_;
	i8desk::ui::SkinLabel wndSet_;
	i8desk::ui::SkinLabel wndSearchDir_;
	i8desk::ui::SkinLabel wndDeep_;
	i8desk::ui::SkinLabel wndClass_;
	i8desk::ui::SkinLabel wndResulte_;
	i8desk::ui::SkinLabel wndRuntype_;

	ATL::CImage outboxLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	void _Scan();
	
public:
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnBnClickedButtonDownloadAddBrowseDir();
	afx_msg void OnBnClickedButtonDownloadAddExecute();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnLvnGetdispinfoListDownloadAddResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnKillfocusDownloadResults();
	afx_msg void OnCbnCloseupDownloadResults();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
