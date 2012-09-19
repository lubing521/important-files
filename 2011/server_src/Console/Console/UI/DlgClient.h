#pragma once

#include "../EasySize.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinComboBox.h"

#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"


// CDlgClient 窗体视图

class CDlgClient 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE
	DECLARE_DYNCREATE(CDlgClient)

public:
	CDlgClient(CWnd* pParent = NULL);       
	virtual ~CDlgClient();

public:
	enum { IDD = IDD_DLG_CLIENT };
	enum ViewStatus { Basic = 0, System, Version };

	
private:
	i8desk::ui::ControlMgr wndMgr_;
	CTreeCtrl wndTree_;
	i8desk::ui::SkinListCtrl wndListBasic_;
	i8desk::ui::SkinListCtrl wndListSystem_;
	i8desk::ui::SkinListCtrl wndListVersion_;

	CCustomButton wndBtnClientMgr_;
	CCustomButton wndBtnAreaAdd_;
	CCustomButton wndBtnAreaDel_;
	CCustomButton wndBtnAreaMod_;

	i8desk::ui::SkinButton wndBtnAdd_;
	i8desk::ui::SkinButton wndBtnMod_;
	i8desk::ui::SkinButton wndBtnDel_;

	i8desk::ui::SkinButton wndBtnRemote_;
	i8desk::ui::SkinButton wndBtnProtect_;
	i8desk::ui::SkinButton wndBtnRestore_;
	i8desk::ui::SkinButton wndBtnSaveHardware_;

	i8desk::ui::SkinComboBox wndComboChangeView_;

	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr rightArea_[5];
	ImagePtr leftArea_[3];
	ImagePtr workOutLine_;

	ATL::CImage outBox_;
	
	CImageList treeImages_;
	ViewStatus viewStatus_;
	stdex::tString AID_;

	i8desk::data_helper::ClientTraits::VectorType curClients_;


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
	// 得到选中的客户机
	void GetSelectClients(i8desk::data_helper::ClientTraits::VectorType &clients);

private:
	void _Init();
	void _ShowClients();
	void _ShowRealClients();
	void _ShowControlMenu(int nItem);
	void _ChangeView(UINT);
	void _ChangeHardware(UINT);
	CCustomListCtrl *_GetCurList();
	
private:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	afx_msg void OnTvnSelchangedTreeClient(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnGetdispinfoListClientBasic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListClientSystem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListClientVersion(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMRClickListClientSystem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListClientBasic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListClientVersion(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnColumnclickListClientVersion(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListClientSystem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListClientBasic(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedButtonClientAreaAdd();
	afx_msg void OnBnClickedButtonClientAreaDelete();
	afx_msg void OnBnClickedButtonClientAreaModify();

	afx_msg void OnBnClickedButtonClientAdd();
	afx_msg void OnBnClickedButtonClientModify();
	afx_msg void OnBnClickedButtonClientDelete();
	afx_msg void OnBnClickedButtonClientSelectView();
	afx_msg void OnBnClickedButtonClientControl();
	afx_msg void OnBnClickedButtonClientAreaProtect();
	afx_msg void OnBnClickedButtonClientInstallRestore();
	afx_msg void OnBnClickedButtonClientSaveHardware();

	afx_msg void OnControlOperate(UINT);
	afx_msg void OnComboChangeView();

    afx_msg LRESULT OnApplySelect(WPARAM, LPARAM);
    afx_msg LRESULT OnReportSelectState(WPARAM wParam,LPARAM lParam);
};


