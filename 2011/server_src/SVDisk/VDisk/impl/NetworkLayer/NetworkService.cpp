#include "stdafx.h"
#include "NetworkService.h"


#include "../../async/Network/BufferHelper.hpp"

#include "../LayerHelper.h"


namespace i8desk
{

	namespace networkimpl
	{

		// ----------------------------------------

		NetworkService::NetworkService()
			: io_(GetFitThreadNum())
			, impl_(new NetworkImpl(io_))
		{
			using std::tr1::placeholders::_1;
			using std::tr1::placeholders::_2;
			using std::tr1::placeholders::_3;

			impl_->Start(5555);

			// 注册回调
			impl_->Register(std::tr1::bind(&NetworkService::_AcceptCallback, this, _1, _2, _3),
				std::tr1::bind(&NetworkService::_RecvCallback, this, _1, _2),
				std::tr1::bind(&NetworkService::_SendCallback, this, _1, _2, _3));
		}

		NetworkService::~NetworkService()
		{
			impl_->Stop();
		}


		// 调用接口
		void NetworkService::SendDiskSize(const SocketPtr &socket, const SocketBufferPtr &buffer, const LARGE_INTEGER &sz)
		{
			const size_t packageLen = ProtocolVersion1::ConstructPackage(buffer->data(), buffer->allocSize(), sz, OP_GET_DISK_SIZE);
			buffer->resize(packageLen);

			impl_->Send(socket, buffer);
		}

		void NetworkService::SendFileData(const SocketPtr &socket, const SocketBufferPtr &buffer)
		{
			
		}

		void NetworkService::SendWriteStatus(const SocketPtr &socket, const SocketBufferPtr &buffer)
		{

		}

		// 内部实现
		void NetworkService::_AcceptCallback(const SocketPtr &socket, const SocketBufferPtr &buffer, const SOCKADDR_IN &addr)
		{
			// 添加新用户
			i8desk::AddNewUser(socket, buffer, addr);


		}

		void NetworkService::_RecvCallback(const SocketPtr &socket, const SocketBufferPtr &buffer)
		{
			ProtocolVersion1 protocolParser(buffer->data(), buffer->size());
		
			switch(protocolParser.OP())
			{
			case OP_READ:
				i8desk::SendFileData(socket, buffer, protocolParser.Offset(), protocolParser.Length());
				break;
			case OP_WRITE:
				
				break;
			case OP_GET_DISK_SIZE:
				i8desk::SendDiskSize(socket, buffer, _T("\\\\.\\PhysicalDrive0"));
				break;
			default:
				{
					assert(0);
					std::cerr << "非法操作码" << std::endl;
				}
			}

			
		}

		void NetworkService::_SendCallback(const SocketPtr &socket, const SocketBufferPtr &buffer, size_t len)
		{
			if( len != buffer->size() )
			{
				assert(0);
				std::cerr << "发送数据不完整" << std::endl;
			}
			else
			{

			}
		}

	} // end of namespace - networkimpl


} // end of namespace - i8desk

	