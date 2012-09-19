#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CPLModifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CPLModifyDlg)

public:
	CPLModifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPLModifyDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PLMODIFY };
	i8desk::GameInfoVector m_GameInfos;
protected:
	i8desk::AreaInfoMap  m_AreaInfos;
	i8desk::DefClassMap  m_ClassInfos;
	i8desk::VDiskInfoMap m_VDiskInfos;

	std::string GetClassGuid();
	std::string GetAIDByName(std::string& Name);
	BOOL ParaseAreaRTTexT(CString szText, i8desk::tagGameRunType& RtType);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CGridListCtrlEx m_lstRunType;
	CComboBox m_cboClass;
	CComboBox m_cboAutoUpt;
	CComboBox m_cboPriority;
	CComboBox m_cb3upPriority;
	CButton m_btnToolbar;
	CButton m_btnDeskLnk;
	CButton m_btnEnModRunType;
	CButton m_btnEnModOther;
	CButton	m_btnSvrPath;
	CButton	m_btnCliPath;
	CButton	m_btnClass;
	CButton	m_btn3up;
	CButton	m_btnPriority;
	CButton	m_btnDownPriority;

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckCtrl();
};
