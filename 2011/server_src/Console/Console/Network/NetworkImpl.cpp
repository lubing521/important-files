#include "stdafx.h"
#include "NetworkImpl.h"

#include "IOService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{
	namespace networklayer
	{

		// -------
		NetworkImpl::NetworkImpl(const ConnectCallback &callback)
			: connectCallback_(callback)
			, connected_(0)
			, exit_(0)
			, socket_(io::GetIODispatcher())
		{
			
		}
		NetworkImpl::~NetworkImpl()
		{
			Close();
		}	


		void NetworkImpl::Connect(const std::string &ip, u_short port)
		{
			if( ::InterlockedExchangeAdd(&exit_, 0) == 1 )
				return;

			_CloseSocket();

			ip_		= ip;
			port_	= port;
			
			try
			{
				//Lock lock(mutex_);

				if( !socket_.IsOpen() )
					socket_.Open();

				socket_.AsyncConnect(IPAddress::Parse(ip), port, 
					std::tr1::bind(&NetworkImpl::_OnConnect, shared_from_this(), _Error));
			}
			catch(std::exception &e)
			{
				::InterlockedExchange(&connected_, 0);
				//throw ConsoleExcept(CA2T(e.what()));
				
			}
		}

		bool NetworkImpl::IsConnect() const
		{
			return ::InterlockedExchangeAdd(&connected_, 0) == 1;
		}

		void NetworkImpl::Close()
		{
			_CloseSocket();
			::InterlockedExchange(&exit_, 1);
		}

		size_t NetworkImpl::Send(const char *buf, size_t len)
		{
			try
			{
				//Lock lock(mutex_);
				size_t size = Write(socket_, Buffer(buf, len));

				if( size == 0 )
					throw std::runtime_error("服务器关闭连接");

				return size;
			}	
			catch(std::exception &e)
			{
				_CloseSocket();
				throw exception::UnConnect(CA2T(e.what()));
			}
			
		}

		size_t NetworkImpl::Recv(char *buf, size_t len)
		{
			try
			{
				//Lock lock(mutex_);
				size_t size = Read(socket_, Buffer(buf, len));
				if( size == 0 )
					throw std::runtime_error("服务器关闭连接");

				return size;
			}	
			catch(std::exception &e)
			{
				_CloseSocket();
				throw exception::UnConnect(CA2T(e.what()));
			}
		}

		void NetworkImpl::_CloseSocket()
		{
			::InterlockedExchange(&connected_, 0);

			//Lock lock(mutex_);
			socket_.Close();
		}

		void NetworkImpl::_OnConnect(u_long err)
		{
			if( ::InterlockedExchangeAdd(&exit_, 0) == 1 )
				return;

			::InterlockedExchange(&connected_, err == 0 ? 1 : 0);

			if( IsConnect() )
			{
				// 连接成功, 通知回调
				connectCallback_();
			}
			else
			{
			}

		}
	}
}