#ifndef __i8desk_define_inc__
#define __i8desk_define_inc__

#pragma once 

#include <Windows.h>
#include <tchar.h>

#include "I8Type.hpp"


//定义所有插件的名字,以及模块
#define PLUG_RTDATASVR_NAME			TEXT("RTDataSvr")			//数据库缓存组件
#define PLUG_RTDATASVR_MODULE		TEXT("RTDataSvr.dll")
#define PLUG_LOGRPT_NAME			TEXT("LogRpt")				//登陆上报插件
#define PLUG_LOGRPT_MODULE			TEXT("LogRpt.dll")
#define PLUG_GAMEMGR_NAME			TEXT("GameMgr")				//游戏下载管理插件
#define PLUG_GAMEMGR_MODULE			TEXT("GameMgr.dll")
#define PLUG_PLUGTOOL_NAME			TEXT("PlugToolPlug")		// 插件中心插件
#define PLUG_PLUGTOOL_MODULE		TEXT("PlugToolPlug.dll")
#define PLUG_BUSINESS_NAME			TEXT("Business")			// 业务插件
#define PLUG_BUSINESS_MODULE		TEXT("Business.dll")	
#define PLUG_REMOTECONTROL_NAME		TEXT("WinVNC")				// 远程控制插件
#define PLUG_REMOTECONTROL_MODULE	TEXT("WinVNC.dll")		



#define PLUG_UPTSVR_NAME			TEXT("UpdateGameSvr")		//内网更新服务端插件名
#define PLUG_SYNCDISK_NAME			TEXT("SyncDisk")			//同步组件插件名
#define PLUG_FORCEUPT_NAME			TEXT("ForceUpdate")			//主动推送插件名
#define PLUG_GAMEUPDATE_MODULE		TEXT("UpdateGame.dll")		//更新与同步组件

#define PLUG_VDISK_NAME				TEXT("VirtualDisk")			//虚拟盘插件
#define PLUG_VDISK_MODULE			TEXT("VirtualDisk.dll")
#define	PLUG_BOOTRUN_NAME			TEXT("BootRun")				//开机任务插件
#define PLUG_BOOTRUN_MODULE			TEXT("BootRun.dll")
#define PLUG_CLIMGR_NAME			TEXT("CliMgr")				//客户机管理插件
#define PLUG_CLIMGR_MODULE			TEXT("CliMgr.dll")
#define PLUG_FRAME_MODULE			TEXT("Frame.dll")			//框架插件
#define MODULE_I8DESKSVR			TEXT("I8DeskSvr.exe")		//I8DeskSvr.exe服务
#define MODULE_I8VDISKSVR			TEXT("I8VDiskSvr.exe")		//I8VDiskSvr.exe服务
#define MODULE_I8DESKCLISVR			TEXT("I8DeskCliSvr.exe")	//I8DeskCliSvr.exe服务
#define MODULE_BARONLINE			TEXT("BarOnline.exe")		//BarOnline.exe菜单
#define MODULE_NODESERVER			TEXT("NodeServer.exe")		//NodeServer.exe节点界面程序
#define KHSTARTNAME                 TEXT("KHUpdate.exe")        //康和安全中心更新文件

//官方游戏最小的游戏gid.
#define MIN_IDC_GID					10000
#define MAIN_SERVER_SID				TEXT("{BDD530E3-E60F-4739-80AF-6E9D845988A4}")

//官方默认的分区.
#define DEFAULT_AREA_GUID			TEXT("{2B35D26B-6E5C-4d18-B108-19F778B6AF18}")
#define DEFAULT_AREA_NAME			TEXT("默认分区")

