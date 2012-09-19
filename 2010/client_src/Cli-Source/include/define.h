#ifndef __i8desk_define_inc__
#define __i8desk_define_inc__

#include <string>

//定义所有插件的名字,以及模块名
#define LOGRPT_PLUG_NAME			TEXT("LogRpt")			//登陆上报插件　
#define LOGRPT_MODULE_NAME			TEXT("LogRpt.dll")		
#define GAMEMGR_PLUG_NAME			TEXT("GameMgr")			//服务端游戏,客户端管理插件
#define GAMEMGR_MODULE_NAME			TEXT("GameMgr.dll")		
#define GAMEUPDATE_PLUG_NAME		TEXT("UpdateGame")		//游戏内网多点更新插件
#define GAMEUPDATE_GAME_CLI_NAME    TEXT("UpdateGameCli.dll") 

#define FORCEUPDATE_PLUG_NAME		TEXT("ForceUpdate_Plug")
#define FORCEUPDATE_MODULE_NAME		TEXT("ForceUpdate.dll")
#define BOOTRUN_PLUG_NAME			TEXT("BootRun")			//开机任务插件
#define BOOTRUN_MODULE_NAME			TEXT("BootRun.dll")
#define CLIMGR_PLUG_NAME			TEXT("CliMgr")			//客户端的客户机管理插件
#define CLIMGR_MODULE_NAME			TEXT("CliMgr.dll")
#define GAMEMENU					TEXT("BarOnline.exe")	//游戏菜单

#define RTDATASVR_PLUG_NAME			TEXT("RTDataSvr")			
#define RTDATASVR_MODULE_NAME		TEXT("RTDataSvr.dll")

//客户端类型	//for packageheader.resv[0]
#define OBJECT_UNKOWN				0
#define OBJECT_CLIENTSVI			1
#define OBJECT_CONSOLE				2
#define OBJECT_VDISKSVI				3
#define OBJECT_BARONLINE			4
#define OBJECT_SYNCTOOL				5

#define MAIN_SERVER_SID TEXT("{BDD530E3-E60F-4739-80AF-6E9D845988A4}")
#define VDISK_SERVER_MODE_GROUP TEXT("群组模式")
#define VDISK_SERVER_MODE_09    TEXT("兼容09版模式")

//定义系统选项
//用户登陆验证相关的选项(系统选项客户端插件需要写到LOCAL_MACHINE\SOFTWARE\Goyoo\i8desk下的值)
#define OPT_U_CPUID					TEXT("cpuid")			//硬件id
#define OPT_U_USERNAME				TEXT("username")		//用户名
#define OPT_U_PASSWORD				TEXT("password")		//密码				//不写,密码是原文
#define OPT_U_NBNAME				TEXT("nbname")			//网吧名称
#define OPT_U_SMAC					TEXT("smac")			//服务端MAC地址
#define OPT_U_SVER					TEXT("sver")			//服务端版本号
#define OPT_U_CMAC					TEXT("cmac")			//客户端MAC地址
#define OPT_U_CVER					TEXT("cver")			//客户端版本号
#define OPT_U_NID					TEXT("nid")				//网吧id
#define OPT_U_SID					TEXT("sid")				//服务器id
#define OPT_U_RID					TEXT("rid")				//地区id
#define OPT_U_AID					TEXT("aid")				//商城aid.
#define OPT_U_REGDATE				TEXT("regdate")			//用户注册日期		//不写.格式time32_t
#define OPT_U_DATEEND				TEXT("dateend")			//过期日期			//不写
#define OPT_U_USERTYPE				TEXT("usertype")		//用户类型
#define OPT_U_OEMID					TEXT("bindoemid")		//渠道id
#define OPT_U_OEMNAME				TEXT("oemname")			//渠道名称
#define OPT_U_TASKINFO				TEXT("taskinfo")		//任务信息			//不写
#define OPT_U_PLUGINFO				TEXT("pluginfo")		//插件信息			//不写
#define OPT_U_MESSAGE				TEXT("message")			//中心公告信息		//不写
#define OPT_U_CTLPWD				TEXT("ctrlpassword")	//控制台密码		//不写,密码是原文
#define OPT_U_ERRINFO				TEXT("ErrInfo")			//用于控制台显示登陆的错误信息。
#define OPT_U_REGDATE				TEXT("regdate")			//用户注册日期

