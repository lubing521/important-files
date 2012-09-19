#include "StdAfx.h"
#include "SocketEx.h"

CSocketEx::CSocketEx(void):m_sock(INVALID_SOCKET)
{
	WSADATA data;
	WSAStartup(0x0202,&data);
	CreateSocket();
}

CSocketEx::~CSocketEx(void)
{

if(m_sock != INVALID_SOCKET)
{
	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
}
	WSACleanup();
}
void CSocketEx::Release()
{
	delete this;
}
bool CSocketEx::CreateSocket()
{

	if(m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock= INVALID_SOCKET;
	}
	 m_sock = socket(AF_INET,SOCK_DGRAM,0);
	 if(m_sock == INVALID_SOCKET)
	 {
		 AfxMessageBox("socket create fail");
		 return false;
	 }
	 SOCKADDR_IN Addr = {0};
	 Addr.sin_family = AF_INET;
	 Addr.sin_port = htons(5970);
	 Addr.sin_addr.s_addr =INADDR_ANY;
	 if(SOCKET_ERROR  == bind(m_sock,(PSOCKADDR)&Addr,sizeof(Addr)))
	 {
		 closesocket(m_sock);
		 m_sock= INVALID_SOCKET;
		
		 return false;
	 }
	

	 return true;
}
void CSocketEx::Close()
{
	if(m_sock != INVALID_SOCKET)
	{
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
	SOCKADDR_IN Addr = {0}; 
	int AddrLen = sizeof(SOCKADDR_IN);

	Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	Addr.sin_port = htons(5969);
	Addr.sin_family = AF_INET;

	while (left)
	{
		int len = sendto(m_sock, (char*) &p[total-left], left, 0,(PSOCKADDR)&Addr,AddrLen);
		if (len <=0)
		{
		
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
		DWORD nSize = 1024 * 64;
		char *p = new char[nSize];
		_packageheader* pheader = (_packageheader*)p; 
		SOCKADDR_IN SerAddr = {0}; 
		int AddrLen = sizeof(SOCKADDR_IN);
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

				delete []p;
				return false;
			}

			int len = recvfrom(m_sock, &p[total], nSize-total,0, (PSOCKADDR)&SerAddr,&AddrLen);
			if (len<=0)	
			{
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

