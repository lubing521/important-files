#pragma once;

#include "afxext.h"
#include "UI.h"

class CFormCtrl : public CFormView
{
public:
	enum 
	{
		ID_BTN_MIN		= 0,

		ID_JKZS_CXTJ	= 0,
		ID_JKZS_VIEW,
		
		ID_SERVER_SVR1,
		ID_SERVER_SVR2,
		ID_SERVER_SVR3,
		ID_SERVER_SVR4,

		ID_CLIENT_CLITOTAL,
		ID_CLIENT_ONLINECLI,
		ID_CLIENT_HYNOTUSE,
		ID_CLIENT_SAFENOTUSE,
		ID_CLIENT_IENOTPROT,
		ID_CLIENT_DOTNOTUSE,
		ID_CLIENT_HARDCHANGE,

		ID_GAME_CENTERNUM,
		ID_GAME_NEWADDNUM,
		ID_GAME_LOCALNUM,
		ID_GAME_VIRTUAL,
		ID_GAME_LOCALUPT,
		ID_GAME_DIRECTRUN,

		ID_GAME_MATCHNUM,
		ID_GAME_HAVEUPT,
		ID_GAME_THIRDLIST,
		ID_GAME_THIRDSPEED,
		ID_GAME_INNERLIST,
		ID_GAME_INNERSPEED,
		
		ID_BTN_MAX
	};
	typedef struct tagDisk
	{
		TCHAR Driver;			//分区的盘符
		float total;			//分区的总大小
		float userd;			//己经使用的大小
		float speed;			//读取速度
	}tagDisk;

	typedef struct tagServer
	{
		TCHAR HostName[MAX_PATH];	//主机名称
		int MainServer;			//主服务
		int ThirdLayerUpdate;	//三层更新
		int VirtualDisk;		//虚拟磁盘
		int InnerUpdate;		//内网更新
		int ShopCity;			//商城收银端
		int cpuusage;			//cpu占用率
		int memoryusage;		//内存占用率
		int networkusage;       //网络使用率
		int LastBackupDatabaseTime; //上次备份数据库时间 
		tagDisk Disk[6];
	}tagServer;

	typedef struct tagClientInfo
	{
		int CliNum;			//客户机总数.
		int CliOnlineNum;	//在线客户机数
		int NotProtNum;		//还原未保护数
		int NotSafeNum;		//安全中心未启用数
		int NotIeNum;		//IE未保护数
		int NotDogNum;		//防狗未启用数
		int HardChgNum;		//硬件改动数
	}tagClientInfo;

	typedef struct tagGameInfo
	{
		TCHAR CGameNum[MAX_PATH];			//中心资源数
		TCHAR CAddGameNum[MAX_PATH];		//近期中心新增资源数
		TCHAR LGameNum[MAX_PATH];			//本地己有资源数
		TCHAR ConVirRunGameNum[MAX_PATH];	//配置为虚拟盘运行的资源数
		TCHAR ConLocRunGameNum[MAX_PATH];	//配置为本地更新运行的资源数
		TCHAR ConRunNotUptGameNum[MAX_PATH];//配置为不更新，直接运行的资源数
		TCHAR MatchGameNum[MAX_PATH];		//本地己匹配到中心资源数
		TCHAR HaveUptGameNum[MAX_PATH];		//有更新的资源数
		TCHAR ThirdDwnListNum[MAX_PATH];	//三层更新下载队列数
		TCHAR ThirdDwnSpeed[MAX_PATH];		//三层下载总速度
		TCHAR InnerUptListNum[MAX_PATH];	//内网更新队列数
		TCHAR InnerUptSpeed[MAX_PATH];		//内网更新总速度
	}tagGameInfo;

public:
	CFormCtrl(UINT nIDTemplate);
	~CFormCtrl();
	BOOL Create(CWnd* parent);
protected:
	//更新各种状态
	void UpdateJKZS(int nJK, const CString& strLastBackupTime);
	void UpdateServer(tagServer* pServer, int nCount);
	void UpdateClient(tagClientInfo* pInfo);
	void UpdateGameInfo(tagGameInfo* pInfo);

	//各种按钮事件。
	virtual void HandleEvent(DWORD id, LPVOID p) = 0;
protected:
	int m_nJKZS;
	CString m_strLastBackupTime;
	int m_SvrCount;
	int m_nCurSvr;
	tagServer m_pServer[4];
	tagClientInfo m_cliInfo;
	tagGameInfo	 m_gameInfo;
	CFont m_font, m_fontbold, m_fontunderline;
	CButtonEx m_btnCXTJ, m_btnViewTJ;
	CButtonEx m_btnServer[4];
	CButtonEx m_btnCliStatus[7];
	CButtonEx m_btnGameStatus[12];

	void PaitJKZS(CDC* pDC);
	void PaitServer(CDC* pDC);
	void PaitClient(CDC* pDC);
	void PaitGame(CDC* pDC);

	void UpdateJKZS(CDC* pDC);
	void UpdateServer(CDC* pDC);
	void UpdateDiskUsage(CDC* pDC, const CRect& rx, TCHAR chDrv, float total, float used);
	void UpdateClient(CDC* pDC);
	void UpdateGame(CDC* pDC);

	virtual void OnDraw(CDC* pDC);
	virtual void PostNcDestroy();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickButton(UINT id);
};
			
