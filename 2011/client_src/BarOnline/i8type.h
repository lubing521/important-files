#ifndef I8TYPE_H
#define I8TYPE_H

#ifdef USED_I8_ATOM_D
#	include "sync.h"
#endif

namespace i8desk {

//---------------------------------------------
//base type define 
typedef char int8;
typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned char byte;
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

//-----------------------------------------------
//原子数据类型
#ifdef USED_I8_ATOM_D

#define ATOM_OP_MONOCULAR_F(op) \
	T operator op () { \
	I8_GUARD(ZGuard, Lty, m_lock); \
	return op m_d ; }

#define ATOM_OP_MONOCULAR_B(op) \
	T operator op (int) { \
	I8_GUARD(ZGuard, Lty, m_lock); \
	return m_d op; }

#define ATOM_OP_BINOCULAR(op) \
	T operator op (const T &d) { \
	I8_GUARD(ZGuard, Lty, m_lock); \
	return m_d op d; }

#define ATOM_OP_RELATIONS(op) \
	bool operator op (const T &d) const { \
	I8_GUARD(ZGuard, Lty, m_lock); \
	return m_d op d; }

#define ATOM_FRIEND_OP_BINOCULAR_R(op) \
	template<typename U, typename T, typename L> \
	U operator op (const U& d, const ZAtomD<T, L>& a) { \
	return d op (T)a; }

#define ATOM_FRIEND_OP_BINOCULAR_L(op) \
	template<typename U, typename T, typename L> \
	T operator op (const ZAtomD<T, L>& a, const U& d) { \
	return (T)a op d; }

#define ATOM_FRIEND_OP_BINOCULAR_D(op) \
	template<typename U, typename L1, typename T, typename L2> \
	U operator op (const ZAtomD<U, L1>& l, const ZAtomD<T, L2>& r) { \
	return (U)l op (T)r; }

template<typename T, typename Lty = ZSync>
class ZAtomD
{
	typedef ZAtomD<T, Lty> _MYty;
public:
	
	//构造方法
	ZAtomD() : m_d(0) {}
	ZAtomD(const T& d) : m_d(d) {}
	template<typename U> ZAtomD(const U& d) : m_d(d) {}

	ZAtomD(const ZAtomD& a) { m_d = a; }
	template<typename U, typename L> ZAtomD(const ZAtomD<U, L>& a) { m_d = a; }

	//重载赋值
	_MYty& operator=(const ZAtomD& a) 
	{
		if (&a != this) {
			I8_GUARD(ZGuard, Lty, m_lock); 
			m_d = a;
		}
		return *this;
	}
	template<typename U, typename L> _MYty& operator=(const ZAtomD<U, L>& a)
	{
		I8_GUARD(ZGuard, Lty, m_lock); 
		m_d = a;
		return *this;
	}

	_MYty& operator=(const T& d)
	{
		I8_GUARD(ZGuard, Lty, m_lock); 
		m_d = d;
		return *this;
	}
	template<typename U> _MYty& operator=(const U& d)
	{
		I8_GUARD(ZGuard, Lty, m_lock); 
		m_d = d;
		return *this;
	}

	//类型转换
	operator T() const 
	{ 
		I8_GUARD(ZGuard, Lty, m_lock); 
		return m_d; 
	}
	template<typename U> operator U() const 
	{ 
		I8_GUARD(ZGuard, Lty, m_lock); 
		return m_d; 
	}

	//运算操作
	ATOM_OP_MONOCULAR_F(++)
	ATOM_OP_MONOCULAR_B(++)
	ATOM_OP_MONOCULAR_F(--)
	ATOM_OP_MONOCULAR_B(--)
	ATOM_OP_MONOCULAR_F(~)
	ATOM_OP_BINOCULAR(+=)
	ATOM_OP_BINOCULAR(-=)
	ATOM_OP_BINOCULAR(*=)
	ATOM_OP_BINOCULAR(/=)
	ATOM_OP_BINOCULAR(%=)
	ATOM_OP_BINOCULAR(&=)
	ATOM_OP_BINOCULAR(|=)
	ATOM_OP_BINOCULAR(^=)
	ATOM_OP_BINOCULAR(>>=)
	ATOM_OP_BINOCULAR(<<=)

