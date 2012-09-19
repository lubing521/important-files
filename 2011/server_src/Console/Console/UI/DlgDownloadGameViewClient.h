#pragma once
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomListCtrl.h"


// CDlgDownloadGameViewClient 对话框

class CDlgDownloadGameViewClient : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgDownloadGameViewClient)

public:
	CDlgDownloadGameViewClient( long gid, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDownloadGameViewClient();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOWNLOAD_VIEWGAMECLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void _ShowView();

private:
	CCustomListCtrl wndListView_;
	long gid_;
	ATL::CImage outboxLine_;
	i8desk::data_helper::ViewClientTraits::VectorType curviewClients_;


public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnGetdispinfoListViewGameClient(NMHDR *pNMHDR, LRESULT *pResult);

};
