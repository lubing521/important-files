#include "stdafx.h"
#include "wtypes.h"
#include "tlhelp32.h"
#include "hook.h"

namespace hook
{
	typedef long (WINAPI *PFNZWCREATEPROCESS)  (DWORD, DWORD, DWORD, HANDLE , DWORD, DWORD, DWORD, DWORD); 
	typedef long (WINAPI *PFNZWCREATEPROCESSEX)(DWORD, DWORD, DWORD, HANDLE , DWORD, DWORD, DWORD, DWORD, DWORD ); 
	static PFNZWCREATEPROCESS pfnZwCreateProcess;
	static PFNZWCREATEPROCESSEX pfnZwCreateProcessEx;

	static BYTE oldAddrOfCreateProcess[8] = {0};
	static BYTE newAddrOfCreateProcess[8] = {0};
	static BYTE oldAddrOfCreateProcessEx[8] = {0};
	static BYTE newAddrOfCreateProcessEx[8] = {0};
	static DWORD hExeplore = 0;

	static DWORD GetExploreID()
	{
		DWORD id = 0;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS,	0);
		PROCESSENTRY32 tpe;
		tpe.dwSize = sizeof(PROCESSENTRY32);
		BOOL ret = Process32First(hSnapshot, &tpe);
		while(ret)
		{
			if (lstrcmpi(tpe.szExeFile, TEXT("explorer.exe")) == 0)
			{
				id = tpe.th32ProcessID;
				ret = FALSE;
			}
			ret = Process32Next(hSnapshot,&tpe);
		}
		CloseHandle(hSnapshot);
		
		return id;
	}

	static void hookapi(DWORD funAddr, PBYTE dwNewAddr)
	{
		HANDLE hProc = GetCurrentProcess();
		DWORD dwOldProtect = 0;
		BOOL ret = FALSE;
		ret = VirtualProtectEx(hProc, (LPVOID)funAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		ret = WriteProcessMemory(hProc, (LPVOID)funAddr, dwNewAddr, 5, NULL);
		ret = VirtualProtectEx(hProc, (LPVOID)funAddr, 5, dwOldProtect, &dwOldProtect);
	}

	long WINAPI ZwCreateProcess(DWORD p0, DWORD p1, DWORD p2, HANDLE p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetExploreID());
		if (hProcess != NULL) p3 = hProcess;

		hookapi((DWORD)pfnZwCreateProcess, oldAddrOfCreateProcess);
		long ret = pfnZwCreateProcess(p0, p1, p2, p3, p4, p5, p6, p7);
		hookapi((DWORD)pfnZwCreateProcess, newAddrOfCreateProcess);
		return ret;
	}

	long WINAPI ZwCreateProcessEx(DWORD p0, DWORD p1, DWORD p2, HANDLE p3, DWORD p4, DWORD p5, DWORD p6, DWORD p7, DWORD p8)
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetExploreID());
		if (hProcess != NULL) p3 = hProcess;

		hookapi((DWORD)pfnZwCreateProcessEx, oldAddrOfCreateProcessEx);
		long ret = pfnZwCreateProcessEx(p0, p1, p2, p3, p4, p5, p6, p7, p8);
		hookapi((DWORD)pfnZwCreateProcessEx, newAddrOfCreateProcessEx);
		
		return ret;
	}

	void InitHookApi()
	{
		hExeplore = GetExploreID();
		HMODULE hMod = LoadLibrary(TEXT("ntdll.dll"));

		pfnZwCreateProcess = (PFNZWCREATEPROCESS) GetProcAddress(hMod, "ZwCreateProcess");	
		if (pfnZwCreateProcess != NULL)
		{
			memcpy(&oldAddrOfCreateProcess, pfnZwCreateProcess, 5);
			newAddrOfCreateProcess[0] = 0xe9;
			*(DWORD*)(newAddrOfCreateProcess+1) = (DWORD)ZwCreateProcess - (DWORD)pfnZwCreateProcess - 5;
			hookapi((DWORD)pfnZwCreateProcess, newAddrOfCreateProcess);
		}

		pfnZwCreateProcessEx = (PFNZWCREATEPROCESSEX) GetProcAddress(hMod, "ZwCreateProcessEx");	
		if (pfnZwCreateProcessEx != NULL)
		{
			memcpy(&oldAddrOfCreateProcessEx, pfnZwCreateProcessEx, 5);
			newAddrOfCreateProcessEx[0] = 0xe9;
			*(DWORD*)(newAddrOfCreateProcessEx+1) = (DWORD)ZwCreateProcessEx - (DWORD)pfnZwCreateProcessEx - 5;
			hookapi((DWORD)pfnZwCreateProcessEx, newAddrOfCreateProcessEx);
		}
	}

	void UnHookApi()
	{
		if (pfnZwCreateProcess != NULL)
			hookapi((DWORD)pfnZwCreateProcess, oldAddrOfCreateProcess);
		if (pfnZwCreateProcessEx != NULL)
			hookapi((DWORD)pfnZwCreateProcessEx, oldAddrOfCreateProcessEx);

		Sleep(1000);
	}
}