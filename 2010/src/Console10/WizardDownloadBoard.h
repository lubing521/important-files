#pragma once
#include "afxcmn.h"

class CClassListCtrl : public CGridListCtrlEx
{
public:
	CClassListCtrl(BOOL& bModify) : m_bModify(bModify) {}
	void SetRootPath(const CString& str) { m_strRootPath = str; }

	virtual bool OnDisplayCellTooltip(int nRow, int nCol, CString& text)
	{
		if (!CGridListCtrlEx::OnDisplayCellTooltip(nRow, nCol, text))
			return false;

		if (nCol == 2) 
		{
			ASSERT(!text.IsEmpty());
			if (text[0] == _T('(')) 
				text += _T("注意：此类别的下载路径用户尚未设置过，这是系统的默认下载路径");
		}
		
		return true;
	}

	virtual bool OnDisplayCellColor(int nRow, int nCol, COLORREF& text, COLORREF& background)
	{
		if (nCol != 2)
			return CGridListCtrlEx::OnDisplayCellColor(nRow, nCol, text, background);

		CString strText = GetItemText(nRow, nCol);
		ASSERT(!strText.IsEmpty());
		if (strText[0] == _T('(')) {
			text = RGB(128, 128, 128);
			return true;
		}

		return false;
	}

protected:
	virtual CWnd* OnTraitEditBegin(CGridColumnTrait* pTrait, CWnd* pEditor, int nRow, int nCol)
	{
		m_strOld;
		pEditor->GetWindowText(m_strOld);
	
		ASSERT(!m_strOld.IsEmpty());
		if (m_strOld[0] == _T('(')) {
			m_strEdit = m_strOld.Mid(1, m_strOld.GetLength() - 2);
			pEditor->SetWindowText(m_strEdit);
		}
		else 
			m_strEdit = m_strOld;

		return pEditor;
	}

	virtual bool OnTraitEditComplete(CGridColumnTrait* pTrait, CWnd* pEditor, LV_DISPINFO* pLVDI)
	{
		CString strNew;
		pEditor->GetWindowText(strNew);

		if (strNew == m_strEdit) {
			pEditor->SetWindowText(m_strOld);
			return false;
		}

		if (!i8desk::IsValidDirName((LPCSTR)strNew)) {
			strNew += _T("不是有效的路径");
			AfxMessageBox(strNew);
			return false;
		}

		m_bModify = TRUE;

		return true; 
	}

private:
	CString m_strRootPath;
	CString m_strOld;
	CString m_strEdit;

	BOOL& m_bModify;
};

// CWizardDownloadBoard 对话框

class CWizardDownloadBoard : public CDialog
{
	DECLARE_DYNAMIC(CWizardDownloadBoard)

public:
	CWizardDownloadBoard(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWizardDownloadBoard();

// 对话框数据
	enum { IDD = IDD_WIZARD_DOWNLOAD };
	BOOL UpdateRunTypeList();
	BOOL UpdateClassList();

protected:
	i8desk::AreaInfoMap  m_AreaInfos;
	i8desk::VDiskInfoMap m_VDiskInfos;
	i8desk::DefClassMap m_DefClasses;

	CString GetAreaRunType(LPCSTR aid, const CString& szAreaRunType);
	CString ParaseAreaRunType(const CString& AreaName, const CString& RunType);
	void WriteP2PConfig(void);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeInitdir();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedCheckTimeupt();
	afx_msg void OnBnClickedSelectMsctmpdir();

	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bModify;
	BOOL m_bModifyClass;
	CGridListCtrlEx m_lstRunType;
	CListCtrl m_lstCtrl;
	CButton m_btnTimeUpt;
	CClassListCtrl m_lcClass;
public:
	afx_msg void OnCbnSelchangeJoinType();
	afx_msg void OnCbnSelchangeTasknum();
};