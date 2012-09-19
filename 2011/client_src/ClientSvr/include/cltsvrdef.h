#ifndef __i8desk_cltsvrdef_inc__
#define __i8desk_cltsvrdef_inc__

#include <string>
#include "define.h"
#include "Extend STL/UnicodeStl.h"
#include "i8type.hpp"

#define    KEY_I8DESK			      TEXT("SOFTWARE\\Goyoo\\i8desk")
#define    KEY_I8DESK_GID		      TEXT("SOFTWARE\\Goyoo\\i8desk\\gidinfo")
#define    KEY_I8DESK_TASK		      TEXT("SOFTWARE\\Goyoo\\i8desk\\task")

//插件管理器可以接受的命令
enum CliPlugMgrCmd_en
{
	E_CPMC_LOAD_LOCAL_PLUG = 1, //加载本地插件
};

#ifndef MAX_GUID_LEN
#define MAX_GUID_LEN 64
#endif

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN		64
#endif

#ifndef MAX_PARAM_LEN
#define MAX_PARAM_LEN		280
#endif

enum BootTask_RunDate_en
{
    E_BTRD_MONDAY    = 0x1,
    E_BTRD_TUESDAY   = 0x2,
    E_BTRD_WEDNESDAY = 0x4,
    E_BTRD_THURSDAY  = 0x8,
    E_BTRD_FRIDAY    = 0x10,
    E_BTRD_SATURDAY  = 0x20,
    E_BTRD_SUNDAY    = 0x40,
};

enum BootTask_RunType_en
{
    E_BTRT_SERVICE     = 0, //开机运行
    E_BTRT_BARONLINE   = 1, //启动菜单运行
};

//开机任务参数
typedef struct tagSTBootTaskParam
{
    TCHAR szTaskId[MAX_GUID_LEN]; //任务ID
    DWORD dwTaskType;             //任务类型
	DWORD dwFlag;                 //只在安装了还原保护的系统盘上运行
    DWORD dwRunDate;              //执行日期 （星期表示 星期一：1； 星期二：2 ... 星期七：64）他是可以组合的（例如：65 = (星期一和星期七)）
    DWORD dwRunBeginTime;         //起始运行时刻 （HH:MM:SS）
    DWORD dwDelayTime;            //延迟运行时间 （SS）
    DWORD dwRunEndTime;           //终止运行时刻 （HH:MM:SS）
	DWORD dwOutofTime;            //过期时间 （YY:MM:DD:HH:MM:SS）
    DWORD dwRunType;              //运行类型 （开机运行：0；启动菜单运行：1）
    DWORD dwStatus;               //启用状态 （未启用：0； 启用：1）
	TCHAR szAppendParam[2048];    //附加参数
}STBootTaskParam;

//虚拟盘结构
typedef struct tagSTVirtualDiskInfo
{
	TCHAR                              szVDiskId[40];                                       //虚拟盘ID  
	DWORD                              dwServerIP;                                          //服务器IP	
	DWORD                              dwServerPort;                                        //服务器端口
	DWORD                              dwServerDrive;                                       //服务端盘符
	DWORD                              dwClientDrive;                                       //客户端盘符
	DWORD                              dwDiskType;                                          //盘类型
	DWORD                              dwStartType;                                         //刷盘类型
    TCHAR                              szServerId[MAX_PATH];                                //SID
    DWORD                              dwSize;                                              //缓存空间大小
    DWORD                              dwSsdDrv;                                            //SSD盘符大写字母的ASCII码
 
	tagSTVirtualDiskInfo()
	{
		ZeroMemory(this, sizeof(STVirtualDiskInfo));
	}
}STVirtualDiskInfo;

//游戏索引信息
typedef struct tagSTGameIndexInfo
{
	DWORD	            			  dwGameId;                                            //游戏ID
	DWORD                             dwGameIndex;                                         //游戏索引
	DWORD                             dwRunTypeIndex;                                      //运行方式

	tagSTGameIndexInfo()
	{
		dwGameId = dwGameIndex = dwRunTypeIndex = 0;
	}
}STGameIndexInfo;

