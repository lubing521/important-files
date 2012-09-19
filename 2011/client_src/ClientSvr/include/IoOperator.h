#pragma once

#include "CommandParam.h"
#include "Hy/hy.h"
I8_NAMESPACE_BEGIN

class CIoOperator
{
public:
	CIoOperator(ICommunication* pCommunication, ILogger* pLog, CHyOperator* pHyOperator);
	virtual ~CIoOperator();

public:
	BOOL                               IsConnected(){ return m_pCommunication->GetTcpSocket() == INVALID_SOCKET?FALSE:TRUE;}
	BOOL                               ExecCommand( CCommandParam *pInCmdParam, BOOL bNeedAck = TRUE, 
		                                            BOOL bAutoLock = TRUE, CCommandParam *pOutCmdParam = NULL);
	BOOL                               FileIsNeedUpdate(DWORD dwProgramType, DWORD dwPathType,
		                                            LPCTSTR lpszSvrPath, LPCTSTR lpszCltPath = NULL);
	BOOL                               DownloadFile( DWORD dwProgramType, DWORD dwPathType, LPCTSTR lpszSvrPath, LPCTSTR lpszCltPath, 
		                                             DWORD &dwRecvLen, char** pRecvBuffer, BOOL bFillBuf = FALSE, BOOL
													 bWrite = FALSE, BOOL bRmvHy = FALSE);
	DWORD                              WriteFile(LPCTSTR lpszFileName, const char* const pInBuffer, DWORD dwBufferLen, BOOL bRmvHy = TRUE);
    DWORD                              ReadFile(LPCTSTR lpszFileName, CCommandParam& cmdParam);
    void                               DeleteDirectory(LPCTSTR pszDir);

public:
	CHyOperator*                       GetHyOperator(){return m_pHyOperator;}

private:
	ICommunication*                    m_pCommunication;
	ILogger*                           m_pLog;
	CHyOperator*                       m_pHyOperator;
};

I8_NAMESPACE_END