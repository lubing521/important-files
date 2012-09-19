#pragma once

#include "Extend STL/UnicodeStl.h"

#define PROT_DLL_NAME    TEXT("wxProt.dll")
#define PROT_PASSWORD    "ST3F95Xfhu9rBqBh"

#pragma pack(1)

typedef struct tag_MOUNT_PACK
{
    LONGLONG qwStartSec;
    LONGLONG qwSecs;
    DWORD    dwDiskNumber;
    char     szDosDevice[MAX_PATH];
}MOUNTPACK, *LPMOUNTPACK;

typedef struct tag_ProtectArea_Info
{
    BYTE    bState;
    BYTE    bCmd;
    BYTE    bCmdAtOnce;
    BYTE    bModifyOutside;
    DWORD    dwStartSec;
    DWORD    dwSecs;
    DWORD    dwDiskNumber;

    DWORD    dwBitmapNotUsedBits;
    DWORD    dwBitmapOldUsedBits;
    DWORD    dwBitmapNewUsedBits;
    DWORD    dwBitmapTmpUsedBits;
}PROTECTAREA_INFO, *LPPROTECTAREA_INFO;

#define MAX_PROTECTAREA_NUMS    100
typedef struct tag_PROTECTAREA_PACK
{
    DWORD    dwInOrOut;
    DWORD    dwNums;
    DWORD    dwReseave[2];
    char    szPassword[256];
    PROTECTAREA_INFO    Area[MAX_PROTECTAREA_NUMS];
}PROTECTAREA_PACK, *LPPROTECTAREA_PACK;

typedef struct tag_PtInfo
{
    DWORD    dwStartSec;    //此分区的绝对开始扇区
    DWORD    dwSecs;        //此分区的大小，扇区数
    DWORD    dwUsedSecs;    //此分区已经在使用的扇区数
    BYTE    bFsType;        //此分区的分区类型
    BYTE    bIsMaster;    //此分是否为主分区，1表示主分区，0表示非主分区
    BYTE    bIsActive;    //此分区是否为活动分区，1表示活动分区，0表示为非活动分区
    BYTE    bDosChar;    //此分区的盘符，为盘符的ASCII值，如，C的ASCII值为67
    char    szVolume[256];    //此分区的卷标
}PTINFO, *LPPTINFO;    

#define MAX_PTS     100
typedef struct tag_HDInfo
{
    DWORD    dwHdNumber;        //此硬盘在系统中的设备顺序号
    DWORD    dwSecs;            //此硬盘的大小，扇区数
    DWORD    dwReseave;        //保留
    DWORD    dwPtNums;        //此硬盘的分区个数
    PTINFO    pt[MAX_PTS];    //分区信息数组
}HD, *LPHD;

#pragma pack()

//注意所有分区字符必须是大写。数据返回０表示成功，１（其它）表示错误或错误代码。

//第一个参数表示安装时要保护的分区列表。第二个是设置的还原密码。如果不设置密码，则用""空字符串。
//返回０表示成功，非０是错误代码。

//安装驱动，保护指定盘符:
typedef int (WINAPI *PFNINSTALL4DOSCHAR)(LPCSTR szProtectList, LPCSTR szPassword);

//得到驱动状态：返回０表示驱动己安装，其它表示没有安装驱动。
typedef int (WINAPI *PFNGETDRIVESTATE)();

//卸载驱动
typedef int (WINAPI *PFNUNINSTALLDRIVER)();

//检查分区是否被保护，如果检查Ｃ盘，则参数是"C"。注意不是"C:"或者"C:\"
//０表示保护，１表示没有保护
typedef int (WINAPI *PFNDRIVERISPROTECTED)(LPCSTR pBuffer);

//检验密码：。如果密码为空，用""空节符串。返回０表示密码正确，其它则密码不正确
typedef int (WINAPI *PFNCHECKPASSWORD)(LPCSTR szPassWord );

//更改密码：参数：旧数密码，新密码。
typedef int (WINAPI *PFNCHANGEPASSWORD)(LPCSTR szOldPassWord, LPCSTR szNewPassword );

//向驱动写一块需要保存的数据， 大小0x100 BYTE（这个数据必须是２５６字节的缓冲空间，即应用程序可以写私有数据到驱动里面，也可以读出来）
typedef int (WINAPI *PFNRWAPDATA)(BYTE *lpBuf, BYTE isWrite );//1表示写，０表示读

//设置保护区域和命令，要所盘符列表:返回０表示成功，其它表示错误代码。
#define PROT_DRIVER        1
#define UNPROT_DRIVER    3
#define FLAG_REBOOT        0
#define FLAG_IMME        1
typedef int (WINAPI *PFNSETPROTECTAREAFORDOSCHAR)(
    LPCSTR szDosCharList, //要操作的盘符列表，如“CDEF”
    int iState /*0:保护(手动还原)，1:保护(重启后数据被自动还原)，3:不保护*/,
    int iCmd, //0:无操作，1:还原，2:保存数据（刷新数据到磁盘）
    LPCSTR szPassword, //驱动密码
    int iIsOnTime /*１表示立即生效，０表示重启后生效。对系统盘以及页面文件放在该分区的话，则需要重启后才生效，因些传成0)*/
    );


//得到保护的分区列表。参数是存放数据的缓冲区，以及缓冲区的大小。一般使用２６个字节即可。因为分区是Ａ－Ｚ。
//如果计算机有Ｃ，Ｄ，Ｅ，Ｆ，Ｇ，而且当前保护了Ｄ，Ｆ。则返回在缓冲区里写"DF".
typedef int (WINAPI *PFNGETPROTECTEDDRIVES)(LPSTR pBuffer, int cxBuffer);

