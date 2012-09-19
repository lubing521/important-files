#ifndef __NETWORK_IMPL_HPP
#define __NETWORK_IMPL_HPP


#include "../../../../include/AsyncIO/Network/TCP.hpp"
#include "../../../../include/AsyncIO/Timer/Timer.hpp"
#include "../../../../include/MultiThread/Lock.hpp"
#include "Network.h"

#include <memory>

namespace i8desk
{
	namespace networklayer
	{

		using namespace async::iocp;
		using namespace async::network;
		using namespace async::timer;

		// ----------------------------
		// class NetworkImpl

		class NetworkImpl
			: public NetworkInterface
			, public std::tr1::enable_shared_from_this<NetworkImpl>
		{
			typedef std::tr1::function<void()>			ConnectCallback;
			typedef async::thread::AutoCriticalSection	Mutex;
			typedef async::thread::AutoLock<Mutex>		Lock;

		private:
			ConnectCallback				connectCallback_;
			mutable volatile long		connected_;
			mutable volatile long		exit_;

			Tcp::Socket					socket_;
			Mutex						mutex_;

			std::string					ip_;
			u_short						port_;	

		public:
			explicit NetworkImpl(const ConnectCallback &callback);
			virtual ~NetworkImpl();

		private:
			NetworkImpl(const NetworkImpl &);
			NetworkImpl &operator=(const NetworkImpl &);

			// Network Interface
		public:
			void Connect(const std::string &ip, u_short port);
			bool IsConnect() const;
			void Close();
			size_t Send(const char *buf, size_t len);
			size_t Recv(char *buf, size_t len);

		private:
			void _OnConnect(u_long);
			void _CloseSocket();
		};
	}
}




#endif