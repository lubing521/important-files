#ifndef __CUSTOM_DATA_TYPE_HPP
#define __CUSTOM_DATA_TYPE_HPP

#include "../../../../include/tablestruct.h"
#include "../../../../include/Utility/SmartHandle.hpp"
#include <vector>
#include <array>
#include <functional>

namespace i8desk
{
	namespace data_helper
	{
		const stdex::tString RT_UNKNOWN = _T("未设置");
		const stdex::tString RT_LOCAL	= _T("本地更新");
		const stdex::tString RT_VDISK	= _T("虚拟盘");
		const stdex::tString RT_DIRECT	= _T("不更新,直接运行");


		// 虚拟盘开始类型
		enum VDStartType 
		{
			VD_BOOT	=	0,			/*开机*/
			VD_STARTMENU,			/*开菜单*/
			VD_STARTGAME,			/*开游戏*/
		};


		// 文件路径名类型
		enum FPType 
		{
			FP_ABSPATH,				/*绝对路径*/
			FP_REALPATH,			/*相对路径*/
		};

		enum FType 
		{
			fFile,
			fDir
		};


		// 上传包的类型
		enum UpLoadPacketType 
		{
			uptFirst,
			uptMiddle,
			uptLast,
			upOnlyOne
		};

		//得到盘符，目录或者文件列表
		enum GetFileOrDirType
		{
			 GET_DRVLIST,		
			 GET_DIRLIST,		
			 GET_FILELIST	
		};


		// 任务下载通知
		enum TaskNotify 
		{
			TASK_COMPLATE,
			TASK_DOWNLOADFAILED,
			TASK_CHECHFAILED,
			TASK_STOPCOMPLATE
		}; 

		// 运行方式
		enum emRunType
		{
			ERT_NONE = 0,		// 未设置
			ERT_LOCAL,			// 本地更新
			ERT_VDISK,			// 虚拟盘
			ERT_DIRECT,			// 直接运行（不更新直接从客户机上运行）
		};


		// 负载平衡
		enum emBalanceType
		{
			BALANCE_UNKNOWN = 0,	// 未知
			BALANCE_POLL,		// 轮询分配
			BALANCE_AREAPRIORITY,	// 区域优先
			BALANCE_DYNAMIC,		// 动态
		};

		// 推送模式
		enum FC_COPY_MODE
		{
			FC_COPY_COMPARE = 0,
			FC_COPY_QUICK	= 1,  // 对比文件
			FC_COPY_FORE	= 2,  // 不对比 全部更新
			FC_COPY_VERSION = 3   // 对比索引版本号
		};

		enum E_CliCtrlCmd
		{
			Ctrl_InstallProt	= 0,	//安装还原，info:后面有个dword表示是否保护所有分区.为１表示保护，为０表示不保护
			Ctrl_RemoveProt		= 1,	//删除还原，info:无
			Ctrl_ProtArea		= 2,	//保护还原分区, info:有两个String,第一个表示保护的分区列表，第二个表示取消保护的分区列表
			Ctrl_ShutDown		= 3,	//开机，重启，info:用1个dword表示：为1表示重启，为0表示关机
			Ctrl_ExecFile		= 4,	//执行指定文件，info:后面有一个String表示需要运行的文件.
			Ctrl_ViewSysInfo	= 5,	//获取系统信息，info:无
			Ctrl_ViewProcInfo	= 6,	//获取进程信息，info:无
			Ctrl_ViewSvrInfo	= 7,	//获取服务信息，info:无
			Ctrl_KillProcSvr	= 8,	//kill进程或者服务，info:一个DWORD(0表示是服务，1表示是进程),一个String表示的进程或者服务名
			Ctrl_Remote         = 9,	//客户端远程控制启动，关闭，info:用个dword表示：为１表示启动，为０表示关闭 
			Ctrl_Chkdsk         = 10,	//客户端修复磁盘
			Ctrl_PushGameAtOnce = 11,	//实时推送游戏 

			Ctrl_EnableSC       = 100,  //启用安全中心
			Ctrl_EnableIEProt   = 101,  //启用IE保护
			Ctrl_EnableDogProt  = 102   //启用防狗驱动
		};

		// 是否为本地游戏
		enum LocalGame
		{
			StatusNone	= 0,			// 不在本地
			StatusLocal					// 在本地
		};


		// 虚拟盘类型
		enum VDiskType
		{
			I8VDisk = 0,				// I8虚拟盘
			OtherVDisk					// 第三方虚拟盘
		};


