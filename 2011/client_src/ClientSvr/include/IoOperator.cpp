//////////////预处理
#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif

#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料


#include <windows.h>
#include <WinInet.h>
#include <ShlObj.h>

#include <Shlwapi.h>
#include <process.h>
#include <time.h>
#include <Userenv.h>
#include <winioctl.h>
#include <tchar.h>
#include <fstream>
#include <wbemidl.h>
#include <Userenv.h>
#include <Psapi.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <dshow.h>
#include <comutil.h>
#pragma comment(lib, "strmiids.lib ")
#pragma comment(lib, "comsuppw.lib")
#pragma warning(disable:4200)
#pragma comment(lib, "version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Wbemuuid") 
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")

#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
////////////////////
#include "cltsvrdef.h"
#include "Tool.h"
#include "Hy/hy.h"
#include "IoOperator.h"
#include "MultiThread/Lock.hpp"
#include "Utility/SmartHandle.hpp"
#include "Utility/utility.h"
#include "Utility/SmartPtr.hpp"
#include "Win32/System/SystemHelper.hpp"

I8_NAMESPACE_BEGIN

CIoOperator::CIoOperator(ICommunication* pCommunication, ILogger* pLog, CHyOperator* pHyOperator)
{
    m_pCommunication = pCommunication;
	m_pLog = pLog;
	m_pHyOperator = pHyOperator;
}

CIoOperator::~CIoOperator()
{

}

BOOL CIoOperator::ExecCommand( CCommandParam *pInCmdParam, BOOL bNeedAck /*= TRUE*/, BOOL bAutoLock /*= TRUE*/, CCommandParam *pOutCmdParam /*= NULL*/ )
{
	char *pRecvBuffer = NULL; int nRecvLen = 0;
	if (bAutoLock) 
		m_pCommunication->Lock();
	if (!m_pCommunication->ExecCmd((char*)pInCmdParam->GetBuffer(), pInCmdParam->GetBufferLength(), pRecvBuffer, nRecvLen, bNeedAck?true:false))
	{
		if (bAutoLock) m_pCommunication->Unlock();
		return FALSE;
	}

	if (!bNeedAck)
	{	
		if (bAutoLock) m_pCommunication->Unlock();			
		return TRUE;
	}

	pOutCmdParam->SetBuffer((BYTE*)pRecvBuffer);
	if (pOutCmdParam->GetPackageHeder()->StartFlag != START_FLAG) 
	{
        if (bAutoLock) 	m_pCommunication->Unlock();	
		m_pLog->WriteLog(LM_INFO, TEXT("收到数据包的标志位错误"));	
		return FALSE;
    }
    if (pOutCmdParam->GetPackageHeder()->Version != PRO_VERSION) 
    {
        if (bAutoLock) 	m_pCommunication->Unlock();	
        m_pLog->WriteLog(LM_INFO, TEXT("服务端通讯协议版本与客户端不匹配"));	
        return FALSE;
    }
	if (*(DWORD*)(pRecvBuffer + sizeof(pkgheader)) == CMD_RET_FAIL)
    {
        if (nRecvLen > sizeof(pkgheader) + sizeof(DWORD))
        {
            DWORD dwReturn;
            pOutCmdParam->Pop(dwReturn);
            TCHAR szError[4096] = {0};
            pOutCmdParam->PopString(szError, 4096);
            m_pLog->WriteLog(LM_INFO, TEXT("命令执行出错:%s"), szError);
        }
		if (bAutoLock) m_pCommunication->Unlock();			
		return FALSE;
	}
	if (bAutoLock) m_pCommunication->Unlock();

	return TRUE; 
}

BOOL CIoOperator::FileIsNeedUpdate(DWORD dwProgramType, DWORD dwPathType, LPCTSTR lpszSvrPath, LPCTSTR lpszCltPath /* = NULL */)
{
	//定义变量
	BOOL	bNeedUpdate = FALSE;
	DWORD   dwRemoteFileSize = 0, dwLocalFileSize = 0, dwRemoteFileCRC = 0, dwLocalFileCRC = 0;
	HANDLE  hLocalFile  = INVALID_HANDLE_VALUE;

	hLocalFile = ::CreateFile(lpszCltPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLocalFile != INVALID_HANDLE_VALUE)
	{
		//获取文件大小
		dwLocalFileSize = GetFileSize(hLocalFile, NULL);
		CloseHandle(hLocalFile);
		dwLocalFileCRC = GetFileCRC(lpszCltPath);
	}
	else return TRUE;

	//构建获取文件索引命令
	CCommandParam  cmdParamIn, cmdParamOut;
	cmdParamIn.InitPackageHeader(CMD_CLIUPT_DOWNFILEINFO);
	cmdParamIn<<dwProgramType<<dwPathType<<lpszSvrPath;

	//执行命令
	if (!ExecCommand(&cmdParamIn, TRUE, TRUE, &cmdParamOut))
	{
		return TRUE;
	}

	DWORD dwRetCode = 0;
	cmdParamOut>>dwRetCode;
	if ( dwRetCode == CMD_RET_SUCCESS )
	{
		cmdParamOut>>dwRemoteFileSize>>dwRemoteFileCRC;
		if (dwRemoteFileSize != dwLocalFileSize 
			|| dwRemoteFileCRC != dwLocalFileCRC)
		{
			bNeedUpdate = TRUE;
		}
	}
	return bNeedUpdate;
}

