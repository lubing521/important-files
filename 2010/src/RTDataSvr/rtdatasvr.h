#ifndef RTDATASVR_H
#define RTDATASVR_H

#include "../../include/plug.h"
#include "../../include/Extend STL/container.h"
#include "tablemgr.h"
#include "../../include/i8mp.h"
#include "../../include/i8logmsg.h"
#include "globalfun.h"

#ifdef USED_I8_FILECACHE
#include "filecache.h"
#endif //USED_I8_FILECACHE

#define DUMP_I8_MP(MP) \
		do { \
		std::stringstream ss; \
		MP.dump(ss); \
		I8_DEBUG((LM_DEBUG, I8_TEXT("%s\n"), ss.str().c_str())); \
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
		I8_TRY {
			return new NetClient(pNetLayer, sck, dwIP);
		} I8_CATCHALL {
		} I8_ENDTRY;
		return 0;
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

	void SetName(const char *name) { SAFE_STRCPY(m_szName, name); }
	const char *GetName(void) const { return m_szName; }

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
	char m_szName[MAX_NAME_LEN];
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



class ZDBWriter;
class ZDataCache;
class ZFileCacheMgr;
class ZClientSoftMonitor;

//插件类
class CRTDataSvr :
	public IPlug
#ifdef USE_ACE_LIB
		, public ACE_Log_Msg_Backend
#else
		, public CI8Log
#endif 
{
public:
	CRTDataSvr(void);
	virtual ~CRTDataSvr(void);

//Release Method
	void Release();

//CI8Log interface
	void i8log(I8_Log_Priority priority, const I8_TCHAR *format, ...);

//IPlug interface 
	LPCTSTR GetPlugName();
	DWORD  	GetVersion();
	LPCTSTR GetCompany();
	LPCTSTR GetDescription();
	DWORD   GetListPort();
	bool	Initialize(IPlugMgr* pPlugMgr);
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
	void SendClientStateMsgToCon(DWORD dwIp, BOOL bOnLine, const char *lpszName);
	void SetConsoleIP(ulong ip);
	ulong GetConsoleIP(void){ I8_GUARD(ZGuard, ZSync, m_ConsoleIPLock); return m_ulConsoleIP; }
private:
	ZSync m_ConsoleIPLock;
	ulong m_ulConsoleIP;

//网络命令处理函数
public:
	void Cmd_Nak(INetLayer *pNetLayer, SOCKET sck, _packageheader &header, const char *lpszError = "错误的命令");
	void Cmd_Unknown(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	void Cmd_Game_Req_Icons_Idx(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_Req_Icons_data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_GetIdxs(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_GetSome(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_IconList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_RepClick(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_ConExeSql(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_IconRefresh(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_ConToClient(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_ClientToCon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_VDisk_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_VDisk_GetUpdateFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_VDisk_GetUpdateFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	void Cmd_SysOpt_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Favorite_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Boot_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Boot_DelList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_User_Register(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_User_LogIn(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_User_ModPwd(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	void Cmd_User_Upload(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_User_Download(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_User_Delete(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_Clirpt_ClientInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Clirpt_Hardware(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Svrrpt_ServerInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Svrrpt_ServerInfo_Ex(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_Vdsvr_GetList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Vdsvr_ReqStatus(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_CliUpt_DownFileInfo(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_CliUpt_DownFile(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_CliUpt_UpLoad(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_CliUpt_GetFileList(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Get_All_Clients(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_CliUpt_DownFile_Start(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_User_Download_Start(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_DownFile_Next(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	void Cmd_Game_Icon(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	void Cmd_Validate(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Sysopt_Syntime(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Game_CliExeSql(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	
	// 保存缓存数据到数据库
	void Cmd_Notify_Save_Data(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	//获取菜单皮肤列表
	void Cmd_Get_BarSkins(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Update_Iconlist_Version(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Update_Client_Index(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Con_GetInfo_From_GameUpdateSvr(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	void Cmd_Get_Server_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Get_VDisk_Config(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);
	void Cmd_Reload_Cache_File(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header);

	//响应网络对表的操作
	void ResponseRecordset(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header);
	void InsertRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header);
	void UpdateRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header);
	void DeleteRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header);
	void EnsureRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header);
	void ComulateRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage, _packageheader &header);

	//多帧报文的后续帧发送
	void SendNextFileBlock(NetClient_Ptr pClient, INetLayer *pNetLayer, SOCKET sck);
	void SendNextIconPackage(NetClient_Ptr pClient, uint32 NextGid, INetLayer *pNetLayer, SOCKET sck);

private:

	int m_RecordLogMasks; //记录文件的日志类型掩码
	int m_ReportLogMasks; //报告到监控界面的的日志类型掩码
#ifndef NDEBUG
	int m_DisplayLogMasks; //显示的日志类型掩码
	ZSync	m_OutputLock;
#endif

	IPlugMgr  *m_pPlugMgr;
	ILogger   *m_pLogger;
	IDatabase *m_pDatabase;


	//实时数据表
	ZTableMgr *m_pTableMgr;
	
	//数据写入者
	ZDBWriter *m_pDBWriter;

	//缓冲数据
	ZDataCache *m_pDataCache;

	//缓冲文件
	ZFileCacheMgr *m_pFileCache;

	//客户端软件版本监视器
	ZClientSoftMonitor *m_pClientSoftMonitor;

	//客户端连接列表  
	typedef ZAssociatedContainer<SOCKET, NetClient_Ptr,
		std::map<SOCKET, NetClient_Ptr>, ZSync> NetClientList;
	NetClientList	m_ClientList;
};

//肯定应答
#define ACK() \
	do { \
		LPSTR lpSendPackage = pNetLayer->GetIMemPool()->Alloc(1024); \
		CPackageHelper outpackage(lpSendPackage, 1024); \
		outpackage << 1L; \
		header.Length=outpackage.GetOffset(); \
		outpackage.pushPackageHeader(header); \
		pNetLayer->SendPackage(sck, lpSendPackage, header.Length);  \
	} while (0)

//肯定应答并返回
#define ACK_RETURN() \
	do { \
		ACK(); return; \
	} while (0)

//带一个消息的否定应答
#define NAK(MSG) \
	do { \
		I8_ERROR((LM_WARNING, I8_TEXT("%s:%s\n"), __FUNCTION__, MSG)); \
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
