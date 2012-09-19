#pragma once


// CMirrorServerMgr 对话框

class CMirrorServerMgr 
	: public CDialog
{
	DECLARE_DYNAMIC(CMirrorServerMgr)

private:
	CListCtrl			m_lcMirrorServer;
	i8desk::SyncTaskMap m_MirrorServers;
	i8desk::AreaInfoMap m_AreaInfos;
	i8desk::std_string  m_strMainServerID;			// 主服务器ID
	bool				m_bMainServerExists;		// 主服务器是否存在

public:
	CMirrorServerMgr(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMirrorServerMgr();

// 对话框数据
	enum { IDD = IDD_DIALOG_MIRROR_SVR_MGR };


public:
	void SetAreaInfo(const i8desk::AreaInfoMap &areaInfos);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	void InsertMirrorServerToList(const i8desk::SyncTask *SyncTask, bool bMainServer);
	void UpdateMirrorServerListItem(int nItem, const i8desk::SyncTask* SyncTask, bool bMainServer);
	
	bool _JudgeSameServer(LPCTSTR lpszID, LPCTSTR lpszName);
	bool _JudgeSameArea(LPCTSTR lpszID);
	bool _InsertMainServer();
	void _InitList();
	void _UpdateListCtrl();

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonAddMirrorSvr();
	afx_msg void OnBnClickedButtonModifyMirrorSvr();
	afx_msg void OnBnClickedButtonDelMirrorSvr();
	
	afx_msg void OnNMClickListMirrorSvr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListMirrorSvr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListMirrorSvr(NMHDR *pNMHDR, LRESULT *pResult);
};
