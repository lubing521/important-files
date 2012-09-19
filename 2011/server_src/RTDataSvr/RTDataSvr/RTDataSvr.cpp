#include "StdAfx.h"
#include "rtdatasvr.h"
#include "dbtrigger.h"
#include "dbwriter.h"
#include "datacache.h"
#include "filemonitor.h"
#include "filecache.h"
#include "LogHelper.h"
#include "2010_v/cmdhandler_2010.h"
#include "Icontable.h"
#include <iostream>
#include <algorithm>
#include "../../../include/irtdatasvr.h"
#include "../../../include/I8Type.hpp"
#include "SeverBallance.hpp"


namespace i8desk {

IMPLMENT_I8_NEW_DELETE_OP(NetClient)

//////////////////////////////////////
CRTDataSvr *g_pPlug = 0;

ISvrPlug* WINAPI CreateSvrPlug(DWORD Reserved)
{
 	if (g_pPlug == 0)
		g_pPlug = new CRTDataSvr;
	return g_pPlug;
}




/////////////////////////////////
// CRTDataSvr

CRTDataSvr::CRTDataSvr(void)
	//: m_pLogger(0)
	: m_ulConsoleIP(0)
	, m_pDatabase(0)
	, m_RecordLogMasks(LM_INFO|LM_WARNING|LM_ERROR)
	, m_ReportLogMasks(LM_INFO)
#ifndef NDEBUG
	, m_DisplayLogMasks(LM_INFO)
#endif
	, m_pTableMgr(new ZTableMgr)
	, m_pDBTrigger(new ZDBTrigger(m_pTableMgr))
	, m_pIconTable(new ZIconTable)
	, m_pDBWriter(new ZDBWriter(m_pTableMgr))
	, m_pDataCache(new ZDataCache(m_pTableMgr))
	, m_pFileCache(new ZFileCacheMgr)
	, m_pClientSoftMonitor(new ZClientSoftMonitor(m_pFileCache))
{

	m_pCmd2010 = new CCmdHandle10(m_pTableMgr,m_pIconTable,m_pDataCache,m_pFileCache,m_pClientSoftMonitor,this);
}

CRTDataSvr::~CRTDataSvr(void)
{
	delete m_pCmd2010;	
	m_ClientList.clear();
	NetClient::ReclaimObjectPool();

	delete m_pClientSoftMonitor;
	delete m_pFileCache;

	delete m_pDataCache;
	delete m_pDBWriter;
	delete m_pIconTable;
	delete m_pDBTrigger;
	delete m_pTableMgr;
	
}

//--------------------------------
//Release Method
void CRTDataSvr::Release()
{
	g_pPlug = 0;
	delete this;
}

//--------------------------------
//ISvrPlug interface 
LPCTSTR CRTDataSvr::GetPlugName()
{
	return RTDATASVR_PLUG_NAME;
}

DWORD  	CRTDataSvr::GetVersion()
{
	return 0x01000001;
}

LPCTSTR CRTDataSvr::GetCompany()
{
	return _T("GoYoo");
}

LPCTSTR CRTDataSvr::GetDescription()
{
	return _T("Realtime Data Server");
}

DWORD CRTDataSvr::GetListPort()
{
	return 17918;
}

bool CRTDataSvr::Initialize(ISvrPlugMgr* pPlugMgr)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

//	__asm {int 3}
	m_pPlugMgr = pPlugMgr;

	//初始化日志组件
	log::LogHelper::GetInstance(pPlugMgr);

	Log(LM_INFO, _T("正在初始化插件[%s]...\n"), GetPlugName());


