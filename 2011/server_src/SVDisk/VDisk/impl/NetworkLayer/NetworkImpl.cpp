#include "stdafx.h"
#include "NetworkImpl.h"


#include"../../async/Network/SocketProvider.hpp"
#include "../../async/Network/SocketOption.hpp"

#include <MSTcpIP.h>




namespace i8desk
{

	// alterable IO
	void WINAPI APCFunc(ULONG_PTR pParam)
	{
		// do nothing
	}


	namespace networkimpl
	{



		NetworkImpl::NetworkImpl(IODispatcher &io)
			: io_(io)
			, acceptor_(new Socket(io))
		{
		}

		NetworkImpl::~NetworkImpl()
		{
			Stop();
		}


		void NetworkImpl::Register(AcceptCallback acceptCallback, RecvCallback recvCallback, SendCallback sendCallback)
		{
			acceptCallback_ = acceptCallback;
			recvCallback_ = recvCallback;
			sendCallback_ = sendCallback;
		}

		void NetworkImpl::Start(u_short uPort)
		{
			try
			{
				// 将建监听socket
				if( *acceptor_ == INVALID_SOCKET )
					acceptor_->Create();

				acceptor_->Bind(uPort);
				acceptor_->Listen();

				// 创建Accept 线程
				acceptThread_.RegisterFunc(std::tr1::bind(&NetworkImpl::_ThreadAccept, this));
				acceptThread_.Start();
			} 
			catch(const std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
		}


		void NetworkImpl::Stop()
		{
			acceptor_->Close();

			// 可提醒IO，退出监听线程
			::QueueUserAPC(i8desk::APCFunc, acceptThread_, NULL);
			acceptThread_.Stop();
		}


		void NetworkImpl::Send(const SocketPtr &socket, const SocketBufferPtr &buffer)
		{
			try 
			{
				socket->BeginSend(buffer, 0, buffer->size(), std::tr1::bind(&NetworkImpl::_OnSend, this,
					std::tr1::placeholders::_1), socket);
			}
			catch(const std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
		}


		void NetworkImpl::_OnAccept(const AsyncResultPtr &asyncResult)
		{
			try 
			{
				// 获取远程连接
				const SocketPtr &listenSocket = asyncResult->m_asynState;
				const SocketPtr &acceptSocket = listenSocket->EndAccept(asyncResult);

				// 获取提供的Buffer
				const SocketBufferPtr &addrBuffer = asyncResult->m_buffer;

				// 复制Listen socket属性
				UpdateAcceptContext context(*listenSocket);
				acceptSocket->SetOption(context);

				// 设置超时
				struct TCPKeepAlive
				{
					u_long onoff;
					u_long keepalivetime;
					u_long keepaliveinterval;				
				};

				TCPKeepAlive inKeepAlive = {0};
				DWORD dwInLen = sizeof(TCPKeepAlive);
				TCPKeepAlive outKeepAlive = {0};
				DWORD dwOutLen = sizeof(TCPKeepAlive);

				inKeepAlive.onoff = 1;
				inKeepAlive.keepalivetime = 30000;
				inKeepAlive.keepaliveinterval = 2000;	

				acceptSocket->IOControl(SIO_KEEPALIVE_VALS, &inKeepAlive, dwInLen, &outKeepAlive, dwOutLen);

				// 获取远程地址
				SOCKADDR_IN *pLocalAddr = NULL, *pRemoteAddr = NULL;
				int szLocalLen = 0, szRemoteLen = 0;

				SocketProvider::GetSingleton(io_).GetAcceptExSockaddrs(addrBuffer->data(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, 
					reinterpret_cast<SOCKADDR **>(&pLocalAddr), &szLocalLen, reinterpret_cast<SOCKADDR **>(&pRemoteAddr), &szRemoteLen);

				// 回调
				if( acceptCallback_ != NULL )
					acceptCallback_(acceptSocket, addrBuffer, *pRemoteAddr);

				// 管理
				_AddRemote(acceptSocket, *pLocalAddr);

				// 投递新的接受请求
				SocketBufferPtr buffer(new SocketBuffer);
				asyncResult->reset(acceptSocket, buffer, acceptSocket, nothing, 
					std::tr1::bind(&NetworkImpl::_OnRecv, this, std::tr1::placeholders::_1));

				acceptSocket->BeginRecv(asyncResult, 0, buffer->allocSize());
			} 
			catch(const std::exception &e) 
			{
				std::cerr << e.what() << std::endl;
			}
		}


		void NetworkImpl::_OnRecv(const AsyncResultPtr &asyncResult)
		{
			const SocketPtr &sock = asyncResult->m_asynState;

			try 
			{
				size_t size = sock->EndRecv(asyncResult);
				const SocketBufferPtr &buffer = asyncResult->m_buffer;
				buffer->resize(size);

				if( size == 0 )
				{
					_RemoveRemote(sock);
					return ;
				}

				// 回调
				//if( recvCallback_ != NULL )
				//	recvCallback_(std::tr1::cref(sock), std::tr1::cref(buffer));
				
				/*asyncResult->m_callback = std::tr1::bind(&NetworkImpl::_OnSend, this, std::tr1::placeholders::_1);
				sock->BeginSend(asyncResult, 0, size);*/

				sock->BeginRecv(asyncResult, 0, buffer->allocSize());
			}
			catch(const std::exception &e) 
			{
				std::cerr << e.what() << std::endl;
				_RemoveRemote(sock);
			}
		}

		void NetworkImpl::_OnSend(const AsyncResultPtr &asyncResult)
		{
			const SocketPtr &socket = asyncResult->m_asynState;

			try 
			{
				const SocketBufferPtr &buffer = asyncResult->m_buffer;
				size_t size = socket->EndSend(asyncResult);
				if( size != 0 )
				{
					// 回调
					if( sendCallback_ != NULL )
						sendCallback_(std::tr1::cref(socket), std::tr1::cref(buffer), size);

					/*asyncResult->m_callback = std::tr1::bind(&NetworkImpl::_OnRecv, this, std::tr1::placeholders::_1);
					socket->BeginRecv(asyncResult, 0, buffer->allocSize());*/
				}
				else 
				{
					_RemoveRemote(socket);
					return;
				}
			}
			catch(const std::exception &e) 
			{
				std::cerr << e.what() << std::endl;
				_RemoveRemote(socket);
			}
		}



		void NetworkImpl::_AddRemote(const SocketPtr &remoteSock, const SOCKADDR_IN &remoteAddr)
		{

		}

		void NetworkImpl::_RemoveRemote(const SocketPtr &remoteSock)
		{

		}


		DWORD NetworkImpl::_ThreadAccept()
		{
			// 通过使用WSAEventSelect来判断是否有足够的AcceptEx，或者检测出一个非正常的客户请求
			HANDLE hAcceptEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			::WSAEventSelect(acceptor_->GetHandle(), hAcceptEvent, FD_ACCEPT);

			while(!acceptThread_.IsAborted())
			{	
				DWORD dwError = ::WaitForSingleObjectEx(hAcceptEvent, INFINITE, TRUE);
				if( dwError == WAIT_FAILED || dwError == WAIT_IO_COMPLETION )
					break;
				else if( dwError == WAIT_OBJECT_0 )
				{
					// 投递接收连接
					for(int i = 0; i != MAX_ACCEPT; ++i)
					{
						acceptor_->BeginAccept(0, std::tr1::bind(&NetworkImpl::_OnAccept, this, std::tr1::placeholders::_1), acceptor_);
					}
				}
			}

			::CloseHandle(hAcceptEvent);

			return 0;
		}

	}
}