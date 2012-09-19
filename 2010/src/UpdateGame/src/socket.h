#ifndef __socket_inc__
#define __socket_inc__

#include "public.h"
#include <comutil.h>
#include <string>
#include <vector>
#include <algorithm>

inline void SplitIpString(std::string strSource, std::vector<DWORD>& ips, char ch = '|')
{
	ips.clear();
	size_t first = 0;
	size_t second = 0;
	do
	{
		second = strSource.find_first_of(ch, first);
		if (second != std::string::npos)
		{
			std::string ip = strSource.substr(first, second-first);
			DWORD dwIp = inet_addr(ip.c_str());
			if (dwIp)
				ips.push_back(dwIp);
			first = second + 1;
		}
		else
		{
			if (first< strSource.size())
			{
				DWORD dwIp = inet_addr(strSource.substr(first).c_str());
				if (dwIp)
					ips.push_back(dwIp);
			}
		}
	}while (second != std::string::npos);
}

inline SOCKET ConnectServerTimeout(LPCSTR svrip, LPCSTR cliip, WORD port, int timeout = 5)
{
	struct tagPeer
	{
		SOCKET		sck;
		sockaddr_in addr;
	};

	SOCKET result = INVALID_SOCKET;
	timeval tv = {timeout, 0};
	fd_set  fd;
	FD_ZERO(&fd);

	std::vector<DWORD>	 ips;
	std::vector<tagPeer> peers;	
	SplitIpString(svrip, ips);
	for (size_t idx = 0; idx < ips.size(); idx++)
	{
		tagPeer Peer = {0};
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port   = 0;
		addr.sin_addr.s_addr = 0;
		if (cliip != NULL && inet_addr(cliip) != INADDR_NONE)
			addr.sin_addr.s_addr = inet_addr(cliip);

		DWORD flag = 1;
		SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);
		if (sck == INVALID_SOCKET || 
			bind(sck, (PSOCKADDR)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR ||
			ioctlsocket(sck, FIONBIO, &flag) == SOCKET_ERROR)
		{
			if (sck != INVALID_SOCKET)
				closesocket(sck);
			continue;
		}

		Peer.sck = sck;
		Peer.addr.sin_family = AF_INET;
		Peer.addr.sin_port   = htons(port);
		Peer.addr.sin_addr.s_addr = ips[idx];
		peers.push_back(Peer);
	}

	for (size_t idx = 0; idx < peers.size(); idx++)
	{
		SOCKET sck = peers[idx].sck;
		FD_SET(sck, &fd);
		connect(sck, (PSOCKADDR)&peers[idx].addr, sizeof(sockaddr_in));
	}
	if (select(0, NULL, &fd, NULL, &tv) <= 0)
		goto do_exit;

	for (size_t idx = 0; idx < peers.size(); idx++)
	{
		SOCKET sck = peers[idx].sck;
		if (FD_ISSET(sck, &fd))
		{
			unsigned long flag = 0;
			if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &flag))
				continue ;
			{
				char szLog[MAX_PATH] = {0};
				in_addr addr = peers[idx].addr.sin_addr;
				sprintf(szLog, "i8desk: ips:%s, connected to %s.", svrip, inet_ntoa(addr));
				OutputDebugStringA(szLog);
			}
			BOOL flg = 1;
			int RecvBuf = 100 * 1024;
			setsockopt(sck, SOL_SOCKET, TCP_NODELAY,	(char*)&flg,	 sizeof(BOOL));
			setsockopt(sck, SOL_SOCKET, SO_RCVBUF,		(char*)&RecvBuf, sizeof(int) );
			setsockopt(sck, SOL_SOCKET, SO_SNDBUF,		(char*)&RecvBuf, sizeof(int) );
			result = sck;
			break;
		}
	}
do_exit:
	for (size_t idx = 0; idx < peers.size(); idx++)
	{
		SOCKET sck = peers[idx].sck;
		if (sck != INVALID_SOCKET && result != sck)
		{
			closesocket(sck);
		}
	}
	
	return result;
}

class CSocket
{
private:
	CLock  m_lock;
	SOCKET m_sock;

	std::string _sip;
	std::string _cip;
	WORD   _port;
	int	   _timeout;
	DWORD  _gid;
	std::wstring _dir;
public:
	CSocket() : m_sock(INVALID_SOCKET)
	{
		WSAData data = {0};
		WSAStartup(0x0202, &data);
	}
	~CSocket()
	{
		CloseServer();
		WSACleanup();
	}
public:
	int ConnectServer(LPCSTR sip, LPCSTR cip = NULL, WORD port = PORT_TCP_UPDATE, int timeout = 3)
	{
		CAutoLock<CLock> lock(&m_lock);
		m_sock = ConnectServerTimeout(sip, cip, port, timeout);
		if (m_sock == INVALID_SOCKET)
			return WSAETIMEDOUT;

		sockaddr_in addr = {0};
		int len = sizeof(addr);
		getpeername(m_sock, (PSOCKADDR)&addr, &len);
		char* p = inet_ntoa(addr.sin_addr);

		_sip = p   ? p   : "";
		_cip = cip ? cip : "";
		_port = port;
		_timeout = timeout;
		return ERROR_SUCCESS;
	}

