#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "../EasySize.h"
#include "../CustomControl/CustomListCtrl.h"
#include "../CustomControl/CustomButton.h"

#include "../ui/WindowManager.h"
#include "../Data/DataHelper.h"

// CDlgPushGame 对话框

class CDlgPushGame 
	: public i8desk::ui::BaseWnd
{
	DECLARE_EASYSIZE
	DECLARE_DYNAMIC(CDlgPushGame)

public:
	CDlgPushGame(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPushGame();

// 对话框数据
	enum { IDD = IDD_DLG_PUSHGAME };

private:
	CTreeCtrl wndTree_;
	CCustomListCtrl wndListGames_;
	CCustomListCtrl wndListClients_;
	
	CCustomButton wndBtnMgr_;
	CCustomButton wndBtnTaskAdd_;
	CCustomButton wndBtnTaskDel_;
	CCustomButton wndBtnTaskMod_;

	CImageList imageTreeList_;

	typedef std::tr1::shared_ptr<ATL::CImage> ImagePtr;
	ImagePtr rightArea_[5];
	ImagePtr leftArea_[3];
	ImagePtr workOutLine_;


	stdex::tString TID_;
	i8desk::data_helper::GameTraits::VectorType localGames_;
	i8desk::data_helper::PushGameStatusTraits::VectorType pushGames_;


public:
	virtual void Register();
	virtual void UnRegister();
	virtual void OnRealDataUpdate();
	virtual void OnReConnect();
	virtual void OnAsyncData();
	virtual void OnDataComplate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	

private:
	void _ChangeList(LPCTSTR TID, const stdex::tString &param);
	void _Init();
	void _ShowView();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonClickAdd();
	afx_msg void OnButtonClickModify();
	afx_msg void OnButtonClickDelete();

	afx_msg void OnTvnSelchangedTreePushgame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListPushgameClients(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListPushgames(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnApplySelect(WPARAM, LPARAM);
};
