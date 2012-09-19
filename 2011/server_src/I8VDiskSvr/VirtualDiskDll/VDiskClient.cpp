#include "stdafx.h"
#include "VDiskClient.hpp"

#include "VDiskService.hpp"

#include "Misc.hpp"
#include "NetworkFlow.hpp"

#include "../../../include/MultiThread/Tls.hpp"

#include <map>




namespace i8desk
{


	using namespace async::iocp;
	using namespace async::network;
	using namespace std::tr1::placeholders;


	

	VDiskClient::VDiskClient(const SocketPtr &socket, VDiskService &service)
		: service_(service)
		, socket_(socket)
		, file_(service_.GetService(), service_.GetFilePath().c_str(), GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED)
	{
		buf_.resize(VDiskMemoryMgr::PackageBufSize);
	}


	void VDiskClient::Start()
	{
		try
		{		
			AsyncRead(socket_, Buffer(buf_), TransferAtLeast(sizeof(DRP)),
				std::tr1::bind(&VDiskClient::_HandleNetworkRead, shared_from_this(), _1, _2));

		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::Stop()
	{
		socket_.Close();
		file_.Close();
	}

	void VDiskClient::_NetworkSend(size_t len)
	{
		try
		{
			// 发送数据
			AsyncWrite(socket_, Buffer(&buf_[0], len), TransferAll(), 
				std::tr1::bind(&VDiskClient::_HandleNetworkWrite, shared_from_this(), _1, _2));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::_FileRead(size_t len, const LARGE_INTEGER &offset)
	{
		try
		{
			AsyncRead(file_, Buffer(&buf_[sizeof(DSTATUS)], len), offset, TransferAtLeast(len),
				std::tr1::bind(&VDiskClient::_HandleFileRead, shared_from_this(), _1, _2, offset));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::_HandleNetworkRead(u_long bytes, u_long error)
	{
		try
		{
			// 产生错误，同时error需要处理
			if( bytes == 0 )
			{
				socket_.AsyncDisconnect(std::tr1::bind(&VDiskClient::_HandleDisconnect, shared_from_this()));
				return;
			}

			DRP *pDrp = reinterpret_cast<DRP *>(&buf_[0]);
			
			switch(pDrp->MajorFunction)
			{
			case DRP_MJ_REFRESH:
				_VDiskRefresh();
				break;

			case DRP_MJ_QUERY:
				_VDiskQuery();
				break;

			case DRP_MJ_READ:
				_VDiskRead(*pDrp);
				break;

			case DRP_MJ_WRITE:
				_VDiskWrite();
				break;

			default:
				assert(0);
				//LogMgr::Instance() << "接收操作指令指令错误(" << pDrp->MajorFunction << ")" << logsystem::Endl;
				Stop();
			}
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::_HandleNetworkWrite(u_long bytes, u_long error)
	{
		try
		{
			if( bytes == 0 )
			{
				socket_.AsyncDisconnect(std::tr1::bind(&VDiskClient::_HandleDisconnect, shared_from_this()));
				return;
			}

			// 进行流量统计

			AsyncRead(socket_, Buffer(buf_), TransferAtLeast(sizeof(DRP)),
				std::tr1::bind(&VDiskClient::_HandleNetworkRead, shared_from_this(), _1, _2));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::_HandleFileRead(u_long bytes, u_long error, const LARGE_INTEGER &offset)
	{
		try
		{
			if( bytes == 0 || error != 0 )
			{
				socket_.AsyncDisconnect(std::tr1::bind(&VDiskClient::_HandleDisconnect, shared_from_this()));
				return;
			}
	
			BufferPtr buf = BufferPool::Allocator(bytes);
			std::copy(buf_.begin() + sizeof(DSTATUS), buf_.begin() + sizeof(DSTATUS) + bytes, buf.get());
			service_.GetDiskCache().Insert(offset.QuadPart, buf, bytes);

			_NetworkSend(bytes + sizeof(DSTATUS));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			Stop();
		}
	}

	void VDiskClient::_HandleDisconnect()
	{
		Stop();
	}


	void VDiskClient::_VDiskRefresh()
	{
		service_.RefreshBufferEx();

		ConstructDSTATUS(&buf_[0], STATUS_SUCCESS, sizeof(DSTATUS));
		_NetworkSend(sizeof(DSTATUS));
	}

	void VDiskClient::_VDiskQuery()
	{
		if( service_.GetSocket() == INVALID_SOCKET )
			service_.SetSocket(*(socket_.Get()));

		LPDSTATUS lpStatus = ConstructDSTATUS(&buf_[0], STATUS_SUCCESS, sizeof(DEVINFO));

		LPDEVINFO lpDevInfo		= reinterpret_cast<LPDEVINFO>(&buf_[sizeof(DSTATUS)]);
		lpDevInfo->Type			= 0;
		lpDevInfo->Size			= utility::GetDiskSize(service_.GetFilePath().c_str());

		// 发送数据
		_NetworkSend(sizeof(DSTATUS) + sizeof(DEVINFO));
	}


	void VDiskClient::_VDiskRead(const DRP &drp)
	{
		// 试图从DiskCache加载
		const DWORD length			= drp.Read.Length;
		const LARGE_INTEGER offset	= drp.Read.Offset;

		ConstructDSTATUS(&buf_[0], STATUS_SUCCESS, length);
			
		Add(g_flow.systemRead_, 1);
		Add(g_flow.systemSize_, length);

		std::pair<BufferPtr, bool> ret = service_.GetDiskCache().Get(offset.QuadPart, length);
		if( !ret.second )
		{
			Add(g_flow.diskRead_, 1);
			Add(g_flow.diskSize_, length);

			// 读取文件
			_FileRead(length, offset);
		}
		else
		{
			Add(g_flow.cacheRead_, 1);
			Add(g_flow.cacheSize_, length);

			// 发送
			std::copy(ret.first.get(), ret.first.get() + length, &buf_[sizeof(DSTATUS)]);

			_NetworkSend(length + sizeof(DSTATUS));
		}
	}

	void VDiskClient::_VDiskWrite()
	{
		// do nonthing
		assert(0);
	}
}