//官方定义的游戏类别
#define CLASS_WL_GUID				TEXT("{E88DD848-DA99-46b0-97D6-23D0658B718A}")	//网络游戏
#define CLASS_DJ_GUID				TEXT("{30CF0706-E9AE-4804-9D57-B39885B672E4}")	//单机游戏
#define CLASS_XX_GUID				TEXT("{C185F5B5-FA0C-466e-BDDF-6E7EDA5A3F3E}")	//休闲游戏
#define CLASS_WY_GUID				TEXT("{17F2FFB5-B4DC-42ce-B424-E17CFF1279B5}")  //网页游戏
#define CLASS_DZ_GUID				TEXT("{FD844C21-BD62-4369-B146-DC06A5D4CE1E}")	//对战游戏
#define CLASS_QP_GUID				TEXT("{E7CDB5F3-D6C7-4607-8B71-3FC087E4B896}")	//棋牌游戏
#define CLASS_PL_GUID				TEXT("{C8DDDA32-EFD6-43af-B94B-FBADF3196977}")	//游戏插件
#define CLASS_LT_GUID				TEXT("{CF59BC68-973B-461a-97AF-29E183EB3645}")	//聊天工具
#define CLASS_CY_GUID				TEXT("{58D87CC9-5BB1-43e0-A919-C7ED3D7CE46A}")	//常用工具
#define CLASS_YY_GUID				TEXT("{E1DDCC22-9AAF-4020-A65B-F022DBFABC47}")	//影音工具
#define CLASS_GP_GUID				TEXT("{B59A5F1F-9431-461f-A5BD-C88949E29709}")	//股票证券

#define CLASS_TJ_NAME	            TEXT("推荐游戏")
#define CLASS_WL_NAME				TEXT("网络游戏")
#define CLASS_DJ_NAME				TEXT("单机游戏")
#define CLASS_XX_NAME				TEXT("休闲游戏")
#define CLASS_WY_NAME				TEXT("网页游戏")
#define CLASS_DZ_NAME				TEXT("对战游戏")
#define CLASS_QP_NAME				TEXT("棋牌游戏")
#define CLASS_PL_NAME				TEXT("游戏插件")
#define CLASS_LT_NAME				TEXT("聊天工具")
#define CLASS_CY_NAME				TEXT("常用工具")
#define CLASS_YY_NAME				TEXT("影音工具")
#define CLASS_GP_NAME				TEXT("股票证券")


//虚拟盘监听的网络端口的基数
#define VDISK_PORT_BASE				17918			// (17918 + 'Driver' - 'A')


//刷新虚拟盘的事件名称
#define VDISK_EVENT_NAME	TEXT("__i8desk_vdisk_refresh_event_")

#define    DEFAULT_SKIN_NAME          TEXT("默认.png")                                     //默认皮肤
#define    PATH_ROOT               	  TEXT("%root%")			                           //表示应用程序安装目录
#define    PATH_SYSTEM		          TEXT("%system%")			                           //表示系统目录
#define    PATH_PROGRAM	              TEXT("%program%")			                           //表示C:\program file\\目录


//定义系统选项
//用户登陆验证相关的选项(系统选项客户端插件需要写到LOCAL_MACHINE\SOFTWARE\Goyoo\i8desk下的值)
#define OPT_U_CPUID					TEXT("cpuid")			//硬件id
#define OPT_U_USERNAME				TEXT("username")		//用户名
#define OPT_U_PASSWORD				TEXT("password")		//密码				//不写,密码是原文
#define OPT_U_OAU1					TEXT("oau1")			// OAU1
#define OPT_U_OAU2					TEXT("oau2")			// OAU2
#define OPT_U_NBNAME				TEXT("nbname")			//网吧名称:显示到网吧名称的第一行
#define OPT_U_NBTITLE				TEXT("nbtitile")		//网吧标题:显示到网吧名称的第二行。
#define OPT_U_NBSUBTITLE			TEXT("nbsubtitile")		//网吧子标题：显示到网吧名称的第三行
#define OPT_U_NBTITLEURL			TEXT("nbtitileurl")		//网吧网址：如果空为，则点周网吧名称后显示公告，否则弹出指定的网址

#define OPT_U_PROVINCE				TEXT("nbprovince")		//网吧所在省
#define OPT_U_CITY					TEXT("nbcity")			//网吧所在市
#define OPT_U_PROVINCEID			TEXT("nbprovinceid")	//网吧所在省ID
#define OPT_U_CITYID				TEXT("nbcityid")		//网吧所在市ID

#define OPT_U_NBADDRESS				TEXT("nbaddress")		//网吧地址：真实的网吧地址
#define OPT_U_NBPHONE				TEXT("nbphone")			//网吧电话（座机）
#define OPT_U_NBMOBILE				TEXT("nbmobile")		//网吧移动电话（手机）
#define OPT_U_REALITYURL			TEXT("nbrealityurl")	//网吧实名修改网址

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
#define OPT_U_ERRINFO				TEXT("ErrInfo")			//用于控制台显示登陆的错误信息。//不写.
#define OPT_U_REGDATE				TEXT("regdate")			//用户注册日期

