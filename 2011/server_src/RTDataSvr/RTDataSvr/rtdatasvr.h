#ifndef RTDATASVR_H
#define RTDATASVR_H

#include "../../../include/frame.h"

#include "container.h"
#include "tablemgr.h"
#include "DBTrigger.h"
#include "i8mp.h"
#include "LogHelper.h"
#include "globalfun.h"
#include "../../../include/TaskInfo.h"
#include "../../../include/win32/Filesystem/FileOperator.hpp"

#ifdef USED_I8_FILECACHE
#include "filecache.h"
#endif //USED_I8_FILECACHE

#include <vector>



#define DUMP_I8_MP(MP) \
		do { \
		std::stringstream ss; \
		MP.dump(ss); \
		Log((LM_DEBUG, _T("%s\n"), ss.str().c_str())); \
		} while (0)


#define CLIENT_UDP_PORT     17901
#define CONTROL_UDP_PORT    17900

#define BLOCK_SIZE          (1024*63)
#define UDP_BLOCK_SIZE          (1024)

namespace i8desk {

//客户连接
class NetClient
{
	NetClient(INetLayer *pNetLayer, SOCKET sck, DWORD dwIP)
		: m_pNetLayer(pNetLayer)
		, m_sck(sck)
		, m_dwIP(dwIP)
		, m_type(OBJECT_UNKOWN)
		, m_cmd(0)
		, m_NextIconGid(0)
#ifdef USED_I8_FILECACHE
		, m_dwOffset(0)
#endif //USED_I8_FILECACHE
		, m_hFile(INVALID_HANDLE_VALUE)
		, m_dwIndex(0)
		, m_dwBlockCount(0)
		, m_dwRemainSize(0)
		, m_online(0)
	{
		memset(m_szName, 0, sizeof(m_szName));
	}
	NetClient(const NetClient&);
	NetClient& operator=(const NetClient&);
public:
	static NetClient* CreateObj(INetLayer *pNetLayer, SOCKET sck, DWORD dwIP)
	{
		try 
		{
			return new NetClient(pNetLayer, sck, dwIP);
		}
		catch (...) 
		{
			return 0;
		}
		
	}

	~NetClient()
	{
		if (m_hFile != INVALID_HANDLE_VALUE) 
			CloseFile();
	}
	
	bool online(void) const { return m_online > 0; }
	void connect(void) { m_online++; }
	void disconnect(void) { m_online--; }

	//连接的基本属性
	INetLayer *GetNetLayer(void) const { return m_pNetLayer; }
	SOCKET GetSocket(void) const { return m_sck; }

	void SetName(const TCHAR *name) { utility::Strcpy(m_szName, name); }
	const TCHAR *GetName(void) const { return m_szName; }

	DWORD GetIP(void) const { return m_dwIP; }
	const char *GetIPAddress(void) const 
	{
		in_addr in;
		in.s_addr = m_dwIP;
		return inet_ntoa(in);
	}

	void SetType(int type) { m_type  = type; }
	int GetType(void) const { return m_type; }

	//当前可能导致多帧的命令码
	void SetCurrentCmd(int cmd) { m_cmd = cmd; }
	void ClearCurrentCmd(void) { SetCurrentCmd(0); }
	int GetCurrentCmd(void) const { return m_cmd; }

	//多帧传送图标数据的状态
	void SetNextIconGid(uint32 gid) { m_NextIconGid = gid; }
	uint32 GetNextIconGid(void) const { return m_NextIconGid; }
	bool HasNextIconGid(void) const { return m_NextIconGid != 0; }

	//文件操作状态
	void ClearIndex(void) { m_dwIndex = 0; }
	void IncreaseIndex(void) { m_dwIndex++; }

	bool HasRemainBlock(void) const { return m_dwIndex < m_dwBlockCount; }
	void SetBlockCount(DWORD dwBlockCount) { m_dwBlockCount = dwBlockCount; }
	void ClearBlockCount(void) { SetBlockCount(0); }
	DWORD GetBlockCount(void) const { return m_dwBlockCount; }

	void SetRemainSize(DWORD dwRemainSize) { m_dwRemainSize = dwRemainSize; }
	DWORD GetRemainSize(void) const { return m_dwRemainSize; }
	
	//文件操作
	DWORD GetFileSize(LPCTSTR lpFileName);
	ulong GetFileCrc(LPCTSTR lpFileName);
	ULONGLONG GetFileLastWriteTime(LPCTSTR lpFileName);

	BOOL OpenFile(
	    LPCTSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes);
	void CloseFile(void);
	BOOL ReadFileData(BYTE *pbuf, DWORD len);
	BOOL WriteFileData(BYTE *pbuf, DWORD len);

private:
	int m_online;

	INetLayer *m_pNetLayer;
	SOCKET m_sck;
	DWORD m_dwIP;
	TCHAR m_szName[MAX_NAME_LEN];
	int m_type;

	int m_cmd;

