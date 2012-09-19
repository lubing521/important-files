// BootRun.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "resource.h"
#include <Shlwapi.h>
#include <process.h>
#include <time.h>
#include <Userenv.h>
#include <winioctl.h>
#include <tchar.h>
#include <fstream>
#include <wbemidl.h>
#include <Userenv.h>
#include <Psapi.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlstr.h>
#include <dshow.h>
#include <comutil.h>
#pragma comment(lib, "strmiids.lib ")
#pragma comment(lib, "comsuppw.lib")
#pragma warning(disable:4200)
#pragma comment(lib, "version.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Wbemuuid") 
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
using namespace std;

#include "BootRun.h"
#include "XUnzip.h"
#include "tool.h"
#include "update.h"
#include "MD5.h"
#include "PlatformPlug.h"

#import "winhttp.dll"

#define PLUG_VERSION		0x01000000L
#define PLUG_COMPANY		TEXT("Goyoo")
#define PLUG_DESCRIPTION	TEXT("BootRun Plug")
#define PLUG_PORT			0

#define KEY_I8DESK			TEXT("SOFTWARE\\Goyoo\\i8desk")
#define KEY_I8DESK_GID		TEXT("SOFTWARE\\Goyoo\\i8desk\\gidinfo")
#define KEY_I8DESK_TASK		TEXT("SOFTWARE\\Goyoo\\i8desk\\task")
#define PROTECTFILE			TEXT("drivers\\wxProt.sys")		//还原驱动名
#define I8DESKGAMEMENU		TEXT("BarOnline.exe")			//游戏菜单
#define I8DESKSafe			TEXT("SafeClt\\KHUpdate.exe")	//桌面图标
#define KHSTARTNAME         TEXT("KHUpdate.exe")            //康和安全中心更新文件
#define VDiskBufVer			TEXT("VDiskBufVer")//gameclient.ini's section
#define MAX_CLIENT_HARDWARE_INFO_LEN   255                             //客户端硬件信息最大长度

#define FRAME_CLASSNAME		TEXT("DeskXFrame")
static int WM_REFRESH_DATA	= RegisterWindowMessage(TEXT("_i8desk_refresh_data"));

namespace i8desk
{
	static DWORD Crc32Table[256] = {
		0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
		0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
		0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
		0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
		0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
		0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
		0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
		0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
		0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
		0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
		0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
		0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
		0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
		0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
		0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
		0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
		0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
		0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
		0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
		0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
		0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
		0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
		0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
		0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
		0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
		0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
		0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
		0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
		0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
		0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
		0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
		0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
		0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
		0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
		0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
		0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
		0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
		0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
		0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
		0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
		0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
		0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
		0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
		0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
		0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
		0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
		0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
		0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
		0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
		0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
		0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
		0x2d02ef8dL
	};

	static DWORD CalBufCRC32(BYTE* buffer, DWORD dwSize)
	{
		ULONG  crc(0xffffffff);
		int len;
		len = dwSize;
		while(len--)
			crc = (crc >> 8) ^ Crc32Table[(crc & 0xFF) ^ *buffer++];
		return crc^0xffffffff;
	}

	//第三方插件调用函数定义;
	typedef DWORD (WINAPI *PFNUPDATEEXEC)(LPCSTR ChannelID);

	//虚拟磁盘刷盘的接口函数定义
	typedef struct tagVDisk//虚拟磁盘对象定义
	{
		char  VID[40];	//VID是GUID字符串（３８个字符）
		DWORD IP;
		WORD  Port;
		char  SvrDrv;
		char  CliDrv;
		DWORD SerialNo;
		DWORD Type;
		DWORD StartType;
		char  SID[MAX_PATH];
		BOOL  bExist;		
	}tagVDisk;	
	typedef DWORD (WINAPI *PFNDISKMOUNT)(UINT index, ULONG addr,USHORT port,CHAR driver,LPCSTR tempfile);
	typedef DWORD (WINAPI *PFNDISKUMOUNT)(char driver);

	//游戏索引定义
	typedef struct tagST_GAME_INDEX_INFO
	{
		DWORD					dwGameId;                   //游戏ID
		DWORD                   dwGameIndex;                //游戏索引
		DWORD                   dwRunTypeIndex;             //运行方式索引

		tagST_GAME_INDEX_INFO()
		{
			dwGameId       = 0;
			dwGameIndex    = 0;
			dwRunTypeIndex = 0;
		}
	}ST_GAME_INDEX_INFO, *PST_GAME_INDEX_INFO;

	//游戏信息对象定义
	typedef struct tagGameInfo
	{
		DWORD	GID;					//游戏GID
		DWORD	PID;					//游戏关联的GID
		char	NAME[MAX_PATH];			//游戏名
		char	IDCCLASS[MAX_PATH];		//游戏类别
		char	GAMEEXE[MAX_PATH];		//游戏官方EXE
		char	PARAM[MAX_PATH];		//游戏运行参数
		DWORD	SIZE;					//游戏大小,以K为单位
		DWORD	DESKLNK;				//游戏是否显示到桌面快捷方式
		DWORD	TOOLBAR;				//游戏是否显示到菜单的工具栏上
		char	SVRPATH[MAX_PATH];		//游戏在服务器上的路径
		char	VDPATH[MAX_PATH];		//游戏在客户端的虚拟盘路径
		char	CLIPATH[MAX_PATH];		//游戏在客户机上的路径
		DWORD	PRIORITY;				//游戏等级，用于删除点击率低的游戏，三层会需要
		DWORD	SHOWPRIORITY;			//游戏在菜单上的显示优先级
		char	MATCHFILE[MAX_PATH];	//游戏自动搜索特征文件
		char	SAVEFILTER[MAX_PATH];	//游戏存档信息
		DWORD	IDCUPTDATE;				//游戏在中心服务器的更新时间
		DWORD	SVRUPTDATE;				//游戏在网吧服务器的更新时间
		DWORD	IDCVER;					//游戏在中心服务器上的版本
		DWORD	SVRVER;					//游戏在网吧服务器上的版本
		DWORD	AUTOUPT;				//游戏是否自动从服务器上更新
		DWORD	I8PLAY;					//游戏需要强制更新
		DWORD	IDCCLICK;				//游戏全网点率
		DWORD	SVRCLICK;				//游戏网吧点率
		DWORD	ADDDATE;				//游戏添加时间
		char	GAMELINK[MAX_PATH];		//游戏官网
		DWORD	RUNTYPE;				//游戏运行方式
		DWORD	IFUPDATE;				//游戏是否更新
		DWORD	IFDISPLAY;				//游戏是否显示
		char	VID[MAX_PATH];			//游戏所在虚拟盘ID
		char    COMMENT[MAX_PATH];		//Comment
		DWORD	STATUS;					//游戏下载状态
		char    GAMEAREA[MAX_PATH];    //专区地址
		char    REGURL[MAX_PATH];      //注册地址
		char    PAYURL[MAX_PATH];      //充值地址
		char    VIDEOAREA[MAX_PATH];   //视频专区
		tagGameInfo()
		{
			GID	=0;//游戏GID
			PID=0;//游戏关联的GID
			ZeroMemory(NAME,   sizeof(MAX_PATH));   
			ZeroMemory(IDCCLASS,   sizeof(MAX_PATH));
			ZeroMemory(GAMEEXE,   sizeof(MAX_PATH));
			ZeroMemory(PARAM,   sizeof(MAX_PATH));
			SIZE=0;//游戏大小,以K为单位
			DESKLNK=0;//游戏是否显示到桌面快捷方式
			TOOLBAR=0;//游戏是否显示到菜单的工具栏上
			ZeroMemory(SVRPATH,   sizeof(MAX_PATH));
			ZeroMemory(VDPATH,   sizeof(MAX_PATH));
			ZeroMemory(CLIPATH,   sizeof(MAX_PATH));
			PRIORITY=0;//游戏等级，用于删除点击率低的游戏，三层会需要
			SHOWPRIORITY=0;//游戏在菜单上的显示优先级
			ZeroMemory(MATCHFILE,   sizeof(MAX_PATH));
			ZeroMemory(SAVEFILTER,   sizeof(MAX_PATH));
			IDCUPTDATE=0;//游戏在中心服务器的更新时间
			SVRUPTDATE=0;//游戏在网吧服务器的更新时间
			IDCVER=0;//游戏在中心服务器上的版本
			SVRVER=0;//游戏在网吧服务器上的版本
			AUTOUPT=0;//游戏是否自动从服务器上更新
			I8PLAY=0;//游戏需要强制更新
			IDCCLICK=0;//游戏全网点率
			SVRCLICK=0;//游戏网吧点率
			ADDDATE=0;//游戏添加时间
			ZeroMemory(GAMELINK,   sizeof(MAX_PATH));
			RUNTYPE=0;//游戏运行方式
			IFUPDATE=0;//游戏是否更新
			IFDISPLAY=0;//游戏是否显示
			ZeroMemory(VID,   sizeof(MAX_PATH));
			STATUS=0;//游戏下载状态
			ZeroMemory(COMMENT,sizeof(MAX_PATH));
			ZeroMemory(GAMEAREA,sizeof(MAX_PATH));
			ZeroMemory(REGURL,sizeof(MAX_PATH));
			ZeroMemory(PAYURL,sizeof(MAX_PATH));
			ZeroMemory(VIDEOAREA,sizeof(MAX_PATH));
		}
	}tagGameInfo;

	//游戏图标信息
	typedef struct tagIconItem
	{
		char* pData;
		DWORD dwSize;
	}tagIconItem;
	//游戏图标信息
	typedef struct tagST_ICON_ITEM
	{
		char                      *pIconData;                       //图标数据
		DWORD                     dwCRC32Data;                      //CRC32校验码

		tagST_ICON_ITEM()
		{
			pIconData   = NULL;
			dwCRC32Data = (DWORD)0;
		}		
	}ST_ICON_ITEM, *PST_ICON_ITEM;

	//客户端硬件信息
	typedef struct tagST_CLIENT_HARDWARE_INFO
	{
		char                      szName[MAX_CLIENT_HARDWARE_INFO_LEN];							     //计算机名
		char                      szMacAddr[MAX_CLIENT_HARDWARE_INFO_LEN];					         //MAC地址
		char                      szCPU[MAX_CLIENT_HARDWARE_INFO_LEN];		            		     //CPU信息
		char                      szMemory[MAX_CLIENT_HARDWARE_INFO_LEN];		    	             //内存信息
		char                      szDisk[MAX_CLIENT_HARDWARE_INFO_LEN];		    	                 //磁盘信息
		char                      szGraphics[MAX_CLIENT_HARDWARE_INFO_LEN];	    	                 //显卡信息
		char                      szMainboard[MAX_CLIENT_HARDWARE_INFO_LEN];	    	             //主板信息
		char                      szNIC[MAX_CLIENT_HARDWARE_INFO_LEN];				                 //网卡信息
		char                      szWebcam[MAX_CLIENT_HARDWARE_INFO_LEN];		    	             //摄像头信息
		char                      szSystem[MAX_CLIENT_HARDWARE_INFO_LEN];		    	             //系统信息

		tagST_CLIENT_HARDWARE_INFO()
		{
			ZeroMemory(szName, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szMacAddr, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szCPU, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szMemory, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szDisk, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szGraphics, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szMainboard, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szNIC, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szWebcam, MAX_CLIENT_HARDWARE_INFO_LEN);
			ZeroMemory(szSystem, MAX_CLIENT_HARDWARE_INFO_LEN);
		}
	}ST_CLIENT_HARDWARE_INFO;

	//服务配置
	typedef struct tagST_SERVER_CONFIG_ITEM
	{
		char                      szSID[MAX_PATH];
		char                      szSyncIP[MAX_PATH];
		char                      szVDiskIP[MAX_PATH];
		char                      szUpdateIP[MAX_PATH];
		DWORD                     dwSyncType;
		DWORD                     dwDestDrv;
		DWORD                     dwBalanceType;

		tagST_SERVER_CONFIG_ITEM()
		{
			ZeroMemory(szSID, MAX_PATH);
			ZeroMemory(szSyncIP, MAX_PATH);
			ZeroMemory(szVDiskIP, MAX_PATH);
			ZeroMemory(szUpdateIP, MAX_PATH);
			dwSyncType = (DWORD)0;
			dwDestDrv  = (DWORD)0;
			dwBalanceType = (DWORD)0;
		}
	}ST_SERVER_CONFIG_ITEM, *PST_SERVER_CONFIG_ITEM;

	enum FC_COPY_MODE
	{
		FC_COPY_COMPARE		= 0,
		FC_COPY_QUICK		= 1,  //对比文件
		FC_COPY_FORE		= 2,  //不对比 全部更新
		FC_COPY_VERSION		= 3   //对比索引版本号
	};
	typedef struct tagUpGameinfo        //主动推送记录游戏任务信息
	{
		DWORD			GID;				//游戏GID
		FC_COPY_MODE	Flag;				//游戏的对比方式
		DWORD			Srvip;				//服务器的ip 
		DWORD			speed;				//限制速度值
		DWORD           resv;				//保留字段
		char			SVRPATH[MAX_PATH]; //游戏在服务器上的路径
		char			CLIPATH[MAX_PATH]; //游戏在客户机上的路径
		tagUpGameinfo()
		{
			ZeroMemory(this,sizeof(tagUpGameinfo)); 
		}
	}UpGameinfo;

	tstring GetRunGameExe()
	{
		char path[MAX_PATH] = {0};
		path[0] = '\"';
		GetModuleFileName(NULL, &path[1], MAX_PATH);
		PathRemoveFileSpec(path);
		PathAddBackslash(path);
		lstrcat(path, "RunGame.exe\" ");
		return tstring(path);
	}

