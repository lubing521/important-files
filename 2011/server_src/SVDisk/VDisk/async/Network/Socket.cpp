#include "stdafx.h"

#include "Socket.hpp"
#include "SocketProvider.hpp"

#include "../IOCP/WinException.hpp"


namespace async
{


	namespace network
	{

		Socket::Socket(IODispatcher &io, int nType /* = SOCK_STREAM */, int nProtocol /* = IPPROTO_TCP */)
			: socket_(INVALID_SOCKET)
			, io_(io)
		{
			SocketProvider &provider = SocketProvider::GetSingleton(io_);

			// 创建
			Create(nType, nProtocol);

			// 绑定到IOCP
			io_.Bind(reinterpret_cast<HANDLE>(socket_));
		}

		Socket::~Socket()
		{
			Close();
		}


		void Socket::Create(int nType /* = SOCK_STREAM */, int nProtocol /* = IPPROTO_TCP */)
		{
			if( socket_ != INVALID_SOCKET )
				return;

			socket_ = ::WSASocket(AF_INET, nType, nProtocol, NULL, 0, WSA_FLAG_OVERLAPPED);
			if( socket_ == INVALID_SOCKET )
				throw Win32Exception("WSASocket");
		}

		void Socket::Close()
		{
			if( socket_ == INVALID_SOCKET )
				return;

			::shutdown(socket_, SD_BOTH);
			::closesocket(socket_);
			socket_ = INVALID_SOCKET;
		}


		void Socket::Bind(u_short uPort /* = 0 */, const IPAddress &addr /* = INADDR_ANY */)
		{
			sockaddr_in addrIn = {0};
			addrIn.sin_family		= AF_INET;
			addrIn.sin_port			= ::htons(uPort);
			addrIn.sin_addr.s_addr	= ::htonl(addr.Address());

			if( 0 != ::bind(socket_, (const SOCKADDR *)&addrIn, sizeof(SOCKADDR)) )
				throw Win32Exception("bind");
		}

		void Socket::Listen(int nMax)
		{
			if( 0 != ::listen(socket_, nMax) )
				throw Win32Exception("listen");
		}

		SocketPtr Socket::Accept()
		{
			return EndAccept(BeginAccept());
		}

		void Socket::Connect(const IPAddress &addr, u_short uPort)
		{
			EndConnect(BeginConnect(addr, uPort));
		}

		void Socket::DisConnect(bool bReuseSocket /* = true */)
		{
			EndDisconnect(BeginDisconnect(bReuseSocket));
		}


		size_t Socket::Recv(const SocketBufferPtr &buf)
		{
			return EndRecv(BeginRecv(buf, 0, buf->allocSize()));
		}

		size_t Socket::Send(const SocketBufferPtr &buf)
		{
			return EndSend(BeginSend(buf, 0, buf->size()));
		}


		size_t Socket::_EndAsyncOperation(const AsyncResultPtr &asyncResult)
		{
			DWORD size = 0;

			if( 0 == ::GetOverlappedResult((HANDLE)socket_, asyncResult.Get(), &size, TRUE) )
				throw Win32Exception("GetOverlappedResult");

			return size;
		}

		AsyncResultPtr Socket::BeginAccept(size_t szOutSide/* = 0*/, const AsyncCallbackFunc &callback /* = 0 */, const ObjectPtr &asyncState/* = nothing*/)
		{
			SocketBufferPtr acceptBuffer(new SocketBuffer((sizeof(sockaddr_in) + 16) * 2 + szOutSide));
			SocketPtr acceptSocket(new Socket(io_));
			AsyncResultPtr asyncResult(new AsyncResult(this, acceptBuffer, asyncState, acceptSocket, callback));
			asyncResult->AddRef();

			// 根据szOutSide大小判断，是否需要接收远程客户机第一块数据才返回。
			// 如果为0，则立即返回。若大于0，则接受数据后再返回
			DWORD dwRecvBytes = 0;
			if( !SocketProvider::GetSingleton(io_).AcceptEx(socket_, acceptSocket->socket_, acceptBuffer->data(), szOutSide,
				sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwRecvBytes, asyncResult.Get()) 
				&& ::WSAGetLastError() != ERROR_IO_PENDING )
			{
				asyncResult->Release();

				throw Win32Exception("AcceptEx");
			}

			return asyncResult;
		}

		SocketPtr Socket::EndAccept(const AsyncResultPtr &asynResult)
		{
			_EndAsyncOperation(asynResult);
			return asynResult->m_internalState;
		}


