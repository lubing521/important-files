#ifndef __HISTORY_PLAY_HPP
#define __HISTORY_PLAY_HPP

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/FileSystem/FileMonitor.hpp"

#include "../../Common/MemoryPool.hpp"


namespace recvSvr
{

	using namespace async;
	// -------------

	class HistoryPlay
		: public std::tr1::enable_shared_from_this<HistoryPlay>
	{
		std::wstring i_path_;
		std::wstring j_path_;

		size_t i_freq_;
		size_t j_freq_;
		iocp::IODispatcher &io_;
		async::thread::AutoEvent exit_;

		typedef std::tr1::function<void (const common::BmpInfo &)> Callback;
		Callback i_callback_;
		Callback j_callback_;

		static volatile long ONLYONE;

	public:
		HistoryPlay(iocp::IODispatcher &io, const std::wstring &i_path, const std::wstring &j_path, size_t i_freq, size_t j_freq, const Callback &i_callback, const Callback &j_callback)
			: i_path_(i_path)
			, j_path_(j_path)
			, i_freq_(i_freq)
			, j_freq_(j_freq)
			, io_(io)
			, i_callback_(i_callback)
			, j_callback_(j_callback)
		{}

		~HistoryPlay()
		{

		}

	public:
		void Start();
		static void Stop();

	private:
		void _Handle(std::wstring path, size_t freq, const Callback &callback);
	};
}





#endif