//游戏信息
typedef struct tagSTGameInfo
{
	DWORD                             dwGameId;                                            //游戏ID
	DWORD                             dwAssociatedId;                                      //关联ID
	TCHAR                             szGameName[MAX_PATH];
	TCHAR                             szClassName[MAX_PATH];                               //类型名字
	TCHAR                             szExeName[MAX_PATH];                                 //运行文件
	TCHAR                             szExeParam[MAX_PATH];                                //运行参数
	DWORD                             dwGameSize;                                          //游戏大小（K）
	DWORD                             dwDesktopLink;                                       //快捷方式
	DWORD                             dwMenuToolBar;                                       //菜单工具栏
	TCHAR                             szServerPath[MAX_PATH];                              //服务端路径
	TCHAR                             szClientPath[MAX_PATH];                              //客户机路径
	DWORD                             dwDeleteFlg;                                          //游戏空间不足时允许删除
	TCHAR                             szFeatureFile[MAX_PATH];                             //特征文件
	TCHAR                             szArchiveInfo[MAX_PATH];                             //存档信息
	DWORD                             dwIDCUpdateTime;                                     //IDC更新时间
	DWORD                             dwIDCVersion;                                        //IDC版本
	DWORD                             dwServerVersion;                                     //服务器版本
	DWORD                             dwUpdate;                                            //是否更新
    DWORD                             dwI8Pay;                                              //增值游戏
	DWORD                             dwIDCClickNum;                                       //IDC点击数
	DWORD                             dwServerClickNum;                                    //服务器点击
	DWORD                             dwRunType;                                           //运行方式
	TCHAR                             szVDiskID[MAX_PATH];                                 //虚拟盘ID
	DWORD                             dwStatus;
	TCHAR                             szComment[MAX_PATH];                                 //游戏评论
	TCHAR                             szVDiskPath[MAX_PATH];                               //虚拟盘路径
    DWORD                             dwRepair;                                            //更新游戏时用CRC比较
	tagSTGameInfo()
	{
		ZeroMemory(this, sizeof(tagSTGameInfo));
	}
}STGameInfo;

//游戏图标信息
typedef struct tagSTIconItem
{
	char                              *pIconData;                                          //图标数据
	DWORD                             dwIconCRC;                                           //图标CRC

	tagSTIconItem()
	{
		pIconData  = NULL;
		dwIconCRC  = 0;
	}
}STIconItem;

//客户端硬件信息
typedef struct tagSTCltHardwareInfo
{
	TCHAR                              szCPU[MAX_PATH];		            		           //CPU信息
	TCHAR                              szMemory[MAX_PATH];		    	                   //内存信息
	TCHAR                              szDisk[MAX_PATH];		    	                       //磁盘信息
	TCHAR                              szSoundCard[MAX_PATH];
	TCHAR                              szGraphics[MAX_PATH];	    	                       //显卡信息
	TCHAR                              szMainboard[MAX_PATH];	    	                   //主板信息
	TCHAR                              szNIC[MAX_PATH];				                       //网卡信息
	TCHAR                              szWebcam[MAX_PATH];		    	                   //摄像头信息

	tagSTCltHardwareInfo()
	{
		ZeroMemory(this, sizeof(tagSTCltHardwareInfo));
	}

	BOOL operator==(const tagSTCltHardwareInfo& rParam) const
	{
		if (lstrcmp(szCPU, rParam.szCPU) != 0
			|| lstrcmp(szMemory, rParam.szMemory) != 0
			|| lstrcmp(szDisk, rParam.szDisk) != 0
			|| lstrcmp(szSoundCard, rParam.szSoundCard) != 0
			|| lstrcmp(szGraphics, rParam.szGraphics) != 0
			|| lstrcmp(szMainboard, rParam.szMainboard) != 0
            || lstrcmp(szNIC, rParam.szNIC) != 0
			|| lstrcmp(szWebcam, rParam.szWebcam) != 0)
		{
			return FALSE;
		}
		return TRUE;
	}
}STCltHardwareInfo;

//客户端系统信息
typedef struct tagSTSysCfgInfo
{
	TCHAR                              szName[MAX_PATH];			       				           //计算机名
	TCHAR                              szOptSystem[MAX_PATH];		    	                       //操作系统
	TCHAR                              szMacAddr[MAX_PATH];			    		                   //MAC地址
	DWORD                              dwIP;    
	DWORD                              dwGate;
	DWORD                              dwDns0;
	DWORD                              dwDns1;
	DWORD                              dwSubMask;

	tagSTSysCfgInfo()
	{
		ZeroMemory(this, sizeof(tagSTSysCfgInfo));
	}

	BOOL operator==(const tagSTSysCfgInfo& rParam) const
	{
		if (lstrcmp(szName, rParam.szName) != 0
			|| lstrcmp(szOptSystem, rParam.szOptSystem) != 0
			|| lstrcmp(szMacAddr, rParam.szMacAddr) != 0
			|| dwIP != rParam.dwIP
			|| dwGate != rParam.dwGate
			|| dwDns0 != rParam.dwDns0
			|| dwDns1 != rParam.dwDns1
			|| dwSubMask != rParam.dwSubMask)
		{
			return FALSE;
		}
		return TRUE;
	}
}STSysCfgInfo;