		// 异步调用
		AsyncResultPtr Socket::BeginConnect(const IPAddress &addr, u_short uPort, const AsyncCallbackFunc &callback /* = NULL */, const ObjectPtr &asyncState /* = nothing */)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, nothing, asyncState, nothing, callback));
			asynResult->AddRef();

			_BeginConnectImpl(asynResult, addr, uPort);

			return asynResult;
		}
		const AsyncResultPtr &Socket::BeginConnect(const AsyncResultPtr &result, const IPAddress &addr, u_short uPort)
		{
			result->AddRef();

			_BeginConnectImpl(result, addr, uPort);
			return result;
		}

		void Socket::EndConnect(const AsyncResultPtr &asyncResult)
		{
			_EndAsyncOperation(asyncResult);
		}


		AsyncResultPtr Socket::BeginDisconnect(bool bReuseSocket /* = true */, const AsyncCallbackFunc &callback /* = NULL */, const ObjectPtr &asyncState /* = nothing */)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, nothing, asyncState, nothing, callback));
			asynResult->AddRef();

			_BeginDisconnectImpl(asynResult, bReuseSocket);

			return asynResult;
		}
		const AsyncResultPtr &Socket::BeginDisconnect(const AsyncResultPtr &result, bool bReuseSocket /* = true */)
		{
			result->AddRef();

			_BeginDisconnectImpl(result, bReuseSocket);

			return result;
		}

		void Socket::EndDisconnect(const AsyncResultPtr &asyncResult)
		{
			_EndAsyncOperation(asyncResult);
		}


		AsyncResultPtr Socket::BeginRecv(const SocketBufferPtr &buf, size_t nOffset, size_t nSize, const AsyncCallbackFunc &callback /* = NULL */, const ObjectPtr &asyncState /* = nothing */, const ObjectPtr &internalState /*= nothing*/)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, buf, asyncState, internalState, callback));
			asynResult->AddRef();

			_BeginRecvImpl(asynResult, nOffset, nSize);

			return asynResult;
		}

		const AsyncResultPtr &Socket::BeginRecv(const AsyncResultPtr &result, size_t offset, size_t size)
		{
			result->AddRef();

			_BeginRecvImpl(result, offset, size);

			return result;
		}

		size_t Socket::EndRecv(const AsyncResultPtr &asyncResult)
		{
			return _EndAsyncOperation(asyncResult);
		}


		AsyncResultPtr Socket::BeginSend(const SocketBufferPtr &buf, size_t nOffset, size_t nSize, const AsyncCallbackFunc &callback /* = NULL */, const ObjectPtr &asyncState /* = nothing */, const ObjectPtr &internalState /*= nothing*/)
		{
			AsyncResultPtr asynResult(new AsyncResult(this, buf, asyncState, internalState, callback));
			asynResult->AddRef();
			
			_BeginSendImpl(asynResult, nOffset, nSize);

			return asynResult;
		}

		const AsyncResultPtr &Socket::BeginSend(const AsyncResultPtr &result, size_t offset, size_t size)
		{
			result->AddRef();

			_BeginSendImpl(result, offset, size);

			return result;
		}

		size_t Socket::EndSend(const AsyncResultPtr &asyncResult)
		{
			return _EndAsyncOperation(asyncResult);
		}


		// --------------------------------

		void Socket::_BeginConnectImpl(const AsyncResultPtr &result, const IPAddress &addr, u_short uPort)
		{
			sockaddr_in remoteAddr = {0};
			remoteAddr.sin_family		= AF_INET;
			remoteAddr.sin_port			= ::htons(uPort);
			remoteAddr.sin_addr.s_addr	= addr.Address();

			// 很变态，需要先bind
			::bind(socket_, reinterpret_cast<const sockaddr *>(&remoteAddr), sizeof(sockaddr_in));

			if( !SocketProvider::GetSingleton(io_).ConnectEx(socket_, reinterpret_cast<SOCKADDR *>(&remoteAddr), sizeof(SOCKADDR), 0, 0, 0, result.Get()) 
				&& ::WSAGetLastError() != WSA_IO_PENDING )
			{
				result->Release();
				throw Win32Exception("ConnectionEx");
			}
		}

		void Socket::_BeginDisconnectImpl(const AsyncResultPtr &result, bool bReuseSocket)
		{
			DWORD dwFlags = bReuseSocket ? TF_REUSE_SOCKET : 0;

			if( !SocketProvider::GetSingleton(io_).DisconnectEx(socket_, result.Get(), dwFlags, 0) 
				&& ::WSAGetLastError() != WSA_IO_PENDING )
			{
				result->Release();
				throw Win32Exception("DisConnectionEx");
			}
		}

		void Socket::_BeginRecvImpl(const AsyncResultPtr &result, size_t offset, size_t size)
		{
			const SocketBufferPtr buffer = result->m_buffer;

			WSABUF wsabuf = {0};
			wsabuf.buf = reinterpret_cast<char *>(buffer->data(offset));
			wsabuf.len = size;

			DWORD dwFlag = 0;
			DWORD dwSize = 0;
			if(  0 != ::WSARecv(socket_, &wsabuf, 1, &dwSize, &dwFlag, result.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
			{
				result->Release();
				throw Win32Exception("WSARecv");
			}
		}

		void Socket::_BeginSendImpl(const AsyncResultPtr &result, size_t offset, size_t size)
		{
			const SocketBufferPtr buffer = result->m_buffer;
			WSABUF wsabuf = {0};
			wsabuf.buf = reinterpret_cast<char *>(buffer->data(offset));
			wsabuf.len = size;

			DWORD dwFlag = 0;
			DWORD dwSize = 0;
			if(  0 != ::WSASend(socket_, &wsabuf, 1, &dwSize, dwFlag, result.Get(), NULL)
				&& ::WSAGetLastError() != WSA_IO_PENDING )
			{
				result->Release();
				throw Win32Exception("WSASend");
			}
		}

	}

}