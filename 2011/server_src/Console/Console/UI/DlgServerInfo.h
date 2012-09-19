#pragma once

#include "../ManagerInstance.h"
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "Skin/SkinComboBox.h"
#include "Skin/SkinButton.h"
#include "Skin/SkinRadioBox.h"
#include "Skin/SkinLabel.h"
#include "Skin/SkinEdit.h"
#include "Skin/SkinListCtrl.h"

// CDlgServerInfo 对话框

class CDlgServerInfo
	: public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgServerInfo)

public:
	CDlgServerInfo(CWnd* pParent);   // 标准构造函数
	virtual ~CDlgServerInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_SERVER_INFO };

private:
	CWnd *parent_;

	CTreeCtrl wndTreeArea_;


	i8desk::ui::SkinRadioBox wndRadioPoll_;
	i8desk::ui::SkinRadioBox wndRadioAreaPriority_;
	
	i8desk::ui::SkinButton btnModify_;
	i8desk::ui::SkinButton btnDel_;


	i8desk::ui::SkinLabel wndTipDefaultIP_;
	i8desk::ui::SkinLabel wndServerName_;
	i8desk::ui::SkinLabel wndServerMgr_;

	i8desk::ui::SkinComboBox wndComboDefaultIP_;
	i8desk::ui::SkinComboBox wndComboSvrName_;

	i8desk::ui::SkinListCtrl wndListServers_;

	i8desk::data_helper::ServerTraits::VectorType curServers_;


	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr btnArea_[3];
	ATL::CImage outboxLine_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	bool _CheckData(i8desk::data_helper::ServerTraits::ValueType &val);
	bool _ModifyPriorityArea(LPCTSTR svrName);
	void _AddComboBoxIP(LPCTSTR IP);
	void _ShowView();
	void _ReadServer();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnBnClickedButtonServerInfoModify();
	afx_msg void OnBnClickedButtonServerInfoDel();
	afx_msg void OnBnClickedRadioServerInfoPoll();
	afx_msg void OnBnClickedRadioServerInfoDynamic();
	afx_msg void OnBnClickedRadioServerInfoPriorityArea();

	afx_msg void OnLvnItemchangedListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboSvrName();


};