#define OPT_U_CONSHOWDIALOG			TEXT("conpopdialog")	//中心下发给控制台的信息:是否弹出对话框(1|0)
#define OPT_U_CONSHOWCANCEL			TEXT("conshowcancel")	//中心下发给控制台的信息:是否显示取消按钮
#define OPT_U_CONMESSAGE			TEXT("conmessage")		//中心下发给控制台的信息:信息内容
#define OPT_U_CONOKURL				TEXT("conokurl")		//中心下发给控制台的信息:点确定后弹出的网址

#define OPT_S_BACKUPDIR				TEXT("sbackupdir")		//服务端备份目录.
#define OPT_S_BACKUPNUM				TEXT("sbackupnum")		//保留备份文件的个数.
#define OPT_S_BACKUPDATE			TEXT("sbackupdate")		//开始备份的时间.
#define OPT_S_FLUSH_DB_INTERVAL     TEXT("sflushdbinterval")//刷新缓存到数据库的间隔时间
#define OPT_S_FLUSH_DB_OPNUMBER     TEXT("sflushdbopnumber")//刷新缓存到数据库的累积操作条数

//服务端三层选项
#define OPT_D_INITDIR				TEXT("dinitdir")		//三层游戏下载的默认目录
#define OPT_D_TEMPDIR				TEXT("dtempdir")		//三层游戏下载临时目录
#define OPT_D_LINETYPE				TEXT("dlinetype")		//网络线路类型 (0表示电信,1表示网通)
#define OPT_D_JOINTYPE				TEXT("djointype")		//网络宽带类型 (0表示ADSL,1表示光纤)
#define OPT_D_TASKNUM				TEXT("dtasknum")		//同时下载的最大任务数 (n)
#define OPT_D_CONNNUM				TEXT("dconnnum")		//最大连接数 (n)
#define OPT_D_ADSLSPEED				TEXT("dADSLpeed")		//ADSL三层下载的速度限制(n)(k/s)
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
#define OPT_M_SHOWGONGGAOTYPE		TEXT("mshowgonggaotype")//客户端显示公告的方式（0：图片公告；1：公告文字 2:公告和文字）
#define OPT_M_GGMSG					TEXT("mggmsg")			//客户端公告信息
#define OPT_M_GGFONT				TEXT("mggfont")			//菜单公告字体 (字体名|大小|颜色|标志(bit0:粗体，bit1:斜线,bit2:下画线,bit3:删除线))
#define OPT_M_TSSPEED				TEXT("mtsspeed")		//推送速度
#define OPT_M_SYNTIME				TEXT("msyntime")		//同步时间选项
#define OPT_M_SAFECENTER			TEXT("msafecenter")		//是否启用安全中心(0|1)
#define OPT_M_WHICHONE_SC			TEXT("mwhichonesc")		//启用哪个安全中心(1：i8,2:康禾)
#define OPT_M_IEHISTORY				TEXT("miehistory")		//是否启用IE历史记录(0|1)
#define OPT_M_USEUDISK				TEXT("museudisk")		//是否启用U盘(0|1)
#define OPT_M_MUSIC					TEXT("mmusic")			//强推的音乐网站 (名字|URL)
#define OPT_M_VIDEO					TEXT("mvideo")			//强推的电影网站 (名字|URL)
#define OPT_M_GGONOFF				TEXT("madonoff")		//控制公告显示与否(p1$n(1,表示强制显示，0表示强制不显示，-1按网吧设置的来显示.),p2$n(保留)).
#define OPT_M_RPOPONOFF				TEXT("mrpoponoff")		//菜单右下角弹出广告控制(p1$n(1,表示打开，0表示关闭),p2$n(n表示显示的机率)).
#define OPT_M_BOTLAYER				TEXT("mbotlayer")		//菜单底部浮动广告p1$n(1,表示打开,0表示关闭),p2$n(保留)
#define OPT_M_MENUPAGE				TEXT("mmenupage")		//菜单默认首页p1$n(1|0表示开关),p2$n(
#define OPT_M_WHICHONESC            TEXT("mwhichonesc")     //启用哪一个安全中心
#define OPT_M_USERDATADIR			TEXT("muserdatadir")	//个人磁盘的自定义保存目录
#define OPT_M_CLASSSHUT             TEXT("ClassShortcuts")  //目录快捷方式
#define OPT_M_NETBARJPG             TEXT("netbarjpgdir")	//客户端显示网吧外观图片
#define OPT_M_GGPICDIR				TEXT("mggpicdir")		//客户端公告图片信息路径
#define OPT_M_CPU_TEMPRATURE		TEXT("cputmprature")	// CPU监测温度
#define OPT_M_FIRSTTIME				TEXT("firstTime")		// 首次安装时间

