#include "stdafx.h"
#include "plugmgr.h"

#include <fstream>

#include "Serialize.h"



const char *VDiskFile = "Data/VDisk.dat";
namespace i8desk
{
	

	CPlugMgr::CPlugMgr(ILogger* pLogger) 
		: m_pLogger(pLogger)
		, m_bInnerUpdate(0)
		, m_hThread(NULL)
		, m_hExited(NULL)
		, update_(NULL)
		, sock_(NULL)
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CPlugMgr::~CPlugMgr()	
	{ Stop(); }


	IPlug* CPlugMgr::FindPlug(LPCTSTR plugName)
	{
		CAutoLock<CLock> lock(&m_lock);
		std::map<tstring, IPlug*>::iterator it = m_mapPlugs.find(tstring(plugName));			
		if (it != m_mapPlugs.end())
			return it->second;
		return NULL;
	}

	bool CPlugMgr::Start()
	{
		if (m_pLogger == NULL)
			return false;

		//加载所有的插件
		{
			//仅从服务器的程序才加载内网更新组件。
			bool bLoadUpdate = false;
			m_bInnerUpdate = 0;
			{
				std::string szIniFile = GetAppPath() + CONFIG_FILE;
				char szIp[MAX_PATH] = {0};
				GetPrivateProfileStringA("SYSTEM", "SvrIp", "",  szIp, MAX_PATH, szIniFile.c_str());
				if (lstrlen(szIp) != 0 && !IsLocalMachine(szIp))
					bLoadUpdate = true;
			}

			if (NULL == LoadPlug(GetAppPath() + VDISK_MODULE_NAME, this))
			{
				m_pLogger->WriteLog(LM_INFO, "Load %s Plug Error.", VDISK_PLUG_NAME);
				return false;
			}
			if (bLoadUpdate)
			{
				if (NULL == LoadPlug(GetAppPath() + GAMEUPDATE_MODULE_NAME, NULL))
				{
					m_pLogger->WriteLog(LM_INFO, "Load %s Plug Error.", GAMEUPDATE_PLUG_NAME);
					return false;
				}
				m_bInnerUpdate = 1;
			}
			if (NULL == LoadPlug(GetAppPath() + SYNC_MODULE_NAME, this))
			{
				m_pLogger->WriteLog(LM_INFO, "Load %s Plug Error.", SYNC_PLUG_NAME);
				return false;
			}
		}

		sock_ = new CSocket(FindPlug(VDISK_PLUG_NAME), m_pLogger, m_hExited);
		sock_->CreateSocket();
		if( sock_->IsConnect() )
		{
			update_ = new AutoUpdateFile(m_pLogger, *sock_);
			update_->Start();
		}
		else
		{
			m_pLogger->WriteLog(LM_INFO, "没有连接上主服务，不能自动更新");
		}


		//创建工作线程
		{
			
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create WorkThread:%s"), (m_hThread != NULL) ? TEXT("Success.") : TEXT("Fail"));		
			m_pLogger->WriteLog(LM_INFO, TEXT("Service already started."));
			if (m_hThread == NULL)
				return false;
		}
		return true;
	}

	bool CPlugMgr::Stop()
	{
		if( update_ != NULL )
		{
			update_->Stop();
		}

		if( sock_ != NULL )
		{
			sock_->CloseSocket();
		}

		//停止线程.
		if (m_hExited != NULL)
		{
			if (m_hThread != NULL)
			{
				SetEvent(m_hExited);
				WaitForSingleObject(m_hThread, INFINITE);
				CloseHandle(m_hThread);
			}
			CloseHandle(m_hExited);
		}
		m_hThread = NULL;
		m_hExited = NULL;

		//停止所有插件启动的网络层
		for (std::map<tstring, IPlug*>::iterator it = m_mapPlugs.begin(); it != m_mapPlugs.end(); it++)
		{
			it->second->UnInitialize();
		}

		for (std::map<tstring, IPlug*>::iterator it = m_mapPlugs.begin(); it != m_mapPlugs.end(); it++)
		{
			it->second->Release();
		}
		m_mapPlugs.clear();
		return true;
	}

	BOOL CPlugMgr::IsLocalMachine(LPCTSTR lpszIP)
	{
		DWORD ip = inet_addr(lpszIP);
		if ((ip & 0xff) == 127)
			return TRUE;

		IP_ADAPTER_INFO info[16] = {0};
		DWORD dwSize = sizeof(info);
		if (ERROR_SUCCESS != GetAdaptersInfo(info, &dwSize))
			return TRUE;

		PIP_ADAPTER_INFO pAdapter = info;
		while (pAdapter != NULL)
		{
			PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
			while (pAddr != NULL)
			{
				DWORD dwIp = inet_addr(pAddr->IpAddress.String);
				if (dwIp == ip)
					return TRUE;
				pAddr = pAddr->Next;
			}
			pAdapter = pAdapter->Next;
		}
		return FALSE;
	}

