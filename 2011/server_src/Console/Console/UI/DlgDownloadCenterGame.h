#pragma once


#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinListCtrl.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinCheckBox.h"

#include "../ManagerInstance.h"


// CDlgDownloadCenterGame 对话框
using i8desk::data_helper::GameTraits;

class CDlgDownloadCenterGame : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadCenterGame)

public:
	CDlgDownloadCenterGame(bool isMultiSelect, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadCenterGame();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNGAME_CENTER_ADD };

private:
	bool isMultiSelect_;

	GameTraits::ValueType curGame_;
	GameTraits::VectorType curGames_;

	CString gameName_;
	CString gameSvrPath_;
	CString gameCliPath_;

	i8desk::data_helper::SyncTaskTraits::MapType diffSyncTasks_;

public:
	i8desk::ui::SkinComboBox wndComboGameUpdate_;
	i8desk::ui::SkinComboBox wndComboDownloadPriority_;
	i8desk::ui::SkinEdit wndEditgameName_;
	i8desk::ui::SkinEdit wndEditgameSvrPath_;
	i8desk::ui::SkinEdit wndEditgameCliPath_;

	i8desk::ui::SkinLabel wndLabelgameName_;
	i8desk::ui::SkinLabel wndLabelgameSvrPath_;
	i8desk::ui::SkinLabel wndLabelgameCliPath_;
	i8desk::ui::SkinLabel wndLabelgameRunType_;
	i8desk::ui::SkinLabel wndLabelgameSyncTask_;
	i8desk::ui::SkinLabel wndLabelGameUpdate_;
	i8desk::ui::SkinLabel wndLabelDownloadPriority_;
	CMFCLinkCtrl wndLabelDeclare_;

	i8desk::ui::SkinListCtrl wndListRunType_;
	i8desk::ui::SkinListCtrl wndListSyncTask_;

	i8desk::ui::SkinButton wndBtnOk_;
	i8desk::ui::SkinButton wndBtnCancel_;

	i8desk::ui::SkinCheckBox wndChkSelSyncTaskAll_;
	
	ATL::CImage outboxLine_;

public:
	void SetCurrentGame(const GameTraits::ValueType &curGame)
	{ curGame_ = curGame; }
	void SetCurrentGames(const GameTraits::VectorType &curGames)
	{ curGames_ = curGames; }


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckSelSyncTaskAll();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnStnClickedStaticDownloadCenterAddDeclare();
};
