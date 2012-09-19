/*******************************************************************************
*   Filename:       I8ClientDataCoreExp.h                                      *
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
*   1.1.0       2011-7-4            夏磊            1.更改函数调用约定         *
*                                                   2.完善GetInputMethod的注释 *
*                                                   3.修改UG_UpdateGame的声明  *
*                                                   4.删除UG_GetUpdateStatus   *
*                                                   5.把两个同名函数ReleaseData*
*                                                     改名为ReleaseData和      *
*                                                     ReleaseIconData          *
*                                                   6.把两个同名函数FindGame改 *
*                                                     名为FindGameByGame       *
*                                                     和FindGameByChar         *
*   1.2.0       2011-7-6            夏磊            增加函数GetCommnet和       *
*                                                   GetClientMenu              *
*   1.3.0       2011-7-14           夏磊            1.函数GetCommnet增加参数   *
*                                                   2.删除ReleaseIconData,     *
*                                                     功能由ReleaseData替代    *
*                                                   3.修改GetAllGameInfo       *
*                                                     为GetAllGameBaseInfo     *
*                                                   4.删除GetGameExtendInfo,   *
*                                                     功能由GetGameInfo替代    *
*                                                   5.把FindGameByString改为   *
*                                                     FindGame,并强化功能      *
*   1.4.0       2011-7-27           夏磊            1.导出C++函数改为导出C函数 *
*                                                   2.所有函数改名             *
*   1.5.0       2011-8-1            夏磊            1.增加函数I8CD_IsVisible   *
*                                                   2.增加I8CD_RemoteControl   *
*   1.6.0       2011-8-2            夏磊            修改I8CD_CheckGameMatching *
*                                                   为I8CD_IsGameEnableRun     *
*   1.7.0       2011-8-10           夏磊            删除I8CD_GetCommnet        *
*   1.7.1       2011-8-15           夏磊            修改I8CD_UpdateGame的参数  *
*   1.7.2       2011-8-23           夏磊            调整I8CD_ReportClickInfo   *
*   1.8.0       2011-10-12          夏磊            1.增加接口I8CD_GetAllDriver*
*                                                   2.调整I8CD_IsVisible       *
*                                                   3.调整I8CD_RemoteControl   *
*   1.9.0       2011-10-20          夏磊            增加I8CD_GetStartupInfo    *
*   1.9.1       2011-11-14          夏磊            增加I8CD_GetGameByClassName*
*                                                   和I8CD_FindGameByChar的参数*
*   1.10.0      2012-1-11           夏磊            增加I8CD_RefreshGameVDisk和*
*                                                   I8CD_IsNeedRefreshGameVDisk*
*******************************************************************************/

/*----------------------------------------------------------------------------*/
/*                            Compilation Control                             */
/*----------------------------------------------------------------------------*/
#pragma once

#ifdef I8CLIENTDATACORE_EXPORTS
#define DLL_API_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_API_EXPORT extern "C" __declspec(dllimport)
#endif

/*----------------------------------------------------------------------------*/
/*                               Include Files                                */
/*----------------------------------------------------------------------------*/
#include "I8ClientDefine.h"