	class CMenuShareData
	{
	private:
		PVOID  m_lpData;
		SECURITY_ATTRIBUTES m_SecAttr;
		SECURITY_DESCRIPTOR m_SecDesc;
		PVOID BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD);
	private:
		HANDLE m_hFileMap;
		DWORD* m_pShareData;
	public:
		CMenuShareData(): m_lpData(NULL), m_hFileMap(NULL), m_pShareData(NULL)
		{
			//生成一个对所有用户可以访问的安全属性.
			m_SecAttr.nLength = sizeof(m_SecAttr);
			m_SecAttr.lpSecurityDescriptor = &m_SecDesc;
			m_SecAttr.bInheritHandle = FALSE;
			m_lpData = BuildRestrictedSD(&m_SecDesc);

			//创建和菜单共享的内存映射.
			if (m_lpData != NULL)
			{
				m_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, &m_SecAttr, PAGE_READWRITE, 0, MAX_PATH, "I8MENU_SHARE");
				if (m_hFileMap != NULL )
				{
					m_pShareData = (DWORD*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, MAX_PATH );
					if (m_hFileMap != NULL)
						ZeroMemory(m_pShareData, MAX_PATH );
				}
			}
		}
		~CMenuShareData()
		{
			if (m_pShareData != NULL)
			{
				UnmapViewOfFile(m_pShareData);
				m_pShareData = NULL;
			}
			if (m_hFileMap != NULL)
			{
				CloseHandle(m_hFileMap);
				m_hFileMap = NULL;
			}

			if (m_lpData)
			{
				HeapFree(GetProcessHeap(), 0, m_lpData);
				m_lpData = NULL;
			}
		}
		void WriteShareData(DWORD dwOnline, DWORD dwProt)
		{
			if (m_pShareData != NULL)
			{
				m_pShareData[0] = dwOnline;	
				m_pShareData[1] = dwProt;
			}
		}
	};
	class CBootRunPlug : public IPlug
	{
	private:
		IPlugMgr*			m_pPlugMgr;
		ILogger*			m_pLogger;
		HANDLE				m_hExited;
		HANDLE				m_hThread;		
		CPlatformPlug       m_oPlatformPlug;
		tagLocMachineInfo   m_LocMacInfo;				//本机信息：计算机名，IP，DNS,MAC...
		ST_CLIENT_HARDWARE_INFO m_stCltHardwareInfo;    //本机硬件信息
		CHyOperator			m_Hy;						//还原操作接口
		map<string, string> m_mapOption;				//系统选项列表
		vector<tagGameInfo*> m_VecGameInfo;				//游戏列表
		vector<tagGameInfo*> m_VecDelGameInfo;			//必须删除的游戏列表>
		vector<PST_SERVER_CONFIG_ITEM> m_VctServerCfg;  //服务器配置
		map<string,tagVDisk*> m_mapVDiskInfo;			//虚拟盘列表
		map<DWORD, PST_ICON_ITEM>	m_mapGameIcon;		//游戏图标;
		map<DWORD, PST_GAME_INDEX_INFO> m_mapGameIndexInfo;//游戏索引信息
		vector<tagIconItem> IconList;					//游戏图标网络包;	
		bool				bFirstRecHeartInfo;			//第一次心跳包数据;
		bool                bGetGameInfoIndex;          //是否取得游戏索引数据
		bool                bNeedNotifyFrameWindow;     //是否需要通知客户端窗口数据变化

		DWORD				dwIcoVer;					//图标压缩包版本号
		char*				pIcoData;					//图标数据;
		int                 m_nSafeCenterIdx;
		bool                bGetGameinfoOnce;           //是否取了一次游戏信息
		BOOL                bIsLoadBHOSuccess;          //加载BHO是否成功
		BOOL                bIsStartProtDiskSuccess;    //防狗是否启用

		//虚拟盘磁盘的临时数据
		tstring				m_strVDTmpPath;				//虚拟盘临时路径
		HMODULE				m_hRefreshDll;				//刷盘ＤＬＬ的模块句柄
		PFNDISKMOUNT		m_pfnDiskMount;				//刷盘的函数指针
		PFNDISKUMOUNT		m_pfnDiskUmount;			//删除虚拟盘的函数指针
	public:
		CBootRunPlug():m_pPlugMgr(NULL), m_pLogger(NULL), m_hExited(NULL), m_hThread(NULL)
		{
			ZeroMemory(&m_LocMacInfo, sizeof(m_LocMacInfo));	
			bFirstRecHeartInfo = false;
			bGetGameInfoIndex  = false;
			m_strVDTmpPath = TEXT("C:\\Temp\\");
			m_hRefreshDll = NULL;
			m_pfnDiskMount = NULL;
			m_pfnDiskUmount = NULL;
			pIcoData=NULL;
			bNeedNotifyFrameWindow = false;
			bGetGameinfoOnce = false;
			bIsLoadBHOSuccess = FALSE;
			bIsStartProtDiskSuccess = FALSE;
			m_nSafeCenterIdx = 0;
		}
		virtual ~CBootRunPlug()				{  							}
		virtual void Release()				{  delete this;				}
	public:
		//实现IPlug的基本接口
		virtual LPCTSTR GetPlugName()		{  return BOOTRUN_PLUG_NAME;}
		virtual DWORD  	GetVersion() 		{  return PLUG_VERSION;		}
		virtual LPCTSTR GetCompany()		{  return PLUG_COMPANY;		}
		virtual LPCTSTR GetDescription() 	{  return PLUG_DESCRIPTION; }
		virtual DWORD   GetListPort()		{  return PLUG_PORT;		}

		//插件的初始化，终止函数
		virtual bool	Initialize(IPlugMgr* pPlugMgr);
		virtual bool	UnInitialize();

		//其它插件不需要和该插件进行交互.
		virtual DWORD	SendMessage(DWORD cmd, DWORD param1, DWORD param2)	{ return ERROR_SUCCESS; }

		//该插件不需要处理客户端的网络命令.
		virtual bool    NeedUdpNotity()		{ return true;				}
		virtual void	UdpRecvNotify(char* pData, int length, sockaddr_in& from);
	private:
		//工作线程
		static UINT __stdcall WorkThread(PVOID lpVoid);
		void GetLocMachineOtherInfo(tagLocMachineInfo& info);//收集客户端相关数据,如:菜单版本，客户端版本，还原状态
		bool SendHeartInfo(tagLocMachineInfo& info);//程序心跳包;
		void ExecGameMenuAndShell();//启动游戏菜单和shell程序;	
		void OtherOperator();		//其它辅助操作。
		void SendCltHardwareInfo(); //发送硬件信息
		int  GetSafeCenterSel();
		//所有的开机任务
		bool ReportInfomation();                                //上报信息
		void ExecCheckdisk(string strParam);
		BOOL GetSystemOption(bool bOnlyRec = false);		//获取服务系统设置数据包
		void CopyRemoteDirectory();                         //远程目录穿透
		void PushGameAtOnce(FC_COPY_MODE cmpMode, DWORD dwSpeed, char* pszContent);                //实时推送游戏
		void GetBootTaskInfo();								//获取开机任务信息
		//	bool GetGamesInfo();								//获取服务游戏数据包
		BOOL GetGamesInfo();                                //获取游戏数据包
		BOOL GetGamesIcon();								//获取游戏图标文件
		BOOL StartOfflineSafeCenter();                      //启动离线安全中心
		//	void GetGamesIcon();                                //获取游戏图标文件
		bool GetOfflineGamesIcon();							//获取离线图标文件
		void GetURLFavorite();								//获取服务端的网站收藏夹信息
		bool GetVDiskInfo();								//获取虚拟盘信息
		bool RefreshVDisk();								//刷新虚拟盘.
		bool SetGameDeskTopLnk(tagGameInfo* pGameInfo);		//设置游戏桌面快捷方式；
		bool DelGameDeskTopLnkExcept(const vector<tagGameInfo*> &vctGameInfo); //删除除vctGameInfo以外的指向baronline的快捷方式
		bool WriteRegistryInfo();							//在注册表里记录服务端和客户端相关参数
		void RegistryPlug();								//注册第三方插件；
		void handleTask(string gid, string pcstart, string i8start);//执行官方配置的任务；
		BOOL GetServerConfig();                             //获取服务器配置
		void GetGameClassLnk();
		void CreateGameClassLnk(const char* const pszClass);  //创建游戏类别快捷方式		
		BOOL GetSafeCenterConfig();
		string GetDir(string src);
		string handleDir(string src);
	private:
		/************************************************************************/
		/* 开机任务的各类型执行函数                                             */
		/************************************************************************/
		bool ExecuteTask(BootRunTask ExecType,char* content);	//执行开机任务；
		bool SetDeskBkPic(char* Content);						//设置桌面墙纸(bmp/jpg/gif).
		bool SetDeskTopLnk(char* Content);						//设置桌面快捷方式；
		bool SetIEHomePage(char* Content);						//设置IE主页；
		bool ExecClientFile(char* Content);						//执行客户端E文件；
		bool ExecServerFile(char* Content);						//执行服务端文件；
		bool ExecSvrFile(char* Content);						//执行Bat文件；
		bool ExecVbs(char* Content);							//执行Vbs文件；
		bool DelClientFile(char* Content,__int64* DelSize);		//删除客户端文件或游戏；
		bool DelClientGames(char* Content);						//删除客户端点击率低的游戏；
		bool SetUpdateGame(char* Content);						//主动推送游戏;
		bool SynServerTime();									//同步服务端时间;


		bool GetSkinInfo();										//获取指定的皮肤包信息		
		void BindBHO();											//绑定bho.
		bool LoadProtDiskDrv();									//加载防机器狗驱动.
		void LoadVDiskInfo();                                   //读取虚拟盘离线信息
		void LoadServerConfig();                                //读取服务器离线信息
		void NotifyFrameWindow(UINT uMessage, WPARAM wParam, LPARAM lParam);
	private:	//工具函数
		//得到主程序的路径
		tstring GetAppPath();
		bool  RunExeFromService(string strfile,string strPara);	//从服务里执行文件；
		bool _ConnectTcpServer(DWORD ip,WORD port);				//连接虚拟盘服务端。
		BOOL RunProcess(LPCSTR lpImage,LPCSTR lpPara);			//从服务里执行文件；
		BOOL GetTokenByName(HANDLE &hToken,LPSTR lpName);		//打开令牌
		tstring GetFileVersion(char* szFileName);				//获取文件版本
		DWORD	GetFileCRC32(const std::string&  szFile);
		LPCTSTR GetCookie();
		string  GetStringOpt(string strOptName);

		//确保系统选项存在
		BOOL    EnsureSysOptExist();
		//游戏盘剩余空间(MB)
		DWORD	GetGameDrvFreeSize();
		//安全中心状态
		DWORD	GetSafeCenterStatus();
		//客户端硬件配置信息
		BOOL	GetClientHardwareInfo();
		//获取摄像头信息
		void    GetWebcamName(char* pInBuffer);

		//获取第一次游戏信息索引
		void    GetFirstGameInfoIndex();
		//读取游戏图标文件
		BOOL    ReadGameIconFile();
		//读取游戏信息文件
		BOOL    ReadGameInfoFile();
		tstring _getWmiInfo(IWbemClassObject *pClassObject,LPCTSTR lpszField);
		tstring GetWmiInfo( LPCTSTR lpszClass,LPCTSTR lpszField );	//通过WMI获取系统信息		
		//调用通信接口来交互一次网络命令
		void InitPackage(_packageheader* header, WORD cmd, int nExtraLen = 0);
		bool ExecCmd(LPSTR pOutData, int nOutLen, LPSTR& pInData, int& nInLen, bool bNeedAck = true, bool bAutoLock = true);
		bool ExecSimpleCmd(WORD cmd, LPSTR& pInData, int& nInLen, LPSTR pOutData = NULL, int nOutLen = 0);

		//加载刷新盘dll
		bool LoadVDiskDll();

		//检测虚拟盘服务
		bool CheckVDiskService();

		//检测防狗驱动启动
		bool CheckProtDiskService();

		//查看共享盘在客户端是否存在。
		bool CheckVDiskDriver(tagVDisk* pVDisk);

		//添加游戏推送任务
		bool AddPushGameTask(UpGameinfo* pArrayGameInfo, DWORD dwGameCount);

	public:
		//计算文件是否需要更新
		BOOL    FileIsNeedUpdate(DWORD dwProgramType, DWORD dwPathType, LPSTR lpRemoteFileName, LPSTR lpLocalFilePath = NULL);
		BOOL    DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen);
		//保存离线信息（包括游戏数据，虚拟盘，系统设置,收藏夹等服务端数据）
		void    WriteDataFile(string FileName, char* data,bool bInprotect, DWORD datalength = 0);	
	};

	bool GameCliNumSort(tagGameInfo *GameInfo1, tagGameInfo *GameInfo2)
	{
		return GameInfo1->SVRCLICK > GameInfo2->SVRCLICK;
	}

	IPlug* WINAPI CreatePlug()
	{
		return new CBootRunPlug;
	}

	bool CBootRunPlug::Initialize(IPlugMgr* pPlugMgr)
	{
		m_pPlugMgr =  pPlugMgr;
		if (m_pPlugMgr== NULL || (m_pLogger = pPlugMgr->CreateLogger()) == NULL)
			return false;
		m_Hy.SetLogger(m_pLogger);
		m_pLogger->SetLogFileName(BOOTRUN_PLUG_NAME);
		m_pLogger->SetAddDateBefore(false);
		m_pLogger->WriteLog(LM_INFO, TEXT("=============================================================="));
		m_pLogger->SetAddDateBefore(true);
		m_pLogger->WriteLog(LM_INFO, TEXT("Plug Initialize."));
		m_hExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hExited == NULL)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("Create Exit Event Fail:%d"), GetLastError());
		}
		else
		{
			//先得到一次计算机信息
			m_pPlugMgr->GetCommunication()->GetLocMachineInfo(m_LocMacInfo);
			m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create WorkThread:%s:%d\r\n"), 
				m_hThread != NULL ? TEXT("Success") : TEXT("False"), GetLastError());
		}
		return m_hThread != NULL;
	}

	bool CBootRunPlug::UnInitialize()
	{
		//停止线程
		if (m_hExited != NULL && m_hThread != NULL)
		{
			SetEvent(m_hExited);
			WaitForSingleObject(m_hThread, INFINITE);
		}
		if (m_hThread != NULL)
			CloseHandle(m_hThread);
		if (m_hExited != NULL)
			CloseHandle(m_hExited);
		m_hThread = NULL;
		m_hExited = NULL;

		//释放分配的内存
		CoTaskMemFree(pIcoData);
		map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.begin();
		for(;it != m_mapVDiskInfo.end();it++)
		{
			delete it->second;
		}

		map<DWORD, PST_ICON_ITEM>::iterator icoIt = m_mapGameIcon.begin();
		for (; icoIt != m_mapGameIcon.end(); icoIt++)
		{
			delete[] icoIt->second->pIconData;
			delete icoIt->second;
			icoIt->second = NULL;
		}

		map<DWORD, PST_GAME_INDEX_INFO>::iterator idxIt = m_mapGameIndexInfo.begin();
		for(;idxIt != m_mapGameIndexInfo.end();idxIt++)
		{
			delete idxIt->second;
			idxIt->second = NULL;
		}

		vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
		for(;item != m_VecGameInfo.end();item++)
		{
			delete *item;
		}

		vector<PST_SERVER_CONFIG_ITEM>::iterator srvItem = m_VctServerCfg.begin();
		for(;srvItem != m_VctServerCfg.end();srvItem++)
		{
			delete *srvItem;
		}

		vector<tagGameInfo*>::iterator Delitem = m_VecDelGameInfo.begin();
		for(;Delitem != m_VecDelGameInfo.end();Delitem++)
		{
			delete *Delitem;
		}
		for (size_t idx=0; idx<IconList.size(); idx++)
		{
			CoTaskMemFree(IconList[idx].pData);
		}
		if (m_pLogger != NULL)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("Plug UnInitialize."));
			m_pLogger->SetAddDateBefore(false);
			m_pLogger->WriteLog(LM_INFO, TEXT("==============================================================\r\n\r\n"));
			Release_Interface(m_pLogger);
		}
		return true;
	}

	bool CBootRunPlug::LoadProtDiskDrv()
	{
        if (!m_Hy.IsInstallHy())
        {
			m_pLogger->WriteLog(LM_INFO, "Hy is not install , ProtDiskDrv will not be loaded!");
			return false;
        }
		HANDLE hFile = CreateFile("\\\\.\\wxDiskGuard", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, 
			OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			CheckProtDiskService();
			hFile = CreateFile("\\\\.\\wxDiskGuard", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{	
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, "Open ProtDisk fail:%d:%s", dwError, Error.ErrorMessage());
				return false;
			}
		}

		DWORD dwByteReturned = 0;
		DeviceIoControl(hFile,
			CTL_CODE(9000, 101, METHOD_BUFFERED, FILE_ANY_ACCESS),
			NULL,
			0,
			NULL,
			0,
			&dwByteReturned,
			NULL);

		CloseHandle(hFile);
		m_pLogger->WriteLog(LM_INFO, "Open ProtDisk success.");
		bIsStartProtDiskSuccess = TRUE;
		return true;
	}

	void CBootRunPlug::LoadVDiskInfo()
	{
		map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.begin();
		for(;it != m_mapVDiskInfo.end();it++)
		{
			delete it->second;
		}
		m_mapVDiskInfo.clear();

		string strFile = GetAppPath() + TEXT("data\\VDisk2.dat");
		HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)   return;

		DWORD dwSize = GetFileSize(hFile, NULL);
		char* pData = new char[dwSize+1];
		DWORD dwReadBytes = 0;
		if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("read vdisk file error:%d"), GetLastError());
			CloseHandle(hFile);
			delete[] pData;
			return ;
		}
		pData[dwReadBytes] = 0;
		try
		{
			CPackageHelper package(pData);
			if (CMD_RET_SUCCESS == package.popDWORD())
			{
				DWORD dwCount = package.popDWORD();
				if (dwCount == 0)
					m_pLogger->WriteLog(LM_INFO, TEXT("no share vdisk."));
				else  
				{		
					for (DWORD idx=0; idx<dwCount; idx++)
					{
						tagVDisk* pVDisk = (tagVDisk*) new tagVDisk;				
						package.popString(pVDisk->VID);
						pVDisk->IP = package.popDWORD();
						pVDisk->Port=(WORD)package.popDWORD();

						DWORD itmp=package.popDWORD();
						pVDisk->SvrDrv = (char)itmp;
						itmp=package.popDWORD();
						pVDisk->CliDrv = (char)itmp;;

						pVDisk->SerialNo= package.popDWORD();
						pVDisk->Type = package.popDWORD();
						pVDisk->StartType= package.popDWORD();
						package.popString(pVDisk->SID);
						pVDisk->bExist = CheckVDiskDriver(pVDisk);
						m_mapVDiskInfo.insert(make_pair(pVDisk->VID, pVDisk));
					}
				}
			}
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("data package exeception.")); }

		CloseHandle(hFile);
		delete []pData;
	}

	void CBootRunPlug::LoadServerConfig()
	{
		vector<PST_SERVER_CONFIG_ITEM>::iterator srvItem = m_VctServerCfg.begin();
		for(;srvItem != m_VctServerCfg.end();srvItem++)
		{
			delete *srvItem;
		}
		m_VctServerCfg.clear();

		string strFile = GetAppPath() + TEXT("data\\ServerCfg.dat");
		HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("Open Server List Config fail:%d"), GetLastError());
		}
		else
		{
			DWORD dwSize = GetFileSize(hFile, NULL);
			char* pData = new char[dwSize+1];
			DWORD dwReadBytes = 0;
			if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
			{
				CloseHandle(hFile);
				delete[] pData;		
				return ;
			}
			pData[dwReadBytes] = 0;
			try
			{
				CPackageHelper package(pData);
				if (CMD_RET_SUCCESS == package.popDWORD())
				{
					tstring file = GetAppPath() + TEXT("gameclient.ini");
					TCHAR buf[MAX_PATH] = {0};
					GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), buf, MAX_PATH, file.c_str());

					BOOL bAddMainServer = TRUE;
					DWORD dwServerCount = package.popDWORD();
					for (DWORD dwLoop = 0; dwLoop < dwServerCount; ++dwLoop)
					{
						PST_SERVER_CONFIG_ITEM pSvrCfgItem = new ST_SERVER_CONFIG_ITEM;
						package.popString(pSvrCfgItem->szSID);
						package.popString(pSvrCfgItem->szSyncIP);
						package.popString(pSvrCfgItem->szVDiskIP);
						package.popString(pSvrCfgItem->szUpdateIP);
						pSvrCfgItem->dwSyncType = package.popDWORD();
						pSvrCfgItem->dwDestDrv  = package.popDWORD();
						pSvrCfgItem->dwBalanceType = package.popDWORD();

						m_VctServerCfg.push_back(pSvrCfgItem);
						if (lstrcmp(pSvrCfgItem->szSyncIP, buf) == 0
							|| lstrcmp(pSvrCfgItem->szVDiskIP, buf) == 0
							|| lstrcmp(pSvrCfgItem->szUpdateIP, buf) == 0)
						{
							bAddMainServer = FALSE;
						}
						m_pLogger->WriteLog(LM_INFO, "服务器SID:%s, 同步IP:%s, 虚拟盘IP:%s, 更新IP:%s, 同步类型:%ld, 目标盘符:%ld, 负载均衡类型:%ld",
							pSvrCfgItem->szSID, pSvrCfgItem->szSyncIP, pSvrCfgItem->szVDiskIP, pSvrCfgItem->szUpdateIP, pSvrCfgItem->dwSyncType, pSvrCfgItem->dwDestDrv, pSvrCfgItem->dwBalanceType);
					}
					if (bAddMainServer)
					{
						PST_SERVER_CONFIG_ITEM pSvrCfgItem = new ST_SERVER_CONFIG_ITEM;
						strcpy(pSvrCfgItem->szSID, "{FE65D7A0-061F-4a5e-A23E-DFF74BF31C24}");
						strcpy(pSvrCfgItem->szSyncIP, buf);
						strcpy(pSvrCfgItem->szVDiskIP, buf);
						strcpy(pSvrCfgItem->szUpdateIP, buf);
						pSvrCfgItem->dwSyncType = 0;
						pSvrCfgItem->dwDestDrv  = 0;
						pSvrCfgItem->dwBalanceType = 0;

						m_VctServerCfg.push_back(pSvrCfgItem);
						m_pLogger->WriteLog(LM_INFO, "服务器列表没有主服务器SID:%s，同步IP:%s, 虚拟盘IP:%s, 更新IP:%s",
							pSvrCfgItem->szSID, pSvrCfgItem->szSyncIP, pSvrCfgItem->szVDiskIP, pSvrCfgItem->szUpdateIP);
					}
				}
			}
			catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("data package exeception.")); }
			CloseHandle(hFile);
			delete []pData;
		}
	}

	HMODULE LoadRemoteControl()
	{
		char szDLL[MAX_PATH] = {0};
		GetModuleFileName(NULL, szDLL, MAX_PATH);
		PathRemoveFileSpec(szDLL);
		PathAddBackslash(szDLL);
		lstrcat(szDLL, "WinVNC\\");
		SetDllDirectory(szDLL);
		lstrcat(szDLL, "WinVNC.dll");
		HMODULE hMod = LoadLibrary(szDLL);
		if (hMod != NULL)
		{
			typedef BOOL (WINAPI* PFNSTARTVNC)();
			PFNSTARTVNC pfnStartVNC = GetProcAddress(hMod, "_StartVNC@0");
			if (pfnStartVNC != NULL)
			{
				try { pfnStartVNC(); } catch (...) {}
			}
		}

		return hMod;
	}

	void  FreeRemoteControl(HMODULE hMod)
	{
		if (hMod != NULL)
		{
			typedef BOOL (WINAPI* PFNSTOPVNC)();
			PFNSTOPVNC pfnStopVNC = GetProcAddress(hMod, "_StopVNC@0");
			if (pfnStopVNC != NULL)
			{
				try { pfnStopVNC(); } catch (...) {}
			}
		}
	}

	void CBootRunPlug::OtherOperator()
	{
		//运行插件中心
		m_oPlatformPlug.Start(m_pLogger, m_pPlugMgr, m_hExited, &m_Hy);

		//启动"ShellHWDetection"服务
		SC_HANDLE   schSCManager = OpenSCManager(
			NULL,                   // machine (NULL == local)
			NULL,                   // database (NULL == default)
			SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE  // access required
			);
		if (schSCManager == NULL)
			return ;

		SC_HANDLE   schService = OpenService(schSCManager, "ShellHWDetection", SERVICE_ALL_ACCESS);
		if (schService != NULL)
		{
			ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_AUTO_START,
				SERVICE_ERROR_IGNORE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			StartService(schService, 0, NULL);

			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
		}
	}

	UINT __stdcall CBootRunPlug::WorkThread(PVOID lpVoid)
	{
		CBootRunPlug* pThis = reinterpret_cast<CBootRunPlug*>(lpVoid);
		bool bProtDisk = pThis->LoadProtDiskDrv();

		CoInitialize(NULL);
		CMenuShareData MenuShareData;
		HMODULE hMod = LoadRemoteControl(); 		
		i8desk::ILogger* pLogger = pThis->m_pLogger;

		HANDLE handle[2];
		handle[0] = CreateEvent(NULL, FALSE, TRUE, NULL);
		handle[1] = pThis->m_hExited;		

		DWORD dwSecond = 0;			//时间间隔的秒数
		bool bRunHeart = false;
		bool bRunTask  = false;
		bool bRunVDisk = false;
		bool bRunOnceTask = false;  //是否执行了连接上后的一次性任务 
		bool bReportInfo = false;   //是否上报了信息
		while (1)
		{			
			DWORD dwRet = WaitForMultipleObjects(2, handle, FALSE, 1000);
			if (dwRet == WAIT_OBJECT_0 + 1)
				break;
			else if (dwRet == WAIT_TIMEOUT || dwRet == WAIT_OBJECT_0)
			{
				dwSecond ++;	//累加时间
				if (!pThis->m_pPlugMgr->GetCommunication()->IsConnected())
				{
					MenuShareData.WriteShareData(0, pThis->m_LocMacInfo.ProtInstal);
					if (dwSecond >= 60)	//连接不上，也是60秒做一次邦订
						pThis->BindBHO();
                    //启动安全中心
					static BOOL bIsStartSafeCenter = FALSE;  
					if (bIsStartSafeCenter == FALSE)
					{ 
						pThis->StartOfflineSafeCenter();
						bIsStartSafeCenter = TRUE;
					}
					//刷新虚拟盘
					if (!bRunVDisk)
					{
						pThis->EnsureSysOptExist();
						pThis->LoadServerConfig();
						pThis->LoadVDiskInfo();
						pThis->RefreshVDisk();//开机刷盘；
						bRunVDisk = true;
					}
					//上报信息
					if (!bReportInfo)
					{
						pThis->ReportInfomation();
						bReportInfo = true;
					}

					continue ;
				}
				MenuShareData.WriteShareData(1, pThis->m_LocMacInfo.ProtInstal);

				//心跳最先执行，当心跳执行后，才能执行其它所有的功能.
				//第一次连接上，或者６０秒执行一次ＢＨＯ帮订和心跳
				if (!bRunHeart || dwSecond >= 60)
				{
					pThis->BindBHO();

					pThis->GetLocMachineOtherInfo(pThis->m_LocMacInfo);
					if (!pThis->SendHeartInfo(pThis->m_LocMacInfo))
						continue;

					bRunHeart = true;
					dwSecond = 0;
				}
				//连接上后，当执行过心跳就先执行刷新虚拟盘.
				if (!bRunVDisk)
				{	
				    if(pThis->GetSystemOption() && pThis->GetServerConfig() && pThis->GetVDiskInfo())
					{
						pThis->RefreshVDisk();//开机刷盘；
					}
					else
					{
						pThis->m_pLogger->WriteLog(LM_INFO, TEXT("在线刷盘时相关数据获取失败，执行离线刷盘"));
						pThis->EnsureSysOptExist();
						pThis->LoadServerConfig();
						pThis->LoadVDiskInfo();
						pThis->RefreshVDisk();//开机刷盘；
					}
					bRunVDisk = true;
				}
				if (!bRunOnceTask)
				{	
					//获取硬件信息
					pThis->GetClientHardwareInfo();
					//发送硬件信息
					pThis->SendCltHardwareInfo();
					bRunOnceTask = true;
				}

				//在Windows登陆后，仅执行一次下面的所有的任务
				if (!bRunTask && FindWindow("Shell_TrayWnd",NULL) != NULL)
				{
					pThis->m_pLogger->WriteLog(LM_INFO, TEXT("Find Windows StartMenu."));
					pThis->m_pPlugMgr->GetCommunication()->GetLocMachineInfo(pThis->m_LocMacInfo);
					pThis->GetLocMachineOtherInfo(pThis->m_LocMacInfo);

					//执行一次磁盘检测
					string strParam = TEXT("/CHKDSK");
					pThis->ExecCheckdisk(strParam);

					//获取目录快捷方式
					pThis->GetGameClassLnk();

					WriteI8deskProtocol();	//i8desk协议；

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;
					pThis->m_pLogger->WriteLog(LM_INFO, TEXT("获取游戏信息"));
					pThis->GetGamesInfo();//获取游戏；

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;
					pThis->m_pLogger->WriteLog(LM_INFO, TEXT("获取收藏夹"));
					pThis->GetURLFavorite();//获取收藏夹；

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;
					pThis->WriteRegistryInfo();//在注册表里记录服务端和客户端相关参数

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;
					if (!bReportInfo)
					{
						pThis->ReportInfomation();
						bReportInfo = true;
					}

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;						
					pThis->RegistryPlug();//注册插件，是系统选项里的一个选项字符串，根据这个字符串，注册一些插件。

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;

					pThis->CopyRemoteDirectory(); //远程穿透

					pThis->GetBootTaskInfo();//执行用户自定义任务列表；

					if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
						break;
					pThis->GetSkinInfo();//获取皮肤文件；
					//获取安全中心配置
					pThis->GetSafeCenterConfig();
					pThis->ExecGameMenuAndShell();//启动菜单和shell程序。

					pThis->OtherOperator();					

					bRunTask = true;

					if (!bProtDisk)
					{
						pThis->LoadProtDiskDrv();
					}
				}
			}
			else
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				pLogger->WriteLog(LM_INFO, TEXT("WorkThread WaitForMultipleObjects Error:%d:%d:%s"),
					dwRet, dwError, Error.ErrorMessage());
				break;
			}
		}
		CloseHandle(handle[0]);
		FreeRemoteControl(hMod);
		pLogger->WriteLog(LM_INFO, TEXT("Exit WorkThread."));
		CoUninitialize();
		return 0;
	}

	BOOL CBootRunPlug::GetSystemOption(bool bOnlyRec)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("start to Get SystemOption."));
		LPSTR pData = NULL;
		int   nSize = 0;
		DWORD length ;
		if (!ExecSimpleCmd(CMD_SYSOPT_GETLIST, pData, nSize))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("Get SysOpt Cmd fail."));
			return FALSE;
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("Get SysOpt Cmd Success."));

		CPackageHelper inpackage(pData);
		length = ((_packageheader*)pData)->Length;
		try
		{
			//先弹出状态字段
			inpackage.popDWORD();	
			//把收到正确的数据写到离线文件
			tstring datafile = GetAppPath()+ TEXT("data\\sysopt.dat");
			WriteDataFile(datafile, pData,true);			
			m_mapOption.clear();
			DWORD dwSize   = inpackage.popDWORD();
			char name[4096]  = {0};		
			char value[4096] = {0};				//?表示最大的值的数据大小只能有4K.
			for (DWORD idx=0; idx<dwSize; idx++)
			{
				inpackage.popString(name);
				inpackage.popByte();			//pop = .
				inpackage.popString(value);	
				m_mapOption.insert(make_pair(string(name), string(value)));
			}
			if (!bOnlyRec)//是否只接收新的系统选项数据，而不处理相关逻辑
			{
				//同步本机系统时间
				map<string, string>::iterator it = m_mapOption.find(OPT_M_SYNTIME);
				if (it != m_mapOption.end())
				{
					tstring strOptValue;
					if (it->second.size())
						strOptValue = it->second;
					if (strOptValue == "1")
					{
						SynServerTime();
					}
				}

				it = m_mapOption.find(OPT_M_IEURL);
				if (it != m_mapOption.end())
				{
					if (it->second.size())
						SetIEHomePage((char*)(it->second).c_str());
				}
			}
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("Get SysOpt Cmd Have ErrorX.")); }

		//释放内存
		CoTaskMemFree(pData);
		m_pLogger->WriteLog(LM_INFO, TEXT("end Get SystemOption.\r\n"));
		return TRUE;
	}

	void CBootRunPlug::GetBootTaskInfo()
	{
		LPSTR pData = NULL;
		int   length = 0;
		m_pLogger->WriteLog(LM_INFO, "********Start to Execute BootTask.********");
		try
		{
			if (!ExecSimpleCmd(CMD_BOOT_GETLIST, pData, length, m_LocMacInfo.szName, strlen(m_LocMacInfo.szName)))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Get BootTask Fail."));
				m_pLogger->WriteLog(LM_INFO, "********End Execute BootTask.********\r\n");
				return ;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("Get BootTask Success.\r\n"));

			CPackageHelper inpackage(pData);	
			inpackage.popDWORD();

			//执行每条开机任务
			char  ID[40] = {0};
			DWORD type = 0;
			DWORD flag = 0;
			char  Content[0x10000] = {0};
			DWORD dwSize = inpackage.popDWORD();
			if (dwSize <= 0)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("BootTask Info Count: %d"),dwSize);
				m_pLogger->WriteLog(LM_INFO, "********End Execute BootTask.********\r\n");
				return;
			}
			map<string, DWORD> mapBootRun;
			for (DWORD idx=0; idx<dwSize; idx++)
			{
				inpackage.popString(ID);			//任务ID
				inpackage.popDWORD();				//区域类型(客户端过滤不要)
				inpackage.popString(Content);		//区域参数(客户端过滤不要)
				type = inpackage.popDWORD();		//任务类型
				flag =	inpackage.popDWORD();		//标志
				inpackage.popString(Content);		//任务内容

				//保证快速退出
				if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
					break;

				//记录下执行成功的开机任务
				if (lstrlen(Content) && ExecuteTask((BootRunTask)type, Content))
				{
					mapBootRun.insert(make_pair(string(ID), type));
				}
			}

			//释放内存
			CoTaskMemFree(pData);
			pData = NULL;

			//应答成功执行的开机任务,如果是只执行一次，服务端会删除该任务
			size_t nCount = mapBootRun.size();
			if (nCount > 0)
			{
				char buf[1024] = {0};
				_packageheader* pheader = (_packageheader*)buf;
				InitPackage(pheader, CMD_BOOT_DELLIST);
				CPackageHelper outpackage(buf);
				outpackage.pushDWORD(nCount);
				for(map<string, DWORD>::iterator it = mapBootRun.begin(); it != mapBootRun.end(); it++)
				{
					outpackage.pushString(it->first.c_str(), it->first.length());
				}
				pheader->Length = outpackage.GetOffset();

				if (!ExecCmd(buf, pheader->Length, pData, length,false))
				{
					m_pLogger->WriteLog(LM_INFO, "Report BootTask Fail.");
					m_pLogger->WriteLog(LM_INFO, "********End Execute BootTask.********\r\n");
					return ;
				}
				m_pLogger->WriteLog(LM_INFO, "Report BootTask Success.");			
			}
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("Get BootTask Cmd Have ErrorX.")); }

		m_pLogger->WriteLog(LM_INFO, "********End Execute BootTask.********\r\n");
		return;
	}

	//bool CBootRunPlug::GetGamesInfo()
	//{
	//	m_pLogger->WriteLog(LM_INFO, TEXT("start to Get GamesInfo."));
	//	LPSTR pData = NULL;
	//	int   nSize = 0;
	//	if (!ExecSimpleCmd(CMD_GAME_GETLIST, pData, nSize, m_LocMacInfo.szName, lstrlen(m_LocMacInfo.szName)))
	//	{
	//		m_pLogger->WriteLog(LM_INFO, TEXT("Get GameInfo Fail."));
	//		return false;
	//	}
	//	m_pLogger->WriteLog(LM_INFO, TEXT("Get GameInfo Success."));

	//	tstring datafile = GetAppPath()+ TEXT("data\\GamesInfo.dat");
	//	WriteDataFile(datafile, pData,true);

	//	tstring icoFolder= GetAppPath()+TEXT("data\\icon\\");
	//	CreateDirectory((LPSTR)icoFolder.c_str(),NULL);		
	//		
	//	try
	//	{
	//		CPackageHelper inpackage(pData);
	//		DWORD flag = inpackage.popDWORD();

	//		if (flag == CMD_RET_SUCCESS)
	//		{
	//			DWORD dwSize   = inpackage.popDWORD();
	//			SHDeleteKey(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID);//删除游戏表里以往的游戏信息；
	//			char skey[MAX_PATH] = {0};//注册表里游戏信息的名称；
	//			char sdata[1024] = {0};//注册表里游戏信息值；
	//			vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
	//			for(;item != m_VecGameInfo.end();item++)
	//			{
	//				delete *item;
	//			}

	//			vector<tagGameInfo*>::iterator Delitem = m_VecDelGameInfo.begin();
	//			for(;Delitem != m_VecDelGameInfo.end();Delitem++)
	//			{
	//				delete *Delitem;
	//			}
	//			m_VecDelGameInfo.clear();
	//			m_VecGameInfo.clear();
	//			//?表示最大的值的数据大小只能有4K.
	//			for (DWORD idx=0; idx<dwSize; idx++)
	//			{
	//				tagGameInfo *pGameInfo=(tagGameInfo *)new tagGameInfo;
	//				pGameInfo->GID = inpackage.popDWORD();//游戏ID；
	//				pGameInfo->PID = inpackage.popDWORD();//父ID
	//				inpackage.popString(pGameInfo->NAME);//游戏名称；
	//				inpackage.popString(pGameInfo->IDCCLASS);//官方游戏类别；
	//				inpackage.popString(pGameInfo->GAMEEXE);//游戏官方EXE
	//				inpackage.popString(pGameInfo->PARAM);//游戏运行参数
	//				pGameInfo->SIZE = inpackage.popDWORD();//游戏大小;
	//				pGameInfo->DESKLNK = inpackage.popDWORD();//游戏是否显示到桌面快捷方式
	//				pGameInfo->TOOLBAR = inpackage.popDWORD();//是否显示在工具栏上;
	//				inpackage.popString(pGameInfo->SVRPATH);//游戏在服务器上的路径
	//				PathAddBackslash(pGameInfo->SVRPATH);
	//				inpackage.popString(pGameInfo->CLIPATH);//游戏在客户机上的路径
	//				PathAddBackslash(pGameInfo->CLIPATH);
	//				pGameInfo->PRIORITY = inpackage.popDWORD();//游戏等级
	//				pGameInfo->SHOWPRIORITY = inpackage.popDWORD();//游戏在菜单上的显示优先级
	//				inpackage.popString(pGameInfo->MATCHFILE);//游戏自动搜索特征文件
	//				inpackage.popString(pGameInfo->SAVEFILTER);//游戏存档信息
	//				pGameInfo->IDCUPTDATE =inpackage.popDWORD();//游戏在中心服务器的更新时间
	//				pGameInfo->SVRUPTDATE =inpackage.popDWORD();//游戏在网吧服务器的更新时间
	//				pGameInfo->IDCVER =inpackage.popDWORD();//游戏在中心服务器上的版本
	//				pGameInfo->SVRVER =inpackage.popDWORD();//游戏在网吧服务器上的版本
	//				pGameInfo->AUTOUPT =inpackage.popDWORD();//游戏是否自动从服务器上更新
	//				pGameInfo->I8PLAY =inpackage.popDWORD();//游戏需要强制更新
	//				pGameInfo->IDCCLICK =inpackage.popDWORD();//游戏全网点率
	//				pGameInfo->SVRCLICK = inpackage.popDWORD();//游戏网吧点率
	//				pGameInfo->ADDDATE = inpackage.popDWORD();//游戏添加日期
	//				inpackage.popString(pGameInfo->GAMELINK);//官网
	//				pGameInfo->RUNTYPE = inpackage.popDWORD();//游戏运行方式
	//				pGameInfo->IFUPDATE = inpackage.popDWORD();//游戏是否更新
	//				pGameInfo->IFUPDATE = inpackage.popDWORD();//游戏是否显示
	//				inpackage.popString(pGameInfo->VID); //游戏所在虚拟盘ID
	//				pGameInfo->STATUS = inpackage.popDWORD();//游戏下载状态；
	//				inpackage.popString(pGameInfo->COMMENT); //comment;

	//				if (pGameInfo->PRIORITY == 1)//游戏执行点击率低的时候，必须删除
	//				{
	//					m_VecDelGameInfo.push_back(pGameInfo);
	//				}
	//				else
	//					m_VecGameInfo.push_back(pGameInfo);

	//				tstring GameSvPath(pGameInfo->SVRPATH);
	//				map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.find(tstring(pGameInfo->VID));
	//				if(it != m_mapVDiskInfo.end())
	//					GameSvPath.at(0) = ((tagVDisk*)(it->second))->CliDrv;				
	//				strcpy(pGameInfo->VDPATH,GameSvPath.c_str());


	//				if (1 == pGameInfo->DESKLNK)
	//				{
	//					char buf[MAX_PATH]={0};
	//					_stprintf(buf,TEXT("%d"),pGameInfo->GID);
	//					map<DWORD,PST_ICON_ITEM>::iterator it1 = m_mapGameIcon.find(pGameInfo->GID);
	//					if(it1 != m_mapGameIcon.end())
	//					{
	//						tstring dataFolder= GetAppPath()+TEXT("data\\icon\\")+tstring(buf)+".ico";
	//						DWORD dwsize = *(DWORD*)(&it1->second->pIconData[0]);
	//						WriteDataFile(dataFolder, it1->second->pIconData+4,false,dwsize);
	//					}		
	//					SetGameDeskTopLnk(pGameInfo);
	//				}

	//				_stprintf(skey, "%d", pGameInfo->GID);
	//				_stprintf(sdata, "name=%s;mode=%d;remotepath=%s;localpath=%s",
	//					pGameInfo->NAME, pGameInfo->RUNTYPE, pGameInfo->VDPATH, pGameInfo->CLIPATH);
	//				SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, skey, REG_SZ, 
	//					sdata, lstrlen(sdata));		

	//			}
	//			std::stable_sort(m_VecGameInfo.begin(),m_VecGameInfo.end(),GameCliNumSort);

	//		}
	//	}
	//	catch (...) 
	//	{
	//		m_pLogger->WriteLog(LM_INFO, TEXT("Rec GameInfo ErrorX.")); 
	//	}	


	//	//释放内存
	//	CoTaskMemFree(pData);
	//	m_pLogger->WriteLog(LM_INFO, TEXT("end to Get GamesInfo.\r\n"));
	//	return true;
	//}

	//获取第一次游戏索引
	void CBootRunPlug::GetFirstGameInfoIndex()
	{
		//定义变量
		char szSendBuffer[1024] = {0};
		char *pRecvBuffer = NULL;
		int  nRecvDataSize = 0;		
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_GAME_GETIDXS);
		CPackageHelper SendPacket(szSendBuffer);
		SendPacket.pushString(m_LocMacInfo.szName, strlen(m_LocMacInfo.szName));
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取游戏索引文件失败");
			return ;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		//查找更新游戏列表
		list<DWORD>   UptGameList;
		DWORD         dwGameCount  = 0;
		DWORD         dwGameId     = 0;
		DWORD         dwGameIdx    = 0;
		DWORD         dwRunTypeIdx = 0;
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
			dwGameCount = RecvPacket.popDWORD();
			for (DWORD dwLoop = 0; dwLoop < dwGameCount; ++dwLoop)
			{
				dwGameId     = RecvPacket.popDWORD();
				dwGameIdx    = RecvPacket.popDWORD();
				dwRunTypeIdx = RecvPacket.popDWORD();
				//保存这个结点
				PST_GAME_INDEX_INFO pGameIdxInfo = new ST_GAME_INDEX_INFO;
				pGameIdxInfo->dwGameId = dwGameId;
				pGameIdxInfo->dwGameIndex = dwGameIdx;
				pGameIdxInfo->dwRunTypeIndex = dwRunTypeIdx;
				m_mapGameIndexInfo.insert(make_pair(dwGameId, pGameIdxInfo));
			}
			bGetGameInfoIndex = true;
		}
		CoTaskMemFree(pRecvBuffer);		
	}

	//读取游戏信息文件
	BOOL CBootRunPlug::ReadGameInfoFile()
	{
		char    *pGameInfoMem = NULL;
		DWORD   dwGameInfoFileSize = 0;	
		HANDLE	hGameInfoFile = INVALID_HANDLE_VALUE;
		tstring strDataFile = GetAppPath()+ TEXT("data\\GamesInfo.dat");
		//清空内存数据
		vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
		for(;item != m_VecGameInfo.end();item++)
		{
			delete *item;
		}
		m_VecGameInfo.clear();
		vector<tagGameInfo*>::iterator Delitem = m_VecDelGameInfo.begin();
		for(;Delitem != m_VecDelGameInfo.end();Delitem++)
		{
			delete *Delitem;
		}
		m_VecDelGameInfo.clear();
		//打开文件			
		hGameInfoFile = ::CreateFile(strDataFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hGameInfoFile != INVALID_HANDLE_VALUE)
		{
			//获取文件大小
			dwGameInfoFileSize = GetFileSize(hGameInfoFile, NULL);
			if(dwGameInfoFileSize == INVALID_FILE_SIZE || dwGameInfoFileSize < sizeof(_packageheader))
			{
				//文件大小获取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("游戏信息文件大小获取失败或游戏信息文件被损坏."));
				CloseHandle(hGameInfoFile);
				return FALSE;
			}
			//分配内存
			pGameInfoMem = new char[dwGameInfoFileSize + 1];
			//读入文件
			DWORD dwReadBytes = 0;
			if (!ReadFile(hGameInfoFile, pGameInfoMem, dwGameInfoFileSize, &dwReadBytes, NULL) || dwReadBytes != dwGameInfoFileSize)
			{
				//文件读取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("游戏信息文件读取失败."));
				CloseHandle(hGameInfoFile);
				delete[] pGameInfoMem;
				pGameInfoMem = NULL;
				return FALSE;
			}
			pGameInfoMem[dwReadBytes] = 0;

			try
			{
				//解析内容
				CPackageHelper FilePackage(pGameInfoMem);
				_packageheader *pHeader = (_packageheader*)pGameInfoMem;
				DWORD dwFlag = FilePackage.popDWORD();
				DWORD dwGameCount = FilePackage.popDWORD();
				for (DWORD dwLoop = 0; dwLoop < dwGameCount; ++dwLoop)
				{
					tagGameInfo *pGameInfo=(tagGameInfo *)new tagGameInfo;
					pGameInfo->GID = FilePackage.popDWORD();//游戏ID；
					pGameInfo->PID = FilePackage.popDWORD();//父ID
					FilePackage.popString(pGameInfo->NAME);//游戏名称；
					FilePackage.popString(pGameInfo->IDCCLASS);//官方游戏类别；
					FilePackage.popString(pGameInfo->GAMEEXE);//游戏官方EXE
					FilePackage.popString(pGameInfo->PARAM);//游戏运行参数
					pGameInfo->SIZE = FilePackage.popDWORD();//游戏大小;
					pGameInfo->DESKLNK = FilePackage.popDWORD();//游戏是否显示到桌面快捷方式
					pGameInfo->TOOLBAR = FilePackage.popDWORD();//是否显示在工具栏上;
					FilePackage.popString(pGameInfo->SVRPATH);//游戏在服务器上的路径
					PathAddBackslash(pGameInfo->SVRPATH);
					FilePackage.popString(pGameInfo->CLIPATH);//游戏在客户机上的路径
					PathAddBackslash(pGameInfo->CLIPATH);
					pGameInfo->PRIORITY = FilePackage.popDWORD();//游戏等级
					pGameInfo->SHOWPRIORITY = FilePackage.popDWORD();//游戏在菜单上的显示优先级
					FilePackage.popString(pGameInfo->MATCHFILE);//游戏自动搜索特征文件
					FilePackage.popString(pGameInfo->SAVEFILTER);//游戏存档信息
					pGameInfo->IDCUPTDATE =FilePackage.popDWORD();//游戏在中心服务器的更新时间
					pGameInfo->SVRUPTDATE =FilePackage.popDWORD();//游戏在网吧服务器的更新时间
					pGameInfo->IDCVER =FilePackage.popDWORD();//游戏在中心服务器上的版本
					pGameInfo->SVRVER =FilePackage.popDWORD();//游戏在网吧服务器上的版本
					pGameInfo->AUTOUPT =FilePackage.popDWORD();//游戏是否自动从服务器上更新
					pGameInfo->I8PLAY =FilePackage.popDWORD();//游戏需要强制更新
					pGameInfo->IDCCLICK =FilePackage.popDWORD();//游戏全网点率
					pGameInfo->SVRCLICK = FilePackage.popDWORD();//游戏网吧点率
					pGameInfo->ADDDATE = FilePackage.popDWORD();//游戏添加日期
					FilePackage.popString(pGameInfo->GAMELINK);//官网
					pGameInfo->RUNTYPE = FilePackage.popDWORD();//游戏运行方式
					pGameInfo->IFUPDATE = FilePackage.popDWORD();//游戏是否更新
					pGameInfo->IFUPDATE = FilePackage.popDWORD();//游戏是否显示
					FilePackage.popString(pGameInfo->VID); //游戏所在虚拟盘ID
					pGameInfo->STATUS = FilePackage.popDWORD();//游戏下载状态；
					FilePackage.popString(pGameInfo->COMMENT); //comment;

					tstring GameSvPath(pGameInfo->SVRPATH);
					map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.find(tstring(pGameInfo->VID));
					if(it != m_mapVDiskInfo.end())
						GameSvPath.at(0) = ((tagVDisk*)(it->second))->CliDrv;				
					strcpy(pGameInfo->VDPATH,GameSvPath.c_str());

					if (pGameInfo->PRIORITY == 1) m_VecDelGameInfo.push_back(pGameInfo);
					else m_VecGameInfo.push_back(pGameInfo);
				} 
			}
			catch(...)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("读取游戏信息文件发生异常，从服务器重新下载"));
				//清空内存数据
				vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
				for(;item != m_VecGameInfo.end();item++)
				{
					delete *item;
				}
				m_VecGameInfo.clear();
				vector<tagGameInfo*>::iterator Delitem = m_VecDelGameInfo.begin();
				for(;Delitem != m_VecDelGameInfo.end();Delitem++)
				{
					delete *Delitem;
				}
				m_VecDelGameInfo.clear();
			}              
			//关闭文件句柄
			CloseHandle(hGameInfoFile);
			//释放内存
			delete[] pGameInfoMem;
			pGameInfoMem = NULL;
			return TRUE;
		}	
		return FALSE;
	}

	BOOL CBootRunPlug::GetGamesInfo()
	{
		//定义变量
		char    szSendBuffer[1024] = {0};
		char    *pRecvBuffer = NULL;		
		char    *pGameInfoMem = NULL;
		int     nRecvDataSize = 0;	
		tstring strDataFile = GetAppPath()+ TEXT("data\\GamesInfo.dat");
		//读取本地游戏信息
		if (m_VecGameInfo.empty() && m_VecDelGameInfo.empty())
		{
			ReadGameInfoFile();
		}
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_GAME_GETIDXS);
		CPackageHelper SendPacket(szSendBuffer);
		SendPacket.pushString(m_LocMacInfo.szName, strlen(m_LocMacInfo.szName));
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取游戏索引文件失败");
			return FALSE;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		//查找更新游戏列表
		list<DWORD>   UptGameList;
		map<DWORD, PST_GAME_INDEX_INFO> mapNewGameIndexInfo; //最新列表
		DWORD         dwGameCount  = 0;
		DWORD         dwGameId     = 0;
		DWORD         dwGameIdx    = 0;
		DWORD         dwRunTypeIdx = 0;
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
			dwGameCount = RecvPacket.popDWORD();
			for (DWORD dwLoop = 0; dwLoop < dwGameCount; ++dwLoop)
			{
				dwGameId     = RecvPacket.popDWORD();
				dwGameIdx    = RecvPacket.popDWORD();
				dwRunTypeIdx = RecvPacket.popDWORD();

				PST_GAME_INDEX_INFO pGameIndexItem = new ST_GAME_INDEX_INFO;
				pGameIndexItem->dwGameId = dwGameId;
				pGameIndexItem->dwGameIndex = dwGameIdx;
				pGameIndexItem->dwRunTypeIndex = dwRunTypeIdx;
				mapNewGameIndexInfo.insert(make_pair(dwGameId, pGameIndexItem));

				map<DWORD, PST_GAME_INDEX_INFO>::iterator it = m_mapGameIndexInfo.find(dwGameId);
				if (it != m_mapGameIndexInfo.end())
				{
					if (it->second->dwGameIndex != dwGameIdx || it->second->dwRunTypeIndex != dwRunTypeIdx)
					{
						//需要更新
						UptGameList.push_back(dwGameId);
					}
				}
				else 
				{
					//保存这个结点
					PST_GAME_INDEX_INFO pGameIdxInfo = new ST_GAME_INDEX_INFO;
					pGameIdxInfo->dwGameId = dwGameId;
					pGameIdxInfo->dwGameIndex = dwGameIdx;
					pGameIdxInfo->dwRunTypeIndex = dwRunTypeIdx;
					m_mapGameIndexInfo.insert(make_pair(dwGameId, pGameIdxInfo));
					//需要更新
					UptGameList.push_back(dwGameId);
				}
			}
			bGetGameInfoIndex = true;
		}
		CoTaskMemFree(pRecvBuffer);		
		//替换为最新列表
		map<DWORD, PST_GAME_INDEX_INFO>::iterator idxIt = m_mapGameIndexInfo.begin();
		for(;idxIt != m_mapGameIndexInfo.end();idxIt++)
		{
			delete idxIt->second;
			idxIt->second = NULL;
		}
		m_mapGameIndexInfo.clear();
		m_mapGameIndexInfo = mapNewGameIndexInfo;

		tstring icoFolder= GetAppPath()+TEXT("data\\icon\\");
		CreateDirectory((LPSTR)icoFolder.c_str(),NULL);	
		//请求更新游戏
		DWORD dwUptListSize = UptGameList.size();
		DWORD dwUptCount = dwUptListSize > 0 ? dwUptListSize/10 : 0;
		dwUptCount += dwUptListSize%10 == 0 ? 0 : 1;

		for (DWORD dwLoop = 0; dwLoop < dwUptCount; ++dwLoop)
		{
			//计算本次更新个数
			DWORD dwCurrentUptCount = (dwUptListSize >= (dwLoop + 1) * 10) ? 10 : (dwUptListSize % 10);

			//构造数据
			pPacketHeader = (_packageheader*)szSendBuffer;
			InitPackage(pPacketHeader, CMD_GAME_GETSOME);
			CPackageHelper ReqGameDataPacket(szSendBuffer);
			ReqGameDataPacket.pushString(m_LocMacInfo.szName, strlen(m_LocMacInfo.szName));
			ReqGameDataPacket.pushDWORD(dwCurrentUptCount);
			for (DWORD i = 0; i < dwCurrentUptCount; ++i)
			{
				DWORD dwGameId = UptGameList.front();
				UptGameList.pop_front();
				ReqGameDataPacket.pushDWORD(dwGameId);
			}
			pPacketHeader->Length = ReqGameDataPacket.GetOffset();
			//执行命令
			if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
			{
				m_pLogger->WriteLog(LM_INFO, "获取游戏文件数据失败\r\n");
				return FALSE;
			}    
			//返回判断
			CPackageHelper ResGameDataPacket(pRecvBuffer);
			dwRetCode =ResGameDataPacket.popDWORD();
			if ( dwRetCode == CMD_RET_SUCCESS )
			{
				DWORD dwGameCount = ResGameDataPacket.popDWORD();
				PST_ICON_ITEM pIconItem = NULL;				
				for (DWORD dwIdx = 0; dwIdx < dwGameCount; ++dwIdx)
				{  			
					tagGameInfo *pGameInfo=(tagGameInfo *)new tagGameInfo;
					pGameInfo->GID = ResGameDataPacket.popDWORD();//游戏ID；
					pGameInfo->PID = ResGameDataPacket.popDWORD();//父ID
					ResGameDataPacket.popString(pGameInfo->NAME);//游戏名称；
					ResGameDataPacket.popString(pGameInfo->IDCCLASS);//官方游戏类别；
					ResGameDataPacket.popString(pGameInfo->GAMEEXE);//游戏官方EXE
					ResGameDataPacket.popString(pGameInfo->PARAM);//游戏运行参数
					pGameInfo->SIZE = ResGameDataPacket.popDWORD();//游戏大小;
					pGameInfo->DESKLNK = ResGameDataPacket.popDWORD();//游戏是否显示到桌面快捷方式
					pGameInfo->TOOLBAR = ResGameDataPacket.popDWORD();//是否显示在工具栏上;
					ResGameDataPacket.popString(pGameInfo->SVRPATH);//游戏在服务器上的路径
					PathAddBackslash(pGameInfo->SVRPATH);
					ResGameDataPacket.popString(pGameInfo->CLIPATH);//游戏在客户机上的路径
					PathAddBackslash(pGameInfo->CLIPATH);
					pGameInfo->PRIORITY = ResGameDataPacket.popDWORD();//游戏等级
					pGameInfo->SHOWPRIORITY = ResGameDataPacket.popDWORD();//游戏在菜单上的显示优先级
					ResGameDataPacket.popString(pGameInfo->MATCHFILE);//游戏自动搜索特征文件
					ResGameDataPacket.popString(pGameInfo->SAVEFILTER);//游戏存档信息
					pGameInfo->IDCUPTDATE =ResGameDataPacket.popDWORD();//游戏在中心服务器的更新时间
					pGameInfo->SVRUPTDATE =ResGameDataPacket.popDWORD();//游戏在网吧服务器的更新时间
					pGameInfo->IDCVER =ResGameDataPacket.popDWORD();//游戏在中心服务器上的版本
					pGameInfo->SVRVER =ResGameDataPacket.popDWORD();//游戏在网吧服务器上的版本
					pGameInfo->AUTOUPT =ResGameDataPacket.popDWORD();//游戏是否自动从服务器上更新
					pGameInfo->I8PLAY =ResGameDataPacket.popDWORD();//游戏需要强制更新
					pGameInfo->IDCCLICK =ResGameDataPacket.popDWORD();//游戏全网点率
					pGameInfo->SVRCLICK = ResGameDataPacket.popDWORD();//游戏网吧点率
					pGameInfo->ADDDATE = ResGameDataPacket.popDWORD();//游戏添加日期
					ResGameDataPacket.popString(pGameInfo->GAMELINK);//官网
					pGameInfo->RUNTYPE = ResGameDataPacket.popDWORD();//游戏运行方式
					pGameInfo->IFUPDATE = ResGameDataPacket.popDWORD();//游戏是否更新
					pGameInfo->IFUPDATE = ResGameDataPacket.popDWORD();//游戏是否显示
					ResGameDataPacket.popString(pGameInfo->VID); //游戏所在虚拟盘ID
					pGameInfo->STATUS = ResGameDataPacket.popDWORD();//游戏下载状态；
					ResGameDataPacket.popString(pGameInfo->COMMENT); //comment;

					if (pGameInfo->PRIORITY == 1)//游戏执行点击率低的时候，必须删除
					{	
						//删除高优先级中它的信息
						vector<tagGameInfo*>::iterator it = m_VecGameInfo.begin();
						for (; it != m_VecGameInfo.end(); ++it)
						{
							if ((*it)->GID == pGameInfo->GID)
							{
								delete *it;
								m_VecGameInfo.erase(it);
								break;
							}
						}	
						//先查找是否存在
						DWORD dwDelCount = m_VecDelGameInfo.size();
						BOOL  bExist = FALSE;
						for (DWORD dwIdx = 0; dwIdx < dwDelCount; ++dwIdx)
						{
							if (m_VecDelGameInfo[dwIdx]->GID == pGameInfo->GID)
							{
								delete m_VecDelGameInfo[dwIdx];
								m_VecDelGameInfo[dwIdx]= pGameInfo;
								bExist = TRUE;
								break;
							}
						}
						if (bExist == FALSE) 
						{
							m_VecDelGameInfo.push_back(pGameInfo);	
						}
					}
					else
					{
						//删除低优先级中它的信息
						vector<tagGameInfo*>::iterator it = m_VecDelGameInfo.begin();
						for (; it != m_VecDelGameInfo.end(); ++it)
						{
							if ((*it)->GID == pGameInfo->GID)
							{
								delete *it;
								m_VecDelGameInfo.erase(it);
								break;
							}
						}
						DWORD dwGameInfoCount = m_VecGameInfo.size();
						BOOL  bExist = FALSE;
						for (DWORD dwIdx = 0; dwIdx < dwGameInfoCount; ++dwIdx)
						{
							if (m_VecGameInfo[dwIdx]->GID == pGameInfo->GID)
							{
								delete m_VecGameInfo[dwIdx];
								m_VecGameInfo[dwIdx] = pGameInfo;
								bExist = TRUE;
								break;
							}
						}
						if (bExist == FALSE) 
						{
							m_VecGameInfo.push_back(pGameInfo);	
						}
					}

					tstring GameSvPath(pGameInfo->SVRPATH);
					map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.find(tstring(pGameInfo->VID));
					if(it != m_mapVDiskInfo.end())
						GameSvPath.at(0) = ((tagVDisk*)(it->second))->CliDrv;				
					strcpy(pGameInfo->VDPATH,GameSvPath.c_str());
				}	
			}
			CoTaskMemFree(pRecvBuffer);
		}
		//写入文件
		//		if (dwUptListSize == 0) return TRUE;
		//统计游戏个数
		DWORD dwHLevelGameCount = 0;    //等级较高的游戏
		DWORD dwLLevelGameCount = 0;    //等级较低的游戏
		BOOL  bIsDeleteGame = FALSE;
		vector<tagGameInfo*> vctNeedDesktopLnk; //需要创建桌面快捷方式的游戏
		for(DWORD dwLoop = 0; dwLoop < m_VecGameInfo.size(); ++dwLoop)
		{
			map<DWORD, PST_GAME_INDEX_INFO>::iterator it = m_mapGameIndexInfo.find(m_VecGameInfo[dwLoop]->GID);
			if (it != m_mapGameIndexInfo.end())
			{
				++dwHLevelGameCount;
			}
			else
			{
				bIsDeleteGame = TRUE;
			}
			if(1 == m_VecGameInfo[dwLoop]->DESKLNK)
			{
				vctNeedDesktopLnk.push_back(m_VecGameInfo[dwLoop]);
			}
		}
		for(DWORD dwLoop = 0; dwLoop < m_VecDelGameInfo.size(); ++dwLoop)
		{
			map<DWORD, PST_GAME_INDEX_INFO>::iterator it = m_mapGameIndexInfo.find(m_VecDelGameInfo[dwLoop]->GID);
			if (it != m_mapGameIndexInfo.end())
			{
				++dwLLevelGameCount;
			}
			else
			{
				bIsDeleteGame = TRUE;
			}
			if (1 == m_VecDelGameInfo[dwLoop]->DESKLNK)
			{
				vctNeedDesktopLnk.push_back(m_VecDelGameInfo[dwLoop]);
			}			
		}
		//创建快捷方式
		for (DWORD dwLoop = 0; dwLoop < vctNeedDesktopLnk.size(); ++dwLoop)
		{
			char buf[MAX_PATH]={0};
			_stprintf(buf,TEXT("%d"), vctNeedDesktopLnk[dwLoop]->GID);
			map<DWORD,PST_ICON_ITEM>::iterator it1 = m_mapGameIcon.find(vctNeedDesktopLnk[dwLoop]->GID);
			if(it1 != m_mapGameIcon.end())
			{
				tstring dataFolder= GetAppPath()+TEXT("data\\icon\\")+tstring(buf)+".ico";
				DWORD dwsize = *(DWORD*)(&it1->second->pIconData[0]);
				WriteDataFile(dataFolder, it1->second->pIconData+4,false,dwsize);
			}		
			SetGameDeskTopLnk(vctNeedDesktopLnk[dwLoop]);
		}
		//删除快捷方式
		DelGameDeskTopLnkExcept(vctNeedDesktopLnk);

		dwGameCount = dwHLevelGameCount + dwLLevelGameCount;
		if (dwUptListSize == 0 //没有文件更新
			&& bIsDeleteGame == FALSE //文件没有删除
			)
		{
			return TRUE;
		}
		char strKey[MAX_PATH] = {0};//注册表里游戏信息的名称；
		char strData[1024] = {0};   //注册表里游戏信息值；
		SHDeleteKey(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID);//删除游戏表里以往的游戏信息；
		DWORD dwTotalNeedSize = sizeof(_packageheader) + 2*sizeof(DWORD) + dwGameCount*sizeof(tagGameInfo);
		//分配内存
		pGameInfoMem = new char[dwTotalNeedSize];
		InitPackage((_packageheader*)pGameInfoMem, CMD_GAME_GETSOME);
		_packageheader* pheader = (_packageheader*)pGameInfoMem;
		CPackageHelper GameInfoPacket(pGameInfoMem);
		GameInfoPacket.pushDWORD((DWORD)1); //flag
		GameInfoPacket.pushDWORD(dwGameCount);//size
		tagGameInfo* pGameInfoItem = NULL;
		//写入高等级游戏
		BOOL bNeedReadFile = FALSE;
		for(DWORD dwLoop = 0; dwLoop < m_VecGameInfo.size(); ++dwLoop)
		{
			map<DWORD, PST_GAME_INDEX_INFO>::iterator it = m_mapGameIndexInfo.find(m_VecGameInfo[dwLoop]->GID);
			if (it != m_mapGameIndexInfo.end())
			{
				pGameInfoItem = m_VecGameInfo[dwLoop];
			}
			else
			{
				bNeedReadFile = TRUE;
				continue;
			}
			GameInfoPacket.pushDWORD(pGameInfoItem->GID);
			GameInfoPacket.pushDWORD(pGameInfoItem->PID);
			GameInfoPacket.pushString(pGameInfoItem->NAME, strlen(pGameInfoItem->NAME));//游戏名称；
			GameInfoPacket.pushString(pGameInfoItem->IDCCLASS, strlen(pGameInfoItem->IDCCLASS));//官方游戏类别；
			GameInfoPacket.pushString(pGameInfoItem->GAMEEXE, strlen(pGameInfoItem->GAMEEXE));//游戏官方EXE
			GameInfoPacket.pushString(pGameInfoItem->PARAM, strlen(pGameInfoItem->PARAM));//游戏运行参数
			GameInfoPacket.pushDWORD(pGameInfoItem->SIZE);//游戏大小;
			GameInfoPacket.pushDWORD(pGameInfoItem->DESKLNK);//游戏是否显示到桌面快捷方式
			GameInfoPacket.pushDWORD(pGameInfoItem->TOOLBAR);//是否显示在工具栏上;
			GameInfoPacket.pushString(pGameInfoItem->SVRPATH, strlen(pGameInfoItem->SVRPATH));//游戏在服务器上的路径
			GameInfoPacket.pushString(pGameInfoItem->CLIPATH, strlen(pGameInfoItem->CLIPATH));//游戏在客户机上的路径
			GameInfoPacket.pushDWORD(pGameInfoItem->PRIORITY);//游戏等级
			GameInfoPacket.pushDWORD(pGameInfoItem->SHOWPRIORITY);//游戏在菜单上的显示优先级
			GameInfoPacket.pushString(pGameInfoItem->MATCHFILE, strlen(pGameInfoItem->MATCHFILE));//游戏自动搜索特征文件
			GameInfoPacket.pushString(pGameInfoItem->SAVEFILTER, strlen(pGameInfoItem->SAVEFILTER));//游戏存档信息
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCUPTDATE);//游戏在中心服务器的更新时间
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRUPTDATE );//游戏在网吧服务器的更新时间
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCVER );//游戏在中心服务器上的版本
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRVER);//游戏在网吧服务器上的版本
			GameInfoPacket.pushDWORD(pGameInfoItem->AUTOUPT);//游戏是否自动从服务器上更新
			GameInfoPacket.pushDWORD(pGameInfoItem->I8PLAY);//游戏需要强制更新
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCCLICK);//游戏全网点率
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRCLICK);//游戏网吧点率
			GameInfoPacket.pushDWORD(pGameInfoItem->ADDDATE);//游戏添加日期
			GameInfoPacket.pushString(pGameInfoItem->GAMELINK, strlen(pGameInfoItem->GAMELINK));//官网
			GameInfoPacket.pushDWORD(pGameInfoItem->RUNTYPE);//游戏运行方式
			GameInfoPacket.pushDWORD(pGameInfoItem->IFUPDATE);//游戏是否更新
			GameInfoPacket.pushDWORD(pGameInfoItem->IFUPDATE);//游戏是否显示
			GameInfoPacket.pushString(pGameInfoItem->VID, strlen(pGameInfoItem->VID)); //游戏所在虚拟盘ID
			GameInfoPacket.pushDWORD(pGameInfoItem->STATUS);//游戏下载状态；
			GameInfoPacket.pushString(pGameInfoItem->COMMENT, strlen(pGameInfoItem->COMMENT)); //comment;

			//写入注册表
			_stprintf(strKey, "%d", pGameInfoItem->GID);
			_stprintf(strData, "name=%s;mode=%d;remotepath=%s;localpath=%s",
				pGameInfoItem->NAME, pGameInfoItem->RUNTYPE, pGameInfoItem->VDPATH, pGameInfoItem->CLIPATH);
			SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, strKey, REG_SZ, 
				strData, lstrlen(strData));	
		}
		//写入低等级游戏
		for(DWORD dwLoop = 0; dwLoop < m_VecDelGameInfo.size(); ++dwLoop)
		{
			map<DWORD, PST_GAME_INDEX_INFO>::iterator it = m_mapGameIndexInfo.find(m_VecDelGameInfo[dwLoop]->GID);
			if (it != m_mapGameIndexInfo.end())
			{
				pGameInfoItem = m_VecDelGameInfo[dwLoop];
			}
			else
			{
				bNeedReadFile = TRUE;
				continue;
			}
			GameInfoPacket.pushDWORD(pGameInfoItem->GID);
			GameInfoPacket.pushDWORD(pGameInfoItem->PID);
			GameInfoPacket.pushString(pGameInfoItem->NAME, strlen(pGameInfoItem->NAME));//游戏名称；
			GameInfoPacket.pushString(pGameInfoItem->IDCCLASS, strlen(pGameInfoItem->IDCCLASS));//官方游戏类别；
			GameInfoPacket.pushString(pGameInfoItem->GAMEEXE, strlen(pGameInfoItem->GAMEEXE));//游戏官方EXE
			GameInfoPacket.pushString(pGameInfoItem->PARAM, strlen(pGameInfoItem->PARAM));//游戏运行参数
			GameInfoPacket.pushDWORD(pGameInfoItem->SIZE);//游戏大小;
			GameInfoPacket.pushDWORD(pGameInfoItem->DESKLNK);//游戏是否显示到桌面快捷方式
			GameInfoPacket.pushDWORD(pGameInfoItem->TOOLBAR);//是否显示在工具栏上;
			GameInfoPacket.pushString(pGameInfoItem->SVRPATH, strlen(pGameInfoItem->SVRPATH));//游戏在服务器上的路径
			GameInfoPacket.pushString(pGameInfoItem->CLIPATH, strlen(pGameInfoItem->CLIPATH));//游戏在客户机上的路径
			GameInfoPacket.pushDWORD(pGameInfoItem->PRIORITY);//游戏等级
			GameInfoPacket.pushDWORD(pGameInfoItem->SHOWPRIORITY);//游戏在菜单上的显示优先级
			GameInfoPacket.pushString(pGameInfoItem->MATCHFILE, strlen(pGameInfoItem->MATCHFILE));//游戏自动搜索特征文件
			GameInfoPacket.pushString(pGameInfoItem->SAVEFILTER, strlen(pGameInfoItem->SAVEFILTER));//游戏存档信息
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCUPTDATE);//游戏在中心服务器的更新时间
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRUPTDATE );//游戏在网吧服务器的更新时间
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCVER );//游戏在中心服务器上的版本
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRVER);//游戏在网吧服务器上的版本
			GameInfoPacket.pushDWORD(pGameInfoItem->AUTOUPT);//游戏是否自动从服务器上更新
			GameInfoPacket.pushDWORD(pGameInfoItem->I8PLAY);//游戏需要强制更新
			GameInfoPacket.pushDWORD(pGameInfoItem->IDCCLICK);//游戏全网点率
			GameInfoPacket.pushDWORD(pGameInfoItem->SVRCLICK);//游戏网吧点率
			GameInfoPacket.pushDWORD(pGameInfoItem->ADDDATE);//游戏添加日期
			GameInfoPacket.pushString(pGameInfoItem->GAMELINK, strlen(pGameInfoItem->GAMELINK));//官网
			GameInfoPacket.pushDWORD(pGameInfoItem->RUNTYPE);//游戏运行方式
			GameInfoPacket.pushDWORD(pGameInfoItem->IFUPDATE);//游戏是否更新
			GameInfoPacket.pushDWORD(pGameInfoItem->IFUPDATE);//游戏是否显示
			GameInfoPacket.pushString(pGameInfoItem->VID, strlen(pGameInfoItem->VID)); //游戏所在虚拟盘ID
			GameInfoPacket.pushDWORD(pGameInfoItem->STATUS);//游戏下载状态；
			GameInfoPacket.pushString(pGameInfoItem->COMMENT, strlen(pGameInfoItem->COMMENT)); //comment;
			//GameInfoPacket.pushString(pGameInfoItem->GAMEAREA, strlen(pGameInfoItem->GAMEAREA));
			//GameInfoPacket.pushString(pGameInfoItem->REGURL, strlen(pGameInfoItem->REGURL));
			//GameInfoPacket.pushString(pGameInfoItem->PAYURL, strlen(pGameInfoItem->PAYURL));
			//GameInfoPacket.pushString(pGameInfoItem->VIDEOAREA, strlen(pGameInfoItem->VIDEOAREA));
			//写入注册表
			_stprintf(strKey, "%d", pGameInfoItem->GID);
			_stprintf(strData, "name=%s;mode=%d;remotepath=%s;localpath=%s",
				pGameInfoItem->NAME, pGameInfoItem->RUNTYPE, pGameInfoItem->VDPATH, pGameInfoItem->CLIPATH);
			SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, strKey, REG_SZ, 
				strData, lstrlen(strData));	
		}
		//写入文件
		DWORD dwWriteSize = GameInfoPacket.GetOffset();
		pheader->Length = dwWriteSize;
		WriteDataFile(strDataFile, pGameInfoMem, true, dwWriteSize);
		//释放内存
		delete[] pGameInfoMem;
		pGameInfoMem = NULL;
		//重读文件，保持内存数据与文件数据一致
		if( ReadGameInfoFile() == FALSE)
			return FALSE;
		bNeedNotifyFrameWindow = true;
		return TRUE;
	}

	bool CBootRunPlug::GetOfflineGamesIcon()
	{
#define  ICON_FILE_NAME TEXT("data\\IconPackage.zip")
#define  DEFAULT_FILED_LENGTH 255
		//变量定义
		HANDLE	hGameIconsFile = INVALID_HANDLE_VALUE;
		char    *pGameIconsMem = NULL;
		DWORD   dwGameIconsFileSize = 0;
		tstring strFileName= GetAppPath()+TEXT("data\\gameicon.zip");
		//检测图标数据在内存中是否存在
		if (m_mapGameIcon.empty())
		{
			//打开文件			
			hGameIconsFile = ::CreateFile(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hGameIconsFile != INVALID_HANDLE_VALUE)
			{
				//获取文件大小
				dwGameIconsFileSize = GetFileSize(hGameIconsFile, NULL);
				if(dwGameIconsFileSize == INVALID_FILE_SIZE)
				{
					//文件大小获取失败
					m_pLogger->WriteLog(LM_INFO, TEXT("游戏图标文件大小获取失败."));
					CloseHandle(hGameIconsFile);
					return false;
				}
				//分配内存
				pGameIconsMem = new char[dwGameIconsFileSize + 1];
				//读入文件
				DWORD dwReadBytes = 0;
				if (!ReadFile(hGameIconsFile, pGameIconsMem, dwGameIconsFileSize, &dwReadBytes, NULL) || dwReadBytes != dwGameIconsFileSize || dwGameIconsFileSize < 30)
				{
					//文件读取失败
					m_pLogger->WriteLog(LM_INFO, TEXT("游戏图标文件读取失败."));
					CloseHandle(hGameIconsFile);
					delete[] pGameIconsMem;
					pGameIconsMem = NULL;
					return false;
				}
				pGameIconsMem[dwReadBytes] = 0;
				//解析内容
				DWORD dwLastPackage = 0;
				char* pTemp = pGameIconsMem;
				DWORD dwTotal = 0;
				while (dwLastPackage == 0 && pTemp < pGameIconsMem + dwGameIconsFileSize)	//dwLastPackage=1代表最后一包
				{
					CPackageHelper package(pTemp);
					DWORD dwflag  = package.popDWORD();			//状态
					dwLastPackage = package.popDWORD();			//是否是最后一包
					DWORD dwCount = package.popDWORD();			//本包ICON数
					dwTotal += dwCount;
					for (DWORD idx=0; idx<dwCount; idx++)
					{
						char gid[DEFAULT_FILED_LENGTH] = {0};						
						package.popString(gid);
						int nOffset = package.GetOffset();
						int nLength = package.popDWORD();
						//分配图标空间，并拷贝
						char *pIconData = new char[nLength + sizeof(DWORD) + 1];					
						CopyMemory(pIconData, pTemp + nOffset, nLength + sizeof(DWORD));
						PST_ICON_ITEM pIconItem = new ST_ICON_ITEM;
						pIconItem->pIconData = pIconData;
						pIconItem->dwCRC32Data = CalBufCRC32((BYTE*)(pIconData + sizeof(DWORD)), nLength);		
						m_mapGameIcon.insert(std::make_pair(atoi(gid), (PST_ICON_ITEM)(pIconItem)));
						package.SetOffset(nOffset + nLength + 4);
					}
					pTemp += package.GetOffset();
				}
				//关闭文件句柄
				CloseHandle(hGameIconsFile);
				//释放内存
				delete[] pGameIconsMem;
				pGameIconsMem = NULL;
			}			
		}
		return true;
	}

	//bool CBootRunPlug::GetGamesIcon()
	//{
	//#define  ICON_FILE_NAME TEXT("data\\IconPackage.zip")
	//	m_pLogger->WriteLog(LM_INFO, TEXT("start to Get online GamesIcon."));
	//char *pData = NULL;
	//int  nSize = 0;
	//bool bRecFlag =true;
	//char buf[1024] = {0};
	//DWORD dwLastPackage = 0;
	//char *gid= new char[MAX_PATH];
	//char *ico= new char[64*1024];
	//_packageheader* pheader = (_packageheader*)buf;
	//InitPackage(pheader, CMD_GAME_GETICONLIST);
	//	CPackageHelper out(buf);
	//pheader->Length = out.GetOffset();
	//int nLastTime = _time32(NULL);				
	//for (size_t idx=0; idx<IconList.size(); idx++)
	//{
	//	CoTaskMemFree(IconList[idx].pData);
	//}
	//IconList.clear();
	//m_mapGameIcon.clear();
	//try
	//{
	//	{
	//		CAutoLock<ICommunication> lock(m_pPlugMgr->GetCommunication());
	//		while (dwLastPackage == 0)//1代表最后一包
	//		{
	//			if (!ExecCmd(buf, pheader->Length, pData, nSize, true, false))
	//			{
	//				m_pLogger->WriteLog(LM_INFO, "Get GamesIcon Data Fail.");
	//				bRecFlag = false;
	//				break;
	//			}
	//			CPackageHelper inpackage(pData);
	//			DWORD dwflag  = inpackage.popDWORD();	//状态
	//			dwLastPackage = inpackage.popDWORD();	//是否是最后一包
	//			DWORD dwCount = inpackage.popDWORD();	//本包ICON数
	//			if (dwCount == 0 && dwLastPackage == 0 )
	//			{
	//				m_pLogger->WriteLog(LM_INFO, "Get GamesIcon Data Fail.");
	//				bRecFlag = false;
	//				break;
	//			}
	//			for (DWORD i= 0;i<dwCount;i++)
	//			{
	//				inpackage.popString(gid);//Gid				
	//				DWORD offset = inpackage.GetOffset();
	//				m_mapGameIcon.insert(make_pair(atoi(gid),pData + offset));
	//				DWORD length = inpackage.popDWORD();				
	//				offset = inpackage.GetOffset();
	//				inpackage.SetOffset(offset + length);
	//			}
	//			tagIconItem Item;
	//			Item.pData = pData;
	//			Item.dwSize = nSize;
	//			IconList.push_back(Item);
	//			pheader->Length = 0;
	//		}
	//	}
	//	if (bRecFlag)//接收游戏数据包是否完整
	//	{
	//		tstring FileName= GetAppPath()+TEXT("data\\gameicon.zip");
	//		DWORD dwByteWrite = 0;
	//		bool flag=true;
	//		HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
	//			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//		if (hFile == INVALID_HANDLE_VALUE)		
	//			flag = false;					
	//		for (size_t idx=0; idx<IconList.size(); idx++)
	//		{
	//			if (flag)
	//			{
	//				if (!WriteFile(hFile,  IconList[idx].pData, IconList[idx].dwSize, &dwByteWrite, NULL))					
	//					flag = false;					
	//			}			
	//		}
	//		SetEndOfFile(hFile);
	//		CloseHandle(hFile);
	//		if (flag)
	//		{
	//			bool bNeedInProtUpdate = m_Hy.MountDosChar((char*)(FileName.c_str()));
	//			if (bNeedInProtUpdate)
	//			{				
	//				tstring strTmp("\\\\.\\");
	//				strTmp += m_Hy.GetDevicePath();
	//				tstring device= FileName;
	//				device.replace(0, 2, strTmp);

	//				BOOL ret = CopyFileA(FileName.c_str(), device.c_str(), FALSE);
	//				if (!ret)
	//				{
	//					m_pLogger->WriteLog(LM_INFO, TEXT("copy %s :%d."), device.c_str(), GetLastError());
	//				}				
	//				m_Hy.UMountDosChar((char*)(FileName.c_str()));
	//			}
	//		}
	//	}
	//}
	//catch (...) 
	//{
	//	m_pLogger->WriteLog(LM_INFO, TEXT("Rec GameIcon ErrorX.")); 
	//}
	//delete []gid;
	//delete []ico;
	//m_pLogger->WriteLog(LM_INFO, "Get GamesIcon Data Success.");
	//m_pLogger->WriteLog(LM_INFO, TEXT("end to Get online GamesIcon.\r\n"));
	//return bRecFlag;
	//}   

	//读取游戏图标文件
	BOOL CBootRunPlug::ReadGameIconFile()
	{
		//变量定义
		HANDLE	hGameIconsFile = INVALID_HANDLE_VALUE;
		char    *pGameIconsMem = NULL;
		DWORD   dwGameIconsFileSize = 0;
		tstring strFileName= GetAppPath()+TEXT("data\\gameicon.zip");
		//清空内存数据
		map<DWORD, PST_ICON_ITEM>::iterator icoIt = m_mapGameIcon.begin();
		for (; icoIt != m_mapGameIcon.end(); icoIt++)
		{
			delete[] icoIt->second->pIconData;
			delete icoIt->second;
			icoIt->second = NULL;
		}
		m_mapGameIcon.clear();
		//打开文件			
		hGameIconsFile = ::CreateFile(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hGameIconsFile != INVALID_HANDLE_VALUE)
		{
			//获取文件大小
			dwGameIconsFileSize = GetFileSize(hGameIconsFile, NULL);
			if(dwGameIconsFileSize == INVALID_FILE_SIZE)
			{
				//文件大小获取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("游戏图标文件大小获取失败."));
				CloseHandle(hGameIconsFile);
				return FALSE;
			}
			//分配内存
			pGameIconsMem = new char[dwGameIconsFileSize + 1];
			//读入文件
			DWORD dwReadBytes = 0;
			if (!ReadFile(hGameIconsFile, pGameIconsMem, dwGameIconsFileSize, &dwReadBytes, NULL) || dwReadBytes != dwGameIconsFileSize || dwGameIconsFileSize < 30)
			{
				//文件读取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("游戏图标文件读取失败."));
				CloseHandle(hGameIconsFile);
				delete[] pGameIconsMem;
				pGameIconsMem = NULL;
				return FALSE;
			}
			pGameIconsMem[dwReadBytes] = 0;
			//解析内容
			DWORD dwLastPackage = 0;
			char* pTemp = pGameIconsMem;
			DWORD dwTotal = 0;

			try
			{
				while (dwLastPackage == 0 && pTemp < pGameIconsMem + dwGameIconsFileSize)	//dwLastPackage=1代表最后一包
				{
					CPackageHelper package(pTemp);
					DWORD dwflag  = package.popDWORD();			//状态
					dwLastPackage = package.popDWORD();			//是否是最后一包
					DWORD dwCount = package.popDWORD();			//本包ICON数
					dwTotal += dwCount;
					for (DWORD idx=0; idx<dwCount; idx++)
					{
						char gid[DEFAULT_FILED_LENGTH] = {0};						
						package.popString(gid);
						int nOffset = package.GetOffset();
						int nLength = package.popDWORD();
						//分配图标空间，并拷贝
						char *pIconData = new char[nLength + sizeof(DWORD) + 1];					
						CopyMemory(pIconData, pTemp + nOffset, nLength + sizeof(DWORD));
						PST_ICON_ITEM pIconItem = new ST_ICON_ITEM;
						pIconItem->pIconData = pIconData;
						pIconItem->dwCRC32Data = CalBufCRC32((BYTE*)(pIconData + sizeof(DWORD)), nLength);		
						m_mapGameIcon.insert(std::make_pair(atoi(gid), (PST_ICON_ITEM)(pIconItem)));
						package.SetOffset(nOffset + nLength + 4);
					}
					pTemp += package.GetOffset();
				}
			}
			catch(...)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("读取游戏图标文件发生异常，从服务器重新下载"));
				//清空内存数据
				map<DWORD, PST_ICON_ITEM>::iterator icoIt = m_mapGameIcon.begin();
				for (; icoIt != m_mapGameIcon.end(); icoIt++)
				{
					delete[] icoIt->second->pIconData;
					delete icoIt->second;
					icoIt->second = NULL;
				}
				m_mapGameIcon.clear();
			}

			//关闭文件句柄
			CloseHandle(hGameIconsFile);
			//释放内存
			delete[] pGameIconsMem;
			pGameIconsMem = NULL;
			return TRUE;
		}
		return FALSE;
	}

	//获取游戏图标数据
	BOOL CBootRunPlug::GetGamesIcon()
	{
#define  ICON_FILE_NAME TEXT("data\\IconPackage.zip")
#define  DEFAULT_FILED_LENGTH 255
		m_pLogger->WriteLog(LM_INFO, TEXT("start to Get online GamesIcon."));
		//变量定义
		HANDLE	hGameIconsFile = INVALID_HANDLE_VALUE;
		char    *pGameIconsMem = NULL;
		tstring strFileName= GetAppPath()+TEXT("data\\gameicon.zip");
		//检测图标数据在内存中是否存在
		if (m_mapGameIcon.empty())
		{
			ReadGameIconFile();          
		}
		//获取图标索引
		//定义变量
		char szSendBuffer[1024] = {0};
		char *pRecvBuffer = NULL;
		int  nRecvDataSize = 0;		
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_GAME_REQ_ICONS_IDX);
		CPackageHelper SendPacket(szSendBuffer);
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取图标索引文件失败\r\n");
			return FALSE;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		//等待更新图标列表
		list<DWORD>   UptIconList;
		//服务器图标列表
		map<DWORD, DWORD> mapSvrIconList;
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
			//获取图标数目
			DWORD dwIconCount = RecvPacket.popDWORD();
			DWORD dwGameId = 0, dwIconCRC = 0;
			//查找要更新的图标
			for (DWORD dwLoop = 0; dwLoop < dwIconCount; ++dwLoop)
			{
				dwGameId  = RecvPacket.popDWORD();
				dwIconCRC = RecvPacket.popDWORD();
				//加入列表
				mapSvrIconList.insert(make_pair(dwGameId, dwIconCRC));
				map<DWORD, PST_ICON_ITEM>::iterator it = m_mapGameIcon.find(dwGameId);
				if(it != m_mapGameIcon.end())
				{
					if (dwIconCRC != it->second->dwCRC32Data) 
						UptIconList.push_back(dwGameId);
				}	
				else UptIconList.push_back(dwGameId);
			}
		}
		CoTaskMemFree(pRecvBuffer);
		//请求更新图标
		DWORD dwUptListSize = UptIconList.size();
		DWORD dwUptCount = dwUptListSize > 0 ? dwUptListSize/10 : 0;
		dwUptCount += ((dwUptListSize%10) == 0 ? 0 : 1);

		for (DWORD dwLoop = 0; dwLoop < dwUptCount; ++dwLoop)
		{
			//计算本次更新个数
			DWORD dwCurrentUptCount = (dwUptListSize - 10*dwLoop >= 10) ? 10 : dwUptListSize%10;
			//构造数据
			pPacketHeader = (_packageheader*)szSendBuffer;
			InitPackage(pPacketHeader, CMD_GAME_REQ_ICONS_DATA);
			CPackageHelper ReqIconDataPacket(szSendBuffer);
			ReqIconDataPacket.pushDWORD(dwCurrentUptCount);
			for (DWORD i = 0; i < dwCurrentUptCount; ++i)
			{
				DWORD dwGameId = UptIconList.front();
				UptIconList.pop_front();
				ReqIconDataPacket.pushDWORD(dwGameId);
			}
			pPacketHeader->Length = ReqIconDataPacket.GetOffset();
			//执行命令
			if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
			{
				m_pLogger->WriteLog(LM_INFO, "获取图标文件数据失败\r\n");
				return FALSE;
			}    
			//返回判断
			CPackageHelper ResIconDataPacket(pRecvBuffer);
			dwRetCode =ResIconDataPacket.popDWORD();
			if ( dwRetCode == CMD_RET_SUCCESS )
			{
				DWORD dwLastPackage = ResIconDataPacket.popDWORD();
				DWORD dwIconCount   = ResIconDataPacket.popDWORD();
				DWORD dwGameId      = 0;
				char  szGameGid[MAX_PATH];
				DWORD dwGidLen      = 0;
				PST_ICON_ITEM pIconItem = NULL;				
				for (DWORD dwIdx = 0; dwIdx < dwIconCount; ++dwIdx)
				{  					               
					ResIconDataPacket.popString(szGameGid);
					DWORD dwOffset = ResIconDataPacket.GetOffset();  
					dwGameId = (DWORD)atol(szGameGid);	
					map<DWORD, PST_ICON_ITEM>::iterator it = m_mapGameIcon.find(dwGameId);
					if(it != m_mapGameIcon.end())
					{
						//删除以前的内存
						pIconItem = it->second;
						delete []pIconItem->pIconData;
						pIconItem->pIconData = NULL;
						//获取图标大小						
						DWORD dwIconSize = ResIconDataPacket.popDWORD();
						//分配新内存
						pIconItem->pIconData = new char[dwIconSize + sizeof(DWORD)];
						//拷贝数据
						CopyMemory(pIconItem->pIconData, pRecvBuffer + dwOffset, dwIconSize + sizeof(DWORD));
						//重新计算CRC
						pIconItem->dwCRC32Data = CalBufCRC32((BYTE*)(pIconItem->pIconData + sizeof(DWORD)), dwIconSize);
						//设置偏移
						dwOffset = ResIconDataPacket.GetOffset();
						ResIconDataPacket.SetOffset(dwOffset + dwIconSize);
					}	
					else
					{
						pIconItem  = new ST_ICON_ITEM;
						//获取图标大小
						DWORD dwIconSize = ResIconDataPacket.popDWORD();
						//分配新内存
						pIconItem->pIconData = new char[dwIconSize + sizeof(DWORD)];
						//拷贝数据
						CopyMemory(pIconItem->pIconData, pRecvBuffer + dwOffset, dwIconSize + sizeof(DWORD));
						//设置偏移
						dwOffset = ResIconDataPacket.GetOffset();
						ResIconDataPacket.SetOffset(dwOffset + dwIconSize);
						//计算CRC
						pIconItem->dwCRC32Data = CalBufCRC32((BYTE*)(pIconItem->pIconData + sizeof(DWORD)), dwIconSize);
						//插入节点						
						m_mapGameIcon.insert(make_pair(dwGameId, (PST_ICON_ITEM)(pIconItem)));
					}
				}
			}
			CoTaskMemFree(pRecvBuffer);
		}
		//写入文件
		//       if (dwUptListSize == 0) return TRUE;
		//统计字节数
		DWORD dwTotalIconSize = 0;
		DWORD dwIconCount = 0;
		DWORD dwGidSize   = 0;
		DWORD dwIconSize  = 0;
		char  szGidBuffer[255] = {0};
		BOOL  bNeedReadFile = FALSE;
		BOOL  bIsDeleteIcon = FALSE;
		for (map<DWORD, PST_ICON_ITEM>::iterator it = m_mapGameIcon.begin(); it != m_mapGameIcon.end(); ++it)
		{
			//查找是否在服务器列表中
			map<DWORD, DWORD>::iterator svrIt = mapSvrIconList.find(it->first);
			if(svrIt == mapSvrIconList.end()) 
			{
				bIsDeleteIcon = TRUE;
				continue;
			}			
			//获取GID长度
			sprintf(szGidBuffer, "%ld", it->first);
			dwGidSize = strlen(szGidBuffer) + 1;
			dwIconSize = sizeof(DWORD) + *(DWORD*)(it->second->pIconData); 
			dwTotalIconSize += sizeof(DWORD) + dwGidSize + dwIconSize;
			dwIconCount++;
		}
		if (dwUptListSize == 0  //没有更新个数
			&& bIsDeleteIcon == FALSE //服务器图标个数和本地图标个数相等
			)
		{
			return TRUE;
		}
		//分配内存
		DWORD dwTotalNeedSize = sizeof(_packageheader) + dwTotalIconSize + 3*sizeof(DWORD);
		pGameIconsMem = new char[dwTotalNeedSize];
		DWORD dwOffset = 0;
		//协议头
		InitPackage((_packageheader*)pGameIconsMem, CMD_GAME_REQ_ICONS_DATA);
		_packageheader* pheader = (_packageheader*)pGameIconsMem;
		CPackageHelper FilePackage(pGameIconsMem);
		//标志位
		FilePackage.pushDWORD(0);
		//最后一块
		FilePackage.pushDWORD(1);
		//本包ICON数
		FilePackage.pushDWORD(dwIconCount);
		//拷贝图标数据
		for (map<DWORD, PST_ICON_ITEM>::iterator it = m_mapGameIcon.begin(); it != m_mapGameIcon.end(); ++it)
		{
			//查找是否在服务器列表中
			map<DWORD, DWORD>::iterator svrIt = mapSvrIconList.find(it->first);
			if(svrIt == mapSvrIconList.end())
			{
				bNeedReadFile = TRUE;
				continue;
			}
			//获取GID数据
			sprintf(szGidBuffer, "%ld", it->first);
			dwGidSize = strlen(szGidBuffer);
			dwIconSize = *(DWORD*)(it->second->pIconData); 
			//推入GID信息
			FilePackage.pushString(szGidBuffer,dwGidSize);
			dwOffset = FilePackage.GetOffset();
			//拷贝Icon信息
			CopyMemory(pGameIconsMem + dwOffset, it->second->pIconData, sizeof(DWORD) + dwIconSize);
			FilePackage.SetOffset(dwOffset + sizeof(DWORD) + dwIconSize);
		}
		dwOffset = FilePackage.GetOffset();
		pheader->Length = dwOffset;

		//文件操作
		WriteDataFile(strFileName, pGameIconsMem, true, dwOffset);
		//释放内存
		delete[] pGameIconsMem;
		pGameIconsMem = NULL;
		if (bNeedReadFile) 
		{
			if (ReadGameIconFile() == FALSE)
				return FALSE; 
		}
		bNeedNotifyFrameWindow = true;
		return TRUE;
	}

	void CBootRunPlug::GetURLFavorite()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("start to Get URLFavorite."));
		LPSTR pData = NULL;
		int   nSize = 0;
		try
		{
			if (!ExecSimpleCmd(CMD_FAVORITE_GETLIST, pData, nSize))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Get URL Favorite Fail."));
				return ;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("Get URL Favorite Success."));

			tstring datafile = GetAppPath()+ TEXT("data\\WebSiteInfo.dat");
			WriteDataFile(datafile, pData,true);
		}
		catch (...) { m_pLogger->WriteLog(LM_INFO, TEXT("Get URLFavorite Cmd Have ErrorX.")); }
		//释放内存
		CoTaskMemFree(pData);
		m_pLogger->WriteLog(LM_INFO, TEXT("end Get URLFavorite.\r\n"));
	}
	bool CBootRunPlug::GetVDiskInfo()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("get vdiskInfo."));
		//定义变量
		char szSendBuffer[1024] = {0};
		char *pRecvBuffer = NULL;
		int  nRecvDataSize = 0;		
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_GET_VDISK_CONFIG);
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取虚拟盘配置信息失败");
			return FALSE;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode = RecvPacket.popDWORD();
		//把收到正确的数据写到离线文件
		tstring datafile = GetAppPath()+ TEXT("data\\VDisk2.dat");
		WriteDataFile(datafile, pRecvBuffer, true);
		//释放内存
		CoTaskMemFree(pRecvBuffer);
		LoadVDiskInfo();
		m_pLogger->WriteLog(LM_INFO, TEXT("end of get VDiskInfo.\r\n"));
		return true;
	}
	bool CBootRunPlug::RefreshVDisk()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("start refresh vdisk."));
		if (/*CheckVDiskService() || */!LoadVDiskDll())
			return false;

		//先删除虚拟盘临时数据。
		map<string, string>::iterator itVdir = m_mapOption.find(OPT_M_VDDIR);
		if (itVdir != m_mapOption.end())
		{
			if (itVdir->second.size())
				m_strVDTmpPath = itVdir->second;

			if (*m_strVDTmpPath.rbegin() != TEXT('\\'))
				m_strVDTmpPath += TEXT('\\');
		}
		SDG_CreateDirectory(m_strVDTmpPath.c_str());

		string strVdir = m_strVDTmpPath + "*.*";
		SHFILEOPSTRUCT fo;
		memset(&fo, 0, sizeof(fo));
		fo.pFrom = strVdir.c_str();
		fo.wFunc = FO_DELETE;
		fo.fFlags = FOF_NOCONFIRMATION | FOF_SILENT |FOF_NOERRORUI;
		SHFileOperation(&fo);
		m_pLogger->WriteLog(LM_INFO, TEXT("Create VDisk Temp Path:%s."), m_strVDTmpPath.c_str());
		//m_strVDTmpPath += TEXT("I8");
		//		DWORD n1 =inet_addr("192.168.157.54");
		bool bNeedWriteVersion = true;	
		string str;
		bool  flag = true;
		map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.begin();

		for(;it != m_mapVDiskInfo.end();it++)
		{			
			tagVDisk* pVDisk = it->second;

			IN_ADDR addr = {0};
			addr.s_addr = pVDisk->IP;
			tstring sIP(inet_ntoa(addr));
			if (pVDisk->Type == VDISK_I8DESK)
			{
				switch (pVDisk->StartType)
				{
				case vdBoot:
					{
						if (!pVDisk->bExist)
						{
							if (flag)
							{
								//计算虚拟盘服务器IP
								DWORD dwVDiskIP = (DWORD)0;
								bool  bNeedCheck = true;
								if (strcmp(pVDisk->SID, MAIN_SERVER_SID) != 0)
								{
									dwVDiskIP = pVDisk->IP;
								}
								else
								{
									for (size_t iLoop = 0; iLoop < m_VctServerCfg.size(); ++iLoop)
									{	
										dwVDiskIP = inet_addr(m_VctServerCfg[iLoop]->szVDiskIP);
										if (_ConnectTcpServer(dwVDiskIP, pVDisk->Port))
										{
											bNeedCheck = false;
											break;
										}	
										else
										{		
											in_addr iAddr;
											CopyMemory(&iAddr, &dwVDiskIP, 4);
											m_pLogger->WriteLog(LM_INFO, TEXT("check connect to vdisk server fail. IP:%s port:%d"), inet_ntoa(iAddr), pVDisk->Port);
										}
									}
									if (bNeedCheck) continue;
								}
								if (!bNeedCheck || _ConnectTcpServer(dwVDiskIP, pVDisk->Port))
								{				
									DWORD dwErr = m_pfnDiskMount(0, dwVDiskIP, 
										htons((WORD)pVDisk->Port), pVDisk->CliDrv, m_strVDTmpPath.c_str());
									if (dwErr != 0)
										bNeedWriteVersion = false;

									in_addr iAddr;
									CopyMemory(&iAddr, &dwVDiskIP, 4);
									m_pLogger->WriteLog(LM_INFO, TEXT("mount server:%s:%d  server's driver %C -- client's driver %C : retcode:%d"),
										inet_ntoa(iAddr), pVDisk->Port,pVDisk->SvrDrv, pVDisk->CliDrv, dwErr);
								}
								else
								{		
									in_addr iAddr;
									CopyMemory(&iAddr, &dwVDiskIP, 4);
									m_pLogger->WriteLog(LM_INFO, TEXT("check connect to vdisk server fail. IP:%s port:%d"), inet_ntoa(iAddr), pVDisk->Port);
								}
							}
						}
						else
							m_pLogger->WriteLog(LM_INFO, TEXT("client drive exist.no mount. server:%s:%d  server's driver %C -- client's driver %C"),
							sIP.c_str(),pVDisk->Port,pVDisk->SvrDrv, pVDisk->CliDrv);	  
					}
					break;
				case vdStartMenu:
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("mountDriver's type:vdStartMenu ,no mount. server:%s:%d  server's driver %C -- client's driver %C"),
							sIP.c_str(),pVDisk->Port,pVDisk->SvrDrv,pVDisk->CliDrv);
					}
					break;
				case vdStartGame:
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("mountDriver's type:vdStartGame ,no mount. server:%s:%d  server's driver %C -- client's driver %C"),
							sIP.c_str(),pVDisk->Port,pVDisk->SvrDrv, pVDisk->CliDrv);
					}
					break;
				}
			}
			else
				m_pLogger->WriteLog(LM_INFO, TEXT("Vdisk is not i8VDisk,no mount. client's driver %C"), pVDisk->CliDrv);	

			str += pVDisk->CliDrv;
			str += ':';
			str += pVDisk->SvrDrv;
			str += ";";
		}
		if (str.size())
			SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, "vpartition", REG_SZ, str.c_str(), str.size()-1);
		// 		if (bNeedWriteVersion)
		// 		{
		// 			char szIniFileName[MAX_PATH] = {0};

		// 			string version;
		// 			map<string, string>::iterator it = m_mapOption.find(string("VDiskVersion"));
		// 			if (it != m_mapOption.end())
		// 				version = it->second;
		// 			SDG_GetAppPath(szIniFileName, TRUE);
		// 			lstrcat(szIniFileName, "GameClient.ini");
		// 			::WritePrivateProfileString("SystemSet", "RefDiskVer", version.c_str(), szIniFileName);
		//		}
		if (m_hRefreshDll != NULL)
			FreeLibrary(m_hRefreshDll);

		m_pLogger->WriteLog(LM_INFO, TEXT("end of refersh VDisk.\r\n"));
		return true;
	}

	bool CBootRunPlug::ExecuteTask(BootRunTask ExecType,char* content)
	{
		switch (ExecType)
		{
		case brDeskTop:
			{
				return SetDeskBkPic(content);
			}
			break;
		case brShortcut:
			{
				return SetDeskTopLnk(content);
			}
			break;
		case brIEHomePage:
			{
				//return SetIEHomePage(content);
			}
			break;
		case brCltFile:
			{
				return ExecClientFile(content);
			}
			break;
		case brSvrFile:
			{
				return ExecSvrFile(content);
			}
			break;
		case brDelFile:
			{
				__int64 DelSize =0;
				return DelClientFile(content,&DelSize);
			}
			break;
		case brDelGames:
			{
				return DelClientGames(content);
			}
			break;
		case brSalfCenterXMl:
			{
				break;
			}
		case brUpdateGame:
			{
				return SetUpdateGame(content);
			}
		}
		return false;
	}

	bool CBootRunPlug::SetDeskBkPic(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("Set desktopbgpic."));
		char *pData = NULL;
		int nSize = 0;
		tstring str,strpic, strtype;
		str = tstring(Content);
		size_t pos = str.find('|');
		if (pos == string::npos)
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("desktopbgpic info error:%s.\r\n"), str.c_str());
			return  false;
		}
		else
		{
			strpic = str.substr(0, pos);
			strtype = str.substr(pos+1);
		}
		tstring datafile = GetAppPath()+ strpic;
		//判断是否需要更新
		BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)strpic.c_str(), (char*)datafile.c_str());
		if (bIsNeedUpdate)
		{
			DWORD bufersize = 0;
			byte* bufer = NULL;
			if (DownloadFile(1, fprelPath, strpic, (char**)&bufer, bufersize) == FALSE)
			{
				return false;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("桌面背景下载成功:%s.\r\n"), str.c_str());
			char DataFolder[MAX_PATH] = {0};
			lstrcpy(DataFolder, datafile.c_str());
			PathRemoveFileSpec(DataFolder);
			PathAddBackslash(DataFolder);
			CreateDirectory(DataFolder,NULL);
			WriteDataFile(datafile,(char*)bufer,false,bufersize);
			delete[]bufer;
		}
		std::string Param = GetRunGameExe() + "setdeskbk|" + datafile + ";" + strtype;
		RunProcess(Param.c_str(), Param.c_str());

		return true;
	}

	bool CBootRunPlug::SetDeskTopLnk(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, "create desktop link.");
		tstring str,StrFile, StrIcoFile,StrName;
		stringstream StrPara;

		str = tstring(Content);
		size_t pos;
		pos = str.find('|');
		if (pos == string::npos)
		{
			m_pLogger->WriteLog(LM_INFO, "desktoplnk inf error:%s.\r\n", str.c_str());
			return  false;
		}
		else
		{
			StrName = str.substr(0, pos);
			str = str.substr(pos+1);
			pos = str.find('|');
			if (pos == string::npos)
			{
				m_pLogger->WriteLog(LM_INFO, "desktoplnk inf error:%s.\r\n", str.c_str());
				return  false;
			}
			else
			{

				StrFile = str.substr(0, pos);
				str = str.substr(pos+1);
				pos = str.find('|');
				if (pos != string::npos)
				{
					StrIcoFile = str.substr(0, pos);
					StrPara << str.substr(pos+1);
					//StrPara = str.substr(pos+1);
				}				
			}
		}
		IShellLink* pIShellLink = NULL;
		HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
			IID_IShellLink, (void**) &pIShellLink);
		if (FAILED(hr))
		{
			m_pLogger->WriteLog(LM_INFO, "create IShellLink fail:%d.\r\n", hr);
			return false;
		}
		IPersistFile* pIPersistFile = NULL;
		hr = pIShellLink->QueryInterface(IID_IPersistFile, (void**) &pIPersistFile);
		if (FAILED(hr))
		{
			pIShellLink->Release();
			m_pLogger->WriteLog(LM_INFO, "create IPersistFile fail:%d\r\n", hr);
			return false;
		}
		char path[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, path, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
		PathAddBackslash(path);
		pIShellLink->SetPath(StrFile.c_str());
		pIShellLink->SetArguments(StrPara.str().c_str());
		char DataFolder[MAX_PATH] = {0};
		lstrcpy(DataFolder,StrFile.c_str());
		PathRemoveFileSpec(DataFolder);
		PathAddBackslash(DataFolder);
		if (DataFolder[0] =='\"')
		{
			pIShellLink->SetWorkingDirectory(&DataFolder[1]);
		}
		else
		{
			pIShellLink->SetWorkingDirectory(DataFolder);
		}
		pIShellLink->SetIconLocation(StrIcoFile.c_str(), 0);
		lstrcat(path, StrName.c_str());
		lstrcat(path, ".lnk");
		wchar_t wpath[MAX_PATH] = {0};
		MultiByteToWideChar(GetACP(), 0, path, -1, wpath, MAX_PATH);
		pIPersistFile->Save(wpath, TRUE);
		m_pLogger->WriteLog(LM_INFO, "create desktop link :%s.", path);
		pIPersistFile->Release();
		pIShellLink->Release();
		m_pLogger->WriteLog(LM_INFO, "end create desktop.\r\n");
		return true;
	}

	bool CBootRunPlug::SetIEHomePage(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("Set IEHomePage!"));
		if (strlen(Content)>0)
		{
			tstring root = GetRunGameExe();
			root = root + "iemain|"+tstring(Content)+"|";
			RunProcess(root.c_str(),root.c_str());
			// 			HKEY hAppKey = NULL;
			// 			DWORD dwRet = SHSetValue(HKEY_CURRENT_USER, 
			// 				"SOFTWARE\\Microsoft\\Internet Explorer\\Main",
			// 				"Start Page", REG_SZ, Content, strlen(Content));
			// //			m_pLogger->WriteLog(LM_INFO, "set IEHomePage:%s:%d", Content, dwRet);
			// 
			// 			dwRet = SHSetValue(HKEY_LOCAL_MACHINE, 
			// 				"SOFTWARE\\Microsoft\\Internet Explorer\\Main",
			// 				"Start Page", REG_SZ, Content, strlen(Content));
			//			m_pLogger->WriteLog(LM_INFO, TEXT("set IEHomePage:%s:%d"), Content, dwRet);
		}
		else
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("IEHomePage Info error: %s."),Content);
			return false;
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("end Set IEHomePage. \r\n"));
		return true;
	}

	bool CBootRunPlug::ExecSvrFile(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, "execute Server File.");
		char*pData = NULL;
		int  nSize = 0;
		tstring str,strFile, strPara;
		str = tstring(Content);
		size_t pos = str.find('|');
		if (pos != string::npos)
		{
			strFile = str.substr(0, pos);
			strPara = str.substr(pos+1);
		}
		else
		{
			strFile = str;
			strPara = "";
		}
		char   fname[MAX_PATH];
		char fext[MAX_PATH] ;
		_splitpath(strFile.c_str(),NULL,NULL,fname,fext);
		//判断是否需要更新
		tstring strLocalFilePath =  GetAppPath() +TEXT("data\\BootExec\\");
		strLocalFilePath +=  string(fname)+string(fext);
		BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)strFile.c_str(), (char*)strLocalFilePath.c_str());
		if (bIsNeedUpdate)
		{
			DWORD bufersize = 0;
			byte* bufer = NULL;
			if (DownloadFile(1, fprelPath, strFile, (char**)&bufer, bufersize) == FALSE)
			{
				return false;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("服务器执行文件下载成功:%s.\r\n"), str.c_str());
			//		_stprintf(strFile,"%s%s",fname,fext);
			strFile = GetAppPath() +TEXT("data\\BootExec\\");
			CreateDirectory(strFile.c_str(), NULL);
			strFile += string(fname)+string(fext);
			WriteDataFile(strFile,(char*)bufer,false,bufersize);
			delete []bufer;
		}		
		else
		{
			strFile = GetAppPath() +TEXT("data\\BootExec\\");
			CreateDirectory(strFile.c_str(), NULL);
			strFile += string(fname)+string(fext);
		}
		if (!PathFileExists(strFile.c_str()))
		{
			m_pLogger->WriteLog(LM_INFO, "SvrFile: file :%s not exist.\r\n", strFile.c_str());
			return false;
		}
		tstring root = GetRunGameExe();
		root = root + "|"+strFile+"|"+strPara;
		RunProcess(root.c_str(),root.c_str());
		m_pLogger->WriteLog(LM_INFO, "end Exec SvrFile:%s.\r\n", root.c_str());
		return true;		
	}

	bool CBootRunPlug::ExecClientFile(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("start to execute Client's File."));
		char *pData = NULL;
		int nSize = 0;
		tstring str,strFile, strPara;
		str = tstring(Content);
		size_t pos = str.find('|');
		if (pos != string::npos)
		{
			strFile = str.substr(0, pos);
			strPara = str.substr(pos+1);
		}
		else
		{
			strFile = str;
			strPara = "";
		}
		string strsub;
		strsub = "%root%";
		pos = strFile.find(strsub);
		if (pos != string::npos)
		{
			strFile = GetAppPath() + strFile.substr(pos+strsub.size());
		}
		pos = strFile.rfind("%sys%");
		if (pos != string::npos)
		{
			char bufer[MAX_PATH]={0};
			GetSystemDirectory(bufer,MAX_PATH);
			PathAddBackslash(bufer);
			strFile = string(bufer) + strFile.substr(pos+strsub.size());
		}

		if (!PathFileExists(strFile.c_str()))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("File :%s,not exist.\r\n"), strFile.c_str());
			return false;
		}
		tstring root = GetRunGameExe();
		root = root + "|"+strFile+"|"+strPara;
		RunProcess(root.c_str(),root.c_str());
		m_pLogger->WriteLog(LM_INFO, TEXT("end execute Client's File :%s.\r\n"), root.c_str());
		return true;
	}

	bool CBootRunPlug::DelClientFile(char* Content,__int64* DelSize)
	{
		m_pLogger->WriteLog(LM_INFO, "delete ClientFile.");
		map<string, string>::iterator itx = m_mapOption.find(OPT_M_EXCLUDEDIR);
		std::string excludedir;
		if (itx != m_mapOption.end())
			excludedir = itx->second;

		bool bIsFile =false;

		DWORD Result =	GetFileAttributes(Content);
		if (Result == INVALID_FILE_ATTRIBUTES)
		{
			m_pLogger->WriteLog(LM_INFO, " delete Client File fail: %s.",Content);
			return false;
		}
		else
		{
			if (Result & FILE_ATTRIBUTE_DIRECTORY)
			{
				bIsFile = false;
			}
			else
				bIsFile = true;
		}
		__int64   VirtualLength=0;
		string dev("I8Desk");
		string drv;
		drv += toupper(Content[0]);
		dev += toupper(Content[0]);
		bool inport = false;
		if (m_Hy.MountDosChar((char*)(drv.c_str())))
			inport = true;	
		if (bIsFile)
		{
			string file(Content);
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(file.c_str());
			if (inport)
			{				
				file.replace(0, 2, dev);
				file.insert(0, "\\\\.\\");
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(file.c_str());				
			}

			m_pLogger->WriteLog(LM_INFO, "delete Client File :%s", Content);
		}
		else
		{
			PathAddBackslash(Content);
			SDG_DeleteDirectory(Content,DelSize, excludedir.c_str());
			if (inport)
			{
				string dir(Content);
				dir.replace(0, 2, dev);
				dir.insert(0, "\\\\.\\");
				SDG_DeleteDirectory(dir.c_str(),&VirtualLength, excludedir.c_str());
			}
			m_pLogger->WriteLog(LM_INFO, "delete Client dir:%s", Content);
		}
		if (inport)
			m_Hy.UMountDosChar((char*)(drv.c_str()));

		m_pLogger->WriteLog(LM_INFO, "end delete ClientFile.\r\n");
		return true;
	}

	bool CBootRunPlug::DelClientGames(char* Content)
	{
		m_pLogger->WriteLog(LM_INFO, "delete client Game!");
		char *pData = NULL;
		int nSize = 0;
		__int64 DelSize=0;
		tstring str,strDriver, strSize;
		str = tstring(Content);
		size_t pos = str.find('|');
		if (pos == string::npos)
		{
			m_pLogger->WriteLog(LM_INFO, "delete client Game error:%s.\r\n", str.c_str());
			return  false;
		}
		else
		{
			strDriver = str.substr(0, pos);
			strSize = str.substr(pos+1);
		}		

		bool DelGameFlag= false;
		ULARGE_INTEGER roomSize;
		strDriver = strDriver + ":\\";
		GetDiskFreeSpaceEx(strDriver.c_str(), &roomSize, NULL, NULL);
		m_pLogger->WriteLog(LM_INFO, "Server'setting %s.",Content);
		m_pLogger->WriteLog(LM_INFO, "Client'Free space:%I64d.", roomSize.QuadPart);
		ULONGLONG settingsize= atoi(strSize.c_str()) * 1000 ;
		settingsize = settingsize * 1000;
		//		if (roomSize.QuadPart < (atoi(strSize.c_str()) * 1000 * 1000) )
		if (roomSize.QuadPart < settingsize)
			DelGameFlag= true;		

		if (DelGameFlag)
		{
			tagGameInfo* pGameInfo;
			vector<tagGameInfo*>::iterator Delitem = m_VecDelGameInfo.begin();
			for(;Delitem != m_VecDelGameInfo.end();Delitem++)
			{
				pGameInfo =*Delitem;
				if (pGameInfo->RUNTYPE == 1)
				{
					if (toupper(strDriver[0]) != toupper(pGameInfo->CLIPATH[0]))
						continue;					 
					m_pLogger->WriteLog(LM_INFO, "Game's PRIORITY = 1,Delete: %s.",pGameInfo->NAME);
					DelClientFile(pGameInfo->CLIPATH,&DelSize);
				}				
			}
			if ((roomSize.QuadPart+DelSize) > settingsize )
				return true;
			vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
			for(;item != m_VecGameInfo.end();item++)
			{
				pGameInfo =*item;
				if (pGameInfo->RUNTYPE == 1 && pGameInfo->PRIORITY<3)
				{
					if (toupper(strDriver[0]) != toupper(pGameInfo->CLIPATH[0]))
						continue;					 
					if ((roomSize.QuadPart+DelSize) > settingsize)
						break;
					m_pLogger->WriteLog(LM_INFO, "Game's PRIORITY = %d,Delete: %s.",pGameInfo->PRIORITY,pGameInfo->NAME);
					DelClientFile(pGameInfo->CLIPATH,&DelSize);	
				}				 
			}
		}

		return true;
	}

	bool CBootRunPlug::SetUpdateGame(char* Content)
	{
		//bool WINAPI BackUpdata(LPCSTR Psvrip,DWORD speed,DWORD nCount, LPVOID pArray)
		m_pLogger->WriteLog(LM_INFO, "start to Update Game.");

		tstring str,strGid;
		DWORD dwGID =0;
		str = tstring(Content);
		tagUpGameinfo* pUpGameinfo = new tagUpGameinfo[4096];
		tagGameInfo* pGameInfo;
		str = str + '|';
		size_t pos = str.find('|');
		DWORD dwIndex=0;

		tstring file = GetAppPath() + TEXT("gameclient.ini");
		TCHAR szIpAddr[MAX_PATH] = {0};
		GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), szIpAddr, MAX_PATH, file.c_str());

		tstring strSpeed, strCmpMode;
		DWORD dwSpeed = 0, dwCmpMode = 0;
		strSpeed = str.substr(0, pos);
		str = str.substr(pos + 1);
		pos = str.find('|');
		strCmpMode = str.substr(0, pos);
		str = str.substr(pos + 1);
		pos = str.find('|');

		dwSpeed = atoi(strSpeed.c_str());
		dwCmpMode = atoi(strCmpMode.c_str());

		while (pos != string::npos)
		{
			strGid = str.substr(0, pos);
			dwGID = atoi(strGid.c_str());
			vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
			for(;item != m_VecGameInfo.end();item++)
			{
				pGameInfo = *item;
				if (pGameInfo->GID ==dwGID)
				{
					if (pGameInfo->RUNTYPE == OPT_RunLocalGame)
					{				
						pUpGameinfo[dwIndex].GID = pGameInfo->GID;
						strcpy(pUpGameinfo[dwIndex].CLIPATH,pGameInfo->CLIPATH);
						strcpy(pUpGameinfo[dwIndex].SVRPATH,pGameInfo->SVRPATH); 	
						pUpGameinfo[dwIndex].Flag = (FC_COPY_MODE)dwCmpMode;
						pUpGameinfo[dwIndex].speed = dwSpeed;
						pUpGameinfo[dwIndex].Srvip = inet_addr(szIpAddr);
						dwIndex++;
					}
					break;
				}
			}			
			str = str.substr(pos+1);
			pos = str.find('|');
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("Update Games Num:%d"), dwIndex);
		if (dwIndex > 0)
		{
           AddPushGameTask(pUpGameinfo, dwIndex);
		}
		delete[] pUpGameinfo;
		m_pLogger->WriteLog(LM_INFO, "end Update Game.");
		return true;
	}

	void CBootRunPlug::CopyRemoteDirectory()
	{
		map<string, string>::iterator it = m_mapOption.find(OPT_M_PENETATEFILE);
		if (it == m_mapOption.end() || it->second.size() == 0)
		{
			return;
		}
		m_pLogger->WriteLog(LM_INFO, "start to Copy remote directory.");
		tstring strContent;
		strContent = tstring(it->second);

		tagUpGameinfo* pRemoteDirInfo = new tagUpGameinfo[4096];
		size_t iPos = strContent.find('|');
		DWORD dwIndex = 0;

		tstring file = GetAppPath() + TEXT("gameclient.ini");
		TCHAR szIpAddr[MAX_PATH] = {0};
		GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), szIpAddr, MAX_PATH, file.c_str());
		DWORD dwSpeed = 0;
		it = m_mapOption.find(OPT_M_TSSPEED);
		if (it != m_mapOption.end())
		{
			if (it->second.size())					
				dwSpeed = atoi(it->second.c_str());
		}

		while(iPos != string::npos)
		{
			tstring strTemp     = strContent.substr(0, iPos);
			size_t iTempPos     = strTemp.find('?');
			tstring strSource   = strTemp.substr(0, iTempPos);
			tstring strDes      = strTemp.substr(iTempPos + 1);

			lstrcpy(pRemoteDirInfo[dwIndex].SVRPATH, strSource.c_str());
			lstrcpy(pRemoteDirInfo[dwIndex].CLIPATH, strDes.c_str());
			pRemoteDirInfo[dwIndex].GID = 0;
			pRemoteDirInfo[dwIndex].Flag = FC_COPY_VERSION;
			pRemoteDirInfo[dwIndex].speed = dwSpeed;
			pRemoteDirInfo[dwIndex].Srvip = inet_addr(szIpAddr);
			strContent = strContent.substr(iPos + 1);
			iPos = strContent.find('|');
			dwIndex++;
		}

		if (dwIndex > 0)
		{
           AddPushGameTask(pRemoteDirInfo, dwIndex);
		}
		delete[] pRemoteDirInfo;
		m_pLogger->WriteLog(LM_INFO, "end Copy remote directory.");
	}

	void CBootRunPlug::PushGameAtOnce(FC_COPY_MODE cmpMode, DWORD dwSpeed, char* pszContent)
	{
		m_pLogger->WriteLog(LM_INFO, "start to push game at once.");

		tstring strContent;
		strContent = tstring(pszContent);

		tagUpGameinfo* pPushGameInfo = new tagUpGameinfo[4096];
		tagGameInfo* pGameInfo = NULL;
		size_t iPos = strContent.find('|');
		DWORD dwIndex = 0;

		tstring file = GetAppPath() + TEXT("gameclient.ini");
		TCHAR szIpAddr[MAX_PATH] = {0};
		GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), szIpAddr, MAX_PATH, file.c_str());

		while(iPos != string::npos)
		{
			tstring strTemp     = strContent.substr(0, iPos);
			//size_t iTempPos     = strTemp.find('?');
			//tstring strGid      = strTemp.substr(0, iTempPos);
			//tstring strCmpMode  = strTemp.substr(iTempPos + 1);
			DWORD  dwGID = _ttoi((char*)strTemp.c_str());

			vector<tagGameInfo*>::iterator item = m_VecGameInfo.begin();
			for(;item != m_VecGameInfo.end();item++)
			{
				pGameInfo = *item;
				if (pGameInfo->GID ==dwGID)
				{
					if (pGameInfo->RUNTYPE == OPT_RunLocalGame)
					{				
						pPushGameInfo[dwIndex].GID = pGameInfo->GID;
						strcpy(pPushGameInfo[dwIndex].CLIPATH,pGameInfo->CLIPATH);
						strcpy(pPushGameInfo[dwIndex].SVRPATH,pGameInfo->SVRPATH); 	
						pPushGameInfo[dwIndex].Flag = cmpMode;
						pPushGameInfo[dwIndex].speed = dwSpeed;
						pPushGameInfo[dwIndex].Srvip = inet_addr(szIpAddr);
						dwIndex++;
					}
					break;
				}
			}	

			strContent = strContent.substr(iPos + 1);
			iPos = strContent.find('|');
		}

		if (dwIndex > 0)
		{
			AddPushGameTask(pPushGameInfo, dwIndex);
		}
		delete[] pPushGameInfo;
		m_pLogger->WriteLog(LM_INFO, "end push game at once.");
	}

	string CBootRunPlug::GetAppPath()
	{
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL, path, MAX_PATH);
		PathRemoveFileSpec(path);
		PathAddBackslash(path);
		return tstring(path);
	}

	void CBootRunPlug::InitPackage(_packageheader* header, WORD cmd, int nExtraLen /* = 0 */)
	{
		ZeroMemory(header, sizeof(_packageheader));
		header->StartFlag = START_FLAG;
		header->Cmd       = cmd;
		header->Length    = sizeof(_packageheader) + nExtraLen;
		header->Version   = PRO_VERSION;
		header->Resv[0]	  = OBJECT_CLIENTSVI;
	}

	bool CBootRunPlug::ExecCmd(LPSTR pOutData, int nOutLen, LPSTR& pInData, int& nInLen, 
		bool bNeedAck /* = true */, bool bAutoLock /* = true */)
	{
		ICommunication* pCommunication = m_pPlugMgr->GetCommunication();
		if (bAutoLock) 
			pCommunication->Lock();
		if (!pCommunication->ExecCmd(pOutData, nOutLen, pInData, nInLen, bNeedAck))
		{
			if (bAutoLock) 
				pCommunication->UnLock();
			m_pLogger->WriteLog(LM_INFO, "网络层执行命令失败" );
			return false;
		}

		if (!bNeedAck)
		{	
			if (bAutoLock)
				pCommunication->UnLock();
			return true;
		}
		_packageheader* pheader = (_packageheader*)pInData;
		if (pheader->StartFlag != START_FLAG) 
		{
			m_pLogger->WriteLog(LM_INFO, "rec Info Package StartFlag Error." );
			CoTaskMemFree(pInData);
			pInData = NULL;
			nInLen = 0;
			if (bAutoLock) 
				pCommunication->UnLock();
			return false;
		}

		CPackageHelper in(pInData);
		DWORD dwStatus = in.popDWORD();
		if (dwStatus == CMD_RET_FAIL)
		{
			char szError[4096] = {0};
			in.popString(szError);
			m_pLogger->WriteLog(LM_INFO, "Exec Cmd Error:%s", szError);
			CoTaskMemFree(pInData);
			pInData = NULL;
			nInLen = 0;
			if (bAutoLock) 
				pCommunication->UnLock();
			return false;
		}
		if (bAutoLock) 
			pCommunication->UnLock();
		return true;
	}

	bool CBootRunPlug::ExecSimpleCmd(WORD cmd, LPSTR& pInData, int& nInLen, 
		LPSTR pOutData /* = NULL */, int nOutLen /* = 0 */)
	{
		LPSTR pData = new char[nOutLen + sizeof(_packageheader) + 5];
		_packageheader* pheader = (_packageheader*)pData;
		InitPackage(pheader, cmd);
		if (pOutData != NULL && nOutLen)
		{
			pheader->Length = sizeof(_packageheader)+ nOutLen + 5;	//修正长度
			CPackageHelper out(pData);
			out.pushString(pOutData, nOutLen);
		}

		bool ret = ExecCmd(pData, pheader->Length, pInData, nInLen);
		delete []pData;
		return ret;
	}

	void CBootRunPlug::WriteDataFile(string FileName, char* data,bool bInprotect, DWORD datalength)
	{
		//	m_pLogger->WriteLog(LM_INFO, "Write offline file:%s", FileName.c_str());
		if (datalength == 0)
		{
			datalength = ((_packageheader*)data)->Length;
		}

		HANDLE hFile = ::CreateFile(FileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, "CreateFile Fail:%d:%s!", dwError, Error.ErrorMessage());
			return ;
		}

		DWORD dwByteWrite = 0;
		if (!WriteFile(hFile, data, datalength, &dwByteWrite, NULL) || dwByteWrite != datalength)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("WriteFile Fail:%d:%s!"), dwError, Error.ErrorMessage());
		}
		else
		{
			if (bInprotect)
			{
				bool bNeedInProtUpdate = m_Hy.MountDosChar((char*)(FileName.c_str()));
				if (bNeedInProtUpdate)
				{				
					tstring strTmp("\\\\.\\");
					strTmp += m_Hy.GetDevicePath();
					tstring device= FileName;
					device.replace(0, 2, strTmp);

					BOOL ret = CopyFileA(FileName.c_str(), device.c_str(), FALSE);
					if (!ret)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("copy %s :%d."), device.c_str(), GetLastError());
					}				
					m_Hy.UMountDosChar((char*)(FileName.c_str()));
				}
			}			
		}

		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}

	bool CBootRunPlug::LoadVDiskDll()
	{
		char path[MAX_PATH] = {0};
		SDG_GetAppPath(path, TRUE);
		lstrcat(path, "Refresh.dll");
		m_hRefreshDll = LoadLibrary(path);
		if (m_hRefreshDll == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("load refresh.dll have error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		else
		{
			m_pfnDiskMount = (PFNDISKMOUNT)GetProcAddress(m_hRefreshDll, "iDiskMount");
			m_pfnDiskUmount = (PFNDISKUMOUNT)GetProcAddress(m_hRefreshDll, "iDiskUmount");
			if (m_pfnDiskMount == NULL || m_pfnDiskUmount == NULL)
			{
				DWORD dwError = GetLastError();
				_com_error Error(dwError);
				m_pLogger->WriteLog(LM_INFO, TEXT("load iDiskMount.dll have error:%d:%s"), dwError, Error.ErrorMessage());
				return false;
			}
		}
		return true;
	}

	bool CBootRunPlug::CheckVDiskService()
	{
		SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("OpenSCManager have error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		char szPath[MAX_PATH] = {0};
		GetSystemDirectory(szPath, MAX_PATH);
		PathAddBackslash(szPath);
		lstrcat(szPath, "drivers\\wxNDA.sys");

		if (!PathFileExists(szPath))
		{
			char szModle[MAX_PATH];
			SDG_GetAppPath(szModle, TRUE);
			lstrcat(szModle, "wxNDA.sys");
			CopyFile(szModle, szPath, TRUE);
		}

		DWORD dwErr = 0;
		SC_HANDLE schService = OpenService(schSCManager, "wxNDA", SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			dwErr = GetLastError();
			if (dwErr == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				schService = CreateService( 
					schSCManager,				// SCManager database 
					"wxNDA",					// name of service 
					"wxNDA",					// service name to display 
					SERVICE_ALL_ACCESS,			// desired access 
					SERVICE_KERNEL_DRIVER,		// service type 
					SERVICE_SYSTEM_START,			// start type 
					SERVICE_ERROR_NORMAL,		// error control type 
					szPath,						// path to service's binary 
					NULL,						// no load ordering group 
					NULL,						// no tag identifier 
					NULL,						// no dependencies 
					NULL,						// LocalSystem account 
					NULL);						// no password 
			}
			else
			{
				CloseServiceHandle(schSCManager);
				m_pLogger->WriteLog(LM_INFO, TEXT("create Service error:%d."),dwErr);
				return false;
			}
		}

		DWORD ret = TRUE;
		if (!StartService(schService, 0, NULL))
		{
			if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
				ret = TRUE;	
			else
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Start Service error:%d."),GetLastError());
			}
		}
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		if (ret)
			m_pLogger->WriteLog(LM_INFO, TEXT("wxNDA is runing."));
		return ret == TRUE;
	}

	bool CBootRunPlug::CheckProtDiskService()
	{
		SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("OpenSCManager have error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		char szPath[MAX_PATH];
		GetSystemDirectory(szPath, MAX_PATH);
		PathAddBackslash(szPath);
		lstrcat(szPath, "drivers\\ProtDisk.sys");
		if (!PathFileExists(szPath))
		{
			char szModle[MAX_PATH];
			SDG_GetAppPath(szModle, TRUE);
			lstrcat(szModle, "ProtDisk.sys");
			CopyFile(szModle, szPath, TRUE);
		}

		DWORD dwErr = 0;
		SC_HANDLE schService = OpenService(schSCManager, "ProtDisk", SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			dwErr = GetLastError();
			if (dwErr == ERROR_SERVICE_DOES_NOT_EXIST)
			{
				schService = CreateService( 
					schSCManager,				// SCManager database 
					"ProtDisk",					// name of service 
					"ProtDisk",					// service name to display 
					SERVICE_ALL_ACCESS,			// desired access 
					SERVICE_KERNEL_DRIVER,		// service type 
					SERVICE_SYSTEM_START,			// start type 
					SERVICE_ERROR_IGNORE,		// error control type 
					szPath,					// path to service's binary 
					NULL,						// no load ordering group 
					NULL,						// no tag identifier 
					NULL,						// no dependencies 
					NULL,						// LocalSystem account 
					NULL);						// no password 
			}
			else
			{				
				CloseServiceHandle(schSCManager);
				m_pLogger->WriteLog(LM_INFO, TEXT("create Service error:%d."),dwErr);
				return false;
			}
		}
		else
		{
		//	SHSetValue(HKEY_LOCAL_MACHINE, TEXT("system\\CurrentControlSet\\Services\\ProtDisk"), 
			//	TEXT("ImagePath"), REG_EXPAND_SZ, szModle, strlen(szModle));
			//
			//if (!ChangeServiceConfig(schService, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, szModle, 
			//	NULL, NULL, NULL, "ProtDisk", NULL, "ProtDisk"))
			//{
			//	dwErr = GetLastError();
			//	m_pLogger->WriteLog(LM_INFO, TEXT("Change Service error:%d."),dwErr);
			//}
		}

		DWORD ret = TRUE;
		if (!StartService(schService, 0, NULL))
		{
			if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
				ret = TRUE;	
			else
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Start Service error:%d."),GetLastError());
				ret = FALSE;
			}
		}
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		if (ret)
			m_pLogger->WriteLog(LM_INFO, TEXT("ProtDisk is runing."));
		return ret == TRUE;
	}

	bool CBootRunPlug::CheckVDiskDriver(tagVDisk* pVDisk)
	{
		char buf[MAX_PATH] = {0};
		_stprintf(buf, "%C:\\", pVDisk->CliDrv);
		return (GetDriveType(buf) != DRIVE_NO_ROOT_DIR);
	}

	void CBootRunPlug::GetLocMachineOtherInfo(tagLocMachineInfo &info)
	{
		TCHAR AppExe[MAX_PATH] = {0};
		GetModuleFileName(NULL, AppExe, MAX_PATH);//获取当前程序的版本；
		lstrcpy((LPSTR)info.CliSvrVer,GetFileVersion(AppExe).c_str());

		tstring datafile = GetAppPath() + I8DESKGAMEMENU;//获取当前菜单的版本；
		lstrcpy((LPSTR)info.MenuVer,GetFileVersion((char*)datafile.c_str()).c_str());

		/*获取还原驱动的版本*/		
		char buf[MAX_PATH] = {0};
		GetSystemDirectory(buf, MAX_PATH);
		PathAddBackslash(buf);
		string ProtFile(buf);
		ProtFile = ProtFile + PROTECTFILE;
		lstrcpy((LPSTR)info.ProtVer,GetFileVersion((char*)ProtFile.c_str()).c_str());

		/*是否安装还原*/
		if (m_Hy.IsInstallHy())
		{
			info.ProtInstal = 1;			
		}
		else
		{
			info.ProtInstal = 0;
		}

		ZeroMemory(info.AllDrivers, MAX_PATH);
		ZeroMemory(info.ProtArea, MAX_PATH);

		/*还原保护的区域*/
		vector<char> vecDriver;
		size_t idx =0;
		int	Index = 0;
		int iDriverIdx = 0;
		string driver;
		for (idx=0; idx<3; idx++)
		{
			tag_HDInfo hdInfo = {0};
			hdInfo.dwHdNumber = idx;
			if (0 == m_Hy.GetHDInfo(&hdInfo))
			{
				for (DWORD loop=0; loop<hdInfo.dwPtNums; loop++)
				{
					driver = hdInfo.pt[loop].bDosChar;
					if (m_Hy.IsInstallHy())
					{					
						if (m_Hy.IsDriverProtected((char*)driver.c_str()))
						{
							info.ProtArea[Index]= hdInfo.pt[loop].bDosChar;
							Index++;
						}
					}
				}
				for (char ch = 'C'; ch <= 'Z'; ++ch)
				{
					TCHAR szDriver[MAX_PATH] = {0};
					sprintf(szDriver, "%c:\\", ch);
					if (GetDriveType(szDriver) == DRIVE_FIXED)
					{
						info.AllDrivers[iDriverIdx] = ch;
						iDriverIdx++;
					}
				}

				break;
			}
			else
			{
				m_pLogger->WriteLog(LM_INFO, "GetHDInfo Fail.");
				break;
			}
		}
	}

	tstring CBootRunPlug::GetFileVersion(char* szFileName)
	{
		DWORD dwSize = GetFileVersionInfoSize(szFileName, 0);
		if (dwSize)
		{
			BYTE *pInfo = new BYTE[dwSize+1];
			GetFileVersionInfo(szFileName, 0, dwSize, pInfo);

			UINT nQuerySize;
			DWORD* pTransTable = NULL;
			VerQueryValue(pInfo, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
			LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			char SubBlock[MAX_PATH] = {0};
			_stprintf(SubBlock, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
			LPSTR lpData;
			VerQueryValue(pInfo, SubBlock, (PVOID*)&lpData, &nQuerySize);
			string str(lpData);
			delete []pInfo;
			//replace ',' to '.'
			size_t pos = 0;
			while ( (pos = str.find(',', pos)) != std::string::npos)
			{
				str[pos] = '.';
			}
			//delete [SP]
			pos = 0;
			while ( (pos = str.find(' ', pos)) != std::string::npos)
			{
				str.erase(pos, 1);
			}
			return str;
		}
		return string("0.0.0.0");	
	}

	bool CBootRunPlug::SendHeartInfo(tagLocMachineInfo &info)
	{
		char *pData = NULL;
		int  nSize = 0;
		char buf[1024] = {0};

		DWORD dwGameDrvFreeSize = GetGameDrvFreeSize();
		DWORD dwSafeCenterStatus = GetSafeCenterStatus();
		_packageheader* pheader = (_packageheader*)buf;
		InitPackage(pheader, CMD_CLIRPT_CLIENTINFO);
		CPackageHelper out(buf);
		out.pushString(info.szName,strlen(info.szName));
		out.pushString(info.szGate,strlen(info.szGate));
		out.pushString(info.szDNS0,strlen(info.szDNS0));
		out.pushString(info.szMac,strlen(info.szMac));
		out.pushDWORD(info.ProtInstal);
		out.pushString(info.ProtArea,strlen(info.ProtArea));
		out.pushString(info.AllDrivers,strlen(info.AllDrivers));
		out.pushString(info.ProtVer,strlen(info.ProtVer));
		out.pushString(info.MenuVer,strlen(info.MenuVer));
		out.pushString(info.CliSvrVer,strlen(info.CliSvrVer));		
		out.pushDWORD(m_pPlugMgr->GetCommunication()->IsFirstPackage() ? 1 : 0);// DWORD( ! bFirstRecHeartInfo));
		out.pushString(info.szDNS1,strlen(info.szDNS1));
		out.pushString(info.szSubMask,strlen(info.szSubMask));
		out.pushDWORD(dwGameDrvFreeSize);
		out.pushDWORD(dwSafeCenterStatus);
		out.pushDWORD(bIsLoadBHOSuccess ? 1 : 0); //IE保护
		out.pushDWORD(bIsStartProtDiskSuccess ? 1 : 0); //防狗驱动
		pheader->Length = out.GetOffset();
		if (!ExecCmd(buf, pheader->Length, pData, nSize))
		{
			m_pLogger->WriteLog(LM_INFO, "Get heart Data Fail.\r\n");
			return false;
		}
		tstring datafile = GetAppPath()+ TEXT("data\\HeartDataInfo.dat");
		WriteDataFile(datafile, pData,false);	
		//	m_pLogger->WriteLog(LM_INFO, "Get heart Data Success.");

		//		m_pLogger->WriteLog(LM_INFO, TEXT("Start Check Version."));
		CPackageHelper inpackage(pData);
		DWORD flag =inpackage.popDWORD();
		if ( flag== CMD_RET_SUCCESS)
		{
			char tmp[MAX_PATH]="";
			char IniValue[MAX_PATH]="";
			bool bWriteIniFlag=false;
			tstring sSetFlag="0";
			string szIniFileName = GetAppPath() +  "GameClient_Version.ini";
			DWORD dwGameVer		= inpackage.popDWORD();//游戏版本号
			DWORD dwVDISKVer	= inpackage.popDWORD();//虚拟盘版本号
			dwIcoVer			= inpackage.popDWORD();//图标压缩包版本号
			DWORD dwUptVer		= inpackage.popDWORD();//软件自动升级的版本号
			DWORD dwSysOptVer   = inpackage.popDWORD();//系统选项的版本号
			DWORD dwCount		= inpackage.popDWORD();//虚拟盘个数;
			char  VID[MAX_PATH];
			DWORD dwVDiskBuferVer;
			WritePrivateProfileString(VDiskBufVer,NULL,NULL,szIniFileName.c_str());
			for (DWORD i= 0;i<dwCount;i++)
			{
				inpackage.popString(VID);
				dwVDiskBuferVer = inpackage.popDWORD();
				_stprintf(tmp,"%u",dwVDiskBuferVer);
				WritePrivateProfileString(VDiskBufVer,VID,tmp,szIniFileName.c_str());	
			}
			CoTaskMemFree(pData);

			//处理系统选项
			_stprintf(tmp,"%u",dwSysOptVer);
			GetPrivateProfileString("SystemSet", "SysOptVer","",IniValue,MAX_PATH,szIniFileName.c_str());
			if ((lstrcmp(tmp, IniValue) != 0) && (lstrcmp(tmp, "0") != 0))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("SysOptPackage need update,Server's ver:%s,Client's ver:%s."),tmp,IniValue);
				GetSystemOption(true);
				WritePrivateProfileString("SystemSet", "SysOptVer", tmp, szIniFileName.c_str());				
				bWriteIniFlag = true;
			}
			//		else
			//			m_pLogger->WriteLog(LM_INFO, TEXT("SysOptPackage NO updat."));

			//处理开机刷盘
			// 			_stprintf(tmp,"%u",dwVDISKVer);
			// 			GetPrivateProfileString("SystemSet", "VDISKVer","",IniValue,MAX_PATH,szIniFileName.c_str());
			// 			if (lstrcmp(tmp, IniValue) != 0)
			// 			{
			// 				m_pLogger->WriteLog(LM_INFO, TEXT("VDISKPackage need update,Server's ver:%s,Client's ver:%s."),tmp,IniValue);
			// 				GetVDiskInfo();
			// 				WritePrivateProfileString("SystemSet", "VDISKVer", tmp, szIniFileName.c_str());
			// 			}				

			map<string, string>::iterator it = m_mapOption.find(OPT_M_ONLYONE);//是否只取一次游戏数据包
			if (it != m_mapOption.end())
			{
				if (it->second.size())
					sSetFlag = it->second;						
			}
			if (!bGetGameinfoOnce || sSetFlag != "1")
			{
				//处理游戏图标
				_stprintf(tmp,"%u",dwIcoVer);
				GetPrivateProfileString("SystemSet", "IcoVer","",IniValue,MAX_PATH,szIniFileName.c_str());
				if ((lstrcmp(tmp, IniValue) != 0) && (lstrcmp(tmp, "0") != 0))
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("GamesIconPackage need update,Server's ver:%s,Client's ver:%s."),tmp,IniValue);
					if (GetGamesIcon())
					{
						WritePrivateProfileString("SystemSet", "IcoVer", tmp, szIniFileName.c_str());
						bWriteIniFlag = true;
					}
				}
				else
				{
					if (!bFirstRecHeartInfo)//处理第一次心跳包判断处理
					{
						if (!GetOfflineGamesIcon())
							GetGamesIcon();
					}
					//		m_pLogger->WriteLog(LM_INFO, TEXT("GamesIconPackage NO updat."));
				}
				if (WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
					return false;
				//处理游戏数据包
				_stprintf(tmp,"%u",dwGameVer);
				GetPrivateProfileString("SystemSet", "GameVer","",IniValue,MAX_PATH,szIniFileName.c_str());
				if ((lstrcmp(tmp, IniValue) != 0) && (lstrcmp(tmp, "0") != 0))
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("GamesPackage need update,Server's ver:%s,Client's ver:%s."),tmp,IniValue);
					if (GetGamesInfo())
					{
						WritePrivateProfileString("SystemSet", "GameVer", tmp, szIniFileName.c_str());
						bWriteIniFlag = true;
					}
				}
				else
				{
					if (bGetGameInfoIndex == false)
					{
						//获取第一次索引包
						GetFirstGameInfoIndex();
					}
				}
			}
			else
				m_pLogger->WriteLog(LM_INFO, TEXT("Rec GamesPackage Onlyone."));

			if (!bFirstRecHeartInfo)	//仅开机执行一次软件自动更新
			{
				sSetFlag = "1";
				//处理客户端软件更新
				it = m_mapOption.find(OPT_M_AUTOUPT);//是否需要进行软件升级
				if (it != m_mapOption.end())
				{
					if (it->second.size())
						sSetFlag = it->second;						
				}
				if (sSetFlag == "1" || sSetFlag == "")
				{
					_stprintf(tmp,"%u",dwUptVer);
					GetPrivateProfileString("SystemSet", "UptVer","",IniValue,MAX_PATH,szIniFileName.c_str());
					if ((lstrcmp(tmp, IniValue) != 0) && (lstrcmp(tmp, "0") != 0))
					{
						CAutoUpdate m_AutoUpdate(m_pLogger,m_pPlugMgr,&m_Hy);
						m_pLogger->WriteLog(LM_INFO, TEXT("Client's files need update,Server's ver:%s,Client's ver:%s."),tmp,IniValue);
						if (m_AutoUpdate.StartUpdate())
						{
							WritePrivateProfileString("SystemSet", "UptVer", tmp, szIniFileName.c_str());				
							bWriteIniFlag = true;
						}
						m_pLogger->WriteLog(LM_INFO, TEXT("end  AutoUpdate...."));
					}
					else
						m_pLogger->WriteLog(LM_INFO, TEXT("Compare Client's files, NO updat."));
				}
				else
					m_pLogger->WriteLog(LM_INFO, TEXT("Sever Setting Client's files NO updat."));
			}

			if (bWriteIniFlag)
			{
				bool bNeedInProtUpdate = m_Hy.MountDosChar((char*)(szIniFileName.c_str()));
				if (bNeedInProtUpdate)
				{
					tstring strTmp("\\\\.\\");
					strTmp += m_Hy.GetDevicePath();
					tstring device= szIniFileName;
					device.replace(0, 2, strTmp);

					BOOL ret = CopyFileA(szIniFileName.c_str(), device.c_str(), FALSE);
					if (!ret)
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("copy %s :%d."), device.c_str(), GetLastError());
					}				
					m_Hy.UMountDosChar((char*)(szIniFileName.c_str()));
				}
			}
			bGetGameinfoOnce = true;
		}	
		bFirstRecHeartInfo = true;
		if (bNeedNotifyFrameWindow)
		{
			NotifyFrameWindow(WM_REFRESH_DATA, (WPARAM)0, (LPARAM)0);
			bNeedNotifyFrameWindow = false;
		}
		return true;
	}
	bool CBootRunPlug::SetGameDeskTopLnk(tagGameInfo* pGameInfo)
	{
		m_pLogger->WriteLog(LM_INFO, "start create Gamedesktop link.");
		string GameFile,GameFolder,GameIconFile;

		char Gid[MAX_PATH]={NULL};
		_stprintf(Gid,"%u",pGameInfo->GID);
		GameFolder= GetAppPath();
		GameFile = GameFolder + I8DESKGAMEMENU;// + " /////Start// "+ tstring(Gid);

		char path[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, path, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
		PathAddBackslash(path);

		m_pLogger->WriteLog(LM_INFO, "Get Desktop path:%s", path);

		lstrcat(path,pGameInfo->NAME);
		lstrcat(path,".lnk");
		WIN32_FIND_DATA   fdt;
		HANDLE hFile = FindFirstFile(path, &fdt);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			m_pLogger->WriteLog(LM_INFO, "Exist, Gamedesktop link:%s", path);
			m_pLogger->WriteLog(LM_INFO, "end create desktop link. \r\n");
			return true;
		}

		wchar_t wpath[MAX_PATH] = {0};
		MultiByteToWideChar(GetACP(), 0, path, -1, wpath, MAX_PATH);

		stringstream sPara;
		sPara << "/start " << Gid;
		GameIconFile = GetAppPath() + "data\\icon\\"+tstring(Gid)+".ico";

		HRESULT hres ;
		IShellLink * psl ;
		IPersistFile* ppf ;

		//创建一个IShellLink实例
		hres = CoCreateInstance( CLSID_ShellLink, NULL,   CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
		if( FAILED( hres))   return FALSE ;

		//设置目标应用程序
		psl -> SetPath( GameFile.c_str()) ;

		//设置图标()
		psl->SetArguments(sPara.str().c_str());
		TCHAR szTmpDir[MAX_PATH] = {0};
		lstrcpy(szTmpDir, GameFolder.c_str());
		PathRemoveFileSpec(szTmpDir);
		PathAddBackslash(szTmpDir);
		psl->SetWorkingDirectory(szTmpDir);
		psl->SetIconLocation(GameIconFile.c_str(),0);

		//从IShellLink获取其IPersistFile接口
		//用于保存快捷方式的数据文件 (*.lnk)

		hres = psl -> QueryInterface( IID_IPersistFile, (void**)&ppf) ;
		if( FAILED( hres))    return FALSE ;

		// 		// 确保数据文件名为ANSI格式
		// 		MultiByteToWideChar( CP_ACP, 0, path, -1,
		// 			wsz, MAX_PATH) ;

		//调用IPersist:Save
		//保存快捷方式的数据文件 (*.lnk)
		hres = ppf -> Save( wpath, STGM_READWRITE) ;

		//释放IPersistFile和IShellLink接口
		ppf -> Release( ) ;
		psl -> Release( ) ;
		m_pLogger->WriteLog(LM_INFO, "create Gamedesktop link:%s", path);
		m_pLogger->WriteLog(LM_INFO, "end create desktop link. \r\n");
		return true;
	}

	bool CBootRunPlug::DelGameDeskTopLnkExcept(const vector<tagGameInfo*> &vctGameInfo)
	{
		//快捷方式文件夹
		char szDesktopDir[MAX_PATH] = {0}, szDesktopFile[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szDesktopDir, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
		PathAddBackslash(szDesktopDir);

		lstrcpy(szDesktopFile, szDesktopDir);
		lstrcat(szDesktopFile, TEXT("*.lnk"));

		string strMenuPath = GetAppPath() + I8DESKGAMEMENU;
		vector<string> vctDelDesktop;

		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(szDesktopFile, &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return FALSE;
		do 
		{
			OutputDebugString(wfd.cFileName);
			OutputDebugString("\r\n");

			HRESULT       hRes ;
			IShellLink    *psl ;
			IPersistFile  *ppf ;

			//创建一个IShellLink实例
			hRes = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
			if( FAILED( hRes))   return FALSE ;

			hRes = psl->QueryInterface( IID_IPersistFile, (LPVOID *)&ppf); 
			if( FAILED( hRes))   return FALSE ;

			char szFullPath[MAX_PATH] = {0};
			_stprintf(szFullPath, TEXT("%s%s"), szDesktopDir, wfd.cFileName);
			wchar_t wPath[MAX_PATH] = {0};
			MultiByteToWideChar(GetACP(), 0, szFullPath, -1, wPath, MAX_PATH);

			hRes = ppf->Load(wPath, 0);
			if (SUCCEEDED(hRes))
			{
				char szTargetPath[MAX_PATH] = {0};
				char szTargetParam[MAX_PATH] = {0};
				psl->GetPath(szTargetPath, MAX_PATH, NULL, SLGP_UNCPRIORITY);
				psl->GetArguments(szTargetParam, MAX_PATH);

				if (lstrcmpi(strMenuPath.c_str(), szTargetPath) == 0
					&& StrNCmpI(szTargetParam, TEXT("/start"), lstrlen(TEXT("/start"))) == 0
					&& strchr(szTargetParam, ':') == NULL)
				{
					DWORD dwGid = _ttoi((char*)szTargetParam + 7);
					BOOL bExist = FALSE;
					for (DWORD dwLoop = 0; dwLoop < vctGameInfo.size(); ++dwLoop)
					{
						if (vctGameInfo[dwLoop]->GID == dwGid)
						{
							bExist = TRUE;
							break;
						}
					}
					if (!bExist)
					{
						vctDelDesktop.push_back(string(szFullPath));
					}
				}
			}
			ppf -> Release( ) ;
			psl -> Release( ) ;
		} while (FindNextFile(hFinder, &wfd));

		for (DWORD dwLoop = 0; dwLoop < vctDelDesktop.size(); ++dwLoop)
		{
			DeleteFile(vctDelDesktop[dwLoop].c_str());
		}
		return true;
	}

	void CBootRunPlug::RegistryPlug()
	{
		m_pLogger->WriteLog(LM_INFO, "start to registry plug.");
		string the3param;
		map<string, string>::iterator it = m_mapOption.find(string("the3param"));
		if (it != m_mapOption.end())
			the3param = it->second;

		DWORD dwRet = 0;
		vector<string> plugs;
		splitString(the3param, plugs);
		for (size_t idx=0; idx<plugs.size(); idx++)
		{
			string str = plugs[idx];
			size_t pos = str.find_first_of(',', 0);

			if (pos != string::npos)
			{
				string dllname = str.substr(0, pos);
				string param = str.substr(pos+1);
				dllname += ".dll";

				char buf[MAX_PATH] = {0};
				SDG_GetAppPath(buf, TRUE);
				lstrcat(buf, dllname.c_str());

				HMODULE hDll = LoadLibrary(buf);

				PFNUPDATEEXEC pfnUpdateExec = (hDll == NULL) ? NULL : ((PFNUPDATEEXEC)GetProcAddress(hDll, "UpdateExec"));
				if (pfnUpdateExec != NULL)
				{
					DWORD dwRet = pfnUpdateExec(param.c_str());
					m_pLogger->WriteLog(LM_INFO, "install %s:%d.", buf, dwRet);
				}			
				else
				{
					m_pLogger->WriteLog(LM_INFO, "load function error:%s.", buf);
				}
				if (hDll != NULL)
				{
					FreeLibrary(hDll);
				}
			}
			else
			{
				m_pLogger->WriteLog(LM_INFO, "format erorr.\r\n");
			}
		}
		m_pLogger->WriteLog(LM_INFO, "end registry plug.\r\n");
	}

	string GetCookieDateString()
	{
		SYSTEMTIME st = {0};
		time_t t = time(NULL) + 30 * 24 * 60 * 60;
		struct tm* ptm = gmtime(&t);
		if (ptm != NULL)
		{
			st.wYear	= ptm->tm_year + 1900;
			st.wMonth	= ptm->tm_mon + 1;
			st.wDay		= ptm->tm_mday;
			st.wHour	= ptm->tm_hour;
			st.wMinute	= ptm->tm_min;
			st.wSecond	= ptm->tm_sec;
			char buf[INTERNET_RFC1123_BUFSIZE] = {0};
			InternetTimeFromSystemTime(&st, INTERNET_RFC1123_FORMAT, buf, INTERNET_RFC1123_BUFSIZE);
			return std::string(buf);
		}
		return std::string("");
	} 

	bool CBootRunPlug::WriteRegistryInfo()
	{
		//标题栏追回网吧标题
	//string strNbName = GetStringOpt(OPT_U_NBNAME);
	//SHSetValue(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Internet Explorer\\Main"),
	//	TEXT("Window Title"), REG_SZ, strNbName.c_str(), strNbName.size());

		std::string strCookie;

		//其中每一个KEY_I8DESK下面的key都要写两个域名的cookie.
		/*
		1、需要写入nid\bindoemid\nbname\aid等(即写到HKLM\software\Goyoo\i8desk下的所有内容)
		写cookie语句范例:
		InternetSetCookie(_T("http://i8.com.cn"),NULL,_T("i8desk_nid=118764; expires=Sat,01-Jan-2010 00:00:00 GMT"));
		InternetSetCookie(_T("http://i8cn.com"),NULL,_T("i8desk_nid=118764; expires=Sat,01-Jan-2010 00:00:00 GMT"));
		...
		expires为有效期，定为当前时间+1个月

		*/
		tstring key[]= 
		{
			OPT_U_AID,
			OPT_U_USERNAME,	//用户名 
			OPT_U_NBNAME,	//网吧名称
			OPT_U_NID,		//网吧id
			OPT_U_RID,		//地区id
			OPT_U_SID,		//服务器id
			OPT_U_CPUID,	//硬件id
			OPT_U_SMAC,		//服务端MAC地址 
			OPT_U_SVER,		//服务端版本号
			OPT_U_OEMID,	//渠道id
			OPT_U_OEMNAME	//渠道名称	
		};

		char cookie[MAX_PATH] = {0};
		string strexpires = GetCookieDateString();

		m_pLogger->WriteLog(LM_INFO, "start to Write Registry ProcInfo.");
		m_pLogger->WriteLog(LM_INFO, "write i8 account param.");
		map<string, string>::iterator it;
		DWORD idx = 0;
		for (idx=0; idx<sizeof(key)/sizeof(key[0]); idx++)
		{
			it = m_mapOption.find(key[idx]);
			if (it != m_mapOption.end())
			{
				SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK,
					key[idx].c_str(), REG_SZ, it->second.c_str(), it->second.size());

				sprintf(cookie,"i8desk_%s=%s;expires=%s", key[idx].c_str(), it->second.c_str(), strexpires.c_str());
				if (strCookie.size() == 0)
					strCookie += cookie;
				else
				{
					strCookie += "\t";
					strCookie +=cookie;
				}
				//InternetSetCookie(_T("http://i8.com.cn"), NULL, cookie); //_T("i8desk_nid=118764; expires=Sat,01-Jan-2010 00:00:00 GMT"));
				//InternetSetCookie(_T("http://i8cn.com"), NULL, cookie); //_T("i8desk_nid=118764; expires=Sat,01-Jan-2010 00:00:00 GMT"));
			}
		}

		SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, OPT_U_CMAC, REG_SZ, m_LocMacInfo.szMac, strlen(m_LocMacInfo.szMac));//客户端MAC地址
		sprintf(cookie, "i8desk_cmac=%s;expires=%s", m_LocMacInfo.szMac, strexpires.c_str());
		//InternetSetCookie("http://i8.com.cn", NULL, cookie);
		//InternetSetCookie("http://i8cn.com", NULL, cookie);
		strCookie += "\t";
		strCookie +=cookie;

		SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, OPT_U_CVER, REG_SZ, m_LocMacInfo.MenuVer, strlen(m_LocMacInfo.MenuVer));//客户端版本号(菜单版本)
		sprintf(cookie, "i8desk_cver=%s;expires=%s", m_LocMacInfo.MenuVer, strexpires.c_str());
		//InternetSetCookie("http://i8.com.cn", NULL, cookie);
		//InternetSetCookie("http://i8cn.com", NULL, cookie);
		strCookie += "\t";
		strCookie +=cookie;

		char path[MAX_PATH] = {0};
		SDG_GetAppPath(path, FALSE);
		SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, "installPath", REG_SZ, path, lstrlen(path));//游戏菜单安装路径
		sprintf(cookie, "i8desk_installPath=%s;expires=%s", path, strexpires.c_str());
		//InternetSetCookie("http://i8.com.cn", NULL, cookie);
		//InternetSetCookie("http://i8cn.com", NULL, cookie);
		strCookie += "\t";
		strCookie +=cookie;

		lstrcat(path, "\\BarOnline.exe");
		SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK, "exec", REG_SZ, path, lstrlen(path));//游戏菜单EXE
		sprintf(cookie, "i8desk_exec=%s;expires=%s", path, strexpires.c_str());
		//InternetSetCookie("http://i8.com.cn", NULL, cookie);
		//InternetSetCookie("http://i8cn.com", NULL, cookie);
		strCookie += "\t";
		strCookie +=cookie;

		//专门为i8商城服务，还需要在cookie中
		/*
		另外为了更好的为i8商城服务，还需要在cookie中如下写入（注意其中有个简单的md5校验）：
		TCHAR AgentID[]=_T("100000");                       //aid
		TCHAR ClientIP[]=_T("192.168.1.10"); 
		TCHAR ClientName[]=_T("PC-10");
		TCHAR expires[]=_T("Sat,01-Jan-2010 00:00:00 GMT"); //这个日期由当前时间+30天得到

		CString csTmp;
		CString csTmp2;

		csTmp2.Format(_T("AgentID=%s&ClientIP=%s|||sdo(10~0s^if*34"),AgentID,ClientIP);
		MD5 md5tmp;                                        
		md5tmp.update(OLE2A(csTmp2));

		csTmp.Format(_T("KOC.I8Esale.DeskBiz=AgentID=%s&ClientIP=%s&ClientName=%s&Sign=%s; expires=%s"),
		AgentID,
		ClientIP,
		ClientName,
		A2T(md5tmp.toString().c_str()),
		expires
		);

		InternetSetCookie(_T("http://i8cn.com"),NULL,csTmp);
		InternetSetCookie(_T("http://i8.com.cn"),NULL,csTmp);
		*/
		{
			map<string, string>::iterator it = m_mapOption.find("nid");
			if(it != m_mapOption.end())
			{
				char buf[MAX_PATH] = {0};
				sprintf(buf, _T("AgentID=%s&ClientIP=%s|||sdo(10~0s^if*34"), it->second.c_str(), m_LocMacInfo.szIp);
				std::string md5string = MDString(buf);
				sprintf(cookie, _T("KOC.I8Esale.DeskBiz=AgentID=%s&ClientIP=%s&ClientName=%s&Sign=%s; expires=%s"),
					it->second.c_str(), m_LocMacInfo.szIp, m_LocMacInfo.szName, md5string.c_str(), strexpires.c_str());
			}

			//InternetSetCookie(_T("http://i8cn.com"),  NULL, cookie);
			//InternetSetCookie(_T("http://i8.com.cn"), NULL, cookie);
			strCookie += "\t";
			strCookie +=cookie;
		}

		m_pLogger->WriteLog(LM_INFO, TEXT("Set Cookie infomation."));
		{
			tstring root = GetRunGameExe() + "SetCookie|";
			root += strCookie;
			RunProcess(root.c_str(), root.c_str());
		}

		m_pLogger->WriteLog(LM_INFO, "write i8desk task info...");
		{
			string info;
			it = m_mapOption.find(OPT_U_TASKINFO);
			if (it != m_mapOption.end())
			{
				info =it->second.c_str();
			}
			SHDeleteKey(HKEY_LOCAL_MACHINE, KEY_I8DESK_TASK);

			vector<string> tasks;
			splitString(info, tasks);
			for (size_t idx=0; idx<tasks.size(); idx++)
			{
				vector<string> task;
				splitString(tasks[idx], task, ';');

				string gid, onpcstart, oni8start;
				for (size_t loop=0; loop<task.size(); loop++)
				{
					string sub = task[loop];
					if (_strnicmp(sub.c_str(), "gid=", 4) == 0)
						gid = sub.substr(4);
					else if (_strnicmp(sub.c_str(), "onpcstart=", 10) == 0)
						onpcstart = sub.substr(10);
					else if (_strnicmp(sub.c_str(), "oni8start=", 10) == 0)
						oni8start = sub.substr(10);
				}
				if (gid.size())
				{
					m_pLogger->WriteLog(LM_INFO, "handle task:gid=%s,onpc=%s,oni8=%s", 
						gid.c_str(), onpcstart.c_str(), oni8start.c_str());
					handleTask(gid, onpcstart, oni8start);
				}
				else
				{
					m_pLogger->WriteLog(LM_INFO, "format error:%s", tasks[idx].c_str());
				}
				Sleep(1000);
			}
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("end of Writeing Registry ProcInfo.\r\n"));		
		return true;
	}
	void CBootRunPlug::handleTask(string gid, string pcstart, string i8start)
	{
		string pc = handleDir(pcstart);
		string i8 = handleDir(i8start);
		m_pLogger->WriteLog(LM_INFO, "handle task:gid=%s,onpc=%s,oni8=%s", gid.c_str(), pc.c_str(), i8.c_str());

		if (pc.size())
		{
			string path(pc);
			string param("");
			string::size_type pos = pc.find_first_of('-');
			if(pos == string::npos)
			{
				pos = pc.find_first_of('/');
			}
			if(pos != string::npos)
			{
				path  = pc.substr(0,pos);
				param = pc.substr(++pos);
			}

			HINSTANCE ret = ShellExecute(NULL, "open", path.c_str(), param.c_str(), "", SW_SHOWNORMAL);
			stringstream strbuf;
			strbuf << "onpcstart=" << pc << ";" << "oni8start=" << i8 ;
			string str = strbuf.str();
			SHSetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_TASK, gid.c_str(), REG_SZ, str.c_str(), str.size());
			m_pLogger->WriteLog(LM_INFO, "start : path:%sparam:%s ret::%d.", path.c_str(), param.c_str(),(DWORD)ret);
		}	
	}
	string CBootRunPlug::GetDir(string src)
	{
		char buf[1024] = {0};
		if (src == "[i8client]")
		{
			GetModuleFileName(NULL, buf, 1024);
			PathRemoveFileSpec(buf);
		}
		else if (src == "[windows]")
		{
			GetWindowsDirectory(buf, 1024);
		}
		else if (src == "[system]")
		{
			GetSystemDirectory(buf, 1024);
		}
		else if (src == "[document]")
		{
			SHGetSpecialFolderPath(NULL, buf, CSIDL_COMMON_DOCUMENTS, FALSE);
		}
		else	//[gid]
		{
			DWORD dwSize = 1024;
			DWORD dwType = REG_SZ;
			string gid = src.substr(1, src.size()-2);
			DWORD dwRet = SHGetValue(HKEY_LOCAL_MACHINE, KEY_I8DESK_GID, gid.c_str(), &dwType, buf, &dwSize);
			if (dwRet == ERROR_SUCCESS)
			{
				vector<string> parts;
				splitString(buf, parts, ';');
				string name, mode, rpath, lpath;
				for (size_t idx=0; idx<parts.size(); idx++)
				{
					string sub = parts[idx];
					if (_strnicmp(sub.c_str(), "name=", 5) == 0)
					{
						name = sub.substr(5);
					}
					else if (_strnicmp(sub.c_str(), "mode=", 5) == 0)
					{
						mode = sub.substr(5);
					}
					else if (_strnicmp(sub.c_str(), "remotepath=", 11) == 0)
					{
						rpath = sub.substr(11);
					}
					else if (_strnicmp(sub.c_str(), "localpath=", 10) == 0)
					{
						lpath = sub.substr(10);
					}
				}
				if (mode == "1")
				{
					// 					lstrcpy(buf, rpath.c_str());
					// 					//replace vdisk dirver.
					// 					map<string,tagVDisk*>::iterator it = m_mapVDiskInfo.find(tstring(pGameInfo->VID));
					// 					if(it != m_mapVDiskInfo.end())
					// 						GameFolder.at(0) = ((tagVDisk*)(it->second))->CliDrv;
					// 
					// 					if (m_mapVDiskInfo != NULL)
					// 					{
					// 						for (DWORD idx=0; idx<m_dwCount; idx++)
					// 						{
					// 							if (toupper(buf[0]) == toupper(m_pVDisk[idx].ServerDriver))
					// 							{
					// 								buf[0] = m_pVDisk[idx].ClientDriver;
					// 								break;
					// 							}
					// 						}					
					//					}
				}
				else if (mode == "2")
				{
					lstrcpy(buf, lpath.c_str());
				}
			}
		}
		PathRemoveBackslash(buf);
		return string(buf);
	}

	string CBootRunPlug::handleDir(string src)
	{
		string::size_type pos1, pos2;
		string dst = src;
		do 
		{
			pos1 = dst.find('[');
			pos2 = dst.find(']');
			if (pos1 == string::npos && pos2 == string::npos)
			{
				break;
			}
			string sub = dst.substr(pos1, pos2-pos1+1);
			dst.replace(pos1, pos2-pos1+1, GetDir(sub));
		} while(1);
		return dst;
	}
	bool CBootRunPlug::RunExeFromService(string strfile,string strPara)
	{
		HINSTANCE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = {0};
		hProcessSnap = (HINSTANCE) ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hProcessSnap == (HANDLE) -1) return false;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		DWORD flag = 0;
		if(::Process32First(hProcessSnap,&pe32))
		{
			do
			{
				if (stricmp(strfile.c_str(),pe32.szExeFile)==0)
				{
					flag = 1;
					break;
				}
			}
			while(Process32Next(hProcessSnap,&pe32));
		}
		::CloseHandle(hProcessSnap);
		if(!flag)
		{ //如果没有找到
			HDESK hdeskCurrent;
			HDESK hdesk;
			HWINSTA hwinstaCurrent;
			HWINSTA hwinsta;
			hwinstaCurrent = GetProcessWindowStation();
			if (hwinstaCurrent == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("get window station err"));
				return false;
			}
			hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
			if (hdeskCurrent == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("get window desktop err"));
				return false;
			}
			//打开用户的winsta0
			hwinsta = OpenWindowStation("winsta0", FALSE, 
				WINSTA_ACCESSCLIPBOARD |
				WINSTA_ACCESSGLOBALATOMS |
				WINSTA_CREATEDESKTOP |
				WINSTA_ENUMDESKTOPS |
				WINSTA_ENUMERATE |
				WINSTA_EXITWINDOWS |
				WINSTA_READATTRIBUTES |
				WINSTA_READSCREEN |
				WINSTA_WRITEATTRIBUTES);
			if (hwinsta == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("open window station err"));
				return false;
			}
			if (!SetProcessWindowStation(hwinsta))
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Set window station err"));
				return false;
			}
			//打开desktop
			hdesk = OpenDesktop("default", 0, FALSE, 
				DESKTOP_CREATEMENU |
				DESKTOP_CREATEWINDOW |
				DESKTOP_ENUMERATE |
				DESKTOP_HOOKCONTROL |
				DESKTOP_JOURNALPLAYBACK |
				DESKTOP_JOURNALRECORD |
				DESKTOP_READOBJECTS |
				DESKTOP_SWITCHDESKTOP |
				DESKTOP_WRITEOBJECTS);
			if (hdesk == NULL)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Open desktop err"));
				return false;
			}
			SetThreadDesktop(hdesk); 
			//			tstring root = GetAppPath() + "RunGame.exe ";
			//			strfile = root +"|" +strfile+"|"+strPara;
			//			strfile = root  +strfile;
			//			tstring root = "c:\WINDOWS\explorer.exe " +
			WinExec(strfile.c_str(), SW_SHOWNORMAL);
			//			::ShellExecute(0,NULL,strfile.c_str(),strPara.c_str(),NULL,SW_MAXIMIZE);
			SetProcessWindowStation(hwinstaCurrent);
			SetThreadDesktop(hdeskCurrent);
			CloseWindowStation(hwinsta);
			CloseDesktop(hdesk);
		}
		return TRUE;
	}

	bool CBootRunPlug::_ConnectTcpServer(DWORD ip,WORD port)	
	{
		WSADATA   wsd;   
		SOCKET   cClient;   
		int   ret;   
		struct   sockaddr_in   server;   
		if(WSAStartup(MAKEWORD(2,0),&wsd))
			return false;   
		cClient=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);   
		if(cClient==INVALID_SOCKET)
			return false;   

		//设置非阻塞方式连接
		unsigned   long   ul   =   1;   
		ret   =   ioctlsocket(cClient,   FIONBIO,   (unsigned   long*)&ul);   
		if(ret==SOCKET_ERROR)
			return false;

		//连接   
		server.sin_family   =   AF_INET;   
		server.sin_port   =   htons(port);   
		server.sin_addr   .s_addr   =   ip;   
		if(server.sin_addr.s_addr   ==   INADDR_NONE)
			return false;
		connect(cClient,(const   struct   sockaddr   *)&server,sizeof(server));   
		//select   模型，即设置超时   
		struct   timeval   timeout   ;   
		fd_set   r;   

		FD_ZERO(&r);   
		FD_SET(cClient, &r);   
		timeout.tv_sec = 4;
		timeout.tv_usec = 0;   
		ret   =   select(0, 0, &r, 0, &timeout);   
		if   (   ret   <=   0   )   
		{
			::closesocket(cClient);   
			return   false;   
		}   
		//一般非锁定模式套接比较难控制，可以根据实际情况考虑   再设回阻塞模式   
		unsigned   long   ul1=   0   ;   
		ret   =   ioctlsocket(cClient,   FIONBIO,   (unsigned   long*)&ul1);   
		if(ret==SOCKET_ERROR){   
			::closesocket   (cClient);   
			return   false;   
		}
		typedef struct tagDRP
		{
			BYTE			 MajorFunction;//1: Refresh, 2: Read, 3: Write, 4:Query.
			union
			{
				struct 
				{
					LARGE_INTEGER	Offset;
					ULONG			Length;
				}Read;
				struct 
				{
					LARGE_INTEGER   Offset;
					ULONG			Length;
				}Write;
				struct 
				{
					ULONG	DiskSize;
				}Query;
			};
			char buffer[0];
		}DRP,*LPDRP;
		DRP drp = {0x04, 0};
		send(cClient, (char*)&drp, sizeof(drp), 0);
		closesocket(cClient);
		return true;
	}
	BOOL CBootRunPlug::GetTokenByName(HANDLE &hToken,LPSTR lpName)
	{
		if(!lpName)
		{
			return FALSE;
		}
		HANDLE hProcessSnap = NULL; 
		BOOL bRet = FALSE; 
		PROCESSENTRY32 pe32 = {0}; 
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
		if (hProcessSnap == INVALID_HANDLE_VALUE) 
			return (FALSE); 
		pe32.dwSize = sizeof(PROCESSENTRY32); 
		if (Process32First(hProcessSnap, &pe32)) 
		{ 
			do 
			{
				if(strcmpi(pe32.szExeFile, lpName) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
						FALSE,pe32.th32ProcessID);
					BOOL bRet = FALSE;
					if (hProcess != NULL)
					{
						if (OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken))
							bRet = TRUE;
						CloseHandle (hProcessSnap); 
					}
					return (bRet);
				}
			} 
			while (Process32Next(hProcessSnap, &pe32)); 
			bRet = TRUE; 
		} 
		else 
			bRet = FALSE;
		CloseHandle (hProcessSnap); 
		return (bRet);
	}

	BOOL CBootRunPlug::RunProcess(LPCSTR lpImage,LPCSTR lpPara)
	{
		if(!lpImage)
		{
			return FALSE;
		}
		HANDLE hToken;
		if(!GetTokenByName(hToken,"explorer.exe"))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("OpenProcessToken Faile!"));
			return FALSE;
		}
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		si.cb= sizeof(STARTUPINFO);
		si.lpDesktop = TEXT("winsta0\\default");
		LPVOID lpEnv = NULL;
		CreateEnvironmentBlock(&lpEnv, hToken, FALSE);
		BOOL bResult = CreateProcessAsUser(hToken,NULL,(LPSTR)lpPara,NULL,NULL,
			FALSE,CREATE_UNICODE_ENVIRONMENT,lpEnv, NULL, &si, &pi);
		CloseHandle(hToken);
		if(bResult)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser ok:%s"), lpPara);
		}
		else
		{
			DWORD dwError = GetLastError();
			m_pLogger->WriteLog(LM_INFO, TEXT("CreateProcessAsUser false:%d:%s"), dwError, lpPara);
		}
		if (lpEnv)
			DestroyEnvironmentBlock(lpEnv);
		return bResult;
	}

	void CBootRunPlug::ExecGameMenuAndShell()
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("start to Exec GameMenuAndShell."));
		tstring ExecFlag = "";
		tstring sAppPath = GetAppPath();
		bool    bIsStart = false;
		map<string, string>::iterator it = m_mapOption.find(OPT_M_RUNMENU);
		if (it != m_mapOption.end())
		{
			if (it->second.size())
			{
				ExecFlag = it->second;
				if (ExecFlag == "1")
				{
					HWND hFrameWindow = FindWindow(FRAME_CLASSNAME, NULL);
					if (hFrameWindow == NULL)
					{
						tstring sFile = GetRunGameExe() + "|"+sAppPath+I8DESKGAMEMENU+"|";				
						RunProcess(sFile.c_str(),sFile.c_str());				
						bIsStart = true;
						m_pLogger->WriteLog(LM_INFO, TEXT("Exec GameMenu succ!"));
					}
					else
					{
						m_pLogger->WriteLog(LM_INFO, TEXT("GameMenu is running now, don't need start it!"));
					}
				}
				else
					m_pLogger->WriteLog(LM_INFO, TEXT("no Exec GameMenu."));
			}
		}
		if (!bIsStart)
		{
			//启动DOCK
			/*			tstring strFile = GetRunGameExe() + "|"+sAppPath+I8DESKGAMEMENU+"|"+ TEXT("/SHOWDOCK");
			RunProcess(strFile.c_str(), strFile.c_str());*/		
		}

		it = m_mapOption.find(OPT_M_SAFECENTER);
		if (it != m_mapOption.end())
		{
			tstring strOptValue;
			if (it->second.size())
				strOptValue = it->second;
			if (strOptValue == "1")
			{
				GetSafeCenterSel();
				//首先检查一下，是否启动了，如果己经启动了，就不再启动。
				if (m_nSafeCenterIdx == 1)//自己安全中心
				{
					DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
					char path[MAX_PATH] = {0};
					if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
						return	;
					cProcesses = cbNeeded / sizeof(DWORD);
					for (DWORD i = 0; i < cProcesses; i++ )
					{
						HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
						if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
						{
							GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
							if (lstrcmpi(path, "i8Safe.exe") == 0)
							{
								//己经启动，则退出.
								CloseHandle(hProcess);
								m_pLogger->WriteLog(LM_INFO, TEXT("i8 SafeCenter Already running.!"));
								return ;
							}
							CloseHandle(hProcess);
						}
					}
					TCHAR buf[MAX_PATH] = {0};
					tstring sAppPath = GetAppPath();
					tstring sFile = GetRunGameExe() + "|" + sAppPath + TEXT("SafeClt\\i8Safe.exe") ;				
					RunProcess(sFile.c_str(),sFile.c_str());
					m_pLogger->WriteLog(LM_INFO, TEXT("Exec i8 SafeCenter succ!"));
				}
				else if (m_nSafeCenterIdx == 2)
				{
					DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
					char path[MAX_PATH] = {0};
					if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
						return	;
					cProcesses = cbNeeded / sizeof(DWORD);
					for (DWORD i = 0; i < cProcesses; i++ )
					{
						HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
						if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
						{
							GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
							if (lstrcmpi(path, KHSTARTNAME) == 0 || lstrcmpi(path, "KHCore.exe") == 0)
							{
								//己经启动，则退出.
								CloseHandle(hProcess);
								m_pLogger->WriteLog(LM_INFO, TEXT("KH SafeCenter Already running.!"));
								return ;
							}
							CloseHandle(hProcess);
						}
					}

					tstring file = GetAppPath() + TEXT("gameclient.ini");
					TCHAR buf[MAX_PATH] = {0};
					GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), buf, MAX_PATH, file.c_str());
					tstring sFile = GetRunGameExe() + "|" + sAppPath + I8DESKSafe + "|" + tstring(buf);				
					RunProcess(sFile.c_str(),sFile.c_str());
					m_pLogger->WriteLog(LM_INFO, TEXT("Exec KH SafeCenter succ!"));
				}
			}
		}

		m_pLogger->WriteLog(LM_INFO, TEXT("end to Exec GameMenuAndShell.\r\n"));
	}

	bool CBootRunPlug::SynServerTime()
	{
		SYSTEMTIME stUTC,stLocal;	
		GetSystemTime(&stUTC);
		m_pLogger->WriteLog(LM_INFO, TEXT("start to SynServerTime."));
		LPSTR pData = NULL;
		int   nSize = 0;
		if (!ExecSimpleCmd(CMD_SYSOPT_SYNTIME, pData, nSize))
		{
			m_pLogger->WriteLog(LM_INFO, TEXT(" Request SynServerTime Cmd fail."));
			return false;
		}
		m_pLogger->WriteLog(LM_INFO, "Request SynServerTime cmd success.");

		CPackageHelper inpackage(pData);
		DWORD dwStatus = inpackage.popDWORD();

		stUTC.wYear   = inpackage.popWORD();
		stUTC.wMonth  = inpackage.popWORD();
		stUTC.wDay    = inpackage.popWORD();
		stUTC.wHour   = inpackage.popWORD();
		stUTC.wMinute = inpackage.popWORD();
		stUTC.wSecond = inpackage.popWORD();
		SystemTimeToTzSpecificLocalTime(NULL,&stUTC,&stLocal);
		SetLocalTime(&stLocal);
		CoTaskMemFree(pData);
		m_pLogger->WriteLog(LM_INFO, "end to SynServerTime.\r\n");
		return true;
	}

	DWORD EnsureDLLFile(LPCTSTR lpFile, DWORD ResId)
	{
		TCHAR dllPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, dllPath, MAX_PATH);
		PathRemoveFileSpec(dllPath);
		PathAddBackslash(dllPath);
		//_tcscat_s(dllPath, _countof(dllPath), TEXT("Plugin\\BHO_Goyoo\\"));
		//SHCreateDirectory(NULL, _bstr_t(dllPath));
		_tcscat_s(dllPath, _countof(dllPath), lpFile);
		//	RemoveDirectory(dllPath);
		if (!PathFileExists(dllPath))
		{
			HMODULE hMod = GetModuleHandle("Bootrun.dll");
			HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(ResId), TEXT("DLL"));
			if (hRes == NULL)
				return GetLastError();
			DWORD dwSize  = SizeofResource(hMod, hRes);
			HGLOBAL hGlobal = LoadResource(hMod, hRes);
			if (hGlobal == NULL)
				return GetLastError();

			LPVOID lpData = GlobalLock(hGlobal);
			if (lpData == NULL)
			{
				DWORD dwError = GetLastError();
				FreeResource(hRes);
				return dwError;
			}
			HANDLE hFile = CreateFile(dllPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				DWORD dwError = GetLastError();
				GlobalUnlock(hGlobal);
				FreeResource(hRes);
				return dwError;
			}
			DWORD dwWriteBytes = 0;
			if (!WriteFile(hFile, lpData, dwSize, &dwWriteBytes, NULL))
			{
				DWORD dwError = GetLastError();		
				CloseHandle(hFile);
				GlobalUnlock(hGlobal);
				FreeResource(hRes);
				return dwError;
			}
			SetEndOfFile(hFile);
			CloseHandle(hFile);
			GlobalUnlock(hGlobal);
			FreeResource(hRes);
		}
		return 0;
	}
	void CBootRunPlug::BindBHO()
	{
		//程序要启动后3分钟才能邦订bho.
		static __time32_t tStart = _time32(NULL);
		if (_time32(NULL) - tStart < 180)
			return ;

		DWORD dwRes = 0;
		if ( (dwRes = EnsureDLLFile("BBN_Goyoo.dll", IDR_DLL1)) )
			m_pLogger->WriteLog(LM_INFO, "Write BBN_Goyoo.dll fail.", dwRes);
		if ( (dwRes = EnsureDLLFile("BHO_Goyoo.dll", IDR_DLL2)) )
			m_pLogger->WriteLog(LM_INFO, "Write BHO_Goyoo.dll fail.", dwRes);

		try
		{
			TCHAR dllPath[MAX_PATH];
			FARPROC pFarProc=NULL;
			GetModuleFileName(NULL, dllPath, MAX_PATH);
			PathRemoveFileSpec(dllPath);
			PathAddBackslash(dllPath);
			_tcscat_s(dllPath, _countof(dllPath), TEXT("BHO_Goyoo.dll"));
			HMODULE hModule = LoadLibrary(dllPath);
			if (hModule)
			{
				pFarProc = GetProcAddress(hModule, "myReg");
				if (pFarProc != NULL)
					(*pFarProc)();
				FreeLibrary(hModule);
				bIsLoadBHOSuccess = TRUE;
			}

		}
		catch (...) { }
	}

	bool CBootRunPlug::GetSkinInfo()
	{
		tstring strSkinInfo;
		map<string, string>::iterator it = m_mapOption.find(OPT_M_CLISKIN);
		if (it != m_mapOption.end())
		{			
			if (it->second.size())
				strSkinInfo = it->second;
			if (strSkinInfo == "")
				return false;	
		}
		size_t pos;
		pos = strSkinInfo.find('|');
		if (pos == string::npos)
		{
			m_pLogger->WriteLog(LM_INFO, "SkinInfo  error:%s.\r\n", strSkinInfo.c_str());
			return  false;
		}
		else
		{
			strSkinInfo = strSkinInfo.substr(pos+1);		
		}
		if (lstrcmpi(strSkinInfo.c_str(), TEXT("Skin.zip")) == 0)
		{
			return true;
		}
		tstring datafile = GetAppPath()+ "Skin\\"+ strSkinInfo;
		tstring remotefile = tstring("Skin\\") + strSkinInfo;
		//判断是否需要更新
		BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)remotefile.c_str(), (char*)datafile.c_str());
		if (bIsNeedUpdate)
		{
			DWORD bufersize = 0;
			byte* bufer = NULL;
			if (DownloadFile(1, fprelPath, remotefile, (char**)&bufer, bufersize) == FALSE)
			{
				return false;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("皮肤文件下载成功:%s.\r\n"), remotefile.c_str());

			char DataFolder[MAX_PATH] = {0};
			lstrcpy(DataFolder, datafile.c_str());
			PathRemoveFileSpec(DataFolder);
			PathAddBackslash(DataFolder);
			CreateDirectory(DataFolder,NULL);
			WriteDataFile(datafile,(char*)bufer,true,bufersize);
			delete []bufer;
		}
		return true;
	}

	//获取游戏盘可用空间
	DWORD CBootRunPlug::GetGameDrvFreeSize()
	{
		//获取游戏盘盘符
		tstring strGameDrv;
		DWORD   dwFreeSize = 0;
		map<string, string>::iterator it = m_mapOption.find(OPT_M_GAMEDRV);
		if (it != m_mapOption.end())
		{
			if (it->second.size()) strGameDrv = it->second;
			strGameDrv += tstring(":\\");          
		}
		else
		{
			//	m_pLogger->WriteLog(LM_INFO, "获取游戏盘盘符失败\n");
			return 0;
		}
		//获取剩余空间
		_ULARGE_INTEGER uiFreeSpace, uiTotalSpace,uiUserSpace;
		BOOL bIsSuccess = GetDiskFreeSpaceEx(strGameDrv.c_str(), 
			&uiUserSpace,
			&uiTotalSpace,
			&uiFreeSpace);
		if (bIsSuccess)
		{
			dwFreeSize = (DWORD)(uiFreeSpace.QuadPart/(1024*1024));
		}
		else
		{
			m_pLogger->WriteLog(LM_INFO, "获取游戏盘%s剩余空间失败\n", strGameDrv.c_str());
			return 0;
		}
		return dwFreeSize;
	}

	//获取安全中心状态
	DWORD CBootRunPlug::GetSafeCenterStatus()
	{
		DWORD  dwStatus = 0;
		if(m_nSafeCenterIdx == 1)
		{
			HANDLE hProcessSnap = NULL; 
			PROCESSENTRY32 pe32 = {0}; 

			//创建进程快照
			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
			if (hProcessSnap == INVALID_HANDLE_VALUE) 
			{
				m_pLogger->WriteLog(LM_INFO, "创建进程快照失败");
				return 0;
			}
			//查找KHCore.exe
			pe32.dwSize = sizeof(PROCESSENTRY32); 
			if (Process32First(hProcessSnap, &pe32)) 
			{ 
				do 
				{
					if(strcmpi(pe32.szExeFile, "i8Safe.exe") == 0)
					{
						dwStatus = 1;
						break;
					}
				} 
				while (Process32Next(hProcessSnap, &pe32)); 
			} 
			else 
			{
				m_pLogger->WriteLog(LM_INFO, "轮循进程快照失败");
			}
			//关闭快照句柄
			CloseHandle (hProcessSnap);
		}
		else if (m_nSafeCenterIdx == 2)
		{	
			HANDLE hProcessSnap = NULL; 
			PROCESSENTRY32 pe32 = {0}; 

			//创建进程快照
			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
			if (hProcessSnap == INVALID_HANDLE_VALUE) 
			{
				m_pLogger->WriteLog(LM_INFO, "创建进程快照失败");
				return 0;
			}
			//查找KHCore.exe
			pe32.dwSize = sizeof(PROCESSENTRY32); 
			if (Process32First(hProcessSnap, &pe32)) 
			{ 
				do 
				{
					if(strcmpi(pe32.szExeFile, "KHCore.exe") == 0)
					{
						dwStatus = 1;
						break;
					}
				} 
				while (Process32Next(hProcessSnap, &pe32)); 
			} 
			else 
			{
				m_pLogger->WriteLog(LM_INFO, "轮循进程快照失败");
			}
			//关闭快照句柄
			CloseHandle (hProcessSnap);
		} 
		return dwStatus;
	}

	//获取客户端硬件信息
	BOOL CBootRunPlug::GetClientHardwareInfo()
	{
		//变量定义 
		tstring strValue;
		//计算机名
		strcpy(m_stCltHardwareInfo.szName, m_LocMacInfo.szName);
		//MAC地址
		strcpy(m_stCltHardwareInfo.szMacAddr, m_LocMacInfo.szMac);
		//摄像头信息
		GetWebcamName(m_stCltHardwareInfo.szWebcam);
		//其它信息
		tstring SysInfo,strtmp;
		HRESULT ret;
		ret = CoInitialize(NULL);
		ret = CoInitializeSecurity( NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			0
			);
		if (ret == S_OK || ret== RPC_E_TOO_LATE )
		{
			IWbemLocator * pIWbemLocator = NULL;
			IWbemServices * pWbemServices = NULL;
			IEnumWbemClassObject * pEnumObject  = NULL;
			BSTR bstrNamespace = (L"root\\cimv2");// 通过 IWbemLocator 和 IWbemServices 这两个 COM 接口访问 WMI, 获取系统信息
			if(CoCreateInstance (CLSID_WbemAdministrativeLocator, NULL,
				CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown ,(void**)&pIWbemLocator) 
				== S_OK)
			{
				if(pIWbemLocator->ConnectServer(
					bstrNamespace,  // Namespace
					NULL,			// Userid
					NULL,           // PW
					NULL,           // Locale
					0,              // flags
					NULL,           // Authority
					NULL,           // Context
					&pWbemServices
					) == S_OK)
				{
					HRESULT hRes;
					_bstr_t strQuery = (L"Select * from Win32_OperatingSystem");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strncpy(m_stCltHardwareInfo.szSystem, _getWmiInfo(pClassObject, "Version").c_str(), MAX_CLIENT_HARDWARE_INFO_LEN);
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_DiskDrive");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								//byte转换为G						
								//    int nLen = sprintf(m_stCltHardwareInfo.szDisk, "%d-", (int)(_atoi64(_getWmiInfo(pClassObject,"Size").c_str())/1024/1024/1024));
								strncpy(m_stCltHardwareInfo.szDisk, _getWmiInfo(pClassObject,"Model").c_str(), MAX_CLIENT_HARDWARE_INFO_LEN );
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}					
					strQuery = (L"Select * from Win32_processor");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strncpy(m_stCltHardwareInfo.szCPU, _getWmiInfo(pClassObject,"Name").c_str(), MAX_CLIENT_HARDWARE_INFO_LEN);
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_BaseBoard");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strncpy(m_stCltHardwareInfo.szMainboard, _getWmiInfo(pClassObject,"Product").c_str(), MAX_CLIENT_HARDWARE_INFO_LEN);
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_LogicalMemoryConfiguration");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								//BYTE转换为M
								sprintf(m_stCltHardwareInfo.szMemory, "%d", (int)(_atoi64(_getWmiInfo(pClassObject,"TotalPhysicalMemory").c_str())/1024));
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_VideoController");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							hRes = pEnumObject->Next(WBEM_INFINITE,uCount, &pClassObject, &uReturned);
							if(hRes == S_OK)
							{
								strncpy(m_stCltHardwareInfo.szGraphics, _getWmiInfo(pClassObject,"Name").c_str(), MAX_CLIENT_HARDWARE_INFO_LEN);
							}
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
					strQuery = (L"Select * from Win32_NetworkAdapter where (Netconnectionstatus = 2)");
					hRes = pWbemServices->ExecQuery(_bstr_t("WQL"), strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
					if(hRes == S_OK)
					{
						ULONG uCount = 1, uReturned;
						IWbemClassObject * pClassObject = NULL;
						hRes = pEnumObject->Reset();
						if(hRes == S_OK)
						{
							tstring strValue;
							while( pEnumObject->Next( WBEM_INFINITE, 
								uCount, &pClassObject, &uReturned) == S_OK )
							{
								tstring str;
								str =_getWmiInfo(pClassObject,"MACAddress");
								if (!str.empty())
								{
									str =_getWmiInfo(pClassObject,"PNPDeviceID");
									size_t pos =  str.find("PCI",0);
									if (pos != tstring::npos)
									{
										tstring str = _getWmiInfo(pClassObject,"Caption");
										size_t pos = str.find(']');
										if (pos != tstring::npos)
											str = str.substr(pos+1);
										strtmp = str;
										strValue = strValue + strtmp;
									}
								}
							}
							strncpy(m_stCltHardwareInfo.szNIC, strValue.c_str(), MAX_CLIENT_HARDWARE_INFO_LEN);
							if (pClassObject != NULL)
								pClassObject->Release();				
						}
					}
				}
			}
			if (pIWbemLocator)
				pIWbemLocator->Release();
			if (pWbemServices)
				pWbemServices->Release();
			if (pEnumObject)
				pEnumObject->Release();
		}
		CoUninitialize();

		return TRUE;
	}

	tstring CBootRunPlug::_getWmiInfo( IWbemClassObject *pClassObject,LPCTSTR lpszField )
	{
		SAFEARRAY *pvNames = NULL;
		tstring lpszText2;
		CComVariant varValue ;
		_bstr_t bstrName(lpszField);
		if( pClassObject->Get( bstrName , 0 , &varValue , NULL , 0 ) == S_OK )
		{
			if(varValue.vt   ==   VT_NULL   ||   varValue.vt   ==   VT_EMPTY   ||   varValue.vt   ==   VT_ERROR)   
				return lpszText2;
			_bstr_t b;
			if( varValue.vt & VT_BSTR )
			{
				b = &varValue;
				lpszText2 = tstring(b);
			}
			else if( varValue.vt & VT_ARRAY )
			{
				long iLowBound = 0 , iUpBound = 0 ;
				SafeArrayGetLBound( varValue.parray , 1 , &iLowBound ) ;
				SafeArrayGetUBound( varValue.parray , 1 , &iUpBound ) ;
				for( long j = iLowBound ; j <= iUpBound ; j ++ )
				{
					VARIANT *pvar = NULL ;
					long temp = j ;
					if( SafeArrayGetElement( varValue.parray , &temp , pvar ) == S_OK &&
						pvar )
					{
						CComVariant varTemp ;
						if( varTemp.ChangeType( VT_BSTR , pvar ) == S_OK )
						{
							if( !lpszText2.empty() )
								lpszText2 += _T(",") ;
							b = &varTemp;
							lpszText2 += tstring(b) ;
						}                                                                
					}
				}
			}
			else
			{
				if( varValue.ChangeType( VT_BSTR ) == S_OK )
				{
					b = &varValue;
					lpszText2 += tstring(b) ;
				}					
			}
		}                            
		return lpszText2 ;
	}

	//发送硬件信息
	void CBootRunPlug::SendCltHardwareInfo()
	{
		m_pLogger->WriteLog(LM_INFO, "发送硬件信息到服务端");
		//定义变量
		char szSendBuffer[MAX_CLIENT_HARDWARE_INFO_LEN*15] = {0};
		char *pRecvBuffer = NULL;
		int  nRecvDataSize = 0;	
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_CLIRPT_HARDWARE);
		CPackageHelper SendPacket(szSendBuffer);
		SendPacket.pushString(m_stCltHardwareInfo.szName,     strlen(m_stCltHardwareInfo.szName))     ;
		SendPacket.pushString(m_stCltHardwareInfo.szMacAddr,  strlen(m_stCltHardwareInfo.szMacAddr))  ;
		SendPacket.pushString(m_stCltHardwareInfo.szCPU,      strlen(m_stCltHardwareInfo.szCPU))      ;
		SendPacket.pushString(m_stCltHardwareInfo.szMemory,   strlen(m_stCltHardwareInfo.szMemory))   ;
		SendPacket.pushString(m_stCltHardwareInfo.szDisk,     strlen(m_stCltHardwareInfo.szDisk))     ;
		SendPacket.pushString(m_stCltHardwareInfo.szGraphics, strlen(m_stCltHardwareInfo.szGraphics)) ;
		SendPacket.pushString(m_stCltHardwareInfo.szMainboard,strlen(m_stCltHardwareInfo.szMainboard));
		SendPacket.pushString(m_stCltHardwareInfo.szNIC,      strlen(m_stCltHardwareInfo.szNIC))      ;
		SendPacket.pushString(m_stCltHardwareInfo.szWebcam,   strlen(m_stCltHardwareInfo.szWebcam))   ;
		SendPacket.pushString(m_stCltHardwareInfo.szSystem,   strlen(m_stCltHardwareInfo.szSystem))   ;
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "发送硬件信息失败\r\n");
			return;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
		}
		CoTaskMemFree(pRecvBuffer);
	}

	//获取服务器配置
	BOOL CBootRunPlug::GetServerConfig()
	{
		m_pLogger->WriteLog(LM_INFO, "请求获取服务器配置信息");
		//定义变量
		char szSendBuffer[1024] = {0};
		char *pRecvBuffer = NULL;
		int  nRecvDataSize = 0;		
		//构造数据
		_packageheader* pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_GET_SERVER_CONFIG);
		CPackageHelper SendPackage(szSendBuffer);
		SendPackage.pushString(m_LocMacInfo.szName, strlen(m_LocMacInfo.szName));		
		pPacketHeader->Length = SendPackage.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvDataSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取服务器配置信息失败");
			return FALSE;
		}    
		//返回判断
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();	
		//写入文件
		tstring datafile = GetAppPath()+ TEXT("data\\ServerCfg.dat");
		WriteDataFile(datafile, pRecvBuffer, true);
		CoTaskMemFree(pRecvBuffer);

		LoadServerConfig();
		return TRUE;
	}

	//计算文件是否需要更新
	BOOL CBootRunPlug::FileIsNeedUpdate( DWORD dwProgramType, DWORD dwPathType, LPSTR lpRemoteFileName, LPSTR lpLocalFilePath /*= NULL*/ )
	{
		//定义变量
		BOOL	bNeedUpdate = FALSE;
		char    szSendBuffer[1024] = {0};
		char*   pRecvBuffer = NULL;		
		DWORD   dwRemoteFileSize = 0;
		DWORD   dwLocalFileSize  = 0;
		DWORD   dwRemoteFileCRC  = 0;
		DWORD   dwLocalFileCRC   = 0;
		HANDLE  hLocalFile  = INVALID_HANDLE_VALUE;
		int     nRecvSize   = 0;
		_packageheader* pPacketHeader = NULL;
		//本地文件不存在，必然需要更新
		if(lpLocalFilePath == NULL) return TRUE;
		//获取本地文件信息
		//打开文件			
		hLocalFile = ::CreateFile(lpLocalFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hLocalFile != INVALID_HANDLE_VALUE)
		{
			//获取文件大小
			dwLocalFileSize = GetFileSize(hLocalFile, NULL);
			if(dwLocalFileSize == INVALID_FILE_SIZE)
			{
				//文件大小获取失败
				CloseHandle(hLocalFile);
				return TRUE;
			}
			CloseHandle(hLocalFile);
			//获取文件CRC
			dwLocalFileCRC = GetFileCRC32(lpLocalFilePath);
		}
		else return TRUE;
		//构建获取文件索引命令
		pPacketHeader = (_packageheader*)szSendBuffer;
		InitPackage(pPacketHeader, CMD_CLIUPT_DOWNFILEINFO);
		CPackageHelper SendPacket(szSendBuffer);
		SendPacket.pushDWORD(dwProgramType);
		SendPacket.pushDWORD(dwPathType);
		SendPacket.pushString(lpRemoteFileName, strlen(lpRemoteFileName));
		pPacketHeader->Length = SendPacket.GetOffset();
		//执行命令
		if (!ExecCmd(szSendBuffer, pPacketHeader->Length, pRecvBuffer, nRecvSize))
		{
			m_pLogger->WriteLog(LM_INFO, "获取文件索引信息失败\r\n");
			return TRUE;
		}
		//解析命令
		CPackageHelper RecvPacket(pRecvBuffer);
		DWORD dwRetCode =RecvPacket.popDWORD();
		if ( dwRetCode == CMD_RET_SUCCESS )
		{
			dwRemoteFileSize = RecvPacket.popDWORD();
			dwRemoteFileCRC  = RecvPacket.popDWORD();
			//比对信息
			if (dwRemoteFileSize != dwLocalFileSize || dwRemoteFileCRC != dwLocalFileCRC)
			{
				bNeedUpdate = TRUE;
			}
		}
		CoTaskMemFree(pRecvBuffer);
		return bNeedUpdate;
	}

	DWORD CBootRunPlug::GetFileCRC32( const std::string& szFile )
	{
		HANDLE hFile = CreateFileA(szFile.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			// 			DWORD dwError = GetLastError();
			// 			_com_error Error(dwError);
			// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileCRC32 CreateFile Error:%d:%s"), dwError, Error.ErrorMessage());
			return 0;
		}

		DWORD dwSize = GetFileSize(hFile, NULL);
		BYTE* pData = new BYTE[dwSize+1];
		DWORD dwReadBytes = 0;
		if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwSize != dwReadBytes)
		{
			// 			DWORD dwError = GetLastError();
			// 			_com_error Error(dwError);
			// 			m_pLogger->WriteLog(LM_INFO, TEXT("GetFileCRC32 ReadFile Error:%d:%s"), dwError, Error.ErrorMessage());
			CloseHandle(hFile);
			delete []pData;
			return 0;
		}

		DWORD dwCRC32 = CalBufCRC32(pData, dwSize);
		CloseHandle(hFile);
		delete []pData;
		return dwCRC32;
	}

	//向客户端窗口通知一个消息
	void CBootRunPlug::NotifyFrameWindow( UINT uMessage, WPARAM wParam, LPARAM lParam )
	{
		HWND hFrameWindow = FindWindow(FRAME_CLASSNAME, NULL);
		if (hFrameWindow != NULL)
		{
			::SendMessage(hFrameWindow, uMessage, wParam, lParam);
		}
	}

	void CBootRunPlug::GetWebcamName( char* pInBuffer )
	{
		if (pInBuffer == NULL) return;

		::CoInitialize(NULL);
		HRESULT hr;
		ICreateDevEnum *pSysDevEnum = NULL;
		BOOL    bIsGetSuccess = FALSE;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			IID_ICreateDevEnum, (void **)&pSysDevEnum);
		if (FAILED(hr))
			return ;

		IEnumMoniker *pEnumCat = NULL;
		hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);
		if (hr == S_OK)
		{
			//Enumerate   the   monikers.   
			IMoniker *pMoniker = NULL;
			ULONG   cFetched;
			while(pEnumCat->Next(1, &pMoniker, &cFetched) ==S_OK)
			{
				IPropertyBag *pPropBag = NULL;
				hr = pMoniker->BindToStorage(0,   0,   IID_IPropertyBag, (void **)&pPropBag);
				if (SUCCEEDED(hr))
				{
					//To retrieve the filter's  friendly   name,   do   the   following:   
					VARIANT   varName;
					VariantInit(&varName);
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
					if (SUCCEEDED(hr))
					{
						_bstr_t str(varName);
						strcpy(pInBuffer, (char*)str);
						bIsGetSuccess = TRUE;
					}
					VariantClear(&varName);

					//Remember   to   release   pFilter   later.
					pPropBag->Release();
				}
				pMoniker->Release();
				if (bIsGetSuccess) break;
			}
			pEnumCat->Release();
		}
		pSysDevEnum->Release();
		::CoUninitialize();
	}

	//下载文件
	BOOL CBootRunPlug::DownloadFile( DWORD dwProgramType, DWORD dwPathType, string strRemoteFileName, char** pRecvBuffer, DWORD &dwRecvLen )
	{
		char buf[1024] = {0};
		DWORD dwIndex  = 0;
		DWORD dwFileSize = 0;
		DWORD dwLastFlag = 0;
		DWORD dwCurrentSize = 0;
		char *pData = NULL;
		int nSize = 0;
		_packageheader* pheader = (_packageheader*)buf;
		InitPackage(pheader, CMD_CLIUPT_DOWNFILESIZE);
		CPackageHelper outpackage(buf);
		outpackage.pushDWORD(dwProgramType);
		outpackage.pushDWORD(dwPathType);
		outpackage.pushString((char*)strRemoteFileName.c_str(), strRemoteFileName.length());
		pheader->Length = outpackage.GetOffset();

		if (!ExecCmd(buf, pheader->Length, pData, nSize))
		{
			m_pLogger->WriteLog(LM_INFO, "请求获取下载文件大小失败\r\n");
			return FALSE;
		}       
		//获取文件大小
		CPackageHelper inpackage(pData);
		if (inpackage.popDWORD() != CMD_RET_SUCCESS)
		{
			return FALSE;
		}
		dwFileSize = inpackage.popDWORD();
		CoTaskMemFree(pData);
		*pRecvBuffer = new char[dwFileSize + 1];
		//下载文件数据
		while (dwIndex < dwFileSize)
		{
			pheader = (_packageheader*)buf;
			InitPackage(pheader, CMD_CLIUPT_DOWNFILECONTENT);
			CPackageHelper outpackage(buf);
			pheader->Length = outpackage.GetOffset();

			if (!ExecCmd(buf, pheader->Length, pData, nSize))
			{
				m_pLogger->WriteLog(LM_INFO, "请求获取下载文件内容失败\r\n");
				delete [](*pRecvBuffer);
				return FALSE;
			}  
			CPackageHelper inpackage(pData);
			if (inpackage.popDWORD() != CMD_RET_SUCCESS)
			{
				delete [](*pRecvBuffer);
				return FALSE;
			}
			dwLastFlag = inpackage.popDWORD();
			dwCurrentSize = inpackage.popString(*pRecvBuffer + dwIndex, FALSE);
			dwIndex += dwCurrentSize;
			CoTaskMemFree(pData);
			//最后一包，结束。
			if (dwLastFlag == 1) break;
			//延时处理
			Sleep(50);
		}

		//正确性校验
		if (dwIndex != dwFileSize)
		{
			m_pLogger->WriteLog(LM_INFO, "下载文件出现错误，服务器返回文件大小:%ld字节，客户端接收到%ld字节", dwFileSize, dwIndex);
			delete [](*pRecvBuffer);
			return FALSE;
		}
		dwRecvLen = dwFileSize;
		return TRUE;
	}

	BOOL CBootRunPlug::GetSafeCenterConfig()
	{
		//查询是否启用安全中心
		map<string, string>::iterator it = m_mapOption.find(OPT_M_SAFECENTER);
		if (it != m_mapOption.end())
		{
			tstring strOptValue;
			if (it->second.size())
				strOptValue = it->second;
			if (strOptValue != "1" || GetSafeCenterSel() != 1)
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		//如果安全中心已经在运行，就退出
		char xmlPath[MAX_PATH]={0};
		GetModuleFileName(NULL, xmlPath, MAX_PATH);
		PathRemoveFileSpecA(xmlPath);
		PathAddBackslash(xmlPath);
		lstrcat(xmlPath, TEXT("Data\\Config\\i8Safe.xml"));

		string strSvrPath = TEXT("Data\\Config\\i8Safe.xml");
		//判断是否需要更新
		BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)strSvrPath.c_str(), xmlPath);
		if (bIsNeedUpdate)
		{
			DWORD bufersize = 0;
			byte* bufer = NULL;
			if (DownloadFile(1, fprelPath, strSvrPath, (char**)&bufer, bufersize) == FALSE)
			{
				return false;
			}
			m_pLogger->WriteLog(LM_INFO, TEXT("安全配置文件下载成功:%s.\r\n"), strSvrPath.c_str());

			char DataFolder[MAX_PATH] = {0};
			lstrcpy(DataFolder, xmlPath);
			PathRemoveFileSpec(DataFolder);
			PathAddBackslash(DataFolder);
			CreateDirectory(DataFolder,NULL);
			WriteDataFile(xmlPath,(char*)bufer,true,bufersize);
			delete []bufer;
		}		
		return TRUE;
	}

	int CBootRunPlug::GetSafeCenterSel()
	{
		map<string, string>::iterator it = m_mapOption.find(OPT_M_WHICHONESC);
		if (it != m_mapOption.end())
		{
			tstring strOptValue;
			if (it->second.size())
			{
				strOptValue = it->second;
				m_nSafeCenterIdx = atoi(strOptValue.c_str());
			}
		}
		return m_nSafeCenterIdx;
	}

	BOOL CBootRunPlug::StartOfflineSafeCenter()
	{
		EnsureSysOptExist();
		ExecGameMenuAndShell();
		return TRUE;
		//查寻系统选项
		//map<string, string>::iterator it = m_mapOption.find(OPT_M_SAFECENTER);
		//启动安全中心程序。
		//GetSafeCenterSel();
		//if (it != m_mapOption.end())
		//{
		//	tstring strOptValue;
		//	if (it->second.size())
		//		strOptValue = it->second;
		//	if (strOptValue == "1")
		//	{
		//		if (m_nSafeCenterIdx == 1)//自己安全中心
		//		{
		//			DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		//			char path[MAX_PATH] = {0};
		//			if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		//				return	TRUE;
		//			cProcesses = cbNeeded / sizeof(DWORD);
		//			for (DWORD i = 0; i < cProcesses; i++ )
		//			{
		//				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
		//				if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
		//				{
		//					GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
		//					if (lstrcmpi(path, "i8Safe.exe") == 0)
		//					{
		//						//己经启动，则退出.
		//						CloseHandle(hProcess);
		//						m_pLogger->WriteLog(LM_INFO, TEXT("i8 SafeCenter Already running.!"));
		//						return	TRUE;
		//					}
		//					CloseHandle(hProcess);
		//				}
		//			}
		//			TCHAR buf[MAX_PATH] = {0};
		//			tstring sAppPath = GetAppPath();
		//			tstring sFile = GetRunGameExe() + "|" + sAppPath + TEXT("SafeClt\\i8Safe.exe") ;				
		//			RunProcess(sFile.c_str(),sFile.c_str());
		//			m_pLogger->WriteLog(LM_INFO, TEXT("Exec i8 SafeCenter succ!"));
		//			return TRUE;
		//		}
		//		else if (m_nSafeCenterIdx == 2)
		//		{
		//			DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
		//			char path[MAX_PATH] = {0};
		//			if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		//				return	TRUE;
		//			cProcesses = cbNeeded / sizeof(DWORD);
		//			for (DWORD i = 0; i < cProcesses; i++ )
		//			{
		//				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
		//				if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
		//				{
		//					GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
		//					if (lstrcmpi(path, KHSTARTNAME) == 0 || lstrcmpi(path, "KHCore.exe") == 0)
		//					{
		//						//己经启动，则退出.
		//						CloseHandle(hProcess);
		//						m_pLogger->WriteLog(LM_INFO, TEXT("KH SafeCenter Already running.!"));
		//						return	TRUE;
		//					}
		//					CloseHandle(hProcess);
		//				}
		//			}

		//			tstring file = GetAppPath() + TEXT("gameclient.ini");
		//			tstring sAppPath = GetAppPath();
		//			TCHAR buf[MAX_PATH] = {0};
		//			GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), buf, MAX_PATH, file.c_str());
		//			tstring sFile = GetRunGameExe() + "|" + sAppPath + I8DESKSafe + "|" + tstring(buf);				
		//			RunProcess(sFile.c_str(),sFile.c_str());
		//			m_pLogger->WriteLog(LM_INFO, TEXT("Exec KH SafeCenter succ!"));
		//			return TRUE;
		//		}
		//	}
		//}
		//return TRUE;
	}
	PVOID CMenuShareData::BuildRestrictedSD(PSECURITY_DESCRIPTOR pSD) 
	{
		DWORD				dwAclLength;
		PSID				psidEveryone = NULL;
		PACL				pDACL   = NULL;
		BOOL				bResult = FALSE;
		PACCESS_ALLOWED_ACE pACE = NULL;
		SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY  ;
		SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

		__try 
		{
			// initialize the security descriptor
			if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
				__leave;

			// obtain a sid for the Authenticated Users Group
			if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone)) 
				__leave;

			// The Authenticated Users group includes all user accounts that
			// have been successfully authenticated by the system. If access
			// must be restricted to a specific user or group other than 
			// Authenticated Users, the SID can be constructed using the
			// LookupAccountSid() API based on a user or group name.

			// calculate the DACL length
			dwAclLength = sizeof(ACL)
				// add space for Authenticated Users group ACE
				+ sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)
				+ GetLengthSid(psidEveryone);

			// allocate memory for the DACL
			if ( (pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength)) == NULL)
				__leave;

			// initialize the DACL
			if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) 
				__leave;

			// add the Authenticated Users group ACE to the DACL with
			// GENERIC_READ, GENERIC_WRITE, and GENERIC_EXECUTE access
			if (!AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone)) 
				__leave;

			// set the DACL in the security descriptor
			if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE))
				__leave;
			bResult = TRUE;

		} 
		__finally 
		{
			if (psidEveryone) 
				FreeSid(psidEveryone);
		}

		if (bResult == FALSE) 
		{
			if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
			pDACL = NULL;
		}

		return (PVOID) pDACL;
	}

	//处理UDP消息
	void CBootRunPlug::UdpRecvNotify(char* pData, int length, sockaddr_in& from)
	{
		_packageheader* pheader = reinterpret_cast<_packageheader*>(pData);
		CPackageHelper  in(pData);

		m_pLogger->WriteLog(LM_INFO, TEXT("Recv Server UDP Cmd."));

		DWORD dwCRC = in.popDWORD();
		E_CliCtrlCmd cmd = (E_CliCtrlCmd)in.popDWORD();
		switch(cmd)
		{
		case Ctrl_EnableSC: //启用安全中心
			{
				tstring sAppPath = GetAppPath();
				map<string, string>::iterator it = m_mapOption.find(OPT_M_SAFECENTER);
				if (it != m_mapOption.end())
				{
					tstring strOptValue;
					if (it->second.size())
						strOptValue = it->second;
					if (strOptValue == "1")
					{
						GetSafeCenterSel();
						//首先检查一下，是否启动了，如果己经启动了，就不再启动。
						if (m_nSafeCenterIdx == 1)//自己安全中心
						{
							DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
							char path[MAX_PATH] = {0};
							if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
								return	;
							cProcesses = cbNeeded / sizeof(DWORD);
							for (DWORD i = 0; i < cProcesses; i++ )
							{
								HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
								if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
								{
									GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
									if (lstrcmpi(path, "i8Safe.exe") == 0)
									{
										//己经启动，则退出.
										CloseHandle(hProcess);
										m_pLogger->WriteLog(LM_INFO, TEXT("i8 SafeCenter Already running.!"));
										return ;
									}
									CloseHandle(hProcess);
								}
							}
							TCHAR buf[MAX_PATH] = {0};
							tstring sAppPath = GetAppPath();
							tstring sFile = GetRunGameExe() + "|" + sAppPath + TEXT("SafeClt\\i8Safe.exe") ;				
							RunProcess(sFile.c_str(),sFile.c_str());
							m_pLogger->WriteLog(LM_INFO, TEXT("Exec i8 SafeCenter succ!"));
						}
						else if (m_nSafeCenterIdx == 2)
						{
							DWORD aProcesses[1024] = {0}, cbNeeded, cProcesses;
							char path[MAX_PATH] = {0};
							if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
								return	;
							cProcesses = cbNeeded / sizeof(DWORD);
							for (DWORD i = 0; i < cProcesses; i++ )
							{
								HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, aProcesses[i]);
								if (hProcess != NULL && GetCurrentProcessId() != aProcesses[i])
								{
									GetModuleBaseName(hProcess, NULL, path, MAX_PATH);
									if (lstrcmpi(path, KHSTARTNAME) == 0 || lstrcmpi(path, "KHCore.exe") == 0)
									{
										//己经启动，则退出.
										CloseHandle(hProcess);
										m_pLogger->WriteLog(LM_INFO, TEXT("KH SafeCenter Already running.!"));
										return ;
									}
									CloseHandle(hProcess);
								}
							}

							tstring file = GetAppPath() + TEXT("gameclient.ini");
							TCHAR buf[MAX_PATH] = {0};
							GetPrivateProfileString(TEXT("SystemSet"), TEXT("ServerAddr"), TEXT(""), buf, MAX_PATH, file.c_str());
							tstring sFile = GetRunGameExe() + "|" + sAppPath + I8DESKSafe + "|" + tstring(buf);				
							RunProcess(sFile.c_str(),sFile.c_str());
							m_pLogger->WriteLog(LM_INFO, TEXT("Exec KH SafeCenter succ!"));
						}
					}
				}
				break;
			}
		case Ctrl_EnableIEProt: //启用IE保护
			{
				BindBHO();
				break;
			}
		case Ctrl_EnableDogProt: //启用防狗驱动
			{
				LoadProtDiskDrv();
				break;
			}
		case Ctrl_Chkdsk: //修复磁盘
			{
				string strParam = TEXT("/SetChkdsk");
				char szDriver[MAX_PATH] = {0};
				in.popString(szDriver);
				strParam += TEXT(" ");
				strParam += szDriver;
				ExecCheckdisk(strParam);
				break;
			}
		case Ctrl_PushGameAtOnce: //实时推送游戏
			{
				char szContent[100*MAX_PATH] = {0};
				DWORD dwCmpMode = in.popDWORD();
				DWORD dwSpeed = in.popDWORD();
				in.popString(szContent);
				PushGameAtOnce((FC_COPY_MODE)dwCmpMode, dwSpeed, szContent);
				break;
			}
		default:
			{
				break;
			}
		}
	}

	void CBootRunPlug::ExecCheckdisk(string strParam)
	{
		string strPath = GetAppPath();
		strPath += TEXT("EncipherC.exe");

		ShellExecute(NULL, TEXT("open"), strPath.c_str(), strParam.c_str(), TEXT(""), SW_SHOWNORMAL);
	}

	void CBootRunPlug::CreateGameClassLnk(const char* const pszClass)
	{
		tstring strClass(pszClass);
		string strMenuPath = GetAppPath() + I8DESKGAMEMENU;
		vector<tstring> vctClass;

		size_t nPos = strClass.find('|');
		while(nPos != tstring::npos)
		{
			tstring strContent = strClass.substr(0, nPos);
			vctClass.push_back(strContent);			
			strClass = strClass.substr(nPos + 1);
			nPos = strClass.find('|');
		}		

		//创建图标
		for (size_t i = 0; i < vctClass.size(); ++i)
		{
			char path[MAX_PATH] = {0};
			char szClass[MAX_PATH] = {0};

			lstrcat(szClass, vctClass[i].c_str());
			SHGetSpecialFolderPath(NULL, path, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
			PathAddBackslash(path);

			lstrcat(path, szClass);
			lstrcat(path,".lnk");
			WIN32_FIND_DATA   fdt;
			HANDLE hFile = FindFirstFile(path, &fdt);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				FindClose(hFile);
				continue ;
			}

			wchar_t wpath[MAX_PATH] = {0};
			MultiByteToWideChar(GetACP(), 0, path, -1, wpath, MAX_PATH);

			stringstream sPara;
			sPara << "class:" << szClass;

			//获取图标
			tstring strSvrIcoFile = TEXT("Data\\ClassIcon\\");
			strSvrIcoFile += szClass;
			strSvrIcoFile += TEXT(".ico");
			tstring GameIconFile = GetAppPath() + "data\\ClassIcon\\"+tstring(szClass)+".ico";
			//判断是否需要更新
			BOOL bIsNeedUpdate = FileIsNeedUpdate(1, fprelPath, (char*)strSvrIcoFile.c_str(), (char*)GameIconFile.c_str());
			if (bIsNeedUpdate)
			{
				DWORD bufersize = 0;
				byte* bufer = NULL;
				if (DownloadFile(1, fprelPath, strSvrIcoFile, (char**)&bufer, bufersize) == FALSE)
				{
					m_pLogger->WriteLog(LM_INFO, TEXT("下载游戏类别图标文件%s失败"), (char*)GameIconFile.c_str());
				}
				else
				{
					char DataFolder[MAX_PATH] = {0};
					lstrcpy(DataFolder, GameIconFile.c_str());
					PathRemoveFileSpec(DataFolder);
					PathAddBackslash(DataFolder);
					CreateDirectory(DataFolder,NULL);
					WriteDataFile(GameIconFile,(char*)bufer,false,bufersize);
					delete[]bufer;
				}
			}

			HRESULT hres ;
			IShellLink * psl ;
			IPersistFile* ppf ;

			//创建一个IShellLink实例
			hres = CoCreateInstance( CLSID_ShellLink, NULL,   CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
			if( FAILED( hres))   return  ;

			//设置目标应用程序
			psl -> SetPath(strMenuPath.c_str()) ;

			//设置参数和图标
			psl->SetArguments(sPara.str().c_str());
			psl->SetWorkingDirectory(GetAppPath().c_str());
			if (PathFileExists(GameIconFile.c_str()))
			{
				psl->SetIconLocation(GameIconFile.c_str(),0);
			}			

			hres = psl -> QueryInterface( IID_IPersistFile, (void**)&ppf) ;
			if( FAILED( hres))    return  ;
			hres = ppf -> Save( wpath, STGM_READWRITE) ;

			//释放IPersistFile和IShellLink接口
			ppf -> Release( ) ;
			psl -> Release( ) ;
		}

		//删除图标
		char szDesktopDir[MAX_PATH] = {0}, szDesktopFile[MAX_PATH] = {0};
		SHGetSpecialFolderPath(NULL, szDesktopDir, CSIDL_COMMON_DESKTOPDIRECTORY, TRUE);
		PathAddBackslash(szDesktopDir);

		lstrcpy(szDesktopFile, szDesktopDir);
		lstrcat(szDesktopFile, TEXT("*.lnk"));

		vector<string> vctDelDesktop;

		WIN32_FIND_DATA wfd = {0};
		HANDLE hFinder = FindFirstFile(szDesktopFile, &wfd);
		if (hFinder == INVALID_HANDLE_VALUE)
			return ;
		do 
		{
			OutputDebugString(wfd.cFileName);
			OutputDebugString("\r\n");

			HRESULT       hRes ;
			IShellLink    *psl ;
			IPersistFile  *ppf ;

			//创建一个IShellLink实例
			hRes = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
			if( FAILED( hRes))   return  ;

			hRes = psl->QueryInterface( IID_IPersistFile, (LPVOID *)&ppf); 
			if( FAILED( hRes))   return  ;

			char szFullPath[MAX_PATH] = {0};
			_stprintf(szFullPath, TEXT("%s%s"), szDesktopDir, wfd.cFileName);
			wchar_t wPath[MAX_PATH] = {0};
			MultiByteToWideChar(GetACP(), 0, szFullPath, -1, wPath, MAX_PATH);

			hRes = ppf->Load(wPath, 0);
			if (SUCCEEDED(hRes))
			{
				char szTargetPath[MAX_PATH] = {0};
				char szTargetParam[MAX_PATH] = {0};
				psl->GetPath(szTargetPath, MAX_PATH, NULL, SLGP_UNCPRIORITY);
				psl->GetArguments(szTargetParam, MAX_PATH);

				if (lstrcmpi(strMenuPath.c_str(), szTargetPath) == 0
					&& StrNCmpI(szTargetParam, TEXT("class:"), lstrlen(TEXT("class:"))) == 0)
				{
					tstring strExistClass(szTargetParam + 6);
					BOOL bExist = FALSE;
					for (DWORD dwLoop = 0; dwLoop < vctClass.size(); ++dwLoop)
					{
						tstring strTmpClass = vctClass[dwLoop];
                      if (strExistClass == strTmpClass)
                      {
						   bExist = TRUE;
						   break;
                      }
					}
					if (!bExist)
					{
						vctDelDesktop.push_back(string(szFullPath));
					}
				}
			}
			ppf -> Release( ) ;
			psl -> Release( ) ;
		} while (FindNextFile(hFinder, &wfd));

		for (DWORD dwLoop = 0; dwLoop < vctDelDesktop.size(); ++dwLoop)
		{
			DeleteFile(vctDelDesktop[dwLoop].c_str());
		}
	}

	void CBootRunPlug::GetGameClassLnk()
	{
		map<string, string>::iterator it = m_mapOption.find(OPT_M_CLASSSHUT);
		if (it != m_mapOption.end())
		{
			CreateGameClassLnk(it->second.c_str());
		}
	}

	bool CBootRunPlug::AddPushGameTask( UpGameinfo* pArrayGameInfo, DWORD dwGameCount )
	{
		m_pLogger->WriteLog(LM_INFO, TEXT("添加%ld个推送任务"), dwGameCount);
		IPlug *pPlug = m_pPlugMgr->FindPlug(FORCEUPDATE_PLUG_NAME);
		if (pPlug)
		{
			pPlug->SendMessage(0, (DWORD)pArrayGameInfo, dwGameCount);
			return true;
		}
		m_pLogger->WriteLog(LM_INFO, TEXT("添加推送任务时，推送组件查找失败"));
		return false;
	}

	BOOL CBootRunPlug::EnsureSysOptExist()
	{
        if (!m_mapOption.empty())
        {
			return true;
        }
		//加载离线系统选项
		tstring datafile = GetAppPath()+ TEXT("data\\sysopt.dat");
		HANDLE hSysOptFile = ::CreateFile(datafile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hSysOptFile != INVALID_HANDLE_VALUE)
		{
			//获取文件大小
			DWORD dwSysOptFileSize = GetFileSize(hSysOptFile, NULL);
			if(dwSysOptFileSize == INVALID_FILE_SIZE)
			{
				//文件大小获取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("系统选项文件大小获取失败."));
				CloseHandle(hSysOptFile);
				return FALSE;
			}
			//分配内存
			char *pSysOptMem = new char[dwSysOptFileSize + 1];
			//读入文件
			DWORD dwReadBytes = 0;
			if (!ReadFile(hSysOptFile, pSysOptMem, dwSysOptFileSize, &dwReadBytes, NULL) || dwReadBytes != dwSysOptFileSize || dwSysOptFileSize < 30)
			{
				//文件读取失败
				m_pLogger->WriteLog(LM_INFO, TEXT("系统选项文件读取失败."));
				CloseHandle(hSysOptFile);
				delete[] pSysOptMem;
				pSysOptMem = NULL;
				return FALSE;
			}		
			CloseHandle(hSysOptFile);
			m_mapOption.clear();

			CPackageHelper inpackage(pSysOptMem);
			//弹出状态字段
			inpackage.popDWORD();
			DWORD dwSize   = inpackage.popDWORD();
			char name[4096]  = {0};		
			char value[4096] = {0};				//?表示最大的值的数据大小只能有4K.
			for (DWORD idx=0; idx<dwSize; idx++)
			{
				inpackage.popString(name);
				inpackage.popByte();			//pop = .
				inpackage.popString(value);	
				m_mapOption.insert(make_pair(string(name), string(value)));
			}
			delete[] pSysOptMem;
		}
		else return FALSE;

		return TRUE;
	}

	LPCTSTR CBootRunPlug::GetCookie()
	{
		DWORD dwSize = 0x1000;
		BOOL  bFlag = FALSE;
		LPTSTR pszCookie = NULL;
		while (!bFlag)
		{
			pszCookie = new TCHAR[dwSize];
			ZeroMemory((LPVOID)pszCookie, dwSize*sizeof(TCHAR));
			bFlag = InternetGetCookie(TEXT("http://i8.com.cn"), NULL, pszCookie, &dwSize);
			if (!bFlag)
			{
				delete []pszCookie;
				pszCookie = NULL;

				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					dwSize *= 2;
				else
					break;
			}
		}
		return pszCookie;
	}

	bool CBootRunPlug::ReportInfomation()
	{
		try
		{
			using namespace WinHttp;
			IWinHttpRequestPtr http(__uuidof(WinHttpRequest));

			char szTimeBuffer[MAX_PATH] = {0};
			time_t t; time(&t);
			struct tm* ptm = localtime(&t);
			if (ptm != NULL)
			{
				sprintf(szTimeBuffer, "%d-%d-%d %d:%d:%d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			}

			http->Open(TEXT("POST"), _bstr_t(TEXT("http://desk.cgi.i8cn.com/Cybercafe/Client/ClientReport.koc")), VARIANT_FALSE);
			http->SetRequestHeader(_bstr_t("Connection"), _bstr_t("Keep-Alive"));
			http->SetRequestHeader(_bstr_t("Content-Type"), _bstr_t("application/x-www-form-urlencoded;charset=GBK"));
			TCHAR szBuf[1000] = {0};
			string strNid = GetStringOpt(OPT_U_NID), strSid = GetStringOpt(OPT_U_SID);
			int   nNid = strNid.empty()?-1:atoi(strNid.c_str());
			int   nSid = strSid.empty()?-1:atoi(strSid.c_str());
			_stprintf(szBuf, TEXT("nid=%d&sid=%d&sver=%s&mac=%s&cver=%s&snowdt=%s"), nNid, nSid, GetStringOpt(OPT_U_SVER).c_str(), m_LocMacInfo.szMac, m_LocMacInfo.CliSvrVer, szTimeBuffer);
			
			http->Send(szBuf);
			if (200 != http->Status)_com_raise_error(0);

			string strFlag = http->ResponseText;
			if(lstrcmpi(strFlag.c_str(), TEXT("flag=1")) == 0)
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Report infomation to server success"));
			}
			else
			{
				m_pLogger->WriteLog(LM_INFO, TEXT("Report infomation to server failed"));
			}
		}
		catch (...) 
		{
			m_pLogger->WriteLog(LM_INFO, TEXT("catch exception, Report infomation failed"));
			return false;
		}
		return true;
	}

	std::string CBootRunPlug::GetStringOpt( string strOptName )
	{
		string strValue;
        map<string, string>::iterator it = m_mapOption.find(strOptName);
		if(it != m_mapOption.end()) strValue = it->second;
		return strValue;
	}
}