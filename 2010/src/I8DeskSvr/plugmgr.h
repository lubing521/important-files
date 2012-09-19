#ifndef _i8desk_plugmgr_h_
#define _i8desk_plugmgr_h_

#define WRITE_LOG	1

#include "../../include/plug.h"
#include "../../include/Utility/SmartHandle.hpp"
#include <Shlwapi.h>
#include <map>
#include <list>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <sstream>
#include <shlobj.h>
#include <ShellAPI.h>
#include <set>
#include "resource.h"
#include <Userenv.h>

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

namespace i8desk
{
	//插件管理器，实现插件的管理，以及消息的统一分发
	class CPlugMgr : public IPlugMgr, public INetLayerEvent
	{
		typedef struct tagPlugEx
		{
			IPlug*		pPlug;
			INetLayer*	pNetLayer;		//如果接口需要监听端口，才有该指针，否则为空
		}PLUGEX;
	private:
		ILogger*   m_pLogger;								//统一的日志输出接口.
		IDatabase* m_pDatabase;								//统一数据库操作接口.
		ISysOpt*   m_pSysOpt;								//统一的系统选项接口.	

		std::map<tstring,     IPlug*> m_mapName2Plugs;		//所有的插件
		std::map<DWORD,    tagPlugEx> m_mapPort2Plugs;		//需要启动网络层的端口与插件的对应关系
		std::map<SOCKET, sockaddr_in> m_mapClient;			//客户端列表
		CLock m_lock;										//客户端列表锁
		typedef std::map<tstring,     IPlug*>::iterator	NAME2PLUGITER;
		typedef std::map<DWORD,    tagPlugEx>::iterator	PORT2PLUGITER;
		typedef std::map<SOCKET, sockaddr_in>::iterator	CLIENTITER;
		std::vector<IPlug *>	ClearPlug;
	public:
		CPlugMgr(ILogger* pLogger):m_pLogger(pLogger),m_pDatabase(NULL), m_pSysOpt(NULL)
		{
			CoInitialize(NULL);
		}
		virtual ~CPlugMgr()
		{
			CoUninitialize();
		}
		virtual void Release()	{ delete this; }
	public:
		//IPlugMgr实现了INetLayerEvent接口，是为增加一个灵活性
		//可以在这里进行统一的消息分发，目的是为了可以进行拦截消息
		virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)
		{
			try
			{
				PORT2PLUGITER it = m_mapPort2Plugs.find(pNetLayer->GetPort());
				if (it != m_mapPort2Plugs.end())
				{
					it->second.pPlug->OnAccept(pNetLayer, sck, param);
				}

				//Write Accept Log.
				/*CAutoLock<CLock> lock(&m_lock);
 				sockaddr_in addr = {0};
 				int nSize = sizeof(addr);
 				if (0 == getpeername(sck, (PSOCKADDR)&addr, &nSize))
 				{
 					m_mapClient.insert(std::make_pair(sck, addr));	
 					m_pLogger->WriteLog(LM_INFO, TEXT("socket %d,%s connected, count:%d."), 
 						sck, inet_ntoa(addr.sin_addr), m_mapClient.size());
 				}*/
			}
			catch ( ... ) { m_pLogger->WriteLog(LM_INFO, TEXT("OnAccept Try Exception Error.")); }
		}
		virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
		{
			try
			{
				PORT2PLUGITER it = m_mapPort2Plugs.find(pNetLayer->GetPort());
				if (it != m_mapPort2Plugs.end())
				{
					it->second.pPlug->OnSend(pNetLayer, sck, lpPackage, param);
				}

				//Write OnSend Log.
 				//_packageheader* pheader = (_packageheader*)lpPackage;
 				//CLIENTITER client = m_mapClient.find(sck);
 				//TCHAR buf[20] = {0};
 				//if (client != m_mapClient.end())
 				//{
 				//	char* p = inet_ntoa((in_addr)client->second.sin_addr);
 				//	if (p != NULL)
 				//	{
 				//		lstrcpy(buf, /*_bstr_t*/(p));
 				//		m_pLogger->WriteLog(LM_INFO, TEXT("ip:%s,send cmd:0x%x,length:0x%x"), 
 				//			buf, pheader->Cmd, pheader->Length);
 				//	}
 				//}
			}
			catch ( ... ) { m_pLogger->WriteLog(LM_INFO, TEXT("OnSend Try Exception Error.")); }
		}
		virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param)
		{
			try
			{
				PORT2PLUGITER it = m_mapPort2Plugs.find(pNetLayer->GetPort());
				if (it != m_mapPort2Plugs.end())
				{
					it->second.pPlug->OnRecv(pNetLayer, sck, lpPackage, param);
				}

				//Write Recv log.
 				//_packageheader* pheader = (_packageheader*)lpPackage;
 				//CLIENTITER client = m_mapClient.find(sck);
 				//TCHAR buf[20] = {0};
 				//if (client != m_mapClient.end())
 				//{
 				//	char* p = inet_ntoa((in_addr)client->second.sin_addr);
 				//	if (p != NULL)
 				//	{
 				//		lstrcpy(buf, /*_bstr_t*/(p));
 				//		m_pLogger->WriteLog(LM_INFO, TEXT("from ip:%s,recv cmd:0x%x,length:0x%x"), 
 				//			buf, pheader->Cmd, pheader->Length);
 				//	}
 				//}
			}
			catch ( ... ) { m_pLogger->WriteLog(LM_INFO, TEXT("OnRecv Try Exception Error.")); }
		}
		virtual void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param)
		{
			try
			{
				PORT2PLUGITER it = m_mapPort2Plugs.find(pNetLayer->GetPort());
				if (it != m_mapPort2Plugs.end())
				{
					it->second.pPlug->OnClose(pNetLayer, sck, param);
				}
				//Write Close log.
 				/*CAutoLock<CLock> lock(&m_lock);
 				TCHAR buf[1024] = {0};
 				CLIENTITER it2 = m_mapClient.find(sck);
 				if (it2 != m_mapClient.end())
 				{
 					char* p = inet_ntoa((in_addr)it2->second.sin_addr);
 					if (p != NULL)
 					{
 						m_pLogger->WriteLog(LM_INFO, TEXT("socket:%d,%s closed,count:%d"), 
 							sck, p, m_mapClient.size()-1);
 					}
 					m_mapClient.erase(it2);
				}*/
			}
			catch ( ... ) { m_pLogger->WriteLog(LM_INFO, TEXT("OnClose Try Exception Error.")); }
		}
	public:
		virtual IDatabase*	GetDatabase()	{ return m_pDatabase; }
		virtual ISysOpt*	GetSysOpt()		{ return m_pSysOpt;	  }
		virtual ILogger*	CreateLogger()	
		{
			ILogger* pLogger = NULL;
			i8desk::CreateLogger(&pLogger);
			return pLogger;
		}
		virtual IPlug*		FindPlug(LPCTSTR plugName)
		{
			CAutoLock<CLock> lock(&m_lock);
			NAME2PLUGITER it = m_mapName2Plugs.find(tstring(plugName));			
			if (it != m_mapName2Plugs.end())
				return it->second;
			return NULL;
		}

		HANDLE GetTokenFromIE()
		{
			HANDLE hToken = NULL;
			PROCESSENTRY32 pe32 = {0}; 
			i8desk::CAutoHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
			if (!hProcessSnap.IsValid()) 
				return NULL; 
			pe32.dwSize = sizeof(PROCESSENTRY32); 
			if (Process32First(hProcessSnap, &pe32)) 
			{ 
				do 
				{
					if(strcmpi(pe32.szExeFile,  "explorer.exe") == 0)
					{
						i8desk::CAutoHandle hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
							FALSE,pe32.th32ProcessID);
						OpenProcessToken(hProcess,TOKEN_ALL_ACCESS, &hToken);

						return hToken;
					}
				} 
				while (Process32Next(hProcessSnap, &pe32)); 
			} 
		
			return NULL;
		}

		void ProcessDatabase()
		{
			//创建一些软件运行需要的目录.
			CreateDirectory((GetAppPath() + "Data\\BootExec\\").c_str(), NULL);
			CreateDirectory((GetAppPath() + "Data\\WallPaper\\").c_str(), NULL);
			CreateDirectory((GetAppPath() + "Data\\Icon\\").c_str(), NULL);
			CreateDirectory((GetAppPath() + "BarOnlineUpdate\\").c_str(), NULL);
			CreateDirectory((GetAppPath() + "Tmp\\").c_str(), NULL);

			//09版本修正一些数据库结构
			m_pDatabase->ExecSql(TEXT("ALTER TABLE rGameArea DROP CONSTRAINT tGamerGameArea"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame DROP CONSTRAINT tCLASStGame"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient DROP CONSTRAINT tAreatClient"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE rGameArea DROP CONSTRAINT tArearGameArea"));

			if (m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add DownPriority LONG")))
			{
				m_pDatabase->ExecSql(TEXT("update tGame set DownPriority = 0"));
			}
			if (m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add SvrClick2 LONG")))
			{
				m_pDatabase->ExecSql(TEXT("update tGame set SvrClick2 = 0"));
			}
			m_pDatabase->ExecSql(TEXT("DROP TABLE tGameLog"));
			if (m_pDatabase->ExecSql(TEXT("ALTER TABLE tFavorite ADD Serial LONG ")))
			{
				_RecordsetPtr rs;
				if (m_pDatabase->ExecSql(TEXT("select UID, Name from tFavorite"), rs))
				{
					int nOrder = 1;
					char szSql[4096] = {0};
					while (rs->adoEOF == VARIANT_FALSE)
					{
						_bstr_t bstr = rs->GetCollect(1L);
						{
							_stprintf(szSql, TEXT("update tFavorite set Serial = %d where UID = \'%s\'"), 
								nOrder, (LPCTSTR)(_bstr_t)rs->GetCollect(0L));
							m_pDatabase->ExecSql(szSql);
							nOrder ++;
						}
						rs->MoveNext();
					}
				}
			}

			//10版本新处理的数据库结构
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame   Add AutoSync	LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame   Add GameSource STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tArea   Add SID STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tVDisk  Add SID STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add GPFreeSize LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add SCStatus LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add CPU STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add Memory STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add Disk STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add Graphics STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add Mainboard STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add NIC STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add Webcam STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add System STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add IEProt LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tClient Add FDogDriver LONG"));
			m_pDatabase->ExecSql(TEXT("CREATE TABLE tSyncGame (SID STRING(64), GID LONG, PRIMARY KEY(SID, GID))"));
			m_pDatabase->ExecSql(TEXT("CREATE TABLE tSyncTask (SID STRING(64) PRIMARY KEY, Name STRING, "
				"SyncType LONG, SyncIP STRING, VDiskIP STRING, UpdateIP STRING, DestDrv LONG, "
				"BalanceType LONG, MaxSyncSpeed LONG)"));


			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add url1 STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add url2 STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add url3 STRING"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tGame Add url4 STRING"));

			TCHAR szSql[4096] = {0};

			//插入默认分区
			_stprintf(szSql, TEXT("insert into tArea (AID, Name) values (\'%s\', \'%s\')"),
				DEFAULT_AREA_GUID, DEFAULT_AREA_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tArea set Name = \'%s\' where AID = \'%s\'"),
					DEFAULT_AREA_NAME, DEFAULT_AREA_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			//插入官方类别
			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_WL_GUID, CLASS_WL_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_WL_NAME, CLASS_WL_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_DJ_GUID, CLASS_DJ_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_DJ_NAME, CLASS_DJ_GUID);
				m_pDatabase->ExecSql(szSql);
			}
			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_XX_GUID, CLASS_XX_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_XX_NAME, CLASS_XX_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_WY_GUID, CLASS_WY_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_WY_NAME, CLASS_WY_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_DZ_GUID, CLASS_DZ_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_DZ_NAME, CLASS_DZ_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_QP_GUID, CLASS_QP_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_QP_NAME, CLASS_QP_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_PL_GUID, CLASS_PL_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_PL_NAME, CLASS_PL_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_LT_GUID, CLASS_LT_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_LT_NAME, CLASS_LT_GUID);
				m_pDatabase->ExecSql(szSql);
			}

			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_CY_GUID, CLASS_CY_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_CY_NAME, CLASS_CY_GUID);
				m_pDatabase->ExecSql(szSql);
			}
			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_YY_GUID, CLASS_YY_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_YY_NAME, CLASS_YY_GUID);
				m_pDatabase->ExecSql(szSql);
			}
			_stprintf(szSql, TEXT("insert into tClass (DefClass, Name, Path, Type) values (\'%s\', \'%s\', '', 0)"),
				CLASS_GP_GUID, CLASS_GP_NAME);
			if (!m_pDatabase->ExecSql(szSql))
			{
				_stprintf(szSql, TEXT("update tClass set Name = \'%s\' where DefClass = \'%s\'"),
					CLASS_GP_NAME, CLASS_GP_GUID);
				m_pDatabase->ExecSql(szSql);
			}
		}

		void SetOptDefautlValue()
		{
			//设置客户端控制密码初值
			if (0 == m_pSysOpt->GetOpt(OPT_M_CLIPWD, 1342400927L))	//"1234567"的CRC值
				m_pSysOpt->SetOpt(OPT_M_CLIPWD, 1342400927L);

			char buf[MAX_PATH] = {0};
			m_pSysOpt->GetOpt(OPT_U_CTLPWD, buf, "1234567");
			if (lstrlen(buf) == 0)
				m_pSysOpt->SetOpt(OPT_U_CTLPWD, "1234567");

			//设置下载的初始目录,临时目录和虚拟盘临时目录初始值
			m_pSysOpt->GetOpt(OPT_D_INITDIR, buf, "E:\\");
			if (lstrlen(buf) == 0)
				m_pSysOpt->SetOpt(OPT_D_INITDIR, "E:\\");
			m_pSysOpt->GetOpt(OPT_D_TEMPDIR, buf, "E:\\msctemp\\");
			if (lstrlen(buf) == 0)
				m_pSysOpt->SetOpt(OPT_D_TEMPDIR, "E:\\msctemp\\");

			m_pSysOpt->GetOpt(OPT_M_VDDIR, buf, TEXT("C:\\Temp\\"));
			if (lstrlen(buf) == 0)
				m_pSysOpt->SetOpt(OPT_M_VDDIR, TEXT("C:\\Temp\\"));
			m_pSysOpt->SetOpt(OPT_M_DEFAULTSIZE, 50L);
			m_pSysOpt->SetOpt(OPT_M_AUTOUPT, 1L);
		}

		//启动其它工具
		void StartOtherTool()
		{
			i8desk::CAutoHandle hToken = GetTokenFromIE();
			LPVOID lpEnv = NULL;
			if ( hToken.IsValid() )
				CreateEnvironmentBlock(&lpEnv, hToken, FALSE);

			TCHAR szExe[MAX_PATH] = {0};
			if (GetSysOpt()->GetOpt(OPT_M_SAFECENTER, false) && 
				GetSysOpt()->GetOpt(OPT_M_WHICHONE_SC, 0L) == 2)
			{
				_stprintf(szExe, TEXT("%sSafeSvr\\KHPrtSvr.exe"), GetAppPath().c_str());
				STARTUPINFO si = {0};
				PROCESS_INFORMATION pi = {0};
				si.cb= sizeof(STARTUPINFO);
				si.lpDesktop = TEXT("winsta0\\default");					
				if (CreateProcess(NULL, szExe, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi))
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					m_pLogger->WriteLog(LM_INFO, TEXT("start kungho server success."));
				}
				else { m_pLogger->WriteLog(LM_INFO, TEXT("start kungho server fail:%d."), GetLastError());}
			}else { m_pLogger->WriteLog(LM_INFO, TEXT("no start kungho server.")); }

			if (GetSysOpt()->GetOpt(OPT_M_IEHISTORY, false))
			{
				_stprintf(szExe, TEXT("%stools\\I8LogView.exe"), GetAppPath().c_str());
				STARTUPINFO si = {0};
				PROCESS_INFORMATION pi = {0};
				si.cb= sizeof(STARTUPINFO);
				si.lpDesktop = TEXT("winsta0\\default");
				if (CreateProcessAsUser(hToken, NULL, szExe, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi))
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					m_pLogger->WriteLog(LM_INFO, TEXT("start ie history tool success."));
				}
				else  { m_pLogger->WriteLog(LM_INFO, TEXT("start ie history tool  fail:%d."), GetLastError()); }
			} else { m_pLogger->WriteLog(LM_INFO, TEXT("no start ie history tool.")); }

			if (GetSysOpt()->GetOpt(OPT_M_USEUDISK, false))
			{
				_stprintf(szExe, TEXT("%sTools\\UDiskServer.exe"), GetAppPath().c_str());
				STARTUPINFO si = {0};
				PROCESS_INFORMATION pi = {0};
				si.cb= sizeof(STARTUPINFO);
				si.lpDesktop = TEXT("winsta0\\default");
				if (CreateProcessAsUser(hToken, NULL, szExe, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi))
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					m_pLogger->WriteLog(LM_INFO, TEXT("start udisk tool success."));
				}
				else  { m_pLogger->WriteLog(LM_INFO, TEXT("start udisk  tool  fail:%d."), GetLastError()); }
			} else { m_pLogger->WriteLog(LM_INFO, TEXT("no start udisk  tool.")); }
			if (lpEnv)
				DestroyEnvironmentBlock(lpEnv);
		}

		IPlug* LoadPlug(std::string& szPlugDLL)
		{
			HMODULE hMod = NULL;
			IPlug* pPlug = NULL;
			try
			{
				hMod = LoadLibrary(szPlugDLL.c_str());
				PFNCREATEPLUG pfnCreatePlug = (hMod == NULL) ? NULL : (PFNCREATEPLUG)GetProcAddress(hMod, "CreatePlug");
				if (hMod != NULL && pfnCreatePlug != NULL && (pPlug = pfnCreatePlug()) != NULL)
				{
					IMemPool*  pMemPool  = NULL;
					INetLayer* pNetLayer = NULL;
					bool bInitialize = pPlug->Initialize(this);
					bool bStartNetLayer = true;

					if (bInitialize && pPlug->GetListPort() != 0)
					{
						if (!CreateINetLayer(&pNetLayer) || !CreateIMemPool(&pMemPool))
						{
							m_pLogger->WriteLog(LM_INFO, TEXT("create netlayer fail."));
							bStartNetLayer = false;
						}
						else
						{
							pNetLayer->SetIMemPool(pMemPool);
							pNetLayer->SetINetLayerEvent(this);	//not pPlug
							if (0 != pNetLayer->Start((WORD)pPlug->GetListPort()))
							{
								m_pLogger->WriteLog(LM_INFO, TEXT("start netlayer fail."));
								bStartNetLayer = false;
							}
						}
						if (!bStartNetLayer)
						{
							Release_Interface(pNetLayer);
							Release_Interface(pMemPool);
						}
					}

					m_pLogger->WriteLog(LM_INFO, 
						TEXT("LoadPlug:Name=%s,Version=0x%08x,Company=%s,Description=%s,ret=%d"),
						pPlug->GetPlugName(),
						pPlug->GetVersion(),
						pPlug->GetCompany(),
						pPlug->GetDescription(),
						(bInitialize && bStartNetLayer) ? 1 : 0);

					if (bInitialize && bStartNetLayer)
					{
						m_mapName2Plugs.insert(std::make_pair(tstring(pPlug->GetPlugName()), pPlug));
						ClearPlug.push_back(pPlug);

						if (pPlug->GetListPort() != 0)
						{
							tagPlugEx plugEx = {pPlug ,pNetLayer};
							m_mapPort2Plugs.insert(std::make_pair(pPlug->GetListPort(), plugEx));
						}
					}
					else
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("loadPlug fail:%s"), pPlug->GetPlugName());
						if (bInitialize)
						{
							pPlug->UnInitialize();
						}
						Release_Interface(pPlug);
					}
					return pPlug;
				}
				else
				{
					DWORD dwError = GetLastError();
					_com_error Error(dwError);
					if (hMod != NULL)	FreeLibrary(hMod);
					m_pLogger->WriteLog(LM_INFO, TEXT("Load Plug Fail:%s:%d:%s"), 
						szPlugDLL.c_str(), dwError, Error.ErrorMessage());
					return NULL;
				}
			}
			catch (...) 
			{ 
				m_pLogger->WriteLog(LM_INFO, TEXT("LoadPlug Unknow Error:%s"), szPlugDLL.c_str());
				return false;
			}
			return pPlug;
		}

		virtual bool Start()
		{
			//创建数据库组件,并进行压缩，然后打开数据库,最后进行数据库的结构的调整．
			m_pLogger->WriteLog(LM_INFO, "process data interface.");
			CreateDatabase(&m_pDatabase);
			tstring dbsrc = GetAppPath() + TEXT("Data\\I8Desk.mdb");
			if (!m_pDatabase->CompactDatabase(dbsrc.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("%s"), m_pDatabase->GetErrInfo());
			}
			tstring szConnString = TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=") + dbsrc;
			if (!m_pDatabase->Open(szConnString.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Open Database Fail:%s:%s"), dbsrc.c_str(), m_pDatabase->GetErrInfo());
				return false;
			}
			ProcessDatabase();

			//创建数据库缓存组件.
			m_pLogger->WriteLog(LM_INFO, "start all plug.");
			IPlug *pPlug = LoadPlug(GetAppPath() + RTDATASVR_MODULE_NAME);
			IRTDataSvr *pRTDataSvr = (pPlug == NULL) ? NULL :
				reinterpret_cast<IRTDataSvr*>(pPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
			if (pRTDataSvr == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("load rtdatasvr fail."));
				return false;
			}

			//创建系统选项组件,以及设置一些数据库的值.
			CreateSysOpt(&m_pSysOpt);
			m_pSysOpt->SetIDatabase(pRTDataSvr->GetSysOptTable());
			SetOptDefautlValue();

			//加载所有内部插件.
			if (NULL == LoadPlug(GetAppPath() + LOGRPT_MODULE_NAME)||
				NULL == LoadPlug(GetAppPath() + GAMEMGR_MODULE_NAME) ||
				NULL == LoadPlug(GetAppPath() + GAMEUPDATE_MODULE_NAME))
			{
				m_pLogger->WriteLog(LM_INFO, "Load I8Desk Plug Error.");
			}
			
			//加载所有第三方插件.
			TCHAR szPlugList[4096] = {0};
			DWORD dwValue = GetPrivateProfileSection(TEXT("Plugin"), szPlugList, sizeof(szPlugList), 
				(GetAppPath() + TEXT("Plugin\\plugin.ini")).c_str());
			if (dwValue < sizeof(szPlugList))
			{
				LPTSTR p = szPlugList;
				while (p - szPlugList < (int)dwValue)
				{
					tstring szPlug(p);
					LoadPlug(GetAppPath() + szPlug.substr(szPlug.find(TEXT('='))+1));
					p += szPlug.size() + 1;
				}
			}

			StartOtherTool();

			return true;
		}

		virtual bool Stop()
		{
			//停止所有插件启动的网络层
			for (PORT2PLUGITER it = m_mapPort2Plugs.begin(); it != m_mapPort2Plugs.end(); it++)
			{
				INetLayer* pNetLayer = it->second.pNetLayer;
				IMemPool*  pMemPool  = pNetLayer->GetIMemPool();
//				m_pLogger->WriteLog(LM_INFO, "停止所有插件启动的网络层.");
				pNetLayer->Stop();
			}

			//停止所有的插件x2

			for(int i = ClearPlug.size() - 1; i >=0; --i)
			{
				ClearPlug[i]->UnInitialize();
			}
			for(int i = ClearPlug.size() - 1; i >=0; --i)
			{
				ClearPlug[i]->Release();
			}
			//for (NAME2PLUGITER it = m_mapName2Plugs.begin(); it != m_mapName2Plugs.end(); it++)
			//{
			//	it->second->UnInitialize();
			//	m_pLogger->WriteLog(LM_INFO, "停止所有的插件x2.");
			//}

			//for (NAME2PLUGITER it = m_mapName2Plugs.begin(); it != m_mapName2Plugs.end(); it++)
			//{
			//	it->second->Release();
			//	m_pLogger->WriteLog(LM_INFO, "clear all m_mapName2Plugs.");
			//}

			for (PORT2PLUGITER it = m_mapPort2Plugs.begin(); it != m_mapPort2Plugs.end(); it++)
			{
				INetLayer* pNetLayer = it->second.pNetLayer;
				IMemPool*  pMemPool  = pNetLayer->GetIMemPool();
				Release_Interface(pNetLayer);
				Release_Interface(pMemPool);
//				m_pLogger->WriteLog(LM_INFO, "clear all m_mapPort2Plugs.");
			}
			m_mapPort2Plugs.clear();

			if (m_pDatabase)
				m_pDatabase->Close();
			Release_Interface(m_pSysOpt);
			Release_Interface(m_pDatabase);
//			m_pLogger->WriteLog(LM_INFO, "clear all m_pDatabase.");
			return true;
		}
	};
}

#endif