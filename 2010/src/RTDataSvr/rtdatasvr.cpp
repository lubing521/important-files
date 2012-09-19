#include "StdAfx.h"
#include "rtdatasvr.h"
#include "dbwriter.h"
#include "datacache.h"
#include "filemonitor.h"
#include "filecache.h"

#include <iostream>
#include <algorithm>

namespace i8desk {

IMPLMENT_I8_NEW_DELETE_OP(NetClient)

//////////////////////////////////////
CRTDataSvr *g_pPlug = 0;

IPlug *CreatePlug(void)
{
 	if (g_pPlug == 0)
		g_pPlug = new CRTDataSvr;
	return g_pPlug;
}


CI8Log *GetI8Log(void)
{
	I8_ASSERT(g_pPlug != 0);
	return g_pPlug;
}

/////////////////////////////////
// CRTDataSvr

CRTDataSvr::CRTDataSvr(void)
	: m_pLogger(0)
	, m_ulConsoleIP(0)
	, m_pDatabase(0)
	, m_RecordLogMasks(LM_INFO|LM_WARNING|LM_ERROR)
	, m_ReportLogMasks(LM_ENSURE_32_BITS)
#ifndef NDEBUG
	, m_DisplayLogMasks(LM_ENSURE_32_BITS)
#endif
{
	m_pTableMgr = new ZTableMgr;

	m_pDBWriter = new ZDBWriter(m_pTableMgr);
	m_pDataCache = new ZDataCache(m_pTableMgr);

	m_pFileCache = new ZFileCacheMgr;
	m_pClientSoftMonitor = new ZClientSoftMonitor(m_pFileCache);
}

CRTDataSvr::~CRTDataSvr(void)
{
	m_ClientList.clear();
	NetClient::ReclaimObjectPool();

	delete m_pClientSoftMonitor;
	delete m_pFileCache;

	delete m_pDataCache;
	delete m_pDBWriter;
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
//CI8Log interface
void CRTDataSvr::i8log(I8_Log_Priority priority, const I8_TCHAR *format, ...)
{
	I8_TCHAR xbuf[1024*4];

	va_list marker;
	va_start(marker, format);
	int nOk = ::_vsnprintf_s(xbuf, sizeof(xbuf), format, marker);
	va_end(marker);

	if (nOk <= 0) {
		I8_ASSERT(0);
		return;
	}

	m_pLogger->WriteLog2(priority, xbuf, nOk);
}

//--------------------------------
//IPlug interface 
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

bool CRTDataSvr::Initialize(IPlugMgr* pPlugMgr)
{
	m_pPlugMgr = pPlugMgr;

	//初始化日志组件
	m_pLogger = pPlugMgr->CreateLogger();
	m_pLogger->SetLogFileName(GetPlugName());
	m_pLogger->SetMaxLogSize(1024*1024*10);

	I8_INFOR((LM_INFO, I8_TEXT("正在初始化插件[%s]...\n"), GetPlugName()));


	I8_INFOR((LM_INFO, I8_TEXT("加载数据库数据\n")));
	//加载数据表
#ifndef USED_FILE_DATABASE
	m_pDatabase = pPlugMgr->GetDatabase();
	if (!m_pDatabase || m_pTableMgr->Load(m_pDatabase) != 0) {
#else
	if (int err = m_pTableMgr->Load()) {
#endif
		I8_ERROR((LM_ERROR, 
			I8_TEXT("初始化实时数据表失败, 初始化插件失败!\n")));
		m_pLogger->Release();
		return false;
	}
	
	I8_INFOR((LM_INFO, I8_TEXT("加载二级缓存\n")));
	if (!m_pDataCache->LoadData()) {
		I8_ERROR((LM_ERROR, I8_TEXT("加载缓冲数据失败, 初始化插件失败\n")));
		m_pLogger->Release();
		return false;
	}

	I8_INFOR((LM_INFO, I8_TEXT("加载写数据库\n")));
	m_pDBWriter->SetDatabase(m_pDatabase);
	if (int err = m_pDBWriter->Startup()) {
		I8_ERROR((LM_ERROR, 
			I8_TEXT("初始化数据库写入器失败, 初始化插件失败, code=%d!\n"), err));
		m_pLogger->Release();
		return false;
	}

	I8_INFOR((LM_INFO, I8_TEXT("加载客户端监视\n")));
	if (int err = m_pClientSoftMonitor->Startup()) {
		I8_ERROR((LM_ERROR, 
			I8_TEXT("客户端软件版本监视器启动失败, 初始化插件失败, code=%d!\n"), err));
		m_pLogger->Release();
		return false;
	}

	I8_INFOR((LM_INFO, I8_TEXT("注册二级缓存事件\n")));
	m_pDataCache->RegisterHandler();

	I8_INFOR((LM_INFO, I8_TEXT("初始化插件[%s]成功.\n"), GetPlugName()));

	//写主服务模块状态到服务模块状态表中
	db::tServerStatus d = {0};
	DWORD nSize = sizeof(d.HostName);
	if (i8desk::GetServerName(d.HostName, &nSize)) {
		d.I8DeskSvr = 1;
		m_pTableMgr->GetServerStatusTable()->Ensure(&d, MASK_TSERVERSTATUS_I8DESKSVR);
	}
		
	return true;
}

bool CRTDataSvr::UnInitialize()
{
	I8_INFOR((LM_INFO, I8_TEXT("正在释放插件[%s]...\n"), GetPlugName()));

	m_pClientSoftMonitor->Shutdown();
	m_pDataCache->UnregisterHandler();
	m_pDBWriter->Shutdown();
	m_pTableMgr->Save();

	I8_INFOR((LM_INFO, I8_TEXT("释放插件[%s]完成.\n"), GetPlugName()));
	
	m_pLogger->Release();

	return true;
}

DWORD CRTDataSvr::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	switch (cmd) {
	case RTDS_CMD_GET_CLIENTLIST: {
		DWORD **pIpList = (DWORD **)param1;
		DWORD *pCount = (DWORD *)param2;

		std::vector<DWORD> clients;
		struct FGetClientIp : public std::unary_function<NetClient_Ptr, void> {
			FGetClientIp(std::vector<DWORD>& _clients) : clients_(_clients) {}
			result_type operator()(const argument_type& _Arg) {
				if (_Arg->GetType() == OBJECT_CLIENTSVI)
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
			I8_ASSERT(0);
			return 0;
		}
		return 0; }
	default:
		return 0;
	}

	return 0;
}

void CRTDataSvr::SetConsoleIP(ulong ip) 
{ 
	I8_GUARD(ZGuard, ZSync, m_ConsoleIPLock); 
	m_ulConsoleIP = ip; 
	m_pLogger->SetConsole(m_ulConsoleIP, htons(CONTROL_UDP_PORT));
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
	typedef void (CRTDataSvr::*FuncType)(INetLayer*, SOCKET, LPCSTR, CPackageHelper &, _packageheader &);

	struct CmdMap
	{
		size_t cmd;
		FuncType func;
	};	

	static const CmdMap cmdMap[] = 
	{
		{ CMD_GAME_REQ_ICONS_IDX,	&CRTDataSvr::Cmd_Game_Req_Icons_Idx },
		{ CMD_GAME_REQ_ICONS_DATA,	&CRTDataSvr::Cmd_Game_Req_Icons_data }, 
		{ CMD_GAME_GETLIST,			&CRTDataSvr::Cmd_Game_GetList},
		{ CMD_GAME_GETIDXS,			&CRTDataSvr::Cmd_Game_GetIdxs},
		{ CMD_GAME_GETSOME,			&CRTDataSvr::Cmd_Game_GetSome},
		{ CMD_GAME_GETICONLIST,		&CRTDataSvr::Cmd_Game_IconList},
		{ CMD_GAME_REPCLICK,		&CRTDataSvr::Cmd_Game_RepClick },
		{ CMD_VDISK_GETLIST,		&CRTDataSvr::Cmd_VDisk_GetList},
		{ CMD_SYSOPT_GETLIST,		&CRTDataSvr::Cmd_SysOpt_GetList},
		{ CMD_FAVORITE_GETLIST,		&CRTDataSvr::Cmd_Favorite_GetList},
		{ CMD_BOOT_GETLIST,			&CRTDataSvr::Cmd_Boot_GetList },
		{ CMD_USER_REGISTER,		&CRTDataSvr::Cmd_User_Register},
		{ CMD_USER_LOGIN,			&CRTDataSvr::Cmd_User_LogIn},
		{ CMD_USER_MODPWD,			&CRTDataSvr::Cmd_User_ModPwd},
		{ CMD_CLIRPT_CLIENTINFO,	&CRTDataSvr::Cmd_Clirpt_ClientInfo},
		{ CMD_CLIRPT_HARDWARE,		&CRTDataSvr::Cmd_Clirpt_Hardware},
		{ CMD_SVRRPT_SERVERINFO,	&CRTDataSvr::Cmd_Svrrpt_ServerInfo},
		{ CMD_SVRRPT_SERVERINFO_EX, &CRTDataSvr::Cmd_Svrrpt_ServerInfo_Ex},
		{ CMD_GAME_ICONREFRESH,		&CRTDataSvr::Cmd_Game_IconRefresh },
		{ CMD_CLIUPT_DELETE,		&CRTDataSvr::Cmd_User_Delete},
		{ CMD_CLIUPT_GET_ALL_CLIENTS, &CRTDataSvr::Cmd_Get_All_Clients},
		{ CMD_USER_DOWNLOAD,		&CRTDataSvr::Cmd_User_Download},
		{ CMD_USER_UPLOAD,			&CRTDataSvr::Cmd_User_Upload}, 
		{ CMD_GAME_CONEXESQL,		&CRTDataSvr::Cmd_Game_ConExeSql},
		{ CMD_VDSVR_GETLIST,		&CRTDataSvr::Cmd_Vdsvr_GetList}, 
		{ CMD_GAME_CONTOCLIENT,		&CRTDataSvr::Cmd_Game_ConToClient},
		{ CMD_GAME_CLIENTTOCON,		&CRTDataSvr::Cmd_Game_ClientToCon},
		{ CMD_VDSVR_REPSTATUS,		&CRTDataSvr::Cmd_Vdsvr_ReqStatus},
		{ CMD_CLIUPT_DOWNFILEINFO,	&CRTDataSvr::Cmd_CliUpt_DownFileInfo},
		{ CMD_CLIUPT_DOWNFILE,		&CRTDataSvr::Cmd_CliUpt_DownFile},
		{ CMD_CLIUPT_DOWNFILE_START,&CRTDataSvr::Cmd_CliUpt_DownFile_Start},
		{ CMD_DOWNFILE_NEXT,		&CRTDataSvr::Cmd_DownFile_Next},
		{ CMD_USER_DOWNLOAD_START,	&CRTDataSvr::Cmd_User_Download_Start},
		{ CMD_BOOT_DELLIST,			&CRTDataSvr::Cmd_Boot_DelList},
		{ CMD_CLIUPT_UPLOAD,		&CRTDataSvr::Cmd_CliUpt_UpLoad},
		{ CMD_CLIUPT_GET_FILELIST,	&CRTDataSvr::Cmd_CliUpt_GetFileList},
		{ CMD_GAME_ICON,			&CRTDataSvr::Cmd_Game_Icon},
		{ CMD_VALIDATE,				&CRTDataSvr::Cmd_Validate},
		{ CMD_SYSOPT_SYNTIME,		&CRTDataSvr::Cmd_Sysopt_Syntime},
		{ CMD_GET_BARSKINS,			&CRTDataSvr::Cmd_Get_BarSkins},
		{ CMD_UPT_ICONLIST_VERSION, &CRTDataSvr::Cmd_Update_Iconlist_Version},
		{ CMD_UPT_CLIENT_INDEX,		&CRTDataSvr::Cmd_Update_Client_Index},
		{ CMD_GAME_CLIEXESQL,		&CRTDataSvr::Cmd_Game_CliExeSql},
		{ CMD_CON_GETINFO_FROM_GAMEUPDATESVR, &CRTDataSvr::Cmd_Con_GetInfo_From_GameUpdateSvr},
		{ CMD_GET_SERVER_CONFIG,	&CRTDataSvr::Cmd_Get_Server_Config},
		{ CMD_GET_VDISK_CONFIG,		&CRTDataSvr::Cmd_Get_VDisk_Config},
		{ CMD_VDSVR_GETUPDATE_FILEINFO, &CRTDataSvr::Cmd_VDisk_GetUpdateFileInfo},
		{ CMD_VDSVR_GETUPDATE_FILELIST, &CRTDataSvr::Cmd_VDisk_GetUpdateFileList},
		{ CMD_RELOAD_CACHE_FILE,	&CRTDataSvr::Cmd_Reload_Cache_File},
		{ CMD_NOTIFY_SAVE_DATA,		&CRTDataSvr::Cmd_Notify_Save_Data}
	};
}


void CRTDataSvr::OnRecv(INetLayer *pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
{
	NetClient_Ptr pClient;
	if (!m_ClientList.peek(sck, pClient))
		return;

	_packageheader header = {0};
	CPackageHelper inpackage((LPSTR)lpPackage, sizeof(_packageheader) + 4);

	inpackage.popPackageHeader(header);
	if (param == 1) 
	{
		pClient->SetType(header.Resv[0]);

		struct Pred : public std::unary_function<NetClient_Ptr, bool> {
			Pred(ZTableMgr *pTableMgr, SOCKET sck, ulong ip, int type) 
				: m_pTableMgr(pTableMgr), m_sck(sck), m_ip(ip), m_type(type) {}
			result_type operator()(const argument_type& _Arg) {
				if (_Arg->GetSocket() == m_sck)
					return false;
				if (_Arg->GetIP() == m_ip && _Arg->GetType() == m_type) {
					m_pTableMgr->GetClientStatusTable()->Delete(_Arg->GetSocket());
					return true;
				}
				return false;
			}
		private:
			ZTableMgr *m_pTableMgr;
			SOCKET m_sck;
			ulong m_ip;
			int m_type;
		};

		switch (pClient->GetType()) {
		case OBJECT_CLIENTSVI: {
			//查找同一个远端并删除
			Pred pred(m_pTableMgr, sck, pClient->GetIP(), pClient->GetType());
			m_ClientList.remove_if_value(pred);

			db::tClientStatus cs = { sck, pClient->GetIP(), 1 };
			SendClientStateMsgToCon(cs.IP, cs.Online, pClient->GetName());
			m_pTableMgr->GetClientStatusTable()->Ensure(&cs, 
				MASK_TCLIENTSTATUS_ONLINE | MASK_TCLIENTSTATUS_IP);
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]客户端连接[IP=%s].\n"), 
				this->GetPlugName(), pClient->GetIPAddress()));
			break; }
		case OBJECT_CONSOLE: {
			SetConsoleIP(pClient->GetIP());
			I8_INFOR((LM_INFO, I8_TEXT("[%s]控制台连接[IP=%s][sock=%d]\n"),
				this->GetPlugName(), pClient->GetIPAddress(),sck));
			break; }
		case OBJECT_VDISKSVI: {
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]虚拟盘连接[IP=%s].\n"), 
				this->GetPlugName(), pClient->GetIPAddress()));
			break; }
		case OBJECT_BARONLINE: {
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]菜单连接[IP=%s].\n"), 
				this->GetPlugName(), pClient->GetIPAddress()));
			break; }
		case OBJECT_SYNCTOOL: {
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]同步工具连接[IP=%s].\n"),
				this->GetPlugName(), pClient->GetIPAddress()));
			break; }
		default:
			I8_ASSERT(0);
			break;
		}
	}



	if( header.Cmd == CMD_CON_HEART_BEAT )
	{
		ACK();
		return;
	}

	for(size_t i = 0; i != _countof(cmdMap); ++i)
	{
		if( cmdMap[i].cmd == header.Cmd )
		{
			(this->*cmdMap[i].func)(pNetLayer, sck, lpPackage, inpackage, header);
			return;
		}
	}
		
	Cmd_Unknown(pNetLayer, sck, lpPackage,inpackage,header);

}