BOOL CIoOperator::DownloadFile(DWORD dwProgramType, DWORD dwPathType, LPCTSTR lpszSvrPath, LPCTSTR lpszCltPath, 
							   DWORD &dwRecvLen, char** pRecvBuffer, BOOL bFillBuf, BOOL bWrite , BOOL bRmvHy)
{
	DWORD dwRecvIndex = 0, dwFileSize = 0, dwLastFlag = 0, dwCurrentSize = 0;
	CCommandParam cmdParamIn, cmdParamOut;

	BOOL bIsNeedUpdate = FileIsNeedUpdate(dwProgramType, dwPathType, lpszSvrPath, lpszCltPath);
	if (!bIsNeedUpdate)
		return TRUE;

    async::thread::AutoLock<ICommunication> guard(*m_pCommunication);
	cmdParamIn.InitPackageHeader(CMD_CLIUPT_DOWNFILE_START);
	cmdParamIn<<dwProgramType<<dwPathType<<lpszSvrPath;
	if (!ExecCommand(&cmdParamIn, TRUE, FALSE, &cmdParamOut))
		return FALSE;
	DWORD dwValue;
	cmdParamOut>>dwValue;	
	if(dwValue != CMD_RET_SUCCESS)
		return FALSE;

	cmdParamOut>>dwFileSize;
	*pRecvBuffer = new char[dwFileSize + 1];
	while (dwRecvIndex < dwFileSize)
	{
		cmdParamIn.SetLength();
		cmdParamOut.SetLength();
		cmdParamIn.InitPackageHeader(CMD_DOWNFILE_NEXT);
		if (!ExecCommand(&cmdParamIn, TRUE, FALSE, &cmdParamOut))
		{
			delete [](*pRecvBuffer);
			return FALSE;
		}
		DWORD dwValue;
		cmdParamOut>>dwValue;
		if (dwValue != CMD_RET_SUCCESS)
		{
			delete [](*pRecvBuffer);
			return FALSE;
		}
		cmdParamOut>>dwLastFlag>>dwCurrentSize;
		cmdParamOut.PopPointer(*pRecvBuffer + dwRecvIndex, dwCurrentSize);
		dwRecvIndex   += dwCurrentSize;
		//最后一包，结束。
		if (dwLastFlag == 1) break;
		Sleep(50);
	}
	if (dwRecvIndex != dwFileSize)
	{
		delete [](*pRecvBuffer);
		return FALSE;
	}
	dwRecvLen = dwFileSize;

	if (bWrite && WriteFile(lpszCltPath, *pRecvBuffer, dwFileSize, bRmvHy) != dwFileSize)
	{
		delete [](*pRecvBuffer);
		return FALSE;
	}

	if (!bFillBuf)
		delete [](*pRecvBuffer);

	return TRUE;
}

DWORD CIoOperator::WriteFile(LPCTSTR lpszFileName, const char* const pInBuffer, DWORD dwBufferLen, BOOL bRmvHy)
{
	utility::MakeSureDirectoryPathExists(lpszFileName);
	utility::CAutoFile clFile = ::CreateFile(lpszFileName, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!clFile.IsValid()) 
	{
		DWORD dwError = GetLastError();
		_com_error Error(dwError);
		return 0;
	}

	DWORD dwByteWrite = 0;
	if (!::WriteFile(clFile, pInBuffer, dwBufferLen, &dwByteWrite, NULL) || dwByteWrite != dwBufferLen)
	{
		SetEndOfFile(clFile);
		return 0;
	}

	SetEndOfFile(clFile);

	if (bRmvHy)
	{
		m_pHyOperator->ProtectFile(lpszFileName);	
	}
	return dwByteWrite;
}

DWORD CIoOperator::ReadFile(LPCTSTR lpszFileName, CCommandParam &cmdParam)
{
	utility::CAutoFile clFile = ::CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!clFile.IsValid()) return 0;

	DWORD dwFileSize = GetFileSize(clFile, NULL), dwReadBytes = 0;
    utility::CAutoArrayPtr<char>::AutoPtr pBuffer(new char[dwFileSize + 1]);
	if (!::ReadFile(clFile, pBuffer, dwFileSize, &dwReadBytes, NULL) || dwReadBytes != dwFileSize/* || dwFileSize < 30*/)
	{
		return 0;
	}		
	cmdParam.SetBuffer(reinterpret_cast<BYTE*>(pBuffer.Detach()), RELEASE_MODE_DELETE);

	return dwReadBytes;
}

void CIoOperator::DeleteDirectory(LPCTSTR pszDir)
{
    assert(pszDir != NULL && lstrlen(pszDir) != 0);
    TCHAR tDriver = pszDir[0];

    __int64 iDelSize = 0;
    SDG_DeleteDirectory(pszDir, &iDelSize);

    if (m_pHyOperator->IsInstallHy())
    {
        m_pHyOperator->DeleteDirectory(pszDir);
    }
}






























I8_NAMESPACE_END