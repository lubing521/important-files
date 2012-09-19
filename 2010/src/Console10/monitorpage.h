#pragma once

#include "formctrl.h"
#include "resource.h"
#include "ui.h"
#include "listctrlex.h"

class CConsoleDlg;
class CTJDlg;

class CMonitorPage : public CFormCtrl
{
public:
	CMonitorPage();
	~CMonitorPage();

	void ViewGameList(int type);
	void ViewDeleteGameList(int partition);
	void ViewClientList(int type);
	
	void NotifyServerMedical(void);

	enum { IDD = IDD_DIALOG_MONITOR_PAGE };

	void SyncServerStatus(const std::map<std::string, std::set<char> >& servers,
									const std::string& strMainServerName
									);
	void SyncHealthStatus(const std::map<std::string, std::set<char> >& servers);

	void UpdateModuleStatus(const std::string& strHostName,
		const std::string& strI8DeskSvr,
		const std::string& strI8VDiskSvr,
		const std::string& strI8UpdateSvr,
		const std::string& strDNAService,
		const std::string& strI8MallCashier,
		const std::string& strLastBackupDBTime,
		const std::string& strCPUUtilization,
		const std::string& strMemoryUsage,
		const std::string& strNetworkSendRate,
		const std::string& strNetworkRecvRate,
		const std::string& strRemainMemory
		);
	void UpdateClientStatus(const i8desk::MachineMap& Machines);
	void UpdateDiskStatus(const std::string& strHostName,
		const std::string& strPartition,
		const std::string& strCapacity,
		const std::string& strType,
		const std::string& strUsedSize,
		const std::string& strFreeSize,
		const std::string& strReadDataRate
		);
	void UpdateGameStatus(const i8desk::GameInfoMap& GameInfos);
	void UpdateDownload3upListCount(int count);
	void UpdateDownload3upTotalSpeed(float speed);
	void UpdateDownloadUpdateGameCount(int count);
	void UpdateDownloadUpdateTotalSpeed(float speed);
	
	void InvalidateMonitorData(void);
protected:
	virtual void HandleEvent(DWORD id, LPVOID p);
	
	void OnJkzsCxtj(void);
	void OnJkzsView(void);
	BOOL PerformTJ(CTJDlg *pTJDlg);
	int CalcJKZS(void);

	friend class CTJDlg;

	DECLARE_MESSAGE_MAP()
private:
	int m_nThirdDwnListNum;	//三层更新下载队列数
	float m_fThirdDwnSpeed;		//三层下载总速度
	int m_nInnerUptListNum;	//内网更新队列数
	float m_fInnerUptSpeed; 		//内网更新总速度

	size_t m_nServer;
	tagServer m_Servers[32];

	tagClientInfo m_ClientInfo;
	tagGameInfo m_GameInfo;

	//体检进度记录
	int m_nJK;
	size_t m_nCurTJServer;
	size_t m_nCurClass;
	size_t m_nCurModule;
	size_t m_nCurTJDisk;
	int m_nTJProgress;

	::__time32_t m_ServerMedicalTime;
};

