#ifndef __HISTORY_MONITOR_MANAGER_HPP
#define __HISTORY_MONITOR_MANAGER_HPP

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/FileSystem/FileMonitor.hpp"

#include "../../Common/MemoryPool.hpp"


namespace recvSvr
{

	using namespace async;
	// -------------

	class HistoryMonitor
	{
		std::wstring path_;
		iocp::IODispatcher &io_;
		async::thread::AutoEvent exit_;

		typedef std::tr1::function<void (const common::BmpInfo &)> Callback;
		Callback callback_;


	public:
		HistoryMonitor(iocp::IODispatcher &io, const std::wstring &monitorPath, const Callback &callback)
			: path_(monitorPath)
			, io_(io)
			, callback_(callback)
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