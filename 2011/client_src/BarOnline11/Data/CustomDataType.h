#ifndef __BARONLINE_CUSTOM_DATA_TYPE_HPP
#define __BARONLINE_CUSTOM_DATA_TYPE_HPP

#include "../../../include/tablestruct.h"

namespace i8desk
{

	namespace data
	{
		// 收藏夹信息
		struct Favorite
		{
			stdex::tString Type;			// 类型
			stdex::tString Name;			// 名字
			stdex::tString Url;				// 网址
		};


		// 游戏信息
		struct GameInfo
		{

			DWORD				Gid;			// 游戏id.
			DWORD				Pid;			// 游戏关联id.
			stdex::tString		Name;			// 游戏名称
			stdex::tString		IdcClass;		// 游戏类别
			stdex::tString		Exe;			// 游戏运行的ＥＸＥ.(相对路径)
			stdex::tString		Param;			// 运行的参数
			stdex::tString		Py;				// 游戏名称的拼音
			stdex::tString		MultiPy;        // 多音字拼音
			size_t				FindPos;		// 匹配的位置
			DWORD				Size;			// 游戏的大小（以Ｋ为单位）
			DWORD				DeleteFlag;     // 游戏空间不足时允许删除
			bool				DeskLnk;		// 是否显示到桌面快捷方式
			bool				Toolbar;		// 是否显示到菜单工具栏
			stdex::tString		SvrPath;		// 游戏服务端路径
			stdex::tString		CliPath;		// 游戏客户端路径
			stdex::tString		MatchFile;		// 游戏自动搜索特征文件
			stdex::tString		SaveFilter;		// 游戏存档信息
			DWORD				I8Play;			// 增值活动游戏
			DWORD				IdcClick;		// 游戏全网点率
			DWORD				SvrClick;		// 游戏网吧点率
			DWORD				RunType;		// 游戏运行方式
			DWORD				NetbarVer;      // 网吧服务器版本
			stdex::tString		VID;			// 游戏所在虚拟盘ID
			stdex::tString      GameArea;       // 专区URL
			stdex::tString      RegUrl;         // 注册地址 
			stdex::tString      PayUrl;         // 支付地址
			stdex::tString      VideoUrl;       // 视频攻略

			utility::ICONPtr	hIcon;			// 游戏图标
		};

		// 虚拟盘信息
		struct VDiskInfo
			: public db::tVDisk
		{

		};

		// 多服务器列表配置
		struct SvrCfg
			: public db::tServer
		{
			DWORD	SyncType;				// 同步方式
		};

		// 图标数据
		struct IconInfo
			: public db::tIcon
		{

		};
	}


}






#endif