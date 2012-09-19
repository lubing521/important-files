#ifndef __MONITOR_MANAGER_HPP
#define __MONITOR_MANAGER_HPP

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/FileSystem/FileMonitor.hpp"

#include "../../Common/MemoryPool.hpp"


namespace recvSvr
{

	using namespace async;
	// -------------

	class MonitorMgr
	{
		std::wstring path_;
		iocp::IODispatcher &io_;
		async::thread::AutoEvent exit_;
		
		typedef std::tr1::function<void (const common::BmpInfo &)> Callback;
		Callback callback_;

		mutable volatile long isPaused_;
		mutable volatile long isOK_;

	public:
		MonitorMgr(iocp::IODispatcher &io, const std::wstring &monitorPath, const Callback &callback)
			: path_(monitorPath)
			, io_(io)
			, callback_(callback)
			, isPaused_(0)
			, isOK_(1)
		{}

		~MonitorMgr()
		{

		}

	public:
		void Start();
		void Stop();

		bool IsOK() const
		{
			return ::InterlockedExchangeAdd(&isOK_, 0) == 1;
		}
		bool IsPaused() const
		{
			return ::InterlockedExchangeAdd(&isPaused_, 0) == 1;
		}

		void Pause()
		{
			::InterlockedExchange(&isPaused_, 1);
		}

		void Resume()
		{
			::InterlockedExchange(&isPaused_, 0);
		}

	private:
		void _Handle();
	};
}





#endif