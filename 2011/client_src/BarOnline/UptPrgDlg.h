#pragma once

class CUptPrgDlg : public CDialog
{
	DECLARE_DYNAMIC(CUptPrgDlg)

public:
	CUptPrgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUptPrgDlg();

	//直接运行游戏
	static CString RunGame(tagGameInfo* pGame);
	//设置游戏
	void           SetStartGame(tagGameInfo* pGame);
	// 对话框数据
	enum { IDD = IDD_DLG_UPTPRG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnDestroy();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedBuycard();
	afx_msg void OnBnClickedVideoSkill();
	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedFavorite();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest,	UINT message);
	DECLARE_MESSAGE_MAP()

protected:
	//更新游戏
	BOOL UpdateGame(tagGameInfo* pGameInfo);
    void UpdateInfo();

private:
	HANDLE  m_hUpdate;
	bool   m_bAutoRun;	//表示是否自动启动游戏	（当鼠标按下任何键时，不自动运行，以让用户可以点其它按钮）
	bool   m_bIsUpdate;	//表示当前是否正在更新。
	bool   m_bNeedWait;	//表示是否仍己经等待固定的秒数没有。
	DWORD  m_fcCopyMode;

	I8SkinCtrl_ns::CI8HtmlView m_WebPage;
	I8SkinCtrl_ns::CI8OldButton m_btnCaption;
	I8SkinCtrl_ns::CI8OldButton m_btnClose;
	I8SkinCtrl_ns::CI8OldButton m_btnGamePlug;
    I8SkinCtrl_ns::CI8Progress m_btnProgress;
	I8SkinCtrl_ns::CI8OldButton m_btnStatus;
	I8SkinCtrl_ns::CI8OldButton m_btnTime;
	I8SkinCtrl_ns::CI8OldButton m_btnRun;
	I8SkinCtrl_ns::CI8OldButton m_btnSearch;
	I8SkinCtrl_ns::CI8OldButton m_btnBuyCard;
	I8SkinCtrl_ns::CI8OldButton m_btnVidoSkill;
	I8SkinCtrl_ns::CI8OldButton m_btnModify;
	I8SkinCtrl_ns::CI8OldButton m_btnFavorite;
	I8SkinCtrl_ns::CI8OldButton m_btnPlug;

	CImageList m_iGamePlug;
	CListCtrl  m_lstGamePlug;
	tagGameInfo m_arGameInfo[6];
	int        m_nGameCount;
	int        m_nCurrentUptIdx;
	HCURSOR    m_hPlugCursor;
public:
	afx_msg void OnNMClickListGame(NMHDR *pNMHDR, LRESULT *pResult);
};
