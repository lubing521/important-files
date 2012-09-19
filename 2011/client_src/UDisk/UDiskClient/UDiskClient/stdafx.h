
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


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning (disable :4996)
#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include "..\..\defin.h"
#include "SocketEx.h"
#include "math.h"
#include "string"
#include "vector"
#include "map"
#include "algorithm"
#include "Dbghelp.h"
#include "vssym32.h"
#include "gdiplus.h"
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib, "gdiplus.lib")
using namespace std;

using namespace Gdiplus;

enum CustomMessage
{
	WM_MSG_POSTFILE = WM_USER + 0x0100,
	WM_MSG_DOWNODFILE  =  WM_USER +  0x0101,
	WM_MSG_DELETEFILE = WM_USER + 0x0102,
	WM_MSG_PROGRESS = WM_USER + 0x0103,
	WM_MSG_SAVEGAME = WM_USER + 0x0104,
	WM_MSG_READGAME = WM_USER + 0x0105
};
typedef struct tagGameInfo
{
	DWORD	GID;//游戏GID
	DWORD	PID;//游戏关联的GID
	char	NAME[MAX_PATH];//游戏名
	char	IDCCLASS[MAX_PATH];//游戏类别
	char	GAMEEXE[MAX_PATH];//游戏官方EXE
	char	PARAM[MAX_PATH];//游戏运行参数
	DWORD	SIZE;//游戏大小,以K为单位
	DWORD	DESKLNK;//游戏是否显示到桌面快捷方式
	DWORD	TOOLBAR;//游戏是否显示到菜单的工具栏上
	char	SVRPATH[MAX_PATH];//游戏在服务器上的路径
	char	VDPATH[MAX_PATH];//游戏在客户端的虚拟盘路径
    char	CLIPATH[MAX_PATH];//游戏在客户机上的路径
    DWORD   dwDeleteFlg;    //游戏空间不足时允许删除
	char	MATCHFILE[MAX_PATH];//游戏自动搜索特征文件
	char	SAVEFILTER[MAX_PATH];//游戏存档信息
	DWORD	IDCUPTDATE;//游戏在中心服务器的更新时间
	DWORD	SVRUPTDATE;//游戏在网吧服务器的更新时间
	DWORD	IDCVER;//游戏在中心服务器上的版本
	DWORD	SVRVER;//游戏在网吧服务器上的版本
	DWORD	AUTOUPT;//游戏是否自动从服务器上更新
	DWORD	I8PLAY;//游戏需要强制更新
	DWORD	IDCCLICK;//游戏全网点率
	DWORD	SVRCLICK;//游戏网吧点率
	DWORD	ADDDATE;//游戏添加时间
	char	GAMELINK[MAX_PATH];//游戏官网
	DWORD	RUNTYPE;//游戏运行方式
	DWORD	IFUPDATE;//游戏是否更新
	DWORD	IFDISPLAY;//游戏是否显示
	char	VID[MAX_PATH];//游戏所在虚拟盘ID
	char    COMMENT[MAX_PATH];//Comment
    DWORD	STATUS;//游戏下载状态
    DWORD   dwRepair; //更新游戏时用CRC比较
	tagGameInfo()
	{
		ZeroMemory(SAVEFILTER,MAX_PATH);
	}

}tagGameInfo;

//虚拟盘信息
typedef struct tagVDiskInfo
{
	CString VID; //虚拟盘GUID
	DWORD IP; //虚拟盘ip
	DWORD Port; //虚拟盘端口
	TCHAR SvrDrv; //服务端盘符
	TCHAR   CliDrv; //客户端指定盘符
	DWORD Serial; //虚拟盘的序列号
	DWORD Type; //虚拟盘的类型
	DWORD LoadType; //虚拟盘的刷盘方式
	CString SID; //镜像服务器的标识。
}VDISKINFO;

#define  NET_CMD_FINDSERVER		0x0500

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


