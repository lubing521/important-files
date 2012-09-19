#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ConsoleDlg.h"


// CAddClientDlg 对话框

class CAddClientDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddClientDlg)

public:
	CAddClientDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddClientDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIENT };
	BOOL m_bAdd;
protected:
	BOOL InitDefaultNetParam();
	bool CheckValid();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void UpdateDataFromSelectedItem(int SelItemIndex);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CIPAddressCtrl m_ipFrom;
	CIPAddressCtrl m_ipTo;
	CIPAddressCtrl m_ipNetGate;
	CIPAddressCtrl m_ipMark;
	CIPAddressCtrl m_ipDNS;
	CIPAddressCtrl m_ipDNS2;
	CComboBox m_cboArea;
	afx_msg void OnDestroy();
	afx_msg void OnEnChangeTo();
	afx_msg void OnIpnFieldchangedIpfrom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeNamePre();
	afx_msg void OnBnClickedAddCli();
	afx_msg void OnBnClickedModCli();
	afx_msg void OnBnClickedRemoveCli();
	// 要添加的客户组列表
	CListCtrl m_lstCliGrp;
//	afx_msg void OnHdnItemclickListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnLvnInsertitemListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnItemchangedListCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_editStartNum;
	CEdit m_editMachineNum;
	CEdit m_editEncodeSize;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
