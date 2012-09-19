// NetLayer.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <process.h>
#include <shlwapi.h>
#include <WinSock2.h>
#include <mswsock.h>
#include <vector>
#include <map>

#include "../../include/MultiThread/Lock.hpp"

#define HEART_INTERVAL	(35*1000)

namespace i8desk
{
	class CMemPool : public IMemPool
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;
	private:
		DWORD m_dwDefPoolSize;
		DWORD m_dwDefBlockSize;
		DWORD m_dwDefIncrease;
		Mutex m_Lock;
		std::map<LPSTR, DWORD> m_mapUsed;
		std::map<LPSTR, DWORD> m_mapFree;

		std::map<LPSTR, DWORD> m_mapBigUsed;
		std::map<LPSTR, DWORD> m_mapBigFree;
	public:
		CMemPool():m_dwDefPoolSize(100), m_dwDefBlockSize(0x10000), m_dwDefIncrease(10) {	}
		virtual ~CMemPool()		{ FreeAll(); }
		virtual void  Release() { delete this; }

		virtual DWORD GetDefPoolSize()  { return m_dwDefPoolSize; }
		virtual DWORD GetDefBlockSize() { return m_dwDefBlockSize;}
		virtual DWORD GetDefIncrease()  { return m_dwDefIncrease; }

		virtual void  SetDefault(DWORD dwdefPoolSize, DWORD dwdefBlockSize, DWORD dwdefIncrease) 
		{
			m_dwDefPoolSize = dwdefPoolSize;
			m_dwDefBlockSize = dwdefBlockSize;
			m_dwDefIncrease  = dwdefIncrease;
			AddBlock(dwdefPoolSize, dwdefBlockSize);
		}

		void AddBlock(DWORD dwSize, DWORD dwBlockSize)
		{
			for (DWORD idx=0; idx<dwSize; idx++)
			{
				char* p = NULL;
				try
				{
					p = new char[dwBlockSize];
				}
				catch (...) { p = NULL; }
				if (p != NULL)
				{
					m_mapFree.insert(std::make_pair(p, dwBlockSize));
				}
				else
				{
					throw std::bad_alloc("memory alloc fail.");
					OutputDebugString(TEXT("netlayer.alloc memory fail.\r\n"));
				}
			}
		}

		virtual char* Alloc(DWORD dwSize)
		{
			AutoLock lock(m_Lock);
			if (dwSize <= m_dwDefBlockSize) //|| dwSize == 0 
			{
				if (m_mapFree.size() == 0)
				{
					AddBlock(m_dwDefIncrease, m_dwDefBlockSize);
				}
				if (m_mapFree.size() == 0)
				{
					OutputDebugString(TEXT("netlayer.alloc memory fail 2.\r\n"));
					return NULL;
				}
				std::map<LPSTR, DWORD>::iterator it = m_mapFree.begin();
				std::pair<LPSTR, DWORD> p = *it;
				m_mapFree.erase(it);
				m_mapUsed.insert(p);
				return p.first;
			}
			else
			{
				std::map<LPSTR, DWORD>::iterator it = m_mapBigFree.begin();
				for (; it != m_mapBigFree.end(); it++)
				{
					if (it->second >= dwSize)
					{
						std::pair<LPSTR, DWORD> p = *it;
						m_mapBigUsed.insert(*it);
						m_mapBigFree.erase(it);
						return p.first;
					}
				}

				DWORD dwBlockSize = ((dwSize/m_dwDefBlockSize)+1) * m_dwDefBlockSize;
				char* p = new char[dwBlockSize];
				m_mapBigUsed.insert(std::make_pair(p, dwBlockSize));

				return p;
			}
		}

