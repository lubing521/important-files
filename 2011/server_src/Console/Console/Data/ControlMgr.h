#ifndef __CONSOLE_CONTROL_MANAGER_HPP
#define __CONSOLE_CONTROL_MANAGER_HPP

#include "DataHelper.h"
#include <vector>


namespace i8desk
{
	// forward declare
	class NetworkDataMgr;

	namespace manager
	{
		// --------------------------
		// class ControlMgr
		
		class ControlMgr
		{
			typedef std::tr1::function<void(const stdex::tString &)>	ErrorCallback;
			typedef NetworkDataMgr::AutoLock							AutoLock;

		private:
			NetworkDataMgr	*network_;
			ErrorCallback	errorCallback_;

		public:
			explicit ControlMgr(NetworkDataMgr *network);
			~ControlMgr();

		private:
			ControlMgr(const ControlMgr &);
			ControlMgr &operator=(const ControlMgr &);

		public:
			void RegisterError(const ErrorCallback &errorCallback);

		public:
			// 安装还原，并保护所有分区.
			bool InstallProt(const std::vector<DWORD>& list, bool bProtAllArea);
			// 启用安全中心
			bool EnableSafeCenter(const std::vector<DWORD>& list);
			// 启用IE保护
			bool EnableIEProt(const std::vector<DWORD>& list);
			// 启用防狗驱动
			bool EnableDogProt(const std::vector<DWORD>& list);
			// 删除还原
			bool RemoveProt(const std::vector<DWORD>& list);
			// 保护分区
			bool ProtArea(const std::vector<DWORD>& list, const stdex::tString &ProtArea, const stdex::tString &UnProtArea);
			// 关机或者重启
			bool ShutDown(const std::vector<DWORD>& list, bool bReBoot);
			// 实时推送游戏
			bool RealPush(const std::vector<DWORD>& list, const stdex::tString &strGids, DWORD dwLimitSpeed, DWORD dwConstructWay);
			// 执行一个EXE文件
			bool ExecExeFile(const std::vector<DWORD>& list, const stdex::tString &szFile);
			// 检查磁盘
			bool Chkdsk(const std::vector<DWORD>& list, const stdex::tString &strArea);
			// 客户机信息查看
			bool ViewClientInfo(const std::vector<DWORD>& list, DWORD flag, stdex::tString &text);
			// 停止服务或者结束进程
			bool KillProcSvr(const stdex::tString &file,  DWORD ip, bool bIsProcess);
			// 远程控制
			bool RemoteControl(DWORD ip, int bstart);
			// 得到盘符，目录或者文件列表
			bool GetFileOrDir(const stdex::tString& szDir, DWORD nType, bool IsAbsPath, 
							 std::vector<stdex::tString>& ListFiles);
			// 获取文件
			bool GetFileData(const stdex::tString &dir, data_helper::Buffer &data);
			// 保存文件
			bool SaveFileData(const stdex::tString &dir, data_helper::Buffer &data);

			// 获取所有游戏图标
			bool GetGameIcons(data_helper::Buffer &data);
			// 获取所有类别图标
			bool GetClassIcons(data_helper::Buffer &data);

			// 添加ICON
			bool AddIcon(DWORD gid, unsigned char* pData, size_t size);

			// 删除ICON
			bool DelIcon(DWORD gid);

			// 获取ICON
			bool GetIcon(DWORD gid, unsigned char *data, size_t &size);

			// 通知缓存组件写数据库
			bool NotifySaveData();

			// 控制PlugTool
			bool PlugToolOperate(long gid, DWORD op);

			// 获取PlugTool图标
			bool GetPlugIcon(long gid, unsigned char *data, size_t &size);

			// 删除三层下载临时文件
			bool DeleteTmpFiles();

			// 获取游戏所在客户机
			bool GetViewGame(DWORD param, data_helper::ViewGameTraits::VectorType &viewGames);

			// 获取客户机所包含游戏
			bool GetViewClient(DWORD param, data_helper::ViewClientTraits::VectorType &viewClients);

			// 验证用户名及密码
			bool CheckUser(const stdex::tString &userName, const stdex::tString &psw);
		
			// 通知修改网吧信息
			bool ModifyNetbarInfo();

			// 保存客户机硬件信息
			bool SaveHardware();

			// 通知重载文件信息
			bool NotifyReloadFile(const stdex::tString &path);

			// 通知服务器删除智能清理文件
			bool NotifySmartClean(const std::vector<long> &gids);

			// 判断硬盘空间是否足够下载
			bool NeedSmartClean(const stdex::tString &path, const ulonglong totalSize, bool &isEnough);
			// 同步任务操作
			bool SyncTaskOperate(int cmd, long gid, const stdex::tString &SID, const stdex::tString &TaskName, const bool &isDelFile = false);
		};
	}
}



#endif