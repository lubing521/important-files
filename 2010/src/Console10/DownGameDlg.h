#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CDownGameDlg 对话框

class CDownGameDlg : public CDialog
{
	DECLARE_DYNAMIC(CDownGameDlg)

public:
	CDownGameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownGameDlg();


// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNGAME };

	BOOL m_bIsMultiSelect;					// 是否多选标志
	i8desk::GameRunTypeList m_RunType;
	i8desk::tagGameInfo m_GameInfo;			// 单选游戏
	i8desk::GameInfoVector m_GamesInfo;		// 多选游戏

protected:
	i8desk::DefClassMap m_ClassInfos;
	i8desk::AreaInfoMap  m_AreaInfos;
	i8desk::VDiskInfoMap m_VDiskInfos;

	CString GetAreaRunType(LPCSTR aid, const CString& szAreaRunType);
	CString ParaseAreaRunType(const CString& AreaName, const CString& RunType);
	CString GetAreaRTText(std::string& AreaName, i8desk::GameRunTypeList& RunType);
	std::string GetAIDByName(std::string& Name);
	BOOL ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CGridListCtrlEx m_lstRunType;
	CGridListCtrlEx syncTypeList_;;

	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	CComboBox m_cboPriority;
	CComboBox m_cboAutoUpt;
	CComboBox m_cb3upPriority;
	i8desk::SyncTaskMap m_SyncTasks;
};