	Log(LM_INFO, _T("加载数据库数据\n"));
	//加载数据表
#ifndef USED_FILE_DATABASE
	m_pDatabase = pPlugMgr->GetDatabase();
	if (!m_pDatabase || m_pTableMgr->Load(m_pDatabase) != 0) {
#else
	if (int err = m_pTableMgr->Load()) {
#endif
		Log(LM_ERROR, 
			_T("初始化实时数据表失败, 初始化插件失败!\n"));
		return false;
	}

	if (m_pIconTable->Load(m_pDatabase) != 0)
	{
		return false;
	}
	
	Log(LM_INFO, _T("加载二级缓存\n"));
	if (!m_pDataCache->LoadData()) {
		Log(LM_ERROR, _T("加载缓冲数据失败, 初始化插件失败\n"));
		return false;
	}

	Log(LM_INFO, _T("加载触发器\n"));
	if (int err = m_pDBTrigger->Startup()) {
		Log(LM_ERROR, 
			_T("加载触发器失败, 初始化插件失败, code=%d!\n"), err);
	}

	Log(LM_INFO, _T("加载写数据库\n"));
	m_pDBWriter->SetDatabase(m_pDatabase);
	if (int err = m_pDBWriter->Startup()) {
		Log(LM_ERROR, 
			_T("初始化数据库写入器失败, 初始化插件失败, code=%d!\n"), err);
		return false;
	}

	Log(LM_INFO, _T("加载客户端监视\n"));
	if (int err = m_pClientSoftMonitor->Startup()) {
		Log(LM_ERROR, 
			_T("客户端软件版本监视器启动失败, 初始化插件失败, code=%d!\n"), err);
		return false;
	}

	Log(LM_INFO, _T("注册二级缓存事件\n"));
	m_pDataCache->RegisterHandler();


	Log(LM_INFO, _T("启动动态负载"));
	balance_.reset(new ballance::ServerLoadBalance(m_pTableMgr));
	balance_->Start();


	// 写主服务模块状态到服务模块状态表中
	WriteMainServer();

	Log(LM_INFO, _T("初始化插件[%s]成功.\n"), GetPlugName());

	return true;
}

bool CRTDataSvr::UnInitialize()
{
	Log(LM_INFO, _T("正在释放插件[%s]...\n"), GetPlugName());
	balance_->Stop();
	balance_.reset();
	m_pClientSoftMonitor->Shutdown();
	m_pDataCache->UnregisterHandler();
	m_pDBWriter->Shutdown();
	m_pDBTrigger->Shutdown();
	m_pTableMgr->Save();

	Log(LM_INFO, _T("释放插件[%s]完成.\n"), GetPlugName());
	

	return true;
}


template < typename IPsT >
UINT MatchServerIP(db::tServer &server, const IPsT &IPs)
{
	UINT ipMask = 0;
	if( IPs.size() >= 1 )
	{
		server.Ip1 = IPs[0];
		ipMask |= MASK_TSERVER_IP1;
	}
	if( IPs.size() >= 2 )
	{
		server.Ip2 = IPs[1];
		ipMask |= MASK_TSERVER_IP2;
	}
	if( IPs.size() >= 3 )
	{
		server.Ip3 = IPs[2];
		ipMask |= MASK_TSERVER_IP3;
	}
	if( IPs.size() >= 4 )
	{
		server.Ip4 = IPs[3];
		ipMask |= MASK_TSERVER_IP4;
	}

	return ipMask;
}
void CRTDataSvr::WriteMainServer()
{
	TCHAR HostName [ MAX_NAME_LEN ] = {0};
	DWORD nSize = sizeof(HostName);

	std::vector<ulong> IPs;

	if (!GetServerName(HostName, &nSize)) 
		return;

	GetLocalIps(IPs);
	stdex::tString guid = utility::CreateGuidString();

	IServerRecordset	*pServerReset = 0;
	db::tServer server;

	m_pTableMgr->GetServerTable()->Select(&pServerReset,0,0);
	if( pServerReset->GetCount() == 0 )
	{
		utility::Strcpy(server.SvrID, guid);
		utility::Strcpy(server.SvrName, HostName);
		server.SvrIP = IPs[0];

		UINT ipMask = MatchServerIP(server, IPs);

		server.SvrType = i8desk::MainServer; 
		server.BalanceType = i8desk::BALANCE_POLL;
	
		m_pTableMgr->GetServerTable()->Ensure(&server, MASK_TSERVER_SVRTYPE | MASK_TSERVER_SVRNAME | MASK_TSERVER_BALANCETYPE | MASK_TSERVER_SVRIP | ipMask );
	}
	else
	{
		for (uint32 i = 0; i < pServerReset->GetCount(); i++)
		{
			db::tServer *d = pServerReset->GetData(i);
			
			// 已存在
			if( d->SvrType == i8desk::MainServer && _tcscmp(d->SvrName , HostName) == 0 )
			{
				server = *d;
				server.SvrIP = IPs[0];
				UINT ipMask = MatchServerIP(server, IPs);
				
				m_pTableMgr->GetServerTable()->Ensure(&server, ipMask);
				break;
			}

			// 删除老的主服务数据,再添加新的主服务
			if( d->SvrType == i8desk::MainServer && _tcscmp(d->SvrName, HostName) != 0 )
			{
				db::tServer tmpserver;
				m_pTableMgr->GetServerTable()->Delete(d->SvrID);
				utility::Strcpy(tmpserver.SvrID, d->SvrID);
				utility::Strcpy(tmpserver.SvrName, HostName);
				tmpserver.SvrIP = IPs[0];
				tmpserver.Ip1	= tmpserver.SvrIP;
				UINT ipMask = MatchServerIP(server, IPs);

				tmpserver.SvrType = i8desk::MainServer; 
				m_pTableMgr->GetServerTable()->Ensure(&tmpserver, MASK_TSERVER_SVRID | MASK_TSERVER_SVRTYPE| MASK_TSERVER_SVRNAME | MASK_TSERVER_SVRIP | ipMask);
			}

			if ( d->SvrType != i8desk::MainServer && _tcscmp(d->SvrName , HostName) == 0)
			{
				utility::Strcpy(server.SvrID,d->SvrID);
				utility::Strcpy(server.SvrName,HostName);
				server.SvrIP= IPs[0];
				server.Ip1	= server.SvrIP;
				UINT ipMask = MatchServerIP(server, IPs);
				server.SvrType = i8desk::MainServer; 
				m_pTableMgr->GetServerTable()->Ensure(&server, MASK_TSERVER_SVRTYPE| MASK_TSERVER_SVRNAME | MASK_TSERVER_SVRIP | ipMask);
			}
		}
	}

	db::tServerStatus serverstatus;

	utility::Strcpy(serverstatus.SvrID,server.SvrID);
	utility::Strcpy(serverstatus.SvrName,HostName);
	serverstatus.I8DeskSvr = i8desk::Online;
	serverstatus.SvrType = i8desk::MainServer;
	m_pTableMgr->GetServerStatusTable()->Ensure(&serverstatus, 
		MASK_TSERVERSTATUS_I8DESKSVR | 
		MASK_TSERVERSTATUS_SVRTYPE |
		MASK_TSERVERSTATUS_SVRNAME);

	pServerReset->Release();
}

void CRTDataSvr::WriteServer(ulong ip)
{
	db::tServer server;
	stdex::tString guid = utility::CreateGuidString();

	IServerRecordset	*pServerReset = 0;

	struct Filter : public IServerFilter {
		Filter(ulong ip) : m_ip(ip) {}
		bool bypass(const db::tServer *const d) {
			return m_ip != d->SvrIP;
		}
	private:
		ulong m_ip;
	} filter(ip);

	m_pTableMgr->GetServerTable()->Select(&pServerReset,&filter,0);
	
	if( 0 == pServerReset->GetCount() )
		utility::Strcpy(server.SvrID, guid.c_str());
	else
	{
		db::tServer *d = pServerReset->GetData(0);
		utility::Strcpy(server.SvrID, d->SvrID);
		server.SvrIP = ip;
		server.SvrType = i8desk::OtherServer;
	}


	m_pTableMgr->GetServerTable()->Ensure(&server, MASK_TSERVER_SVRTYPE|  MASK_TSERVER_SVRIP );

	pServerReset->Release();

}

BOOL CRTDataSvr::GetServerName(LPTSTR lpBuffer, LPDWORD nSize)
{
	return ::GetComputerName(lpBuffer, nSize);
}

bool CRTDataSvr::GetLocalIps(std::vector<ulong> &IPs)
{
	IP_ADAPTER_INFO info[16] = {0};
	DWORD dwSize = sizeof(info);
	if( ERROR_SUCCESS != ::GetAdaptersInfo(info, &dwSize) )
		return false; 

	PIP_ADAPTER_INFO pAdapter = info;
	while (pAdapter != NULL)
	{
		PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
		while (pAddr != NULL && utility::Strcmp(pAddr->IpAddress.String,"0.0.0.0") != 0)
		{
			IPs.push_back(::inet_addr(pAddr->IpAddress.String));
			pAddr = pAddr->Next;
		}
		pAdapter = pAdapter->Next;
	}
	
	if(IPs.empty())
		IPs.push_back(::inet_addr("127.0.0.1"));

	return true;
}

DWORD CRTDataSvr::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	switch (cmd) {
	case RTDS_CMD_ICON_OPERATE:
		{
			switch (param1) 
			{
			case RTDS_CMD_ICON_OPENSURE:
				{		
					db::tIcon *pIcon = reinterpret_cast<db::tIcon *>(param2);
					assert(pIcon);
					return m_pIconTable->AddCustomFile2(pIcon->gid,pIcon->size,pIcon->data);

				}
			case RTDS_CMD_ICON_OPDELETE:
				{
					size_t size = 0;
					void *data;
					assert(param2);
					if(m_pIconTable->GetFileData(param2,size,data))
						return m_pIconTable->DeleteCustomFile(param2);
					else
						return false;
				}
			}
			break;
		}
	case RTDS_CMD_GET_CLIENTLIST: {
		DWORD **pIpList = (DWORD **)param1;
		DWORD *pCount = (DWORD *)param2;

		std::vector<DWORD> clients;
		struct FGetClientIp : public std::unary_function<NetClient_Ptr, void> {
			FGetClientIp(std::vector<DWORD>& _clients) : clients_(_clients) {}
			result_type operator()(const argument_type& _Arg) {
				if (_Arg->GetType() == OBJECT_I8DESKCLISVR)
					clients_.push_back(_Arg->GetIP());
			}
			std::vector<DWORD>& clients_;
		};
		m_ClientList.foreach_value(FGetClientIp(clients));

		*pCount  = clients.size();
		if (*pCount) {
			*pIpList = (DWORD*)CoTaskMemAlloc(*pCount * sizeof(DWORD));
			for (DWORD i = 0; i < *pCount; i++) {
				(*pIpList)[i] = clients[i];
			}
		}
		else {
			*pIpList=NULL;
		}
		break; }
	case RTDS_CMD_GET_INTERFACE: {
		switch (param1) {
		case RTDS_INTERFACE_RTDATASVR:
			return reinterpret_cast<DWORD>(static_cast<IRTDataSvr *>(m_pTableMgr));
		default:
			assert(0);
			return 0;
		}
		return 0; }

	case RTDS_CMD_NOTIFYCONSOEL: 
		{
			DWORD ip = GetConsoleIP();
			if(ip == 0)
				return 0;

			GamePlugNotify *pnotify = reinterpret_cast<GamePlugNotify *>(param1);

			SendConsoleNotify((i8desk::TaskNotifyType)pnotify->TaskNotifyType, (i8desk::TaskNotify)pnotify->TaskNotify, pnotify->Gid, pnotify->Error_, ip);
			break;
		}
	case RTDS_CMD_REFRESH_PL:
		{
			DWORD ip = GetConsoleIP();
			if(ip == 0)
				return 0;

			SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (s == INVALID_SOCKET)
			{
				Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
				return 0;
			}

			try
			{
				sockaddr_in address		= {0};
				int len					= sizeof(address);
				address.sin_family		= AF_INET;
				address.sin_addr.s_addr	= ip;
				address.sin_port		= ::htons(CONTROL_UDP_PORT);

				char buf[512] = {0};
				i8desk::CPkgHelper out(buf, CMD_REFRESHPL, i8desk::OBJECT_UNKOWN);
				out << 1;

				sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len );
			}
			catch(std::exception &e)
			{
				stdex::tString info = CA2T(e.what());
				Log(i8desk::LM_ERROR, _T("发送UDP数据失败[TO:IP=%d, err=%d, info=%s]!"), ip, ::GetLastError(), info.c_str());
			}

			closesocket(s);
		}
		break;
	case RTDS_CMD_NOTIFYCHECKUSER:
		{
			DWORD ip = GetConsoleIP();
			if(ip == 0)
				return 0;

			SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (s == INVALID_SOCKET)
			{
				Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
				return 0;
			}

			try
			{
				sockaddr_in address		= {0};
				int len					= sizeof(address);
				address.sin_family		= AF_INET;
				address.sin_addr.s_addr	= ip;
				address.sin_port		= ::htons(CONTROL_UDP_PORT);

				char buf[512] = {0};
				i8desk::CPkgHelper out(buf, CMD_CHECKUSER_NOTIFY, i8desk::OBJECT_UNKOWN);
				out << param1;

				sendto(s, out.GetBuffer(), out.GetLength(), 0, (sockaddr*)&address, len );
			}
			catch(std::exception &e)
			{
				stdex::tString info = CA2T(e.what());
				Log(i8desk::LM_ERROR, _T("发送UDP数据失败[TO:IP=%d, err=%d, info=%s]!"), ip, ::GetLastError(), info.c_str());
			}

			closesocket(s);
		}
		break;