#define OPT_S_BACKUPDIR				TEXT("sbackupdir")		//服务端备份目录.
#define OPT_S_BACKUPNUM				TEXT("sbackupnum")		//保留备份文件的个数.
#define OPT_S_BACKUPDATE			TEXT("sbackupdate")		//开始备份的时间.
#define OPT_S_FLUSH_DB_INTERVAL     TEXT("sflushdbinterval")//刷新缓存到数据库的间隔时间
#define OPT_S_FLUSH_DB_OPNUMBER     TEXT("sflushdbopnumber")//刷新缓存到数据库的累积操作条数

//服务端三层选项
#define OPT_D_INITDIR				TEXT("dinitdir")		//三层游戏下载的默认目录
#define OPT_D_TEMPDIR				TEXT("dtempdir")		//三层游戏下载临时目录
#define OPT_D_LINETYPE				TEXT("dlinetype")		//网络线路类型 (0表示电信,1表示网通)
#define OPT_D_TASKNUM				TEXT("dtasknum")		//同时下载的最大任务数 (n)
#define OPT_D_CONNNUM				TEXT("dconnnum")		//最大连接数 (n)
#define OPT_D_SPEED					TEXT("dpeed")			//三层下载的速度限制(n)(k/s)
#define OPT_D_TIMEUPT				TEXT("dtimeupt")		//启用按时段更新(0,1)
#define OPT_D_TIMEUPTVALUE			TEXT("dtimeuptvalue")	//时段更新的值.HH:MM,HH:MM|HH:MM,HH:MM
#define OPT_D_AREADEFRUNTYPE		TEXT("dareadefruntype")	//区域默认运行方式.aid(guid),value(0,1,vid)|aid(guid),value(0,1,vid)

//客户端选项名
#define OPT_M_WINMODE				TEXT("mwinmode")		//菜单窗口模式		(0|1)
#define OPT_M_BROWSEDIR				TEXT("mbrowsedir")		//允许右键浏览目录	(0|1)
#define OPT_M_RUNSHELL				TEXT("mrunshell")		//启动客户端shell程序(0|1)
#define OPT_M_RUNMENU				TEXT("mrunmenu")		//启动客户端菜单程序(0|1)
#define OPT_M_CLISKIN				TEXT("mclientskin")		//客户端皮肤skinname|skinfilename
#define OPT_M_CLIPWD				TEXT("mclipwd")			//客户端高级设置密码(密码的CRC32字符串)
#define OPT_M_MENUACT				TEXT("mmenuact")		//运行游戏后菜单执行的动作(０:表示无动，１表示最小化，２表示关闭)
#define OPT_M_HOTCOUNT				TEXT("mhotcount")		//显示热门游戏的个数n
#define OPT_M_ONLYONE				TEXT("monlyone")		//仅开机读取一次数据0|1
#define OPT_M_AUTOUPT				TEXT("mautoupdate")		//菜单是否自动升级
#define OPT_M_VDDIR					TEXT("mvddir")			//虚拟盘临时路径
#define OPT_M_IEURL					TEXT("mieurl")			//客户端ＩＥ主页
#define OPT_M_SHOWGONGGAO			TEXT("mshowgonggao")	//客户端是否显示公告
#define OPT_M_GGMSG					TEXT("mggmsg")			//客户端公告信息
#define OPT_M_GGFONT				TEXT("mggfont")			//菜单公告字体 (字体名|大小|颜色|标志(bit0:粗体，bit1:斜线,bit2:下画线,bit3:删除线))
#define OPT_M_TSSPEED				TEXT("mtsspeed")		//推送速度
#define OPT_M_SYNTIME				TEXT("msyntime")		//同步时间选项
#define OPT_M_MUSIC					TEXT("mmusic")			//强推的音乐网站 (名字|URL)
#define OPT_M_VIDEO					TEXT("mvideo")			//强推的电影网站 (名字|URL)


