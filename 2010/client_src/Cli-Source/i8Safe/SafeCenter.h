#pragma once

#include <fltuser.h>
#include <strsafe.h>
#include <WinSvc.h>
#include <Tlhelp32.h>
#include <Psapi.h>
#include <Iphlpapi.h.>
#include <string>
#include <vector>
#include "Markup.h"
#include "../include/dbengine.h"
using namespace std;

#pragma comment(lib, "fltLib.lib")
#pragma comment(lib, "fltMgr.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Iphlpapi.lib")

#pragma pack(1)
//攻击类型
enum ATTACH_TYPE
{ 
	ATTACH_NONE = 0,			                      // 无攻击行为
	GATEWAY_ARP_QUERY_ATTACH,                         // 伪造网关查询包攻击
	GATEWAY_ARP_REPLY_ATTACH,                         // 伪造网关回应包攻击
	LAN_SAMEIP_ATTACH,		                          // 局域网IP冲突攻击
	WAN_SAMEIP_ATTACH,		                          // 广域网IP冲突攻击
	WRONG_PROTOCOL_ATTACH,	                          // 错误的协议攻击
	UNKNOWN_ATTACH
};

//IP MAC结构
typedef struct  _IP_MAC
{
	DWORD                   Ip;                       //IP地址
	BYTE                    Mac[6];                   //MAC地址，不包含"-"
}IP_MAC, *PIP_MAC;

//攻击信息
typedef struct _ATTACK_INFO
{
    ATTACH_TYPE             AType;                    //攻击类型
	BYTE                    AttackByMac[6];           //攻击源MAC
	BYTE                    AttackByIp[6];            //攻击源IP
}ATTACK_INFO, *PATTACK_INFO;
#pragma pack ()

//工具函数

BOOL InstallDriver(LPCTSTR lpFileName, LPCTSTR lpServiceName);
FARPROC WINAPI GetArpModProcAddress(int nIndex);

//初始化ARP的回调函数类型
typedef void (__stdcall *ARP_ATTACK_CALLBACK)(PATTACK_INFO AttackInfo, BOOL IsKnownIP);

//动态加载

//ARP防御导出接口类型定义
//typedef BOOL (__stdcall *PFNINITARPMOD)(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac);
//typedef BOOL (__stdcall *PFNARPMODISOK)();
//typedef BOOL (__stdcall *PFNSTARTPROTECT)();
//typedef BOOL (__stdcall *PFNDISABLESIGN)();
//typedef BOOL (__stdcall *PFNENABLESIGN)();
//typedef BOOL (__stdcall *PFNINSTALLDRIVER)();
//typedef BOOL (__stdcall *PFNREMOVEDRIVER)();
//typedef BOOL (__stdcall *PFNGETGATEWAYMAC)(DWORD GatewayAddr, char MacAddr[]);
//typedef BOOL (__stdcall *PFNSTOPPROTECT)();
//
////ARP防御接口调用
//BOOL __stdcall InitArpMod(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac);
//BOOL __stdcall ArpModIsOK();
//BOOL __stdcall StartProtect();
//void __stdcall DisableSign();
//void __stdcall EnableSign();
//BOOL __stdcall InstallDriver();
//BOOL __stdcall RemoveDriver();
//BOOL __stdcall GetGatewayMac(DWORD GatewayAddr, char MacAddr[]);
//BOOL __stdcall StopProtect();
//void __stdcall UnInitArpMod();

//静态加载
__declspec(dllimport) BOOL __stdcall InitArpMod(ARP_ATTACK_CALLBACK pfnAttackCallback, PIP_MAC LocalIpMac, DWORD Count, PIP_MAC GatewayIpMac);
__declspec(dllimport) BOOL __stdcall ArpModIsOK();
__declspec(dllimport) BOOL __stdcall StartProtect();
__declspec(dllimport) void __stdcall DisableSign();
__declspec(dllimport) void __stdcall EnableSign();
__declspec(dllimport) BOOL __stdcall InstallDriver();
__declspec(dllimport) BOOL __stdcall RemoveDriver();
__declspec(dllimport) BOOL __stdcall StopProtect();
__declspec(dllimport) BOOL __stdcall GetGatewayMac(DWORD GatewayAddr, char* pMac);

