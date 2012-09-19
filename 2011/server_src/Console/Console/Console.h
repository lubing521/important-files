
// Console.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <memory>


namespace i8desk
{
	struct ManagerInstance;
}


namespace except
{
	class MiniDump;
}
// CConsoleApp:
// 有关此类的实现，请参阅 Console.cpp
//

class CConsoleApp : public CWinAppEx
{
public:
	CConsoleApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();


protected:
	void ParseCommandLine(CCommandLineInfo& rCmdInfo);

private:
	std::auto_ptr<except::MiniDump> miniDump_;
	bool isRunWizard_;
// 实现

	DECLARE_MESSAGE_MAP()

};


i8desk::ManagerInstance *AfxManagerInstance();

extern CConsoleApp theApp;