	uint32 m_NextIconGid;

#ifdef USED_I8_FILECACHE
	CacheFile_Ptr m_CacheFile;
	DWORD m_dwOffset;
#endif //USED_I8_FILECACHE
	HANDLE m_hFile;
	DWORD m_dwIndex;//发送位置
	DWORD m_dwBlockCount;//下载的文件分块数
	DWORD m_dwRemainSize;//剩余字节数

	DECLARE_I8_NEW_DELETE_OP()
};
typedef std::tr1::shared_ptr<NetClient> NetClient_Ptr;


class ZDBTrigger;
class ZDBWriter;
class ZDataCache;
class ZFileCacheMgr;
class ZClientSoftMonitor;
class CCmdHandle10;
class ZIconTable;

namespace ballance
{
	class ServerLoadBalance;
}


//插件类
class CRTDataSvr :
	public ISvrPlug
{
public:
	CRTDataSvr(void);
	virtual ~CRTDataSvr(void);

//Release Method
	void Release();

//ISvrPlug interface 
	LPCTSTR GetPlugName();
	DWORD  	GetVersion();
	LPCTSTR GetCompany();
	LPCTSTR GetDescription();
	DWORD   GetListPort();
	bool	Initialize(ISvrPlugMgr* pPlugMgr);
	bool	UnInitialize();
	DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);

//INetLayerEvent interface
	void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param);
	void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param); 
	void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param);
	void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param);

//	
	ZFileCacheMgr *GetFileCache(void) { return m_pFileCache; }

private:
	void RemoveVDiskStatus(ulong ip);
	void RemoveServerStatus(ulong ip);
	bool GetLocalIps(std::vector<ulong> &IPs);

	void SendClientStateMsgToCon(DWORD dwIp, BOOL bOnLine, const TCHAR *lpszName);
	void SetConsoleIP(ulong ip);
	//ulong GetConsoleIP(void){ I8_GUARD(ZGuard, ZSync, m_ConsoleIPLock); return m_ulConsoleIP; }
	BOOL GetServerName(LPTSTR lpBuffer, LPDWORD nSize);

	DWORD LastConsole( DWORD dwIp);
	void SendConOffLineNotify(const DWORD& dwIp);

	bool IsFirstReportHardware(const db::tClient &client);
	bool IsHardwareChange(const db::tClient &newclient, const db::tClient &oldclient, uint64 &mask);

private:
	ZSync m_ConsoleIPLock;
	ulong m_ulConsoleIP;
	std::vector<db::tClient> ClientHDChanged_;