#define KeyCAD                       TEXT("Ctrl + Alt + Del")
#define KeyAT                        TEXT("Alt + Tab")
#define KeyWin                       TEXT("Win")
#define KeyCE                        TEXT("Ctrl + Esc");

//进程，注册表保护
interface  IGuardEngine
{
	//
	// 连接到保护引擎，安全认证
	//
	virtual bool __stdcall  Connect(DWORD Seed) = 0;

	//
	// 保护调用者进程不被打开，不被杀掉
	//
	virtual bool __stdcall  ProtectMe() = 0;

	//
	// 检查驱动是否工作正常
	//
	virtual DWORD __stdcall Subsist() = 0;

	//
	// 结束指定进程
	//
	virtual bool __stdcall  TerminateProcess(DWORD Pid) = 0;

	//
	// 心跳接口，万象客户端专用接口，驱动在一定时间内没有收到心跳时重启电脑
	//
	virtual bool __stdcall  ClientLiveIt() = 0;

	//
	// 关闭心跳检查
	//
	virtual bool __stdcall  DisableLiveCheck() = 0;

	//
	// 通知驱动不进行心跳检查
	//
	virtual bool __stdcall  RebootNotify() = 0;

	//
	// 重启系统
	//
	virtual bool __stdcall  Reboot() = 0;

	//
	// 注册表保护，控制指定位置的注册表键值是否可写
	//
	virtual bool __stdcall  RegKeyCanWrite(bool CanWrite, HKEY RootKey,	LPCTSTR Key) = 0;

	//
	// 屏蔽按键 
	//
	//virtual bool __stdcall  DisableKey(tstring Keys[]) = 0;

	//
	// 设置数据, 注意：数据块大小必须在2Kb以内
	//
	virtual bool __stdcall  SetData(TCHAR Data[], DWORD DataLength) = 0;

	//
	// 获取数据, 注意: 数据块大小必须在2Kb以内
	//
	virtual bool __stdcall  GetData(TCHAR Data[], DWORD DataLength) = 0;        
};

//
// 得到保护引擎的一个实例对象
//
IGuardEngine* __stdcall GuardEngineInstance();

//
// 获取一个安全认证种子，用于连接到保护引擎的时候
//
DWORD __stdcall GetAuthSeed();

class TGuardEngine : public IGuardEngine
{
public:
	TGuardEngine(){ GEHandle = INVALID_HANDLE_VALUE;}
	virtual ~TGuardEngine()
	{ 
		if (GEHandle != INVALID_HANDLE_VALUE)
	    {
			CloseHandle(GEHandle);
			GEHandle = INVALID_HANDLE_VALUE;
	    }
	}

public:
	//
	// 连接到保护引擎，安全认证
	//
	bool __stdcall  Connect(DWORD Seed);

	//
	// 保护调用者进程不被打开，不被杀掉
	//
	bool __stdcall  ProtectMe();

	//
	// 检查驱动是否工作正常
	//
	DWORD __stdcall Subsist();

	//
	// 结束指定进程
	//
	bool __stdcall  TerminateProcess(DWORD Pid);

	//
	// 心跳接口，万象客户端专用接口，驱动在一定时间内没有收到心跳时重启电脑
	//
	bool __stdcall  ClientLiveIt();

	//
	// 关闭心跳检查
	//
	bool __stdcall  DisableLiveCheck();

	//
	// 通知驱动不进行心跳检查
	//
	bool __stdcall  RebootNotify();

	//
	// 重启系统
	//
	bool __stdcall  Reboot();

	//
	// 注册表保护，控制指定位置的注册表键值是否可写
	//
	bool __stdcall  RegKeyCanWrite(bool CanWrite, HKEY RootKey,	LPCTSTR Key);

	//
	// 屏蔽按键 
	//
	//bool __stdcall  DisableKey(tstring Keys[]);

	//
	// 设置数据, 注意：数据块大小必须在2Kb以内
	//
	bool __stdcall  SetData(TCHAR Data[], DWORD DataLength);

