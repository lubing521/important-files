#pragma once
#include "afxcmn.h"


// CSyncTaskMgr 对话框

class CSyncTaskMgr : public CDialog
{
	DECLARE_DYNAMIC(CSyncTaskMgr)

public:
	CSyncTaskMgr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSyncTaskMgr();

	i8desk::GameInfoMap		m_GameInfos;
	i8desk::DefClassMap m_ClassInfos;

// 对话框数据
	enum { IDD = IDD_DIALOG_SYNC_TASK_MGR };

protected:
	void InsertSyncTaskToList(const i8desk::SyncTask *SyncTask);
	void UpdateSyncTaskListItem(int nItem, const i8desk::SyncTask* SyncTask);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAddSyncTask();
	afx_msg void OnBnClickedButtonModirySyncTask();
	afx_msg void OnBnClickedButtonDelSynTask();
	afx_msg void OnLvnDeleteitemListSyncTask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSyncTask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListSyncTask(NMHDR *pNMHDR, LRESULT *pResult);
	
	CListCtrl m_lcSyncTask;
	i8desk::SyncTaskMap m_SyncTasks;
};
