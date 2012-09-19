#ifndef I8TYPE_H
#define I8TYPE_H

#include <string>
#include <memory>
#include <winsock2.h>


namespace i8desk {
	typedef short int16;
	typedef unsigned short uint16;
	typedef unsigned short ushort;
	typedef int int32;
	typedef unsigned int uint32;
	typedef unsigned int uint;
	typedef unsigned long ulong;
	typedef long long int64;
	typedef long long longlong;
	typedef unsigned long long uint64;
	typedef unsigned long long ulonglong;
//-----------------------------------------

const UINT MAX_LOG_LEN = 1024 * 8;
const UINT MAX_PLUGIN_NAME_LEN = MAX_PATH;
}

//////////////////////////////////////////////////
//for database
namespace i8desk {
namespace db {
	
#pragma pack(push, 8)

static const long MAX_AREA_NUM  = 32;
static const long MAX_GAME_GID = 1024*64;

#define MAX_NAME_LEN		64
#define MAX_GUID_LEN		64
#define MAX_PARAM_LEN		280
#define MAX_REMARK_LEN		280
#define MAX_SYSOPT_LEN		1024
#define MAX_IPSTRING_LEN	16
#define MAX_MACSTRING_LEN	32
#define MAX_VERSTRING_LEN	32
#define MAX_DISKPARTION_LEN	64
#define MAX_TEXT_LEN		255

struct tClass {
	TCHAR	CID[ MAX_GUID_LEN ];
	TCHAR	Name[ MAX_NAME_LEN ];
	TCHAR	SvrPath[ MAX_PATH ];