	//
	// 获取数据, 注意: 数据块大小必须在2Kb以内
	//
	bool __stdcall  GetData(TCHAR Data[], DWORD DataLength);        

protected:
	HANDLE                     GEHandle;
};

#pragma pack(1)
typedef struct TParams
{
	DWORD                      CtlCode;
	DWORD                      InputBufferLength;
	void                       *InputBuffer;
	DWORD                      OutputBufferLength;
	void                       *OutputBuffer;
	DWORD                      *BytesReturned;
}*PParams;

typedef struct TProtSetting
{
	DWORD                      ProtMaskCode;
}*PProtSetting;

typedef struct TRegKeyProt 
{
	DWORD                      CanWrite;
	char                       RegKey[MAX_PATH];
}PRegKeyProt;

#pragma pack()
#define SID_REVISION           (1)
#define METHOD_BUFFERED         0 
#define METHOD_IN_DIRECT        1 
#define METHOD_OUT_DIRECT       2 
#define METHOD_NEITHER          3 

#define FILE_ANY_ACCESS         0 

#define IOCTL_INIT              0x00010001
#define IOCTL_PROT_ME           0x00010002
#define IOCTL_DRV_SUBSIST       0x00010003
#define IOCTL_GET_LOG           0x00010004
#define IOCTL_ADD_LOG           0x00010006

#define IOCTL_END_PROCESS       0x00010007
#define IOCTL_CHECK_LIVE_IT     0x00010008
#define IOCTL_REBOOT            0x00010009

#define IOCTL_REGKEY_CAN_WRITE  0x00010010
#define IOCTL_NOTIFY_SHUTDOWN   0x00010011

#define IOCTL_SET_DATA          0x00020001
#define IOCTL_GET_DATA          0x00020002

#define IOCTL_DISABLE_CHECK_LIVE_IT 0x00030001

#define PROT_INITED             0x00000001
#define PROT_MASK_PROCESS       0x00000002

#define ICAFE_GUARD_TYPE        50000


//DWORD __stdcall CTL_CODE( WORD DeviceType, WORD Function, WORD Method, WORD Access );
IGuardEngine* __stdcall GuardEngineInstance();
DWORD __stdcall GetAuthSeed();

void FreeTokenInformation(void* Buffer);
void QueryTokenInformation(HANDLE Token, TOKEN_INFORMATION_CLASS InformationClass, void** Buffer);
char* SIDToString(PSID ASID);
string RegKeyToKernelRegKey( HKEY RootKey, LPCTSTR Key);

//文件保护
#define STATUS_NOPROTECT_ACTIVE        0x00000000; // 没有保护生效
#define STATUS_FILEPROTECT_ACTIVE      0x00000001; // 文件保护有效
#define STATUS_FOLDERPROTECT_ACTIVE    0x00000002; // 文件夹保护有效
#define STATUS_TIMEPROTECT_ACTIVE      0x00000004; // 时间保护有效
#define STATUS_PROCESSPROTECT_ACTIVE   0x00000008; // 进程保护有效

#define FILE_ATTR_DENY_ALL             0x00000001; // 拒绝访问
#define FILE_ATTR_READ_ONLY            0x00000002; // 只读
#define FILE_ATTR_DENY_DELETE          0x00000004; // 禁止删除
#define FILE_ATTR_HIDE                 0x00000008; // 隐藏

#pragma pack(1)
enum TSniperCommand
{
	cmdAddRuleFile,
	cmdAddRuleFolder,
	cmdAddRuleProcess,

	cmdFileProtStart,
	cmdFileProtStop,

	cmdFolderProtStart,
	cmdFolderProtStop,

	cmdProcessProtStart,
	cmdProcessProtStop,

	cmdClearRuleFile,
	cmdClearRuleFolder,
	cmdClearRuleProcess,

	cmdAddTrustedApp,

	cmdClearTrustedApp,

	cmdEnableTimeProt,
	cmdDisableTimeProt,

	cmdSetTime,

	cmdTerminateProcess,

	cmdGetProcesses, // 获取进程列表
	cmdGetDrivers,   // 获取驱动列表

	cmdGetStatus
};

