#pragma once


#include "string"


using namespace i8desk;


class CSocketEx
{
private: 
	SOCKET m_sock;
	CLock m_optexSocket;
	HANDLE m_hExit,m_hThread;
	std::string m_Srvip;

public:
	SOCKET GetSocket(){return m_sock;}
	bool ConnectSever(LPCSTR Srvip,DWORD dwTimeOut = 3000);
	void CloseConnect();
	bool SendCmdToServer(int cmd, void* pData  = NULL , DWORD dwSize  = 0 );
	bool RecvDataFromServer(char* &pData, DWORD& dwlength);
	bool ExeCommand(int cmd, char* &p, DWORD& length, char* data = NULL, DWORD datalength = 0);
	CSocketEx(void);
	~CSocketEx(void);
	void Release();
	bool Start(LPCSTR Srvip);
	static UINT _stdcall WorkThread(LPVOID lparam);
	bool IsConnect();
};
