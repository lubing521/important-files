#pragma once
#include "I8UIFrame/NonFrameChildDlg.h"
#include "ui/Skin/SkinListCtrl.h"
#include "ui/Skin/SkinCheckBox.h"
#include "ManagerInstance.h"

#include "EasySize.h"

// CDlgViewRunState 对话框

class CDlgViewRunState : public CNonFrameChildDlg
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CDlgViewRunState)

public:
	CDlgViewRunState(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgViewRunState();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIEWRUNSTATE };

private:
	i8desk::ui::SkinListCtrl wndListView_;
	i8desk::ui::SkinCheckBox wndCheckViewMore_;
	ATL::CImage outboxLine_;
	i8desk::data_helper::ViewClientTraits::VectorType curviewRunStates_;

private:
	void _AddLog(const i8desk::LogRecordPtr &log);
	void _ShowView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnGetdispinfoListViewRunState(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnLogMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void OnBnClickedCheckMore();
};
