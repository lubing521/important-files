#ifndef __STATE_MANAGER_HPP
#define __STATE_MANAGER_HPP

#include <array>
#include <functional>

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"

#include "../../Common/PackageHelper.hpp"

namespace proxy
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
		void Start(const OnReadCallback &readCallback)
		{	
			readCallback_ = readCallback;

			using namespace std::tr1::placeholders;
			try
			{
				acceptor_.AsyncAccept(0, std::tr1::bind(&StateMgr::_AcceptImpl, this, _1, _2));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}
			
		}

		void Stop()
		{
			sck_.reset();
		}

		void Send(size_t len)
		{
			try
			{
				iocp::AsyncWrite(*sck_, iocp::Buffer(buf_, len), iocp::TransferAll(), 
					std::tr1::bind(&StateMgr::_WriteImpl, this, iocp::_Size, iocp::_Error));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				_ReStart();
			}
		}

		std::tr1::array<char, common::MAX_TCP_PACKAGE> &Buffer()
		{
			return buf_;
		}


	private:
		void _ReStart()
		{
			Stop();
			Start(readCallback_);
		}

		void _AcceptImpl(u_long error, const network::SocketPtr &acceptSocket)
		{
			try
			{
				if( error == 0 )
				{
					sck_.reset(new network::Tcp::Socket(acceptSocket));

					iocp::AsyncRead(*sck_, iocp::Buffer(buf_), iocp::TransferAtLeast(sizeof(common::TCPPKHeader)),
						std::tr1::bind(&StateMgr::_ReadImpl, this, iocp::_Size, iocp::_Error));
				}
			}
			catch(const std::exception &e)
			{
				::OutputDebugStringA(e.what());
				_ReStart();
			}
		}

		void _ReadImpl(u_long size, u_long error)
		{
			if( error != 0 || size == 0 )
			{
				_ReStart();
				return;
			}

			readCallback_(buf_.data(), size);
		}

		void _WriteImpl(u_long size, u_long error)
		{
			_ReStart();	
		}
	

	};
}






#endif