#define OPT_M_USERALLOC				TEXT("museralloc")		//允许申请用户(0|1)
#define OPT_M_USERUPFILE			TEXT("muserupfile")		//允许用户上传数据(0|1)
#define OPT_M_DEFAULTSIZE			TEXT("musersize")		//默认用户空间大小(n=50M)单位为M
#define OPT_M_USERDATADIR			TEXT("muserdatadir")	//个人磁盘的自定义保存目录

#define OPT_M_GAMEDRV				TEXT("mgamedrv")		//默认本地游戏存放的盘符.('E')
#define OPT_M_P2P					TEXT("mp2p")			//本地更新是否启用p2p.(1|0)
#define OPT_M_INPROTTYPE			TEXT("minprottype")		//本地更新穿透还原的方式.如果是0表示立即穿透.否则是穿透的速度

#define OPT_M_SAFECENTER			TEXT("msafecenter")		//是否启用安全中心(0|1)
#define OPT_M_WHICHONESC            TEXT("mwhichonesc")     //启用哪一个安全中心
#define OPT_M_IEHISTORY				TEXT("miehistory")		//是否启用IE历史记录(0|1)

#define OPT_M_EXCLUDEDIR			TEXT("mexcludedir")		//磁盘清理时要排除的目录(用|分隔)
#define OPT_M_USEUDISK				TEXT("museudisk")		//是否启用U盘(0|1)

#define OPT_M_PENETATEFILE			TEXT("PenetateFile")	//开机穿透的目录对(用|分隔)

#define OPT_M_CMDMASK				TEXT("gamemgrcmdmask")	//游戏管理插件命令屏蔽选项

#define OPT_M_CLASSSHUT            TEXT("ClassShortcuts")  //目录快捷方式
#define OPT_RunLocalGame				1//游戏本地运行方式
#define OPT_RunRemoteGame				2//游戏远程运行方式；

//类型定义
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

//定义通信命令
#define PRO_VERSION					0x0200					//表示通讯协议版本号packageheader.version.
#define CMD_RET_SUCCESS				1						//命令应答成功
#define CMD_RET_FAIL				0						//命令应答失败

#define I8DESK_INVALID_CMD       0x0000		//无意义的命令
#define CMD_CON_HEART_BEAT       0x0001		//控制台心跳报文

#define CMD_GAME_GETLIST         0x0101		//得到游戏列表
#define CMD_GAME_GETICONLIST	 0x0102		//得到游戏图标压缩包
#define CMD_GAME_REPCLICK	     0x0103		//上报游戏点击数,菜单启动一次
#define CMD_GAME_CLIEXESQL	     0x0104		//客户机执行SQL
#define CMD_GAME_ICONREFRESH     0x0105		//图标缓存失效
#define CMD_GAME_CONGETSTAUTS    0x0106	    //控制台取状态信息
#define CMD_GAME_CONTOCLIENT	 0x0107		//插件转发控制台的数据给客户端
#define CMD_GAME_CLIENTTOCON     0x0108		//客户端发送数据给控制台
#define CMD_GAME_ICON			 0x0109		//图标操作
#define CMD_TASK_OPERATION		 0x010A		//三层操作
#define CMD_TASK_STATE_CHANGED   0x010B		//三层任务状态 
#define CMD_VALIDATE             0x010C		//通知主服务器去立即验证

#define CMD_CON_OFFLINE_NOTIFY   0x010D		//控制台下线通知 
#define CMD_CON_GETINFO_FROM_GAMEUPDATESVR  0x010E	//查看游戏信息
#define CMD_CON_NOTIFY_REFRESH_PL	0x010F	//控制台通知主服务器插件去刷新pl

//日志时间(dword)+日志类型(dword)+日志内容(string)+插件名(string)
#define CMD_PLUGIN_LOG_REPORT		0x0110	//插件报告运行日志

#define CMD_GAME_REQ_ICONS_IDX		0x0121	//请求游戏图标索引
#define CMD_GAME_RSP_ICONS_IDX		0x0122	//返回游戏图标索引
#define CMD_GAME_REQ_ICONS_DATA		0x0123	//请求游戏图标数据
#define CMD_GAME_RSP_ICONS_DATA		0x0124	//返回游戏图标数据
#define CMD_GAME_CONEXESQL	        0x0125	//控制台执行SQL