		// 游戏下载优先级
		enum DownloadPriority
		{
			Lowest = 1,					// 最低
			Lower,						// 较低
			Normal,						// 正常
			Higher,						// 较高
			Highest						// 最高
		};

		// 客户机在线
		enum ClientStatus
		{
			Offline = 0,				// 离线
			Online						// 在线
		};


		// 开机任务执行日期
		enum StartupRunDate
		{
			Mon	= 1,							// 星期一
			Tue	= 2	<< 0,						// 星期二
			Wed	= 2	<< 1,						// 星期三
			Thu	= 2	<< 2,						// 星期四
			Fri = 2	<< 3,						// 星期五
			Sat	= 2	<< 4,						// 星期六
			Sun	= 2	<< 5						// 星期七
		};

		// 开机任务执行方式
		enum StartupRunType
		{
			Boot,						// 开机启动
			Menu						// 菜单启动
		};

	}


	


	namespace db
	{
		// ---------------------------------

		struct tClientStatus
			: public tClient
		{};
		typedef std::tr1::shared_ptr<tClientStatus> TClientStatusPtr;


		struct tPushGame
			: public tCmpBootTask
		{};
		typedef std::tr1::shared_ptr<tPushGame> TPushGamePtr;

		struct tCustomRunType
			: public tRunType
		{
			stdex::tString Name;
		};
		typedef std::tr1::shared_ptr<tCustomRunType>	TCustomRunTypePtr;

		struct tCustomGame
			: public tGame
		{
			utility::ICONPtr Icon;
			std::vector<TCustomRunTypePtr> Runtypes;
			stdex::tString RuntypeDesc;
			stdex::tString StatusDesc;
			stdex::tString GIDDesc;
		};
		typedef std::tr1::shared_ptr<tCustomGame>		TCustomGamePtr;

		struct tCustomClass
			: public tClass
		{
			utility::BITMAPPtr Icon;
			utility::BITMAPPtr Thumbnail;
			size_t ImgIndex;

			tCustomClass()
				: ImgIndex(0)
			{}
		};
		typedef std::tr1::shared_ptr<tCustomClass>		TCustomClassPtr;


		struct tCustomPlugTool
			: public tPlugTool
		{
			utility::ICONPtr Icon;
			std::tr1::array<utility::BITMAPPtr, 4> StatusBmp;
			utility::BITMAPPtr rateBmp;

		};
		typedef std::tr1::shared_ptr<tCustomPlugTool>	TCustomPlugToolPtr;


		// 统计信息
		struct tCustomStatics
		{
			ulong IdcCount;
			unsigned long long IdcSize;
			ulong LocalCount;
			unsigned long long LocalSize;
		
			ulong OnlineClients;
			ulong TotalClients;

			DWORD LogState;

			tCustomStatics()
			{
				memset(this, 0, sizeof(*this));
			}
		};
		typedef std::tr1::shared_ptr<tCustomStatics> TCustomStaticsPtr;


		// 监控首页数据结构
		struct tCustomMonitor
		{
			struct tServer
			{
				TCHAR SvrName[MAX_NAME_LEN];
				int   SvrType;
				DWORD Status;
				ulong ClientConnects;
				ulong Connects;
				ulong TotalSpeed;
				uint64 TotalReadData; 

				tServer()
				{
					memset(this, 0, sizeof(*this));
				}
			};
			typedef std::tr1::shared_ptr<tServer> tServerPtr;
			std::vector<tServerPtr> ServersInfo;


			struct tDownloadRes
			{
				ulong Status;
				ulong DownlaodingCount;
				ulong TotalSpeed;
				ulong QueueCount;
				ushort TotalConnect;
				ulonglong TmpSize;
				long LimitSpeed;
				ulong CPUUsage;

				tDownloadRes()
				{
					memset(this, 0, sizeof(*this));
				}
			};
			tDownloadRes DownloadResInfo;

			struct tUpdate
			{
				ulong TotalClients;
				ulong TotalSpeed;

				tUpdate()
				{
					memset(this, 0, sizeof(*this));
				}
			};
			tUpdate UpdateInfo;

			struct tSyncTask
			{
				ulong TotalClients;
				ulong TotalSpeed;

				tSyncTask()
				{
					memset(this, 0, sizeof(*this));
				}
			};
			tSyncTask SyncTaskInfo;
		};
		typedef std::tr1::shared_ptr<tCustomMonitor> TCustomMonitorPtr;


		// Area排序
		struct AreaLess
			: std::less<stdex::tString>
		{
			bool operator()(const stdex::tString &lhs, const stdex::tString &rhs) const
			{
				return lhs < rhs;
			}
		};
	}
}





#endif