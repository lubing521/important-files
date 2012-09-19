#pragma once
#include "afxwin.h"

class CSpeedEdit : public CEdit
{
	DECLARE_DYNAMIC(CSpeedEdit)
protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};

// CSyncTaskInfo 对话框

class CSyncTaskInfo : public CDialog
{
	DECLARE_DYNAMIC(CSyncTaskInfo)

public:
	CSyncTaskInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSyncTaskInfo();

	i8desk::GameInfoMap		m_GameInfos;
	i8desk::DefClassMap		m_ClassInfos;

	BOOL m_bAdd;
	i8desk::SyncTask m_SyncTask;

// 对话框数据
	enum { IDD = IDD_DIALOG_SYNC_TASK_INFO };

protected:
	BOOL GetData(i8desk::SyncTask *SyncTask);
	void UpdateData(const i8desk::SyncTask *SyncTask);
	void UpdateTargetData(const i8desk::SyncTask *SyncTask);
	void UpdateCustomData(const i8desk::SyncTask *SyncTask);
	void ModifyCustomSyncGame(const char *SID);

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnCbnSelchangeComboSyncType();
	afx_msg void OnNMClickTreeCustomSyncgame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:

	CComboBox m_cbSyncType;
	CImageList m_ImageList;
	CTreeCtrl m_tcCustomSyncGame;
	CIPAddressCtrl m_ipcTargetIP;
	CComboBox m_cbTargetPartition;
	CSpeedEdit m_editMaxSpeed;

public:
	afx_msg void OnBnClickedCheck();
	afx_msg void OnIpnFieldchangedIpaddressTargetIp(NMHDR *pNMHDR, LRESULT *pResult);
};
