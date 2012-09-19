#include "stdafx.h"
#include "svrsync.h"
#include "socket.h"
#include "UpdateGame.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

CSvrSyncPlug::CSvrSyncPlug()
{
	WSADATA wsaData = {0};
	WSAStartup(0x0202, &wsaData);
}

CSvrSyncPlug::~CSvrSyncPlug()
{
	stop();
	WSACleanup();
}

bool CSvrSyncPlug::start()
{
	if ((m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
		return false;
	
	m_pPipeSync = new PCMomule<tGameInfo*>(m_hExited);
	m_pPipeDelete = new PCMomule<tGameInfo*>(m_hExited);

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdateThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_hThread.push_back(hThread);
	
	hThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdateThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_hThread.push_back(hThread);

	hThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdateThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_hThread.push_back(hThread);

	hThread = (HANDLE)_beginthreadex(NULL, 0, CheckUpdateThread, this, 0, NULL);
	if (hThread == NULL)
		return false;
	m_hThread.push_back(hThread);

	return true;
}

void CSvrSyncPlug::stop()
{
	if (m_hExited != NULL)
	{
		if (m_hThread.size())
		{
			SetEvent(m_hExited);
			WaitForMultipleObjects(m_hThread.size(), &m_hThread[0], TRUE, INFINITE);
			m_hThread.clear();
		}
		CloseHandle(m_hExited);
		m_hExited = NULL;
	}
	Release_Pointer(m_pPipeSync);
	Release_Pointer(m_pPipeDelete);
}

UINT __stdcall CSvrSyncPlug::CheckUpdateThread(LPVOID lpVoid)
{
	CSvrSyncPlug* pThis = reinterpret_cast<CSvrSyncPlug*>(lpVoid);
	HANDLE hEvent[2];
	hEvent[0] = pThis->m_hExited;
	hEvent[1] = CreateEvent(NULL, FALSE, TRUE, NULL);

	while (1)
	{
		DWORD ret = WaitForMultipleObjects(_countof(hEvent), hEvent, FALSE, INFINITE);
		if (ret == WAIT_OBJECT_0)
			break;
		else if (ret == WAIT_OBJECT_0 + 1 || ret == WAIT_TIMEOUT)
		{
			pThis->CheckUpdate();
		}
	}
	return 0;
}

UINT __stdcall CSvrSyncPlug::SyncGameThread(LPVOID lpVoid)
{
	CSvrSyncPlug* pThis = reinterpret_cast<CSvrSyncPlug*>(lpVoid);
	
	tGameInfo* pTask = NULL;
	while ((pTask = pThis->m_pPipeSync->Pop()) != NULL)
	{
		pThis->SyncGame(1);
	}
	return 0;
}

UINT __stdcall CSvrSyncPlug::DeleteGameThread(LPVOID lpVoid)
{
	CSvrSyncPlug* pThis = reinterpret_cast<CSvrSyncPlug*>(lpVoid);

	tGameInfo* pTask = NULL;
	while ((pTask = pThis->m_pPipeDelete->Pop()) != NULL)
	{
		pThis->DeleteGame(L"");
	}
	return 0;
}

UINT __stdcall CSvrSyncPlug::UdpThread(LPVOID lpVoid)
{
	CSvrSyncPlug* pThis = reinterpret_cast<CSvrSyncPlug*>(lpVoid);

	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
		return 0;
	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port	= htons(5000);
	addr.sin_addr.s_addr = ADDR_ANY;
	if (SOCKET_ERROR == bind(sck, (PSOCKADDR)&addr, sizeof(addr)))
	{
		closesocket(sck);
		return 0;
	}
	int RecvBufSize = 0x10000;
	setsockopt(sck,SOL_SOCKET, SO_RCVBUF,	(char*)&RecvBufSize, sizeof(int) );
	setsockopt(sck,SOL_SOCKET, SO_SNDBUF,	(char*)&RecvBufSize, sizeof(int) );
	WSAEVENT hNetEvent = WSACreateEvent();
	if (hNetEvent == NULL || SOCKET_ERROR == WSAEventSelect(sck, hNetEvent, FD_READ))
	{
		closesocket(sck);
		return 0;
	}
	
	HANDLE hEvents[2] = {pThis->m_hExited, hNetEvent};
	while (1)
	{
		DWORD ret = WSAWaitForMultipleEvents(_countof(hEvents), hEvents, FALSE,WSA_INFINITE, FALSE);
		if (ret == WAIT_OBJECT_0)
			break;
		else if (ret == WAIT_OBJECT_0 + 1)
		{
			WSANETWORKEVENTS NetworkEvents;
			WSAEnumNetworkEvents(sck, hNetEvent, &NetworkEvents);
			if(NetworkEvents.lNetworkEvents == FD_READ && NetworkEvents.iErrorCode[FD_READ_BIT] == 0)
			{
				char data[4096] = {0};
				pkgheader* pheader = (pkgheader*)data;
				int len = recvfrom(sck, data, sizeof(data), 0, NULL, NULL);
				if (len > 0 && pheader->Length == len)
				{
					data[len] = 0;
					try {  pThis->UdpProcess(data, len); }  catch (...) {}
				}
			}
		}
		else break;
	}
	closesocket(sck);
	WSACloseEvent(hNetEvent);
	return 0;
}

void CSvrSyncPlug::CheckUpdate()
{

}

void CSvrSyncPlug::SyncGame(DWORD gid)
{
	CGameUpdate update;
	if (!update.StartUpdate(L"", L"", UPDATE_FLAG_DELETE_MOREFILE, ""))
		return ;

	while (WAIT_TIMEOUT == WaitForSingleObject(m_hExited, 1000))
	{
		tagUpdateStatus Status = {0};
		update.GetUpdateStatus(Status);
		if (Status.Status == UPDATE_STATUS_ERROR || Status.Status == UPDATE_STATUS_FINISH)
			break;
	}
	update.StopUpdate();
}

void CSvrSyncPlug::DeleteGame(LPCWSTR dir)
{
	wchar_t filename[MAX_PATH] = {0};
	wchar_t find[MAX_PATH] = {0};
	swprintf(find, L"%s*.*", dir);

	WIN32_FIND_DATAW wfd = {0};
	HANDLE hFinder = FindFirstFile(find, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;
	do 
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
			break;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//filter dot directory.
			if (!((wfd.cFileName[0] == L'.') && 
				((wfd.cFileName[1] == 0) || (wfd.cFileName[1] == L'.' && wfd.cFileName[2] == 0))))
			{
				swprintf(filename, L"%s%s\\", dir, wfd.cFileName);
				DeleteGame(filename);
			}
		}
		else
		{
			swprintf(filename, L"%s%s", dir, wfd.cFileName);
			SetFileAttributesW(filename, FILE_ATTRIBUTE_NORMAL);
			DeleteFileW(filename);
		}
	}while (FindNextFileW(hFinder, &wfd));
	FindClose(hFinder);
}

void CSvrSyncPlug::UdpProcess(char* pdata, int len)
{
	pkgheader* phead = reinterpret_cast<pkgheader*>(pdata);
	switch (phead->Cmd)
	{
		//主服务器的通知。
	case 0x0306:	//删除游戏
		break;
	case 666:		//通知更新游戏
		break;

		//界面程序的查询

	}
}
