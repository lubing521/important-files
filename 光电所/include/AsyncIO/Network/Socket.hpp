#ifndef __NETWORK_SOCKET_HPP
#define __NETWORK_SOCKET_HPP



#include "../IOCP/Dispatcher.hpp"
#include "../IOCP/Buffer.hpp"

#include "IPAddress.hpp"
#include "SocketProvider.hpp"


namespace async
{


	namespace network
	{
		// forward declare

		class Socket;
		typedef std::tr1::shared_ptr<Socket> SocketPtr;



		//----------------------------------------------------------------------
		// class Socket

		class Socket
		{
		public:
			typedef iocp::IODispatcher	AsyncIODispatcherType;

		private:
			// socket handle
			SOCKET socket_;

			// IO服务
			AsyncIODispatcherType &io_;

		public:
			explicit Socket(AsyncIODispatcherType &);
			Socket(AsyncIODispatcherType &, SOCKET sock);
			Socket(AsyncIODispatcherType &, int family, int type, int protocol);
			~Socket();

			// non-copyable
		private:
			Socket(const Socket &);
			Socket &operator=(const Socket &);

		public:
			// explicit转换
			operator SOCKET()				{ return socket_; }
			operator const SOCKET() const	{ return socket_; }

			// 显示获取
			SOCKET GetHandle()				{ return socket_; }
			const SOCKET GetHandle() const	{ return socket_; }

		public:
			// WSAIoctl
			template<typename InT, typename OutT>
			DWORD IOControl(DWORD dwIOControl, InT *inData, DWORD inSize, OutT *outData, DWORD outSize);

			// setsocketopt
			template<typename SocketOptionT>
			bool SetOption(const SocketOptionT &option);

			// getsockopt
			template<typename SocketOptionT>
			bool GetOption(SocketOptionT &option) const;

			// WSASocket
			void Open(int family, int nType, int nProtocol);
			// shutdown
			void Shutdown(int shut);
			// closesocket
			void Close();

			bool IsOpen() const;
			// CancelIO/CancelIOEx
			void Cancel();

			// bind
			void Bind(int family, u_short uPort, const IPAddress &addr);
			// listen
			void Listen(int nMax);


			// 不需设置回调接口,同步函数
		public:
			SocketPtr Accept();
			void Connect(int family, const IPAddress &addr, u_short uPort);
			void DisConnect(int shut, bool bReuseSocket = true);

			size_t Read(char *buf, size_t size, DWORD flag);
			size_t Write(const char *buffer, size_t size, DWORD flag);

			size_t SendTo(const char *buffer, size_t sz, const SOCKADDR_IN *addr, DWORD flag);
			size_t RecvFrom(char *buffer, size_t sz, SOCKADDR_IN *addr, DWORD flag);

			// 异步调用接口
		public:
			// szOutSize指定额外的缓冲区大小，以用来Accept远程连接后且收到第一块数据包才返回
			template < typename HandlerT >
			void AsyncAccept(size_t szOutSize, const HandlerT &callback);
			// 异步连接需要先绑定端口
			void AsyncConnect(const IPAddress &addr, u_short uPort, const iocp::CallbackType &callback);

			// 异步断开连接
			void AsyncDisconnect(const iocp::CallbackType &callback, bool bReuseSocket);

			// 异步TCP读取
			void AsyncRead(char *buf, size_t size, const iocp::CallbackType &callback);

			// 异步TCP写入
			void AsyncWrite(const char *buf, size_t size, const iocp::CallbackType &callback);

			// 异步UDP读取
			void AsyncSendTo(const char *buf, size_t size, const SOCKADDR_IN *addr, const iocp::CallbackType &callback);

			// 异步UDP读入
			void AsyncRecvFrom(char *buf, size_t size, SOCKADDR_IN *addr, const iocp::CallbackType &callback);
		};
	}
}

namespace async
{
	namespace iocp
	{
		typedef async::network::Socket Socket;

