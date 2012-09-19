#ifndef __NETWORK_IMPL_HPP
#define __NETWORK_IMPL_HPP


#include "../../async/network/Socket.hpp"
#include "../../async/MultiThread/Thread.hpp"

//#include <Comm/Allocator/ContainerAllocator.hpp>


using namespace async;
using namespace async::network;

namespace i8desk
{

	namespace networkimpl
	{
		// ------------------------------------------------
		// class NetworkImpl

		class NetworkImpl
		{
			// 回调接口
			typedef std::tr1::function<void(const SocketPtr &, const SocketBufferPtr &, const SOCKADDR_IN &)>	AcceptCallback;
			typedef std::tr1::function<void(const SocketPtr &, const SocketBufferPtr &)>						RecvCallback;
			typedef std::tr1::function<void(const SocketPtr &, const SocketBufferPtr &, size_t)>				SendCallback;

		public:
			// 一次投递Accept个数
			enum { MAX_ACCEPT = 10 };

		private:
			IODispatcher &io_;
			SocketPtr acceptor_;

			// 投递Accept线程
			async::thread::ThreadImplEx acceptThread_;

			// 回调接口
			AcceptCallback acceptCallback_;
			RecvCallback recvCallback_;
			SendCallback sendCallback_;

		public:
			explicit NetworkImpl(IODispatcher &io);
			~NetworkImpl();


		public:
			// 注册回调
			void Register(AcceptCallback, RecvCallback, SendCallback);
			// 启动
			void Start(u_short);
			// 停止
			void Stop();

			// 发送数据
			void Send(const SocketPtr &socket, const SocketBufferPtr &buffer);


			// IOCP回调处理
		private:
			void _OnAccept(const AsyncResultPtr &);
			void _OnRecv(const AsyncResultPtr &);
			void _OnSend(const AsyncResultPtr &);

		private:
			DWORD _ThreadAccept();

			// 增加远程连接
			void _AddRemote(const SocketPtr &, const SOCKADDR_IN &);
			// 移除远程连接
			void _RemoveRemote(const SocketPtr &);

		};


	} // end of namespace - networkimpl


} // end of namespace - i8desk

#endif