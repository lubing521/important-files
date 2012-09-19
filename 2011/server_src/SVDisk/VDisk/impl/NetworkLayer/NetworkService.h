#ifndef __NETWORK_LAYER_HPP
#define __NETWORK_LAYER_HPP


#include "NetworkImpl.h"
#include "ParserImpl.h"
#include "ProtocolParser.h"


namespace i8desk
{

	namespace networkimpl
	{
		// -----------------------------------------------------
		// class NetworkLayer

		class NetworkService
		{
			// 第一版本协议
			typedef ProtocolParser<ProtocolV1> ProtocolVersion1;

		private:
			IODispatcher io_;
			std::auto_ptr<NetworkImpl> impl_;

		private:
			NetworkService();
			~NetworkService();

		private:
			NetworkService(const NetworkService &);
			NetworkService &operator=(const NetworkService &);

		public:
			static NetworkService &GetInstance()
			{
				static NetworkService layer;
				return layer;
			}

		public:
			// 返回硬盘大小
			void SendDiskSize(const SocketPtr &socket, const SocketBufferPtr &buffer, const LARGE_INTEGER &sz);

			// 返回读取数据请求
			void SendFileData(const SocketPtr &socket, const SocketBufferPtr &buffer);

			// 返回写数据状态
			void SendWriteStatus(const SocketPtr &socket, const SocketBufferPtr &buffer);


		private:
			void _AcceptCallback(const SocketPtr &, const SocketBufferPtr &, const SOCKADDR_IN &);
			void _RecvCallback(const SocketPtr &, const SocketBufferPtr &);
			void _SendCallback(const SocketPtr &, const SocketBufferPtr &, size_t);

		private:
			void _SendDiskSize();
		};
	}
	
}



#endif