	bool IsConnected()
	{
		CAutoLock<CLock> lock(&m_lock);
		return m_sock != INVALID_SOCKET;
	}

	void CloseServer()
	{
		CAutoLock<CLock> lock(&m_lock);
		if (m_sock != INVALID_SOCKET)
		{
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
	}

	bool GetGameInfo(DWORD gid, LPCWSTR dir, DWORD& dwVersion, DWORD& dwIdxSize, FILETIME& ft)
	{
		_gid = gid;
		_dir = dir;
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);
		InitPkgheader(pheader, CMD_UPT_GETGMAEINFO);
		sendpackage.pushDWORD(gid);
		sendpackage.pushString((LPCSTR)dir, wcslen(dir)*2);
		sendpackage.pushByte(0);
		pheader->Length = sendpackage.GetOffset();
		if (!ExeCmd(buf, pheader->Length, buf, 24) || *(DWORD*)buf != ERROR_SUCCESS)
			return false;
		DWORD dwIdxSizeHigh = 0;
		dwVersion			= *(DWORD*)(&buf[ 4]);
		dwIdxSize			= *(DWORD*)(&buf[ 8]);
		dwIdxSizeHigh		= *(DWORD*)(&buf[12]);
		ft.dwLowDateTime	= *(DWORD*)(&buf[16]);
		ft.dwHighDateTime	= *(DWORD*)(&buf[20]);
		if (dwVersion == -1 || dwIdxSize == 0)
			return false;
		return true;
	}

	bool RecvIdxfile(LPCWSTR dir, char*& pIdxfile, int filesize)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		pIdxfile = new char[filesize + 100];
		int left = filesize;
		char* p = pIdxfile;
		while (left)
		{
			DWORD blksize = left >= 0xA0000 ? 0xA0000 : left;
			if (!RecvBlock(L"i8desk.idx", filesize, (__int64)(filesize-left), blksize, p == pIdxfile, p))
				return false;
			p    += blksize;
			left -= blksize;
		}
		pIdxfile[filesize] = pIdxfile[filesize+1] = 0;
		return true;
	}

	bool RecvBlock(LPCWSTR file, __int64 filesize, __int64 offset, int blksize, bool first, char* pfile, 
		HANDLE hExited = NULL)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);

		InitPkgheader(pheader, CMD_UPT_RECVBLOCK);
		sendpackage.pushDWORD((DWORD)filesize);
		sendpackage.pushDWORD((DWORD)(filesize >> 32));
		sendpackage.pushDWORD((DWORD)offset);
		sendpackage.pushDWORD((DWORD)(offset >> 32));
		sendpackage.pushDWORD(blksize);
		sendpackage.pushDWORD(first ? 1: 0);
		sendpackage.pushString((LPCSTR)file, wcslen(file)*2);
		sendpackage.pushByte(0);
		pheader->Length = sendpackage.GetOffset();

		if (!ExeCmd(buf, pheader->Length, pfile, blksize))
		{
			//retry 3 times.
			for (int idx=0; idx<3; idx++)
			{
				m_sock = ConnectServerTimeout(_sip.c_str(), _cip.c_str(), _port, _timeout);
				if (hExited != NULL && WaitForSingleObject(hExited, 0) == WAIT_OBJECT_0)
					return false;

				DWORD dwVersion;
				DWORD dwIdxSize;
				FILETIME ft = {0};
				*(DWORD*)(&buf[36]) = 1; //first = true.
				if (m_sock != INVALID_SOCKET && 
					GetGameInfo(_gid, _dir.c_str(), dwVersion, dwIdxSize, ft) && 
					ExeCmd(buf, pheader->Length, pfile, blksize))
				{
					OutputDebugStringA("i8desk: retry connect to server.");
					return true;
				}
			}
			
			return false;
		}
		return true;
	}

	bool ReportGameStatus(DWORD gid, DWORD speed, DWORD progress, __int64 BytesUpdated, __int64 BytesLeft)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf; 
		CPackageHelper sendpackage(buf);

		InitPkgheader(pheader, CMD_UPT_RPTPROGRESS);
		sendpackage.pushDWORD(0);	//client ip fill 0.
		sendpackage.pushDWORD(gid);
		sendpackage.pushDWORD(speed);
		sendpackage.pushDWORD(progress);
		sendpackage.pushDWORD((DWORD)BytesUpdated);
		sendpackage.pushDWORD((DWORD)(BytesUpdated >> 32));
		sendpackage.pushDWORD((DWORD)BytesLeft);
		sendpackage.pushDWORD((DWORD)(BytesLeft >> 32));
		pheader->Length = sendpackage.GetOffset();

		if (!ExeCmd(buf, pheader->Length, buf, 4) || *(DWORD*)buf != ERROR_SUCCESS)
			return false;
		return true;
	}

	bool ReportUpdateFinished(DWORD gid, DWORD ver, LPCWSTR dir)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);

		InitPkgheader(pheader, CMD_UPT_RPTFINISH);
		sendpackage.pushDWORD(0);	//ip fill 0.
		sendpackage.pushDWORD(gid);
		sendpackage.pushDWORD(ver);
		sendpackage.pushString((LPCSTR)dir, wcslen(dir)*2);
		sendpackage.pushByte(0);
		pheader->Length = sendpackage.GetOffset();

		if (!ExeCmd(buf, pheader->Length, buf, 4) || *(DWORD*)buf != ERROR_SUCCESS)
			return false;
		return true;
	}

	bool ReportGameIdxError(DWORD gid)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf; 
		CPackageHelper sendpackage(buf);

		InitPkgheader(pheader, CMD_UPT_INDEXERR);
		sendpackage.pushDWORD(gid);
		pheader->Length = sendpackage.GetOffset();

		if (!ExeCmd(buf, pheader->Length, buf, 4) || *(DWORD*)buf != ERROR_SUCCESS)
			return false;
		return true;
	}

	bool GetAllSyncTask(char*& pData, int& length)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);
		InitPkgheader(pheader, CMD_SYNC_GETALLTASK);
		
		return ExeCmd2(buf, pheader->Length, pData, length);
	}

	bool GetSyncTaskInfo(char* pSid, DWORD dwSyncType, char*& pData, int& length)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);
		InitPkgheader(pheader, CMD_SYNC_GETTASKGAME);
		sendpackage.pushString(pSid, lstrlenA(pSid));
		sendpackage.pushDWORD(dwSyncType);
		pheader->Length = sendpackage.GetOffset();

		return ExeCmd2(buf, pheader->Length, pData, length);
	}

	bool SyncSetGameVersion(DWORD gid, DWORD ver)
	{
		char buf[1024] = {0};
		_packageheader* pheader = (_packageheader*)buf;
		CPackageHelper sendpackage(buf);
		InitPkgheader(pheader, CMD_SYNC_SETGMAEVER);
		sendpackage.pushDWORD(gid);
		sendpackage.pushDWORD(ver);
		pheader->Length = sendpackage.GetOffset();

		return ExeCmd(buf, pheader->Length, buf, sizeof(_packageheader) + 4);
	}

	void ConvertDataToMainServer(char*p, int len)
	{
		char data[4] = {0};
		ExeCmd(p, len, data, sizeof(data));
	}