	case RTDS_CMD_GET_CONSOLE_IP:
		return GetConsoleIP();	
	default:
		return 0;
	}

	return 0;
}

void CRTDataSvr::SetConsoleIP(ulong ip) 
{ 
	I8_GUARD(ZGuard, ZSync, m_ConsoleIPLock); 
	m_ulConsoleIP = ip; 
	//m_pLogger->SetConsole(m_ulConsoleIP, htons(CONTROL_UDP_PORT));
}

//--------------------------------
//INetLayerEvent interface
void CRTDataSvr::OnAccept(INetLayer *pNetLayer, SOCKET sck, DWORD param)
{
	sockaddr_in address;
	int len = sizeof(sockaddr_in);
	getpeername(sck, (sockaddr*)&address, &len);

	NetClient_Ptr pObj(NetClient::CreateObj(pNetLayer, sck, address.sin_addr.s_addr));
	if (!pObj) {
		return;
	}
	m_ClientList.set(sck, pObj);	
}

namespace 
{
	typedef void (CRTDataSvr::*FuncType)(INetLayer*, SOCKET, LPCSTR, CPkgHelper &, pkgheader &);
	typedef void (CCmdHandle10::*FuncType1)(INetLayer*, SOCKET, LPCSTR, CPkgHelper &, pkgheader &);

