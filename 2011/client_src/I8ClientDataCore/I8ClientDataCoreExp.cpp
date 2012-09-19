/*******************************************************************************
*   Filename:       I8ClientDataCoreExp.cpp                                    *
*   Author:         夏磊                                                       *
*   eMail:          None.                                                      *
*   Tel:            None.                                                      *
*   Date:           2011-6-23                                                  *
*   Description:    客户端数据接口声明                                         *
*   Reference:      None.                                                      *
*   Module:         客户端数据层                                               *
*   Version history:                                                           *
*------------------------------------------------------------------------------*
*   Version     YYYY-MM-DD-INDEX    Modified By     Description                *
*   1.0.0       2011-6-23           夏磊            创建初始版本               *
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "I8ClientDataCoreExp.h"
#include "I8ClientDataCore.h"

/*----------------------------------------------------------------------------*/
/*                             Global Variables                               */
/*----------------------------------------------------------------------------*/
CI8ClientDataCore g_clDataCore;

/*----------------------------------------------------------------------------*/
/*                            Function Declaration                            */
/*----------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// FunctionName: I8CD_Init
// Description:  初始化客户端数据层，软件启动时调用，不允许在调用它之前调用其它数据层函数
//-----------------------------------------------------------------------------
void _stdcall
I8CD_Init(void)
{
    g_clDataCore.Init();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_Release
// Description:  释放客户端数据层，在软件退出时调整，不允许在它调用后调用其它数据层函数
//-----------------------------------------------------------------------------
void _stdcall
I8CD_Release(void)
{
    g_clDataCore.Release();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetVersion
// Description:  取得客户端数据层的版本，保证动态库的版本和头文件的版本一致。
//               请在初始化客户端数据层后取得版本与C_CLIENT_DATA_CORE_VERSION比较，
//               不一致的话，需要更新数据层的动态库和头文件
//-----------------------------------------------------------------------------
UINT _stdcall //客户端数据层版本
I8CD_GetVersion(void)
{
    return g_clDataCore.GetVersion();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckRepeatedProcess
// Description:  重复进程检查，保证同一台机子只有一个客户端启动，请在初始化客户端数据层后调用
//-----------------------------------------------------------------------------
BOOL _stdcall              //返回FALSE表示另一个客户端存在，需要关闭当前客户端
I8CD_CheckRepeatedProcess(
    IN LPCTSTR lpszCmdLine //启动参数，如果有其它客户端存在将把参数发给该客户端执行
)
{
    return g_clDataCore.CheckRepeatedWindow(lpszCmdLine);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetUICallBack
// Description:  设置消息回调函数，用于数据层发起与界面层的通讯，请在界面层可以正常工作后调用
//-----------------------------------------------------------------------------
void _stdcall
I8CD_SetUICallBack(
    IN I8CD_UICallBack_pfn pfnCallBack,    //消息回调函数
    IN void*               pCallBackParam  //回调函数用的参数，可以传空
)
{
    g_clDataCore.SetUICallBack(pfnCallBack, pCallBackParam);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetSkinInfo
// Description:  取得客户端界面的皮肤文件
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //格式“e:\皮肤文件.zip”
I8CD_GetSkinFile(void)
{
    return g_clDataCore.GetSkinFile();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_WriteLog
// Description:  输出字符串到日志
//-----------------------------------------------------------------------------
void _stdcall
I8CD_WriteLog(
    IN LPCTSTR lpszLog,  //输出字符串
    IN BOOL    bAddDate  //输出生成时间
)
{
    g_clDataCore.WriteLog(lpszLog, bAddDate == TRUE);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsShowPost
// Description:  是否显示公告
//-----------------------------------------------------------------------------
BOOL _stdcall //显示公告
I8CD_IsShowPost(void)
{
    return g_clDataCore.IsShowPost();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetMacAddr
// Description:  取得MAC地址
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //MAC地址
I8CD_GetMacAddr(void)
{
    return g_clDataCore.GetMacAddr();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInternetBarID
// Description:  取得网吧ID
//-----------------------------------------------------------------------------
long _stdcall //网吧ID
I8CD_GetInternetBarID(void)
{
    return g_clDataCore.GetInternetBarID();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetChannelsID
// Description:  取得渠道ID
//-----------------------------------------------------------------------------
long _stdcall //渠道ID
I8CD_GetChannelsID(void)
{
    return g_clDataCore.GetChannelsID();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetPostInfo
// Description:  取得公告信息
//-----------------------------------------------------------------------------
void _stdcall
I8CD_GetPostInfo(
    OUT I8CD_PostInfo_st* pstPostInfo //公告信息
)
{
    return g_clDataCore.GetPostInfo(pstPostInfo);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInternetBarInfo
// Description:  取得网吧信息
//-----------------------------------------------------------------------------
void _stdcall
I8CD_GetInternetBarInfo(
    OUT I8CD_InternetBarInfo_st* pstInternetBarInfo //网吧信息
)
{
    return g_clDataCore.GetInternetBarInfo(pstInternetBarInfo);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsEnableBrowseGameDirectory
// Description:  是否允许浏览游戏目录
//-----------------------------------------------------------------------------
BOOL _stdcall //允许浏览游戏目录
I8CD_IsEnableBrowseGameDirectory(void)
{
    return g_clDataCore.IsEnableBrowseDirectory();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReleaseData
// Description:  释放数据，用于释放由数据层在返回值中生成的数据
//-----------------------------------------------------------------------------
void _stdcall
I8CD_ReleaseData(
    IN void* pData //要释放的数据指针
)
{
    return g_clDataCore.ReleaseData(pData);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllGameBaseInfo
// Description:  取得全部游戏基础信息
//-----------------------------------------------------------------------------
I8CD_GameBaseInfoArray_st* _stdcall //全部游戏基础信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetAllGameBaseInfo(void)
{
    return g_clDataCore.GetAllGameBaseInfo();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllIconInfo
// Description:  取得全部图标信息
//-----------------------------------------------------------------------------
I8CD_IconInfoArray_st* _stdcall //全部图标信息，需要用ReleaseIconData释放返回的指针
I8CD_GetAllIconInfo(void)
{
    return g_clDataCore.GetAllIconInfo();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllNav
// Description:  取得导航栏按钮集
//-----------------------------------------------------------------------------
I8CD_NavArray_st* _stdcall //全部导航栏按钮信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetAllNav(void)
{
    return g_clDataCore.GetAllNav();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameBaseInfo
// Description:  取得游戏基础信息
//-----------------------------------------------------------------------------
BOOL _stdcall                             //取得成功
I8CD_GetGameBaseInfo(
    IN  DWORD                 gid,             //游戏ID
    OUT I8CD_GameBaseInfo_st* pstGameBaseInfo  //游戏基础信息
)
{
    return g_clDataCore.GetGameBaseInfo(gid, pstGameBaseInfo);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameInfo
// Description:  取得游戏信息
//-----------------------------------------------------------------------------
I8CD_GameInfo_st* _stdcall //游戏信息，为空表示游戏不存在，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetGameInfo(
    IN DWORD gid      //游戏ID
)
{
    return g_clDataCore.GetGameInfo(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetClientServiceState
// Description:  取得客户端服务状态
//-----------------------------------------------------------------------------
void _stdcall
I8CD_GetClientServiceState(
    OUT BOOL* pbOnline, //连线中
    OUT BOOL* pbProt    //还原保护可用
)
{
    g_clDataCore.GetClientServiceState(pbOnline, pbProt);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReportClickInfo
// Description:  向服务器上报点击信息
//-----------------------------------------------------------------------------
BOOL _stdcall                                       //操作成功
I8CD_ReportClickInfo(
    IN const I8CD_NavClickInfo_st*    pstNavClickInfo,      //导航栏点击信息
    IN int                            iNavClickInfoCount,   //导航栏点击信息数量
    IN const I8CD_GameClickInfo_st*   pstGameClickInfo,     //游戏点击信息
    IN int                            iGameClickInfoCount,  //游戏点击信息数量
    IN const I8CD_NavTabClickInfo_st* pstNavTabClickInfo,   //二级菜单点击信息
    IN int                            iNavTabClickInfoCount,//二级菜单点击信息数量
    IN BOOL                           bWaitAnswer           //需要等待回应
)
{
    return g_clDataCore.ReportClickInfoToServer(pstNavClickInfo, iNavClickInfoCount, pstGameClickInfo, iGameClickInfoCount, pstNavTabClickInfo, iNavTabClickInfoCount, bWaitAnswer);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetNavTab
// Description:  取得导航栏下属的标签页
//-----------------------------------------------------------------------------
I8CD_NavTabArray_st* _stdcall    //标签页信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetNavTab(
    IN I8CD_NavType_en enNavType //导航栏按钮类型
)
{
    return g_clDataCore.GetNavTab(enNavType);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_FindGame
// Description:  查找游戏
//-----------------------------------------------------------------------------
I8CD_GameIDArray_st* _stdcall         //匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_FindGame(
    IN LPCTSTR             lpszKey,   //关键字,可以为空
    IN I8CD_FindFilter_st* pstFilter, //筛选器,可以为空
    IN I8CD_Order_en       enOrder    //排序方式
)
{
    return g_clDataCore.FindGame(lpszKey, pstFilter, enOrder);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_FindGameByChar
// Description:  根据字母查找游戏
//-----------------------------------------------------------------------------
I8CD_GameIDArray_st* _stdcall //匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_FindGameByChar(
    IN TCHAR         tKey,    //字母(A-Z)
    IN I8CD_Order_en enOrder  //排序方式
    )
{
    return g_clDataCore.FindGame(tKey, enOrder);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameByClassName
// Description:  根据类别查找游戏
//-----------------------------------------------------------------------------
I8CD_GameIDArray_st* _stdcall       //匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetGameByClassName(
    IN LPCTSTR       lpszClassName, //游戏类别
    IN I8CD_Order_en enOrder        //排序方式
)
{
    return g_clDataCore.GetGameByClassName(lpszClassName, enOrder);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameOnToolBar
// Description:  取得工具栏上的游戏
//-----------------------------------------------------------------------------
I8CD_GameIDArray_st* _stdcall //匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetGameOnToolBar(void)
{
    return g_clDataCore.GetGameOnToolBar();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckAdvancedPassword
// Description:  检查客户端高级设置密码是否正确
//-----------------------------------------------------------------------------
BOOL _stdcall               //客户端高级设置密码匹配成功
I8CD_CheckAdvancedPassword(
    IN LPCTSTR lpszPassword //客户端高级设置密码
)
{
    return g_clDataCore.CheckAdvancedPassword(lpszPassword);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGBKFromString
// Description:  取得汉字国标扩展码
//-----------------------------------------------------------------------------
void _stdcall
I8CD_GetGBKFromString(
    OUT LPTSTR  lpszOutString, //生成的汉字国标扩展码
    IN  int     iOutMaxCount,  //生成字符串的最大字符数
    IN  LPCTSTR lpszString     //字符串
)
{
    return g_clDataCore.GetGBKFromString(lpszOutString, iOutMaxCount, lpszString);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetServerIP
// Description:  取得服务器IP
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //服务器IP
I8CD_GetServerIP(void)
{
    return g_clDataCore.GetServerIP();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetServerIP
// Description:  设置服务器IP
//-----------------------------------------------------------------------------
void _stdcall
I8CD_SetServerIP(
    IN LPCTSTR lpszServerIP //服务器IP
)
{
    g_clDataCore.SetServerIP(lpszServerIP);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetOperatorAfterRunGame
// Description:  取得运行游戏后界面执行的操作
//-----------------------------------------------------------------------------
I8CD_OperatorAfterRunGame_en _stdcall //运行游戏后界面需要执行的操作
I8CD_GetOperatorAfterRunGame(void)
{
    return g_clDataCore.GetOperatorAfterRunGame();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsInitializeHy
// Description:  还原保护是否可用
//-----------------------------------------------------------------------------
BOOL _stdcall //还原保护可用
I8CD_IsInitializeHy(void)
{
    return g_clDataCore.IsInitializeHy();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsInstallHy
// Description:  是否安装还原保护
//-----------------------------------------------------------------------------
BOOL _stdcall //还原保护已安装
I8CD_IsInstallHy(void)
{
    return g_clDataCore.IsInstallHy();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetProtectedDriver
// Description:  取得开启还原保护的磁盘
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //开启还原保护的磁盘，格式“CDE”
I8CD_GetProtectedDriver(void)
{
    return g_clDataCore.GetProtectedDriver();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetUnProtectedDriver
// Description:  取得未开启还原保护的磁盘
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //未开启还原保护的磁盘，格式“CDE”
I8CD_GetUnProtectedDriver(void)
{
    return g_clDataCore.GetUnProtectedDriver();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ProtectDriver
// Description:  对磁盘开启还原保护和关闭还原保护(格式：CDE)
//-----------------------------------------------------------------------------
BOOL _stdcall                          //操作成功
I8CD_ProtectDriver(
    IN  LPCTSTR lpszProtectedDriver,   //需要开启还原保护的磁盘
    IN  LPCTSTR lpszUnProtectedDriver, //需要关闭还原保护的磁盘
    OUT BOOL*   pbNeedRestart          //需要重启计算机
)
{
    return g_clDataCore.ProtectDriver(lpszProtectedDriver, lpszUnProtectedDriver, pbNeedRestart);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckDisk
// Description:  磁盘检测
//-----------------------------------------------------------------------------
void _stdcall
I8CD_CheckDisk(
    IN LPCTSTR lpszDriver //需要检测的磁盘，格式“CDE”
)
{
    g_clDataCore.CheckDisk(lpszDriver);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SaveArchiveToFlashDisk
// Description:  个人优盘存档
//-----------------------------------------------------------------------------
BOOL _stdcall    //操作成功
I8CD_SaveArchiveToFlashDisk(
    IN DWORD gid //游戏ID
)
{
    return g_clDataCore.SaveArchiveToFlashDisk(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReadArchiveToFlashDisk
// Description:  个人优盘取档
//-----------------------------------------------------------------------------
BOOL _stdcall    //操作成功
I8CD_ReadArchiveToFlashDisk(
    IN DWORD gid //游戏ID
)
{
    return g_clDataCore.ReadArchiveToFlashDisk(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ShowFlashDisk
// Description:  显示个人优盘
//-----------------------------------------------------------------------------
BOOL _stdcall //操作成功
I8CD_ShowFlashDisk(void)
{
    return g_clDataCore.ShowFlashDisk();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_LockComputer
// Description:  锁定计算机
//-----------------------------------------------------------------------------
void _stdcall
I8CD_LockComputer(void)
{
    g_clDataCore.LockComputer();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetResolution
// Description:  设置屏幕分辨率
//-----------------------------------------------------------------------------
void _stdcall
I8CD_SetResolution(void)
{
    g_clDataCore.SetResolution();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetSysMouse
// Description:  设置系统鼠标
//-----------------------------------------------------------------------------
void _stdcall
I8CD_SetSysMouse(void)
{
    g_clDataCore.SetSysMouse();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetVolume
// Description:  设置音量
//-----------------------------------------------------------------------------
void _stdcall
I8CD_SetVolume(void)
{
    g_clDataCore.SetVolume();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_OpenGameDirectory
// Description:  打开游戏所在目录
//-----------------------------------------------------------------------------
BOOL _stdcall    //操作成功
I8CD_OpenGameDirectory(
    IN DWORD gid //游戏ID
)
{
    return g_clDataCore.OpenGameDirectory(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsGameEnableRun
// Description:  检查游戏是否可运行，没考虑人为删除游戏文件和游戏版本不是最新的问题
//-----------------------------------------------------------------------------
BOOL _stdcall    //游戏可运行
I8CD_IsGameEnableRun(
    IN DWORD gid //游戏ID
)
{
    return g_clDataCore.IsGameEnableRun(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInputMethod
// Description:  取得输入法的游戏ID，返回零表示网吧没有在三层更新中配置输入法
//-----------------------------------------------------------------------------
DWORD _stdcall //输入法的游戏ID
I8CD_GetInputMethod(void)
{
    return g_clDataCore.GetInputMethod();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_UpdateGame
// Description:  更新游戏
//-----------------------------------------------------------------------------
HANDLE _stdcall                                   //更新任务句柄，无需释放，如果返回值为空表示游戏无需更新，可以直接运行
I8CD_UpdateGame(
    IN DWORD                   gid,               //游戏ID
    IN I8CD_DownloadMode_en    enDownloadMode,    //下载方式
    IN I8CD_UpdateCallBack_pfn pfnUpdateCallBack, //更新回调函数，可以传空
    IN void*                   pCallBackParam     //更新回调函数用的参数，可以传空
)
{
    return g_clDataCore.UG_UpdateGame(gid, enDownloadMode, pfnUpdateCallBack, pCallBackParam);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_StopUpdate
// Description:  停止下载
//-----------------------------------------------------------------------------
void _stdcall
I8CD_StopUpdate(
    IN HANDLE hUpdate //更新任务句柄
)
{
    g_clDataCore.UG_StopUpdate(hUpdate);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetPertinentGame
// Description:  取得游戏相关工具
//-----------------------------------------------------------------------------
I8CD_GameIDArray_st* _stdcall //匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
I8CD_GetPertinentGame(
    IN DWORD gid         //游戏ID
)
{
    return g_clDataCore.GetPertinentGame(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RunGame
// Description:  运行游戏，运行游戏前需要"更新游戏"或"检查游戏信息和游戏文件是否匹配"。
//               "更新游戏"要等待更新任务为完成，"检查游戏信息和游戏文件是否匹配"要保证版本一致
//-----------------------------------------------------------------------------
I8CD_RunGameResult_en _stdcall //运行游戏结果
I8CD_RunGame(
    IN DWORD gid          //游戏ID
)
{
    return g_clDataCore.RunGame(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetClientMenu
// Description:  取得菜单分级信息，如果没有返回NULL
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //菜单分级信息，XML数据格式
I8CD_GetClientMenu(void)
{
    return g_clDataCore.GetClientMenu();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsVisible
// Description:  检查窗口是否可见，如果不可见，客户端界面层以不可见方式启动，直到回调函数收到显示命令
//-----------------------------------------------------------------------------
BOOL _stdcall  //正常/隐式启动客户端
I8CD_IsVisible(void)
{
    return g_clDataCore.IsVisible();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RemoteControl
// Description:  启动服务端的远程控制
//-----------------------------------------------------------------------------
BOOL _stdcall  //操作成功
I8CD_RemoteControl(void)
{
    return g_clDataCore.RemoteControl();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllDriver
// Description:  取得所有物理磁盘
//-----------------------------------------------------------------------------
LPCTSTR _stdcall //所有物理磁盘，格式“CDE”
I8CD_GetAllDriver(void)
{
    return g_clDataCore.GetAllDriver();
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetStartupInfo
// Description:  取得菜单启动信息
//-----------------------------------------------------------------------------
void _stdcall
I8CD_GetStartupInfo(
    OUT I8CD_StartupInfo_st* pstStartupInfo //菜单启动信息
)
{
    return g_clDataCore.GetStartupInfo(pstStartupInfo);
}

//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsNeedRefreshGameVDisk
// Description:  检测是否需要为游戏刷新虚拟盘
//-----------------------------------------------------------------------------
BOOL _stdcall    //需要刷新虚拟盘
I8CD_IsNeedRefreshGameVDisk(
    IN DWORD gid //游戏ID
)
{
    return g_clDataCore.IsNeedRefreshGameVDisk(gid);
}
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RefreshGameVDisk
// Description:  为游戏刷新虚拟盘
//-----------------------------------------------------------------------------
void _stdcall
I8CD_RefreshGameVDisk(
    IN DWORD gid //游戏ID
)
{
    g_clDataCore.RefreshGameVDisk(gid);
}
