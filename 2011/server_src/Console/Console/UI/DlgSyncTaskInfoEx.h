#pragma once

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinGroup.h"
#include "Skin/SkinListCtrl.h"

#include <vector>

// CDlgSyncTaskInfoEx 对话框

class CDlgSyncTaskInfoEx : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgSyncTaskInfoEx)

public:
	CDlgSyncTaskInfoEx(int status, i8desk::data_helper::SyncTaskTraits::ElementType *syncTask, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSyncTaskInfoEx();

	// 对话框数据
	enum { IDD = IDD_DIALOG_WZ_SYNCTASK_INFO };
	enum ViewStatus { LocalGames = 0, Tasks };
	enum BtnStatus { isadd = 0, ismodify, iscopy };

private:
	int status_;
	i8desk::data_helper::SyncTaskTraits::ValueType cursyncTask_;

	i8desk::data_helper::GameTraits::VectorType localGames_;
	i8desk::data_helper::GameTraits::VectorType curGames_;
	i8desk::data_helper::SyncGameTraits::VectorType curtaskGames_;
	stdex::tString curCID_;
	ViewStatus viewStatus_;

	i8desk::ui::SkinListCtrl wndListLocalGames_;
	i8desk::ui::SkinListCtrl wndListTaskGames_;

	i8desk::ui::SkinButton wndbtnOk_;
	i8desk::ui::SkinButton wndbtnCancel_;
	i8desk::ui::SkinButton wndBtnSyncTaskAdd_;
	i8desk::ui::SkinButton wndBtnSyncTaskModify_;
	i8desk::ui::SkinButton wndBtnSyncTaskDelete_;
	

	i8desk::ui::SkinEdit wndEditQuery_;
	i8desk::ui::SkinButton wndBtnQuery_;

	CString maxSpeed_;
	CString syncTaskName_;

	i8desk::ui::SkinGroup  wndGroup_;

	i8desk::ui::SkinLabel wndTipSyncTaskName_;
	i8desk::ui::SkinLabel wndTipFrom_;
	i8desk::ui::SkinLabel wndTipTo_;
	i8desk::ui::SkinLabel wndTipSvrName_;
	i8desk::ui::SkinLabel wndTipSvrIP_;
	i8desk::ui::SkinLabel wndTipSvrDir_;
	i8desk::ui::SkinLabel wndTipCliName_;
	i8desk::ui::SkinLabel wndTipCliIP_;
	i8desk::ui::SkinLabel wndTipCliDir_;
	i8desk::ui::SkinLabel wndTipSvrDirParam_;

	i8desk::ui::SkinLabel wndTipSpeed_;
	i8desk::ui::SkinLabel wndTipSpeedKB_;
	i8desk::ui::SkinLabel wndGroupLocal_;
    i8desk::ui::SkinLabel wndGroupSyncTask_;
    i8desk::ui::SkinLabel wndGroupSyncTaskTotalSizeText_;
    i8desk::ui::SkinLabel wndGroupSyncTaskTotalSize_;

	i8desk::ui::SkinComboBox wndComboSvrName_;
	i8desk::ui::SkinComboBox wndComboSvrIP_;
	i8desk::ui::SkinComboBox wndComboCliName_;
	i8desk::ui::SkinComboBox wndComboCliIP_;
	i8desk::ui::SkinComboBox wndComboCliDir_;
	i8desk::ui::SkinComboBox wndComboClass_;

	i8desk::ui::SkinEdit  wndEditsyncTaskName_;
	i8desk::ui::SkinEdit  wndEdimaxSpeed_;


	ATL::CImage outboxLine_;

	bool IsInit_;

public:
	i8desk::data_helper::SyncTaskTraits::ValueType GetCurSycTask( ){ return cursyncTask_; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


private:
	bool _CheckData();
	void _ShowViewLocal();
	void _ShowViewTask();
	CListCtrl &_GetCurList();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnCbnSelchangeComboSynctaskSvrName();
	afx_msg void OnCbnSelchangeComboSynctaskCliName();
	afx_msg void OnBnClickedCheckSyncTaskInfoSelectAll();
	afx_msg void OnCbnSelchangeComboSynctaskClass();

	afx_msg void OnTvnSelchangedTreeSyncTask(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnGetdispinfoListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListTaskGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListTaskGames(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnColumnclickListLocalGames(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListTaskGames(NMHDR *pNMHDR, LRESULT *pResult);


	afx_msg void OnBnClickedButtonSynctaskAddSyncGame();
	afx_msg void OnBnClickedButtonSynctaskDelSyncGame();

	afx_msg void ONBnClickedButtonSearch();
	afx_msg void OnEnChangeEditDownloadSearch();
    stdex::tString GetGameSize(ULONG gid);

};