	/*
	比较操作
	ATOM_OP_RELATIONS(==)
	ATOM_OP_RELATIONS(!=)
	ATOM_OP_RELATIONS(>)
	ATOM_OP_RELATIONS(>=)
	ATOM_OP_RELATIONS(<)
	ATOM_OP_RELATIONS(<=)
	//*/
private:
	mutable Lty m_lock;
	T m_d;
};

ATOM_FRIEND_OP_BINOCULAR_L(+)
ATOM_FRIEND_OP_BINOCULAR_L(-)
ATOM_FRIEND_OP_BINOCULAR_L(*)
ATOM_FRIEND_OP_BINOCULAR_L(/)
ATOM_FRIEND_OP_BINOCULAR_L(%)
ATOM_FRIEND_OP_BINOCULAR_L(&)
ATOM_FRIEND_OP_BINOCULAR_L(|)
ATOM_FRIEND_OP_BINOCULAR_L(^)
ATOM_FRIEND_OP_BINOCULAR_L(<<)
ATOM_FRIEND_OP_BINOCULAR_L(>>)
ATOM_FRIEND_OP_BINOCULAR_R(+)
ATOM_FRIEND_OP_BINOCULAR_R(-)
ATOM_FRIEND_OP_BINOCULAR_R(*)
ATOM_FRIEND_OP_BINOCULAR_R(/)
ATOM_FRIEND_OP_BINOCULAR_R(%)
ATOM_FRIEND_OP_BINOCULAR_R(&)
ATOM_FRIEND_OP_BINOCULAR_R(|)
ATOM_FRIEND_OP_BINOCULAR_R(^)
ATOM_FRIEND_OP_BINOCULAR_R(<<)
ATOM_FRIEND_OP_BINOCULAR_R(>>)
ATOM_FRIEND_OP_BINOCULAR_D(+)
ATOM_FRIEND_OP_BINOCULAR_D(-)
ATOM_FRIEND_OP_BINOCULAR_D(*)
ATOM_FRIEND_OP_BINOCULAR_D(/)
ATOM_FRIEND_OP_BINOCULAR_D(%)
ATOM_FRIEND_OP_BINOCULAR_D(&)
ATOM_FRIEND_OP_BINOCULAR_D(|)
ATOM_FRIEND_OP_BINOCULAR_D(^)
ATOM_FRIEND_OP_BINOCULAR_D(<<)
ATOM_FRIEND_OP_BINOCULAR_D(>>)

#endif

#include <string>
#ifdef _UNICODE
typedef std::wstring std_string; 
#else
typedef std::string  std_string; 
#endif

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

struct tClass {
	char	DefClass[ MAX_GUID_LEN ];
	char	Name[ MAX_NAME_LEN ];
	char	Path[ MAX_PATH ];
	char	Type[ MAX_NAME_LEN ];
};

#define MASK_TCLASS_DEFCLASS		(1LL << 1L)
#define MASK_TCLASS_NAME			(1LL << 2L)
#define MASK_TCLASS_PATH			(1LL << 3L)
#define MASK_TCLASS_TYPE			(1LL << 4L)

struct tGame {
	uint32	GID;
	uint32	PID;
	char	Name[ MAX_NAME_LEN ];
	char	DefClass[ MAX_GUID_LEN ];
	char	GameExe[ MAX_PATH ];
	char	Param[ MAX_PARAM_LEN ];
	ulong	Size;	
	char	URL[ MAX_PATH ];	
	int		DeskLnk;
	int		Toolbar;
	char	SvrPath[ MAX_PATH ];	
	char	CliPath[ MAX_PATH ];
	int		Priority;
	int     DownPriority;
	int		ShowPriority;
	char	MatchFile[ MAX_PATH ];
	char	SaveFilter[ MAX_REMARK_LEN ];
	ulong	IdcUptDate;
	ulong	SvrUptDate;
	ulong	IdcVer;
	ulong	SvrVer;
	int		AutoUpt;
	int		I8Play;
	ulong	IdcClick;
	ulong	SvrClick;
	ulong	AddDate;
	int		Status;
	int		IfUpdate;
	int		IfDisplay;
	char	Comment[ MAX_REMARK_LEN ];
	int		Auto;
	int		Declinectr;
	int		hide;
	char	LocalPath[ MAX_PATH ];
	char	SaveFilter2[ MAX_REMARK_LEN ];
	char	GameExe2[ MAX_PATH ];
	char	Param2[ MAX_PARAM_LEN ];
	ulong	SvrClick2;
};

#define MASK_TGAME_GID				(1LL << 1L)
#define MASK_TGAME_PID				(1LL << 2L)
#define MASK_TGAME_NAME				(1LL << 3L)
#define MASK_TGAME_DEFCLASS			(1LL << 4L)
#define MASK_TGAME_GAMEEXE			(1LL << 5L)
#define MASK_TGAME_PARAM			(1LL << 6L)
#define MASK_TGAME_SIZE				(1LL << 7L)	
#define MASK_TGAME_URL				(1LL << 8L)	
#define MASK_TGAME_DESKLNK			(1LL << 9L)
#define MASK_TGAME_TOOLBAR			(1LL << 10L)
#define MASK_TGAME_SVRPATH			(1LL << 11L)	
#define MASK_TGAME_CLIPATH			(1LL << 12L)
#define MASK_TGAME_PRIORITY			(1LL << 13L)
#define MASK_TGAME_DOWNPRIORITY		(1LL << 14L)
#define MASK_TGAME_SHOWPRIORITY		(1LL << 15L)
#define MASK_TGAME_MATCHFILE		(1LL << 16L)
#define MASK_TGAME_SAVEFILTER		(1LL << 17L)
#define MASK_TGAME_IDCUPTDATE		(1LL << 18L)
#define MASK_TGAME_SVRUPTDATE		(1LL << 19L)
#define MASK_TGAME_IDCVER			(1LL << 20L)
#define MASK_TGAME_SVRVER			(1LL << 21L)
#define MASK_TGAME_AUTOUPT			(1LL << 22L)
#define MASK_TGAME_I8PLAY			(1LL << 23L)
#define MASK_TGAME_IDCCLICK			(1LL << 24L)
#define MASK_TGAME_SVRCLICK			(1LL << 25L)
#define MASK_TGAME_ADDDATE			(1LL << 26L)
#define MASK_TGAME_STATUS			(1LL << 27L)
#define MASK_TGAME_IFUPDATE			(1LL << 28L)
#define MASK_TGAME_IFDISPLAY		(1LL << 29L)
#define MASK_TGAME_COMMENT			(1LL << 30L)
#define MASK_TGAME_AUTO				(1LL << 31L)
#define MASK_TGAME_DECLINECTR		(1LL << 32L)
#define MASK_TGAME_HIDE				(1LL << 33L)
#define MASK_TGAME_LOCALPATH		(1LL << 34L)
#define MASK_TGAME_SAVEFILTER2		(1LL << 35L)
#define MASK_TGAME_GAMEEXE2			(1LL << 36L)
#define MASK_TGAME_PARAM2			(1LL << 37L)
#define MASK_TGAME_SVRCLICK2		(1LL << 38L)

struct tArea {
	char	AID[MAX_GUID_LEN];		//区域ID
	char	Name[MAX_NAME_LEN];		//区域名称
	char	SID[ MAX_GUID_LEN ];	//任务ID,优先选中的服务器标识
};

#define MASK_TAREA_AID				(1LL << 1L)
#define MASK_TAREA_NAME				(1LL << 2L)
#define MASK_TAREA_SID				(1LL << 3L)

struct rGameArea {
	char	AID[MAX_GUID_LEN];	//区域ID
	int		GID;				//游戏ID
	int		RunType;			//运行方式
	char	VID[MAX_GUID_LEN];	//虚拟盘ID
};
typedef rGameArea tGameArea;

#define MASK_TGAMEAREA_AID			(1LL << 1L)
#define MASK_TGAMEAREA_GID			(1LL << 2L)
#define MASK_TGAMEAREA_RUNTYPE		(1LL << 3L)
#define MASK_TGAMEAREA_VID			(1LL << 4L)


struct tClient {
	char Name[MAX_NAME_LEN];
	ulong IP;
	char Mark[MAX_IPSTRING_LEN];
	char NetGate[MAX_IPSTRING_LEN];
	char DNS[MAX_IPSTRING_LEN];
	char DNS2[MAX_IPSTRING_LEN];
	char MAC[MAX_MACSTRING_LEN];
	char AID[MAX_GUID_LEN];
	int  ProtInstall;
	char AllArea[MAX_DISKPARTION_LEN];
	char ProtArea[MAX_DISKPARTION_LEN];
	char ProtVer[MAX_VERSTRING_LEN];
	char MenuVer[MAX_VERSTRING_LEN];
	char CliSvrVer[MAX_VERSTRING_LEN];
	ulong  GPFreeSize;						//游戏盘剩余空间，单位MB
	int    SCStatus;						//安全中心状态，1：运行
	char CPU[ MAX_REMARK_LEN ];				//CPU信息
	char Memory[ MAX_REMARK_LEN ];			//内存信息
	char Disk[ MAX_REMARK_LEN ];			//磁盘信息
	char Graphics[ MAX_REMARK_LEN ];		//显卡信息
	char Mainboard[ MAX_REMARK_LEN ];		//主板信息
	char NIC[ MAX_REMARK_LEN ];				//网卡信息
	char Webcam[ MAX_REMARK_LEN ];			//摄像头信息
	char System[ MAX_REMARK_LEN ];			//系统信息
};

#define MASK_TCLIENT_NAME			(1LL << 1L)
#define MASK_TCLIENT_IP				(1LL << 2L)
#define MASK_TCLIENT_MARK			(1LL << 3L)
#define MASK_TCLIENT_NETGATE		(1LL << 4L)
#define MASK_TCLIENT_DNS			(1LL << 5L)
#define MASK_TCLIENT_DNS2			(1LL << 6L)
#define MASK_TCLIENT_MAC			(1LL << 7L)	
#define MASK_TCLIENT_AID			(1LL << 8L)	
#define MASK_TCLIENT_PROTINSTALL	(1LL << 9L)
#define MASK_TCLIENT_ALLAREA		(1LL << 10L)
#define MASK_TCLIENT_PROTAREA		(1LL << 11L)	
#define MASK_TCLIENT_PROTVER		(1LL << 12L)
#define MASK_TCLIENT_MENUVER		(1LL << 13L)
#define MASK_TCLIENT_CLISVRVER		(1LL << 14L)
#define MASK_TCLIENT_GPFREESIZE		(1LL << 15L)
#define MASK_TCLIENT_SCSTATUS		(1LL << 16L)
#define MASK_TCLIENT_CPU			(1LL << 17L)
#define MASK_TCLIENT_MEMORY			(1LL << 18L)
#define MASK_TCLIENT_DISK			(1LL << 19L)
#define MASK_TCLIENT_GRAPHICS		(1LL << 20L)
#define MASK_TCLIENT_MAINBOARD		(1LL << 21L)
#define MASK_TCLIENT_NIC			(1LL << 22L)	
#define MASK_TCLIENT_WEBCAM			(1LL << 23L)
#define MASK_TCLIENT_SYSTEM			(1LL << 24L)

struct tVDisk {
	char	VID[MAX_GUID_LEN];
	ulong	IP;
	int		Port;
	int		SvrDrv;
	int		CliDrv;
	int		Serial;
	int		Type;
	int		LoadType;
	char    SID[MAX_GUID_LEN];
};

#define MASK_TVDISK_VID				(1LL << 1L)
#define MASK_TVDISK_IP				(1LL << 2L)
#define MASK_TVDISK_PORT			(1LL << 3L)
#define MASK_TVDISK_SVRDRV			(1LL << 4L)
#define MASK_TVDISK_CLIDRV			(1LL << 5L)
#define MASK_TVDISK_SERIAL			(1LL << 6L)
#define MASK_TVDISK_TYPE			(1LL << 7L)	
#define MASK_TVDISK_LOADTYPE		(1LL << 8L)
#define MASK_TVDISK_SID				(1LL << 9L)

struct tVDiskStatus {
	ulong		IP;
	char		VID[MAX_GUID_LEN];
	ulong		Version;
	ulong		ClientCount;
};

#define MASK_TVDISKSTATUS_IP				(1LL << 1L)
#define MASK_TVDISKSTATUS_VID				(1LL << 2L)
#define MASK_TVDISKSTATUS_VERSION			(1LL << 3L)
#define MASK_TVDISKSTATUS_CLIENTCOUNT		(1LL << 4L)


//SELECT a.AID as aid, a.Name as name, r.GID, r.RunType, v.VID, v.CliDrv
//FROM rGameArea r, tArea a, tVDisk v
//WHERE a.AID=r.AID AND v.VID=r.VID AND r.GID=%u
struct vRunType {
	char	AID[MAX_GUID_LEN];		//区域ID
	char	AName[MAX_NAME_LEN];	//区域名
	int		GID;					//游戏ID
	int		RunType;				//运行方式
	char	VID[MAX_GUID_LEN];		//虚拟盘ID
	char	VCliDrv;				//虚拟盘客户端盘符
};

//菜单皮肤
struct tBarSkin {
	char	Syle[ MAX_NAME_LEN ];	//风格名
	char	PathName[ MAX_PATH ];	//文件名
	char	Version[ MAX_GUID_LEN ];//版本号
};

#define MASK_TBARSKIN_STYLE				(1LL << 1L)
#define MASK_TBARSKIN_PATHNAME			(1LL << 2L)
#define MASK_TBARSKIN_VERSION			(1LL << 3L)

struct tFavorite {
	uint Serial;
	char UID[MAX_GUID_LEN];
	char Name[MAX_NAME_LEN];
	char Type[MAX_NAME_LEN];
	char URL[MAX_PATH];
};

#define MASK_TFAVORITE_SERIAL			(1LL << 1L)
#define MASK_TFAVORITE_UID				(1LL << 2L)
#define MASK_TFAVORITE_NAME				(1LL << 3L)
#define MASK_TFAVORITE_TYPE				(1LL << 4L)
#define MASK_TFAVORITE_URL				(1LL << 5L)

struct tCmpStartTask {
	char UID[MAX_GUID_LEN];
	int AreaType;
	char AreaParam[MAX_GUID_LEN];
	int Type;
	int Flag;
	char Content[MAX_PATH];
};

#define MASK_TCMPSTARTTASK_UID			(1LL << 1L)
#define MASK_TCMPSTARTTASK_AREATYPE		(1LL << 2L)
#define MASK_TCMPSTARTTASK_AREAPARAM	(1LL << 3L)
#define MASK_TCMPSTARTTASK_TYPE			(1LL << 4L)
#define MASK_TCMPSTARTTASK_FLAG			(1LL << 5L)
#define MASK_TCMPSTARTTASK_CONTENT		(1LL << 6L)

struct tSysOpt {
	char key[MAX_NAME_LEN];
	char value[MAX_SYSOPT_LEN];
};

#define MASK_TSYSOPT_KEY			(1LL << 1L)
#define MASK_TSYSOPT_VALUE			(1LL << 2L)

struct tUser {
	char Name[MAX_NAME_LEN];
	char Pwd[MAX_NAME_LEN];
	int RoomSize;
	int CreateDate;
	int LastLoginDate;
	int LastLoginIp;
};

#define MASK_TUSER_NAME					(1LL << 1L)
#define MASK_TUSER_PWD					(1LL << 2L)
#define MASK_TUSER_ROOMSIZE				(1LL << 3L)
#define MASK_TUSER_CREATEDATE			(1LL << 4L)
#define MASK_TUSER_LASTLOGINDATE		(1LL << 5L)
#define MASK_TUSER_LASTLOGINIP			(1LL << 6L)

struct tClientStatus {
	ulong IP;
	int Online; //1:ONLINE,0:OFFLINE
	char Name[MAX_NAME_LEN];
};

#define MASK_TCLIENTSTATUS_IP					(1LL << 1L)
#define MASK_TCLIENTSTATUS_ONLINE				(1LL << 2L)
#define MASK_TCLIENTSTATUS_NAME					(1LL << 3L)

#define MAX_ICON_DATA_SIZE (5*1024)

struct tIcon {
	uint32 gid;
	ulong size;
	uchar data[MAX_ICON_DATA_SIZE];
};

#define MASK_TICON_GID					(1LL << 1L)
#define MASK_TICON_DATA					(1LL << 2L)
#define MASK_TICON_SIZE					(1LL << 3L)

struct tSyncTask {
	char	SID[ MAX_GUID_LEN ];			//任务ID
	char	Name[ MAX_NAME_LEN ];			//任务名称
	int		SyncType;						//同步类型
	char	SyncIP[ MAX_IPSTRING_LEN ];		//同步IP
	char	VDiskIP[ MAX_IPSTRING_LEN ];	//虚拟盘IP
	char	UpdateIP[ MAX_IPSTRING_LEN];	//内网更新IP
	int		DestDrv;						//同步盘符
	int		BalanceType;					//负载平衡类型
};

#define MASK_TSYNCTASK_SID			(1LL << 1L)
#define MASK_TSYNCTASK_NAME			(1LL << 2L)
#define MASK_TSYNCTASK_SYNCTYPE		(1LL << 3L)
#define MASK_TSYNCTASK_SYNCIP		(1LL << 4L)
#define MASK_TSYNCTASK_VDISKIP		(1LL << 5L)
#define MASK_TSYNCTASK_UPDATEIP		(1LL << 6L)
#define MASK_TSYNCTASK_DESTDRV		(1LL << 7L)
#define MASK_TSYNCTASK_BALANCETYPE	(1LL << 8L)

struct tSyncGame {
	char	SID[ MAX_GUID_LEN ];		//任务ID
	uint32	GID;						//游戏ID
};

#define MASK_TSYNCGAME_SID			(1LL << 1L)
#define MASK_TSYNCGAME_GID			(1LL << 2L)


struct tModuleUsage {
	char	Name[ MAX_NAME_LEN ];			//客户机名称任务ID
	char	MAC[ MAX_MACSTRING_LEN ];		//客户机MAC地址
	uint32	MID;							//模块编号
	ulong	ClickCount;						//点击数
	ulong	UsedTime;						//停留时间
};

#define MASK_TMODULEUSAGE_NAME			(1LL << 1L)
#define MASK_TMODULEUSAGE_MAC			(1LL << 2L)
#define MASK_TMODULEUSAGE_MID			(1LL << 3L)
#define MASK_TMODULEUSAGE_CLICKCOUNT	(1LL << 4L)
#define MASK_TMODULEUSAGE_USEDTIME		(1LL << 5L)

// RECORD OR FIELD OPERATE HELPER MACRO
#define CLEAR_CHAR_ARRAY(str) str[0] = 0
#define CLEAR_STRING(str) str[0] = 0
#define STRING_ISEMPTY(str) (str[0] == 0)
#define LAST_CHAR(str) str[strlen(str) - 1]
#define SAFE_STRCPY(des, src) strncpy(des, src, sizeof(des) - 1)
#define SAFE_STRCAT(des, src) strncat(des, src, sizeof(des) - strlen(des) - 1)

#define STRING_FIELD_COMP(f1, f2) (strcmp(f1, f2) != 0)
#define STRING_FIELD_COPY(des, src) memcpy(des, src, sizeof(des))

#define VALUE_FIELD_COMP(f1, f2) (f1 != f2)
#define VALUE_FIELD_COPY(des, src) des = src

#define BUFFER_FIELD_COMP(f1, f2) (memcmp(f1, f2, sizeof(f1)) != 0)
#define BUFFER_FIELD_COPY(des, src) memcpy(des, src, sizeof(des))

#define STRING_FIELD_VALID(field) (field[0] != 0)
#define STRING_FIELD_CUMULATE(des, src) SAFE_STRCAT(des, src)

#define VALUE_FIELD_VALID(field) (field != 0)
#define VALUE_FIELD_CUMULATE(des, src) des += src

#define SQL_COND_MODE_EQUAL 8
#define SQL_COND_MODE_KEY 1

#pragma pack(pop)
} //namespace db
} //namespace i8desk

#endif //I8TYPE_H