//以下三个函数用于穿透还原。穿透还原的原理是，如果穿透Ｄ盘，则穿透前把Ｄ设置一个虚拟设备，然后向虚拟盘写数据，就穿透了。更新完后，把虚拟盘删除。
//得到虚拟盘的信息，
typedef int (WINAPI *PFNGETMOUNTINFO)( MOUNTPACK *lpMountPack, LPCSTR szPassword );

//删除虚拟设备。
typedef int (WINAPI *PFNUMOUNTPTDOSCHAR)(LPCSTR szdevicePath,   LPCSTR szPassword);

//添加虚拟盘。第一个参数是分区字母。比如"D".第二个是虚拟设置名称是自己取的，
//由于我们的虚拟盘要使用分区字母。则我们最好取其它的名字。如I8DESKD,而不取
//D:，如果这样会显示在资源管理器中，第三个参数是密码。
typedef int (WINAPI *PFNMOUNTPTDOSCHAR)(LPCSTR szDosChar, LPCSTR szdevicePath, LPCSTR szPassword);

//例子：穿透Ｄ盘。
//穿透前首先调用MoutPtDosChar（"D", "I8DESKD","password");虚拟一个设备出来。

//向虚拟设备写文件就会直接穿透。\\\\.\\虚拟的设备名称\文件路径。
//CopyFile(“Ｆ:\\boot.ini”, “\\\\.\\I8DESKD\\boot.ini, 0 ).则会把Ｆ盘的这个文件写到Ｄ盘里。                                                                                 
//更新完后删除虚拟设备。
//UMoutPtDosChar("I8DESKD", "password");

typedef int (WINAPI * PFNGETPROTECTAREA)(PROTECTAREA_PACK *lpProtect);

typedef int (WINAPI * PFNGETHDINFO)( LPHD lphd );

typedef int (WINAPI * PFNUPDATEDIR)( char *lpPath );

class CHyOperator
{
public:
    CHyOperator(BOOL bDisableUnloadVDisk = TRUE);
    ~CHyOperator();

    //还原相关操作
public:
    BOOL IsInitialize();
    BOOL IsInstallHy();                                              //还原是否安装    
    BOOL InstallHy(LPCTSTR szProtectList);                            //安装还原    
    BOOL UnInstallHy();                                              //卸载还原
    int  GetHDInfo(LPHD lphd );                                      //获取硬盘信息；
    BOOL IsDriverProtected(LPCTSTR lpszArea);                        //分区是否保护；
    BOOL IsDriverProtected(TCHAR chDriver);
    BOOL ProtectDriver(LPCTSTR lpszArea, BOOL* pbNeedRestart = NULL); //保护分区
    BOOL UnProtectDriver(LPCTSTR lpszArea, BOOL* pbNeedRestart = NULL); //取消分区保护
    void CreateProtDiskService(LPCTSTR lpszSvrName);
    BOOL IsAnyDriverProt();                                          //有磁盘处于还原保护中
    BOOL ShutdownComputer(BOOL bReboot);
    BOOL LoadVirtualDevice(TCHAR drv);
    BOOL UnloadVirtualDevice(TCHAR drv);
    BOOL LoadVirtualDeviceEx(TCHAR drv, LPCTSTR pVirtualDeviceName);
    BOOL UnloadVirtualDeviceEx(TCHAR drv, LPCTSTR pVirtualDeviceName);
    BOOL LoadVDevice(TCHAR drv);
    BOOL UnloadVDevice(TCHAR drv);
    stdex::tString ConvertPath(LPCTSTR lpszPath);
    BOOL GetDiskRoomSize(TCHAR drv, PLARGE_INTEGER pliSize);
    BOOL RefreshDirectory(LPCTSTR dir);
    BOOL RefreshDriver(TCHAR drv);
    BOOL IsVirtualDeviceExist(TCHAR drv);
    BOOL ProtectFile(LPCTSTR lpszFile);
    BOOL DeleteFile(LPCTSTR lpszFile);
    BOOL DeleteDirectory(LPCTSTR pPath);
    BOOL MakeSureDirectoryPathExists(const std::wstring& dir);
protected:
    std::string GetVirtualDeviceName(TCHAR drv);
    std::string GetDriverName(TCHAR drv);
private:        
    
    PFNINSTALL4DOSCHAR                    pfnInstall4DosChar;
    PFNGETDRIVESTATE                   pfnGetDriveState;
    PFNUNINSTALLDRIVER                   pfnUnInstallDriver;
    PFNDRIVERISPROTECTED               pfnDriverIsProtected;       
    PFNCHECKPASSWORD                   pfnCheckPassword;
    PFNCHANGEPASSWORD                   pfnChangePassword;
    PFNRWAPDATA                           pfnRWAPData;
    PFNSETPROTECTAREAFORDOSCHAR        pfnProtArea;
    PFNGETPROTECTEDDRIVES               pfnGetProtectDrivers;
    PFNMOUNTPTDOSCHAR                   pfnMountDosChar;                
    PFNUMOUNTPTDOSCHAR                   pfnUMountDosChar;
    PFNGETMOUNTINFO                       pfnGetMountInfo;
    PFNGETPROTECTAREA                   pfnGetProtectArea;
    PFNGETHDINFO                       pfnGetHDInfo;
    PFNUPDATEDIR                       pfnUpdateDir;
    HMODULE                               m_hModule;
    BOOL                               m_bDisableUnloadVDisk;
};