/*
1001  游戏大厅-热门游戏
1002  游戏大厅-网络游戏
1003  游戏大厅-单机游戏
1004  游戏大厅-休闲游戏
1005  游戏大厅-对战游戏
1006  游戏大厅-棋牌游戏
2001  聊天/软件-聊天工具
3001  网页浏览-网址导航
*/
#define OPT_M_DOTCARD				TEXT("mdotcard")		//中心控制的点卡商城地址。
#define OPT_M_URLNAV				TEXT("murlnav")			//中心控制的网址导航列表
#define OPT_M_VDMUS					TEXT("mvdmus")			//中心控制的影视音乐列表
#define OPT_M_PERSONURL				TEXT("mpersonurl")		//中心控制的个人用户中心网址
#define OPT_M_GAMEROOMURL			TEXT("mgameroomurl")	//游戏大厅后置网址列表


#define OPT_M_GAMEDRV				TEXT("mgamedrv")		//默认本地游戏存放的盘符.('E')
#define OPT_M_P2P					TEXT("mp2p")			//本地更新是否启用p2p.(1|0)
#define OPT_M_INPROTTYPE			TEXT("minprottype")		//本地更新穿透还原的方式.如果是0表示立即穿透.否则是穿透的速度

#define OPT_M_SAFECENTER			TEXT("msafecenter")		//是否启用安全中心(0|1)
#define OPT_M_WHICHONE_SC			TEXT("mwhichonesc")		//启用哪个安全中心(1：i8,2:康禾)
#define OPT_M_IEHISTORY				TEXT("miehistory")		//是否启用IE历史记录(0|1)

#define OPT_M_EXCLUDEDIR			TEXT("mexcludedir")		//磁盘清理时要排除的目录(用|分隔)
#define OPT_M_USEUDISK				TEXT("museudisk")		//是否启用U盘(0|1)

#define OPT_M_CMDMASK				TEXT("gamemgrcmdmask")	//游戏管理插件命令屏蔽选项

#define OPT_M_PENETATEFILE			TEXT("PenetateFile")	//开机穿透的目录对(用|分隔)

#define OPT_M_LASTTJTIME			TEXT("LastTJTime")		//上次体检时间
#define OPT_M_LASTJKZS				TEXT("LastJKZS")		//上次健康指数
#define OPT_M_GAMEVERSION			TEXT("GameVersion")		// 单机游戏版权提示

#define OPT_M_LOCALPLUG				TEXT("localplug")		//是否启用本地插件(0|1)
#define OPT_M_CREATEDAY				TEXT("nbCreateday")		// 网吧创建的天数
#define OPT_M_DISKTYPE				TEXT("disktype")		// 磁盘类型 1:无盘 2:有盘 3:混合
#define OPT_M_DISKPRODUCTNAME		TEXT("diskproductname")	// 无盘产品名称
#define OPT_M_FEEPRODUCTNAME		TEXT("feeproductname")	// 收费产品名称
#define OPT_M_CULTURALNAME			TEXT("culturalcontrolproductname") // 安全监控软件名称
#define OPT_M_BROADBANDTYPE			TEXT("broadbandtype")	// 网络状况1:其它 2:ADSL 3:光纤
#define OPT_M_REALITY				TEXT("reality")			// 是否已通过实名 1:是 0:否
#define OPT_D_SELSYNCTASKALL		TEXT("isSelSyncTaskAll")// 是否默认选中所有同步任务  1:是 0:否

//定义通信命令
#define PRO_VERSION					0x0303		//表示通讯协议版本号packageheader.version.
#define CMD_RET_SUCCESS				1			//命令应答成功
#define CMD_RET_FAIL				0			//命令应答失败

