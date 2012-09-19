#pragma once

#include "NetLayer.h"
#include "string"
using namespace i8desk;


class CLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	CLock()			{ InitializeCriticalSection(&m_lock);	}
	~CLock()		{ DeleteCriticalSection(&m_lock);		}
	void Lock()		{ EnterCriticalSection(&m_lock);		}
	void UnLock()	{ LeaveCriticalSection(&m_lock);		}
};

//同步guard工具类  ex:CLock m_lock; CAutoLock<CLock> lock(m_lock);
template <class T>
struct CAutoLock		
{
private:
	T* m_pT;
public:
	CAutoLock(T* pt): m_pT(pt) 
	{
		if (m_pT != NULL)
			m_pT->Lock();
	}
	~CAutoLock()
	{
		if (m_pT != NULL)
			m_pT->UnLock();
	}
};

class CSocketEx
{
private: 
	SOCKET m_sock;
	
public:
	SOCKET GetSocket(){return m_sock;}
	bool CreateSocket();
	void Close();
	bool SendCmdToServer(int cmd, void* pData  = NULL , DWORD dwSize  = 0 );
	bool RecvDataFromServer(char* &pData, DWORD& dwlength);
	bool ExeCommand(int cmd, char* &p, DWORD& length, char* data = NULL, DWORD datalength = 0);
	CSocketEx(void);
	~CSocketEx(void);
	void Release();

};
