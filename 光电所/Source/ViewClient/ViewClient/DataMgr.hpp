#ifndef __DATA_MANAGER_HPP
#define __DATA_MANAGER_HPP

#include <memory>
#include "../../../include/Container/BlockingQueue.hpp"
#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"
#include "../../Common/MemoryPool.hpp"


using namespace async;

namespace view
{
	struct BmpInfo
	{
		common::Buffer buf_;
		size_t width_;
		size_t height_;
	};
	class DataMgr
		: public std::tr1::enable_shared_from_this<DataMgr>
	{
		typedef async::container::BlockingQueue<BmpInfo> QueueBuffer;
		QueueBuffer buffer_;

		iocp::IODispatcher io_;
		network::Tcp::Socket sck_;
		
		typedef std::tr1::function<BOOL()> Callback;
		Callback callback_;

		u_short port_;
		std::string ip_;

		std::wstring taskID_;
		std::wstring taskTime_;

	public:
		DataMgr(u_short port, const std::string &ip, const Callback &callback);
		~DataMgr();

	public:
		void Start();
		void Stop();

		BmpInfo GetBuffer();

		const std::wstring &GetTaskID() const; 
		std::wstring GetTaskTime() const; 

	private:
		void _OnConnect(u_long, u_long);
		void _OnReadHeader(u_long, u_long, const std::tr1::shared_ptr<common::TCPPKHeader> &);
		void _OnReadContent(u_long, u_long, common::Buffer tcpBuf);

		void _DisConnect();
	};

	typedef std::tr1::shared_ptr<DataMgr> DataMgrPtr;
}




#endif