void CRTDataSvr::OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)
{
	NetClient_Ptr pClient;
	if (m_ClientList.get(sck, pClient)) {
		in_addr in;
		in.s_addr = pClient->GetIP();
		switch (pClient->GetType()) {
		case OBJECT_CLIENTSVI: {
			SendClientStateMsgToCon(pClient->GetIP(), 0, pClient->GetName());
			m_pTableMgr->GetClientStatusTable()->Delete(sck);
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]客户端连接关闭[IP=%s].\n"), 
				this->GetPlugName(), inet_ntoa(in)));
			break; }
		case OBJECT_CONSOLE: {
			SetConsoleIP(0);
			I8_INFOR((LM_INFO, I8_TEXT("[%s]控制台连接关闭[IP=%s][sosk=%d].\n"), 
				this->GetPlugName(), inet_ntoa(in),sck));
			break; }
		case OBJECT_VDISKSVI: {
			RemoveVDiskStatus(pClient->GetIP());
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]虚拟盘连接关闭[IP=%s].\n"), 
				this->GetPlugName(), inet_ntoa(in)));
			break; }
		case OBJECT_BARONLINE: {
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]菜单连接关闭[IP=%s].\n"), 
				this->GetPlugName(), inet_ntoa(in)));
			break; }
		case OBJECT_SYNCTOOL: {
			I8_DEBUG2((LM_DEBUG, I8_TEXT("[%s]同步工具连接关闭[IP=%s].\n"),
				this->GetPlugName(), inet_ntoa(in)));
			break; }
		default:
			I8_ASSERT(0);
			break;
		}
	}
}

