#ifndef __UDP_PROXY_HPP
#define __UDP_PROXY_HPP

#include <array>
#include <ctime>

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/UDP.hpp"
#include "../../../include/AsyncIO/Timer/Timer.hpp"

#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MultiThread/Thread.hpp"

#include "../../../include/Container/BlockingQueue.hpp"
#include "../../../include/Utility/utility.h"

#include "../../Common/PackageHelper.hpp"
#include "../../Common/MemoryPool.hpp"


using namespace async;

namespace proxy
{
	class UDPProxy
	{
		mutable volatile long isPaused_;
		mutable volatile long isOK_;
		
		iocp::IODispatcher &io_;
		network::Udp::Socket recvSck_;
		network::Udp::Socket sendSck_;

		async::container::BlockingQueue<common::Buffer> queue_;
		async::thread::ThreadImplEx recvThread_;
		async::thread::ThreadImplEx sendThread_;

	public:
		u_short recvPort_;
		u_short sendPort_;
		std::wstring localIP_;

	public:
		UDPProxy(iocp::IODispatcher &io, u_short recvPort, const std::string &sendIP, u_short sendPort)
			: io_(io)
			, recvSck_(io_, network::Udp::V4(), recvPort)
			, sendSck_(io_, network::Udp::V4())
			, isPaused_(0)
			, isOK_(1)
		{
			sendSck_.Connect(AF_INET, network::IPAddress::Parse(sendIP), sendPort);

			sendSck_.SetOption(network::RecvTimeOut(3000));
			recvSck_.SetOption(network::RecvTimeOut(3000));

			localIP_ = utility::GetLocalIP();
			recvPort_ = recvPort;
		}

	public:
		void Start()
		{
			try
			{
				recvThread_.RegisterFunc(std::tr1::bind(&UDPProxy::_RecvImpl, this));
				sendThread_.RegisterFunc(std::tr1::bind(&UDPProxy::_SendImpl, this));

				recvThread_.Start();
				sendThread_.Start();
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				return;
			}

		}

		void Stop()
		{
			recvSck_.Close();

			queue_.Put(common::Buffer());
			sendSck_.Close();

			recvThread_.Stop();
			sendThread_.Stop();
		}

		void Pause()
		{
			::InterlockedExchange(&isPaused_, 1);
		}

		void Resume()
		{
			::InterlockedExchange(&isPaused_, 0);
		}

		bool IsOK() const
		{
			return ::InterlockedExchangeAdd(&isOK_, 0) == 1;
		}

		bool IsPaused() const
		{
			return ::InterlockedExchangeAdd(&isPaused_, 0) == 1;
		}

	private:
		DWORD _SendImpl()
		{
			while(!sendThread_.IsAborted())
			{
				
				try
				{
					common::Buffer buf = queue_.Get();
					if( buf.first.get() == 0 )
						break;

					size_t len = sendSck_.SendTo(iocp::Buffer(buf.first.get(), buf.second), 0);
					if( len == 0 )
						break;

					SOCKADDR_IN addr = {0};
					char retBuf[4] = {0};
					sendSck_.RecvFrom(iocp::Buffer(retBuf), &addr);

					::InterlockedExchange(&isOK_, 1);
				}
				catch(std::exception &e)
				{
					::InterlockedExchange(&isOK_, 0);
					::OutputDebugStringA(e.what());
					::Sleep(1000);
				}
				
			}

			return 0;
		}

		DWORD _RecvImpl()
		{
			while(!recvThread_.IsAborted())
			{
				try
				{
					common::Buffer buf(common::MakeBuffer(common::MAX_UDP_PACKAGE));

					SOCKADDR_IN addr = {0};
					size_t len = recvSck_.RecvFrom(iocp::Buffer(buf.first.get(), buf.second), &addr);
					if( len == 0 )
						continue;

					char retBuf[4] = {0};
					size_t sendLen = recvSck_.SendTo(iocp::Buffer(retBuf), &addr);
					assert(sendLen == sizeof(retBuf));

					buf.second = len;

					if( !IsPaused() )
						queue_.Put(buf);
				}
				catch(std::exception &e)
				{
					::InterlockedExchange(&isOK_, 0);

					::OutputDebugStringA(e.what());
				}
			}

			return 0;
		}

	};
}


#endif