/*----------------------------------------------------------------------------*/
/*                            Function Prototypes                             */
/*----------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// FunctionName: I8CD_Init
// Description:  初始化客户端数据层，软件启动时调用，不允许在调用它之前调用其它数据层函数
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_Init(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_Release
// Description:  释放客户端数据层，在软件退出时调整，不允许在它调用后调用其它数据层函数
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_Release(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetVersion
// Description:  取得客户端数据层的版本，保证动态库的版本和头文件的版本一致。请在初始化客户端数据层后取得版本与C_CLIENT_DATA_CORE_VERSION比较，不一致的话，需要更新数据层的动态库和头文件
// Parameters:   None.
// Return:       客户端数据层版本
//-----------------------------------------------------------------------------
DLL_API_EXPORT UINT _stdcall I8CD_GetVersion(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckRepeatedProcess
// Description:  重复进程检查，保证同一台机子只有一个客户端启动，请在初始化客户端数据层后调用
// Parameters:   1) LPCTSTR lpszCmdLine [IN] 启动参数，如果有其它客户端存在将把参数发给该客户端执行
// Return:       返回FALSE表示另一个客户端存在，需要关闭当前客户端
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_CheckRepeatedProcess(IN LPCTSTR lpszCmdLine);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetUICallBack
// Description:  设置消息回调函数，用于数据层发起与界面层的通讯，请在界面层可以正常工作后调用
// Parameters:   1) I8CD_UICallBack_pfn pfnCallBack    [IN] 消息回调函数
//               2) void*               pCallBackParam [IN] 回调函数用的参数，可以传空
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_SetUICallBack(IN I8CD_UICallBack_pfn pfnCallBack, IN void* pCallBackParam);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetSkinFile
// Description:  取得客户端界面的皮肤文件
// Parameters:   None.
// Return:       格式“e:\皮肤文件.zip”
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetSkinFile(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_WriteLog
// Description:  输出字符串到日志
// Parameters:   1) LPCTSTR lpszLog  [IN] 输出字符串
//               2) BOOL    bAddDate [IN] 输出生成时间
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_WriteLog(IN LPCTSTR lpszLog, IN BOOL bAddDate);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsShowPost
// Description:  是否显示公告
// Parameters:   None.
// Return:       显示公告
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsShowPost(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetMacAddr
// Description:  取得MAC地址
// Parameters:   None.
// Return:       MAC地址
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetMacAddr(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInternetBarID
// Description:  取得网吧ID
// Parameters:   None.
// Return:       网吧ID
//-----------------------------------------------------------------------------
DLL_API_EXPORT long _stdcall I8CD_GetInternetBarID(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetChannelsID
// Description:  取得渠道ID
// Parameters:   None.
// Return:       渠道ID
//-----------------------------------------------------------------------------
DLL_API_EXPORT long _stdcall I8CD_GetChannelsID(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetPostInfo
// Description:  取得公告信息
// Parameters:   1) I8CD_PostInfo_st* pstPostInfo [OUT] 公告信息
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_GetPostInfo(OUT I8CD_PostInfo_st* pstPostInfo);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInternetBarInfo
// Description:  取得网吧信息
// Parameters:   1) I8CD_InternetBarInfo_st* pstInternetBarInfo [OUT] 网吧信息
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_GetInternetBarInfo(OUT I8CD_InternetBarInfo_st* pstInternetBarInfo);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsEnableBrowseGameDirectory
// Description:  是否允许浏览游戏目录
// Parameters:   None.
// Return:       允许浏览游戏目录
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsEnableBrowseGameDirectory(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReleaseData
// Description:  释放数据，用于释放由数据层在返回值中生成的数据
// Parameters:   1) void* pData [IN] 要释放的数据指针
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_ReleaseData(IN void* pData);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllGameBaseInfo
// Description:  取得全部游戏基础信息
// Parameters:   None.
// Return:       全部游戏基础信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameBaseInfoArray_st* _stdcall I8CD_GetAllGameBaseInfo(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllIconInfo
// Description:  取得全部图标信息
// Parameters:   None.
// Return:       全部图标信息，需要用ReleaseIconData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_IconInfoArray_st* _stdcall I8CD_GetAllIconInfo(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllNav
// Description:  取得导航栏按钮集
// Parameters:   None.
// Return:       全部导航栏按钮信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_NavArray_st* _stdcall I8CD_GetAllNav(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameBaseInfo
// Description:  取得游戏基础信息
// Parameters:   1) DWORD                 gid             [IN]  游戏ID
//               2) I8CD_GameBaseInfo_st* pstGameBaseInfo [OUT] 游戏基础信息
// Return:       取得成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_GetGameBaseInfo(IN DWORD gid, OUT I8CD_GameBaseInfo_st* pstGameBaseInfo);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameInfo
// Description:  取得游戏信息
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       游戏信息，为空表示游戏不存在，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameInfo_st* _stdcall I8CD_GetGameInfo(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetClientServiceState
// Description:  取得客户端服务状态
// Parameters:   1) BOOL* pbOnline [OUT] 连线中
//               2) BOOL* pbProt   [OUT] 还原保护可用
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_GetClientServiceState(OUT BOOL* pbOnline, OUT BOOL* pbProt);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReportClickInfo
// Description:  向服务器上报点击信息
// Parameters:   1) const I8CD_NavClickInfo_st*    pstNavClickInfo       [IN] 导航栏点击信息
//               2) int                            iNavClickInfoCount    [IN] 导航栏点击信息数量
//               3) const I8CD_GameClickInfo_st*   pstGameClickInfo      [IN] 游戏点击信息
//               4) int                            iGameClickInfoCount   [IN] 游戏点击信息数量
//               5) const I8CD_NavTabClickInfo_st* pstNavTabClickInfo    [IN] 二级菜单点击信息
//               6) int                            iNavTabClickInfoCount [IN] 二级菜单点击信息数量
//               7) BOOL                           bWaitAnswer           [IN] 需要等待回应
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_ReportClickInfo(IN const I8CD_NavClickInfo_st* pstNavClickInfo, IN int iNavClickInfoCount, IN const I8CD_GameClickInfo_st* pstGameClickInfo, IN int iGameClickInfoCount, IN const I8CD_NavTabClickInfo_st* pstNavTabClickInfo, IN int iNavTabClickInfoCount, IN BOOL bWaitAnswer);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetNavTab
// Description:  取得导航栏下属的标签页
// Parameters:   1) I8CD_NavType_en enNavType [IN] 导航栏按钮类型
// Return:       标签页信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_NavTabArray_st* _stdcall I8CD_GetNavTab(IN I8CD_NavType_en enNavType);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_FindGame
// Description:  查找游戏
// Parameters:   1) LPCTSTR             lpszKey   [IN] 关键字,可以为空
//               2) I8CD_FindFilter_st* pstFilter [IN] 筛选器,可以为空
//               3) I8CD_Order_en       enOrder   [IN] 排序方式
// Return:       匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameIDArray_st* _stdcall I8CD_FindGame(IN LPCTSTR lpszKey, IN I8CD_FindFilter_st* pstFilter, IN I8CD_Order_en enOrder);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_FindGameByChar
// Description:  根据字母查找游戏
// Parameters:   1) TCHAR    tKey    [IN] 字母(A-Z)
//               2) Order_en enOrder [IN] 排序方式
// Return:       匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameIDArray_st* _stdcall I8CD_FindGameByChar(IN TCHAR tKey, IN I8CD_Order_en enOrder);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameByClassName
// Description:  根据类别查找游戏
// Parameters:   1) LPCTSTR  lpszClassName [IN] 游戏类别
//               2) Order_en enOrder       [IN] 排序方式
// Return:       匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameIDArray_st* _stdcall I8CD_GetGameByClassName(IN LPCTSTR lpszClassName, IN I8CD_Order_en enOrder);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGameOnToolBar
// Description:  取得工具栏上的游戏
// Parameters:   None.
// Return:       匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameIDArray_st* _stdcall I8CD_GetGameOnToolBar(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckAdvancedPassword
// Description:  检查客户端高级设置密码是否正确
// Parameters:   1) LPCTSTR lpszPassword [IN] 客户端高级设置密码
// Return:       客户端高级设置密码匹配成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_CheckAdvancedPassword(IN LPCTSTR lpszPassword);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetGBKFromString
// Description:  取得汉字国标扩展码
// Parameters:   1) LPTSTR  lpszOutString [OUT] 生成的汉字国标扩展码
//               2) int     iOutMaxCount  [IN]  生成字符串的最大字符数
//               3) LPCTSTR lpszString    [IN]  字符串
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_GetGBKFromString(OUT LPTSTR lpszOutString, IN int iOutMaxCount, IN LPCTSTR lpszString);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetServerIP
// Description:  取得服务器IP
// Parameters:   None.
// Return:       服务器IP
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetServerIP(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetServerIP
// Description:  设置服务器IP
// Parameters:   1) LPCTSTR lpszServerIP [IN] 服务器IP
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_SetServerIP(IN LPCTSTR lpszServerIP);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetOperatorAfterRunGame
// Description:  取得运行游戏后界面执行的操作
// Parameters:   None.
// Return:       运行游戏后界面需要执行的操作
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_OperatorAfterRunGame_en _stdcall I8CD_GetOperatorAfterRunGame(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsInitializeHy
// Description:  还原保护是否可用
// Parameters:   None.
// Return:       还原保护可用
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsInitializeHy(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsInstallHy
// Description:  是否安装还原保护
// Parameters:   None.
// Return:       还原保护已安装
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsInstallHy(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetProtectedDriver
// Description:  取得开启还原保护的磁盘
// Parameters:   None.
// Return:       开启还原保护的磁盘，格式“CDE”
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetProtectedDriver(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetUnProtectedDriver
// Description:  取得未开启还原保护的磁盘
// Parameters:   None.
// Return:       未开启还原保护的磁盘，格式“CDE”
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetUnProtectedDriver(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ProtectDriver
// Description:  对磁盘开启还原保护和关闭还原保护(格式：CDE)
// Parameters:   1) LPCTSTR lpszProtectedDriver   [IN]  需要开启还原保护的磁盘
//               2) LPCTSTR lpszUnProtectedDriver [IN]  需要关闭还原保护的磁盘
//               3) BOOL*   pbNeedRestart         [OUT] 需要重启计算机
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_ProtectDriver(IN LPCTSTR lpszProtectedDriver, IN LPCTSTR lpszUnProtectedDriver, OUT BOOL* pbNeedRestart);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_CheckDisk
// Description:  磁盘检测
// Parameters:   1) LPCTSTR lpszDriver [IN] 需要检测的磁盘，格式“CDE”
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_CheckDisk(IN LPCTSTR lpszDriver);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SaveArchiveToFlashDisk
// Description:  个人优盘存档
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_SaveArchiveToFlashDisk(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ReadArchiveToFlashDisk
// Description:  个人优盘取档
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_ReadArchiveToFlashDisk(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_ShowFlashDisk
// Description:  显示个人优盘
// Parameters:   None.
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_ShowFlashDisk(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_LockComputer
// Description:  锁定计算机
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_LockComputer(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetResolution
// Description:  设置屏幕分辨率
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_SetResolution(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetSysMouse
// Description:  设置系统鼠标
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_SetSysMouse(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_SetVolume
// Description:  设置音量
// Parameters:   None.
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_SetVolume(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_OpenGameDirectory
// Description:  打开游戏所在目录
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_OpenGameDirectory(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsGameEnableRun
// Description:  检查游戏是否可运行，没考虑人为删除游戏文件和游戏版本不是最新的问题
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       游戏可运行
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsGameEnableRun(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetInputMethod
// Description:  取得输入法的游戏ID，返回零表示网吧没有在三层更新中配置输入法
// Parameters:   None.
// Return:       输入法的游戏ID，如果没有返回0
//-----------------------------------------------------------------------------
DLL_API_EXPORT DWORD _stdcall I8CD_GetInputMethod(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_UpdateGame
// Description:  更新游戏
// Parameters:   1) DWORD                   gid               [IN] 游戏ID
//               2) I8CD_DownloadMode_en    enDownloadMode    [IN] 下载方式
//               3) I8CD_UpdateCallBack_pfn pfnUpdateCallBack [IN] 更新回调函数，可以传空
//               4) void*                   pCallBackParam    [IN] 更新回调函数用的参数，可以传空
// Return:       更新任务句柄，无需释放，如果返回值为空表示游戏无需更新，可以直接运行
//-----------------------------------------------------------------------------
DLL_API_EXPORT HANDLE _stdcall I8CD_UpdateGame(IN DWORD gid, IN I8CD_DownloadMode_en enDownloadMode, IN I8CD_UpdateCallBack_pfn pfnUpdateCallBack, IN void* pCallBackParam);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_StopUpdate
// Description:  停止下载
// Parameters:   1) HANDLE hUpdate [IN] 更新任务句柄
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_StopUpdate(IN HANDLE hUpdate);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetPertinentGame
// Description:  取得游戏相关工具
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       匹配的游戏信息，需要用I8CD_ReleaseData释放返回的指针
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_GameIDArray_st* _stdcall I8CD_GetPertinentGame(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RunGame
// Description:  运行游戏，运行游戏前需要"更新游戏"或"检查游戏信息和游戏文件是否匹配"。"更新游戏"要等待更新任务为完成，"检查游戏信息和游戏文件是否匹配"要保证版本一致
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       运行游戏结果
//-----------------------------------------------------------------------------
DLL_API_EXPORT I8CD_RunGameResult_en _stdcall I8CD_RunGame(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetClientMenu
// Description:  取得菜单分级信息，如果没有返回NULL
// Parameters:   None.
// Return:       菜单分级信息，XML数据格式
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetClientMenu(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsVisible
// Description:  检查窗口是否可见，如果不可见，客户端界面层以不可见方式启动，直到回调函数收到显示命令
// Parameters:   None.
// Return:       正常/隐式启动客户端
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsVisible(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RemoteControl
// Description:  启动服务端的远程控制
// Parameters:   None.
// Return:       操作成功
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_RemoteControl(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetAllDriver
// Description:  取得所有物理磁盘
// Parameters:   None.
// Return:       所有物理磁盘，格式“CDE”
//-----------------------------------------------------------------------------
DLL_API_EXPORT LPCTSTR _stdcall I8CD_GetAllDriver(void);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_GetStartupInfo
// Description:  取得菜单启动信息
// Parameters:   1) I8CD_StartupInfo_st* pstStartupInfo [OUT] 菜单启动信息
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_GetStartupInfo(OUT I8CD_StartupInfo_st* pstStartupInfo);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_IsNeedRefreshGameVDisk
// Description:  检测是否需要为游戏刷新虚拟盘
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       需要刷新虚拟盘
//-----------------------------------------------------------------------------
DLL_API_EXPORT BOOL _stdcall I8CD_IsNeedRefreshGameVDisk(IN DWORD gid);
//-----------------------------------------------------------------------------
// FunctionName: I8CD_RefreshGameVDisk
// Description:  为游戏刷新虚拟盘
// Parameters:   1) DWORD gid [IN] 游戏ID
// Return:       None.
//-----------------------------------------------------------------------------
DLL_API_EXPORT void _stdcall I8CD_RefreshGameVDisk(IN DWORD gid);


