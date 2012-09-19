
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类

#include "process.h"

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcview.h>

#include "Shlwapi.h"
#include "Dbghelp.h"
#include "Winsvc.h"
#include "Psapi.h"
#include "Tlhelp32.h"
#pragma comment (lib,"Shlwapi.lib")
#pragma comment (lib,"Dbghelp.lib")
#pragma comment (lib,"Psapi.lib")
#include "string"
 using namespace std;



#define STR_VDISKINFOVIEW		_T("虚拟盘信息")
#define STR_UPDATEGAMEINFO		_T("内网更新信息")
#define STR_SYNCGAMEINFO		_T("同步游戏信息")
#define STR_LONGVIEW			_T("日志信息")
enum TUserData
{
	NODE_VDISK,
	NODE_UPDATEGAME,
	NODE_SYNCGAME
};

#define NET_CMD_INTERACT_GUI		0x0400
#define NET_CMD_SYNCLOG				0x0401
#define NET_CMD_GETSYNCGAMEINFO			0x0402
#define NET_CMD_GETCONNECTINFO			0x0403
#define NET_CMD_IMMUPDATEALLGAME		0x0404
#define NET_CMD_REFRESHGUI				0x0405
#define NET_CMD_ENDSYNCGAME				0x0406

typedef struct tagVDInfo
{
	DWORD dwIp;
	WORD  nPort;
	TCHAR szSvrDrv;
	TCHAR szCliDrv;
	DWORD dwConnect;
	DWORD dwVersion;//(time_t);
	TCHAR szTempDir[MAX_PATH];
}tagVDInfo;
struct FastCopyStatus
{
	DWORD   Progress;					//更新的阶段。
	DWORD   StartType;					//当成功更新后，菜单是从虚拟盘启动，还是从本地启动。

	DWORD	TotalUpdateFile;			//总共需要更新文件数。
	DWORD	UpdateFile;					//己经更新的文件数。

	DWORD   CurProgress;				//当前文件的进度.
	DWORD   TotalProgress;				//整体进度。

	DWORD   AverageSpeed;				//平均速度
	DWORD   UsedSecond;					//更新己经用了多少秒。

	DWORD   LeftSecond;					//还需要多少秒。

	__int64 TotalFileSize;				//总共需要更新的字节数。//当空间不足时，这个值是告诉菜单需要删除的字节数。他应该是大于等于总需要更新的字节数。
	__int64 UpdateFizeSize;				//己更新字节数。

	char 	szCurFile[260];		//当前正在操作的文件.如果有错，则他是错误信息.
};

struct FastCopyStatusEx: public FastCopyStatus
{

	bool bConnect;
	DWORD gid;
	char GameName[MAX_PATH];
};

typedef struct tagUpdataInfo
{

	DWORD dwip;
	DWORD gid;
	DWORD speed;
	DWORD progress;
	DWORD64 uptsize;
	DWORD64 leftsize;
	char Name[64];

}UpdataInfo;



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


