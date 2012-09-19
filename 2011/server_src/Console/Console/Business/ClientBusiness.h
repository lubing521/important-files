#ifndef __CLIENT__BUSINESS_HPP
#define __CLIENT__BUSINESS_HPP

#include "../Data/DataHelper.h"
#include "../ui/DlgClientSystemInfo.h"

namespace i8desk
{
	namespace business
	{

		namespace Client
		{

			typedef std::tr1::function<bool()> Callback;

			// 根据IP得到客户几名
			stdex::tString GetNameByIP(ulong ip);

			// 网络唤醒
			void ControlStart(const data_helper::ClientTraits::VectorType &clients);

			// 重启客户机
			void ControlRestart(const data_helper::ClientTraits::VectorType &clients);

			// 关闭客户机
			void ControlShutdown(const data_helper::ClientTraits::VectorType &clients);

			// 磁盘检测
			void ControlDiskRecorver(const data_helper::ClientTraits::VectorType &clients, const stdex::tString &areas);

			// 执行客户机文件
			void ControlRunClientFile(const data_helper::ClientTraits::VectorType &clients,  const stdex::tString &path);

			// 安装全盘保护还原
			void ControlInstallAll(const data_helper::ClientTraits::VectorType &clients);

			// 安装还原
			void ControlInstall(const data_helper::ClientTraits::VectorType &clients);

			// 分区保护还原
			void ControlProtoArea(const data_helper::ClientTraits::VectorType &clients, const stdex::tString &protAreas, const stdex::tString &unProtAreas);

			// 卸载还原
			void ControlRestore(const data_helper::ClientTraits::VectorType &clients);

			// 查看系统信息
			void ControlViewSystem(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text);

			// 查看进程信息
			void ControlViewProcess(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text);

			// 查看服务信息
			void ControlViewService(const data_helper::ClientTraits::VectorType &clients, stdex::tString &text);

			// 查看游戏信息
			void ControlViewGames(const data_helper::ClientTraits::VectorType &clients);

			// 远程控制
			void ControlRemoteControl(const data_helper::ClientTraits::VectorType &clients);

			// 远程查看
			void ControlRemoteView(const data_helper::ClientTraits::VectorType &clients);

			// 刷新客户机
			void ControlRefresh(const data_helper::ClientTraits::VectorType &clients);
			
			// 结束进程/停止服务
			void ControlKillProcess(const stdex::tString &file, const DWORD clientIP, bool IsProcess );
			
			// 分区过滤
			void AreaFilter(data_helper::ClientTraits::VectorType &curClients, const stdex::tString &AID);

			// 分区过滤
			void AreaRealFilter(data_helper::ClientTraits::VectorType &sClients, data_helper::ClientTraits::VectorType &dClients, const stdex::tString &AID);

			
			// 解释系统信息
			void PraseViewSystem( CString &text );
			
			// 解释进程信息
			void PraseViewProcess( std::vector<ListVector> &listVector ,const stdex::tString &Info);
			
			// 解释服务信息
			void PraseViewService( std::vector<ListVector> &listVector ,const stdex::tString &Info );

			// 得到未保护分区
			void GetUnProtArea(const CString &allAreas, const CString &protAreas, stdex::tString &unprotAreas);
		
			// 格式化分区
			void ParseProtArea(const stdex::tString &clientName,  std::vector<stdex::tString> &partiions);
			
			// 得到保护分区
			void GetProtArea( const stdex::tString &Partition, stdex::tString &protarea);
		}
	}
}

#endif