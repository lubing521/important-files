#pragma once

#include "resource.h"
#include "afxcmn.h"

// CSCArpProtectDlg 对话框

class CSCArpProtectDlg : public CDialog
{
	DECLARE_DYNAMIC(CSCArpProtectDlg)

public:
	CSCArpProtectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSCArpProtectDlg();

// 对话框数据
	enum { IDD = IDD_SC_ARP_PROTECT };
	void SetConfig(BOOL bIsOpen,
		BOOL bIsAutoGetGatewayAddr,
		LPCTSTR lpIP, 
		LPCTSTR lpMAC);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnApplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedEnable();
	afx_msg void OnBnClickedAutoGetGateway();
	afx_msg void OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeMacaddress();
	afx_msg void OnBnClickedSaveConfig();
	CString m_strGatewayMac;
	CIPAddressCtrl m_IPCtrl;
	BOOL m_enable;
	BOOL m_bAutoGetAddr;
};
