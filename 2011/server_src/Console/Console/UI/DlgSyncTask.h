#pragma once

#include "../EasySize.h"
#include "../ui/WindowManager.h"
#include "../ManagerInstance.h"

#include "Skin/SkinEdit.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"

// CDlgSyncTask 对话框

class CDlgSyncTask 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CDlgSyncTask)

public:
	CDlgSyncTask(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSyncTask();

// 对话框数据
	enum { IDD = IDD_DLG_SYNC_TASK };
	enum ViewStatus { LocalGames = 0, Tasks };
	enum BtnStatus { isadd = 0, ismodify, iscopy };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
private:
	ViewStatus viewStatus_;

	i8desk::ui::ControlMgr wndMgr_;

	CTreeCtrl wndTreeLocalGame_;
	CTreeCtrl wndTreeSyncTask_;

	i8desk::ui::SkinListCtrl wndListLocalGames_;
	i8desk::ui::SkinListCtrl wndListTaskGames_;

	CCustomEdit wndEditQuery_;
	CCustomButton wndBtnQuery_;
	
	i8desk::ui::SkinButton wndBtnTaskStart_;
	i8desk::ui::SkinButton wndBtnTaskPause_;

	i8desk::ui::SkinButton wndBtnNotifySync_;
	i8desk::ui::SkinButton wndBtnDelSyncGameToSID_;
	i8desk::ui::SkinButton wndBtnDelSyncGame_;
	i8desk::ui::SkinButton wndBtnAddSyncGame_;

	CCustomButton wndBtnLocalGameMgr_;
	CCustomButton wndBtnSyncTaskMgr_;
	CCustomButton wndBtnSyncTaskAdd_;
	CCustomButton wndBtnSyncTaskModify_;
	CCustomButton wndBtnSyncTaskDelete_;

	CImageList localTreeImageList_;
	CImageList syncTreeImageList_;

	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr rightArea_[5];
	ImagePtr leftArea_[3];
	ImagePtr workOutLine_;

	i8desk::data_helper::GameTraits::VectorType localGames_;
	i8desk::data_helper::SyncTaskStatusTraits::VectorType curtaskGames_;
	i8desk::data_helper::SyncTaskTraits::ElementType *cursynctask_;

	stdex::tString curSID_;
	HTREEITEM curTreeItem_;

	std::map<stdex::tString, size_t> localGameCnt_;
public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

private:
	void _ShowView();
	void _ShowButton();
	void _Cummulate();

	CListCtrl &_GetCurList();


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnChar(UINT, UINT, UINT);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDestroy();

	afx_msg void OnTvnSelchangedTreeLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeSyncTask(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnGetdispinfoListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListTaskGames(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnLvnOdcachehintListLocalSyncGames(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedButtonSynctaskAdd();
	afx_msg void OnBnClickedButtonSynctaskDelete();
	afx_msg void OnBnClickedButtonSynctaskModify();

	afx_msg void ONBnClickedButtonNotifySync();
	afx_msg void ONBnClickedButtonDelSyncGame();
	afx_msg void ONBnClickedButtonAddSyncGame();
	afx_msg void ONBnClickedButtonDelSyncGameToSID();

	afx_msg void OnBnClickedButtonSyncTaskStart();
	afx_msg void OnBnClickedButtonSyncTaskPause();

	afx_msg void ONBnClickedButtonSearch();
	afx_msg void OnQueryTextChanged();
	afx_msg void OnEnChangeEditDownloadSearch();

	afx_msg void OnMenuLocalNotify();
	afx_msg void OnMenuLocalDelGame();
	afx_msg void OnMenuLocalAddGame();
	afx_msg void OnMenuSyncTaskNotify();
	afx_msg void OnMenuSyncTaskDelGame();
	afx_msg void OnMenuSyncTreeCopy();
	afx_msg void OnMenuSyncTreeDel();

	afx_msg void OnMenuSyncTree(UINT uID);
	afx_msg void OnNMRClickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListSyncTasks(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnCummulate(WPARAM, LPARAM);
    afx_msg LRESULT OnApplySelect(WPARAM, LPARAM);
    afx_msg LRESULT OnReportSelectState(WPARAM wParam,LPARAM lParam);
    ULONGLONG GetSelectedLocalGameTotalSize();
    ULONGLONG GetSelectedTaskGameTotalSize();
    stdex::tString GetSizeText(ULONGLONG totalSize);

	afx_msg void OnLvnColumnclickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListSyncTasks(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMRClickTreeSynctaskTasks(NMHDR *pNMHDR, LRESULT *pResult);
};