	struct CmdMap
	{
		size_t cmd;
		FuncType func;
	};	

	struct CmdMap2010
	{
		size_t cmd;
		FuncType1 func;
	};	
	
	static const CmdMap cmdMap[] = 
	{
		{ CMD_BOOT_GETLIST,			&CRTDataSvr::Cmd_Boot_GetList },
		{ CMD_CLIRPT_CLIENTINFO,	&CRTDataSvr::Cmd_Clirpt_ClientInfo},
		{ CMD_CLIUPT_DOWNFILEINFO,	&CRTDataSvr::Cmd_CliUpt_DownFileInfo},
		{ CMD_CLIUPT_DOWNFILE,		&CRTDataSvr::Cmd_CliUpt_DownFile},
		{ CMD_CLIUPT_DOWNFILE_START,&CRTDataSvr::Cmd_CliUpt_DownFile_Start},
		{ CMD_CLIUPT_GET_FILELIST,	&CRTDataSvr::Cmd_CliUpt_GetFileList},
		{ CMD_CLIRPT_HARDWARE,		&CRTDataSvr::Cmd_Clirpt_Hardware},
		{ CMD_CON_GETINFO_FROM_GAMEUPDATESVR, &CRTDataSvr::Cmd_Con_GetInfo_From_GameUpdateSvr},
		{ CMD_DOWNFILE_NEXT,		&CRTDataSvr::Cmd_DownFile_Next},
		{ CMD_FAVORITE_GETLIST,		&CRTDataSvr::Cmd_Favorite_GetList},
		{ CMD_GAME_CLIENTTOCON,		&CRTDataSvr::Cmd_Game_ClientToCon},
		{ CMD_GAME_CONEXESQL,		&CRTDataSvr::Cmd_Game_ConExeSql},
		{ CMD_GAME_CONTOCLIENT,		&CRTDataSvr::Cmd_Game_ConToClient},
		{ CMD_GAME_GETIDXS,			&CRTDataSvr::Cmd_Game_GetIdxs},
		{ CMD_GAME_GETSOME,			&CRTDataSvr::Cmd_Game_GetSome},
		{ CMD_CON_GAME_ICON,		&CRTDataSvr::Cmd_Game_Icon},
		{ CMD_GAME_REPCLICK,		&CRTDataSvr::Cmd_Game_RepClick },
		{ CMD_GAME_REQ_ICONS_IDX,	&CRTDataSvr::Cmd_Game_Req_Icons_Idx },
		{ CMD_GAME_REQ_ICONS_DATA,	&CRTDataSvr::Cmd_Game_Req_Icons_data }, 		
		{ CMD_CON_GET_BARSKINS,		&CRTDataSvr::Cmd_Get_BarSkins},
		{ CMD_GET_SERVER_CONFIG,	&CRTDataSvr::Cmd_Get_Server_Config},
		{ CMD_GET_VDISK_CONFIG,		&CRTDataSvr::Cmd_Get_VDisk_Config},
		{ CMD_NOTIFY_SAVE_DATA,		&CRTDataSvr::Cmd_Notify_Save_Data},
		{ CMD_RELOAD_CACHE_FILE,	&CRTDataSvr::Cmd_Reload_Cache_File},
		{ CMD_SYSOPT_GETLIST,		&CRTDataSvr::Cmd_SysOpt_GetList},
		{ CMD_SYSOPT_SYNTIME,		&CRTDataSvr::Cmd_Sysopt_Syntime},
		{ CMD_SVRRPT_SERVERINFO_EX, &CRTDataSvr::Cmd_Svrrpt_ServerInfo_Ex},
		{ CMD_UPT_CLIENT_INDEX,		&CRTDataSvr::Cmd_Update_Client_Index},
		{ CMD_VDISK_GETLIST,		&CRTDataSvr::Cmd_VDisk_GetList},
		{ CMD_VDSVR_REPSTATUS,		&CRTDataSvr::Cmd_Vdsvr_ReqStatus},
		{ CMD_VDSVR_GETUPDATE_FILEINFO, &CRTDataSvr::Cmd_VDisk_GetUpdateFileInfo},
		{ CMD_VDSVR_GETUPDATE_FILELIST, &CRTDataSvr::Cmd_VDisk_GetUpdateFileList},
		{ CMD_VALIDATE,				&CRTDataSvr::Cmd_Validate},
		{ CMD_GETCIENTVERSION,		&CRTDataSvr::Cmd_GetClientVersion},
		{ CMD_VDSVR_VDISKCLIENT,	&CRTDataSvr::Cmd_Vdsvr_VDiskClient},
		{ CMD_CON_SETFILE_CLASSICON,&CRTDataSvr::Cmd_Con_SetClassIcon },
		{ CMD_CON_PLUGTOOL_ICON,	&CRTDataSvr::Cmd_Con_PlugToolIcon },
		{ CMD_CON_PLUGTOOL_OPERATE,	&CRTDataSvr::Cmd_Con_PlugToolOperate },
		{ CMD_CON_MONITOR_STATUS,	&CRTDataSvr::Cmd_Con_MonitorStatus },
		{ CMD_CON_STATISTIC_STATUS,	&CRTDataSvr::Cmd_Con_StatisticStatus },
		{ CMD_CON_MONITOR_DELETEFILE,	&CRTDataSvr::Cmd_Con_MonitorDeleteFile },
		{ CMD_BUSINESS_INFOEX,		&CRTDataSvr::Cmd_Business_InfoEx},
		{ CMD_CON_GETALL_GAMEICONS,	&CRTDataSvr::Cmd_Con_GetAll_GameIcon},
		{ CMD_CON_GETALL_CLASSICONS,&CRTDataSvr::Cmd_Con_GetAll_ClassIcon},
		{ CMD_CON_REPORT_USERINFOMODIFY,&CRTDataSvr::Cmd_Con_UserInfoModify},
		{ CMD_PLUGTOOL_GETIDXS,		&CRTDataSvr::Cmd_PlugTool_GetIdxs},
		{ CMD_REPORT_SVRIP,			&CRTDataSvr::Cmd_Report_ConnectSvrIP},
		{ CMD_PLUGTOOL_GETSOME,		&CRTDataSvr::Cmd_PlugTool_GetSome},
		{ CMD_TASK_OPERATION,		&CRTDataSvr::Cmd_GameMgr_TaskOperation	},
		{ CMD_CON_NOTIFY_REFRESH_PL,&CRTDataSvr::Cmd_GameMgr_NotifyRefreshPL	},
		{ CMD_NOTIFY_SERVER_MEDICAL,&CRTDataSvr::Cmd_GameMgr_NotifyServerMedical},
		{ CMD_CON_NOTIFY_DELETEFILE,&CRTDataSvr::Cmd_GameMgr_NotifyDeleteFile},
		{ CMD_CON_SAVE_HARDWARE,	&CRTDataSvr::Cmd_Con_SaveHardWare},
		{ CMD_CON_SMART_CLEAN,		&CRTDataSvr::Cmd_Con_NotifySmartClean},
		{ CMD_CON_MONITOR_DISK_SIZE,&CRTDataSvr::Cmd_Con_CheckSpace},
		{ CMD_BUSINISS_GETGAMEEXVER,&CRTDataSvr::Cmd_Businisss_GetGameExVer},
		{ CMD_CLIENT_GETDELGAME,	&CRTDataSvr::Cmd_Client_GetDelGame},
		{ CMD_CLIENT_RPTDELGAMESTATUS,&CRTDataSvr::Cmd_Client_RptGameStatus},
		{ CMD_CON_SYNCTASK_OPERATE , &CRTDataSvr::Cmd_UpdateSvr_Operate },


	};