	IPlug* CPlugMgr::LoadPlug(std::string& szPlugDLL, IPlugMgr* pPlugMgr)
	{
		HMODULE hMod = NULL;
		IPlug* pPlug = NULL;
		try
		{
			hMod = LoadLibrary(szPlugDLL.c_str());
			PFNCREATEPLUG pfnCreatePlug = (hMod == NULL) ? NULL : (PFNCREATEPLUG)GetProcAddress(hMod, "CreatePlug");
			if (hMod != NULL && pfnCreatePlug != NULL && (pPlug = pfnCreatePlug()) != NULL)
			{
				bool bInitialize = pPlug->Initialize(pPlugMgr);
				m_pLogger->WriteLog(LM_INFO,
					TEXT("LoadPlug:Name=%s,Version=0x%08x,Company=%s,Description=%s,ret=%d"),
					pPlug->GetPlugName(),
					pPlug->GetVersion(),
					pPlug->GetCompany(),
					pPlug->GetDescription(),
					(bInitialize ? 1 : 0));

				if (bInitialize)
				{
					//没有需要监听的端口。
					m_mapPlugs.insert(std::make_pair(tstring(pPlug->GetPlugName()), pPlug));
				}
				else
				{
					Release_Interface(pPlug);
				}

				return pPlug;
			}
		}
		catch (...)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("LoadPlug Unknow Error:%s"), szPlugDLL.c_str());
			return NULL;
		}
		return pPlug;
	}

	UINT __stdcall CPlugMgr::WorkThread(LPVOID lpVoid)
	{
		CPlugMgr* pThis = reinterpret_cast<CPlugMgr*>(lpVoid);
		i8desk::ILogger* pLogger = pThis->m_pLogger;
		CSocket &_Socket = *(pThis->sock_);

		CEvenyOneSD sd;
		HANDLE  hEvent[3];
		hEvent[0] = pThis->m_hExited;
		hEvent[1] = CreateEvent(NULL, FALSE, TRUE, NULL);
		hEvent[2] = CreateEvent(sd.GetSA(), FALSE, FALSE, VDISK_EVENT_NAME);
		if (hEvent[2] == NULL)
			pLogger->WriteLog(LM_INFO, "Create Event Fail:%d", GetLastError());

		std::map<DWORD, tagVDInfoEx*> VDList;

		// 虚拟盘离线数据文件
		static i8desk::std_string strPath = pThis->GetAppPath() + VDiskFile;
		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(3, hEvent, FALSE, HEART_INTERVAL);
			if (dwRet == WAIT_OBJECT_0)
				break;

			//刷新虚拟盘
			else if (dwRet == WAIT_OBJECT_0 + 2)
			{
				pLogger->WriteLog(LM_INFO, "Refresh Virtual Disk.");
				std::map<DWORD, tagVDInfoEx*>::iterator it = VDList.begin();
				for (; it != VDList.end(); it++)
				{
					if (it->second->handle != NULL)
					{
						IPlug* pPlug = pThis->FindPlug(VDISK_PLUG_NAME);
						if (pPlug != NULL)
							pPlug->SendMessage(VDISK_CMD_REFRESH, (DWORD)it->second->handle, 0);
					}
				}
			}
			//连接主服务器取出虚拟盘列表，并相应的处理，以及报告状态
			else if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
			{
				//__asm {int 3 }

				if (!_Socket.IsConnect())
					_Socket.CreateSocket();
				else
				{
					if( pThis->update_ == NULL )
					{
						pThis->update_ = new AutoUpdateFile(pThis->m_pLogger, _Socket);
						pThis->update_->Start();
					}
				}

				if( pThis->update_ != NULL && !pThis->update_->NeedUpdate() )
					continue;

				if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
					break;

				//get vdisk list.
				if(_Socket.IsConnect())
				{
					// 获取服务器当前数据
					std::map<DWORD, tagVDInfoEx*> newVDList;
					if (_Socket.GetVDiskList(newVDList))
					{

						// 写入离线数据
						std::ofstream out(strPath.c_str(), std::ios::ios_base::out | std::ios::ios_base::binary);
						out << newVDList;

						_Socket.ProcessVDisk(VDList, newVDList);
						_Socket.FreeVDisk(newVDList);
					}
					else
					{
						pLogger->WriteLog(LM_INFO, TEXT("获取虚拟盘列表出错"));
					}
				}
				else
				{
					pLogger->WriteLog(LM_INFO, TEXT("未连接上主服务"));

					// 从离线数据读取
					std::ifstream in(strPath.c_str(), std::ios::ios_base::in | std::ios::ios_base::binary);

					std::map<DWORD, tagVDInfoEx*> newVDList;
					in >> newVDList;

					_Socket.ProcessVDisk(VDList, newVDList);
					_Socket.FreeVDisk(newVDList);
				}

				if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
					break;

				//report vdisk status.
				if (_Socket.IsConnect()) //_Socket.ReportStatus();
				{
					_Socket.ReportVDiskStatus(VDList);
					_Socket.ReportServerStatus(VDList, pThis->m_bInnerUpdate, pThis->FindPlug(SYNC_PLUG_NAME));
				}
			}
			//出现错误了。
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				pLogger->WriteLog(LM_INFO, TEXT("WaitForMultipleObjects Erorr:%d, %d, %s"), 
					dwRet, dwError, Error.ErrorMessage());
				break;
			}
		}
		//free object.
		_Socket.CloseSocket();
		_Socket.FreeVDisk(VDList);

		pLogger->WriteLog(LM_INFO, TEXT("Exit Work Thread."));
		return 0;
	}
}