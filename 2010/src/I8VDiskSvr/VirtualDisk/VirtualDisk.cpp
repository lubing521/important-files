#include "stdafx.h"
#include <process.h>
#include <winsock2.h>
#include <mswsock.h>
#include <Winioctl.h>
#include <time.h>
#include <shlwapi.h>
#include "VirtualDisk.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

#pragma warning(disable:4786)
#include <map>
#include <vector>

#define     DEFAULT_BLOCK	0x1000ul

#pragma pack(push,8)
#pragma warning(disable:4200)

#define DRP_MJ_REFRESH	0x01		//刷新虚拟为盘，上行只有一个命令，应答的数据空。只是成功。
#define DRP_MJ_READ		0x02		//读数据，上行，包括读数据的位置（扇区的偏移量），以及长度。应答表示成功或者失败，成功时要应答传回的数据的长度（与请求长度一样大）　
#define DRP_MJ_WRITE	0x03		//没有用。
#define DRP_MJ_QUERY	0x04		//查询分区大小，应答是devinfo.

typedef struct tagDRP
{
	BYTE			 MajorFunction;//1: Refresh, 2: Read, 3: Write, 4:Query.
	union
	{
		struct 
		{
			LARGE_INTEGER	Offset;
			ULONG			Length;
		}Read;
		struct 
		{
			LARGE_INTEGER   Offset;
			ULONG			Length;
		}Write;
		struct 
		{
			ULONG	DiskSize;
		}Query;
	};
	char buffer[0];
}DRP,*LPDRP;

typedef struct tagDSTATUS
{
	WORD	Status;
	DWORD	Information;
	DWORD	Length;
	char	buffer[0];
}DSTATUS,*LPDSTATUS;

#define STATUS_SUCCESS		0x0000
#define STATUS_INVALID_CALL 0x2001
#define STATUS_READ_ERROR	0x2002
#define STATUS_WRITE_ERROR  0x2003

typedef struct tagDEVINFO
{
	UCHAR			Type;	//0:protect 1:readonly 2:read write
	LARGE_INTEGER	Size;
}DEVINFO, *LPDEVINFO;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////
//缓存驱动的接口定义
typedef struct _CACHEOBJECT
{
	LPCWSTR    tempFile;			/*in */
	WCHAR      PartitionLetter;		/*in */
	HANDLE     PartitionHandle;
	BOOL       Locked;	
	LARGE_INTEGER      CacheLeft;			/*in */
	HANDLE     ReadEvent;
	HANDLE     WriteEvent;
	LARGE_INTEGER LastUse;
	HANDLE     hTmpFile;
	ULONG      ReadCacheCounter;
	ULONG      ReadDiskCounter;
	LARGE_INTEGER DriverSize;		/*in */
} CACHEOBJECT,*PCACHEOBJECT;

typedef VOID (__stdcall *PFNRELEASE)(PCACHEOBJECT cacheobj);
typedef int  (__stdcall *PFNOPENPARTITION)(PCACHEOBJECT cacheobj);
typedef BOOL (__stdcall *PFNREADDATA)(PCACHEOBJECT cacheobj, LARGE_INTEGER offset,PVOID Buf,ULONG Len);
//////////////////////////////////////////////////////////////////////////



#include <iostream>
#define PACKAGE_BUFFER_SIZE	(0x10000 + sizeof(DRP))



namespace i8desk
{
	class COptex
	{
	public:
		COptex()		{ InitializeCriticalSection(&cs); }
		~COptex()		{ DeleteCriticalSection(&cs);	  }
	public:
		void Enter()	{ EnterCriticalSection(&cs); }
		void Leave()	{ LeaveCriticalSection(&cs); }
	private:
		CRITICAL_SECTION cs;
	};

	class CObjectLock
	{
	public:
		CObjectLock(COptex& lock) : m_lock(lock) { m_lock.Enter();}
		~CObjectLock() { m_lock.Leave(); }
	private:
		COptex& m_lock;
	};

