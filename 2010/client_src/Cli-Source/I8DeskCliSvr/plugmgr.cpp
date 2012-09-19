#include "stdafx.h"
#include "plugmgr.h"
#include <Shlwapi.h>
#include <process.h>
#include <Iphlpapi.h>
#include <shellapi.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "DbEngine.lib")
#pragma comment(lib, "Iphlpapi.lib")

#define TCP_PORT	17918	//服务端Service监听的端口
#define UDP_PORT	17901	//客户端Service绑订的UDP端口

namespace i8desk
{
	//工具函数
	inline tstring GetAppPath()
	{
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL, path, MAX_PATH);
		PathRemoveFileSpec(path);
		PathAddBackslash(path);
		return tstring(path);
	}

	CCommunication::CCommunication(ILogger* pLogger)
	{
		CoInitialize(NULL);
		WSADATA wsaData;
		WSAStartup(0x0202, &wsaData);
		m_pLogger = pLogger;

		//get server ip addr from gameclient.ini
		{
			tstring file = GetAppPath() + TEXT("gameclient.ini");
			TCHAR buf[MAX_PATH] = {0};
			GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), buf, MAX_PATH, file.c_str());
			m_SrvIP = buf;
		}
		m_sckTcp = m_sckUdp = INVALID_SOCKET;
		m_hTcpThread = m_hUdpThread = NULL;
		m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_bIsFirstPackage = false;
	}
	
	CCommunication::~CCommunication()
	{
		CloseHandle(m_hExit);
		WSACleanup();
		CoUninitialize();
	}
	
	bool CCommunication::Start()
	{
		m_sckUdp = _CreateUdpSocket();
		if (m_sckUdp == INVALID_SOCKET)
			return false;

		//create udp,tcp thread.
		m_hUdpThread = (HANDLE)_beginthreadex(NULL, 0, UdpWorkThread, this, 0, NULL);
		if (m_hUdpThread == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create UdpWorkThread Erorr:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
			Stop();
			return false;
		}
		m_hTcpThread = (HANDLE)_beginthreadex(NULL, 0, TcpWorkThread, this, 0, NULL);
		if (m_hTcpThread == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create TcpWorkThread Erorr:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
			Stop();
			return false;
		}
		return true;
	}
	void CCommunication::Stop()
	{
		SetEvent(m_hExit);
		if (m_hTcpThread != NULL)
		{
			WaitForSingleObject(m_hTcpThread, INFINITE);
			CloseHandle(m_hTcpThread);
			m_hTcpThread = NULL;
		}
		if (m_hUdpThread != NULL)
		{
			WaitForSingleObject(m_hUdpThread, INFINITE);
			CloseHandle(m_hUdpThread);
			m_hUdpThread = NULL;
		}

		_CloseUdpSocket();
		_CloseTcpSocket();
	}
	bool CCommunication::IsConnected()
	{
		CAutoLock<CLock> lock(&m_lockTcp);
		if (m_sckTcp != INVALID_SOCKET)
			return true;
		return false;
	}
	
	bool CCommunication::ExecCmd(char*pOutData, int nOutLen, char* &pInData, int& nInLen, bool bNeedAck)
	{
		pInData = NULL;  nInLen = 0;

		if (m_sckTcp == INVALID_SOCKET)
		{
			m_pLogger->WriteLog(LM_INFO, "网络层套接子句柄无效" );
			return false;
		}
			

		_packageheader* pheader = (_packageheader*)pOutData;
		WORD dwCmd = pheader->Cmd;

		//send data to server.
		int nSend = 0;
		while(nSend < nOutLen)
		{
			int len = send(m_sckTcp, pOutData+nSend, nOutLen-nSend, 0);
			if(len <= 0)
			{
				DWORD dwError = WSAGetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("Send Data Error:Cmd:%X, Error Code: %d, Error Message: %s"), 
					pheader->Cmd, dwError, Error.ErrorMessage());
				_CloseTcpSocket();
				return false;
			}
			nSend += len;
		}

		if (!bNeedAck) 
			return true;

		//recv data from data.
		int   dwSize = 64 * 1024;
		char* pData = (char*)CoTaskMemAlloc(dwSize);
		int   dwRecvLen = 0;
		int   nTryTimes = 0;

		while (1)
		{
			FD_SET fdset;
			timeval tv = {1, 0};
			FD_ZERO(&fdset);
			FD_SET(m_sckTcp, &fdset);
			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret <= 0)
			{
				if (ret == 0 && ++nTryTimes < 10)
					continue ;
				else
				{
					DWORD dwError = WSAGetLastError();
					_com_error error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("Select Fail:%d:%s"), dwError, 
						ret == 0 ? TEXT("Recv Data Package Timeout.") : error.ErrorMessage());
					CoTaskMemFree(pData);
					_CloseTcpSocket();
					return false;
				}
			}

			nTryTimes = 0;

			if (FD_ISSET(m_sckTcp, &fdset))
			{
				int nBufLen = (dwRecvLen < sizeof(_packageheader)) ? 
					(sizeof(_packageheader) - dwRecvLen) : (dwSize - dwRecvLen);
				int len = recv(m_sckTcp, pData + dwRecvLen, nBufLen, 0);
				if (len <= 0)
				{
					DWORD dwError = WSAGetLastError();
					_com_error error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("Recv Data Fail:%d:%s"), dwError, error.ErrorMessage());					
					CoTaskMemFree(pData);
					_CloseTcpSocket();
					return false;
				}
				dwRecvLen += len;
				if (dwRecvLen == sizeof(i8desk::_packageheader))
				{
					WORD dwStartFlag = *(WORD*)pData;
					if (dwStartFlag != START_FLAG)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("rec Info Package StartFlag Error."));
						CoTaskMemFree(pData);
						_CloseTcpSocket();
						return false;
					}
					if (dwCmd != ((i8desk::_packageheader*)pData)->Cmd )
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("UpPackage's Cmd:%d is different from DownloadPackage's Cmd:%d:"), dwCmd,((i8desk::_packageheader*)pData)->Cmd );					
						CoTaskMemFree(pData);
						_CloseTcpSocket();
						return false;
					}
					
					int dwTotal = ((i8desk::_packageheader*)pData)->Length;
					if (dwSize < dwTotal)
					{
						dwSize = dwTotal;
						char* pTemp = (char*)CoTaskMemAlloc(dwSize);
						memcpy(pTemp, pData, dwRecvLen);
						CoTaskMemFree(pData);
						pData = pTemp;
					}
					else
					{
						dwSize = dwTotal;
					}
				}
				if (dwRecvLen == dwSize)
					break;
				else if (dwRecvLen > dwSize)
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("Recv Data Length is Error."));
					_CloseTcpSocket();
					return false;
				}
			}
		}
		pInData = pData;
		nInLen  = dwRecvLen;
		m_bIsFirstPackage = false;
		return true;
	}
	
	void CCommunication::GetLocMachineInfo(tagLocMachineInfo& info)
	{
		ZeroMemory(&info, sizeof(info));
		DWORD dwSize = sizeof(info.szName);
		GetComputerName(info.szName, &dwSize);

		//首先取得连接上服务器的socket的本机ip
		tstring strIp;
		try
		{
			CAutoLock<CLock> lock(&m_lockTcp);
			if (m_sckTcp == INVALID_SOCKET)
				throw 1;
			sockaddr_in addr = {0};
			int nSize = sizeof(addr);
			if (SOCKET_ERROR == getsockname(m_sckTcp, (PSOCKADDR)&addr, &nSize))
			{
				DWORD dwError = WSAGetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("getsockname Error:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
				throw 1;
			}
			char* p = inet_ntoa(addr.sin_addr);
			if (p == NULL)
			{
				DWORD dwError = WSAGetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("getsockname Error:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
				throw 1;
			}
			strIp = (LPCTSTR)_bstr_t(p);
		}
		catch (...) { }

		IP_ADAPTER_INFO  Adapter[10] = {0};
		PIP_ADAPTER_INFO pAdapter = Adapter;
		{
			dwSize = sizeof(Adapter);
			if (ERROR_SUCCESS == GetAdaptersInfo(Adapter, &dwSize))
			{
				while (strIp.size() && pAdapter != NULL)
				{
					_IP_ADDR_STRING* pIpAddress = &pAdapter->IpAddressList;
					while (pIpAddress != NULL)
					{
						if (strIp == pIpAddress->IpAddress.String)
							goto do_next;
						pIpAddress = pIpAddress->Next;
					}
					pAdapter = pAdapter->Next;
				}
			}
			else 
			{
				pAdapter = NULL;
				m_pLogger->WriteLog(LM_INFO, TEXT("GetAdapterInfo Fail:%d"), GetLastError());
			}
		}
do_next:
		if (pAdapter != NULL)
		{
			lstrcpy(info.szIp, pAdapter->IpAddressList.IpAddress.String);
			lstrcpy(info.szGate, pAdapter->GatewayList.IpAddress.String);
			lstrcpy(info.szSubMask, pAdapter->IpAddressList.IpMask.String);
			sprintf_s(info.szMac, "%02X-%02X-%02X-%02X-%02X-%02X", 
				pAdapter->Address[0],
				pAdapter->Address[1],
				pAdapter->Address[2],
				pAdapter->Address[3],
				pAdapter->Address[4],
				pAdapter->Address[5]);
			IP_PER_ADAPTER_INFO perAdapter[10] = {0};
			dwSize = sizeof(perAdapter);
			if (ERROR_SUCCESS == GetPerAdapterInfo(pAdapter->Index, perAdapter, &dwSize))
			{
				lstrcpy(info.szDNS0, perAdapter->DnsServerList.IpAddress.String);
				if (perAdapter->DnsServerList.Next != NULL)
				{
					lstrcpy(info.szDNS1, perAdapter->DnsServerList.Next->IpAddress.String);
				}
			}
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("Get Local Machine info:%s:%s:%s:%s:%s:%s"),
			info.szName, info.szIp, info.szGate, info.szDNS0, info.szDNS1, info.szMac);
	}
	
	bool CCommunication::IsFirstPackage()
	{
		CAutoLock<CLock> lock(&m_lockTcp);
		return m_bIsFirstPackage;
	}

	SOCKET CCommunication::_CreateUdpSocket()
	{
		SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
		if (sck == INVALID_SOCKET)
		{
			WORD dwError = WSAGetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("_CreateUdpSocket socket Erorr:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
			return INVALID_SOCKET;
		}
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port   = htons(UDP_PORT);
		addr.sin_addr.s_addr = 0;
		if (SOCKET_ERROR == bind(sck, (PSOCKADDR)&addr, sizeof(addr)))
		{
			WORD dwError = WSAGetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("_CreateUdpSocket bind Erorr:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
			closesocket(sck);
			return INVALID_SOCKET;
		}
		int buf_size = 256*1024;
		setsockopt(sck, SOL_SOCKET, SO_RCVBUF, (char *)&buf_size, sizeof(buf_size));
		return sck;			
	}
	void  CCommunication::_CloseUdpSocket()
	{
		//CAutoLock<CLock> lock(&m_LockUdp);
		if (m_sckTcp != INVALID_SOCKET)
		{
			closesocket(m_sckUdp);
			m_sckTcp = INVALID_SOCKET;
		}			
	}
	SOCKET CCommunication::_ConnectTcpServer()
	{
		SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);
		if (sck == INVALID_SOCKET)
		{
			WORD dwError = WSAGetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("_CreateTcpSocket socket Erorr:%d:%s"), dwError, (LPCTSTR)Error.ErrorMessage());
			return INVALID_SOCKET;
		}

		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port   = htons(TCP_PORT);
		addr.sin_addr.s_addr = inet_addr((LPCSTR)_bstr_t(m_SrvIP.c_str()));
		if (SOCKET_ERROR == connect(sck, (PSOCKADDR)&addr, sizeof(addr)))
		{
			WORD dwError = WSAGetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("_CreateTcpSocket connect Erorr:%s:%d:%s"), 
				m_SrvIP.c_str(), dwError, (LPCTSTR)Error.ErrorMessage());
			closesocket(sck);
			return INVALID_SOCKET;
		}
		CAutoLock<CLock> lock(&m_lockTcp);
		m_bIsFirstPackage = true;
		return sck;
	}

	void  CCommunication::_CloseTcpSocket()
	{
		CAutoLock<CLock> lock(&m_lockTcp);
		if (m_sckTcp != INVALID_SOCKET)
		{
			closesocket(m_sckTcp);
			m_sckTcp = INVALID_SOCKET;
		}
	}

	UINT __stdcall CCommunication::TcpWorkThread(LPVOID lpVoid)
	{
		CCommunication* pThis = reinterpret_cast<CCommunication*>(lpVoid);
		HANDLE handle[2];
		handle[0] = pThis->m_hExit;
		handle[1] = CreateEvent(NULL, FALSE, TRUE, NULL);
		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(2, handle, FALSE, 2000);
			if (dwRet == WAIT_OBJECT_0)
				break;
			else if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0 + 1)
			{
				if (!pThis->IsConnected())
				{
					SOCKET  sck = pThis->_ConnectTcpServer();
					if (sck != INVALID_SOCKET)
					{
						CAutoLock<CLock> lock(&pThis->m_lockTcp);
						pThis->m_sckTcp = sck;
					}
				}
			}
		}
		pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Exit Tcp WorkThread"));
		return 0;
	}

	UINT _stdcall CCommunication::UdpWorkThread(LPVOID lparam)
	{
		CCommunication* pThis = (CCommunication*)lparam;

		int  nBufLen = 64* 1024;			//表示可以存放数据的缓冲长度
		int  nDataLen = 0;					//表示在缓冲区中实际数据的长度
		char *pData = new char[nBufLen];	//存放数据的缓冲区

		while(WaitForSingleObject(pThis->m_hExit, 0) != WAIT_OBJECT_0)
		{
			fd_set fd;
			FD_ZERO(&fd);
			FD_SET(pThis->m_sckUdp, &fd);
			timeval tv = {1, 0};
			int ret = select(0, &fd, NULL, NULL, &tv);
			if (ret <= 0)
			{
				if (ret < 0)
				{
					DWORD dwError = WSAGetLastError();
					_com_error Error(dwError);
					pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Recv Upd Thread Select Error:%d:%s"),  dwError, Error.ErrorMessage());

					//////////////////////////////////////////////////////////////////////////
					//如果出错后，再反复的出错，过能出现一直占用cpu.相当于没有阻塞
					Sleep(100);
					//////////////////////////////////////////////////////////////////////////
				}
				continue;
			}
			else
			{
				sockaddr_in addr = {0};
				int nAddrLen = sizeof(addr);
				if(FD_ISSET(pThis->m_sckUdp, &fd))
				{
					int len = recvfrom(pThis->m_sckUdp, pData+nDataLen, nBufLen - nDataLen, 0, (PSOCKADDR)&addr, &nAddrLen);
					if (len <= 0)
					{
						DWORD dwError = WSAGetLastError();
						_com_error Error(dwError);
						pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Recv Upd Thread Select Error:%d:%s"),  dwError, Error.ErrorMessage());
						nDataLen = 0;
						continue;
					}
					nDataLen += len;
					_packageheader* pheader = (_packageheader*)pData;
					if(nDataLen >= sizeof(_packageheader))
					{							
						if(pheader->Length > (DWORD)nBufLen)
						{
							nBufLen = pheader->Length + 10;
							char* pTemp = new char[nBufLen];
							memcpy(pTemp, pData, nDataLen);
							delete []pData;
							pData = pTemp;
						}
					}
					if ((DWORD)nBufLen >= pheader->Length)
					{
						pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Recv Udp Package.Cmd:0x%x,Length:0x%x"), pheader->Cmd, pheader->Length);
						//if (addr.sin_addr.s_addr == inet_addr(pThis->m_SrvIP.c_str()))
							pThis->UdpRecvNotify(pData, nDataLen, addr);
// 						else
// 							pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Not Handle Cmd"));
						nDataLen = 0;
					}
				}
			}
		}
		delete []pData;
		pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Exit Udp WorkThread."));
		return 0;
	}
	
	ILogger* CPlugMgr::CreateLogger()
	{
		ILogger* pLogger = NULL;
		i8desk::CreateLogger(&pLogger);
		return pLogger;
	}

	IPlug* CPlugMgr::FindPlug(LPCTSTR plugName)
	{
		CAutoLock<CLock> lock(&m_lock);
		NAME2PLUGITER it = m_mapName2Plugs.find(tstring(plugName));
		if(it != m_mapName2Plugs.end())
			return it->second;
		return NULL;
	}

	bool CPlugMgr::Start()
	{
		if (!CCommunication::Start())
			return false;

		//删除旧的插件目录，以后不需要了。
		{
			char path[MAX_PATH] = {0};
			SHFILEOPSTRUCT fo = {0};
			sprintf_s(path, "%sPlugin\\setlocalhost\\*", GetAppPath().c_str());
			fo.pFrom = path;
			fo.wFunc = FO_DELETE;
			fo.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
			SHFileOperation(&fo);

			sprintf_s(path, "%sPlugin\\setlocalhost\\", GetAppPath().c_str());
			RemoveDirectory(path);
		}
		CreateDirectory((GetAppPath() + TEXT("Data\\")).c_str(), NULL);

		//load plug.
		m_pLogger->WriteLog(LM_INFO, TEXT("Start Load Plug"));
		std::vector<tstring> vPlugList;
		tstring root = GetAppPath();
		vPlugList.push_back(root + BOOTRUN_MODULE_NAME);
		vPlugList.push_back(root + CLIMGR_MODULE_NAME);
		vPlugList.push_back(root + GAMEUPDATE_GAME_CLI_NAME);
		vPlugList.push_back(root + FORCEUPDATE_MODULE_NAME);        //加载推送组件
		
		char szPlugList[4096] = {0};
		DWORD dwValue = GetPrivateProfileSection(TEXT("Plugin"), szPlugList, sizeof(szPlugList), 
			(root + TEXT("Plugin\\plugin.ini")).c_str());
		if (dwValue < sizeof(szPlugList))
		{
			char* p = szPlugList;
			while (p - szPlugList < (int)dwValue)
			{
				tstring szPlug(p);
				vPlugList.push_back(root + szPlug.substr(szPlug.find(TEXT('='))+1));
				p += szPlug.size() + 1;
			}
		}
		for (std::vector<tstring>::iterator it=vPlugList.begin(); it!=vPlugList.end(); it++)
		{
			HMODULE hMod = NULL;
			IPlug*  pPlug = NULL;
			try
			{
				hMod = LoadLibrary(it->c_str());
				PFNCREATEPLUG pfnCreatePlug = hMod ? (PFNCREATEPLUG)GetProcAddress(hMod, "CreatePlug") : NULL;
				if (hMod != NULL && pfnCreatePlug != NULL && (pPlug = pfnCreatePlug()) != NULL)
				{
					bool ret = pPlug->Initialize(this);
					m_pLogger->WriteLog(LM_INFO, TEXT("LoadPlug:%s:0x%x:%s:%s:ret:%d"),
						pPlug->GetPlugName(), 
						pPlug->GetVersion(),
						pPlug->GetCompany(),
						pPlug->GetDescription(),
						ret ? 1 : 0);
					m_mapName2Plugs.insert(std::make_pair(pPlug->GetPlugName(), pPlug));
				}
				else
				{
					DWORD dwError = GetLastError();
					_com_error Error(dwError);
					m_pLogger->WriteLog(LM_INFO, TEXT("LoadPlug Fail:%s:%d:%s"), 
						it->c_str(), dwError, Error.ErrorMessage());
				}
			}
			catch (...)	{ m_pLogger->WriteLog(LM_INFO, TEXT("LoadPlug Unknown Error:%s"), it->c_str()); }
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("Load Plug Finish"));
		return true;
	}
	
	void CPlugMgr::Stop()
	{
		CCommunication::Stop();
		m_pLogger->WriteLog(LM_INFO, TEXT("Stop Tcp Udp Service."));

		NAME2PLUGITER it = m_mapName2Plugs.begin();
		for(;it != m_mapName2Plugs.end();it++)
		{
			it->second->UnInitialize();
			m_pLogger->WriteLog(LM_INFO, TEXT("UnInitialize Plug %s"), it->first.c_str());
		}

		NAME2PLUGITER iter =m_mapName2Plugs.begin();
		for(;iter != m_mapName2Plugs.end();iter++)
		{
			iter->second->Release();
		}
		m_mapName2Plugs.clear();
	}
	
	void CPlugMgr::UdpRecvNotify(char* pData, int length, sockaddr_in& from)
	{
		NAME2PLUGITER it= m_mapName2Plugs.begin();
		for(;it != m_mapName2Plugs.end();it++)
		{
			if(it->second->NeedUdpNotity())
			{
				it->second->UdpRecvNotify(pData,length, from);
			}
		}
	}
}