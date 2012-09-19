#pragma once
#include "../I8UIFrame/NonFrameChildDlg.h"
#include "../CustomControl/CustomListCtrl.h"


// CDlgClientClientViewGame 对话框

class CDlgClientClientViewGame : public CNonFrameChildDlg
{
	DECLARE_DYNAMIC(CDlgClientClientViewGame)

public:
	CDlgClientClientViewGame( unsigned long IP, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgClientClientViewGame();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLIENT_CLIENTVIEWGAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void _ShowView();

private:
	CCustomListCtrl wndListView_;
	unsigned long IP_;
	ATL::CImage outboxLine_;
	i8desk::data_helper::ViewGameTraits::VectorType curviewGames_;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLvnGetdispinfoListViewGameClient(NMHDR *pNMHDR, LRESULT *pResult);

};
