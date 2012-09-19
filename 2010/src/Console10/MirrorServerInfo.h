#pragma once


// CMirrorServerInfo 对话框

class CMirrorServerInfo : public CDialog
{
	DECLARE_DYNAMIC(CMirrorServerInfo)

private:
	CImageList		m_ImageList;
	CTreeCtrl		m_tcPriorityArea;

	CString			m_strSyncIP;
	CString			m_strVirtualIP;
	CString			m_strNetworkIP;
	
	
public:
	BOOL				m_bAdd;
	bool				m_bServerMachine;

	const i8desk::std_string		&m_strMainServerID;

	i8desk::SyncTask	m_SyncTask;
	i8desk::AreaInfoMap m_AreaInfos;
	i8desk::SyncTaskMap m_MirrorServers;

public:
	CMirrorServerInfo(const i8desk::std_string &strMainServerID, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMirrorServerInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_MIRROR_SERVER_INFO };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


private:
	BOOL GetData(i8desk::SyncTask *SyncTask);
	void UpdateData(const i8desk::SyncTask *SyncTask);
	void UpdatePriorityAreaData(const char *SID);
	bool ModifyPriorityArea(const char *SID);
	void ClearPriorityArea(const char *SID);
	
	// 不同服务器不能有相同区域
	bool _JudgeArea(LPCTSTR AID);
	
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioBalanceDynamic();
	afx_msg void OnBnClickedRadioPriorityArea();

	afx_msg void OnCbnEditchangeComboSync();
	afx_msg void OnCbnSelchangeComboSync();
};
