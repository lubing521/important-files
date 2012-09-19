#ifndef __HISTORY_MONITOR_MANAGER_HPP
#define __HISTORY_MONITOR_MANAGER_HPP

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/FileSystem/FileMonitor.hpp"

#include "../../Common/MemoryPool.hpp"


namespace sendsvr
{

	using namespace async;
	// -------------

	class HistoryMonitor
	{
		std::wstring monitorPath_;
		std::wstring ftpIP_;
		u_short ftpPort_;
		std::wstring ftpUserName_;
		std::wstring ftpPassword_;

		iocp::IODispatcher &io_;
		async::thread::AutoEvent exit_;

	public:
		HistoryMonitor(iocp::IODispatcher &io, const std::wstring &monitorPath, 
			const std::wstring &ftpIP, u_short ftpPort, const std::wstring &ftpUserName, const std::wstring &ftpPassword)
			: monitorPath_(monitorPath)
			, io_(io)
			, ftpIP_(ftpIP)
			, ftpPort_(ftpPort)
			, ftpUserName_(ftpUserName)
			, ftpPassword_(ftpPassword)

		{}

		~HistoryMonitor()
		{

		}

	public:
		void Start();
		void Stop();

	private:
		void _Handle();
	};
}





#endif