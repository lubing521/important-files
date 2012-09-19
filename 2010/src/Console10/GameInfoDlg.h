#pragma once


class CGameInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameInfoDlg)

public:
	CGameInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameInfoDlg();

	BOOL SetGameFile(const tstring& filename);

// 对话框数据
	enum { IDD = IDD_DIALOG_GAMEINFO };

	bool    m_bIsDrop;
	bool    m_bIsAdd;
	bool    m_bAutoOk;
	i8desk::tagGameInfo m_GameInfo;
private:
	i8desk::AreaInfoMap  m_AreaInfos;
	i8desk::DefClassMap  m_ClassInfos;
	i8desk::VDiskInfoMap m_VDiskInfos;
	i8desk::GameRunTypeList m_lstRT;
	HICON m_hIcon;
	char  m_IconData[10240];
	DWORD m_dwIconSize;

	//static ATL::CAutoVectorPtr<std::pair<std::wstring, i8desk::GameInfo_Ptr>> m_pStrDatabaseGame;
	static std::pair<std::wstring, i8desk::GameInfo_Ptr> m_pStrDatabaseGame[];

protected:
	BOOL InitGameInfoForDrop(void);
	CString GetGameClass(i8desk::GameInfo *pGameInfo);
	std::string GetAIDByName(const std::string& Name);
	CString GetAreaRTText(const std::string& AreaName, i8desk::GameRunTypeList& RunType);
	CString GetAreaRunType(const std::string& aid, const CString& szAreaRunType);
	BOOL    ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType);
	
	CString OpenLocMachineDir();
	CString OpenRemMachineDir();

	CString BrowseLocFile(CString szDir);
	CString BrowseRemFile(CString szDir);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CButton m_btnToolbar;
	CButton m_btnDeskLnk;
	CComboBox m_cboClass;
	CComboBox m_cboPriority;
	CStatic m_staGameIcon;
	CComboBox m_cboAutoUpt;
	CGridListCtrlEx m_lstRunType;
	//afx_msg void OnBnClickedBrowse();
	afx_msg void OnStnClickedGameicon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBrowseDir();
	afx_msg void OnBnClickedBrowseFile();
	CString m_strGameName;
	CString m_strSvrPath;
	CString m_strExecFile;
	CString m_strCliPath;
	CComboBox m_cb3upPriority;
};
