#pragma once

#include "resource.h"
#include "afxcmn.h"

// CGameListDlg 对话框

class CGameListDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameListDlg)

public:
	enum {
		MATCH, TODAYUPDATE, VALUENODOWN, AUTOUPDATE, MAYDELETE, IDCADDNODOWN,
		RTLOCALUPDATE, RTVDISK, RTDIRECTRUN,
	};
	CGameListDlg(
		i8desk::DefClassMap& DefClasses,
		i8desk::GameInfoMap& Games,
		int selector, 
		int partition = 0, //for delete game
		CWnd* pParent = NULL
		);   
	virtual ~CGameListDlg();

// 对话框数据
	enum { IDD = IDD_GAME_LIST };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
	
	CString GetGameClass(const i8desk::GameInfo& GameInfo);
	size_t GetSelectedGames(std::vector<DWORD>& games);

private:

	int m_selector;
	int m_partition;
	i8desk::DefClassMap m_DefClasses;
	i8desk::GameInfoVector m_Games;
	CListCtrl m_list;
public:
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAll();
	afx_msg void OnBnClickedClear();
};
