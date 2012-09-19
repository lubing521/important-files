
// i8Safe.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "i8Safe.h"
#include "SafeCenter.h"
#include "i8SafeDlg.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#include "vld.h"
#endif

// Ci8SafeApp

BEGIN_MESSAGE_MAP(Ci8SafeApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Ci8SafeApp 构造

Ci8SafeApp::Ci8SafeApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 Ci8SafeApp 对象

Ci8SafeApp theApp;
BOOL Ci8SafeApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	//执行安全中心任务
	CLogFile::WriteLog("启动安全中心");
	ExcuteSafeCenter();

	//Ci8SafeDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “确定”来关闭对话框的代码
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: 在此放置处理何时用
	//	//  “取消”来关闭对话框的代码
	//}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL Ci8SafeApp::ExcuteSafeCenter()
{
	CSafeCenter safeCenter;
	char xmlPath[MAX_PATH]={0};
	GetModuleFileName(NULL, xmlPath, MAX_PATH);
	PathRemoveFileSpecA(xmlPath);
	int iIdx = strlen(xmlPath) - 1;
	while (xmlPath[iIdx] && xmlPath[iIdx] != '\\')
	{
		iIdx--;
	}
	xmlPath[iIdx] = '\0';
	lstrcat(xmlPath, TEXT("\\Data\\Config\\i8Safe.xml"));

	char szLog[2*MAX_PATH] = {0};
	sprintf(szLog, "读取配置文件:%s", xmlPath);
	CLogFile::WriteLog(szLog);

	BOOL bOK = safeCenter.ReadConfig(xmlPath);
	if(bOK == FALSE)
	{
		CLogFile::WriteLog("配置文件读取失败.");
		return FALSE;
	}
	bOK = safeCenter.Start();
	if (!bOK)
	{
		CLogFile::WriteLog("安全中心启动失败.");
		return FALSE;
	}
	else
	{
		CLogFile::WriteLog("安全中心启动成功.");
	}
	while (safeCenter.IsRun())
	{
		Sleep(100);
	}
	return TRUE;
}