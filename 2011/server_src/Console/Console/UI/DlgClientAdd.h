#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomLabel.h"
#include "../CustomControl/CustomComboBox.h"
#include "../CustomControl/CustomListCtrl.h"
#include "../CustomControl/CustomEdit.h"
#include "../CustomControl/CustomButton.h"
#include "../CustomControl/CustomIPAddress.h"

// CDlgClientAdd 对话框
using i8desk::data_helper::AreaTraits;


class CDlgClientAdd : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgClientAdd)

public:
	CDlgClientAdd(const AreaTraits::ElementType *area, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgClientAdd();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIENT_ADD };

private:
	CString namePre_;
	CString nameSuffix_;
	CCustomComboBox wndComboArea_;
	CCustomIPAddress wndIPStart_;
	CCustomIPAddress wndIPStop_;
	CCustomIPAddress wndIPGate_;
	CCustomIPAddress wndIPMask_;
	CCustomIPAddress wndIPDNS_;
	CCustomIPAddress wndIPDNS2_;
	int startNum_;
	int stopNum_;
	int numLen_;
	CCustomListCtrl wndListClients_;

	CCustomEdit wndEditnamePre_;
	CCustomEdit wndEditnameSuffix_;
	CCustomEdit wndEditstartNum_;
	CCustomEdit wndEditstopNum_;
	CCustomEdit wndEditnumLen_;

	CCustomLabel	wndLabelnamePre_;
	CCustomLabel	wndLabelnameSuffix_;
	CCustomLabel	wndLabelstartNum_;
	CCustomLabel	wndLabelstopNum_;
	CCustomLabel	wndLabelnumLen_;
	CCustomLabel	wndLabelComboArea_;
	CCustomLabel	wndLabelIPStart_;
	CCustomLabel	wndLabelIPStop_;
	CCustomLabel	wndLabelIPGate_;
	CCustomLabel	wndLabelIPMask_;
	CCustomLabel	wndLabelIPDNS_;
	CCustomLabel	wndLabelIPDNS2_;
	CCustomLabel	wndLabelName_;
	CCustomLabel	wndLabelAreaSet_;
	CCustomLabel	wndLabelParamSet_;
	CCustomLabel	wndLabelNumSet_;
	CCustomLabel	wndLabelGroupList_;
	CCustomLabel	wndLabelTriger_;
	CCustomLabel	wndLabelStartNum_;

	CCustomButton	wndBtnAdd_;
	CCustomButton	wndBtnMod_;
	CCustomButton	wndBtnRemove_;
	CCustomButton	wndBtnOk_;
	const AreaTraits::ElementType *curArea_;
	ATL::CImage outboxLine_;

	struct ClientInfo
	{
		TCHAR	prefix[MAX_NAME_LEN];
		TCHAR	suffix[MAX_NAME_LEN];
		TCHAR	area[MAX_GUID_LEN];
		int		startnum;
		int		stopnum;
		int		numlen;
		unsigned long	ip;
		unsigned long	mark;
		unsigned long	netgate;
		unsigned long	dns;
		unsigned long	dns2;

		ClientInfo(){ memset(this,0,sizeof(*this)); }
	};
	typedef std::tr1::shared_ptr<ClientInfo> ClientInfoPtr;
	typedef std::vector<ClientInfoPtr> ClientInfos;
	ClientInfos clientInfos_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

private:
	void _ShowView();
	bool _CheckDatas(const ClientInfoPtr &val);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonClientAddCli();
	afx_msg void OnBnClickedButtonClientModCli();
	afx_msg void OnBnClickedButtonClientRemoveCli();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMClickListClientCligrp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListClientCligrp(NMHDR *pNMHDR, LRESULT *pResult);

};