	class CMemPool
	{
	private:
		DWORD m_dwDefPoolSize;
		DWORD m_dwDefBlockSize;
		DWORD m_dwDefIncrease;
		COptex m_Lock;
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
			//CObjectLock lock(m_Lock);
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
					OutputDebugString(TEXT("netlayer.alloc memory fail."));
				}
			}
		}

		virtual void* Alloc(DWORD dwSize)
		{
			CObjectLock lock(m_Lock);

			if (dwSize <= m_dwDefBlockSize) //|| dwSize == 0 
			{
				if (m_mapFree.size() == 0)
				{
					AddBlock(m_dwDefIncrease, m_dwDefBlockSize);
				}
				if (m_mapFree.size() == 0)
					return NULL;
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
			CObjectLock lock(m_Lock);


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
			CObjectLock lock(m_Lock);
			try 
			{
				std::map<LPSTR, DWORD>::iterator it = m_mapFree.begin();
				for (; it != m_mapFree.end(); it++)
				{
					delete [](it->first);
				}
				m_mapFree.clear();
				for (it=m_mapUsed.begin(); it != m_mapUsed.end(); it++)
				{
					delete [](it->first);
				}
				m_mapUsed.clear();

				for (it=m_mapBigFree.begin(); it != m_mapBigFree.end(); it++)
				{
					delete [](it->first);
				}
				m_mapBigFree.clear();

				for (it=m_mapBigUsed.begin(); it != m_mapBigUsed.end(); it++)
				{
					delete [](it->first);
				}
				m_mapBigUsed.clear();
			}
			catch (...) { }
		}

		virtual DWORD GetBufSize(void* pBlock)
		{
			CObjectLock lock(m_Lock);
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
			CObjectLock lock(m_Lock);
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


	class CNetLayer
	{
	public:
		enum EVENT_ENUM
		{
			EVENT_ACCEPT	= 0,	//连接事件
			EVENT_SEND		= 1,	//发送事件
			EVENT_RECV		= 2,	//接收事件
			EVENT_FREAD		= 3
		};
		struct PER_HANLDE_CONTENT
		{
			SOCKET socket;
			sockaddr_in	addr;
			DWORD		speed;
			DWORD		total;
			DWORD		lasttime;
		};

		struct PER_IO_CONTEXT : public WSAOVERLAPPED
		{
			EVENT_ENUM dwEvent;		//事件类型
			SOCKET socket;			//收发数据的socket.
			DWORD  dwLength;		//己接收，或者还需要发送的字节数
			char*  pPackage;		//指向数据包的头指针，它不会改变.该缓冲是通过IMemPool分配和释放的
			WSABUF Buffer;			//用于WSASend/WSARecv操作的缓冲.

			DWORD  dwFileLength;
		};
	public:
		CNetLayer();
		virtual ~CNetLayer();
		virtual void Release() { delete this; }
	public:
		virtual DWORD Start(LPCSTR lpszIp, WORD wPort, LPCSTR lpszFileName);
		virtual void  Stop();

		virtual DWORD ParasePackage(char* package, DWORD length);

		virtual void  SendPackage(SOCKET sck, LPSTR pPackage, DWORD length);
		virtual void  DisConnect(SOCKET sck);

		BOOL  GetVDiskInfo(PDWORD pdwConnect, PDWORD pdwVersion)
		{
			CObjectLock lock(m_OptexClient);			
			*pdwConnect= m_mapClient.size();
			*pdwVersion = m_dwVersion;

			#pragma warning(disable:4996)
			if ((m_pCacheObject != NULL) && (m_pCacheObject->ReadDiskCounter + m_pCacheObject->ReadCacheCounter))
			{
				TCHAR szLog[MAX_PATH] = {0};
				int n = m_pCacheObject->ReadCacheCounter*100 / (m_pCacheObject->ReadCacheCounter + m_pCacheObject->ReadDiskCounter);
				_stprintf(szLog, TEXT("vdisk:%d"), n);
				OutputDebugString(szLog);
			}
			return TRUE;
		}

		void  RefreshBufferEx()
		{
			FlushFileBuffers(m_hFile);
			CObjectLock lock(m_OptexDiskBuffer);
			m_poolDiskBuffer.FreeAll();
			m_pDiskBuffer.clear();
			m_dwVersion = _time32(NULL);
			g_sock = INVALID_SOCKET;
		}

	private:
		DWORD StartListenSocket();
		DWORD CreateDiskFile(LPCSTR lpFileName);

		DWORD PostAccepRequest();
		DWORD PostRecvRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData = NULL);
		DWORD PostSendRequest(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);

		void  OnAccept(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnRecv (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnSend (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  OnClose(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData);

		void  OnFileRead (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength);
		void  Handle(PER_HANLDE_CONTENT*pHandle, PER_IO_CONTEXT* pIoData);

		static UINT __stdcall AcceptThread(PVOID lpVoid);
		static UINT __stdcall WorkThread(PVOID lpVoid);
	private:
		HANDLE m_hIocp;
		SOCKET m_ListenSock;
		WORD   m_wPort;
		char   m_szIp[32];
		LPFN_ACCEPTEX m_lpfnAcceptEx;
		HANDLE m_hExited;
		std::vector<HANDLE>	m_vecThreadHandle;

		CMemPool m_poolIoCtx;
		CMemPool m_poolHandleCtx;
		CMemPool m_pMemPool;

		COptex m_OptexClient;		
		std::map<SOCKET, PER_HANLDE_CONTENT*> m_mapClient;

		HANDLE	 m_hFile;
		HANDLE   m_hFile2;
		LARGE_INTEGER  m_nFileSize;

		//for buffer.
		SOCKET g_sock;	//first socket of requesting refersh disk.
		std::map<__int64, char*> m_pDiskBuffer;
		COptex	m_OptexDiskBuffer;
		CMemPool m_poolDiskBuffer;
		DWORD    m_dwVersion;

		PCACHEOBJECT		m_pCacheObject;
		PFNOPENPARTITION	m_pfnOpenPartition;
		PFNREADDATA			m_pfnReadData;
		PFNRELEASE			m_pfnRelease;
	};

	CNetLayer::CNetLayer() : 
		m_hIocp(NULL),
		m_ListenSock(INVALID_SOCKET),
		m_wPort(7918),
		m_lpfnAcceptEx(NULL),
		m_hExited(NULL),
		m_hFile(INVALID_HANDLE_VALUE),
		m_hFile2(INVALID_HANDLE_VALUE),
		m_dwVersion(_time32(NULL)), 
		g_sock(INVALID_SOCKET), 
		m_pCacheObject(NULL),
		m_pfnOpenPartition(NULL),
		m_pfnReadData(NULL),
		m_pfnRelease(NULL)
	{
		WSAData wsaData = {0};
		WSAStartup(MAKEWORD(2,2), &wsaData);
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		ZeroMemory(m_szIp, sizeof(m_szIp));
		m_poolIoCtx.SetDefault(100, sizeof(PER_IO_CONTEXT), 10);
		m_poolHandleCtx.SetDefault(100, sizeof(PER_HANLDE_CONTENT), 10);
		m_pMemPool.SetDefault(100, PACKAGE_BUFFER_SIZE, 10);
		m_poolDiskBuffer.SetDefault(100, DEFAULT_BLOCK, 100);

		TCHAR szDll[MAX_PATH] = {0};
		GetModuleFileName(NULL, szDll, _countof(szDll));
		PathRemoveFileSpec(szDll);
		PathAddBackslash(szDll);
		lstrcat(szDll, TEXT("i8Cache.dll"));
		HMODULE hModCache  = LoadLibrary(szDll);
		m_pfnOpenPartition = (PFNOPENPARTITION)GetProcAddress(hModCache, "OpenPartition");
		m_pfnReadData	   = (PFNREADDATA)GetProcAddress(hModCache, "ReadData");
		m_pfnRelease	   = (PFNRELEASE)GetProcAddress(hModCache, "Release");
		if (!m_pfnOpenPartition || !m_pfnReadData || !m_pfnRelease)
		{
			m_pfnOpenPartition = NULL;
			m_pfnReadData	   = NULL;
			m_pfnRelease	   = NULL;
		}
	}

	CNetLayer::~CNetLayer()
	{
		Stop();
		CloseHandle(m_hExited);
		WSACleanup();
	}

	DWORD CNetLayer::Start(LPCSTR lpszIp, WORD wPort, LPCSTR lpszFileName)
	{
		::OutputDebugString("开始启动...");
		g_sock = INVALID_SOCKET;
		m_wPort = wPort;
		if (lpszIp != NULL)
			lstrcpy(m_szIp, lpszIp);
		else
			m_szIp[0] = 0;
		m_hIocp = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (m_hIocp == NULL)
			return GetLastError();

		::OutputDebugString("创建DiskFile");
		DWORD dwError = CreateDiskFile(lpszFileName);
		if (dwError != ERROR_SUCCESS)
			return dwError;

		::OutputDebugString("启动监听socket...");
		dwError = StartListenSocket();
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
		
		try
		{
			::OutputDebugString("创建缓存...");
			if (m_pfnOpenPartition != NULL && m_pfnReadData != NULL && m_pfnRelease != NULL)
			{
				m_pCacheObject = new _CACHEOBJECT;
				ZeroMemory(m_pCacheObject, sizeof(_CACHEOBJECT));
				m_pCacheObject->tempFile = NULL;
				m_pCacheObject->PartitionLetter = (WCHAR)lpszFileName[4];

				//read cache size from ini.
				{
					TCHAR szIniFile[MAX_PATH] = {0};
					GetModuleFileName(NULL, szIniFile, _countof(szIniFile));
					PathRemoveFileSpec(szIniFile);
					PathAddBackslash(szIniFile);
					lstrcat(szIniFile, TEXT("Data\\I8SyncSvr.ini"));
					m_pCacheObject->CacheLeft.QuadPart = GetPrivateProfileInt(TEXT("System"), "CacheSize", 0, szIniFile) * 1024LL * 1024;
				}

				m_pCacheObject->DriverSize = m_nFileSize;
				if (m_pfnOpenPartition(m_pCacheObject))
				{
					delete m_pCacheObject;
					m_pCacheObject = NULL;
				}
			}
		}
		catch (...) { delete m_pCacheObject; m_pCacheObject = NULL; }

		::OutputDebugString("成功完成...");
		return ERROR_SUCCESS;
	}

	void CNetLayer::Stop()
	{
		try
		{
			g_sock = INVALID_SOCKET;
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

			std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = m_mapClient.begin();
			for (; it != m_mapClient.end(); it ++)
			{
				closesocket(it->first);
				m_poolHandleCtx.Free(it->second);
			}
			m_mapClient.clear();
			m_poolHandleCtx.FreeAll();
			m_poolIoCtx.FreeAll();
			if (m_hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
			}
			if (m_hFile2 != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hFile2);
				m_hFile2 = INVALID_HANDLE_VALUE;
			}
			m_poolDiskBuffer.FreeAll();

			try
			{
				if (m_pCacheObject && m_pfnRelease)
				{
					m_pfnRelease(m_pCacheObject);
					delete m_pCacheObject;
					m_pCacheObject = NULL;
				}
			}
			catch (...) { m_pCacheObject = NULL; }

		}catch (...) {}
	}

	DWORD CNetLayer::ParasePackage(char* package, DWORD length)
	{
		DRP *pDrp = (DRP *)package;

		if( pDrp->MajorFunction == DRP_MJ_READ && pDrp->Write.Length > 0x10000 )
		{
			assert(0);
			return 0;
		}

		return sizeof(DRP);
	}

	void CNetLayer::SendPackage(SOCKET sck, LPSTR pPackage, DWORD length)
	{
		PER_HANLDE_CONTENT* pHandle = NULL;
		{
			CObjectLock lock(m_OptexClient);
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
			m_pMemPool.Free(pPackage);
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
// 		if (lstrlen(m_szIp))
// 			addr.sin_addr.s_addr = inet_addr(m_szIp);
// 		else
			addr.sin_addr.s_addr = 0;
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

	DWORD CNetLayer::CreateDiskFile(LPCSTR lpFileName)
	{
		m_hFile = CreateFile(lpFileName, 
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING,
			NULL
			);

		if (m_hFile == INVALID_HANDLE_VALUE)
			return GetLastError();

		m_hFile2 = CreateFile(lpFileName, 
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, 
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_NO_BUFFERING,
			NULL);

		if (m_hFile2 == INVALID_HANDLE_VALUE)
		{
			DWORD dwErr = GetLastError();
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			return dwErr;
		}

		//get disk file size.
		PARTITION_INFORMATION PartInfo;
		ULONG IoLength = sizeof(PARTITION_INFORMATION);
		OVERLAPPED ov;
		memset(&ov, 0, sizeof(ov));
		ov.hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\VDISK_DSCSISERVERINIT");
		if( ov.hEvent == NULL )
			return GetLastError();

		DeviceIoControl(m_hFile, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0,
			&PartInfo, sizeof(PARTITION_INFORMATION), &IoLength, &ov);

		WaitForSingleObject(ov.hEvent, INFINITE);
		::CloseHandle(ov.hEvent);

		m_nFileSize.QuadPart  = PartInfo.PartitionLength.QuadPart;

		CreateIoCompletionPort(m_hFile, m_hIocp, NULL, 0);

		return ERROR_SUCCESS;
	}

	void CNetLayer::DisConnect(SOCKET sck)
	{
		try
		{
			shutdown(sck, SD_BOTH);
			closesocket(sck);
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
			pHandleCtx->pPackage = (char*)m_pMemPool.Alloc(0);
			pHandleCtx->Buffer.buf = pHandleCtx->pPackage;
			pHandleCtx->dwLength = 0;
			pHandleCtx->Buffer.len = m_pMemPool.GetBufSize(pHandleCtx->pPackage);
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
					char buf[MAX_PATH] = {0};
					sprintf_s(buf, "VDisk:m_lpfnAcceptEx error:%d\r\n", dwError);
					OutputDebugStringA(buf);
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
			pIoData->pPackage = (char*)m_pMemPool.Alloc(0);
			pIoData->dwLength = 0;
			pIoData->Buffer.buf = pIoData->pPackage;
			pIoData->Buffer.len = m_pMemPool.GetBufSize(pIoData->pPackage);
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
		pIoData->Buffer.len = pIoData->dwLength;
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
			CObjectLock lock(m_OptexClient);
			m_mapClient.insert(std::make_pair(pHandle->socket, pHandle));
			pHandle->lasttime = GetTickCount();
			pHandle->speed = 0;
			pHandle->total = 0;
			int nSize = sizeof(pHandle->addr);
			getpeername(pHandle->socket, (PSOCKADDR)&pHandle->addr, &nSize);
			{
				char buf[MAX_PATH] = {0};
				sprintf_s(buf, "VDisk: %s connected.\r\n", inet_ntoa(pHandle->addr.sin_addr));
				OutputDebugStringA(buf);
			}
						
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
		CreateIoCompletionPort((HANDLE)pHandle->socket, m_hIocp, (DWORD)pHandle, 0);
		if (dwLength)
			OnRecv(pHandle, pIoData, dwLength);
	}

	void CNetLayer::OnRecv (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		pIoData->dwLength += dwLength;
		DWORD dwTotal = ParasePackage(pIoData->pPackage, pIoData->dwLength);
		DWORD dwBuffSize = m_pMemPool.GetBufSize(pIoData->pPackage);
		
		// 接收数据大小出错，主动断开
		if( dwTotal == 0 )
		{
			::shutdown(pHandle->socket, SD_BOTH);
			return;
		}

		if (dwTotal > dwBuffSize)
		{
			char* pPackage = (char*)m_pMemPool.Alloc(dwTotal);
			memcpy(pPackage, pIoData->pPackage, pIoData->dwLength);
			pIoData->Buffer.buf = (pPackage + pIoData->dwLength);
			pIoData->Buffer.len = m_pMemPool.GetBufSize(pPackage) - pIoData->dwLength;
			m_pMemPool.Free(pIoData->pPackage);
			pIoData->pPackage = pPackage;
			PostRecvRequest(pHandle, pIoData);
		}
		else
		{
			if (dwTotal == pIoData->dwLength)
			{
				Handle(pHandle, pIoData);
				m_pMemPool.Free(pIoData->pPackage);
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
					char* pNewPackage = (char*)m_pMemPool.Alloc(dwTotal);
					memcpy(pNewPackage, pIoData->pPackage, dwTotal);					
					m_pMemPool.Free(pNewPackage);
					dwDataLen -= dwTotal;
					if (dwDataLen)
					{	
						pNewPackage = (char*)m_pMemPool.Alloc(dwDataLen);
						memcpy(pNewPackage, pIoData->pPackage+ dwTotal, dwDataLen);
						m_pMemPool.Free(pIoData->pPackage);
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
						break;
					}
				}
				if (dwDataLen == 0)
				{
					m_pMemPool.Free(pIoData->pPackage);
					m_poolIoCtx.Free(pIoData);
					PostRecvRequest(pHandle, NULL);
				}
				else
				{
					pIoData->dwLength = dwDataLen;
					pIoData->Buffer.buf = pIoData->pPackage + dwDataLen;
					pIoData->Buffer.len = m_pMemPool.GetBufSize(pIoData->pPackage) - dwDataLen;					
					PostRecvRequest(pHandle, pIoData);
				}
			}
		}
	}

	void CNetLayer::OnSend (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{	
		if (dwLength >= pIoData->dwLength)
		{
			m_pMemPool.Free(pIoData->pPackage);
			m_poolIoCtx.Free(pIoData);
		}
		else
		{
			PostSendRequest(pHandle, pIoData, dwLength);
		}


		{
			pHandle->total += dwLength;
			DWORD dwDiff = GetTickCount() - pHandle->lasttime;
			if (dwDiff >= 100)
			{
				pHandle->speed = (DWORD)(pHandle->total * 1000.0 / dwDiff);
				pHandle->lasttime = GetTickCount();
				pHandle->total = 0;
				/*#ifdef _DEBUG
				char buf[MAX_PATH] = {0};
				sprintf(buf, "ip:%s,Speed:%.2f\r\n", 
				inet_ntoa(pHandle->addr.sin_addr),
				pHandle->speed*1.0/1000/1000);
				OutputDebugString(buf);
				#endif*/
			}
		}
	}

	void CNetLayer::OnClose(PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData)
	{
		if (pHandle != NULL)
		{
			CObjectLock lock(m_OptexClient);
			std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = m_mapClient.find(pHandle->socket);
			if (it != m_mapClient.end())
			{
				char buf[MAX_PATH] = {0};
				sprintf_s(buf, "VDisk: %s closed:%d.\r\n", inet_ntoa(pHandle->addr.sin_addr), WSAGetLastError());
				OutputDebugStringA(buf);
				shutdown(pHandle->socket, SD_BOTH);
				closesocket(pHandle->socket);
				m_mapClient.erase(pHandle->socket);
				m_poolHandleCtx.Free(pHandle);
				
			}
		}
		if (pIoData != NULL)
		{
			m_pMemPool.Free(pIoData->pPackage);
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
			DWORD  dwError = WaitForMultipleObjects(2, handle, FALSE, INFINITE);
			if (dwError == WAIT_FAILED || dwError == WAIT_OBJECT_0)
				break;
			else if (dwError == WAIT_TIMEOUT)
			{
				//???
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

			if (pIoData->dwEvent != EVENT_ACCEPT && dwReadBytes == 0)
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
						char buf[MAX_PATH] = {0};
						sprintf_s(buf, "VDisk:setsockopt error:%d\r\n", dwError);
						OutputDebugStringA(buf);
					}
					pHandle = (PER_HANLDE_CONTENT*)pNetLayer->m_poolHandleCtx.Alloc(0);
					pHandle->socket = pIoData->socket;
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
			case EVENT_FREAD:
				{
					{
						CObjectLock lock(pNetLayer->m_OptexClient);
						std::map<SOCKET, PER_HANLDE_CONTENT*>::iterator it = pNetLayer->m_mapClient.find(pIoData->socket);
						if (it != pNetLayer->m_mapClient.end())
							pHandle = it->second;
					}
					if (pHandle != NULL)
						pNetLayer->OnFileRead(pHandle, pIoData, dwReadBytes);
					break;
				}
			}
		}
		OutputDebugString(TEXT("i8desk Netlayer exit work thread.\r\n"));
		return 0;
	}

	void CNetLayer::OnFileRead (PER_HANLDE_CONTENT* pHandle, PER_IO_CONTEXT* pIoData, DWORD dwLength)
	{
		pIoData->dwLength += dwLength;
		if (pIoData->dwLength == pIoData->dwFileLength)
		{
			LPDSTATUS lpStatus = (LPDSTATUS)pIoData->pPackage;
			lpStatus->Length = lpStatus->Information = pIoData->dwLength;
			lpStatus->Status = STATUS_SUCCESS;

			pIoData->dwLength += sizeof(DSTATUS);
			pIoData->Buffer.buf = pIoData->pPackage;
			pIoData->Buffer.len = pIoData->dwLength;
			pIoData->dwEvent = EVENT_SEND;
			memset((LPOVERLAPPED)pIoData, 0, sizeof(WSAOVERLAPPED));
			PostSendRequest(pHandle, pIoData, 0);
		}
		else
		{
			DWORD dwReadBytes;
			LARGE_INTEGER li;
			li.LowPart = pIoData->Offset;
			li.HighPart = pIoData->OffsetHigh;
			li.QuadPart += dwLength;

			pIoData->Offset = li.LowPart;
			pIoData->OffsetHigh = li.HighPart;		
			LPDSTATUS lpStatus = (LPDSTATUS)pIoData->pPackage;
			ReadFile(m_hFile, lpStatus->buffer + pIoData->dwLength, 
				pIoData->dwFileLength-pIoData->dwLength, &dwReadBytes, (LPOVERLAPPED)pIoData);
		}
	}

	void CNetLayer::Handle(PER_HANLDE_CONTENT*pHandle, PER_IO_CONTEXT* pIoData)
	{
		DRP* pDrp = (DRP*)pIoData->pPackage;

		switch( pDrp->MajorFunction)
		{
		case DRP_MJ_REFRESH:
			{
				OutputDebugString("I8VDisk:Recv Refresh Vdisk.");
				RefreshBufferEx();
				LPDSTATUS lpStatus = (LPDSTATUS)m_pMemPool.Alloc(0);
				lpStatus->Information = 0;
				lpStatus->Length = 0;
				lpStatus->Status = STATUS_SUCCESS;
				SendPackage(pHandle->socket, (LPSTR)lpStatus, sizeof(DSTATUS));
				OutputDebugString("I8VDisk:Ack Refresh Vdisk.");
			}
			break;
		case DRP_MJ_QUERY:
			{
				CObjectLock lock(m_OptexDiskBuffer);
				if (g_sock == INVALID_SOCKET) 
				{
					g_sock = pHandle->socket;
				}
				LPDSTATUS lpStatus = (LPDSTATUS)m_pMemPool.Alloc(0);
				lpStatus->Information = sizeof(DEVINFO);
				lpStatus->Length = sizeof(DEVINFO);
				lpStatus->Status = STATUS_SUCCESS;

				LPDEVINFO lpDevInfo = (LPDEVINFO)lpStatus->buffer;
				lpDevInfo->Type = 0;
				lpDevInfo->Size.QuadPart = m_nFileSize.QuadPart;

				SendPackage(pHandle->socket, (LPSTR)lpStatus, sizeof(DSTATUS) + sizeof(DEVINFO));
			}
			break;
		case DRP_MJ_READ:
			{
				DWORD dwReadBytes = 0;
				//alloc a buffer for reading data from disk.
				PER_IO_CONTEXT* pNewIoData = (PER_IO_CONTEXT*)m_poolIoCtx.Alloc(0);
				pNewIoData->pPackage = (char*)m_pMemPool.Alloc(0);
				pNewIoData->Buffer.buf = pNewIoData->pPackage;
				pNewIoData->socket = pHandle->socket;
				pNewIoData->dwEvent = EVENT_FREAD;
				pNewIoData->dwLength = 0;

				LPDSTATUS lpStatus = (LPDSTATUS)pNewIoData->pPackage;
				lpStatus->Length = lpStatus->Information = pDrp->Read.Length;
				lpStatus->Status = STATUS_SUCCESS;

				//read file.
				BOOL  bNeedReadFile = TRUE;
				DWORD dwLength = pDrp->Read.Length;
				char buf[MAX_PATH] = {0};
				if (dwLength <= DEFAULT_BLOCK)
				{
					LARGE_INTEGER begin;
					LARGE_INTEGER off;
					begin.QuadPart = pDrp->Read.Offset.QuadPart;
					off.QuadPart = begin.QuadPart - (begin.QuadPart % DEFAULT_BLOCK);

					if (DEFAULT_BLOCK - dwLength  >= begin.QuadPart - off.QuadPart)
					{
						CObjectLock lock(m_OptexDiskBuffer);
						std::map<__int64, char*>::iterator it = m_pDiskBuffer.find(off.QuadPart);
						if (it != m_pDiskBuffer.end())
						{
							bNeedReadFile = FALSE;
							memcpy(lpStatus->buffer, it->second + (DWORD)(begin.QuadPart % DEFAULT_BLOCK), dwLength);		
						}
						else
						{
							if (g_sock == pHandle->socket && m_pDiskBuffer.size() < 10000)
							{
								char* p = (char*)m_poolDiskBuffer.Alloc(0);
								LONG nHigh =  off.HighPart;
								SetFilePointer(m_hFile2, off.LowPart, &nHigh, FILE_BEGIN);
								if (ReadFile(m_hFile2, p, DEFAULT_BLOCK, &dwReadBytes, NULL))
								{
									bNeedReadFile = FALSE;
									memcpy(lpStatus->buffer, p + (DWORD)(begin.QuadPart % DEFAULT_BLOCK), dwLength);
									m_pDiskBuffer.insert(std::make_pair(off.QuadPart, p));
								}
								else
								{
									bNeedReadFile = TRUE;
									char szLog[MAX_PATH] = {0};
									sprintf_s(szLog, "VDisk:ReadFile Line:%d,Error:%d\r\n", __LINE__, GetLastError());
									OutputDebugStringA(szLog);
								}
							}
						}
					}
				}

				if (bNeedReadFile && m_pCacheObject != NULL)
				{
					if (0 == m_pfnReadData(m_pCacheObject, pDrp->Read.Offset, lpStatus->buffer, pDrp->Read.Length))
					{
						/*char szLog[MAX_PATH] = {0};
						sprintf_s(szLog, "VDisk:Read Cache File Line:%I64d,length:%d\r\n", 
							pDrp->Read.Offset.QuadPart, pDrp->Read.Length);
						OutputDebugStringA(szLog);
						*/
						bNeedReadFile = FALSE;
					}
				}
				if (bNeedReadFile)
				{
					ZeroMemory((LPOVERLAPPED)pNewIoData, sizeof(OVERLAPPED));
					pNewIoData->Offset = pDrp->Read.Offset.LowPart;
					pNewIoData->OffsetHigh = pDrp->Read.Offset.HighPart;
					pNewIoData->dwFileLength = pDrp->Read.Length;
					if (!ReadFile(m_hFile, lpStatus->buffer, pNewIoData->dwFileLength, &dwReadBytes, (LPOVERLAPPED)pNewIoData))
					{
						DWORD dwError = GetLastError();
						if (dwError != ERROR_IO_PENDING)
						{
							char szLog[MAX_PATH] = {0};
							sprintf_s(szLog, "VDisk:ReadFile Line:%d,Error:%d\r\n", __LINE__, dwError);
							OutputDebugStringA(szLog);
							lpStatus->Length = 0;
							lpStatus->Information = 0;
							lpStatus->Status = STATUS_READ_ERROR;
							pNewIoData->dwEvent = EVENT_SEND;
							pNewIoData->Buffer.len = sizeof(DSTATUS);
							pNewIoData->dwLength = sizeof(DSTATUS);
							PostSendRequest(pHandle, pNewIoData, 0);
						}
					}
				}
				else
				{
					pNewIoData->dwEvent = EVENT_SEND;
					pNewIoData->dwLength = sizeof(DSTATUS)+ dwLength;
					pNewIoData->Buffer.len = sizeof(DSTATUS) + dwLength;
					PostSendRequest(pHandle, pNewIoData, 0);
				}
			}
			break;
		case DRP_MJ_WRITE:
			{
				PER_IO_CONTEXT* pNewIoData = (PER_IO_CONTEXT*)m_poolIoCtx.Alloc(0);
				pNewIoData->pPackage = (char*)m_pMemPool.Alloc(0);
				pNewIoData->Buffer.buf = pNewIoData->pPackage;
				pNewIoData->Buffer.len = pNewIoData->dwLength = sizeof(DSTATUS);
				pNewIoData->socket = pHandle->socket;
				pNewIoData->dwEvent = EVENT_SEND;

				LPDSTATUS lpStatus = (LPDSTATUS)pNewIoData->pPackage;
				lpStatus->Length = lpStatus->Information = sizeof(DSTATUS);
				lpStatus->Status = STATUS_SUCCESS;
				PostSendRequest(pHandle, pNewIoData, 0);
			}
			break;
		}
	}

	DWORD WINAPI StartServer(PHANDLE handle, LPCSTR lpszSvrIP, char szDriver, WORD wPort)
	{
		CNetLayer* pserver = new CNetLayer;
		char szFileName[MAX_PATH] = {0};
		sprintf_s(szFileName, "\\\\.\\%C:", szDriver);
		DWORD dwError = pserver->Start(lpszSvrIP, wPort, szFileName);
		if (ERROR_SUCCESS != dwError)
		{
			*handle = NULL;
			delete pserver;
			return dwError;
		}
		*handle = reinterpret_cast<HANDLE>(pserver);
		return ERROR_SUCCESS;
	}

	void WINAPI GetVdiskInfo(HANDLE handle, PDWORD pdwConnect, PDWORD pdwVersion)
	{
		if (handle == NULL) 
			return ;
		try
		{
			CNetLayer* pServer = (CNetLayer*)handle;
			pServer->GetVDiskInfo(pdwConnect, pdwVersion);
		}
		catch (...) { }
	}


	void WINAPI StopServer(HANDLE handle)
	{
		try
		{
			CNetLayer* pServer = (CNetLayer*)handle;
			if (pServer != NULL)
			{
				pServer->Stop();
				delete pServer;
			}
			pServer = NULL;
		}
		catch (...) {	}
	}

	void WINAPI RefreshBuffer(HANDLE handle)
	{
		try
		{
			CNetLayer* pServer = (CNetLayer*)handle;
			if (pServer != NULL)
				pServer->RefreshBufferEx();
		}
		catch (...) { }
	}


	class CVDiskPlug : public IPlug
	{
	public:
		CVDiskPlug()					{			   }
		virtual ~CVDiskPlug();
		virtual void	Release()		{ delete this; }
	public:
		virtual void OnAccept(INetLayer*pNetLayer, SOCKET sck, DWORD param) {}
		virtual void OnSend(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) {}
		virtual void OnRecv(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage, DWORD param) {}
		virtual void OnClose(INetLayer*pNetLayer, SOCKET sck, DWORD param) {}

		virtual LPCTSTR GetPlugName()	{ return VDISK_PLUG_NAME; }
		virtual DWORD  	GetVersion()	{ return 0x0000001;		  }
		virtual LPCTSTR GetCompany()	{ return TEXT("Goyoo");	  }
		virtual LPCTSTR GetDescription(){ return TEXT("Virtual Disk plug"); }

		virtual DWORD   GetListPort()	{ return 0;}
		virtual bool	Initialize(IPlugMgr* pPlugMgr);
		virtual bool	UnInitialize();
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2);
	private:
		IPlugMgr* m_pPlugMgr;
		CLock m_lock;
		std::map<HANDLE, tagVDInfo*> m_lstVDisk;
	};

	CVDiskPlug::~CVDiskPlug()
	{
		UnInitialize();
	}

	bool CVDiskPlug::Initialize(i8desk::IPlugMgr *pPlugMgr)
	{
		m_pPlugMgr = pPlugMgr;
		return (m_pPlugMgr != NULL);
	}

	bool CVDiskPlug::UnInitialize()
	{
		for (std::map<HANDLE, tagVDInfo*>::iterator it = m_lstVDisk.begin();
			it != m_lstVDisk.end(); it++)
		{
			delete it->second;
		}
		m_lstVDisk.clear();
		m_pPlugMgr = NULL;

		return true;
	}

	DWORD	CVDiskPlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
	{
		CAutoLock<CLock> lock(&m_lock);
		switch (cmd)
		{
		case VDISK_CMD_START:
			{
				tagVDInfo* pVDInfo = (tagVDInfo*)param1;
				HANDLE  handle = NULL;
				StartServer(&handle, "", pVDInfo->szSvrDrv, pVDInfo->nPort);
				if (handle != NULL)
				{
					tagVDInfo* pTemp = new tagVDInfo;
					*pTemp = *pVDInfo;
					m_lstVDisk.insert(std::make_pair(handle, pVDInfo));
				}
				return (DWORD)handle;
			}
			break;
		case VDISK_CMD_STOP:
			{
				HANDLE handle = (HANDLE)param1;
				std::map<HANDLE, tagVDInfo*>::iterator it = m_lstVDisk.find(handle);
				if (it != m_lstVDisk.end())
				{
					StopServer(handle);

					delete it->second;
					m_lstVDisk.erase(it);
				}
				return 0;
			}
			break;
		case VDISK_CMD_REFRESH:
			{
				HANDLE handle = (HANDLE)param1;
				std::map<HANDLE, tagVDInfo*>::iterator it = m_lstVDisk.find(handle);
				if (it != m_lstVDisk.end())
				{
					RefreshBuffer(handle);
				}
				return 0;
			}
		case VDISK_CMD_GETINFO:
			{
				HANDLE handle = (HANDLE)param1;
				tagVDInfo* pVDInfo = (tagVDInfo*)param2;
				std::map<HANDLE, tagVDInfo*>::iterator it = m_lstVDisk.find(handle);
				if (it != m_lstVDisk.end())
				{
					GetVdiskInfo(handle, &pVDInfo->dwConnect, &pVDInfo->dwVersion);
				}
				return 0;
			}
			break;
		case VDISK_CMD_GETALLINFO:
			{
				tagVDInfo* pVDInfo = (tagVDInfo*)param1;
				DWORD dwCount = param2;

				std::map<HANDLE, tagVDInfo*>::iterator it = m_lstVDisk.begin();
				DWORD idx = 0;
				for (; it != m_lstVDisk.end() && idx<dwCount; it ++)
				{
					pVDInfo[idx].dwIp		= it->second->dwIp;
					pVDInfo[idx].nPort		= it->second->nPort;
					pVDInfo[idx].szSvrDrv	= it->second->szSvrDrv;
					pVDInfo[idx].szCliDrv	= it->second->szCliDrv;
					lstrcpy(pVDInfo[idx].szTempDir, it->second->szTempDir);
					GetVdiskInfo(it->first, &pVDInfo[idx].dwConnect, &pVDInfo[idx].dwVersion);
					idx++;
				}
				return idx;
			}
			break;
		}
		return -1;
	}

	IPlug* WINAPI CreatePlug()
	{
		IPlug* pPlug = NULL;
		try
		{
			pPlug = new CVDiskPlug;
		}
		catch (...) { pPlug = NULL; }

		return pPlug;
	}
}