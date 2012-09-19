#include "stdafx.h"
#include "forceupdate.h"
#include "UpdateGame.h"
#include <process.h>
#include <Shlwapi.h>

bool CForceUpdatePlug::Initialize(ICliPlugMgr* pPlugMgr)
{
	if (m_hExited == NULL || m_hMenuUpdate == NULL)
		return false;
	
	m_pPipe = new PCMomule<UpGameinfo, _func_<UpGameinfo>, MAX_TASK_NUMS>(m_hExited);
	if (m_pPipe == NULL)
		return false;

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThreadProc, this, 0, NULL);
	if (m_hThread == NULL)
		return false;

	WriteLog("Initialize plug success.");
	return true;
}

bool CForceUpdatePlug::UnInitialize()
{
	WriteLog("UnInitialize plug.");
	if (m_hThread != NULL && m_hExited != NULL && m_hMenuUpdate != NULL)
	{
		SetEvent(m_hExited);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	Delete_Pointer(m_pPipe);
	Release_Interface(m_pILogger);
	return true;
}

DWORD CForceUpdatePlug::SendMessage(DWORD cmd, DWORD param1, DWORD param2)
{
	WriteLog("send message:%x, %x, %u", cmd, param1, param2);
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

	while (WAIT_TIMEOUT == WaitForSingleObject(pThis->m_hExited, 1000))
	{
		if (WaitForSingleObject(pThis->m_hMenuUpdate, 0) != WAIT_OBJECT_0)
		{
			pThis->SyncGame();
		}
	}

	return 0;
}

void CForceUpdatePlug::SyncGame()
{
	char szIp[MAX_PATH] = {0};
	{
		char szIniFile[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, szIniFile, _countof(szIniFile));
		PathRemoveFileSpecA(szIniFile);
		PathAddBackslashA(szIniFile);
		lstrcatA(szIniFile, "GameClient.ini");
		GetPrivateProfileStringA("systemset", "ServerAddr", "127.0.0.1", szIp, _countof(szIp), szIniFile);
	}
	tagUpGameinfo   GameInfo = {0};
	if (m_pPipe->Pop(GameInfo))
	{
		WriteLog("start update:%d, %s, %dk/s", GameInfo.dwGid, GameInfo.CliPath, GameInfo.dwSpeed);
		DWORD flag = UPDATE_FLAG_DELETE_MOREFILE | UPDATE_FLAG_DELETE_GAME;
		if (GameInfo.dwGid == 0)
			flag = 0;
		if (GameInfo.dwFlag == FC_COPY_FORE)
			flag |= UPDATE_FLAG_FORCE_UPDATE;
		if (GameInfo.dwFlag == FC_COPY_QUICK)
			flag |= UPDATE_FLAG_QUICK_COMPARE;

		HANDLE hUpdate = UG_StartUpdate(GameInfo.dwGid, _bstr_t(GameInfo.SvrPath), _bstr_t(GameInfo.CliPath), 
			flag, _bstr_t(szIp), NULL, GameInfo.dwSpeed);
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
				WriteLog("update error: %d, %s, get update status fail.", GameInfo.dwGid, GameInfo.CliPath);
				break;
			}

			if (Status.Status == UPDATE_STATUS_FINISH)
			{
				WriteLog("update finish:%d, %s, %0.2fMBytes.", GameInfo.dwGid, GameInfo.CliPath, 
					Status.m_qNeedsUpdateBytes / 1000.0 / 1000);
				break;
			}
			else if (Status.Status == UPDATE_STATUS_ERROR)
			{
				WriteLog("update error:%d, %s, %s", GameInfo.dwGid, GameInfo.CliPath, (LPCSTR)_bstr_t(Status.Info));
				break;
			}
		}
		UG_StopUpdate(hUpdate);
	}
}

void __cdecl CForceUpdatePlug::WriteLog(char* formater, ...)
{
	CAutoLock<CLock> lock(&m_lockWriteLog);
	if (m_pILogger == NULL)
	{
		typedef BOOL (WINAPI *PFNCREATELOGGER)(ILogger** ppILogger);
		char szFile[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, szFile, _countof(szFile));
		PathRemoveFileSpecA(szFile);
		PathAddBackslashA(szFile);
		lstrcatA(szFile, "DbEngine.dll");
		HMODULE hMod = LoadLibraryA(szFile);
		PFNCREATELOGGER pfnCreateLogger = NULL;
		if (hMod != NULL && (pfnCreateLogger = (PFNCREATELOGGER)GetProcAddress(hMod, MAKEINTRESOURCEA(3))))
		{
			pfnCreateLogger(&m_pILogger);
			if (m_pILogger != NULL)
			{
				m_pILogger->SetLogFileName(TEXT("forceupdate"));
				m_pILogger->SetAddDateBefore(false);
				m_pILogger->WriteLog(LM_INFO, "===============================================");
				m_pILogger->SetAddDateBefore(true);
			}
		}
	}
	if (m_pILogger != NULL)
	{
		char szLog[4096] = {0};
		va_list marker;
		va_start(marker, formater);
		vsprintf(szLog, formater, marker);
		va_end(marker);
		m_pILogger->WriteLog(LM_INFO, szLog);
	}
}