//软件版本信息
typedef struct tagSTI8Version
{  
    TCHAR                              szProtVer[MAX_PATH];
    TCHAR                              szVirtualDiskVer[MAX_PATH];
    TCHAR                              szCltUIVer[MAX_PATH];
    TCHAR                              szCltDataVer[MAX_PATH];
	TCHAR                              szCltSvrVer[MAX_PATH];

	tagSTI8Version()
	{
		ZeroMemory(this, sizeof(tagSTI8Version));
	}
}STI8Version;

//还原信息
typedef struct tagSTHyInfo
{
	DWORD                              dwInstall;
	TCHAR                              szAllDriver[MAX_PATH];
	TCHAR                              szProtDriver[MAX_PATH];

	tagSTHyInfo()
	{
		ZeroMemory(this, sizeof(tagSTHyInfo));
	}
}STHyInfo;

//服务配置
typedef struct tagSTServerInfo
{
    DWORD                             dwUpdateIP;                                //更新IP
    DWORD                             dwVDiskIPCount;                            //虚拟盘IP数量
	DWORD                             adwVDiskIP[10];                            //虚拟盘IP

	tagSTServerInfo()
	{
		ZeroMemory(this, sizeof(tagSTServerInfo));
	}
}STServerInfo;

typedef struct tagUpGameinfo        //主动推送记录游戏任务信息
{
	DWORD			GID;				//游戏GID
    i8desk::FC_COPY_MODE	Flag;		//游戏的对比方式
	DWORD			Srvip;				//服务器的ip 
	DWORD			speed;				//限制速度值
    DWORD           resv;				//保留字段
    TCHAR           szTaskId[MAX_GUID_LEN]; //开机任务ID
	TCHAR			SVRPATH[MAX_PATH]; //游戏在服务器上的路径
	TCHAR			CLIPATH[MAX_PATH]; //游戏在客户机上的路径
	tagUpGameinfo()
	{
		ZeroMemory(this,sizeof(tagUpGameinfo)); 
	}
}UpGameinfo;


//更新文件信息
typedef struct tagSTUpdateFileInfo
{
	stdex::tString                            strFilePath;                                         //文件路径
	stdex::tString                            strTempFilePath;                                     //临时路径
	stdex::tString                            strRelativePath;                                     //相对路径
	DWORD                                     dwFileCRC;                                           //文件CRC
    tagSTUpdateFileInfo() : dwFileCRC(0)
	{
	}

}STUpdateFileInfo;

//文件信息
typedef struct tagSTUptFileItem
{
	DWORD                             dwFileCRC;                                           //文件CRC
	char                              szFileName[MAX_PATH];                                //文件名称, ASCLL值，方便服务端兼容旧版本

	tagSTUptFileItem()
	{
		ZeroMemory(this, sizeof(tagSTUptFileItem));
	}
}STUptFileItem;

//文件列表
typedef struct tagSTUptFileIdx
{
	DWORD                             dwFileListCRC;                                       //文件列表CRC
	DWORD                             dwVersion;                                           //更新版本号
	DWORD                             dwFileCount;                                         //更新文件数

	tagSTUptFileIdx()
	{
		ZeroMemory(this, sizeof(tagSTUptFileIdx));
	}
}STUptFileIdx;

//快捷方式
typedef struct tagSTShortcut
{
	TCHAR                              szName[MAX_PATH];                                    //名字
	TCHAR                              szTargetPath[MAX_PATH];                              //目标路径
	TCHAR                              szParam[MAX_PATH];                                   //参数
	TCHAR                              szValue[MAX_PATH];                                   //参数值
	TCHAR                              szIconPath[MAX_PATH];                                //图标路径

	tagSTShortcut()
	{
		ZeroMemory(this, sizeof(tagSTShortcut));
	}
}STShortcut;



struct PlugInIndex_st //插件索引
{
    DWORD    gid;                           //工具ID
    DWORD    dwVersion;                     //插件版本
    PlugInIndex_st() : gid(0), dwVersion(0)
    {
    }
};


#define I8_AUTO_TRY       try {
#define I8_AUTO_CATCH     } catch (...) {}
#define I8_NAMESPACE_BEGIN         namespace i8desk{
#define I8_NAMESPACE_END           }


#endif