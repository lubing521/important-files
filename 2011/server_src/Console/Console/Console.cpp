
// Console.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Console.h"
#include "ConsoleDlg.h"

#include <atlwin.h>

#include "DlgLogin.h"
#include "LoginMgr.h"
#include "LogHelper.h"
#include "DlgWaitMsg.h"
#include "UI/WZDialog.h"


#include "../../../include/Win32/System/except.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/Utility/ScopeGuard.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



i8desk::ManagerInstance *AfxManagerInstance()
{
	return i8desk::loginHelper.mgrInstance_.get();
}



// CConsoleApp

BEGIN_MESSAGE_MAP(CConsoleApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CConsoleApp 构造

CConsoleApp::CConsoleApp()
: miniDump_(new except::MiniDump)
, isRunWizard_(false)
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}



// 唯一的一个 CConsoleApp 对象

CConsoleApp theApp;


// CConsoleApp 初始化

BOOL CConsoleApp::InitInstance()
{
	CWinAppEx::InitInstance();


	struct ScopeExit
	{
		void Run()
		{
			::TerminateProcess(::GetCurrentProcess(), 0);
		}
	}scopeExit;
	async::ScopeGuard<void> scope(std::tr1::bind(&ScopeExit::Run, scopeExit));

	SetRegistryKey(_T("i8desk"));
	SECURITY_ATTRIBUTES arributes = {0};
	async::thread::AutoMutex Mutex(&arributes, TRUE, _T("i8DeskConsoleEvent"));

	if( ::GetLastError() == ERROR_ALREADY_EXISTS )
		return FALSE;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 以向导运行
	if( isRunWizard_ )
	{
		if( !i8desk::loginHelper.Connect(_T("127.0.0.1")) )
			AfxMessageBox(_T("不能连接到主服务"));
		CWZDialog dlg;
		if( IDOK != dlg.DoModal() )
			return FALSE;
	}
	else
	{
		CDlgLogin login(std::tr1::bind(&i8desk::LoginMgr::Connect, &i8desk::loginHelper, std::tr1::placeholders::_1),
			std::tr1::bind(&i8desk::LoginMgr::Password, &i8desk::loginHelper, std::tr1::placeholders::_1));
		if( login.DoModal() != IDOK )
			return FALSE;

	}

	//CDlgWaitMsg waitDlg;
	//waitDlg.Exec(std::tr1::bind(&AsyncDataImpl::Run));

	//i8desk::Log() << logsystem::Endl;
	//i8desk::Log() << _T("===================================================");

	CConsoleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 退出
	i8desk::loginHelper.LogOff();
	i8desk::DestroyLog();


	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


int CConsoleApp::ExitInstance()
{


	return CWinAppEx::ExitInstance();
}


void CConsoleApp::ParseCommandLine(CCommandLineInfo& rCmdInfo)
{
	for (int i = 1; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		if( (pszParam[0] == '-' || pszParam[0] == '/') 
			&& pszParam[1] == 'w' )
		{
			isRunWizard_ = true;
		}
	}

}
