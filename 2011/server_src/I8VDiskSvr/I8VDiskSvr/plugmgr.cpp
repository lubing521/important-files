#include "stdafx.h"
#include "plugmgr.h"

#include <fstream>

#include "Serialize.h"
#include "../../../include/Utility/utility.h"

#ifdef _DEBUG 
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif


const TCHAR *VDiskFile = _T("Data/VDisk.dat");
const stdex::tString ConfigFile = _T("Data\\I8SyncSvr.ini");


namespace i8desk
{
	

	CPlugMgr::CPlugMgr(ILogger* pLogger) 
		: m_pLogger(pLogger)
		, m_bInnerUpdate(0)
		, m_hThread(NULL)
		, m_hRcvThread(NULL)
		, m_hExited(NULL)
	{
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CPlugMgr::~CPlugMgr()	
	{  }


	ISvrPlug* CPlugMgr::FindPlug(LPCTSTR plugName)
	{
		AutoLock lock(m_lock);
		std::map<stdex::tString, ISvrPlug*>::iterator it = m_mapPlugs.find(stdex::tString(plugName));			
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
			{
				stdex::tString szIniFile = utility::GetAppPath() + ConfigFile;
				TCHAR szIp[MAX_PATH] = {0};
				GetPrivateProfileString(_T("System"), _T("SvrIp"), _T(""), szIp, MAX_PATH, szIniFile.c_str());
				if (lstrlen(szIp) != 0 && !IsLocalMachine(szIp))
					bLoadUpdate = true;
			}

			if (NULL == LoadPlug(utility::GetAppPath() + PLUG_VDISK_MODULE, this, 0))
			{
				m_pLogger->WriteLog(LM_INFO, _T("Load %s Plug Error."), PLUG_VDISK_NAME);
				return false;
			}

			if (bLoadUpdate)
			{
				if (NULL == LoadPlug(GetAppPath() + PLUG_GAMEUPDATE_MODULE, NULL, (DWORD)PLUG_UPTSVR_NAME))
				{
					m_pLogger->WriteLog(LM_INFO, _T("Load %s Plug Error."), PLUG_UPTSVR_NAME);	
					return false;
				}
				m_bInnerUpdate = 1;
			}

			if (NULL == LoadPlug(utility::GetAppPath() + PLUG_GAMEUPDATE_MODULE, NULL, (DWORD)PLUG_SYNCDISK_NAME))
			{
				m_pLogger->WriteLog(LM_INFO, _T("Load %s Plug Error."), PLUG_SYNCDISK_NAME);
				return false;
			}
			
		}

		sock_.reset(new CSocket(FindPlug(PLUG_VDISK_NAME), m_pLogger, m_hExited));
		sock_->CreateSocket();
		if( sock_->IsConnect() )
		{
			update_.reset(new AutoUpdateFile(m_pLogger, *(sock_.get())));
			update_->Start();
		}
		else
		{
			m_pLogger->WriteLog(LM_INFO, _T("没有连接上主服务，不能自动更新"));
		}


		//创建工作线程
		{
			
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
			m_hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, NULL);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create WorkThread:%s"), (m_hThread != NULL && m_hRcvThread != NULL) ? TEXT("Success.") : TEXT("Fail"));		
			m_pLogger->WriteLog(LM_INFO, TEXT("Service already started."));
			if (m_hThread == NULL || m_hRcvThread == NULL )
				return false;
		}
		return true;
	}

	bool CPlugMgr::Stop()
	{
		if( update_.get() != 0 )
		{
			update_->Stop();
		}

		//停止线程.
		if (m_hExited != NULL)
		{
			if (m_hThread != NULL)
			{
				SetEvent(m_hExited);
				WaitForSingleObject(m_hThread, INFINITE);
				WaitForSingleObject(m_hRcvThread, INFINITE);
				CloseHandle(m_hThread);
				CloseHandle(m_hRcvThread);
			}
			CloseHandle(m_hExited);
		}
		m_hThread = NULL;
		m_hRcvThread = NULL;
		m_hExited = NULL;

		if( sock_.get() != 0 )
		{
			sock_->CloseSocket();
		}

		//停止所有插件启动的网络层
		for (std::map<stdex::tString, ISvrPlug*>::iterator it = m_mapPlugs.begin(); it != m_mapPlugs.end(); it++)
		{
			it->second->UnInitialize();
		}

		for (std::map<stdex::tString, ISvrPlug*>::iterator it = m_mapPlugs.begin(); it != m_mapPlugs.end(); it++)
		{
			it->second->Release();
		}
		m_mapPlugs.clear();
		return true;
	}

	BOOL CPlugMgr::IsLocalMachine(LPCTSTR lpszIP)
	{
		std::string szIP = CT2A(lpszIP);
		DWORD ip = inet_addr(szIP.c_str());
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

	ISvrPlug* CPlugMgr::LoadPlug(stdex::tString& szPlugDLL, ISvrPlugMgr* pPlugMgr, DWORD param)
	{
		HMODULE hMod = NULL;
		ISvrPlug* pPlug = NULL;
		try
		{
			hMod = LoadLibrary(szPlugDLL.c_str());
			PFNCREATESVRPLUG pfnCreatePlug = (hMod == NULL) ? NULL : (PFNCREATESVRPLUG)GetProcAddress(hMod, "CreateSvrPlug");
			if (hMod != NULL && pfnCreatePlug != NULL && (pPlug = pfnCreatePlug(param)) != NULL)
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
					m_mapPlugs.insert(std::make_pair(stdex::tString(pPlug->GetPlugName()), pPlug));
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

	UINT __stdcall CPlugMgr::RecvThread(LPVOID lpVoid)
	{
		CPlugMgr* pThis = reinterpret_cast<CPlugMgr*>(lpVoid);
		i8desk::ILogger* pLogger = pThis->m_pLogger;
		CSocket &_Socket = *(pThis->sock_);

		async::thread::AutoEvent event;
		event.Create();
		HANDLE handles[] = { event, pThis->m_hExited};
		while(1)
		{
			DWORD ret = ::WaitForMultipleObjects(_countof(handles), handles, FALSE, 2000);
			if( ret == WAIT_TIMEOUT && _Socket.IsConnect() )
			{
				Lock lock(pThis->mutex_);
				_Socket.ReportVDiskClientStatus(pThis->VDList_);
			}
			else if( ret == WAIT_OBJECT_0 + 1 )
				break;
		}

		return 0;
	}

	UINT __stdcall CPlugMgr::WorkThread(LPVOID lpVoid)
	{
		CPlugMgr* pThis = reinterpret_cast<CPlugMgr*>(lpVoid);
		i8desk::ILogger* pLogger = pThis->m_pLogger;
		CSocket &_Socket = *(pThis->sock_);

		utility::CEvenyOneSD sd;
		HANDLE  hEvent[3];
		hEvent[0] = pThis->m_hExited;
		hEvent[1] = CreateEvent(NULL, FALSE, TRUE, NULL);
		hEvent[2] = CreateEvent(sd.GetSA(), FALSE, FALSE, VDISK_EVENT_NAME);
		if (hEvent[2] == NULL)
			pLogger->WriteLog(LM_INFO, _T("Create Event Fail:%d"), GetLastError());

		std::map<DWORD, tagVDInfoEx*> &VDList = pThis->VDList_;

		// 虚拟盘离线数据文件
		static stdex::tString strPath = pThis->GetAppPath() + VDiskFile;
		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(3, hEvent, FALSE, HEART_INTERVAL);
			if (dwRet == WAIT_OBJECT_0)
				break;

			//刷新虚拟盘
			else if (dwRet == WAIT_OBJECT_0 + 2)
			{
				pLogger->WriteLog(LM_INFO, _T("Refresh Virtual Disk."));
				std::map<DWORD, tagVDInfoEx*>::iterator it = VDList.begin();
				for (; it != VDList.end(); it++)
				{
					if (it->second->handle != NULL)
					{
						ISvrPlug* pPlug = pThis->FindPlug(PLUG_VDISK_NAME);
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

						Lock lock(pThis->mutex_);
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
					Lock lock(pThis->mutex_);
					_Socket.ReportVDiskStatus(VDList);
					_Socket.ReportServerStatus(VDList, pThis->m_bInnerUpdate, pThis->FindPlug(PLUG_SYNCDISK_NAME));
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