enum TSniperRuleType
{
	RuleTypeFile,
	RuleTypeFolder,
	RuleTypeTrustedApp,
	RuleTypeProcess,
	RULE_COUNT
};

enum TSniperFilenameType
{
	FileNameTypeFull,     // 绝对路径，完整路径
	FileNameTypeRelative, // 相对路径
	FileNameTypeOnly      // 只有文件名
};

enum TSniperFolderType
{
	FolderNameTypeFull,
	FolderNameTypeOnly
};

typedef struct TRuleFile
{
    char                   FileName[MAX_PATH * 2];
    TSniperFilenameType    FileNameType;
    DWORD                  Attributes;
}*PRuleFile;

typedef struct TRuleFolder
{
	char                   FolderName[MAX_PATH * 2];
	TSniperFolderType      FolderNameType;
	DWORD                  Attributes;
}*PRuleFolder;

typedef struct TRuleTrustedApp
{
	char                   ImageName[MAX_PATH];
}*PRuleTrustedApp;

typedef struct TRuleProcess
{
	char                   ImageName[MAX_PATH];
	TSniperFilenameType    FileNameType;
}*PRuleProcess;

typedef struct TCommandMessage 
{
    TSniperCommand         Command;
    DWORD                  Reserved;
    char                   Data[1];
}*PCommandMessage;

typedef struct TRuleDetail
{
	union 
	{
	    TRuleFile          RuleFile ;
	    TRuleFolder        RuleFolder;
	    TRuleTrustedApp    RuleTrustedApp;
	    TRuleProcess       RuleProcessProt;
	};
}*PRuleDetail;

typedef struct TDriverInfoEntry
{
    DWORD                  NextEntry;
    TCHAR                  BaseName[MAX_PATH];
    TCHAR                  FullName[MAX_PATH];
    DWORD                  DllBase;
    DWORD                  SizeOfImage;
}*PDriverInfoEntry;

typedef struct TProcessInfoEntry
{
    DWORD                  NextEntry;
    DWORD                  Pid;
    char                   ImageName[16];
    char                   CommandLine[260];
}*PProcessInfoEntry;

#pragma pack()

interface IISecEngine
{
	//
	// 连接到保护引擎，安全认证
	//
	virtual bool __stdcall Connect(DWORD Seed) = 0;

	//
	// 是否连接到保护引擎
	//
	virtual bool __stdcall IsConnected() = 0;

	//
	// 文件保护停止
	//
	virtual bool __stdcall FileProtStop() = 0;

	//
	// 文件保护继续
	//
	virtual bool __stdcall FileProtResume() = 0;

	//
	// 文件夹保护停止
	//
	virtual bool __stdcall FolderProtStop() = 0;

	//
	// 文件夹保护继续
	//
	virtual bool __stdcall FolderProtResume() = 0;

	//
	// 进程保护停止
	//
	virtual bool __stdcall ProcessProtStop() = 0;

	//
	// 进程保护继续
	//
	virtual bool __stdcall ProcessProtResume() = 0;

	//
	// 加入一个文件保护规则
	//
	virtual bool __stdcall AddRuleFile(const string FileName, DWORD ProtectAttributes) = 0;

	//
	// 加入一个文件夹保护规则
	//
	virtual bool __stdcall AddRuleFolder(const string FolderName, DWORD ProtectAttributes) = 0;

	//
	// 加入进程保护规则
	//
	virtual bool __stdcall AddRuleProcess(const string ProcessName) = 0;

	//
	// 加入一个受信任的应用程序
	//
	virtual bool __stdcall AddTrustedApp(const string FileName) = 0;

	//
	// 清除文件保护规则
	//
	virtual bool __stdcall ClearRuleFile() = 0;

	//
	// 清除文件夹保护规则
	//
	virtual bool __stdcall ClearRuleFolder() = 0;

	//
	// 清除进程保护规则
	//
	virtual bool __stdcall ClearRuleProcess() = 0;

	//
	// 清除信任程序
	//
	virtual bool __stdcall ClearTrustedApp() = 0;

	//
	// 开启时间保护
	//
	virtual bool __stdcall EnabledTimeProtect() = 0;

