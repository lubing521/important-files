#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"

class CDeskLockerApp : public CWinAppEx
{
public:
	CDeskLockerApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
private:
	GdiplusStartupInput	gdiplusStartupInput;
	ULONG_PTR			gdiplusToken;
};

extern CDeskLockerApp theApp;