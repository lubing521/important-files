#pragma once


// CDropListCtrl


class CDropListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CDropListCtrl)



public:
	char m_szPath[MAX_PATH];
	CDropListCtrl();
	virtual ~CDropListCtrl();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	bool m_bShowLst;
	vector<string>m_vecFileLst;
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnShowlst();

	afx_msg void OnPostfile();
	afx_msg void OnPostdir();
	afx_msg void OnDowndfile();
	afx_msg void OnDeletefile();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnVlist();
	afx_msg void OnVicon();
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


