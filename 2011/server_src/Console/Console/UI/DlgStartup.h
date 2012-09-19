#pragma once
#include "afxcmn.h"

#include "../EasySize.h"
#include "../CustomControl/CustomListCtrl.h"
#include "../CustomControl/CustomButton.h"

#include "../ui/WindowManager.h"
#include "../ManagerInstance.h"



// CDlgStartup 窗体视图

class CDlgStartup 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE
	DECLARE_DYNCREATE(CDlgStartup)

public:
	CDlgStartup(CWnd* pParent = NULL);           // 动态创建所使用的受保护的构造函数
	virtual ~CDlgStartup();

public:
	enum { IDD = IDD_DLG_STARTUP };

private:
	CCustomListCtrl wndListBootTask_;

	CCustomButton wndBtnAdd_;
	CCustomButton wndBtnDel_;
	CCustomButton wndBtnMod_;

	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr area_[5];
	ImagePtr workOutLine_;


	i8desk::data_helper::BootTaskTraits::VectorType bootTasks_;

public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	void _ShowView();
	
private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedButtonStartupModify();

	afx_msg void OnLvnGetdispinfoListStartupBoottasks(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRDblclkListStartupBoottasks(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnApplySelect(WPARAM, LPARAM);
};


