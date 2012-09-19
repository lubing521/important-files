#ifndef __STATE_MANAGER_HPP
#define __STATE_MANAGER_HPP


#include <array>
#include <functional>

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"

#include "../../Common/PackageHelper.hpp"


using namespace async;

namespace sendsvr
{
	class StateMgr
	{
		std::tr1::array<char, common::MAX_TCP_PACKAGE> buf_;
		iocp::IODispatcher &io_;
		network::Tcp::Accpetor acceptor_;
		std::auto_ptr<network::Tcp::Socket> sck_;

		typedef std::tr1::function<void(const char *, size_t)> OnReadCallback;
		OnReadCallback readCallback_;

	public:
		StateMgr(iocp::IODispatcher &io, u_short port)
			: io_(io)
			, acceptor_(io, network::Tcp::V4(), port)
		{

		}
		~StateMgr()
		{

		}

	public:
		void Start(const OnReadCallback &readCallback);
		void Stop();

		void Send(size_t len);

		std::tr1::array<char, common::MAX_TCP_PACKAGE> &Buffer()
		{
			return buf_;
		}


	private:
		void _ReStart();
		void _AcceptImpl(u_long error, const network::SocketPtr &acceptSocket);
		void _ReadImpl(u_long size, u_long error);
		void _WriteImpl(u_long size, u_long error);
	};

}



#endif