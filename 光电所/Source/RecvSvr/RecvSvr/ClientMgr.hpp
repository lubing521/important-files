#ifndef __SEND_SVR_CLIENT_MANAGER_HPP
#define __SEND_SVR_CLIENT_MANAGER_HPP

#include <memory>
#include <atlbase.h>
#include <atlconv.h>

#include "../../../include/Container/SyncContainer.hpp"
#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"
#include "../../Common/MemoryPool.hpp"


using namespace async;

namespace recvSvr
{
	class Session;
	typedef std::tr1::shared_ptr<Session> SessionPtr;


	class SessionMgr
	{
		typedef async::container::SyncAssocContainer<SOCKET, SessionPtr> SessionContainer;
		SessionContainer sessions_;
		iocp::IODispatcher &io_;
		network::Tcp::Accpetor acceptor_;

		typedef std::tr1::function<void (const std::string &)> AcceptCallback;
		AcceptCallback acceptCallback_;
		mutable volatile long isPaused_;

	public:
		SessionMgr(iocp::IODispatcher &io, u_short port, const AcceptCallback &acceptCallback)
			: io_(io)
			, acceptor_(io_, network::Tcp::V4(), port)
			, acceptCallback_(acceptCallback)
			, isPaused_(0)
		{

		}
		~SessionMgr()
		{

		}

		void Start();
		void Stop();

		void Pause()
		{
			::InterlockedExchange(&isPaused_, 0);
		}

		void Resume()
		{
			::InterlockedExchange(&isPaused_, 1);
		}

		void Run(const common::BmpInfo &buffer) const;

		common::RemoteClients GetClients() const
		{
			common::RemoteClients clients;
			struct OP
			{
				common::RemoteClients &client_;
				OP(common::RemoteClients &client)
					: client_(client)
				{}

				void operator()(const SessionContainer::value_type& val) const
				{
					SOCKADDR_IN addr = {0};
					int len = sizeof(addr);
					::getpeername(val.first, (SOCKADDR *)&addr, &len);
					std::wstring ip = CA2W(async::network::IPAddress::Parse(addr.sin_addr.S_un.S_addr).c_str());

					client_.push_back(ip);
				}
			};

			sessions_.for_each(OP(clients));
			return clients;
		}

	private:
		void _OnAccept(u_long error, const network::SocketPtr &acceptSocket);
		void _OnDisconnect(const network::SocketPtr &sck);

	};
}


#endif