		virtual void Free(void* pBlock)
		{
			AutoLock lock(m_Lock);

			std::map<LPSTR, DWORD>::iterator it = m_mapUsed.find((LPSTR)pBlock);
			if (it != m_mapUsed.end())
			{
				std::pair<LPSTR, DWORD> p = *it;
				m_mapUsed.erase(it);
				m_mapFree.insert(p);
				return ;
			}
			it = m_mapBigUsed.find((LPSTR)pBlock);
			if (it != m_mapBigUsed.end())
			{
				std::pair<LPSTR, DWORD> p = *it;
				m_mapBigUsed.erase(it);
				m_mapBigFree.insert(p);
			}
		}

		virtual void FreeAll()
		{
			AutoLock lock(m_Lock);
			std::map<LPSTR, DWORD>::iterator it = m_mapFree.begin();
			for (; it != m_mapFree.end(); it++)
			{
				delete []it->first;
			}
			m_mapFree.clear();
			for (it=m_mapUsed.begin(); it != m_mapUsed.end(); it++)
			{
				delete []it->first;
			}
			m_mapUsed.clear();

			for (it=m_mapBigFree.begin(); it != m_mapBigFree.end(); it++)
			{
				delete []it->first;
			}
			m_mapBigFree.clear();

			for (it=m_mapBigUsed.begin(); it != m_mapBigUsed.end(); it++)
			{
				delete []it->first;
			}
			m_mapBigUsed.clear();
		}

		virtual DWORD GetBufSize(void* pBlock)
		{
			AutoLock lock(m_Lock);
			std::map<LPSTR, DWORD>::iterator it = m_mapFree.find((LPSTR)pBlock);
			if (it != m_mapFree.end())
				return it->second;

			it = m_mapUsed.find((LPSTR)pBlock);
			if (it != m_mapUsed.end())
				return it->second;

			it = m_mapBigFree.find((LPSTR)pBlock);
			if (it != m_mapBigFree.end())
				return it->second;

			it = m_mapBigUsed.find((LPSTR)pBlock);
			if (it != m_mapBigUsed.end())
				return it->second;

			return 0;
		}

