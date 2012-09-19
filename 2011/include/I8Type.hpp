#ifndef __I8_TYPE_HPP
#define __I8_TYPE_HPP



namespace i8desk
{

	//上传包的类型
	enum UpLoadPacketType 
	{
		uptFirst,
		uptMiddle,
		uptLast,
		upOnlyOne
	};

	//虚拟盘的类型
	enum VDiskType
	{
		VDISK_TYPE_I8DESK				=	0,			//表示I8Desk虚拟盘
		VDISK_TYPE_OTHER				=	1			//第三方虚拟盘,仅把服务器盘符映射成到客户机的盘符.
	};


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

	//得到盘符，目录或者文件列表
	enum GetFileOrDirType
	{
		GET_DRVLIST,		
		GET_DIRLIST,		
		GET_FILELIST	
	};

	// 通知类型
	enum TaskNotifyType
	{
		GamePlug,
		PlugPlug
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

	// 同步任务
	enum emSyncType
	{
		SYNC_UNKNOWN = 0,	// 未设置
		SYNC_LOCAL,			// 仅同步本地更新资源
		SYNC_VDISK,			// 仅同步虚拟盘资源
		SYNC_CUSTOM,		// 自定义同步
		SYNC_ALL,			// 同步所有资源
		SYNC_SERVER			// 同步镜像服务器
	};

	// 负载平衡
	enum emBalanceType
	{
		BALANCE_UNKNOWN = 0,	// 未知
		BALANCE_POLL,			// 轮询
		BALANCE_AREAPRIORITY,	// 区域优先
		BALANCE_DYNAMIC			// 动态
	};

	// 推送模式
	enum FC_COPY_MODE
	{
		FC_COPY_COMPARE = 0,
		FC_COPY_QUICK	= 1,  // 对比文件
		FC_COPY_FORE	= 2,  // 不对比全部更新
		FC_COPY_VERSION = 3   // 对比索引版本号
	};

	// 推送参数设置格式
	enum PUSH_PARAM_FORMAT
	{
		fst_maxspeed	= 1,	// 最大速度
		scd_cheecktype	= 2,	// 对比方式
		trd_gid			= 3		// 推送游戏的GID
	};

	// 开机任务
	enum BootRunTask 
	{  
		brDeskTop		= 0,		// 换桌面
		brShortcut		= 1,		// 桌面快捷方式
		brIEHomePage	= 2,		// IE主页控制
		brCliExecFile	= 3,		// 执行客户端任一自定义的EXE
		brSvrExecFile	= 4,		// 处理服务端下放的任意文件
		brDelFile		= 5,		// 删除文件或文件夹或某一游戏
		brDelGames		= 6,		// 删除游戏
		brSalfCenterXMl = 7,		// 处理安全中心XML
		brUpdateGame	= 8,		// 更新游戏
		brDirectUpdate  = 9,		// 客户机定向升级
		brExecSvrFolder = 10,    	// 复制服务端文件夹/执行文件夹内程序

        //客户端子开机任务(由开机任务生成多条子开机任务)
        brAtom_DownloadFile,       // 下载文件
        brAtom_DeskTop,            // 换桌面(使用本地图片)
	};


	// 是否为本地游戏
	enum LocalGame
	{
		StatusNone	= 0,			// 不在本地
		StatusLocal					// 在本地
	};


	// 游戏下载优先级
	enum DownloadPriority
	{
		Lowest = 0,					// 最低
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

	// 客户机连接服务器IP
	enum ClientConnectSvr
	{
		UpdateIP = 0,				// 内网更新IP类型
		VDiskIP						// 虚拟盘IP类型
	};

	//虚拟盘的启动类型，即客户机刷盘的时机。
	enum VDiskRefreshType 
	{
		VDISK_RT_Boot					=	0,			/*开机*/
		VDISK_RT_RunMenu				=	1,			/*开菜单*/
		VDISK_RT_RunGame				=	2			/*开游戏*/
	};

	//游戏的运行方式
	enum GameRunType
	{
		GAME_RT_NONE					=	0,			//未设置
		GAME_RT_LOCAL					=	1,			//本地更新
		GAME_RT_VDISK					=	2,			//虚拟盘.(会进一步指定从那个虚拟盘运行)
		GAME_RT_DIRECT					=	3			//直接运行(不更新直接从客户机上运行,主要是针对第三方虚拟盘)
	};

	//客户端对象类型	//for packageheader.resv[0]
	enum OBJECT_TYPE
	{
		OBJECT_UNKOWN					=	0,
		OBJECT_I8DESKCLISVR				=	1,
		OBJECT_CONSOLE					=	2,
		OBJECT_VDISKSVR					=	3,
		OBJECT_BARONLINE				=	4,
		OBJECT_NODESERVER				=	5
	};


