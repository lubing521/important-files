#ifndef __hooklocker_inc_
#define __hooklocker_inc_

#pragma warning(disable:4996)
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


inline void lock_Initialize(HMODULE hMod, DWORD ResId)
{
	TCHAR szDll[MAX_PATH] = {0};
	GetSystemDirectory(szDll, sizeof(szDll) - 1);
	PathAddBackslash(szDll);
	lstrcat(szDll, DLL_NAME);

	//output dll file.
	{
		HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(ResId), TEXT("DLL"));
		if (hRes == NULL)
			return ;
		DWORD dwSize  = SizeofResource(hMod, hRes);
		HGLOBAL hGlobal = LoadResource(hMod, hRes);
		if (hGlobal == NULL)
		{
			DWORD dwErr = GetLastError();
			return ;
		}

		LPVOID lpData = GlobalLock(hGlobal);
		if (lpData == NULL)
		{
			DWORD dwErr = GetLastError();
			FreeResource(hRes);
			return ;
		}
		HANDLE hFile = CreateFile(szDll, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			DWORD dwErr = GetLastError();
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return ;
		}
		DWORD dwWriteBytes = 0;
		if (!WriteFile(hFile, lpData, dwSize, &dwWriteBytes, NULL))
		{
			DWORD dwErr = GetLastError();
			CloseHandle(hFile);
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
			return ;
		}

		GlobalUnlock(hGlobal);
		FreeResource(hRes);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		SetFileAttributes(szDll, FILE_ATTRIBUTE_HIDDEN);
	}

	{
		HMODULE hMod = LoadLibrary(szDll);
		if (hMod == NULL)
			return ;

		PFNINSTALLHOOK		  pfnInstallHook		= (PFNINSTALLHOOK)GetProcAddress(hMod, (LPCSTR)1);
		PFNSTOPHOOK			  pfnStopHook			= (PFNSTOPHOOK)GetProcAddress(hMod, (LPCSTR)2);
		PFNINJECTTOWINLOGON   pfnInjectToWinlogon	= (PFNINJECTTOWINLOGON)GetProcAddress(hMod, (LPCSTR)3);
		PFNUNINJECTTOWINLOGON pfnUnInjectToWinlogon	= (PFNUNINJECTTOWINLOGON)GetProcAddress(hMod, (LPCSTR)4);
		if (pfnInstallHook == NULL || pfnStopHook == NULL || pfnInstallHook == NULL || pfnUnInjectToWinlogon == NULL)
		{
			FreeLibrary(hMod);
			return ;
		}
		DWORD dwRes = pfnInjectToWinlogon(DLL_NAME);
		if (dwRes)
		{
			TCHAR szLog[MAX_PATH] = {0};
			_stprintf(szLog, TEXT("Inject fail:%d"), dwRes);
			MessageBox(NULL, szLog, TEXT("提示"), MB_OK);
		}
		dwRes = pfnInstallHook();
		if (dwRes)
		{
			TCHAR szLog[MAX_PATH] = {0};
			_stprintf(szLog, TEXT("Hook fail:%d"), dwRes);
			MessageBox(NULL, szLog, TEXT("提示"), MB_OK);
		}
		FreeLibrary(hMod);
	}
}

inline void lock_UnInitialize()
{
	TCHAR szDll[MAX_PATH] = {0};
	GetSystemDirectory(szDll, sizeof(szDll) - 1);
	PathAddBackslash(szDll);
	lstrcat(szDll, DLL_NAME);
	HMODULE hMod = LoadLibrary(szDll);
	if (hMod == NULL)
	{
		DeleteFile(szDll);
		return ;
	}

	PFNINSTALLHOOK		  pfnInstallHook		= (PFNINSTALLHOOK)GetProcAddress(hMod, (LPCSTR)1);
	PFNSTOPHOOK			  pfnStopHook			= (PFNSTOPHOOK)GetProcAddress(hMod, (LPCSTR)2);
	PFNINJECTTOWINLOGON   pfnInjectToWinlogon	= (PFNINJECTTOWINLOGON)GetProcAddress(hMod, (LPCSTR)3);
	PFNUNINJECTTOWINLOGON pfnUnInjectToWinlogon	= (PFNUNINJECTTOWINLOGON)GetProcAddress(hMod, (LPCSTR)4);
	if (pfnInstallHook == NULL || pfnStopHook == NULL || pfnInstallHook == NULL || pfnUnInjectToWinlogon == NULL)
	{
		FreeLibrary(hMod);
		DeleteFile(szDll);
		return ;
	}
	pfnStopHook();
	SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
	pfnUnInjectToWinlogon(DLL_NAME);
	FreeLibrary(hMod);
	Sleep(1000);

	if (!DeleteFile(szDll))
	{
		TCHAR szLog[MAX_PATH] = {0};
		_stprintf(szLog, TEXT("deletefile fail:%d"), GetLastError());
		MessageBox(NULL, szLog, TEXT("提示"), MB_OK);
	}
}

#endif