#define CMD_GAME_GETIDXS	        0x0130	//取游戏的索引列表
#define CMD_GAME_GETSOME	        0x0131	//取指定的游戏列表信息

//每包最多能传送的游戏数目
static const DWORD MAX_COUNT_GAME_PER_PACKEG = 10;

//每包最多能传送的图标数目
static const DWORD MAX_COUNT_ICON_PER_PACKEG = 10;

#define CMD_VDISK_GETLIST		 0x0201		//得到虚拟盘列表
#define CMD_SYSOPT_GETLIST	     0x0301		//得到系统选项
#define CMD_SYSOPT_SYNTIME	     0x0302     //得到服务端的同步时间
#define CMD_FAVORITE_GETLIST	 0x0401		//得到收藏夹列表
#define CMD_BOOT_GETLIST		 0x0501		//得到开机任务列表
#define CMD_BOOT_DELLIST         0x0502     //删除开机任务列表
#define CMD_GET_BARSKINS         0x0503     //请求菜单皮肤列表
#define CMD_REP_BARSKINS		 0x0504     //返回菜单皮肤列表

#define CMD_GET_SERVER_CONFIG	 0x0505     //获取多服务器配置
#define CMD_GET_VDISK_CONFIG	 0x0506     //获取多服务器的虚拟盘配置

#define CMD_CLIRPT_CLIENTINFO	 0x0601		//service启动时上报客户端详细信息 
#define CMD_CLIRPT_HARDWARE      0x0602		//上报客户机硬件信息 
#define CMD_SVRRPT_SERVERINFO    0x0603		//上报服务器信息

#define CMD_USER_REGISTER		 0x0701		//用户注册
#define CMD_USER_LOGIN		     0x0702		//用户登陆
#define CMD_USER_MODPWD		     0x0703		//修改密码

#define CMD_USER_UPLOAD		     0x0704		//数据上传	上传，下载会分块
#define CMD_USER_DOWNLOAD	     0x0705		//数据下载
#define CMD_USER_DELETE		     0x0706		//删除文件
#define CMD_USER_STATE_CHANGE    0x0707

#define CMD_CLIUPT_GETLIST       0x0901		//得到总版本号.
#define CMD_CLIUPT_DOWNFILE	     0x0902		//下载文件，首先下载一个特殊文件
#define CMD_CLIUPT_UPLOAD        0x0903		//上传
#define CMD_CLIUPT_GET_FILELIST	 0x0904		//得到盘符，目录或者文件列表
#define CMD_CLIUPT_DELETE	     0x0905
#define CMD_CLIUPT_UPDATE        0x0906		//通知客户端更新的版本号.
#define CMD_CLIUPT_DOWNFILEINFO  0x0907		//下载文件前先取得文件的CRC
#define CMD_CLIUPT_DOWNFILESIZE  0x0908     //下载文件时先得到文件的大小
#define CMD_CLIUPT_DOWNFILECONTENT 0x0909   //文件具体内容

#define CMD_VDSVR_GETLIST	     0x0a01		//虚拟盘服务器得到虚拟盘列表
#define CMD_VDSVR_REPSTATUS      0x0a02		//虚拟盘报告状态


#define CMD_SYNCTOOL_TASKSTATE   0x0b01		//通知同步工具三层下载状态
#define CMD_SYNCTOOL_ADD_GAME    0x0b02		//同步工具通知服务添加游戏

#define CMD_UPT_ICONLIST_VERSION 0x0c01		//通知服务更新图标列表的版本号
#define CMD_UPT_CLIENT_INDEX     0x0c02		//通知服务更新客户软件包的索引


//任务类型
enum TaskType
{
	tUserTask,
	tForceTask,
	tSmartForceTask		/*人性化强推*/,
	tTimeTask			/*时段任务*/
};

enum TaskState 
{
	tsChecking,
	tsQueue,
	tsDownloading,
	tsSuspend,
	tsComplete,
	tsSelfQueue,
	tsWaitSeed,
	tsHashmap,
	tsCopyfile,
	tsDeleteOldFile,
};

//任务操作
enum TaskOperation
{
	toAdd,
	toDel,
	toGetState,
	toSync,		//通知同步工具同步游戏
	toSuspend,
	toRestart,
};