	//
	// 关闭时间保护
	//
	virtual bool __stdcall DisableTimeProtect() = 0;

	//
	// 修改日期时间
	//
	virtual bool __stdcall SetDateTime(SYSTEMTIME sysDateTime ) = 0;

	//
	// 结束指定进程
	//
	virtual bool __stdcall TerminateProcess(DWORD Pid) = 0;

	//
	// 获取驱动列表
	//
	virtual bool __stdcall EnumDrivers(PDriverInfoEntry DriversInfo,
                           DWORD DriversInfoLength, DWORD &ReturnLength) = 0;

	//
	// 获取进程列表
	virtual bool __stdcall EnumProcesses(PProcessInfoEntry ProcessInfo,
                           DWORD ProcessInfoLength, DWORD &ReturnLength) = 0;

	//
	//
	// 取得当前的状态, 返回值可能为多个状态值的组合
	//
	virtual DWORD __stdcall GetStatus() = 0;
};

class TISecEngine : public IISecEngine
{
public:
	TISecEngine(){ Port = INVALID_HANDLE_VALUE;}
	virtual ~TISecEngine()
	{
		if (Port != INVALID_HANDLE_VALUE)
		{
			CloseHandle(Port);
			Port = INVALID_HANDLE_VALUE;
		}
	}

protected:
	HANDLE                  Port;

private:
	string __stdcall   FileNameToNtName(const string FileName);
	bool __stdcall     IsFullPath(const string FileName);
	bool __stdcall     IsRelativePath(const string FileName);
public:
	//
	// 连接到保护引擎，安全认证
	//
	bool __stdcall Connect(DWORD Seed);

	//
	// 是否连接到保护引擎
	//
	bool __stdcall IsConnected();

	//
	// 文件保护停止
	//
	bool __stdcall FileProtStop();

	//
	// 文件保护继续
	//
	bool __stdcall FileProtResume();

	//
	// 文件夹保护停止
	//
	bool __stdcall FolderProtStop();

	//
	// 文件夹保护继续
	//
	bool __stdcall FolderProtResume();

	//
	// 进程保护停止
	//
	bool __stdcall ProcessProtStop();

	//
	// 进程保护继续
	//
	bool __stdcall ProcessProtResume();

	//
	// 加入一个文件保护规则
	//
	bool __stdcall AddRuleFile(const string FileName, DWORD ProtectAttributes);

	//
	// 加入一个文件夹保护规则
	//
	bool __stdcall AddRuleFolder(const string FolderName, DWORD ProtectAttributes);

	//
	// 加入进程保护规则
	//
	bool __stdcall AddRuleProcess(const string ProcessName);

	//
	// 加入一个受信任的应用程序
	//
	bool __stdcall AddTrustedApp(const string FileName);

	//
	// 清除文件保护规则
	//
	bool __stdcall ClearRuleFile();

	//
	// 清除文件夹保护规则
	//
	bool __stdcall ClearRuleFolder();

	//
	// 清除进程保护规则
	//
	bool __stdcall ClearRuleProcess();

	//
	// 清除信任程序
	//
	bool __stdcall ClearTrustedApp();

	//
	// 开启时间保护
	//
	bool __stdcall EnabledTimeProtect();

	//
	// 关闭时间保护
	//
	bool __stdcall DisableTimeProtect();

	//
	// 修改日期时间
	//
	bool __stdcall SetDateTime(SYSTEMTIME sysDateTime );

	//
	// 结束指定进程
	//
	bool __stdcall TerminateProcess(DWORD Pid);

	//
	// 获取驱动列表
	//
	bool __stdcall EnumDrivers(PDriverInfoEntry DriversInfo,
		DWORD DriversInfoLength, DWORD &ReturnLength);

	//
	// 获取进程列表
	bool __stdcall EnumProcesses(PProcessInfoEntry ProcessInfo,
		DWORD ProcessInfoLength, DWORD &ReturnLength);

	//
	//
	// 取得当前的状态, 返回值可能为多个状态值的组合
	//
	DWORD __stdcall GetStatus();
};
IISecEngine* ISecEngineInstance();