#define I8DESK_INVALID_CMD			0x0000		//无意义的命令
#define CMD_CON_HEART_BEAT			0x0001		//控制台心跳报文
#define CMD_RELOAD_CACHE_FILE		0x0002		//重新加载缓存文件

// 客户机相关使用命令
#define CMD_GAME_REPCLICK			0x0103		//上报游戏点击数,菜单启动一次
#define CMD_GAME_ICONREFRESH		0x0105		//图标缓存失效
#define CMD_GAME_CLIENTTOCON		0x0108		//客户端发送数据给控制台
#define CMD_GAME_REQ_ICONS_IDX		0x0121		//请求游戏图标索引
#define CMD_GAME_RSP_ICONS_IDX		0x0122		//返回游戏图标索引
#define CMD_GAME_REQ_ICONS_DATA		0x0123		//请求游戏图标数据
#define CMD_GAME_RSP_ICONS_DATA		0x0124		//返回游戏图标数据
#define CMD_GAME_GETIDXS	        0x0130		//取游戏的索引列表
#define CMD_GAME_GETSOME	        0x0131		//取指定的游戏列表信息 2010版本使用
#define CMD_SYSOPT_SYNTIME			0x0302		//得到服务端的同步时间
#define CMD_SYSOPT_GETLIST			0x0301		//得到系统选项
#define CMD_VDISK_GETLIST			0x0201		//得到虚拟盘列表
#define CMD_FAVORITE_GETLIST		0x0401		//得到收藏夹列表
#define CMD_BOOT_GETLIST			0x0501		//得到开机任务列表
#define CMD_BOOT_DELLIST			0x0502		//删除开机任务列表
#define CMD_GET_SERVER_CONFIG		0x0505		//获取多服务器配置
#define CMD_SVRRPT_SERVERINFO		0x0603		//上报服务器信息（旧的命令）
#define CMD_SVRRPT_SERVERINFO_EX	0x0604		//上报服务器信息扩展
#define CMD_CLIUPT_DOWNFILEINFO		0x0907		//下载文件前先取得文件的CRC
#define CMD_CLIUPT_DOWNFILE_START	0x0908		//请求下载文件
#define CMD_DOWNFILE_NEXT			0x0909		//请求文件后续数据
#define CMD_GETCIENTVERSION			0x0d04		//（自动升级请求）
#define CMD_CLIENT_REMOTECONTROLSVR	0x0d06		// 客户端远程控制主服务器
#define CMD_PLUGTOOL_GETIDXS		0x0d07		// 取插件工具的索引列表
#define CMD_PLUGTOOL_GETSOME		0x0d08		// 取指定的插件工具列表信息
#define CMD_BUSINISS_GETGAMEEXVER	0x0d09		// 取BUSINISSS插件游戏版本信息
#define CMD_REPORT_SVRIP			0x0d10		// 上报链接服务器IP
#define CMD_CLIENT_GETDELGAME		0x0d11		// 开机取得删除客户机文件
#define CMD_CLIENT_RPTDELGAMESTATUS 0x0d12		// 上报客户机删除文件是否成功

// 菜单到客户端服务再到主服务命令
#define CMD_BUSINESS_INFOEX			0x0d05		// 取扩展信息

//控制台到缓存相关使用命令
#define CMD_GAME_CONTOCLIENT		0x0107		//插件转发控制台的数据给客户端
#define CMD_CON_GAME_ICON			0x0109		//图标操作
#define CMD_VALIDATE				0x010C		//通知主服务器去立即验证
#define CMD_CON_GETINFO_FROM_GAMEUPDATESVR  0x010E	//查看游戏信息
#define CMD_CON_NOTIFY_REFRESH_PL	0x010F		//控制台通知主服务器插件去刷新pl
#define CMD_CON_GETSVR_STATUS		0x0111		// 控制台获取主服务所有信息
#define CMD_CON_SETFILE_CLASSICON	0x0112		// 控制台回写类别图标数据
#define CMD_CON_GET_VDISKCLIENT		0x0113		// 控制台得到虚拟盘客户端数据
#define CMD_CON_OFFLINE_NOTIFY		0x010D		//控制台下线通知 
#define CMD_GAME_CONEXESQL	        0x0125		//控制台执行SQL
#define CMD_CON_SAVE_HARDWARE		0x0126		// 控制台保存客户机硬件信息
#define CMD_CON_SMART_CLEAN			0x0127		// 控制台请求进行智能清理
#define CMD_CON_MONITOR_DISK_SIZE	0x0128		// 控制台请求硬盘空间是否足够
#define CMD_CON_SYNCTASK_OPERATE	0x0129		// 控制台请求操作同步任务

