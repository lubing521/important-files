#pragma once
#include "resource.h"
#include "afxwin.h"
#include <vector>
#include "UI.h"

class CMonitorPage;

class CTJDlg : public CDialog
{
	DECLARE_DYNAMIC(CTJDlg)
public:
	typedef struct tagList_Item
	{
		DWORD Id;					//0表示第一级（服务器）,1表示第二级:测试的大类,2表示具体测试的项
		TCHAR Caption[100];			//标题 
		BOOL  Status;				//真表示正常，否则表示异常
		TCHAR StatusText[MAX_PATH];	//状态文本
	}tagList_Item;
private:
	class CMyListBox : public CListBox
	{
	public:
		CMyListBox();
		virtual ~CMyListBox();
	public:
		int AddLine(tagList_Item* pItem)
		{
			tagList_Item* p = new tagList_Item;
			*p = *pItem;
			m_lstData.push_back(p);
			return AddString(TEXT(""));
		}
		BOOL SetLine(DWORD nLine, tagList_Item* pItem)
		{
			if (nLine < 0 || nLine >= m_lstData.size())
				return FALSE;
			tagList_Item* p = new tagList_Item;
			*p = *pItem;
			m_lstData[nLine] = p;
			return TRUE;
		}
	public:
		virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDS);
		int CompareItem(LPCOMPAREITEMSTRUCT) { return 0; }
	private:
		CFont m_font, m_fontbold;
		std::vector<tagList_Item*> m_lstData;
	};
public:
	CTJDlg(CMonitorPage *pMonitorPage, CWnd* pParent = NULL);
	virtual ~CTJDlg();

	enum { IDD = IDD_DIALOG_TJPRT };

	BOOL m_bIsOnlyView;
public:
	int AddLine(tagList_Item* pItem)
	{
		return m_lstBox.AddLine(pItem);
	}
	BOOL SetLine(DWORD nLine, tagList_Item* pItem)
	{
		return m_lstBox.SetLine(nLine, pItem);
	}
	void SetCheckStatusText(const CString& str)
	{
		m_staStatus.SetWindowText(str);
	}
	void SetCheckProgress(int nProgress);

	void TJOver(int nTJ);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	int m_nCheckProgress;
	CFont m_fontTitle, m_fontStatus;
	CStatic m_staTitle;
	CStatic m_staStatus;
	CButtonEx m_btnProgress;
	CButtonEx m_btnCancel;
	CMyListBox m_lstBox;
	CButtonEx m_btnClose;
	UINT_PTR m_nTimerID;
	CMonitorPage *m_pMonitorPage;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
};
