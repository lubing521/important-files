#pragma once

#include <vector>
#include "ui.h"
#include "afxcmn.h"

class CListBoxExWZ : public CListBox
{
	// Construction
public:
	CListBoxExWZ();
	virtual ~CListBoxExWZ();
public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
	int CompareItem(LPCOMPAREITEMSTRUCT) { return 0; }
public:
	void AddItem(UINT nIconID, LPCTSTR lpszText);
	CString GetItemText(int index) const 
	{
		ASSERT(index < (int)m_lstItemData.size());
		return m_lstItemData[index].szText;
	}
	void SetItemHeight (int nHeight);

protected:
	CBitmap m_bmListBoxExDef;
	CBitmap m_bmListBoxExSel;
	int m_nItemHeight;

	COLORREF m_crBkColor;
	COLORREF m_crTextColor;

	typedef struct tagItem
	{
		DWORD IconID;
		CString szText;
	}tagItem;

	std::vector<tagItem> m_lstItemData;
	//{{AFX_MSG(CListBoxExWZ)
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// CConfigWizard 对话框

class CConfigWizard : public CDialog
{
	DECLARE_DYNAMIC(CConfigWizard)

public:
	CConfigWizard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigWizard();

// 对话框数据
	enum { IDD = IDD_CONFIG_WIZARD };

	int m_flags; //数据改变的标志

protected:
	template<typename T> 
	void AddBoard(LPRECT lpRect, LPCTSTR desc);
	void ShowBoard(size_t idx);

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeListbox();
	afx_msg void OnBnClickedNextWizard();
	afx_msg void OnBnClickedPrevWizard();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	
	DECLARE_MESSAGE_MAP()

private:
	size_t m_iCurrentWizard;
	std::vector<CDialog *> m_wizards;

	CListBoxExWZ m_lbStep;

	CButtonEx m_bnStepDesc;
	CButtonEx m_bnBoardPlace;
};
