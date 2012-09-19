#ifndef _HY_H_
#define _HY_H_

#pragma pack(1)
typedef struct tag_MOUNT_PACK
{
	LONGLONG qwStartSec;
	LONGLONG qwSecs;
	DWORD  dwDiskNumber;
	char  szDosDevice[MAX_PATH];
}MOUNTPACK, *LPMOUNTPACK;
#pragma pack()

//注意所有分区字符必须是大写。数据返回０表示成功，１（其它）表示错误或错误代码。

//第一个参数表示安装时要保护的分区列表。第二个是设置的还原密码。如果不设置密码，则用""空字符串。
//返回０表示成功，非０是错误代码。


//安装驱动，保护指定盘符:
typedef int (WINAPI *PFNINSTALL4DOSCHAR)(LPCSTR szProtectList, LPCSTR szPassword );

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
#define PROT_DRIVER		1
#define UNPROT_DRIVER	3
#define FLAG_REBOOT		0
#define FLAG_IMME		1
typedef int (WINAPI *PFNSETPROTECTAREAFORDOSCHAR)(
	LPCSTR szDosCharList, 
	int iState /*1:表示保护分区，3表示取消保护分区*/,
	int iCmd, //写0
	LPCSTR szPassword, //驱动密码
	int iIsOnTime /*１表示立即生效，０表示重启后生效。对系统盘以及页面文件放在该分区的话，则需要重启后才生效，因些传成0)*/
    );


//得到保护的分区列表。参数是存放数据的缓冲区，以及缓冲区的大小。一般使用２６个字节即可。因为分区是Ａ－Ｚ。
//如果计算机有Ｃ，Ｄ，Ｅ，Ｆ，Ｇ，而且当前保护了Ｄ，Ｆ。则返回在缓冲区里写"DF".
typedef int (WINAPI *PFNGETPROTECTEDDRIVES)(LPSTR pBuffer, int cxBuffer);
																	   
//以下三个函数用于穿透还原。穿透还原的原理是，如果穿透Ｄ盘，则穿透前把Ｄ设置一个虚拟设备，然后向虚拟盘写数据，就穿透了。更新完后，把虚拟盘删除。
//得到虚拟盘的信息，
typedef int (WINAPI *PFNGETMOUNTINFO)( MOUNTPACK *lpMountPack, char *szPassword );

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

#define PROT_DLL_NAME		"wxProt.dll"
#define PROT_PASSWORD		"ST3F95Xfhu9rBqBh"

HMODULE						_hModule = NULL;
PFNINSTALL4DOSCHAR 			pfnInstall4DosChar = NULL;
PFNGETDRIVESTATE			pfnGetDriveState = NULL;
PFNUNINSTALLDRIVER			pfnUnInstallDriver = NULL;
PFNDRIVERISPROTECTED		pfnDriverIsProtected = NULL;	   
PFNCHECKPASSWORD			pfnCheckPassword = NULL;
PFNCHANGEPASSWORD			pfnChangePassword = NULL;
PFNRWAPDATA					pfnRWAPData = NULL;
PFNSETPROTECTAREAFORDOSCHAR pfnProtArea = NULL;
PFNGETPROTECTEDDRIVES		pfnGetProtectDrivers = NULL;
PFNMOUNTPTDOSCHAR			pfnMountDosChar = NULL;				
PFNUMOUNTPTDOSCHAR			pfnUMountDosChar = NULL;

inline bool CheckBeforeOperator()
{
	if (pfnGetDriveState() == 0)
	{
		if (pfnCheckPassword("") == 0)
			pfnChangePassword("", PROT_PASSWORD);
		return true;
	}
	return false;
}
	
inline bool Initialize()
{
	char szModule[MAX_PATH] = {0};
	GetModuleFileName(NULL, szModule, MAX_PATH);
	PathRemoveFileSpec(szModule);
	PathAddBackslash(szModule);
	lstrcat(szModule, PROT_DLL_NAME);
	_hModule = LoadLibrary(szModule);		
	if (_hModule != NULL)
	{
		pfnInstall4DosChar		= (PFNINSTALL4DOSCHAR)GetProcAddress(_hModule, "Install4DosChar");
		pfnGetDriveState		= (PFNGETDRIVESTATE)GetProcAddress(_hModule, "GetDriveState");
		pfnUnInstallDriver		= (PFNUNINSTALLDRIVER)GetProcAddress(_hModule, "UnInstallDriver");
		pfnDriverIsProtected	= (PFNDRIVERISPROTECTED)GetProcAddress(_hModule, "DriverIsProtected");	   
		pfnCheckPassword		= (PFNCHECKPASSWORD)GetProcAddress(_hModule, "CheckPassWord");
		pfnChangePassword		= (PFNCHANGEPASSWORD)GetProcAddress(_hModule, "ChangePassWord");
		pfnRWAPData				= (PFNRWAPDATA)GetProcAddress(_hModule, "RWAPData");
		pfnProtArea				= (PFNSETPROTECTAREAFORDOSCHAR)GetProcAddress(_hModule, "SetProtectAreaForDosChar");
		pfnGetProtectDrivers	= (PFNGETPROTECTEDDRIVES)GetProcAddress(_hModule, "GetProtectedDrives");
		pfnMountDosChar			= (PFNMOUNTPTDOSCHAR)GetProcAddress(_hModule, "MountPtDosChar");
		pfnUMountDosChar		= (PFNUMOUNTPTDOSCHAR)GetProcAddress(_hModule, "UMountPtDosChar");

		if (pfnInstall4DosChar		!= NULL &&
			pfnGetDriveState		!= NULL &&
			pfnUnInstallDriver		!= NULL &&
			pfnDriverIsProtected	!= NULL &&
			pfnCheckPassword		!= NULL && 
			pfnChangePassword		!= NULL &&
			pfnRWAPData				!= NULL &&
			pfnProtArea				!= NULL &&
			pfnGetProtectDrivers	!= NULL &&
			pfnMountDosChar			!= NULL &&
			pfnUMountDosChar		!= NULL )
		return true;
	}
	else
	{
//		SDG_WriteLog("load %s fail:%d", PROT_DLL_NAME, GetLastError());
	}
	return false;
}

inline void UnInitialize()
{
	if (_hModule != NULL)
		FreeLibrary(_hModule);
}

#endif
