#include "stdafx.h"
#include "forceupdate.h"
#include "UpdateGame.h"
#include "../../../include/MultiThread/Lock.hpp"

#include <process.h>
#include <Shlwapi.h>

bool CForceUpdatePlug::Initialize(ICliPlugMgr* pPlugMgr)
{
    WriteLog(TEXT("初始化内网更新插件."));
	if (m_hExited == NULL || m_hMenuUpdate == NULL)
		return false;
	
	m_pPipe = new PCMomule<UpGameinfo, _func_<UpGameinfo>, MAX_TASK_NUMS>(m_hExited);
	if (m_pPipe == NULL)
		return false;

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, this, 0, NULL);
	if (m_hThread == NULL)
		return false;

	return true;
}

bool CForceUpdatePlug::UnInitialize()
{
	if (m_hThread != NULL && m_hExited != NULL && m_hMenuUpdate != NULL)
	{
		SetEvent(m_hExited);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	Delete_Pointer(m_pPipe);
    Release_Interface(m_pILogger);
    WriteLog(TEXT("释放内网更新插件."));
	return true;
}

DWORD CForceUpdatePlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	WriteLog(TEXT("send message:%x, %x, %u"), cmd, param1, param2);
	if (cmd == 0)
	{
		DWORD dwCount   = param2;
		tagUpGameinfo* pGaemInfos = (tagUpGameinfo*)param1;
		if (pGaemInfos != NULL && dwCount != 0)
		{
			for(DWORD idx = 0; idx < dwCount; idx++)
			{
				tagUpGameinfo* pGame = (tagUpGameinfo*)(pGaemInfos + idx);
				if (pGame != NULL && m_pPipe != NULL)
				{
					m_pPipe->Push(*pGame);
				}
			}
		}
	}
	return ERROR_SUCCESS;
}

UINT32 __stdcall CForceUpdatePlug::WorkThreadProc(LPVOID lpVoid)
{
	CForceUpdatePlug* pThis = reinterpret_cast<CForceUpdatePlug*>(lpVoid);

    pThis->WriteLog(TEXT("内网更新插件主线程启动"));
	while (WAIT_TIMEOUT == WaitForSingleObject(pThis->m_hExited, 1000))
	{
		if (WaitForSingleObject(pThis->m_hMenuUpdate, 0) != WAIT_OBJECT_0)
		{
			pThis->SyncGame();
		}
    }
    pThis->WriteLog(TEXT("内网更新插件主线程退出"));

	return 0;
}

void CForceUpdatePlug::SyncGame()
{
	TCHAR szIp[MAX_PATH] = {0};
	{
		TCHAR szIniFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szIniFile, _countof(szIniFile));
		PathRemoveFileSpec(szIniFile);
		PathAddBackslash(szIniFile);
		lstrcat(szIniFile, TEXT("GameClient.ini"));
        GetPrivateProfileString(TEXT("systemset"), TEXT("ServerAddr"), TEXT("127.0.0.1"), szIp, _countof(szIp), szIniFile);
        WriteLog(TEXT("取得主服务地址%s"), szIp);
	}
	tagUpGameinfo   GameInfo = {0};
	if (m_pPipe->Pop(GameInfo))
	{
		WriteLog(TEXT("开始更新游戏:%d, %s, %dk/s"), GameInfo.dwGid, GameInfo.CliPath, GameInfo.dwSpeed);
		DWORD flag = UPDATE_FLAG_DELETE_MOREFILE | UPDATE_FLAG_DELETE_GAME;
		if (GameInfo.dwGid == 0)
			flag = 0;
		if (GameInfo.dwFlag == UPDATE_FLAG_FORCE_UPDATE)
			flag |= UPDATE_FLAG_FORCE_UPDATE;
		if (GameInfo.dwFlag == UPDATE_FLAG_QUICK_COMPARE)
			flag |= UPDATE_FLAG_QUICK_COMPARE;

		
		HANDLE hUpdate = UG_StartUpdate(GameInfo.dwGid, GameInfo.Tid, _bstr_t(GameInfo.SvrPath), _bstr_t(GameInfo.CliPath), 
			flag, _bstr_t(szIp), NULL, GameInfo.dwSpeed);//ligc add NULL
		HANDLE hEvents[] = {m_hExited, m_hMenuUpdate};
		while (1)
		{
			DWORD dwRet = WaitForMultipleObjects(_countof(hEvents), hEvents, FALSE, 1000);
			if (dwRet != WAIT_TIMEOUT)
			{
				if (dwRet == WAIT_OBJECT_0 + 1)
					m_pPipe->Push(GameInfo);
				break;
			}

			tagUpdateStatus Status = {sizeof(tagUpdateStatus)};
			if (!UG_GetUpdateStatus(hUpdate, &Status))
			{
				WriteLog(TEXT("更新失败: %d, %s, 取得更新状态失败"), GameInfo.dwGid, GameInfo.CliPath);
				break;
			}

			if (Status.dwStatus == UPDATE_STATUS_FINISH)
			{
				WriteLog(TEXT("更新完成:%d, %s, %0.2fMBytes."), GameInfo.dwGid, GameInfo.CliPath, 
					Status.qNeedsUpdateBytes / 1000.0 / 1000);
				break;
			}
			else if (Status.dwStatus == UPDATE_STATUS_ERROR)
			{
				WriteLog(TEXT("更新失败:%d, %s, %s"), GameInfo.dwGid, GameInfo.CliPath, (LPCTSTR)_bstr_t(Status.awInfo));
				break;
			}
		}
		UG_StopUpdate(hUpdate);
	}
}

void __cdecl CForceUpdatePlug::WriteLog(LPCTSTR formater, ...)
{
	AutoLock lock(m_lockWriteLog);
	if (m_pILogger == NULL)
	{
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger** ppILogger);
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, _countof(szFile));
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("Frame.dll"));
		HMODULE hMod = LoadLibrary(szFile);
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)GetProcAddress(hMod, MAKEINTRESOURCEA(5))))
		{
			pfnCreateLogger(&m_pILogger);
			if (m_pILogger != NULL)
			{
				m_pILogger->SetLogFileName(TEXT("forceupdate"));
				m_pILogger->SetAddDateBefore(false);
				m_pILogger->WriteLog(LM_INFO, TEXT("==============================================="));
				m_pILogger->SetAddDateBefore(true);
			}
		}
	}
	if (m_pILogger != NULL)
	{
		TCHAR szLog[4096] = {0};
		va_list marker;
		va_start(marker, formater);
		_vstprintf_s(szLog, formater, marker);
		va_end(marker);
		m_pILogger->WriteLog(LM_INFO, szLog);
	}
}