		virtual BOOL  IsAllocFrom(void* pBlock)
		{
			AutoLock lock(m_Lock);
			if (m_mapFree.find((LPSTR)pBlock) != m_mapFree.end())
				return TRUE;

			if (m_mapUsed.find((LPSTR)pBlock) != m_mapUsed.end())
				return TRUE;

			if (m_mapBigFree.find((LPSTR)pBlock) != m_mapBigFree.end())
				return TRUE;

			if (m_mapBigUsed.find((LPSTR)pBlock) != m_mapBigUsed.end())
				return TRUE;
			return FALSE;
		}
	};

	class CNetLayer : public INetLayer
	{
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

	public:
		enum EVENT_ENUM
		{
			EVENT_ACCEPT	= 0,	//连接事件
			EVENT_SEND		= 1,	//发送事件
			EVENT_RECV		= 2		//接收事件
		};
		struct PER_HANLDE_CONTENT
		{
			SOCKET socket;
			bool   firstpackage;		//是否是客户端发来的第一包数据.
			DWORD  dwLastRecvDataTime;	//上次接收数据的时间
		};

		struct PER_IO_CONTEXT : public WSAOVERLAPPED
		{
			EVENT_ENUM dwEvent;		//事件类型
			SOCKET socket;			//收发数据的socket.
			DWORD  dwLength;		//己接收，或者还需要发送的字节数
			char*  pPackage;		//指向数据包的头指针，它不会改变.该缓冲是通过IMemPool分配和释放的
			WSABUF Buffer;			//用于WSASend/WSARecv操作的缓冲.
		};
	public:
		CNetLayer();
		virtual ~CNetLayer();
		virtual void Release() { delete this; }

		virtual void SetINetLayerEvent(INetLayerEvent* pINetLayerEvent) { m_pNetLayerEvent = pINetLayerEvent; }
		virtual INetLayerEvent* GetINetLayerEvent() { return m_pNetLayerEvent; }

		virtual void SetIMemPool(IMemPool* pIMemPool) { m_pMemPool = pIMemPool; }
		virtual IMemPool* GetIMemPool() { return m_pMemPool; }

		virtual void SetIPackageParse(IPackageParse* pPackageParse) { m_pPackageParse = pPackageParse; }
		virtual IPackageParse* GetIPackageParse() { return m_pPackageParse; }
	public:
		virtual DWORD Start(WORD wPort);
		virtual void  Stop();
		virtual WORD  GetPort() { return m_wPort; }

		virtual DWORD ParasePackage(char* package, DWORD length);
		virtual void  SendPackage(SOCKET sck, LPSTR pPackage, DWORD length);
		virtual void  DisConnect(SOCKET sck);
	private:
		DWORD StartListenSocket();
		DWORD PostAccepRequest();
		DWORD PostRecvRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData = NULL);
		DWORD PostSendRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);

		void  OnAccept(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnRecv (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnSend (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnClose(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData);
		static UINT __stdcall AcceptThread(PVOID lpVoid);
		static UINT __stdcall WorkThread(PVOID lpVoid);
	private:
		IMemPool* m_pMemPool;
		INetLayerEvent* m_pNetLayerEvent;
		IPackageParse*  m_pPackageParse;
		HANDLE m_hIocp;
		SOCKET m_ListenSock;
		WORD   m_wPort;
		char   m_szIp[32];
		LPFN_ACCEPTEX m_lpfnAcceptEx;
		HANDLE m_hExited;
		std::vector<HANDLE>	m_vecThreadHandle;

		CMemPool m_poolIoCtx;
		CMemPool m_poolHandleCtx;

		Mutex m_OptexClient;	
		std::map<SOCKET, PER_HANLDE_CONTENT*> m_mapClient;
	};

	BOOL __stdcall CreateIMemPool(IMemPool** pIMemPool)
	{
		if (pIMemPool == NULL)
			return FALSE;
		*pIMemPool = NULL;
		try
		{
			*pIMemPool = new CMemPool();
		}
		catch (...) { *pIMemPool = NULL; }
		if (*pIMemPool == NULL)
			return FALSE;

		return TRUE;
	}

	BOOL __stdcall CreateINetLayer(INetLayer** pINetLayer)
	{
		if (pINetLayer == NULL)
			return FALSE;
		*pINetLayer = NULL;
		try
		{
			*pINetLayer = new CNetLayer();
		}
		catch (...) { *pINetLayer = NULL; }
		if (*pINetLayer == NULL)
			return FALSE;

		return TRUE;
	}

	CNetLayer::CNetLayer() : 
		m_pMemPool(NULL),
		m_pNetLayerEvent(NULL),
		m_pPackageParse(NULL),
		m_hIocp(NULL),
		m_ListenSock(INVALID_SOCKET),
		m_wPort(7918),
		m_lpfnAcceptEx(NULL),
		m_hExited(NULL)
	{
		WSAData wsaData = {0};
		WSAStartup(MAKEWORD(2,2), &wsaData);
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_poolIoCtx.SetDefault(100, sizeof(PER_IO_CONTEXT), 10);
		m_poolHandleCtx.SetDefault(100, sizeof(PER_HANLDE_CONTENT), 10);
	}

	CNetLayer::~CNetLayer()
	{
		Stop();
		CloseHandle(m_hExited);
		WSACleanup();
	}

	DWORD CNetLayer::Start(WORD wPort)
	{
		m_wPort = wPort;
		m_hIocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (m_hIocp == NULL)
			return GetLastError();

		DWORD dwError = StartListenSocket();
		if (dwError != ERROR_SUCCESS)
		{
			CloseHandle(m_hIocp);
			m_hIocp = NULL;
			return dwError;
		}

		HANDLE hThread = (HANDLE)_beginthreadex(
			NULL, 
			0,
			AcceptThread,
			this,
			0,
			NULL
			);
		m_vecThreadHandle.push_back(hThread);

		SYSTEM_INFO info = {0};
		GetSystemInfo(&info);
		for (DWORD idx=0; idx<info.dwNumberOfProcessors*2; idx++)
		{
			hThread = (HANDLE)_beginthreadex(
				NULL, 
				0,
				WorkThread,
				this,
				0,
				NULL
				);
			m_vecThreadHandle.push_back(hThread);
		}

		return ERROR_SUCCESS;
	}

	void CNetLayer::Stop()
	{
		try
		{
			if (m_ListenSock != INVALID_SOCKET)
			{
				closesocket(m_ListenSock);
				m_ListenSock = INVALID_SOCKET;
			}
			if (m_vecThreadHandle.size())
			{
				SetEvent(m_hExited);
				for (size_t idx=0; idx<m_vecThreadHandle.size(); idx++)
				{
					PostQueuedCompletionStatus(m_hIocp, 0, 0, (LPOVERLAPPED)-1);
				}
				WaitForMultipleObjects((DWORD)m_vecThreadHandle.size(), &m_vecThreadHandle[0], TRUE, INFINITE);
				m_vecThreadHandle.clear();
			}

			if (m_hIocp != NULL)
			{
				CloseHandle(m_hIocp);
				m_hIocp = NULL;
			}

			AutoLock lock(m_OptexClient);
			std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = m_mapClient.begin();
			for (; it != m_mapClient.end(); it ++)
			{
				closesocket(it->first);
				m_poolHandleCtx.Free(it->second);
			}
			m_mapClient.clear();
			m_poolHandleCtx.FreeAll();
			m_poolIoCtx.FreeAll();
		}
		catch (...) { } 
	}

	DWORD CNetLayer::ParasePackage(char* package, DWORD length)
	{
		/*if (length >= sizeof(_packageheader) && *(WORD*)package != START_FLAG)
		{
			char buf[MAX_PATH] = {0};
			_packageheader* header = (_packageheader*)package;
			sprintf(buf, "netlayer:package invalid:start:%d, cmd:%d, ver:%d, length:%d\r\n",
			header->StartFlag, header->Cmd, header->Version, header->Length);
			OutputDebugString(buf);
			return 0;
		}*/
		if (m_pPackageParse != NULL)
			return m_pPackageParse->ParasePackage(package, length);
		
		if (length >= sizeof(pkgheader))
		{
			pkgheader* header = (pkgheader*)package;
			if (header->StartFlag != START_FLAG)
				return 0;
			return header->Length;// + sizeof(_packageheader);
		}
		return m_pMemPool->GetBufSize(package);
	}

	void CNetLayer::SendPackage(SOCKET sck, LPSTR pPackage, DWORD length)
	{
		PER_HANLDE_CONTENT* pHandle = NULL;
		{
			AutoLock lock(m_OptexClient);
			std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = m_mapClient.find(sck);
			if (it != m_mapClient.end())
				pHandle = it->second;
		}
		
		if (pHandle != NULL)
		{
			PER_IO_CONTEXT* pIoData = (PER_IO_CONTEXT*)m_poolIoCtx.Alloc(0);
			pIoData->pPackage = pPackage;
			pIoData->Buffer.buf = pPackage;
			pIoData->Buffer.len = length;
			pIoData->dwLength = length;
			pIoData->socket = sck;
			pIoData->dwEvent = EVENT_SEND;
			
			ZeroMemory((LPOVERLAPPED)pIoData, sizeof(OVERLAPPED));
			PostSendRequest(pHandle, pIoData, 0);
		}
		else
		{
			m_pMemPool->Free(pPackage);
		}
	}

	DWORD CNetLayer::StartListenSocket()
	{
		DWORD dwError = ERROR_SUCCESS;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		DWORD dwBytes = 0;
		sockaddr_in addr = {0};

		m_ListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_ListenSock == INVALID_SOCKET)
			return WSAGetLastError();

		if (NULL == CreateIoCompletionPort((HANDLE)m_ListenSock, m_hIocp, 0, 0))
			goto _do_err;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_wPort);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (SOCKET_ERROR == bind(m_ListenSock, (PSOCKADDR)&addr, sizeof(addr)))
			goto _do_err;

		if (SOCKET_ERROR == listen(m_ListenSock, SOMAXCONN))
			goto _do_err;		

		if (SOCKET_ERROR == WSAIoctl(m_ListenSock, SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidAcceptEx, sizeof(GuidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx), 
			&dwBytes, NULL, NULL))
			goto _do_err;

		PostAccepRequest();

		return ERROR_SUCCESS;
	_do_err:
		dwError = WSAGetLastError();
		if (m_ListenSock != INVALID_SOCKET)
			closesocket(m_ListenSock);
		m_ListenSock = INVALID_SOCKET;
		return dwError;
	}

	//not call this function.
	void CNetLayer::DisConnect(SOCKET sck)
	{
		try
		{
			shutdown(sck, SD_BOTH);
		}
		catch (...)	{ }
	}

	DWORD CNetLayer::PostAccepRequest()
	{
		for(int idx=0;idx<10;idx++)
		{
			SOCKET AcceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if(INVALID_SOCKET == AcceptSocket)
				return WSAGetLastError();

			//int nZero = 0;
			//setsockopt(AcceptSocket, SOL_SOCKET, SO_SNDBUF, (LPCSTR)&nZero, sizeof(nZero));
			//setsockopt(AcceptSocket, SOL_SOCKET, SO_RCVBUF, (LPCSTR)&nZero, sizeof(nZero));

			DWORD dwByte;
			PER_IO_CONTEXT* pHandleCtx = (PER_IO_CONTEXT*)m_poolIoCtx.Alloc(0);
			pHandleCtx->pPackage = (char*)m_pMemPool->Alloc(0);
			pHandleCtx->Buffer.buf = pHandleCtx->pPackage;
			pHandleCtx->dwLength = 0;
			pHandleCtx->Buffer.len = m_pMemPool->GetBufSize(pHandleCtx->pPackage);
			pHandleCtx->dwEvent = EVENT_ACCEPT;
			pHandleCtx->socket = AcceptSocket;
			ZeroMemory((LPOVERLAPPED)pHandleCtx, sizeof(OVERLAPPED));

			if(!m_lpfnAcceptEx(m_ListenSock, AcceptSocket, pHandleCtx->Buffer.buf, 
				pHandleCtx->Buffer.len -((sizeof(SOCKADDR_IN)+16)*2),
				sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwByte, (LPOVERLAPPED)pHandleCtx))
			{
				DWORD dwError = WSAGetLastError();
				if(ERROR_IO_PENDING != dwError)
				{
					::closesocket(AcceptSocket);
					return dwError;
				}
			}
		}
		return ERROR_SUCCESS;
	}

	DWORD CNetLayer::PostRecvRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData /* = NULL */)
	{
		if (pIoData == NULL)
		{
			pIoData = (PER_IO_CONTEXT*)m_poolIoCtx.Alloc(0);
			pIoData->pPackage = (char*)m_pMemPool->Alloc(0);
			pIoData->dwLength = 0;
			pIoData->Buffer.buf = pIoData->pPackage;
			pIoData->Buffer.len = m_pMemPool->GetBufSize(pIoData->pPackage);
		}

		pIoData->socket = pHandle->socket;
		pIoData->dwEvent = EVENT_RECV;
		ZeroMemory((LPOVERLAPPED)pIoData, sizeof(OVERLAPPED));

		DWORD dwLength = 0;
		DWORD dwFlag = 0;
		int ret = WSARecv(
			pHandle->socket,
			&pIoData->Buffer,
			1,
			&dwLength, 
			&dwFlag, 
			(LPOVERLAPPED)pIoData, 
			NULL
			);

		if (ret == SOCKET_ERROR && ERROR_IO_PENDING != WSAGetLastError())
		{
			OnClose(pHandle, pIoData);
		}

		return ERROR_SUCCESS;
	}

	DWORD CNetLayer::PostSendRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		pIoData->socket = pHandle->socket;
		pIoData->dwEvent = EVENT_SEND;
		pIoData->Buffer.buf += dwLength;
		pIoData->dwLength -= dwLength;
		//pIoData->Buffer.len -= dwLength;
		pIoData->Buffer.len = pIoData->dwLength >= 0x10000 ? 0x10000 : pIoData->dwLength;
		ZeroMemory((LPOVERLAPPED)pIoData, sizeof(OVERLAPPED));

		DWORD dwSendbytes = 0;
		int ret = WSASend(
			pHandle->socket,
			&pIoData->Buffer, 
			1, 
			&dwSendbytes, 
			0, 
			(LPOVERLAPPED)pIoData, 
			NULL
			);
		if (ret == SOCKET_ERROR && ERROR_IO_PENDING != WSAGetLastError())
		{
			OnClose(pHandle, pIoData);
		}

		return ERROR_SUCCESS;
	}

	void CNetLayer::OnAccept(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		{
			AutoLock lock(m_OptexClient);
			pHandle->dwLastRecvDataTime = GetTickCount();
			m_mapClient.insert(std::make_pair(pHandle->socket, pHandle));
			{
	#define  SIO_RCVALL  IOC_IN | IOC_VENDOR | 1
	#define  SIO_RCVALL_MCAST  IOC_IN | IOC_VENDOR | 2
	#define  SIO_RCVALL_IGMPMCAST  IOC_IN | IOC_VENDOR | 3
	#define  SIO_KEEPALIVE_VALS  IOC_IN | IOC_VENDOR | 4
	#define  SIO_ABSORB_RTRALERT  IOC_IN | IOC_VENDOR | 5
	#define  SIO_UCAST_IF  IOC_IN | IOC_VENDOR | 6
	#define  SIO_LIMIT_BROADCASTS  IOC_IN | IOC_VENDOR | 7
	#define  SIO_INDEX_BIND  IOC_IN | IOC_VENDOR | 8
	#define  SIO_INDEX_MCASTIF  IOC_IN | IOC_VENDOR | 9
	#define  SIO_INDEX_ADD_MCAST  IOC_IN | IOC_VENDOR | 10
	#define  SIO_INDEX_DEL_MCAST  IOC_IN | IOC_VENDOR | 11

				struct TCP_KEEPALIVE
				{
					u_long onoff;
					u_long keepalivetime;
					u_long keepaliveinterval;					
				};
				TCP_KEEPALIVE inKeepAlive = {0};
				DWORD dwInLen = sizeof(TCP_KEEPALIVE);
				TCP_KEEPALIVE outKeepAlive = {0};
				DWORD dwOutLen = sizeof(TCP_KEEPALIVE);
				DWORD dwBytesReturn = 0;
				inKeepAlive.onoff = 1;
				inKeepAlive.keepalivetime = 30000;
				inKeepAlive.keepaliveinterval = 2000;				
				WSAIoctl(pHandle->socket, SIO_KEEPALIVE_VALS, (LPVOID)&inKeepAlive, dwInLen, 
					(LPVOID)&outKeepAlive, dwOutLen, &dwBytesReturn, NULL, NULL);
			}
		}
		if (m_pNetLayerEvent != NULL)
			m_pNetLayerEvent->OnAccept(this, pHandle->socket, 0);
		CreateIoCompletionPort((HANDLE)pHandle->socket, m_hIocp, (DWORD)pHandle, 0);
		if (dwLength)
			OnRecv(pHandle, pIoData, dwLength);
	}

	void CNetLayer::OnRecv (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		pHandle->dwLastRecvDataTime = GetTickCount();
		pIoData->dwLength += dwLength;
		DWORD dwTotal = ParasePackage(pIoData->pPackage, pIoData->dwLength);
		if (0 == dwTotal)
		{
			OnClose(pHandle, pIoData);
			return ;
		}
		DWORD dwBuffSize = m_pMemPool->GetBufSize(pIoData->pPackage);

		if (dwTotal > dwBuffSize)
		{
			char* pPackage = (char*)m_pMemPool->Alloc(dwTotal);
			memcpy(pPackage, pIoData->pPackage, pIoData->dwLength);
			pIoData->Buffer.buf = (pPackage + pIoData->dwLength);
			pIoData->Buffer.len = m_pMemPool->GetBufSize(pPackage) - pIoData->dwLength;
			m_pMemPool->Free(pIoData->pPackage);
			pIoData->pPackage = pPackage;
			PostRecvRequest(pHandle, pIoData);
		}
		else
		{
			if (dwTotal == pIoData->dwLength)
			{
				if (m_pNetLayerEvent != NULL)
				{
					m_pNetLayerEvent->OnRecv(this, pHandle->socket, pIoData->pPackage, pHandle->firstpackage ? 1 : 0);
					pHandle->firstpackage = false;
				}
				m_pMemPool->Free(pIoData->pPackage);
				m_poolIoCtx.Free(pIoData);	
				PostRecvRequest(pHandle, NULL);
			}
			else if (dwTotal > pIoData->dwLength)
			{
				pIoData->Buffer.buf += dwLength;
				pIoData->Buffer.len -= dwLength;
				PostRecvRequest(pHandle, pIoData);
			}
			else if (dwTotal < pIoData->dwLength)
			{
				DWORD dwDataLen = pIoData->dwLength;
				SOCKET sck = pHandle->socket;
				while (dwTotal <= dwDataLen && dwDataLen > 0)
				{
					char* pNewPackage = (char*)m_pMemPool->Alloc(dwTotal);
					memcpy(pNewPackage, pIoData->pPackage, dwTotal);

					if (m_pNetLayerEvent != NULL)
					{
						m_pNetLayerEvent->OnRecv(this, sck, pNewPackage, pHandle->firstpackage ? 1 : 0);
						pHandle->firstpackage = false;
					}		
					m_pMemPool->Free(pNewPackage);
					dwDataLen -= dwTotal;
					if (dwDataLen)
					{	
						pNewPackage = (char*)m_pMemPool->Alloc(dwDataLen);
						memcpy(pNewPackage, pIoData->pPackage+ dwTotal, dwDataLen);

						m_pMemPool->Free(pIoData->pPackage);
						pIoData->pPackage = pNewPackage;

						dwTotal = ParasePackage(pIoData->pPackage, dwDataLen);
						if (0 == dwTotal)
						{
							OnClose(pHandle, pIoData);
							return ;
						}
					}
					else
					{
						m_pMemPool->Free(pIoData->pPackage);
						m_poolIoCtx.Free(pIoData);
						PostRecvRequest(pHandle, NULL);
					}
				}
				if (dwDataLen)
				{
					pIoData->dwLength = dwDataLen;
					pIoData->Buffer.buf = pIoData->pPackage + dwDataLen;
					pIoData->Buffer.len = m_pMemPool->GetBufSize(pIoData->pPackage) - dwDataLen;					
					PostRecvRequest(pHandle, pIoData);
				}
			}
		}
	}

	void CNetLayer::OnSend (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		if (dwLength >= pIoData->dwLength)
		{
			if (m_pNetLayerEvent != NULL)
				m_pNetLayerEvent->OnSend(this, pHandle->socket, pIoData->pPackage, 0);
			m_pMemPool->Free(pIoData->pPackage);
			m_poolIoCtx.Free(pIoData);
		}
		else
		{
			PostSendRequest(pHandle, pIoData, dwLength);
		}
	}

	void CNetLayer::OnClose(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData)
	{
		bool flag = false;
		SOCKET sck = INVALID_SOCKET;
		if (pHandle != NULL)
		{
			sck = pHandle->socket;
			AutoLock lock(m_OptexClient);

			std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = m_mapClient.find(sck);
			if (it != m_mapClient.end())
			{
				if (m_pNetLayerEvent != NULL)
				{
					m_pNetLayerEvent->OnClose(this, sck, 0);
				}

				shutdown(sck, SD_BOTH);
				closesocket(sck);
				m_poolHandleCtx.Free(pHandle);
				m_mapClient.erase(it);
				flag = true;
			}
		}

		if (pIoData != NULL)
		{
			m_pMemPool->Free(pIoData->pPackage);
			m_poolIoCtx.Free(pIoData);
		}
	}

	UINT __stdcall CNetLayer::AcceptThread(PVOID lpVoid)
	{
		CNetLayer* pNetlayer = (CNetLayer*)lpVoid;

		HANDLE hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		WSAEventSelect(pNetlayer->m_ListenSock, hAcceptEvent, FD_ACCEPT);

		while(1)
		{
			HANDLE handle[] = {pNetlayer->m_hExited, hAcceptEvent};
			DWORD  dwError = WaitForMultipleObjects(2, handle, FALSE, HEART_INTERVAL);
			if (dwError == WAIT_FAILED || dwError == WAIT_OBJECT_0)
				break;
			else if (dwError == WAIT_TIMEOUT)
			{
				//handle connect but not send data's socket.
//				CGuard<CLock> lock(pNetlayer->m_OptexClient);
// 				std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = pNetlayer->m_mapClient.begin();
// 				for (; it != pNetlayer->m_mapClient.end(); it++)
// 				{
// 					if (GetTickCount() - it->second->dwLastRecvDataTime > HEART_INTERVAL)
// 					{
// 						pNetlayer->DisConnect(it->first);
// 					}
// 				}
			}
			else if ( dwError == WAIT_OBJECT_0 + 1)
			{
				pNetlayer->PostAccepRequest();
			}
		}
		OutputDebugString(TEXT("i8desk Netlayer exit accept thread.\r\n"));
		return 0;
	}

	UINT __stdcall CNetLayer::WorkThread(PVOID lpVoid)
	{
		CoInitialize(NULL);
		CNetLayer* pNetLayer = (CNetLayer*)lpVoid;
		DWORD dwReadBytes = 0;
		PER_IO_CONTEXT* pIoData = NULL;
		PER_HANLDE_CONTENT* pHandle = NULL;
		while (1)
		{
			BOOL flag = ::GetQueuedCompletionStatus(pNetLayer->m_hIocp,	&dwReadBytes, 
				(PDWORD)&pHandle, (LPOVERLAPPED*)&pIoData, INFINITE);

			if (dwReadBytes ==0 && pHandle == NULL && pIoData == (LPOVERLAPPED)-1)
				break;

			if (!flag || !pIoData)
			{
				pNetLayer->OnClose(pHandle, pIoData);
				continue ;
			}

			if (/*pIoData->dwEvent != EVENT_ACCEPT &&*/ dwReadBytes == 0)
			{
				pNetLayer->OnClose(pHandle, pIoData);
				continue ;
			}

			switch (pIoData->dwEvent)
			{
			case EVENT_ACCEPT:
				{
					DWORD dwError = 0;
					if(SOCKET_ERROR == setsockopt(
						pIoData->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
						(char*)&(pNetLayer->m_ListenSock), sizeof(pNetLayer->m_ListenSock)))
					{
						dwError = WSAGetLastError();
					}
					pHandle = (PER_HANLDE_CONTENT*)pNetLayer->m_poolHandleCtx.Alloc(0);
					pHandle->socket = pIoData->socket;
					pHandle->firstpackage = true;
					pNetLayer->OnAccept(pHandle, pIoData, dwReadBytes);
					break;
				}
			case EVENT_RECV:
				{
					pNetLayer->OnRecv(pHandle, pIoData, dwReadBytes);
					break;
				}
			case EVENT_SEND:
				{
					pNetLayer->OnSend(pHandle, pIoData, dwReadBytes);
					break;
				}
			}
		}
		CoUninitialize();
		OutputDebugString(TEXT("i8desk Netlayer exit work thread.\r\n"));
		return 0;
	}

}//end of namespace i8desk.