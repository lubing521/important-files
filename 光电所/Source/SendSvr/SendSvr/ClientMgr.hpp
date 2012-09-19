#ifndef __SEND_SVR_CLIENT_MANAGER_HPP
#define __SEND_SVR_CLIENT_MANAGER_HPP

#include <memory>
#include <unordered_map>

#include "../../../include/Container/SyncContainer.hpp"
#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"
#include "../../Common/MemoryPool.hpp"


using namespace async;

namespace sendsvr
{
	class Session;
	typedef std::tr1::shared_ptr<Session> SessionPtr;


	class SessionMgr
	{
		typedef async::container::SyncAssocContainer<SOCKET, SessionPtr,
			std::tr1::unordered_map<SOCKET, SessionPtr>> SessionContainer;
		SessionContainer sessions_;
		iocp::IODispatcher &io_;
		network::Tcp::Accpetor acceptor_;

	public:
		SessionMgr(iocp::IODispatcher &io, u_short port)
			: io_(io)
			, acceptor_(io_, network::Tcp::V4(), port)
		{

		}
		~SessionMgr()
		{

		}

		void Start();
		void Stop();

		void Run(const common::BmpInfo &buffer) const;

	private:
		void _OnAccept(u_long error, const network::SocketPtr &acceptSocket);
		void _OnDisconnect(const network::SocketPtr &sck);

	};
}


#endif