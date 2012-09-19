#include "stdafx.h"
#include <tchar.h>

#define INJECT_PROC_NAME	TEXT("winlogon.exe")

#pragma warning(disable:4996)
#pragma comment(lib, "psapi.lib")

//forward declare.
void HookWndProc(BOOL bLoad);

//ÆÁ±Îctrl + alt + delete.
//hook FindWindow('SAS window class', 'SAS window') proc.
static WNDPROC	glWndProc	= NULL;
static HWND		glWnd		= NULL;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		HookWndProc(TRUE);
		break;
	case DLL_PROCESS_DETACH:
		HookWndProc(FALSE);
		break;
	}
	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((uMsg == WM_HOTKEY) && (HIWORD(lParam) == VK_DELETE))
	{
		WORD ctrl = LOWORD(lParam);
		if ((ctrl & VK_CONTROL) && (ctrl & VK_MENU))
		{
			OutputDebugString(TEXT("hook:ctrl + alt + del.\r\n"));
			return TRUE;
		}
	}
	return CallWindowProc(glWndProc, hwnd, uMsg, wParam, lParam);
}

void HookWndProc(BOOL bLoad)
{
	if (bLoad)
	{
		if (glWnd == NULL && glWndProc == NULL)
		{
			glWnd = FindWindow(TEXT("SAS window class"), TEXT("SAS window"));
			if (!IsWindow(glWnd))
				return ;
			glWndProc = (WNDPROC)GetWindowLong(glWnd, GWL_WNDPROC);
			SetWindowLong(glWnd, GWL_WNDPROC, (LONG)WindowProc);
		}
	}
	else
	{
		if (glWnd != NULL && glWndProc != NULL)
		{
			SetWindowLong(glWnd, GWL_WNDPROC, (LONG)glWndProc);
		}
		glWnd = NULL;
		glWndProc = NULL;
	}
}