	//客户端控制命令
	enum ClientCtrlCmd
	{
		Ctrl_InstallProt				=	0,	//安装还原，info:后面有个dword表示是否保护所有分区.为１表示保护，为０表示不保护
		Ctrl_RemoveProt					=	1,	//删除还原，info:无
		Ctrl_ProtArea					=	2,	//保护还原分区, info:有两个String,第一个表示保护的分区列表，第二个表示取消保护的分区列表
		Ctrl_ShutDown					=	3,	//开机，重启，info:用1个dword表示：为1表示重启，为0表示关机
		Ctrl_ExecFile					=	4,	//执行指定文件，info:后面有一个String表示需要运行的文件.
		Ctrl_ViewSysInfo				=	5,	//获取系统信息，info:无
		Ctrl_ViewProcInfo				=	6,	//获取进程信息，info:无
		Ctrl_ViewSvrInfo				=	7,	//获取服务信息，info:无
		Ctrl_KillProcSvr				=	8,	//kill进程或者服务，info:一个DWORD(0表示是服务，1表示是进程),一个String表示的进程或者服务名
		Ctrl_Remote						=	9,	//客户端远程控制启动，关闭，info:用个dword表示：为１表示启动，为０表示关闭 
		Ctrl_Chkdsk						=	10,	//客户端修复磁盘
		Ctrl_PushGameAtOnce				=	11,	//实时推送游戏 

		Ctrl_EnableSC					=	100,  //启用安全中心
		Ctrl_EnableIEProt				=	101,  //启用IE保护
		Ctrl_EnableDogProt				=	102  //启用防狗驱动
	};

	// 获取三层组件命令
	enum RtStatusCmd
	{
		UP_SPEED						= 0x10,	// 长传总速度 
		DOWN_SPEED						,		// 下载总速度
		DOWNLOAD_STATUS					,		// 三层信息
		UPDATE_DISK						,		// 更新游戏盘信息
		PL_VERSION						,		// PL版本
		DELETE_FILE						,		// 清除三层临时文件
		GAMEEX_VERSION					,		// 得到游戏扩展版本
		SMART_CLEAN						,		// 智能删除
		MSC_TMP_SIZE					,		// 三层下载临时目录大小
		CLEINT_DELETE_GAME				,		// 客户机完成执行删除游戏文件
		CON_SAVE_DATA
	};

	// CmdType
	enum CmdType
	{	
		CMD_PLUG_TOOL					= 0x333	// 插件通知
	};

	// GameMgr删除操作
	enum DelOperation 
	{
		DelNone			= 0,					// 不做删除操作
		DelRecord		= 1,					// 只删除数据库记录
		DelLocal		= 2,					// 只删除服务器文件	
		DelSync			= 4,					// 只通知同步删除
		DelClient		= 8,					// 只删除客户机文件
		DelLock			= 16,					// 停运游戏
		DelTmp			= 32,					// 删除临时目录
		DelIdc			= 64,					// 删除记录
		DelAll			= DelIdc | DelLocal | DelSync | DelClient, // 删除以上所有

	};


	// 图标操作
	enum IconOperation 
	{
		ioAdd,									// 增加	
		ioDel,									// 删除
		ioGet									// 修改
	};



	// 服务器标志
	enum emServerType
	{
		OtherServer	= 0,					// 从服务
		MainServer		= 1					// 主服务
	};

	// 扩展信息类型
	enum ExInfo_Type_en
	{
		E_EXINFO_GAME = 1,						//游戏扩展信息
		E_EXINFO_MENU,                          //菜单分级信息
	};


	// 同步任务操作
	enum SyncOperate
	{
		Driver = 1,								// 分区
		Pause,									// 暂停
		Resume									// 恢复
	};
	
	// 安全中心类型
	enum SafeCenterType
	{
		UNKOWN	= 0,							// 未知
		DISKLESS= 1,						// 无盘安全中心
		KONGHE	= 2								// 康禾安全中心
	};

	// 禁止运行类型
	enum ForbidType
	{
		ForbidExe	= 1,							// 禁止运行的Exe
		ForbidWnd	= 2,							// 禁止运行的窗口
	};

	// 游戏是否隐藏
	enum HideType
	{
		NoHide	= 0,							// 游戏不隐藏
		Hide	= 1,							// 游戏隐藏
	};

	// 公告运行方式
	enum GongGaoRunType
	{
		Pic		= 0,							// 公告显示图片
		Text	= 1,							// 公告显示文字
		All		= 2,							// 公告即显示文字，又显示图片	
	};

	// 同步任务类型
	enum SyncType
	{
		Class	= 0,							// 按类别同步
		Game	= 1								// 按游戏同步
	};

    //同步任务状态
    enum SyncTaskState
    {
        TaskStarted = 1,
        TaskStopped,
        TaskPaused,
        TaskResumed
    };

	// 同步游戏状态
	enum SyncGameState
	{
		GameComplate = 0,							// 完成
		GameUnComplate,								// 未完成
		GamePaused,									// 暂停
		GameError,									// 错误
		GameRunning,								// 进行中
	};

	// 网络状况
	enum NetType
	{
		other = 1,							// 其他
		adsl,								// ADSL
		opticalfiber						// 光纤
	};

	// 网络状况
	enum DelClientGameOp
	{
		Add = 0,							// 添加
		Del,								// 删除
		Find								// 查找
	};
}






#endif