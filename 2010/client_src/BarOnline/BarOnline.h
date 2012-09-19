#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"

typedef struct tagFileMap
{
	HWND hWnd;
	TCHAR szCmdLine[1024];
}tagFileMap;

class CBarOnlineApp : public CWinAppEx
{
public:
	CBarOnlineApp();
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CString GetCmdLine();

	DECLARE_MESSAGE_MAP()
private:
	Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
	ULONG_PTR						gdiplusToken;
	HANDLE							m_hFileMap;
};

extern CBarOnlineApp theApp;