#pragma once

#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"

#include "Skin/SkinButton.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinListCtrl.h"
#include <set>

// CDlgServer 窗体视图



class CDlgServer 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE

	DECLARE_DYNCREATE(CDlgServer)

public:
	CDlgServer(CWnd* pParent = NULL);           // 动态创建所使用的受保护的构造函数
	virtual ~CDlgServer();

public:
	enum { IDD = IDD_DLG_SERVER };
	enum ViewStatus { BASIC, VDISK };

public:
	void SelChangeServerSyncTask();

public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

private:
	i8desk::ui::ControlMgr wndMgr_;


	CCustomButton wndStaticServerMgr_;

	
	i8desk::ui::SkinButton wndBtnVirtualMgr_;
	i8desk::ui::SkinButton wndBtnServerMgr_;

	CCustomEdit wndEditQuery_;
	CCustomButton wndBtnQuery_;
	
	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr rightArea_[5];
	ImagePtr leftArea_[3];
	ImagePtr workOutLine_;
	

	CTreeCtrl wndTreeServer_;

	CImageList treeServerImg_;

	i8desk::ui::SkinListCtrl wndListServerBasic_;
	i8desk::ui::SkinListCtrl wndListServerVDisk_;

	ViewStatus viewStatus_;
	stdex::tString curSvrID_;
	stdex::tString curVDiskID_;

	i8desk::data_helper::VDiskClientTraits::VectorType diskClients_;
	i8desk::data_helper::ServerStatusTraits::VectorType serverStatus_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void _Init();
	void _AddNewServer();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	afx_msg void OnLvnGetdispinfoListServerBasic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListServerVDisk(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnTvnSelchangedTreeServerServer(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedButtonServerServerMgr();
	afx_msg void OnBnClickedButtonServerVirtualMgr();
	afx_msg void ONBnClickedButtonSearch();
	
	afx_msg LRESULT OnApplySelect(WPARAM, LPARAM);
	afx_msg LRESULT OnAddVDisk(WPARAM, LPARAM);
	afx_msg LRESULT OnDelVDisk(WPARAM, LPARAM);
	afx_msg LRESULT OnModVDisk(WPARAM, LPARAM);
	afx_msg LRESULT OnAddServer(WPARAM, LPARAM);
};


