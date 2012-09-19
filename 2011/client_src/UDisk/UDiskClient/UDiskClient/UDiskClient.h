
// UDiskClient.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUDiskClientApp:
// 有关此类的实现，请参阅 UDiskClient.cpp
//

class CUDiskClientApp : public CWinAppEx
{
public:
	CUDiskClientApp();
	CSocketEx* m_pSocket ;

	ULONG_PTR m_gdiplusToken;
	HANDLE m_hMapFile;
// 重写
	public:
	virtual BOOL InitInstance();
	void SetMainHWnd(HWND hWnd);
	BOOL OneInstance();
	bool Init(CString& szUserName);
// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
protected:
	int FindServer(vector<string>& veclst);
};

extern CUDiskClientApp theApp;