#define CMD_CON_GET_BARSKINS		0x0503		//请求菜单皮肤列表
#define CMD_NOTIFY_SERVER_MEDICAL	0x0c03		//通知服务重新体检
#define CMD_NOTIFY_SAVE_DATA		0x0c04		// 通知服务把缓存组件数据写到数据库
#define CMD_CON_PLUGTOOL_ICON		0x0c05		// 控制台插件中心获取插件图标
#define CMD_CON_PLUGTOOL_OPERATE	0x0c06		// 控制台插件中心操作插件命令
#define CMD_CON_MONITOR_STATUS		0x0c07		// 控制台监控首页统计信息命令
#define CMD_CON_STATISTIC_STATUS	0x0c08		// 控制台统计游戏与客户机信息命令
#define CMD_CON_MONITOR_DELETEFILE	0x0c09		// 控制台监控首页清除文件命令
#define CMD_CON_GETALL_GAMEICONS	0x0c10		// 控制台获取所有游戏图标
#define CMD_CON_GETALL_CLASSICONS	0x0c11		// 控制台获取所有类别图标
#define CMD_CON_REPORT_USERINFOMODIFY 0x0c12	// 控制台报告用户信息被修改

//控制台到三层相关使用命令
#define CMD_TASK_OPERATION			0x010A		//三层操作
#define CMD_TASK_STATE_CHANGED		0x010B		//三层任务状态
#define CMD_CON_NOTIFY_DELETEFILE	0x015D		//删除游戏文件 
#define CMD_PLUG_TASK_COMPLATE		0x015E		// 插件下载完成
#define CMD_ADD_NEW_SERVER			0x015F		// 新增从服务器
#define CMD_REFRESHPL				0x0160		// 刷新PL

// 缓存检测客户机硬件变化向控制台发起UDP通知
#define CMD_CLIENT_HARDWARECHANGE	0xd15E		// 客户机硬件发生改变通知

//上报组件到缓存相关使用命令
//日志时间(dword)+日志类型(dword)+日志内容(string)+插件名(string)
#define CMD_PLUGIN_LOG_REPORT		0x0110		//插件报告运行日志
#define CMD_CLIRPT_CLIENTINFO		0x0601		//service启动时上报客户端详细信息 
#define CMD_CLIRPT_HARDWARE			0x0602		//上报客户机硬件信息 
#define CMD_CHECKUSER_NOTIFY		0x0605		// 验证结果通知

//插件中心到缓存的使用命令
#define CMD_PLUGTOOL_OPERATE		0x0d01		// 插件中心操作命令

//客户机与虚拟盘到缓存使用命令
#define CMD_GET_VDISK_CONFIG		0x0506		//获取多服务器的虚拟盘配置
#define CMD_VDSVR_REPSTATUS			0x0a02		//虚拟盘报告状态
#define CMD_VDSVR_GETUPDATE_FILEINFO	0x0a03	// 获取（虚拟盘节点服务器）需要升级文件
#define CMD_VDSVR_GETUPDATE_FILELIST	0x0a04	// 获取（虚拟盘节点服务器）需要升级文件列表
#define CMD_VDSVR_VDISKCLIENT			0x0a05	//虚拟盘报告客户机连接信息

//兼容老版本的命令
#define CMD_GAME_GETLIST			0x0101		//得到游戏列表 2009版本使用
#define CMD_GAME_GETICONLIST		0x0102		//得到游戏图标压缩包(10版使用)
#define CMD_GAME_CONGETSTAUTS		0x0106	    //控制台取状态信息
#define CMD_REP_BARSKINS			0x0504		//返回菜单皮肤列表

#define CMD_USER_REGISTER			0x0701		//用户注册
#define CMD_USER_LOGIN				0x0702		//用户登陆
#define CMD_USER_MODPWD				0x0703		//修改密码

#define CMD_USER_UPLOAD				0x0704		//数据上传	上传，下载会分块
#define CMD_USER_DOWNLOAD			0x0705		//数据下载
#define CMD_USER_DELETE				0x0706		//删除文件
#define CMD_USER_STATE_CHANGE		0x0707		// 客户机上下线通知
#define CMD_USER_DOWNLOAD_START		0x0708		//请求下载文件

