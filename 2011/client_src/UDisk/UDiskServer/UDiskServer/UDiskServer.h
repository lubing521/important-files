
// UDiskServer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUDiskServerApp:
// 有关此类的实现，请参阅 UDiskServer.cpp
//

class CUDiskServerApp : public CWinAppEx
{
public:
	CUDiskServerApp();

// 重写
	public:
	virtual BOOL InitInstance();
	BOOL OneInstance();
	void SetMainHWnd(HWND hWnd);
	SrvConfigInfo* m_pConfigInfo;
	HANDLE m_hMapFile;
// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CUDiskServerApp theApp;