	tClass(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tClass> TClassPtr;


#define MASK_TCLASS_DEFCLASS		(1LL << 1L)
#define MASK_TCLASS_NAME			(1LL << 2L)
#define MASK_TCLASS_SVRPATH			(1LL << 3L)

struct tGame {
	uint32	GID;								//游戏ID
	uint32	PID;								//关联游戏ID
	TCHAR	CID[ MAX_GUID_LEN ];				//类别ID
	TCHAR	Name[ MAX_NAME_LEN ];				//游戏名称
	TCHAR	Exe[ MAX_PATH ];					//游戏的执行文件名称(GameExe)
	TCHAR	Param[ MAX_PARAM_LEN ];				//游戏运行参数
	ulong	Size;								//游戏文件大小(KB为单位)	
	TCHAR	SvrPath[ MAX_PATH ];				//游戏服务端路径
	TCHAR	CliPath[ MAX_PATH ];				//游戏客服端路径
	TCHAR	TraitFile[ MAX_PATH ];				//游戏特征文件
	TCHAR	SaveFilter[ MAX_REMARK_LEN ];		//游戏存档文件(以|分隔，并整体以base64编码)
	int		DeskLink;							//0|1桌面快捷方式
	int		Toolbar;							//0|1菜单工具栏显示
	TCHAR	Memo[ MAX_REMARK_LEN ];				//备注(commemt)
	TCHAR	GameSource[ MAX_REMARK_LEN ];		//游戏来源
	ulong	IdcAddDate;							//中心游戏添加时间
	int		EnDel;								//0|1空间不足时，允许删除
	ulong	IdcVer;								//中心版本号
	ulong	SvrVer;								//本地服务端版本号
	ulong	IdcClick;							//热门度
	ulong	SvrClick;							//本地游戏点击次数
	ulong	SvrClick2;							//上报游戏点击次数
	int		I8Play;								//增值游戏
	int		AutoUpt;							//手动,自动更新
	int		Priority;							//优先级
	int		Force;								//强推(Auto)
	TCHAR	ForceDir[ MAX_PATH ];				//强推目录
	int		Status;								//是否本地游戏
	int		StopRun;							//是否中心停运游戏
	int		Repair;								//是否强制更新游戏
	int		Hide;								//是否隐藏游戏

	tGame(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tGame> TGamePtr;


#define MASK_TGAME_GID				(1LL << 1L)
#define MASK_TGAME_PID				(1LL << 2L)
#define MASK_TGAME_CID				(1LL << 3L)
#define MASK_TGAME_NAME				(1LL << 4L)
#define MASK_TGAME_EXE				(1LL << 5L)
#define MASK_TGAME_PARAM			(1LL << 6L)
#define MASK_TGAME_SIZE				(1LL << 7L)	
#define MASK_TGAME_SVRPATH			(1LL << 8L)	
#define MASK_TGAME_CLIPATH			(1LL << 9L)
#define MASK_TGAME_TRAITFILE		(1LL << 10L)
#define MASK_TGAME_SAVEFILTER		(1LL << 11L)
#define MASK_TGAME_DESKLINK			(1LL << 12L)
#define MASK_TGAME_TOOLBAR			(1LL << 13L)
#define MASK_TGAME_MEMO				(1LL << 14L)
#define MASK_TGAME_GAMESOURCE		(1LL << 15L)
#define MASK_TGAME_IDCADDDATE		(1LL << 16L)
#define MASK_TGAME_ENDEL			(1LL << 17L)
#define MASK_TGAME_IDCVER			(1LL << 18L)
#define MASK_TGAME_SVRVER			(1LL << 19L)
#define MASK_TGAME_IDCCLICK			(1LL << 20L)
#define MASK_TGAME_SVRCLICK			(1LL << 21L)
#define MASK_TGAME_SVRCLICK2		(1LL << 22L)
#define MASK_TGAME_I8PLAY			(1LL << 23L)
#define MASK_TGAME_AUTOUPT			(1LL << 24L)
#define MASK_TGAME_PRIORITY			(1LL << 25L)
#define MASK_TGAME_FORCE			(1LL << 26L)
#define MASK_TGAME_FORCEDIR			(1LL << 27L)
#define MASK_TGAME_STATUS			(1LL << 28L)
#define MASK_TGAME_STOPRUN			(1LL << 29L)
#define MASK_TGAME_REPAIR			(1LL << 30L)
#define MASK_TGAME_HIDE				(1LL << 31L)


struct tArea {
	TCHAR	AID[MAX_GUID_LEN];		//区域ID
	TCHAR	Name[MAX_NAME_LEN];		//区域名称
	TCHAR   SvrID[MAX_GUID_LEN];	//服务器ID 

	tArea(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tArea>	TAreaPtr;

#define MASK_TAREA_AID				(1LL << 1L)
#define MASK_TAREA_NAME				(1LL << 2L)
#define MASK_TAREA_SVRID			(1LL << 3L)


struct tRunType {
	TCHAR	AID[MAX_GUID_LEN];	//区域ID
	uint32	GID;				//游戏ID
	int		Type;				//运行方式
	TCHAR	VID[MAX_GUID_LEN];	//虚拟盘ID

	tRunType(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tRunType>	TRunTypePtr;


#define MASK_TRUNTYPE_GID			(1LL << 1L)
#define MASK_TRUNTYPE_AID			(1LL << 2L)
#define MASK_TRUNTYPE_TYPE			(1LL << 3L)
#define MASK_TRUNTYPE_VID			(1LL << 4L)


struct tClient 
{
	TCHAR Name[MAX_NAME_LEN];				//客服机机名
	TCHAR AID[MAX_GUID_LEN];				//客服机所在区域
	ulong IP;								//客服机IP
	TCHAR MAC[MAX_MACSTRING_LEN];			//客服机MAC地址
	ulong Mark;								//子网掩码，网络序
	ulong Gate;								//网关
	ulong DNS;								//域名服务器1
	ulong DNS2;								//域名服务器2
	int  ProtInstall;						//是否安装保护还原
	TCHAR Partition[MAX_DISKPARTION_LEN];	//保护分区	
	TCHAR ProtVer[MAX_VERSTRING_LEN];		//还原版本号
	TCHAR VDiskVer[MAX_VERSTRING_LEN];		//虚拟机版本号
	TCHAR MenuVer[MAX_VERSTRING_LEN];		//菜单版本号
	TCHAR CliSvrVer[MAX_VERSTRING_LEN];		//客服端服务版本号
	TCHAR System[ MAX_REMARK_LEN ];			//操作系统信息
	TCHAR CPU[ MAX_REMARK_LEN ];			//CPU信息
	TCHAR Mainboard[ MAX_REMARK_LEN ];		//主板信息
	TCHAR Memory[ MAX_REMARK_LEN ];			//内存信息
	TCHAR Disk[ MAX_REMARK_LEN ];			//磁盘信息
	TCHAR Video[ MAX_REMARK_LEN ];			//显卡信息
	TCHAR Audio[ MAX_REMARK_LEN ];			//声卡信息
	TCHAR Network[ MAX_REMARK_LEN ];		//网卡信息
	TCHAR Camera[ MAX_REMARK_LEN ];			//摄像头信息
	int		Online;							// 1:ONLINE,0:OFFLINE
	SOCKET	Socket;
	int   Temperature;						// 温度

	tClient(){ memset(this,0,sizeof(*this)); }
};

typedef std::tr1::shared_ptr<tClient>	TClientPtr;


#define MASK_TCLIENT_NAME			(1LL << 1L)
#define MASK_TCLIENT_AID			(1LL << 2L)	
#define MASK_TCLIENT_IP				(1LL << 3L)
#define MASK_TCLIENT_MAC			(1LL << 4L)	
#define MASK_TCLIENT_MARK			(1LL << 5L)
#define MASK_TCLIENT_GATE			(1LL << 6L)
#define MASK_TCLIENT_DNS			(1LL << 7L)
#define MASK_TCLIENT_DNS2			(1LL << 8L)
#define MASK_TCLIENT_PROTINSTALL	(1LL << 9L)
#define MASK_TCLIENT_PARTITION		(1LL << 10L)	
#define MASK_TCLIENT_PROTVER		(1LL << 11L)
#define MASK_TCLIENT_VDISKVER		(1LL << 12L)
#define MASK_TCLIENT_MENUVER		(1LL << 13L)
#define MASK_TCLIENT_CLISVRVER		(1LL << 14L)
#define MASK_TCLIENT_SYSTEM			(1LL << 15L)
#define MASK_TCLIENT_CPU			(1LL << 16L)
#define MASK_TCLIENT_MAINBOARD		(1LL << 17L)
#define MASK_TCLIENT_MEMORY			(1LL << 18L)
#define MASK_TCLIENT_DISK			(1LL << 19L)
#define MASK_TCLIENT_VIDEO			(1LL << 20L)
#define MASK_TCLIENT_AUDIO			(1LL << 21L)
#define MASK_TCLIENT_NETWORK		(1LL << 22L)	
#define MASK_TCLIENT_CAMERA			(1LL << 23L)
#define MASK_TCLIENT_ONLINE			(1LL << 24L)
#define MASK_TCLIENT_SOCKET			(1LL << 25L)
#define MASK_TCLIENT_TEMPERATURE	(1LL << 26L)



struct tVDisk {
	TCHAR	VID[MAX_GUID_LEN];		//虚拟机ID
	TCHAR	SvrID[MAX_GUID_LEN];	//服务器ID
	ulong	SoucIP;					//源IP
	int		Port;					//虚拟机端口
	int		SvrDrv;					//盘符大写字母的ASCII码
	int		CliDrv;					//盘符大写字母的ASCII码
	int		Type;					//0:I8Desk虚拟盘，1:第三方虚拟盘
	int		LoadType;				//0:开机刷盘，1：运行菜单刷盘，2：运行游戏刷盘
	TCHAR   SvrMode[MAX_NAME_LEN];	//服务模式
	ulong   Size;					// 缓存空间大小
	int     SsdDrv;					// SSD盘符大写字母的ASCII码

	tVDisk(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tVDisk>	TVDiskPtr;

#define MASK_TVDISK_VID				(1LL << 1L)
#define MASK_TVDISK_SVRID			(1LL << 2L)
#define MASK_TVDISK_SOUCIP			(1LL << 3L)
#define MASK_TVDISK_PORT			(1LL << 4L)
#define MASK_TVDISK_SVRDRV			(1LL << 5L)
#define MASK_TVDISK_CLIDRV			(1LL << 6L)
#define MASK_TVDISK_TYPE			(1LL << 7L)	
#define MASK_TVDISK_LOADTYPE		(1LL << 8L)
#define MASK_TVDISK_SVRMODE			(1LL << 9L)
#define MASK_TVDISK_SIZE			(1LL << 10L)
#define MASK_TVDISK_SSDDRV			(1LL << 11L)



struct tVDiskStatus {
	ulong		VDiskIP;
	TCHAR		VID[MAX_GUID_LEN];
	ulong		Version;
	ulong		ClientCount;
	TCHAR       HostName[MAX_NAME_LEN];

	tVDiskStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tVDiskStatus>	TVDiskStatusPtr;


#define MASK_TVDISKSTATUS_VDISKIP			(1LL << 1L)
#define MASK_TVDISKSTATUS_VID				(1LL << 2L)
#define MASK_TVDISKSTATUS_VERSION			(1LL << 3L)
#define MASK_TVDISKSTATUS_CLIENTCOUNT		(1LL << 4L)
#define MASK_TVDISKSTATUS_HOSTNAME			(1LL << 5L)

struct tVDiskClient {
	ulong		ClientIP;					//　客户机IP
	TCHAR		VID[MAX_GUID_LEN];			//　虚拟盘ID
	ulong		VDiskIP;					//　虚拟机IP
	uint64		ReadCount;					//　读取数据量				
	ulong		ReadSpeed;					//　读取速度
	ulong		ReadMax;					//　读取峰值
	ulong		ConnectTime;				//　连接时长
	ulong		CacheShooting;				//　缓存命中率
	ulong		OnlineTime;					//　在线时长
	tVDiskClient(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tVDiskClient>	TVDiskClientPtr;


#define MASK_TVDISKCLIENT_CLIENTIP				(1LL << 1L)
#define MASK_TVDISKCLIENT_VID					(1LL << 2L)
#define MASK_TVDISKCLIENT_SVRNAME				(1LL << 3L)
#define MASK_TVDISKCLIENT_VDISKIP				(1LL << 4L)
#define MASK_TVDISKCLIENT_CLIENTNAME			(1LL << 5L)
#define MASK_TVDISKCLIENT_READCOUNT				(1LL << 6L)
#define MASK_TVDISKCLIENT_READSPEED				(1LL << 7L)
#define MASK_TVDISKCLIENT_READMAX				(1LL << 8L)
#define MASK_TVDISKCLIENT_CONNECTTIME			(1LL << 9L)
#define MASK_TVDISKCLIENT_CACHESHOOTING			(1LL << 10L)
#define MASK_TVDISKCLIENT_ONLINETIME			(1LL << 11L)

//SELECT a.AID as aid, a.Name as name, r.GID, r.RunType, v.VID, v.CliDrv
//FROM rGameArea r, tArea a, tVDisk v
//WHERE a.AID=r.AID AND v.VID=r.VID AND r.GID=%u
//2011版注释
//struct vRunType {
//	TCHAR	AID[MAX_GUID_LEN];		//区域ID
//	TCHAR	AName[MAX_NAME_LEN];	//区域名
//	int		GID;					//游戏ID
//	int		RunType;				//运行方式
//	TCHAR	VID[MAX_GUID_LEN];		//虚拟盘ID
//	TCHAR	VCliDrv;				//虚拟盘客户端盘符
//};

//菜单皮肤
struct tBarSkin {
	TCHAR	Style[ MAX_NAME_LEN ];	//风格名
	TCHAR	PathName[ MAX_PATH ];	//文件名
	TCHAR	Version[ MAX_GUID_LEN ];//版本号

	tBarSkin(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tBarSkin>	TBarSkinPtr;

#define MASK_TBARSKIN_STYLE				(1LL << 1L)
#define MASK_TBARSKIN_PATHNAME			(1LL << 2L)
#define MASK_TBARSKIN_VERSION			(1LL << 3L)

//控制台通过IP向UpDateGame查询客户机有哪些游戏。命令：CMD_UPT_SM_IPHAVEGAME = 0x03
struct tViewGame
{
	TCHAR			GameName[MAX_NAME_LEN];	
	unsigned long   UpdateTime;	//version			
	long			GID;					

	tViewGame(){ ::memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tViewGame>	TViewGamePtr;

#define MASK_TVIEWGAME_GAMENAME				(1LL << 1L)
#define MASK_TVIEWGAME_UPDATETIME			(1LL << 2L)
#define MASK_TVIEWGAME_GID					(1LL << 3L)

//控制台通过GID向UpDateGame查询游戏存在于哪些客户机上。命令：CMD_UPT_SM_GAMEINIP = 0x04
struct tViewClient
{
	TCHAR			ClientName[MAX_NAME_LEN];	
	unsigned long   UpdateTime;	//version	
	unsigned long   Ip;

	tViewClient(){ ::memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tViewClient>	TViewClientPtr;

#define MASK_TVIEWCLIENT_CLIENTNAME			(1LL << 1L)
#define MASK_TVIEWCLIENT_UPDATETIME			(1LL << 2L)
#define MASK_TVIEWCLIENT_IP					(1LL << 3L)


struct tFavorite {
	TCHAR FID[MAX_GUID_LEN];		//收藏夹ID(GUID)
	int	  Type;						//0:网站，1:电影，2:音乐
	TCHAR Name[MAX_NAME_LEN];
	TCHAR URL[MAX_PATH];
	uint32 Serial;					//顺序，用于排序

	tFavorite(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tFavorite>	TFavoritePtr;

#define MASK_TFAVORITE_FID				(1LL << 1L)
#define MASK_TFAVORITE_TYPE				(1LL << 2L)
#define MASK_TFAVORITE_NAME				(1LL << 3L)
#define MASK_TFAVORITE_URL				(1LL << 4L)
#define MASK_TFAVORITE_SERIAL			(1LL << 5L)

struct tCmpBootTask {
	TCHAR TID[MAX_GUID_LEN];
	TCHAR Name[MAX_NAME_LEN];		// 任务名称
	int Type;						// brPushGame :游戏推送， 非brPushGame :开机任务
	int Flag;						// 是否还原保护
	int RunDate;					// 运行日期（星期表示 星期一：1LL<<1L； 星期二：1LL<<2L ... 星期七：1LL<<7L）他是可以组合的（例如：3 = (星期一和星期二)）
	ulong StartTime;				// 起始时间段 （HH:MM:SS）
	ulong DelayTime;				// 延迟时间 （SS）
	ulong EndTime;					// 结束时间 （HH:MM:SS）
	ulong EndDate;					// 过期时间 （YY:MM:DD:HH:MM:SS）
	int RunType;					// 运行类型 （开机运行：0；启动菜单运行：1）
	int Status;						// 启用状态 （未启用：0； 启用：1）
	TCHAR Parameter[2048];			// 参数

	tCmpBootTask(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tCmpBootTask>	TCmpBootTaskPtr;


#define MASK_TCMPBOOTTASK_TID			(1LL << 1L)
#define MASK_TCMPBOOTTASK_NAME			(1LL << 2L)
#define MASK_TCMPBOOTTASK_TYPE			(1LL << 3L)
#define MASK_TCMPBOOTTASK_FLAG			(1LL << 4L)
#define MASK_TCMPBOOTTASK_RUNDATE		(1LL << 5L)
#define MASK_TCMPBOOTTASK_STARTTIME		(1LL << 6L)
#define MASK_TCMPBOOTTASK_DELAYTIME		(1LL << 7L)
#define MASK_TCMPBOOTTASK_ENDTIME		(1LL << 8L)
#define MASK_TCMPBOOTTASK_ENDDATE		(1LL << 9L)
#define MASK_TCMPBOOTTASK_RUNTYPE		(1LL << 10L)
#define MASK_TCMPBOOTTASK_STATUS		(1LL << 11L)
#define MASK_TCMPBOOTTASK_PARAMETER		(1LL << 12L)


struct tSysOpt {
	TCHAR key[MAX_NAME_LEN];
	TCHAR value[MAX_SYSOPT_LEN];

	tSysOpt(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tSysOpt>	TSysOptPtr;

#define MASK_TSYSOPT_KEY			(1LL << 1L)
#define MASK_TSYSOPT_VALUE			(1LL << 2L)


#define MAX_ICON_DATA_SIZE (11*1024)

struct tIcon {
	uint32 gid;
	ulong size;
	unsigned char data[MAX_ICON_DATA_SIZE];

	tIcon(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tIcon>	TIconPtr;


#define MASK_TICON_GID					(1LL << 1L)
#define MASK_TICON_DATA					(1LL << 2L)
#define MASK_TICON_SIZE					(1LL << 3L)

struct tSyncTask {
	TCHAR	SID[ MAX_GUID_LEN ];			//任务ID
	TCHAR	SvrID[ MAX_GUID_LEN ];			//服务器ID
	TCHAR	Name[ MAX_NAME_LEN ];			//任务名称
	TCHAR	SvrDir[MAX_PATH];				//服务器的目录
	TCHAR	NodeDir[MAX_PATH];				//节点的目录
	ulong	SoucIp;							//源IP
	ulong	DestIp;							//目标IP
	int		Speed;							//最高同步速度
	int		SyncType;						//同步任务类型
	int		Status;							//同步任务状态
	TCHAR	NameID[ MAX_GUID_LEN ];			//任务名称ID是个时间值

	tSyncTask(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tSyncTask>	TSyncTaskPtr;

#define MASK_TSYNCTASK_SID			(1LL << 1L)
#define MASK_TSYNCTASK_SVRID		(1LL << 2L)
#define MASK_TSYNCTASK_NAME			(1LL << 3L)
#define MASK_TSYNCTASK_SVRDIR		(1LL << 4L)
#define MASK_TSYNCTASK_NODEDIR		(1LL << 5L)
#define MASK_TSYNCTASK_SOUCIP		(1LL << 6L)
#define MASK_TSYNCTASK_DESTIP		(1LL << 7L)
#define MASK_TSYNCTASK_SPEED		(1LL << 8L)
#define MASK_TSYNCTASK_SYNCTYPE		(1LL << 9L)
#define MASK_TSYNCTASK_STATUS		(1LL << 10L)
#define MASK_TSYNCTASK_NAMEID		(1LL << 11L)


struct tSyncGame {
	TCHAR	SID[ MAX_GUID_LEN ];			//任务ID
	uint32	GID;							//GID

	tSyncGame(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tSyncGame>	TSyncGamePtr;

#define MASK_TSYNCGAME_SID			(1LL << 1L)
#define MASK_TSYNCGAME_GID			(1LL << 2L)


struct tServer {
	TCHAR	SvrID[ MAX_GUID_LEN ];			//服务器ID
	TCHAR	SvrName[ MAX_NAME_LEN ];		//服务器名称
	int		SvrType;						//服务器类型
	ulong	SvrIP;							//本服务器IP
	ulong	Ip1;							//IP1
	ulong	Ip2;							//IP2
	ulong	Ip3;							//IP3
	ulong	Ip4;							//IP4
	int		Speed;							//最高同步速度
	int		BalanceType;					//负载平衡类型

	tServer(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tServer>	TServerPtr;

#define MASK_TSERVER_SVRID			(1LL << 1L)
#define MASK_TSERVER_SVRNAME		(1LL << 2L)
#define MASK_TSERVER_SVRTYPE		(1LL << 3L)
#define MASK_TSERVER_SVRIP			(1LL << 4L)
#define MASK_TSERVER_IP1			(1LL << 5L)
#define MASK_TSERVER_IP2			(1LL << 6L)
#define MASK_TSERVER_IP3			(1LL << 7L)
#define MASK_TSERVER_IP4			(1LL << 8L)
#define MASK_TSERVER_SPEED			(1LL << 9L)
#define MASK_TSERVER_BALANCETYPE	(1LL << 10L)

struct tBootTaskArea {
	TCHAR	TID[ MAX_GUID_LEN ];		//任务ID
	TCHAR	AID[ MAX_GUID_LEN ];		//区域ID

	tBootTaskArea(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tBootTaskArea>	TBootTaskAreaPtr;


#define MASK_TBOOTTASKAREA_TID			(1LL << 1L)
#define MASK_TBOOTTASKAREA_AID			(1LL << 2L)

struct tPushGameStatic {
	uint32	GID;			//游戏GID
	ulong	Size;			//游戏大小
	ulong	BeginDate;		//开始推送游戏时间
	ulong	EndDate;		//结束推送游戏时间

	tPushGameStatic(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tPushGameStatic>	TPushGameStaticPtr;


#define MASK_TPUSHGAMESTATIC_GID			(1LL << 1L)
#define MASK_TPUSHGAMESTATIC_SIZE			(1LL << 2L)
#define MASK_TPUSHGAMESTATIC_BEGINDATE		(1LL << 3L)
#define MASK_TPUSHGAMESTATIC_ENDDATE		(1LL << 4L)

struct tModuleUsage {
	TCHAR	Name[ MAX_NAME_LEN ];			//客户机名称任务ID
	TCHAR	MAC[ MAX_MACSTRING_LEN ];		//客户机MAC地址
	uint32	MID;							//模块编号
	ulong	ClickCount;						//点击数
	ulong	UsedTime;						//停留时间

	tModuleUsage(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tModuleUsage>	TModuleUsagePtr;


#define MASK_TMODULEUSAGE_NAME			(1LL << 1L)
#define MASK_TMODULEUSAGE_MAC			(1LL << 2L)
#define MASK_TMODULEUSAGE_MID			(1LL << 3L)
#define MASK_TMODULEUSAGE_CLICKCOUNT	(1LL << 4L)
#define MASK_TMODULEUSAGE_USEDTIME		(1LL << 5L)

struct tModule2Usage {
	TCHAR	MAC[ MAX_MACSTRING_LEN ];		//客户机MAC地址
	uint32	MID;							//模块编号
	ulong	ClickCount;						//点击数
	ulong	UsedTime;						//停留时间
	TCHAR	Name[MAX_NAME_LEN];				// 二级名称
	tModule2Usage(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tModule2Usage>	TModule2UsagePtr;

#define MASK_TMODULE2USAGE_MAC			(1LL << 1L)
#define MASK_TMODULE2USAGE_MID			(1LL << 2L)
#define MASK_TMODULE2USAGE_CLICKCOUNT	(1LL << 3L)
#define MASK_TMODULE2USAGE_USEDTIME		(1LL << 4L)
#define MASK_TMODULE2USAGE_NAME			(1LL << 5L)

#define MAX_DEVICEINFO_LEN      4096

//服务状态监控
struct tServerStatus {
	TCHAR SvrID[ MAX_GUID_LEN ];			//服务器ID
	TCHAR SvrName[ MAX_NAME_LEN ];			//服务器名称
	int I8DeskSvr;							//主服务状态
	int I8VDiskSvr;							//虚拟盘服务状态
	int I8UpdateSvr;						//内网更新服务状态
	int DNAService;							//三层下载服务状态
	int I8MallCashier;						//商城收银端状态
	ulong LastBackupDBTime;					//上次备份数据库时间
	size_t CPUUtilization;					//CPU利用率
	size_t MemoryUsage;						//内存使用率
	size_t DeadCounter;						//僵死计数
	ulonglong RemainMemory;					//剩余内存,kB
	uint32 NetworkSendRate;					//网络发送平均速度,kB/s
	uint32 NetworkRecvRate;					//网络接收平均速度,kB/s
	TCHAR DeviceInfo[ MAX_DEVICEINFO_LEN ];  //设备信息
	int SvrType;							//1:主服务；0：从服务
	tServerStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tServerStatus>	TServerStatusPtr;

#define MASK_TSERVERSTATUS_SVRID			(1LL << 1L)
#define MASK_TSERVERSTATUS_SVRNAME			(1LL << 2L)
#define MASK_TSERVERSTATUS_I8DESKSVR		(1LL << 3L)
#define MASK_TSERVERSTATUS_I8VDISKSVR		(1LL << 4L)
#define MASK_TSERVERSTATUS_I8UPDATESVR		(1LL << 5L)
#define MASK_TSERVERSTATUS_DNASERVICE		(1LL << 6L)
#define MASK_TSERVERSTATUS_I8MALLCASHIER	(1LL << 7L)
#define MASK_TSERVERSTATUS_LASTBACKUPDBTIME	(1LL << 8L)
#define MASK_TSERVERSTATUS_CPUUTILIZATION	(1LL << 9L)
#define MASK_TSERVERSTATUS_MEMORYUSAGE		(1LL << 10L)
#define MASK_TSERVERSTATUS_DEADCOUNTER		(1LL << 11L)
#define MASK_TSERVERSTATUS_REMAINMEMORY		(1LL << 12L)
#define MASK_TSERVERSTATUS_NETWORKSENDRATE	(1LL << 13L)
#define MASK_TSERVERSTATUS_NETWORKRECVRATE	(1LL << 14L)
#define MASK_TSERVERSTATUS_DEVICEINFO		(1LL << 15L)
#define MASK_TSERVERSTATUS_SVRTYPE			(1LL << 16L)


//磁盘状态监控
struct tDiskStatus {
	TCHAR SvrID[ MAX_GUID_LEN ];			//服务器ID
	uint32 Partition;						//分区盘符
	TCHAR SvrName[ MAX_NAME_LEN ];			//服务器名称
	ulong Type;								//分区类型
	ulonglong Capacity;						//磁盘总容量
	ulonglong UsedSize;						//游戏大小
	ulonglong FreeSize;						//剩余空间
	size_t	DeadCounter;					//僵死计数
	uint32 ReadDataRate;					//平均读取数据速度,KB/s

	tDiskStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tDiskStatus>	TDiskStatusPtr;

#define MASK_TDISKSTATUS_SVRID				(1LL << 1L)
#define MASK_TDISKSTATUS_PARTITION			(1LL << 2L)
#define MASK_TDISKSTATUS_SVRNAME			(1LL << 3L)
#define MASK_TDISKSTATUS_TYPE				(1LL << 4L)
#define MASK_TDISKSTATUS_CAPACITY			(1LL << 5L)
#define MASK_TDISKSTATUS_USEDSIZE			(1LL << 6L)
#define MASK_TDISKSTATUS_FREESIZE			(1LL << 7L)
#define MASK_TDISKSTATUS_DEADCOUNTER		(1LL << 8L)
#define MASK_TDISKSTATUS_READDATARATE		(1LL << 9L)


//三层任务状态监控
struct tTaskStatus {
	uint32 GID;					//游戏ID
	int	   State;				//任务状态
	int    TaskType;			//任务类型
	ushort Connect;				//连接数
	ulong  DownloadState;		//下载中状态
	ulong  TransferRate;		//传输速度
	ulong  TimeLeft;			//剩余时间
	ulong  TimeElapsed;			//消耗时间
	ulong  Progress;			//进度
	uint64 BytesTotal;			//总下载量
	uint64 UpdateSize;			//更新量
	uint64  BytesTransferred;	//传输量

	tTaskStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tTaskStatus>	TTaskStatusPtr;


#define MASK_TTASKSTATUS_GID				(1LL << 1L)
#define MASK_TTASKSTATUS_STATE				(1LL << 2L)
#define MASK_TTASKSTATUS_TASKTYPE			(1LL << 3L)
#define MASK_TTASKSTATUS_CONNECT			(1LL << 4L)
#define MASK_TTASKSTATUS_DOWNLOADSTATE		(1LL << 5L)
#define MASK_TTASKSTATUS_BYTESTRANSFERRED	(1LL << 6L)
#define MASK_TTASKSTATUS_PROGRESS			(1LL << 7L)
#define MASK_TTASKSTATUS_TIMEELAPSED		(1LL << 8L)
#define MASK_TTASKSTATUS_TIMELEFT			(1LL << 9L)
#define MASK_TTASKSTATUS_TRANSFERRATE		(1LL << 10L)
#define MASK_TTASKSTATUS_BYTESTOTAL			(1LL << 11L)
#define MASK_TTASKSTATUS_UPDATESIZE			(1LL << 12L)


//同步任务状态监控
struct tUpdateGameStatus {
	ulong  ClientIP;				// 机器IP
	TCHAR  ClientName[MAX_NAME_LEN];// 机器名称
	uint32 GID;						// 游戏ID
	TCHAR  GameName[MAX_NAME_LEN];	// 游戏名称
	TCHAR  TID[ MAX_GUID_LEN ];		// 开机任务中游戏推送对应的GID或者同步任务中的SID
	int	   Type	;					// 0：游戏推送；1：同步任务；2：内网更新
	int    UpdateState;				// 同步状态
	uint64 SizeLeft;				// 剩余量
	ulong TransferRate;				// 传输速度
	uint64 UpdateSize;				// 更新量
	TCHAR  SvrDir[MAX_PATH];		// 服务端路径
	TCHAR  NodeDir[MAX_PATH];		// 客户端路径


	tUpdateGameStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tUpdateGameStatus>	TUpdateGameStatusPtr;


#define MASK_TUPDATEGAMESTATUS_CLIENTIP				(1LL << 1L)
#define MASK_TUPDATEGAMESTATUS_CLIENTNAME			(1LL << 2L)
#define MASK_TUPDATEGAMESTATUS_GID					(1LL << 3L)
#define MASK_TUPDATEGAMESTATUS_GAMENAME				(1LL << 4L)
#define MASK_TUPDATEGAMESTATUS_TID					(1LL << 5L)
#define MASK_TUPDATEGAMESTATUS_TYPE					(1LL << 6L)
#define MASK_TUPDATEGAMESTATUS_UPDATESTATE			(1LL << 7L)
#define MASK_TUPDATEGAMESTATUS_SIZELEFT				(1LL << 8L)
#define MASK_TUPDATEGAMESTATUS_TRANSFERRATE			(1LL << 9L)
#define MASK_TUPDATEGAMESTATUS_UPDATESIZE			(1LL << 10L)
#define MASK_TUPDATEGAMESTATUS_SVRDIR				(1LL << 11L)
#define MASK_TUPDATEGAMESTATUS_NODEDIR				(1LL << 12L)



//同步任务状态监控
struct tSyncTaskStatus {
	TCHAR  SID[ MAX_GUID_LEN ];		// 同步任务中的SID
	uint32 GID;						// 游戏ID
	TCHAR  NameID[ MAX_GUID_LEN ];	// 同步任务中的NameID
	TCHAR  GameName[MAX_NAME_LEN];	// 游戏名称
	TCHAR  SvrDir[MAX_PATH];		// 服务器的目录
	TCHAR  NodeDir[MAX_PATH];		// 节点的目录
	int    UpdateState;				// 同步状态
	ulong  TransferRate;			// 传输速度
	uint64 BytesTotal;				// 总下载量
	uint64 UpdateSize;				// 更新量
	uint64 BytesTransferred;		// 传输量
	TCHAR  Error[ MAX_NAME_LEN ];	// 同步任务中的错误信息

	tSyncTaskStatus(){ memset(this,0,sizeof(*this)); }
};
typedef std::tr1::shared_ptr<tSyncTaskStatus>	TSyncTaskStatusPtr;

#define MASK_TSYNCTASKSTATUS_SID					(1LL << 1L)
#define MASK_TSYNCTASKSTATUS_GID					(1LL << 2L)
#define MASK_TSYNCTASKSTATUS_NAMEID					(1LL << 3L)
#define MASK_TSYNCTASKSTATUS_GAMENAME				(1LL << 4L)
#define MASK_TSYNCTASKSTATUS_SVRDIR					(1LL << 5L)
#define MASK_TSYNCTASKSTATUS_NODEDIR				(1LL << 6L)
#define MASK_TSYNCTASKSTATUS_UPDATESTATE			(1LL << 7L)
#define MASK_TSYNCTASKSTATUS_TRANSFERRATE			(1LL << 8L)
#define MASK_TSYNCTASKSTATUS_BYTESTOTAL				(1LL << 9L)
#define MASK_TSYNCTASKSTATUS_UPDATESIZE				(1LL << 10L)
#define MASK_TSYNCTASKSTATUS_BYTESTRANSFERRED		(1LL << 11L)
#define MASK_TSYNCTASKSTATUS_ERROR					(1LL << 12L)

struct tPlugTool
{
	//Attribuild
	ulong	PID;							// 工具ID
	TCHAR	CID[ MAX_GUID_LEN ];			// 工具类别 
	ulong	IdcVer;							// 中心版本号
	ulong   SvrVer;							// 本地版本号
	ulong	Priority;						// 优先级		
	//Base Elenment
	TCHAR	Name[ MAX_NAME_LEN ];			// 工具名称
	TCHAR	Comment[ MAX_TEXT_LEN ];		// 备注
	TCHAR	ToolSource[ MAX_TEXT_LEN ];	// 工具来源(提供者)
	ulong	Size;							// 工具大小	
	ulong	IdcClick;						// 热门度
	ulong	DownloadType;					// 下载类型	enum{
											//1：表示Server Element;
											//2：表示Client Element;
											//4：表示Config Element;}
	ulong  DownloadStatus;					// 0:未下载；1：已下载
	ulong	Status;							// 0：未启用;1：已启用；
	//Server Element
	TCHAR	SvrName[ MAX_NAME_LEN ];
	TCHAR	SvrExe[ MAX_PATH ];
	TCHAR	SvrParam[ MAX_PARAM_LEN ];
	TCHAR	SvrPath[ MAX_PATH ];
	ulong	SvrRunType;						// 已服务方式启动；或者exe方式启动 			
	//Client Element
	TCHAR   CliName[ MAX_NAME_LEN ];
	TCHAR	CliExe[ MAX_PATH ];
	TCHAR	CliParam[ MAX_PARAM_LEN ];
	TCHAR	CliPath[ MAX_PATH ];
	ulong	CliRunType;
	//Config Element
	TCHAR   ConfigName[ MAX_NAME_LEN ];
	TCHAR	ConfigExe[ MAX_PATH ];
	TCHAR	ConfigParam[ MAX_PARAM_LEN ];
	TCHAR	ConfigPath[ MAX_PATH ];
	ulong	ConfigRunType;
//	unsigned char	Icon[1024 * 15/*0*/];


	tPlugTool(){memset(this,0 ,sizeof(*this));}
};
typedef std::tr1::shared_ptr<tPlugTool> TPlugToolPtr;

#define MASK_TPLUGTOOL_PID				(1LL << 1L)
#define MASK_TPLUGTOOL_CID				(1LL << 2L)
#define MASK_TPLUGTOOL_IDCVER			(1LL << 3L)
#define MASK_TPLUGTOOL_SVRVER			(1LL << 4L)
#define MASK_TPLUGTOOL_PRIORITY			(1LL << 5L)
#define MASK_TPLUGTOOL_NAME				(1LL << 6L)
#define MASK_TPLUGTOOL_COMMENT			(1LL << 7L)
#define MASK_TPLUGTOOL_TOOLSOURCE		(1LL << 8L)
#define MASK_TPLUGTOOL_SIZE				(1LL << 9L)
#define MASK_TPLUGTOOL_IDCCLICK			(1LL << 10L)
#define MASK_TPLUGTOOL_DOWNLOADTYPE		(1LL << 11L)
#define MASK_TPLUGTOOL_DOWNLOADSTATUS	(1LL << 12L)
#define MASK_TPLUGTOOL_STATUS			(1LL << 13L)
#define MASK_TPLUGTOOL_SVRNAME			(1LL << 14L)
#define MASK_TPLUGTOOL_SVREXE			(1LL << 15L)
#define MASK_TPLUGTOOL_SVRPARAM			(1LL << 16L)
#define MASK_TPLUGTOOL_SVRPATH			(1LL << 17L)
#define MASK_TPLUGTOOL_SVRRUNTYPE		(1LL << 18L)
#define MASK_TPLUGTOOL_CLINAME			(1LL << 19L)
#define MASK_TPLUGTOOL_CLIEXE			(1LL << 20L)
#define MASK_TPLUGTOOL_CLIPARAM			(1LL << 21L)
#define MASK_TPLUGTOOL_CLIPATH			(1LL << 22L)
#define MASK_TPLUGTOOL_CLIRUNTYPE		(1LL << 23L)
#define MASK_TPLUGTOOL_CONFIGNAME		(1LL << 24L)
#define MASK_TPLUGTOOL_CONFIGEXE		(1LL << 25L)
#define MASK_TPLUGTOOL_CONFIGPARAM		(1LL << 26L)
#define MASK_TPLUGTOOL_CONFIGPATH		(1LL << 27L)
#define MASK_TPLUGTOOL_CONFIGRUNTYPE	(1LL << 28L)


struct tPlugToolStatus
{
	long	PID;							// 工具ID
	TCHAR	CID[ MAX_GUID_LEN ];			// 工具类别 
	uint64	Size;							// 插件大小
	uint64	ReadSize;						// 已读取大小
	uint64  TransferRate;					// 传输速度
	uint64	AvgTransferRate;				// 平均传输速度	
	ulong	TotalTime;						// 总共耗时
	ulong	TimeLeft;						// 剩余时间
	
	tPlugToolStatus(){memset(this,0 ,sizeof(*this));}
};
typedef std::tr1::shared_ptr<tPlugToolStatus> TPlugToolStatusPtr;

#define MASK_TPLUGTOOLSTATUS_PID				(1LL << 1L)
#define MASK_TPLUGTOOLSTATUS_CID				(1LL << 2L)
#define MASK_TPLUGTOOLSTATUS_SIZE				(1LL << 3L)
#define MASK_TPLUGTOOLSTATUS_TRANSFERRATE		(1LL << 4L)
#define MASK_TPLUGTOOLSTATUS_AVGTRANSFERRATE	(1LL << 5L)
#define MASK_TPLUGTOOLSTATUS_TOTALTIME			(1LL << 6L)
#define MASK_TPLUGTOOLSTATUS_TIMELEFT			(1LL << 7L)


struct tGameExVersion
{
	long	GID;							// 游戏ID
	ulong	GameExVer;						// 游戏扩展版本
	ulong	CommentVer;						// 游戏评论版本

	tGameExVersion(){memset(this,0 ,sizeof(*this));}
};
typedef std::tr1::shared_ptr<tGameExVersion> TGameExVersionPtr;

#define MASK_TGAMEEXVERSION_GID						(1LL << 1L)
#define MASK_TGAMEEXVERSION_GAMEEXVER				(1LL << 2L)
#define MASK_TGAMEEXVERSION_COMMENTVER				(1LL << 3L)
////////////////////////////////////////////////////////////////////////
// RECORD OR FIELD OPERATE HELPER MACRO
//
#define CLEAR_CHAR_ARRAY(str) str[0] = 0
#define CLEAR_STRING(str) str[0] = 0
#define STRING_ISEMPTY(str) (str[0] == 0)
#define LAST_CHAR(str) str[_tcslen(str) - 1]

inline bool VALUE_FIELD_COMP(const uint64 &lhs, const uint64 &rhs)
{
	 return lhs != rhs;
}

template<typename T>
bool VALUE_FIELD_COMP(const T &lhs, const T &rhs)
{
	return lhs != rhs;
}


inline bool VALUE_FIELD_COMP(const TCHAR *lhs, const TCHAR *rhs)
{
	return _tcscmp(lhs, rhs) != 0;
}


template<typename T>
void VALUE_FIELD_COPY(T &lhs, const T &rhs)
{
	lhs = rhs;
}

template<typename CharT, size_t N>
void VALUE_FIELD_COPY(CharT (&lhs)[N], const CharT (&rhs)[N])
{
	_tcscpy_s(lhs, _countof(lhs), rhs);
}

#define BUFFER_FIELD_COMP(f1, f2) (memcmp(f1, f2, sizeof(f1)) != 0)
#define BUFFER_FIELD_COPY(des, src) memcpy(des, src, sizeof(des))

#define STRING_FIELD_VALID(field) (field[0] != 0)
#define STRING_FIELD_CUMULATE(des, src) _tcsnicmp(des, src, _countof(des))

#define VALUE_FIELD_VALID(field) (field != 0)
#define VALUE_FIELD_CUMULATE(des, src) des += src

#define SQL_COND_MODE_EQUAL 2
#define SQL_COND_MODE_KEY 1
#define SQL_COND_MODE_NEQUAL 3

#pragma pack(pop)
} //namespace db
} //namespace i8desk



#endif //I8TYPE_H