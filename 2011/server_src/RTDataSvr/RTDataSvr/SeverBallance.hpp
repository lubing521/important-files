#ifndef __SERVER_BALANCE_HPP
#define __SERVER_BALANCE_HPP

#include "../../../include/Container/SyncContainer.hpp"
#include <tuple>
#include <set>
#include <vector>

namespace i8desk
{
	class ZTableMgr;

	namespace ballance
	{

		// 服务器信息
		struct ServerInfo;
		typedef std::tr1::shared_ptr<ServerInfo> ServerInfoPtr;

		// 服务器负载分配
		class ServerLoadBalance
		{
		public:
			typedef std::pair<ulong, std::set<ulong>>	ServerIP;
			typedef std::vector<ServerIP>				ServersIP;
			typedef async::container::SyncSequenceContainer<ServerInfoPtr> ServerInfos; 

		private:
			// 服务器信息
			ServerInfos svrInfos_;	
			ZTableMgr *rtData_;

			std::auto_ptr<i8desk::IServerEventHandler> svrHandler_;
			std::auto_ptr<i8desk::IServerStatusEventHandler> svrStatusHandler_;

		public:
			explicit ServerLoadBalance(ZTableMgr *rtData);

		public:
			void Start();
			void Stop();
			
			
			void UpdateConnect(DWORD flag, ulong svrIP, ulong clientIP);
			void UpdateDisconnect(ulong clientIP);

			void GetServersIPs(const stdex::tString &cliName, ServersIP &serversIP);
			std::pair<size_t, size_t> GetLoadCount(const stdex::tString &srvID) const;

		private:
			ServerIP _AvgLoading();
			ServerIP _AreaLoading(const stdex::tString &cliName);
			std::tr1::shared_ptr<std::vector<ServerIP>> _DynamicLoading();

			void _EraseClientIP(DWORD flag, ulong clientIP);

		};
	}

}



#endif