//DWORD        GetAuthSeed()
//{
//    DWORD Result = GetCurrentProcessId();
//    Result = (Result << 3);
//    Result = (Result ^ 0x65438088);
//	return Result;
//}


// 窗口查杀，进程查杀
#define    POOLINGTIME  1 * 1000 
#pragma pack(1)
enum TOperType
{
	uAdd,
	uDel,
	uClear
};
enum TDataType
{
	uProc,
	uWindows
};

typedef struct TMsgBuffer
{
    TOperType    OperType;
    TDataType    DataType;
    char*        Buffer;
    int          BufferLen;
    TMsgBuffer*  Next;
}*PMsgBuffer;

typedef struct TProhiProc 
{
	char    ProcessName[50];
	char    ProcessPath[1000];
}*PProhiProc;

typedef struct TProhiWindows 
{
	char    WindowsName[50];
}*PProhiWindows;

typedef struct TProcessInfo
{
    HANDLE  ProcessHandle;
    string  ProcessName;
    string  ProcessFillName;
}*PProcessInfo;

typedef struct TWindowsInfo
{
    char  WindowsName[MAX_PATH];
}*PWindowsInfo;
#pragma pack()

//销毁窗口
void DisposeWindow(char* Buf);
//销毁进程
void DisposeProc(char* Buf);

//销毁进程链表
void DisposeProcList(char* Buf);
BOOL CALLBACK EnumWindowsFunc(HWND Handle, LPARAM Param);
void DisposeWinInfo(char* Buf);


class TProhiThread
{
public:
	TProhiThread();
	virtual~ TProhiThread();

private:
	void AddInfo(TDataType DataType, char* Buffer, int Bufferlen);
	void DelInfo(TDataType DataType, char* Buffer, int Bufferlen);
	void Clear(TDataType DataType);
	void PollingTask();
	void AddQueue(TOperType OperType, TDataType DataType, char* Data, int Datalen);
	void GetExeNameList(vector<PProcessInfo>& ProcessName);
	void CloseWindows();
	void CloseProc();

public:
	vector<PWindowsInfo> m_WndList;
	static UINT __stdcall ExecuteWorkThread(LPVOID lpVoid);
	void Execute();
	void Start();
	void Stop();

public:
	HANDLE	   m_hThread;
    HANDLE     FEvent;
    CRITICAL_SECTION FCS;
	PMsgBuffer FFirst;
    PMsgBuffer FLast;
    DWORD      FTick;
	vector<PProhiProc> FProhiProcList;
    vector<PProhiWindows> FProhiWindowsList; //禁止进程和窗口列表

public:
	void ClearData(TDataType DataType);
	void AddData(TDataType DataType, string Name, string Path);
	void DelData(TDataType DataType, string Name, string Path);
};


void __stdcall Arp_Attck_Callback(PATTACK_INFO AttackInfo, BOOL IsKnownIP);
//安全中心类
class CSafeCenter
{
	//构造/析构
public:
	CSafeCenter();
	virtual ~CSafeCenter();

public:
	//读取配置文件
	BOOL		ReadConfig(LPCTSTR lpFilePath);
	//启动安全中心
	BOOL        Start();
	//停止安全中心
	BOOL        Stop();
	//是否启用
	BOOL        IsRun(){return m_bIsStart;}

protected:
	//开启ARP防御
	BOOL        StartArpProtect();
	//关闭ARP防御
	BOOL        StopArpProtect();
	//启动窗口进程查杀
	BOOL        StartWndProcDetect();
	//关闭窗口进程查杀
	BOOL        StopWndProcDetect();

private:
	BOOL        GetLocalIpMac(IP_MAC LocalAddr[], int &iCount);
	BOOL        GetGatewayAddr();
	BOOL        ParserMacToByte(string strMac, BYTE cbMac[]);
private:
	//窗口进程查杀类
    TProhiThread                 m_KillWndProc;
    //是否启动ARP防御
    BOOL                         m_bIsStartARP;
	BOOL                         m_bIsStart;
	BOOL                         m_bIsAutoGetGateAddr;
	//网关IP
	IP_MAC                       m_GateAddr;
};