#define CMD_CLIUPT_GETLIST			0x0901		//得到总版本号.
#define CMD_CLIUPT_DOWNFILE			0x0902		//下载文件，首先下载一个特殊文件
#define CMD_CLIUPT_UPLOAD			0x0903		//上传
#define CMD_CLIUPT_GET_FILELIST		0x0904		//得到盘符，目录或者文件列表
#define CMD_CLIUPT_DELETE			0x0905
#define CMD_CLIUPT_UPDATE			0x0906		//通知客户端更新的版本号.
#define CMD_CLIUPT_GET_ALL_CLIENTS	0x0910		// 获取所有的客户端

#define CMD_VDSVR_GETLIST			0x0a01		//虚拟盘服务器得到虚拟盘列表

#define CMD_SYNCTOOL_TASKSTATE		0x0b01		//通知同步工具三层下载状态
#define CMD_SYNCTOOL_ADD_GAME		0x0b02		//同步工具通知服务添加游戏

#define CMD_UPT_ICONLIST_VERSION	0x0c01		//通知服务更新图标列表的版本号
#define CMD_UPT_CLIENT_INDEX		0x0c02		//通知服务更新客户软件包的索引



//End of command define
///////////////////////////////////////////////////////////////////////////////////////

//每包最多能传送的游戏数目
static const DWORD MAX_COUNT_GAME_PER_PACKEG = 10;

//每包最多能传送的图标数目
static const DWORD MAX_COUNT_ICON_PER_PACKEG = 10;


#ifndef BitN
#define BitN(x) (1<<x) 
#endif

//由控制台和服务端使用的常量，控制台的SQL中如果修改了表则设置表相应的位
#define FLAG_RUNTYPE_BIT			     BitN(0)
#define FLAG_VDAREA_BIT					 BitN(1)
#define FLAG_AREA_BIT					 BitN(2)
#define FLAG_CLASS_BIT					 BitN(3)
#define FLAG_CLIENT_BIT					 BitN(4)
#define FLAG_CMPBOOTTASK_BIT			 BitN(5)
#define FLAG_FAVORITE_BIT				 BitN(6)
#define FLAG_GAME_BIT					 BitN(7)
#define FLAG_GAMELOG_BIT				 BitN(8)
#define FLAG_SYSOPT_BIT					 BitN(9)
#define FLAG_USER_BIT					 BitN(10)
#define FLAG_VDISK_BIT					 BitN(11)
#define FLAG_SYNCTASK_BIT				 BitN(12)
#define FLAG_SERVER_BIT					 BitN(13)
#define FLAG_BOOTTASKAREA_BIT			 BitN(14)
#define FLAG_ICON_BIT					 BitN(15)
#define FLAG_MODULEUSAGE_BIT			 BitN(16)
#define FLAG_CLIENTSTATUS_BIT			 BitN(17)
#define FLAG_VDISKCLIENT_BIT			 BitN(18)
#define FLAG_VDISKSTATUS_BIT			 BitN(19)
#define FLAG_SERVERSTATUS_BIT			 BitN(20)
#define FLAG_DISKSTATUS_BIT				 BitN(21)
#define FLAG_TASKSTATUS_BIT				 BitN(22)
#define FLAG_UPDATEGAMESTATUS_BIT		 BitN(23)
#define FLAG_PLUGTOOL_BIT				 BitN(24)
#define FLAG_PLUGTOOLSTATUS_BIT			 BitN(25)
#define FLAG_PUSHGAMESTATUS_BIT			 BitN(26)
#define FLAG_SYNCGAMESTATUS_BIT			 BitN(27)
#define FLAG_SYNCGAME_BIT				 BitN(28)
#define FLAG_SYNCTASKSTATUS_BIT			 BitN(29)
#define FLAG_PUSHGAMESTATIC_BIT			 BitN(30)


#define SQL_OP_NONE						 0x00
#define SQL_OP_SELECT					 0x01
#define SQL_OP_INSERT					 0x02
#define SQL_OP_UPDATE					 0x04
#define SQL_OP_DELETE					 0x08
#define SQL_OP_ENSURE					 0x10
#define SQL_OP_COMULATE					 0x20

#endif