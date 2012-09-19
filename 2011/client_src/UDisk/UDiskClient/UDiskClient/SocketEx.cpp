#include "StdAfx.h"
#include "SocketEx.h"

CSocketEx::CSocketEx(void):m_sock(INVALID_SOCKET),m_hThread(NULL)
{
	WSADATA data;
	WSAStartup(0x0202,&data);
	m_hExit = CreateEvent(NULL,TRUE,FALSE,NULL);
}

CSocketEx::~CSocketEx(void)
{
	SetEvent(m_hExit);
	if(m_hThread)
	{
		WaitForSingleObject(m_hThread,INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	CloseConnect();
	CloseHandle(m_hExit);
	WSACleanup();
}
void CSocketEx::Release()
{
	delete this;
}
bool CSocketEx::ConnectSever(LPCSTR Srvip,DWORD dwTimeOut)
{
	CAutoLock<CLock> lock(&m_optexSocket);
	if(m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock= INVALID_SOCKET;
	}
	 m_sock = socket(AF_INET,SOCK_STREAM,0);
	 if(m_sock == INVALID_SOCKET)
	 {
		 AfxMessageBox("socket create fail");
		 return false;
	 }
	 SOCKADDR_IN Addr = {0};
	 Addr.sin_family = AF_INET;
	 Addr.sin_port = htons(PORT);
	 Addr.sin_addr.s_addr =inet_addr(Srvip); 
	 int dwTime = 2000;
	int n =  setsockopt(m_sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&dwTime,sizeof(int));
	n =  setsockopt(m_sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&dwTime,sizeof(int));
	 if(SOCKET_ERROR  == connect(m_sock,(PSOCKADDR)&Addr,sizeof(Addr)))
	 {
		 closesocket(m_sock);
		 m_sock= INVALID_SOCKET;
		 TRACE0("¡¨Ω” ß∞‹\n");
		 return false;
	 }
	return true;
}
void CSocketEx::CloseConnect()
{
	CAutoLock <CLock> lock(&m_optexSocket);
	if(m_sock != INVALID_SOCKET)
	{
		shutdown(m_sock,SD_BOTH);
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}
bool CSocketEx::SendCmdToServer(int cmd, void* pData   , DWORD dwSize  )
{
	int total = sizeof(_packageheader) + dwSize + 5;
	_packageheader *p = (_packageheader*)new char[total];
	memset(p, 0, sizeof(_packageheader));
	p->StartFlag = 0x5E7D;
	p->Cmd = cmd;
	p->Length = total;
	if (pData != NULL && dwSize )
	{
		CPackageHelper outpackage((char*)p);
		outpackage.pushString((char*)pData, dwSize);
	}

	int left = total;

	while (left)
	{
		int len = send(m_sock, (char*) &p[total-left], left, 0);
		if (len <=0)
		{
			CloseConnect();
			delete []p;			
			return false;
		}
		left -= len;
	}
	delete []p;		
	return true;
}
bool CSocketEx::RecvDataFromServer(char* &pData, DWORD& dwlength)
{
	pData = NULL;	dwlength = 0;

	DWORD nSize = 1024 * 100;
	char *p = new char[nSize];
	_packageheader* pheader = (_packageheader*)p;

	bool finish = false;
	int total = 0;
	while (!finish)
	{
		FD_SET fdset;
		timeval tv = {10, 0};
		FD_ZERO(&fdset);
		FD_SET(m_sock, &fdset);
		int ret = select(0, &fdset, NULL, NULL, &tv);
		if (ret <= 0)
		{
			if (ret == 0)
				continue;
			CloseConnect();
			delete []p;
			return false;
		}

		int len = recv(m_sock, &p[total], nSize-total, 0);
		if (len<=0)	
		{
			CloseConnect();
			delete []p;
			return false;
		}
		total += len;
		if (total >= sizeof(_packageheader))
		{
			if (nSize < pheader->Length)
			{				
				char* temp = new char[pheader->Length];
				memcpy(temp, p, total);
				nSize = pheader->Length;
				delete []p;
				p = temp;
				pheader = (_packageheader*)temp;				
			}

			if (total == (int)pheader->Length)
				finish = true;
		}
	}
	pData = p;
	dwlength = total; 
	return true;
}
bool CSocketEx::ExeCommand(int cmd, char* &p, DWORD& length, char* data , DWORD datalength )
{
	CAutoLock<CLock> lock(&m_optexSocket);
	if (!SendCmdToServer(cmd, data, datalength))
	{
		return false;
	}
	if (!RecvDataFromServer(p, length))
	{
		return false;
	}
	return true;

}
UINT CSocketEx:: WorkThread(LPVOID lparam)
{
	CSocketEx* pThis = reinterpret_cast<CSocketEx*>(lparam);
	HANDLE handle[2];
	handle[0] = pThis->m_hExit;
	handle[1] = CreateEvent(NULL,TRUE,FALSE,NULL);

	while (1)
	{
		DWORD dwRet = WaitForMultipleObjects(2,handle,FALSE,2000);
		if(dwRet == WAIT_OBJECT_0)
		{
			break;
		}
		else
		if(dwRet == WAIT_TIMEOUT)
		{
			if(!pThis->IsConnect())
				pThis->ConnectSever(pThis->m_Srvip.c_str());
		}

	}

	CloseHandle(handle[1]);
	return 0;
}
bool CSocketEx::IsConnect()
{
	CAutoLock<CLock> lock (&m_optexSocket);
	return m_sock != INVALID_SOCKET;
}
bool CSocketEx::Start(LPCSTR Srvip)
{
	m_Srvip = std::string(Srvip);
	bool bret = ConnectSever(Srvip);
	if(!bret)
		return false;

	m_hThread = (HANDLE)_beginthreadex(NULL,0,WorkThread,this,0,NULL);
	return  m_hThread != NULL;
}