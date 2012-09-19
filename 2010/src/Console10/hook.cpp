#include "stdafx.h"
#include "hook.h"
#include <set>

namespace i8desk
{
	static std::set<HWND> m_setDlg;

	//hook CreateDialogIndirectParamA, EndDialog
	typedef HWND (WINAPI *PFNCREATEDIALOGINDIRECTPARAMA)(
		__in_opt HINSTANCE hInstance,
		__in LPCDLGTEMPLATEW lpTemplate,
		__in_opt HWND hWndParent,
		__in_opt DLGPROC lpDialogFunc,
		__in LPARAM dwInitParam);

	typedef BOOL (WINAPI *PFNENDDIALOG)(HWND hDlg, INT_PTR nResult);

	PFNCREATEDIALOGINDIRECTPARAMA	pfnCreateDialogIndirectParamA	= NULL;
	PFNENDDIALOG					pfnEndDialog					= NULL;

	static BYTE oldAddrCreateDialogIndirectParamW[8] = {0};
	static BYTE newAddrCreateDialogIndirectParamW[8] = {0};
	static BYTE oldAddrEndDialog[8] = {0};
	static BYTE newAddrEndDialog[8] = {0};

	static void HookApi(DWORD funAddr, PBYTE dwNewAddr)
	{
		HANDLE hProc = GetCurrentProcess();
		DWORD dwOldProtect = 0;
		BOOL ret = FALSE;
		ret = VirtualProtectEx(hProc, (LPVOID)funAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		ret = WriteProcessMemory(hProc, (LPVOID)funAddr, dwNewAddr, 5, NULL);
		ret = VirtualProtectEx(hProc, (LPVOID)funAddr, 5, dwOldProtect, &dwOldProtect);
	}

	HWND WINAPI Hook_CreateDialogIndirectParamA(
		__in_opt HINSTANCE hInstance,
		__in LPCDLGTEMPLATEA lpTemplate,
		__in_opt HWND hWndParent,
		__in_opt DLGPROC lpDialogFunc,
		__in LPARAM dwInitParam)		
	{
		HookApi((DWORD)pfnCreateDialogIndirectParamA, oldAddrCreateDialogIndirectParamW);
		HWND hWnd = pfnCreateDialogIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
		m_setDlg.insert(hWnd);
		char buf[MAX_PATH] = {0};
		GetWindowText(hWnd, buf, MAX_PATH);
		OutputDebugString(buf);
		OutputDebugString("\r\n");
		HookApi((DWORD)pfnCreateDialogIndirectParamA, newAddrCreateDialogIndirectParamW);
		return hWnd;
	}

	BOOL WINAPI Hook_EndDialog(HWND hDlg, INT_PTR nResult)
	{
		HookApi((DWORD)pfnEndDialog, oldAddrEndDialog);
		BOOL ret = pfnEndDialog(hDlg, nResult);
		m_setDlg.erase(hDlg);
		HookApi((DWORD)pfnEndDialog, newAddrEndDialog);
		return ret;
	}

	void InitHookApi()
	{
		HMODULE hMod = LoadLibrary("user32.dll");
		pfnCreateDialogIndirectParamA = (PFNCREATEDIALOGINDIRECTPARAMA) GetProcAddress(hMod, "CreateDialogIndirectParamA");
		if (pfnCreateDialogIndirectParamA != NULL)
		{
			memcpy(&oldAddrCreateDialogIndirectParamW, pfnCreateDialogIndirectParamA, 5);
			newAddrCreateDialogIndirectParamW[0] = 0xe9;
			*(DWORD*)(newAddrCreateDialogIndirectParamW+1) = (DWORD)Hook_CreateDialogIndirectParamA - (DWORD)pfnCreateDialogIndirectParamA - 5;
			HookApi((DWORD)pfnCreateDialogIndirectParamA, newAddrCreateDialogIndirectParamW);
		}
	
		pfnEndDialog = (PFNENDDIALOG)GetProcAddress(hMod, "EndDialog");
		if (pfnEndDialog != NULL)
		{
			memcpy(&oldAddrEndDialog, pfnEndDialog, 5);
			newAddrEndDialog[0] = 0xe9;
			*(DWORD*)(newAddrEndDialog+1) = (DWORD)Hook_EndDialog- (DWORD)pfnEndDialog - 5;
			HookApi((DWORD)pfnEndDialog, newAddrEndDialog);
		}
	}

	void UnHookApi()
	{
		if (pfnCreateDialogIndirectParamA != NULL)
		{
			HookApi((DWORD)pfnCreateDialogIndirectParamA, oldAddrCreateDialogIndirectParamW);
		}
		if (pfnEndDialog != NULL)
		{
			HookApi((DWORD)pfnEndDialog, oldAddrEndDialog);
		}
	}
	void DesotryPopDialog()
	{
		/*
		while (m_setDlg.size())
		{
			HWND hWnd = *m_setDlg.begin();
			::PostMessage(hWnd, WM_CLOSE, 0, 0);
			::EndDialog(hWnd, IDCANCEL);
		}
		//*/
	}
}