		// Socket 工厂
		template<>
		struct ObjectFactory< Socket >
		{
			typedef async::memory::FixedMemoryPool<true, sizeof(Socket)>	PoolType;
			typedef ObjectPool< PoolType >									ObjectPoolType;
		};
	}
}

#include "Accept.hpp"

namespace async
{
	namespace network
	{

		inline SocketPtr MakeSocket(Socket::AsyncIODispatcherType &io)
		{
			return SocketPtr(ObjectAllocate<Socket>(io), &ObjectDeallocate<Socket>);
		}

		inline SocketPtr MakeSocket(Socket::AsyncIODispatcherType &io, SOCKET sock)
		{
			return SocketPtr(ObjectAllocate<Socket>(io, sock), &ObjectDeallocate<Socket>);
		}

		inline SocketPtr MakeSocket(Socket::AsyncIODispatcherType &io, int family, int type, int protocol)
		{
			return SocketPtr(ObjectAllocate<Socket>(io, family, type, protocol), &ObjectDeallocate<Socket>);
		}


		// ---------------------------

		inline bool Socket::IsOpen() const
		{
			return socket_ != INVALID_SOCKET;
		}

		template<typename InT, typename OutT>
		inline DWORD Socket::IOControl(DWORD dwIOControl, InT *inData, DWORD inSize, OutT *outData, DWORD outSize)
		{
			DWORD dwRet = 0;

			if( 0 != ::WSAIoctl(socket_, dwIOControl, inData, inSize, outData, outSize, &dwRet, 0, 0) )
				throw Win32Exception("WSAIoCtl");

			return dwRet;
		}

		template<typename SocketOptionT>
		inline bool Socket::SetOption(const SocketOptionT &option)
		{
			if( !IsOpen() )
				return false;

			/*if( option.level() == SOL_SOCKET && option.name() == SO_LINGER )
			{
			const ::linger *lingerData = reinterpret_cast<const ::linger *>(option.data());
			if( lingerData->l_onoff != 0 && lingerData->l_linger != 0 )

			}*/

			return SOCKET_ERROR != ::setsockopt(socket_, option.level(), option.name(), option.data(), option.size());
		}

		template<typename SocketOptionT>
		inline bool Socket::GetOption(SocketOptionT &option) const
		{
			if( !IsOpen() )
				return false;

			size_t sz = option.size();
			if( SOCKET_ERROR != ::getsockopt(socket_, option.level(), option.name(), option.data(), sz) )
			{
				option.resize(sz);
				return true;
			}
			else
				return false;
		}



		template < typename HandlerT >
		inline void Socket::AsyncAccept(size_t szOutSize, const HandlerT &callback)
		{
			if( !IsOpen() ) 
				throw std::logic_error("Socket not open");

			SocketPtr remoteSocket(MakeSocket(io_, Tcp::V4().Family(), Tcp::V4().Type(), Tcp::V4().Protocol()));
			iocp::AutoBufferPtr acceptBuffer(MakeBuffer((sizeof(sockaddr_in) + 16) * 2 + szOutSize));

			typedef detail::AcceptorHandle<HandlerT> HookAcceptor;
			AsyncCallbackBasePtr asyncResult(MakeAsyncCallback(HookAcceptor(*this, remoteSocket, acceptBuffer, callback), &callback));

			// 根据szOutSide大小判断，是否需要接收远程客户机第一块数据才返回。
			// 如果为0，则立即返回。若大于0，则接受数据后再返回
			DWORD dwRecvBytes = 0;
			if( !SocketProvider::GetSingleton(io_).AcceptEx(socket_, remoteSocket->socket_, acceptBuffer->data(), szOutSize,
				sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwRecvBytes, asyncResult.Get()) 
				&& ::WSAGetLastError() != ERROR_IO_PENDING )
				throw Win32Exception("AcceptEx");

			asyncResult.Release();
		}
		
	}
}



#endif