//网络命令处理函数
public:
	ulong GetConsoleIP(void){ I8_GUARD(ZGuard, ZSync, m_ConsoleIPLock); return m_ulConsoleIP; }

	void Cmd_Nak(INetLayer *pNetLayer, SOCKET sck, pkgheader &header, const TCHAR *lpszError = _T("错误的命令"));
	void Cmd_Unknown(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_Game_Req_Icons_Idx(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_Req_Icons_data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_GetIdxs(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_GetSome(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_RepClick(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_ConExeSql(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_IconRefresh(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_ConToClient(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Game_ClientToCon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Business_InfoEx(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_PlugTool_GetIdxs(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_PlugTool_GetSome(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Report_ConnectSvrIP(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Client_GetDelGame(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Client_RptGameStatus(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);


	// 控制台获取主服务所有信息
	void Cmd_Con_SaveHardWare(INetLayer*, SOCKET, LPCSTR, CPkgHelper &, pkgheader &);
	void Cmd_Con_NotifySmartClean(INetLayer*, SOCKET, LPCSTR, CPkgHelper &, pkgheader &);
	void Cmd_Con_CheckSpace(INetLayer*, SOCKET, LPCSTR, CPkgHelper &, pkgheader &);

	void Cmd_VDisk_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_VDisk_GetUpdateFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_VDisk_GetUpdateFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_SysOpt_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Favorite_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Boot_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Boot_DelList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	void Cmd_Clirpt_ClientInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Clirpt_Hardware(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Svrrpt_ServerInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Svrrpt_ServerInfo_Ex(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	void Cmd_Vdsvr_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Vdsvr_ReqStatus(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Vdsvr_VDiskClient(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	void Cmd_CliUpt_DownFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_CliUpt_DownFile(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_CliUpt_UpLoad(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_CliUpt_GetFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	void Cmd_CliUpt_DownFile_Start(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
//	void Cmd_User_Download_Start(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_DownFile_Next(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_Game_Icon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_Validate(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Sysopt_Syntime(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_NotifyCon_HardwareChange(const db::tClient &client, uint64 mask);

	// 保存缓存数据到数据库
	void Cmd_Notify_Save_Data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	//获取菜单皮肤列表
	void Cmd_Get_BarSkins(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Update_Client_Index(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_GetInfo_From_GameUpdateSvr(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_SetClassIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	void Cmd_Con_GetAll_GameIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_GetAll_ClassIcon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_UserInfoModify(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	
	void Cmd_Get_Server_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Get_VDisk_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Reload_Cache_File(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	//响应网络对表的操作
	void ResponseRecordset(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void InsertRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header);
	void UpdateRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header);
	void DeleteRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header);
	void EnsureRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header);
	void ComulateRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage, pkgheader &header);
	
	// 控制台插件中心的操作命令
	void Cmd_Con_PlugToolOperate(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_PlugToolIcon(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	// 控制台监控首页的状态统计
	void Cmd_Con_MonitorStatus(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_StatisticStatus(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_Con_MonitorDeleteFile(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	
	// 控制台GameMgr的操作命令
	void Cmd_GameMgr_TaskOperation(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_GameMgr_NotifyRefreshPL(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_GameMgr_NotifyServerMedical(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void Cmd_GameMgr_NotifyDeleteFile(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void SendConsoleNotify(i8desk::TaskNotifyType taskType, i8desk::TaskNotify notify, long gid, size_t err, DWORD &ip);
	 
	// 客户端得到游戏扩展信息版本号
	void Cmd_Businisss_GetGameExVer(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	// 控制台对UpdateSvr操作
	void Cmd_UpdateSvr_Operate(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);

	//多帧报文的后续帧发送
	void SendNextFileBlock(NetClient_Ptr pClient, INetLayer *pNetLayer, SOCKET sck);
	void SendNextIconPackage(NetClient_Ptr pClient, uint32 NextGid, INetLayer *pNetLayer, SOCKET sck);

	void Cmd_GetClientVersion(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header);
	void GetBarSkins(std::vector<i8desk::db::tBarSkin>& BarSkins);

	void EnsureStatus(uint32 table);

private:
	void StatisticGame_(CPkgHelper &out);
	void StatisticClient_(CPkgHelper &out);
	void StatisticUpdateGame_(CPkgHelper &out);
	void StatisticSyncGame_(CPkgHelper &out);
	void StatisticTask_( CPkgHelper &out );
	void StatisticServer_( CPkgHelper &out );
	void StatisticLogRptStatus_( CPkgHelper &out );

	void WriteUpdateGameStatus_(int  type);
	void WriteMainServer();
	void WriteServer(ulong ip);

private:

	int m_RecordLogMasks; //记录文件的日志类型掩码
	int m_ReportLogMasks; //报告到监控界面的的日志类型掩码
#ifndef NDEBUG
	int m_DisplayLogMasks; //显示的日志类型掩码
	ZSync	m_OutputLock;
#endif




	//ILogger   *m_pLogger;
	IDatabase *m_pDatabase;


	//实时数据表
	ZTableMgr *m_pTableMgr;

	//数据表触发器
	ZDBTrigger *m_pDBTrigger;
	
	//数据写入者
	ZDBWriter *m_pDBWriter;

	//缓冲数据
	ZDataCache *m_pDataCache;

	//缓冲文件
	ZFileCacheMgr *m_pFileCache;

	//客户端软件版本监视器
	ZClientSoftMonitor *m_pClientSoftMonitor;

	// 动态负载
	std::auto_ptr<ballance::ServerLoadBalance> balance_;

public:
	HMODULE		m_hRemoteCtrl;


	std::vector<i8desk::db::tBarSkin> BarSkins;

	ISvrPlugMgr  *m_pPlugMgr;
	//2010命令版本兼容
	CCmdHandle10  *m_pCmd2010;

	// 处理获取主服务状态信息
	//std::auto_ptr<ServerStatus> serverStatus_;

	//处理图标数据
	ZIconTable *m_pIconTable;

	//客户端连接列表  
	typedef ZAssociatedContainer<SOCKET, NetClient_Ptr,
		std::map<SOCKET, NetClient_Ptr>, ZSync> NetClientList;
	NetClientList	m_ClientList;

	//从服务链接列表
	typedef ZAssociatedContainer<DWORD, stdex::tString,
		std::map<DWORD, stdex::tString>, ZSync> ServerList;
	ServerList	m_SvrList;
};

//肯定应答
#define ACK() \
	do { \
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(1024); \
		CPkgHelper outpackage(lpSendPackage, 1024, header.Cmd,OBJECT_UNKOWN); \
		outpackage << 1L; \
		pNetLayer->SendPackage(sck, lpSendPackage, outpackage.GetLength());  \
	} while (0)

//肯定应答并返回
#define ACK_RETURN() \
	do { \
		ACK(); return; \
	} while (0)

//带一个消息的否定应答
#define NAK(MSG) \
	do { \
		stdex::tString func = CA2T(__FUNCTION__); \
		Log(LM_WARNING, _T("%s:%s\n"), func.c_str(), MSG); \
		Cmd_Nak(pNetLayer, sck, header, MSG); \
	} while (0)

//带一个消息的否定应答并返回
#define NAK_RETURN(MSG) \
	do { \
		NAK(MSG); return; \
	} while (0)


//带一个消息的否定应答并跳转
#define NAK_GOTO(MSG, LABLE) \
	do { \
		NAK(MSG); goto LABLE; \
	} while (0)

} //namespace i8desk



#endif //RTDATASVR_H