	static const CmdMap2010 cmdMap2010[] = 
	{
		{ CMD_GAME_REQ_ICONS_IDX,	&CCmdHandle10::Cmd_Game_Req_Icons_Idx },
		{ CMD_GAME_REQ_ICONS_DATA,	&CCmdHandle10::Cmd_Game_Req_Icons_data }, 
		{ CMD_GAME_GETLIST,			&CCmdHandle10::Cmd_Game_GetList},
		{ CMD_GAME_GETIDXS,			&CCmdHandle10::Cmd_Game_GetIdxs},
		{ CMD_GAME_GETSOME,			&CCmdHandle10::Cmd_Game_GetSome},
		{ CMD_GAME_GETICONLIST,		&CCmdHandle10::Cmd_Game_IconList},
		{ CMD_GAME_REPCLICK,		&CCmdHandle10::Cmd_Game_RepClick },
		{ CMD_GAME_ICONREFRESH,		&CCmdHandle10::Cmd_Game_IconRefresh },
		{ CMD_GAME_CONEXESQL,		&CCmdHandle10::Cmd_Game_ConExeSql},
		{ CMD_GAME_CONTOCLIENT,		&CCmdHandle10::Cmd_Game_ConToClient},
		{ CMD_GAME_CLIENTTOCON,		&CCmdHandle10::Cmd_Game_ClientToCon},
		{ CMD_VDISK_GETLIST,		&CCmdHandle10::Cmd_VDisk_GetList},
		{ CMD_SYSOPT_GETLIST,		&CCmdHandle10::Cmd_SysOpt_GetList},
		{ CMD_SYSOPT_SYNTIME,		&CCmdHandle10::Cmd_Sysopt_Syntime},
		{ CMD_FAVORITE_GETLIST,		&CCmdHandle10::Cmd_Favorite_GetList},
		{ CMD_BOOT_GETLIST,			&CCmdHandle10::Cmd_Boot_GetList },
		{ CMD_CLIUPT_DOWNFILEINFO,	&CCmdHandle10::Cmd_CliUpt_DownFileInfo},
		{ CMD_CLIUPT_DOWNFILE,		&CCmdHandle10::Cmd_CliUpt_DownFile},
		{ CMD_CLIUPT_DOWNFILE_START,&CCmdHandle10::Cmd_CliUpt_DownFile_Start},
		{ CMD_CLIUPT_GET_ALL_CLIENTS, &CCmdHandle10::Cmd_Get_All_Clients},
		{ CMD_CLIUPT_UPLOAD,		&CCmdHandle10::Cmd_CliUpt_UpLoad},
		{ CMD_CLIUPT_GET_FILELIST,	&CCmdHandle10::Cmd_CliUpt_GetFileList},
		{ CMD_CLIRPT_CLIENTINFO,	&CCmdHandle10::Cmd_Clirpt_ClientInfo},
		{ CMD_CLIRPT_HARDWARE,		&CCmdHandle10::Cmd_Clirpt_Hardware},
		{ CMD_SVRRPT_SERVERINFO,	&CCmdHandle10::Cmd_Svrrpt_ServerInfo},
		{ CMD_SVRRPT_SERVERINFO_EX, &CCmdHandle10::Cmd_Svrrpt_ServerInfo_Ex},
		{ CMD_VDSVR_GETLIST,		&CCmdHandle10::Cmd_Vdsvr_GetList}, 
		{ CMD_VDSVR_REPSTATUS,		&CCmdHandle10::Cmd_Vdsvr_ReqStatus},
		{ CMD_VDSVR_GETUPDATE_FILEINFO, &CCmdHandle10::Cmd_VDisk_GetUpdateFileInfo},
		{ CMD_VDSVR_GETUPDATE_FILELIST, &CCmdHandle10::Cmd_VDisk_GetUpdateFileList},
		{ CMD_DOWNFILE_NEXT,		&CCmdHandle10::Cmd_DownFile_Next},
		{ CMD_BOOT_DELLIST,			&CCmdHandle10::Cmd_Boot_DelList},
		{ CMD_CON_GAME_ICON,			&CCmdHandle10::Cmd_Game_Icon},
		{ CMD_VALIDATE,				&CCmdHandle10::Cmd_Validate},
		{ CMD_UPT_CLIENT_INDEX,		&CCmdHandle10::Cmd_Update_Client_Index},
		{ CMD_CON_GETINFO_FROM_GAMEUPDATESVR, &CCmdHandle10::Cmd_Con_GetInfo_From_GameUpdateSvr},
		{ CMD_CON_GET_BARSKINS,			&CCmdHandle10::Cmd_Get_BarSkins},
		{ CMD_GET_SERVER_CONFIG,	&CCmdHandle10::Cmd_Get_Server_Config},
		{ CMD_GET_VDISK_CONFIG,		&CCmdHandle10::Cmd_Get_VDisk_Config},
		{ CMD_RELOAD_CACHE_FILE,	&CCmdHandle10::Cmd_Reload_Cache_File},
		{ CMD_NOTIFY_SAVE_DATA,		&CCmdHandle10::Cmd_Notify_Save_Data},
	};


