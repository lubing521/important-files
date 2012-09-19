#ifndef _i8desk_plugmgr_h_
#define _i8desk_plugmgr_h_

#pragma once

#include "../../include/frame.h"
#include "../../include/Utility/SmartHandle.hpp"
#include "../../include/MultiThread/Lock.hpp"
#include "../../include/Utility/utility.h"

#include "../../include/Win32/WinService/win32svr.h"
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <Userenv.h>
#include <vector>
#include <map>
#include <list>
#include <set>

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace i8desk
{
	//插件管理器，实现插件的管理，以及消息的统一分发
	class CPlugMgr : public ISvrPlugMgr, public INetLayerEvent
	{
		typedef struct tagPlugEx
		{
			ISvrPlug*		pPlug;
			INetLayer*	pNetLayer;							//如果接口需要监听端口，才有该指针，否则为空
		}PLUGEX;

		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

	private:
		ILogger*   m_pLogger;								//统一的日志输出接口.
		IDatabase* m_pDatabase;								//统一数据库操作接口.
		ISysOpt*   m_pSysOpt;								//统一的系统选项接口.	

		std::map<stdex::tString,  ISvrPlug*> m_mapName2Plugs;		//所有的插件
		std::map<DWORD,    tagPlugEx> m_mapPort2Plugs;		//需要启动网络层的端口与插件的对应关系
		std::map<DWORD,	 sockaddr_in> m_mapClient;			//客户端列表
		Mutex m_lock;										//客户端列表锁
		typedef std::map<stdex::tString,  ISvrPlug*>::iterator	NAME2PLUGITER;
		typedef std::map<DWORD,    tagPlugEx>::iterator	PORT2PLUGITER;
		typedef std::map<DWORD, sockaddr_in>::iterator	CLIENTITER;
		std::vector<ISvrPlug *>	ClearPlug;
	public:
		CPlugMgr(ILogger* pLogger):m_pLogger(pLogger),m_pDatabase(NULL), m_pSysOpt(NULL)
		{
			CoInitialize(NULL);
			CreateDirectory((utility::GetAppPath() + TEXT("Data\\BootExec\\")).c_str(),	NULL);
			CreateDirectory((utility::GetAppPath() + TEXT("Data\\WallPaper\\")).c_str(), NULL);
			CreateDirectory((utility::GetAppPath() + TEXT("Data\\ClassIcon\\")).c_str(), NULL);
			CreateDirectory((utility::GetAppPath() + TEXT("plugin_tools\\")).c_str(), NULL);
			CreateDirectory((utility::GetAppPath() + TEXT("Business\\")).c_str(), NULL);
		}

		virtual ~CPlugMgr()
		{
			Stop();
			CoUninitialize();
		}
		virtual void Release()	{ delete this; }
	public:
		virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param)
		{
			try
			{
				PORT2PLUGITER it = m_mapPort2Plugs.find(pNetLayer->GetPort());
				if (it != m_mapPort2Plugs.end())
				{
					it->second.pPlug->OnAccept(pNetLayer, sck, param);
				}
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
		virtual ISvrPlug* FindPlug(LPCTSTR plugName)
		{
			AutoLock lock(m_lock);
			NAME2PLUGITER it = m_mapName2Plugs.find(stdex::tString(plugName));			
			if (it != m_mapName2Plugs.end())
				return it->second;
			return NULL;
		}

		HANDLE GetTokenFromProcName(LPCTSTR lpProcName = TEXT("explorer.exe"))
		{
			HANDLE hToken = NULL;
			PROCESSENTRY32 pe32 = {0};
			utility::CAutoHandle hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
			if (!hProcessSnap.IsValid())
				return NULL; 
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hProcessSnap, &pe32))
			{ 
				do
				{
					if(lstrcmpi(pe32.szExeFile, lpProcName) == 0)
					{
						utility::CAutoHandle hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
							FALSE,pe32.th32ProcessID);
						OpenProcessToken(hProcess,TOKEN_ALL_ACCESS, &hToken);

						return hToken;
					}
				} 
				while (Process32Next(hProcessSnap, &pe32)); 
			} 
		
			return NULL;
		}

		bool CreateDatabaseInterface()
		{
			//create Database Interface.
			if (!CreateDatabase(&m_pDatabase))
				return false;
			stdex::tString dbsrc = utility::GetAppPath() + TEXT("Data\\i8desk2011.mdb");
			if (!m_pDatabase->CompactDatabase(dbsrc.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("%s"), m_pDatabase->GetErrInfo());
			}
			stdex::tString szConnString = TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=") + dbsrc;
			if (!m_pDatabase->Open(szConnString.c_str()))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Open Database Fail:%s"), m_pDatabase->GetErrInfo());
				return false;
			}

		
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tblGame Add SvrClick2 LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tblSyncTask Add Status LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tblVdisk Add [Size] LONG"));
			m_pDatabase->ExecSql(TEXT("ALTER TABLE tblVdisk Add SsdDrv LONG"));
			m_pDatabase->ExecSql(TEXT("CREATE TABLE tblPushGameStatic (GID LONG PRIMARY KEY NOT NULL, [Size] LONG, BeginDate LONG, EndDate LONG)"));

			
			struct pair {LPCTSTR table; LPCTSTR field1; LPCTSTR field2; LPCTSTR Value1; LPCTSTR Value2;};
			pair array[] = 
			{
				{TEXT("tblArea"),  TEXT("AID"),	TEXT("Name"), DEFAULT_AREA_GUID, DEFAULT_AREA_NAME},

				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_WL_GUID, CLASS_WL_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_DJ_GUID, CLASS_DJ_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_XX_GUID, CLASS_XX_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_WY_GUID, CLASS_WY_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_DZ_GUID, CLASS_DZ_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_QP_GUID, CLASS_QP_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_PL_GUID, CLASS_PL_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_LT_GUID, CLASS_LT_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_CY_GUID, CLASS_CY_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_YY_GUID, CLASS_YY_NAME},
				{TEXT("tblClass"), TEXT("CID"), TEXT("Name"), CLASS_GP_GUID, CLASS_GP_NAME},
			};

			//write database initialize value.
			for (int idx=0; idx<_countof(array); idx++)
			{
				TCHAR szSql[4096] = {0};
				_stprintf(szSql, TEXT("select %s, %s from %s where %s = \'%s\'"),
					array[idx].field1, array[idx].field2, array[idx].table, array[idx].field1, array[idx].Value1);

				_RecordsetPtr rcd;
				if (!m_pDatabase->ExecSql(szSql, rcd))
					return false;

				if (rcd->GetRecordCount() == 0)
				{
					_stprintf(szSql, TEXT("insert into %s (%s, %s) values (\'%s\', \'%s\')"),
						array[idx].table, array[idx].field1, array[idx].field2,
						array[idx].Value1, array[idx].Value2);
					m_pDatabase->ExecSql(szSql);
				}
				else
				{
					_variant_t var = rcd->GetCollect(1L);
					if (var.vt == VT_NULL || lstrcmpi((LPCTSTR)(_bstr_t)(var), array[idx].Value2) != 0)
					{
						_stprintf(szSql, TEXT("update %s set %s = \'%s\' where %s = \'%s\'"),
							array[idx].table, array[idx].field2, array[idx].Value2, array[idx].field1, array[idx].Value1);
						m_pDatabase->ExecSql(szSql);
					}
				}
				rcd->Close();
				rcd.Release();
			}
			return true;
		}

		bool CreateSysOptInterface(ISvrPlug* pPlug)
		{
			//create SysOpt Interface.
			IRTDataSvr *pRTDataSvr = (IRTDataSvr*)(pPlug->SendMessage(RTDS_CMD_GET_INTERFACE, RTDS_INTERFACE_RTDATASVR, 0));
			if (pRTDataSvr == NULL || !CreateSysOpt(&m_pSysOpt) )
				return false;
			
			m_pSysOpt->SetIDatabase(pRTDataSvr->GetSysOptTable());
			stdex::tString netbarjpgpath = utility::GetAppPath() + _T("Skin\\NetBar\\netbar.png");

			struct pair { bool	IsString;	LPCTSTR	Name;	LPCTSTR	pValue;};
			pair array[] = 
			{
				{true, OPT_M_CLIPWD,	TEXT("2361622901")	},		//客户端控制的密码初值:"1234567"的CRC值
				{false, OPT_M_AUTOUPT,	TEXT("1")			},		//客户端软件自动升级
				{true,	OPT_U_CTLPWD,	TEXT("1234567")		},		//控制台密码的初值
				{true,	OPT_D_INITDIR,	TEXT("E:\\")		},		//三层下载的初始目录
				{true,	OPT_D_TEMPDIR,	TEXT("E:\\msctmp")	},		//三层下载临时目录的初值
				{true,	OPT_M_VDDIR,	TEXT("C:\\Temp")	},		//虚拟盘的临时目录
				{false, OPT_D_TASKNUM,	TEXT("3")			},		//同时下载的最大任务数 (3)
				{true, OPT_M_CPU_TEMPRATURE,	TEXT("60")	},		//CPU 报警温度
				{false, OPT_M_BROWSEDIR,TEXT("1")			},		//允许右键浏览目录
				{true,	OPT_M_NETBARJPG,netbarjpgpath.c_str()	},		//客户端显示网吧外观图片
				{true,	OPT_M_CLISKIN,	TEXT("默认|默认.png")	},		//客户端皮肤

			};

			//write database initialize value.
			for (int idx=0; idx<_countof(array); idx++)
			{
				if (array[idx].IsString)
				{
					TCHAR Value[MAX_SYSOPT_LEN] = {0};
					m_pSysOpt->GetOpt(array[idx].Name, Value, array[idx].pValue);
					if (lstrlen(Value) == 0)
						m_pSysOpt->SetOpt(array[idx].Name, array[idx].pValue);
				}
				else
				{
					if (0 == m_pSysOpt->GetOpt(array[idx].Name, 0L))
						m_pSysOpt->SetOpt(array[idx].Name, (long)_ttoi(array[idx].pValue));
				}
			}

			long time = 0 ; 
			if( m_pSysOpt->GetOpt(OPT_M_FIRSTTIME, 0L ) == 0 )
			{
				time = _time32(NULL);
				m_pSysOpt->SetOpt(OPT_M_FIRSTTIME, (long)time);
			}
			
			StartOtherTool();
			return true;
		}

		void StartOtherTool()
		{
			utility::CAutoHandle hToken = GetTokenFromProcName();
			LPVOID lpEnv = NULL;
			if (hToken.IsValid())
				CreateEnvironmentBlock(&lpEnv, hToken, FALSE);

			TCHAR szExe[MAX_PATH] = {0};

			if (GetSysOpt()->GetOpt(OPT_M_SAFECENTER, false) && 
				GetSysOpt()->GetOpt(OPT_M_WHICHONE_SC, 0L) == i8desk::KONGHE)
			{
				_stprintf(szExe, TEXT("%sSafeSvr\\KHPrtSvr.exe"), utility::GetAppPath().c_str());
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


			if (GetSysOpt()->GetOpt(OPT_M_SAFECENTER, false) && 
				GetSysOpt()->GetOpt(OPT_M_WHICHONE_SC, 0L) == i8desk::DISKLESS)
			{
				_stprintf(szExe, TEXT("%sSafeSvr\\Server\\UndiskSafe.exe"), utility::GetAppPath().c_str());
				STARTUPINFO si = {0};
				PROCESS_INFORMATION pi = {0};
				si.cb= sizeof(STARTUPINFO);
				si.lpDesktop = TEXT("winsta0\\default");					
				if (CreateProcess(NULL, szExe, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, lpEnv, NULL, &si, &pi))
				{
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					m_pLogger->WriteLog(LM_INFO, TEXT("start diskless kungho server success."));
				}
				else { m_pLogger->WriteLog(LM_INFO, TEXT("start diskless kungho server fail:%d."), GetLastError());}
			}else { m_pLogger->WriteLog(LM_INFO, TEXT("no start diskless kungho server.")); }


			if (GetSysOpt()->GetOpt(OPT_M_IEHISTORY, false))
			{
				_stprintf(szExe, TEXT("%stools\\I8LogView.exe"), utility::GetAppPath().c_str());
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
				_stprintf(szExe, TEXT("%sTools\\UDiskServer.exe"), utility::GetAppPath().c_str());
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

		ISvrPlug* LoadPlug(stdex::tString& szPlugDLL, DWORD reserved = 0)
		{
			HMODULE hMod = NULL;
			ISvrPlug* pPlug = NULL;
			try
			{
				hMod = LoadLibrary(szPlugDLL.c_str());
				PFNCREATESVRPLUG pfnCreatePlug = (hMod == NULL) ? NULL : (PFNCREATESVRPLUG)GetProcAddress(hMod, "CreateSvrPlug");
				if (hMod != NULL && pfnCreatePlug != NULL && (pPlug = pfnCreatePlug(reserved)) != NULL)
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
							pNetLayer->SetINetLayerEvent(this);
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
						m_mapName2Plugs.insert(std::make_pair(stdex::tString(pPlug->GetPlugName()), pPlug));
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
			m_pLogger->WriteLog(LM_INFO, TEXT("start open database."));
			if (!CreateDatabaseInterface())
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("create IDatabase Interface fail."));
				return false;
			}

			m_pLogger->WriteLog(LM_INFO, TEXT("start all plug."));
			ISvrPlug *pPlug = LoadPlug(utility::GetAppPath() + PLUG_RTDATASVR_MODULE);
			if (pPlug == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("create IRTDataSvr Interface fail."));
				return false;
			}

			if (!CreateSysOptInterface(pPlug))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("create ISysOpt Interface fail."));
				return false;
			}

 			if( NULL == LoadPlug(utility::GetAppPath() + PLUG_LOGRPT_MODULE) )
 				m_pLogger->WriteLog(LM_INFO, TEXT("Load LogRpt Plug Error."));

			if( NULL == LoadPlug(utility::GetAppPath() + PLUG_GAMEMGR_MODULE) )
				m_pLogger->WriteLog(LM_INFO, TEXT("Load GameMgr Plug Error."));

			if( NULL == LoadPlug(utility::GetAppPath() + PLUG_GAMEUPDATE_MODULE, (DWORD)PLUG_UPTSVR_NAME) )
				m_pLogger->WriteLog(LM_INFO, TEXT("Load GameUpdate Plug Error."));

			if( NULL == LoadPlug(utility::GetAppPath() + PLUG_PLUGTOOL_MODULE) )
				m_pLogger->WriteLog(LM_INFO, TEXT("Load PlugTool Plug Error."));

			if( NULL == LoadPlug(utility::GetAppPath() + PLUG_BUSINESS_MODULE) )
				m_pLogger->WriteLog(LM_INFO, TEXT("Load Business Plug Error."));
			
			
			TCHAR szPlugList[4096] = {0};
			DWORD dwValue = GetPrivateProfileSection(TEXT("Plugin"), szPlugList, sizeof(szPlugList), 
				(utility::GetAppPath() + TEXT("Plugin\\plugin.ini")).c_str());
			if (dwValue < sizeof(szPlugList))
			{
				LPTSTR p = szPlugList;
				while (p - szPlugList < (int)dwValue)
				{
					stdex::tString szPlug(p);
					LoadPlug(utility::GetAppPath() + szPlug.substr(szPlug.find(TEXT('='))+1));
					p += szPlug.size() + 1;
				}
			}
			return true;
		}

		virtual bool Stop()
		{
			for (PORT2PLUGITER it = m_mapPort2Plugs.begin(); it != m_mapPort2Plugs.end(); it++)
			{
				INetLayer* pNetLayer = it->second.pNetLayer;
				IMemPool*  pMemPool  = pNetLayer->GetIMemPool();
				pNetLayer->Stop();
			}
			for(int i = ClearPlug.size() - 1; i >=0; --i)
			{
				ClearPlug[i]->UnInitialize();
			}
			for(int i = ClearPlug.size() - 1; i >=0; --i)
			{
				ClearPlug[i]->Release();
			}

			for (PORT2PLUGITER it = m_mapPort2Plugs.begin(); it != m_mapPort2Plugs.end(); it++)
			{
				INetLayer* pNetLayer = it->second.pNetLayer;
				IMemPool*  pMemPool  = pNetLayer->GetIMemPool();
				Release_Interface(pNetLayer);
				Release_Interface(pMemPool);
			}
			m_mapPort2Plugs.clear();
			m_mapName2Plugs.clear();
			ClearPlug.clear();

			if (m_pDatabase)
				m_pDatabase->Close();
			Release_Interface(m_pSysOpt);
			Release_Interface(m_pDatabase);
			return true;
		}
	};
}

#endif