enum VDStartType 
{
	vdBoot			=	0,			/*开机*/
	vdStartMenu,					/*开菜单*/
	vdStartGame,					/*开游戏*/
};

//图标操作
enum IconOperation {ioAdd,ioDel,ioGetIconFromZip,ioGetIconFromFile};

//文件路径名类型
enum FPType 
{
	fpabsPath,						/*绝对路径*/
	fprelPath,						/*相对路径*/
};

enum FType {fFile,fDir};


//上传包的类型
enum UpLoadPacketType 
{
	uptFirst,
	uptMiddle,
	uptLast,
	upOnlyOne
};

enum TaskNotify 
{
	tnDownloadComplete,
	tnDownloadComFailed,
	tnDownloadCheckFailed,
	tnStopComplete
}; 

enum emRunType
{
	ERT_NONE = 0,	//未设置
	ERT_LOCAL,		//本地更新
	ERT_VDISK,		//虚拟盘
	ERT_DIRECT,		//直接运行（不更新直接从客户机上运行）
};

#define VDISK_I8DESK	0		//表示I8Desk虚拟盘
#define VDISK_OTHER		1		//第三方虚拟盘

#ifndef BitN
#define BitN(x) (1<<x) 
#endif

//由控制台和服务端使用的常量，控制台的SQL中如果修改了表则设置表相应的位
#define FLAG_GAMEAREA_BIT                BitN(0)
#define FLAG_VDAREA_BIT					 BitN(1)
#define FLAG_AREA_BIT					 BitN(2)
#define FLAG_CLASS_BIT					 BitN(3)
#define FLAG_CLIENT_BIT					 BitN(4)
#define FLAG_CMPSTARTTASK_BIT			 BitN(5)
#define FLAG_FAVORITE_BIT				 BitN(6)
#define FLAG_GAME_BIT					 BitN(7)
#define FLAG_GAMELOG_BIT				 BitN(8)
#define FLAG_SYSOPT_BIT					 BitN(9)
#define FLAG_USER_BIT					 BitN(10)
#define FLAG_VDISK_BIT					 BitN(11)
#define FLAG_SYNCTASK_BIT				 BitN(12)
#define FLAG_SYNCGAME_BIT				 BitN(14)
#define FLAG_ICON_BIT					 BitN(15)
#define FLAG_MODULEUSAGE_BIT			 BitN(16)

#define SQL_OP_NONE						 0x00
#define SQL_OP_SELECT					 0x01
#define SQL_OP_INSERT					 0x02
#define SQL_OP_UPDATE					 0x04
#define SQL_OP_DELETE					 0x08
#define SQL_OP_ENSURE					 0x10
#define SQL_OP_COMULATE					 0x20

enum BootRunTask 
{  
	brDeskTop	= 0,	/*换桌面*/
	brShortcut,			/*桌面快捷方式*/
	brIEHomePage,		/*IE主页控制*/
	brCltFile,			/*执行客户端任一自定义的EXE*/
	brSvrFile,			/*处理服务端下放的任意文件*/
	brDelFile,			/*删除文件或文件夹或某一游戏*/
	brDelGames,			/*删除点击率低的游戏*/
	brSalfCenterXMl,	/*处理安全中心xml*/
	brUpdateGame		/*更新游戏*/
};


#define DEFAULT_AREA_GUID	TEXT("{2B35D26B-6E5C-4d18-B108-19F778B6AF18}")
#define DEFAULT_AREA_NAME	TEXT("默认分区")