	struct CmdMap302
	{
		size_t cmd;
		FuncType func;
	};	

	static const CmdMap302 cmdMap302[] = 
	{
		{ CMD_CLIRPT_CLIENTINFO,	&CRTDataSvr::Cmd_Clirpt_ClientInfo},
		{ CMD_CLIUPT_DOWNFILEINFO,	&CRTDataSvr::Cmd_CliUpt_DownFileInfo},
		{ CMD_DOWNFILE_NEXT,		&CRTDataSvr::Cmd_DownFile_Next},
		{ CMD_CLIUPT_DOWNFILE_START,&CRTDataSvr::Cmd_CliUpt_DownFile_Start},
	};
}

void CRTDataSvr::OnRecv(INetLayer *pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
{
	NetClient_Ptr pClient;
	if (!m_ClientList.peek(sck, pClient))
		return;

	pkgheader header = {0};
	CPkgHelper inpackage((LPSTR)lpPackage);

	inpackage.popHeader(header);
	if (param == 1)		// 表示第一包数据
	{
		pClient->SetType(header.Resv[0]);

		switch (pClient->GetType()) {
		case OBJECT_I8DESKCLISVR: {
			//查找同一个远端并删除
			Log(LM_DEBUG, _T("[%s]客户端连接[IP=%s].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(pClient->GetIPAddress()));
			break; }
		case OBJECT_CONSOLE: {
			Log(LM_INFO, _T("[%s]控制台连接[IP=%s][sock=%d]\n"),
				this->GetPlugName(), (LPCTSTR)CA2T(pClient->GetIPAddress()),sck);
			SetConsoleIP(pClient->GetIP());
			// 日志写向控制台
			i8desk::log::LogHelper::GetInstance().GetLogger()->SetConsole(pClient->GetIP(), CONTROL_UDP_PORT);
			// 关闭上一个控制台
			DWORD lastIP = LastConsole(pClient->GetIP()) ;
			if( lastIP )
				SendConOffLineNotify(lastIP);
			break; }
		case OBJECT_VDISKSVR: {
			//WriteServer(pClient->GetIP());
			Log(LM_DEBUG, _T("[%s]虚拟盘连接[IP=%s].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(pClient->GetIPAddress()));
			break; }
		case OBJECT_BARONLINE: {
			Log(LM_DEBUG, _T("[%s]菜单连接[IP=%s].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(pClient->GetIPAddress()));
			break; }
		case OBJECT_NODESERVER: {
			Log(LM_DEBUG, _T("[%s]同步工具连接[IP=%s].\n"),
				this->GetPlugName(), (LPCTSTR)CA2T(pClient->GetIPAddress()));
			break; }
		default:
			assert(0);
			break;
		}
	}


	//Log(LM_DEBUG,_T("收到命令 version = %x , Cmd = %x"),header.Version,header.Cmd);

	if( header.Cmd == CMD_CON_HEART_BEAT )
	{
		ACK();
		return;
	}

	switch (header.Version)
	{
	case 0x200:
		for(size_t i = 0; i != _countof(cmdMap2010); ++i)
		{
			if( cmdMap2010[i].cmd == header.Cmd )
			{
				(m_pCmd2010->*cmdMap2010[i].func)(pNetLayer, sck, lpPackage, inpackage, header);
				return;
			}
		}

		Cmd_Unknown(pNetLayer, sck, lpPackage,inpackage,header);

		break;
	case PRO_VERSION:
		for(size_t i = 0; i != _countof(cmdMap); ++i)
		{
			if( cmdMap[i].cmd == header.Cmd )
			{
				(this->*cmdMap[i].func)(pNetLayer, sck, lpPackage, inpackage, header);
				return;
			}
		}

		Cmd_Unknown(pNetLayer, sck, lpPackage,inpackage,header);
		break;
	case 0x302:
		for(size_t i = 0; i != _countof(cmdMap); ++i)
		{
			if( cmdMap302[i].cmd == header.Cmd )
			{
				(this->*cmdMap302[i].func)(pNetLayer, sck, lpPackage, inpackage, header);
				return;
			}
		}

		Cmd_Unknown(pNetLayer, sck, lpPackage,inpackage,header);

		break;
	default:
		NAK_RETURN(_T("错误的版本格式"));
		break;
	}
}

void CRTDataSvr::OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)
{
	NetClient_Ptr pClient;
	if (m_ClientList.get(sck, pClient)) {
		in_addr in;
		in.s_addr = pClient->GetIP();
		switch (pClient->GetType()) {
		case OBJECT_I8DESKCLISVR: {

			struct Filter : public IClientFilter {
				Filter(ulong ip) 
					: m_ip(ip)
				{
				}
				bool bypass(const db::tClient *const d)
				{
					return d->IP != m_ip;
				}
			private:
				ulong m_ip;
			} filter( in.s_addr);
			db::tClient client;
			client.Online = 0;
			client.Temperature = 0;
			Log(LM_INFO,_T("客户机下线"));
			m_pTableMgr->GetClientTable()->Update(&client,MASK_TCLIENT_ONLINE|MASK_TCLIENT_TEMPERATURE,&filter); 
			Log(LM_DEBUG, _T("[%s]客户端连接关闭[IP=%s].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(inet_ntoa(in)));
			
			//balance_->UpdateDisconnect(pClient->GetIP());

			struct VDiskClientFilter : public IVDiskClientFilter {
				VDiskClientFilter(ulong ip) : m_ip(ip) {}
				bool bypass(const db::tVDiskClient *const d) {
					return d->ClientIP != m_ip;
				}
			private:
				ulong m_ip;
			} vDiskClientFilter(in.s_addr);

			m_pTableMgr->GetVDiskClientTable()->Delete(&vDiskClientFilter); 

			break; }

		case OBJECT_CONSOLE: {
			SetConsoleIP(0);
			Log(LM_INFO, _T("[%s]控制台连接关闭[IP=%s][sosk=%d].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(inet_ntoa(in)),sck);
			break; }

		case OBJECT_VDISKSVR: {
			RemoveVDiskStatus(pClient->GetIP());
			RemoveServerStatus(pClient->GetIP());
			Log(LM_DEBUG, _T("[%s]虚拟盘连接关闭[IP=%s].\n"), 
				this->GetPlugName(), (LPCTSTR)CA2T(inet_ntoa(in)));
			break; }

		case OBJECT_BARONLINE: {
			Log(LM_DEBUG, _T("[%s]菜单连接关闭[IP=%s].\n"), 
				this->GetPlugName(),(LPCTSTR)CA2T(inet_ntoa(in)));
			break; }

		case OBJECT_NODESERVER: {
			Log(LM_DEBUG, _T("[%s]同步工具连接关闭[IP=%s].\n"),
				this->GetPlugName(), (LPCTSTR)CA2T(inet_ntoa(in)));
			break; }

		default:
			assert(0);
			break;
		}
	}
}

DWORD CRTDataSvr::LastConsole( DWORD dwIp)
{
	NetClientList::iterator iter;
	for ( iter = m_ClientList.begin(); iter != m_ClientList.end(); ++iter)
	{
		if( iter->second->GetType() == OBJECT_CONSOLE && iter->second->GetIP() != dwIp)
			return iter->second->GetIP();
	}
	return 0;
}

void CRTDataSvr::SendConOffLineNotify(const DWORD& dwIp)
{
		char buf[500];
		CPkgHelper outpackage(buf, CMD_CON_OFFLINE_NOTIFY, OBJECT_CONSOLE);
		outpackage << 1L;

		SOCKET s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		if (s==INVALID_SOCKET) {
			Log(LM_WARNING, _T("UDP套接字打开失败!\n"));
			return;
		}
		sockaddr_in address;
		int len=sizeof(address);
		address.sin_family=AF_INET;
		address.sin_addr.s_addr=dwIp;
		address.sin_port=htons(CONTROL_UDP_PORT);

		std::string str = outpackage.GetBuffer();

		int iret = sendto(s, outpackage.GetBuffer(), outpackage.GetLength(), 0, (sockaddr*)&address, len);
		if (iret != outpackage.GetLength()) 
		{
			stdex::tString Ip = CA2T(::inet_ntoa(address.sin_addr));
			Log(LM_WARNING, _T("发送UDP数据失败[TO:IP=%s]!\n"),Ip.c_str());
		}

		closesocket(s);
}

void CRTDataSvr::RemoveVDiskStatus(ulong ip)
{
	struct Filter : public IVDiskStatusFilter {
		Filter(ulong ip) : m_ip(ip) {}
		bool bypass(const db::tVDiskStatus *const d) {
			return d->VDiskIP != m_ip;
		}
	private:
		ulong m_ip;
	} filter(ip);

	m_pTableMgr->GetVDiskStatusTable()->Delete(&filter); 
}

void CRTDataSvr::RemoveServerStatus(ulong ip)
{
	stdex::tString svrName;
	if( !m_SvrList.get(ip, svrName) )
		return;

	struct Filter : public IServerStatusFilter {
		Filter(const stdex::tString &svrName) : svrName_(svrName) {}
		bool bypass(const db::tServerStatus *const d) {
			return d->SvrName != svrName_;
		}
	private:
		const stdex::tString &svrName_;
	} filterserver(svrName);

	IServerStatusRecordset *pRsd = 0;
	m_pTableMgr->GetServerStatusTable()->Select( &pRsd, &filterserver,0);
	if( pRsd->GetCount() != 0  )
	{
		db::tServerStatus *server = pRsd->GetData(0);
		server->I8VDiskSvr = 0;
		m_pTableMgr->GetServerStatusTable()->Ensure(server, MASK_TSERVERSTATUS_I8VDISKSVR);
	}

	pRsd->Release();

}


//////////////////////////////////////////////////////////////////////////
//NetClient 文件操作
//
DWORD NetClient::GetFileSize(LPCTSTR lpFileName)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = g_pPlug->GetFileCache()->Open(lpFileName)) {
		return CacheFile->GetSize();
	}
#endif //USED_I8_FILECACHE

	ULONGLONG ullSize;
	if (utility::GetFileSize(lpFileName, ullSize) && (ullSize >> 32) == 0)
		return static_cast<DWORD>(ullSize);

	return 0;
}

ulong NetClient::GetFileCrc(LPCTSTR lpFileName)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = g_pPlug->GetFileCache()->Open(lpFileName)) {
		return CacheFile->GetCrc();
	}
#endif //USED_I8_FILECACHE

	ulong crc;
	if (utility::GetFileCrc32(lpFileName, crc))
		return crc;

	return 0;
}

ULONGLONG NetClient::GetFileLastWriteTime(LPCTSTR lpFileName)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = g_pPlug->GetFileCache()->Open(lpFileName)) {
		return CacheFile->GetLastWriteTime();
	}
#endif //USED_I8_FILECACHE

	ULONGLONG t;
	if (utility::GetFileLastWriteTime(lpFileName, t))
		return t;

	return 0;
}

BOOL NetClient::OpenFile(
						 LPCTSTR lpFileName,
						 DWORD dwDesiredAccess,
						 DWORD dwShareMode,
						 DWORD dwCreationDisposition,
						 DWORD dwFlagsAndAttributes)
{
	Log(LM_DEBUG, 
		_T("客户端请求文件传输:IP=%s, 文件名=%s\n"),
		(LPCTSTR)CA2T(this->GetIPAddress()), lpFileName);

#ifdef USED_I8_FILECACHE
	//assert(!m_CacheFile);
	if (dwDesiredAccess == GENERIC_READ ) {
		m_CacheFile = g_pPlug->GetFileCache()->Open(lpFileName);
		if (m_CacheFile) {
			m_dwOffset = 0;
			Log(LM_DEBUG, 
				_T("返回缓冲文件对象成功:IP=%s, 文件名=%s\n"),
				(LPCTSTR)CA2T(this->GetIPAddress()), lpFileName);
			return TRUE;
		}
	}
#endif //USED_I8_FILECACHE

	assert(m_hFile == INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hFile);
	}
	m_hFile = CreateFile(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		NULL,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		NULL);
	return m_hFile != INVALID_HANDLE_VALUE;
}

void NetClient::CloseFile(void)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = m_CacheFile) {
		assert(m_hFile == INVALID_HANDLE_VALUE);
		Log(LM_DEBUG, _T("关闭传输的文件:IP=%s, 文件名=%s\n"),
			(LPCTSTR)CA2T(this->GetIPAddress()), CacheFile->GetFileName().c_str());
		m_CacheFile.reset(); //注意，关闭的是m_CacheFile
		return;
	}
#endif //USED_I8_FILECACHE

	assert(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE) {
		Log(LM_DEBUG, _T("关闭传输的文件:IP=%s, 文件句柄=%X\n"),
			(LPCTSTR)CA2T(this->GetIPAddress()), (DWORD)m_hFile);
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

BOOL NetClient::ReadFileData(BYTE *pbuf, DWORD len)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = m_CacheFile) {
		assert(m_hFile == INVALID_HANDLE_VALUE);
		assert(m_dwOffset + len <= CacheFile->GetSize());
		return CacheFile->ReadData(m_dwOffset, pbuf, len);
	}
#endif //USED_I8_FILECACHE

	assert(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE)
		return utility::ReadFileContent(m_hFile, pbuf, len);
	return FALSE;
}

BOOL NetClient::WriteFileData(BYTE *pbuf, DWORD len)
{
	assert(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE)
		return utility::WriteFileContent(m_hFile, pbuf, len);
	return FALSE;
}



} //namespace i8desk

