#pragma once

#include "../EasySize.h"

#include "../ui/WindowManager.h"
#include "../ManagerInstance.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
// CDlgDownLoad 窗体视图

class CDlgDownLoad 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE

	DECLARE_DYNCREATE(CDlgDownLoad)

public:
	CDlgDownLoad(CWnd* pParent = NULL);           // 动态创建所使用的受保护的构造函数
	virtual ~CDlgDownLoad();

public:
	enum { IDD = IDD_DLG_DOWNLOAD };
	enum ViewStatus { Center = 0, Local, Download, Update };
	enum CenterMask 
	{ 
		CENTER_SYSTEM, CENTER_ACTIVE, CENTER_NO_DOWNLOAD, CENTER_ADD_RECENT, CENTER_DELETE, CENTER_UPDATE, CENTER_DEFAULT			
	};

	enum MonitorMask { MONITOR_DOWNLOAD, MONITOR_UPDATE };

private:
	ViewStatus viewStatus_;
	CenterMask centerMask_;
	i8desk::ui::ControlMgr wndMgr_;

	CMenu menuClassFilter_;
	i8desk::ui::SkinButton wndBtnTaskStart_;
	i8desk::ui::SkinButton wndBtnTaskPause_;
	i8desk::ui::SkinButton wndBtnTaskStop_;
	i8desk::ui::SkinButton wndBtnTaskAdd_;
	i8desk::ui::SkinButton wndBtnClassFilter_;

	i8desk::ui::SkinButton wndBtnForceUpdate_;
	i8desk::ui::SkinButton wndBtnAddGame_;
	i8desk::ui::SkinButton wndBtnAddSomeGames_;
	i8desk::ui::SkinButton wndBtnSmartClean_;
	i8desk::ui::SkinButton wndBtnModifyGame_;
	i8desk::ui::SkinButton wndBtnEraseGame_;

	CCustomButton wndBtnClassAdd_;
	CCustomButton wndBtnClassDel_;
	CCustomButton wndBtnClassMod_;

	i8desk::ui::SkinButton wndBtnDownloadTop_;
	i8desk::ui::SkinButton wndBtnDownloadBottom_;
	i8desk::ui::SkinButton wndBtnShowLog_;

	CCustomButton wndBtnResMgr_;
	CCustomButton wndBtnLocalMgr_;
	CCustomButton wndBtnDataMgr_;

	CCustomEdit wndEditQuery_;
	CCustomButton wndBtnQuery_;

	CTreeCtrl wndTreeCenter_;
	CTreeCtrl wndTreeLocal_;
	CTreeCtrl wndTreeMonitor_;

	i8desk::ui::SkinListCtrl wndListCenter_;
	i8desk::ui::SkinListCtrl wndListDownload_;
	i8desk::ui::SkinListCtrl wndListLocal_;
	i8desk::ui::SkinListCtrl wndListUpdate_;

	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr rightArea_[5];
	ImagePtr leftArea_[3];
	ImagePtr workOutLine_;

	CImageList centerTreeImageList_;
	CImageList localTreeImageList_;
	CImageList monitorTreeImageList_;

	i8desk::data_helper::GameTraits::VectorType curGames_;

	std::map<int, size_t> centerGameSize_;
	std::map<stdex::tString, size_t> localGameCnt_;

public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

private:
	void _HandleTask(i8desk::TaskNotify notify, long gid, DWORD err);
	void _HandleRefreshPL();
	void _Cummulate();
	void _ShowView();
	void _ShowTree();
	void _ShowButton();
	CListCtrl &_GetCurList();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	
private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDestroy();
	afx_msg void OnTvnItemChangedTreeDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemChangedTreeDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemChangedTreeDownloadMonitor(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnGetdispinfoListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListDownloadDownload(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListDownloadUpdate(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnOdcachehintListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehintListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMRClickListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListDownloadDownload(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnColumnclickListDownloadCenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListDownloadLocal(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedButtonDownloadStart();
	afx_msg void OnBnClickedButtonDwonloadPause();
	afx_msg void OnBnClickedButtonDownloadStop();

	afx_msg void OnBnClickedButtonDownloadLocalAdd();
	afx_msg void OnBnClickedButtonDownloadLocalDelete();
	afx_msg void OnBnClickedButtonDownloadLocalModify();
	afx_msg void OnBnClickedButtonDownloadSmartClean();

	afx_msg void OnBnClickedButtonDownloadFilter();
	afx_msg void OnBnClickedButtonDownloadToadd();

	afx_msg void OnBnClickedButtonDownloadTop();
	afx_msg void OnBnClickedButtonDownloadBottom();
	afx_msg void OnBnClickedButtonShowLog();

	afx_msg void ONBnClickedButtonSearch();
	afx_msg void OnQueryTextChanged();

	afx_msg void OnMenuClassFilter(UINT uID);
	afx_msg void OnMenuCenter(UINT uID);
	afx_msg void OnMenuLocal(UINT uID);
	afx_msg void OnMenuTask(UINT uID);
	afx_msg void OnBnClickedButtonDownloadForceupdate();
	afx_msg void OnBnClickedButtonDownloadAddGame();
	afx_msg void OnBnClickedButtonDownloadAddSomeGame();
	afx_msg void OnBnClickedButtonDownloadModifyGame();
	afx_msg void OnBnClickedButtonDownloadEraseGame();
	
	afx_msg LRESULT OnApplySelect(WPARAM wParam, LPARAM);
	
	afx_msg void OnEnChangeEditDownloadSearch();
	afx_msg LRESULT OnRefreshPL(WPARAM, LPARAM);
	afx_msg LRESULT OnCummulate(WPARAM, LPARAM);

	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg	void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg	LRESULT OnReportSelectState(WPARAM wParam,LPARAM lParam);
    stdex::tString GetSizeText(ULONGLONG totalSize);
    ULONGLONG GetSelectedGameTotalSize(CListCtrl& listCtrl);
};