void CRTDataSvr::RemoveVDiskStatus(ulong ip)
{
	struct Filter : public IVDiskStatusFilter {
		Filter(ulong ip) : m_ip(ip) {}
		bool bypass(const db::tVDiskStatus *const d) {
			return d->IP != m_ip;
		}
	private:
		ulong m_ip;
	} filter(ip);

	m_pTableMgr->GetVDiskStatusTable()->Delete(&filter); 
}

void CRTDataSvr::SendClientStateMsgToCon(DWORD dwIp, BOOL bOnLine, const char *lpszName)
{
	TEST_THIS_SECTION(__FUNCTION__);

	DWORD dwConIp = this->GetConsoleIP();
	if (dwConIp == 0) {
		return;
	}

	char buf[5000];
	CPackageHelper outpackage(buf, sizeof(buf));
	_packageheader header = {START_FLAG, CMD_USER_STATE_CHANGE, 0, 0x0200, {0}};

	outpackage << 0L; //crc

	bool Named = lpszName && lpszName[0];
	if (Named) {
		outpackage << lpszName;
	}

	if (!Named) {
		struct Filter : public IClientFilter {
			Filter(ulong ip) : m_ip(ip) {}
			bool bypass(const db::tClient *const d) {
				return d->IP != m_ip;
			}
		private:
			ulong m_ip;
		} filter(dwIp);
		
		IClientRecordset *pRecordset = 0;
		m_pTableMgr->GetClientTable()->Select(&pRecordset, &filter, 0);
		if (pRecordset->GetCount() == 1) {
			db::tClient *d = pRecordset->GetData(0);
			outpackage << d->Name;
			Named = true;
		}
		pRecordset->Release();
	}
	
	if (!Named) {
		struct in_addr addr;
		addr.s_addr = dwIp;
		struct hostent *remoteHost = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
		if (remoteHost == 0) {
			I8_DEBUG3((LM_DEBUG, 
				I8_TEXT("取客户机名(gethostbyaddr)失败[IP:%s,ERROR=%08x]\n"), 
				::inet_ntoa(addr), ::WSAGetLastError()));
			return;
		}
		outpackage.pushString(remoteHost->h_name,strlen(remoteHost->h_name));
	}

	outpackage.pushDWORD(bOnLine);
	header.Length = outpackage.GetOffset();

	DWORD *lpCrc = reinterpret_cast<DWORD *>(buf + sizeof(header));
	DWORD dwCrc = CalBufCRC32(reinterpret_cast<BYTE*>(buf), header.Length);
	*lpCrc = dwCrc;

	outpackage.pushPackageHeader(header);

	SOCKET s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (s == INVALID_SOCKET) {
		I8_ERROR((LM_WARNING, I8_TEXT("UDP套接字打开失败!\n")));
		return;
	}

	sockaddr_in address;
	int len = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = dwConIp;
	address.sin_port = htons(CONTROL_UDP_PORT);

	int iRet=sendto(s, (char*)buf, header.Length, 0, (sockaddr*)&address,len);
	if (iRet != header.Length) {
		I8_ERROR((LM_WARNING, I8_TEXT("发送UDP数据失败[TO:IP=%s]!\n"),
			::inet_ntoa(address.sin_addr)));
	}

	closesocket(s);
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
	if (::GetFileSize(lpFileName, ullSize) && (ullSize >> 32) == 0)
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
	if (GetFileCrc32(lpFileName, crc))
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
	if (::GetFileLastWriteTime(lpFileName, t))
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
	I8_DEBUG3((LM_DEBUG, 
		I8_TEXT("客户端请求文件传输:IP=%s, 文件名=%s\n"),
		this->GetIPAddress(), lpFileName));

#ifdef USED_I8_FILECACHE
	//I8_ASSERT(!m_CacheFile);
	if (dwDesiredAccess == GENERIC_READ ) {
		m_CacheFile = g_pPlug->GetFileCache()->Open(lpFileName);
		if (m_CacheFile) {
			m_dwOffset = 0;
			I8_DEBUG3((LM_DEBUG, 
				I8_TEXT("返回缓冲文件对象成功:IP=%s, 文件名=%s\n"),
				this->GetIPAddress(), lpFileName));
			return TRUE;
		}
	}
#endif //USED_I8_FILECACHE

	I8_ASSERT(m_hFile == INVALID_HANDLE_VALUE);
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
		I8_ASSERT(m_hFile == INVALID_HANDLE_VALUE);
		I8_DEBUG3((LM_DEBUG, I8_TEXT("关闭传输的文件:IP=%s, 文件名=%s\n"),
			this->GetIPAddress(), CacheFile->GetFileName().c_str()));
		m_CacheFile.reset(); //注意，关闭的是m_CacheFile
		return;
	}
#endif //USED_I8_FILECACHE

	I8_ASSERT(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE) {
		I8_DEBUG3((LM_DEBUG, I8_TEXT("关闭传输的文件:IP=%s, 文件句柄=%X\n"),
			this->GetIPAddress(), (DWORD)m_hFile));
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

BOOL NetClient::ReadFileData(BYTE *pbuf, DWORD len)
{
#ifdef USED_I8_FILECACHE
	if (CacheFile_Ptr CacheFile = m_CacheFile) {
		I8_ASSERT(m_hFile == INVALID_HANDLE_VALUE);
		I8_ASSERT(m_dwOffset + len <= CacheFile->GetSize());
		return CacheFile->ReadData(m_dwOffset, pbuf, len);
	}
#endif //USED_I8_FILECACHE

	I8_ASSERT(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE)
		return ReadFileContent(m_hFile, pbuf, len);
	return FALSE;
}

BOOL NetClient::WriteFileData(BYTE *pbuf, DWORD len)
{
	I8_ASSERT(m_hFile != INVALID_HANDLE_VALUE);
	if (m_hFile != INVALID_HANDLE_VALUE)
		return WriteFileContent(m_hFile, pbuf, len);
	return FALSE;
}

} //namespace i8desk