private:
	bool  SendBytes(char* pdata, int length)
	{
		int left = length;
		while (left)
		{
			int len = send(m_sock, &pdata[length-left], left, 0);
			if (len <=0)
			{
				CloseServer();
				return false;
			}
			left -= len;
		}
		return true;
	}

	inline bool recvpackage(SOCKET sck, char* buf, int size)
	{
		int left = size;
		while (left)
		{
			FD_SET fdset;
			timeval tv = {10, 0};
			FD_ZERO(&fdset);
			FD_SET(sck, &fdset);

			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret <= 0)
				return false;

			int len = recv(sck, buf + size - left, left, 0);
			if (len <= 0)
				return false;

			left -= len;
		}
		return true;
	}

	inline bool recvpackage2(SOCKET sck, char*& pdata, int& len)
	{
		pdata = NULL;	len = 0;
		DWORD nSize = 1024 * 64;
		char *p = new char[nSize];
		_packageheader* pheader = (_packageheader*)p;
		int nRecv = 0;
		while (1)
		{
			FD_SET fdset;
			timeval tv = {10, 0};
			FD_ZERO(&fdset);
			FD_SET(sck, &fdset);
			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret <= 0)
			{
				delete []p;
				return false;
			}
			int len = recv(sck, &p[nRecv], nSize-nRecv, 0);
			if (len<=0)	
			{
				delete []p;
				return false;
			}
			nRecv += len;
			if (nRecv >= sizeof(_packageheader))
			{
				if (nSize < pheader->Length)
				{				
					char* temp = new char[pheader->Length];
					memcpy(temp, p, nRecv);
					nSize = pheader->Length;
					delete []p;
					p = temp;
					pheader = (_packageheader*)temp;				
				}
				if (nRecv >= sizeof(_packageheader) + 4)
				{
					if (*(DWORD*)pheader->data != ERROR_SUCCESS)
					{
						delete[]p;
						return false;
					}
				}
				if (nRecv == (int)pheader->Length)
					break;
			}
		}
		pdata = p;
		len = nRecv;
		return true;
	}

	bool ExeCmd(char* pOutData, int nOutLen, char* pInData, int length)
	{
		CAutoLock<CLock> lock(&m_lock);
		if (!SendBytes(pOutData, nOutLen) || !recvpackage(m_sock, pInData, length))
		{
			CloseServer();
			return false;
		}
		return true;
	}

	bool ExeCmd2(char* pOutData, int nOutLen, char*& pInData, int& length)
	{
		CAutoLock<CLock> lock(&m_lock);
		if (!SendBytes(pOutData, nOutLen) || !recvpackage2(m_sock, pInData, length))
		{
			CloseServer();
			return false;
		}
		return true;
	}
};

#endif