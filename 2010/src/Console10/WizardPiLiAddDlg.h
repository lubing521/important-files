#pragma once
#include "afxcmn.h"
#include "afxwin.h"


class CWizardAddGameBoard : public CDialog
{
	DECLARE_DYNAMIC(CWizardAddGameBoard)

public:
	CWizardAddGameBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWizardAddGameBoard();

// 对话框数据
	enum { IDD = IDD_WIZARD_PILIANGADD };

	bool m_bModified;

	typedef struct tagItem
	{
		CString dir;
		int nMacthCount;
		bool bMatchType; //是否精确匹配
		i8desk::GameInfoVector MatchGame;
		i8desk::GameInfo_Ptr pGame;	//用于匹配重复的选择
		CString exe;				//用于不能匹配的游戏,记录用于选择的主程序.
	}tagItem;
protected:
	bool m_bDirSortDesc;
	bool m_bMatchSortDesc;
	bool m_bResultSortDesc;
	bool m_bGidSortDesc;
	CString m_strPath;
	int		m_nDeep;
	std::vector<tagItem> m_lstDir;
	HANDLE m_hThread;
	HANDLE m_hExited;
	BOOL   m_bCanClick;
	i8desk::DefClassMap ClassInfos;
	i8desk::AreaInfoMap AreaInfos;
	i8desk::VDiskInfoMap VDiskInfos;
	i8desk::GameInfoMap GameInfos;
	std::set<DWORD> m_IDArray;
	CListCtrl m_lstCtrl;

	char *m_pbuf; //存放索引文件的缓冲
	long GetGameIdFromIdx(const CString& dir);
	static UINT __stdcall WorkThread(LPVOID lpVoid);
	void ScanDir(const CString& dir, std::vector<tagItem>& lstDir, int nDeep, int nCurDeep);

	std::string GetAIDByName(std::string& Name);
	BOOL ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType);
	DWORD GetNextID()
	{
		for (int idx = 1; idx < MIN_IDC_GID; idx++)
		{
			if (m_IDArray.find(idx) == m_IDArray.end())
				return idx;
		}
		return 0; //????
	}

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBrowseDir();
	afx_msg void OnBnClickedExecute();
	afx_msg void OnEnChangeDeep();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMessage(WPARAM wParam, LPARAM lParam);
	CProgressCtrl m_bProgress;
	CGridListCtrlEx m_lstRunType;
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_cboGame;
	afx_msg void OnCbnKillfocusCombo1();
	afx_msg void OnCbnCloseupCombo1();
	CComboBox m_cboClass;
	afx_msg void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
};