#define CLASS_WL_GUID	TEXT("{E88DD848-DA99-46b0-97D6-23D0658B718A}")	//网络游戏
#define CLASS_DJ_GUID	TEXT("{30CF0706-E9AE-4804-9D57-B39885B672E4}")	//单机游戏
#define CLASS_XX_GUID	TEXT("{C185F5B5-FA0C-466e-BDDF-6E7EDA5A3F3E}")	//休闲游戏
#define CLASS_DZ_GUID	TEXT("{FD844C21-BD62-4369-B146-DC06A5D4CE1E}")	//对战游戏
#define CLASS_QP_GUID	TEXT("{E7CDB5F3-D6C7-4607-8B71-3FC087E4B896}")	//棋牌游戏
#define CLASS_PL_GUID	TEXT("{C8DDDA32-EFD6-43af-B94B-FBADF3196977}")	//游戏插件
#define CLASS_SP_GUID	TEXT("{D5443BC8-7839-4ce3-A56A-1018CE19800D}")	//游戏视频
#define CLASS_LT_GUID	TEXT("{CF59BC68-973B-461a-97AF-29E183EB3645}")	//聊天工具
#define CLASS_CY_GUID	TEXT("{58D87CC9-5BB1-43e0-A919-C7ED3D7CE46A}")	//常用工具
#define CLASS_YY_GUID	TEXT("{E1DDCC22-9AAF-4020-A65B-F022DBFABC47}")	//影音工具
#define CLASS_GP_GUID	TEXT("{B59A5F1F-9431-461f-A5BD-C88949E29709}")	//股票证券

#define DEFAULT_CLASS_GUID	TEXT("{5D00AA45-3155-4a71-BE0B-88804931BD6D}")

#define CLASS_WL_NAME	TEXT("网络游戏")
#define CLASS_DJ_NAME	TEXT("单机游戏")
#define CLASS_XX_NAME	TEXT("休闲游戏")
#define CLASS_DZ_NAME	TEXT("对战游戏")
#define CLASS_QP_NAME	TEXT("棋牌游戏")
#define CLASS_PL_NAME	TEXT("游戏插件")
#define CLASS_SP_NAME	TEXT("游戏视频")
#define CLASS_LT_NAME	TEXT("聊天工具")
#define CLASS_CY_NAME	TEXT("常用工具")
#define CLASS_YY_NAME	TEXT("影音工具")
#define CLASS_GP_NAME	TEXT("股票证券")

#define DEFAULT_CLASS_NAME	TEXT("默认类别")

//_packageheader+crc(DWORD)+cmd(DWORD) + info.
typedef enum E_CliCtrlCmd
{
	Ctrl_InstallProt	= 0,	//安装还原，info:后面有个dword表示是否保护所有分区.为１表示保护，为０表示不保护
	Ctrl_RemoveProt		= 1,	//删除还原，info:无
	Ctrl_ProtArea		= 2,	//保护还原分区, info:有两个String,第一个表示保护的分区列表，第二个表示取消保护的分区列表
	Ctrl_ShutDown		= 3,	//开机，重启，info:用1个dword表示：为1表示重启，为0表示关机
	Ctrl_ExecFile		= 4,	//执行指定文件，info:后面有一个String表示需要运行的文件.
	Ctrl_ViewSysInfo	= 5,	//获取系统信息，info:无
	Ctrl_ViewProcInfo	= 6,	//获取进程信息，info:无
	Ctrl_ViewSviInfo	= 7,	//获取服务信息，info:无
	Ctrl_KillProcSvr	= 8,	//kill进程或者服务，info:一个DWORD(0表示是服务，1表示是进程),一个String表示的进程或者服务名
	Ctrl_Remote         = 9,    //客户端远程控制启动，关闭，info:用个dword表示：为１表示启动，为０表示关闭 
	Ctrl_Chkdsk         = 10,	//客户端修复磁盘
	Ctrl_PushGameAtOnce  = 11,   //实时推送游戏
	Ctrl_EnableSC       = 100,  //启用安全中心
	Ctrl_EnableIEProt   = 101,  //启用IE保护
	Ctrl_EnableDogProt  = 102,  //启用防狗驱动
}E_CliCtrlCmd;

#define VDISK_EVENT_NAME	TEXT("__i8desk_vdisk_refresh_event_")


namespace i8desk {

#define MAX_LOG_LEN (1024 * 8)
#define MAX_PLUGIN_NAME_LEN MAX_PATH

	struct PluginLogRecord {
		int time;
		int type;
		char data[ MAX_LOG_LEN ];
		char plugin[ MAX_PLUGIN_NAME_LEN ];
	};
}

static const TCHAR *I8DESK_SPECIAL_DIR = _T("i8desk");
static const size_t I8DESK_SPECIAL_DIR_LEN = ::lstrlen(I8DESK_SPECIAL_DIR);

#define VDISK_PORT_BASE 17918








#include "i8type.h"

#endif