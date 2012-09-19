#include "stdafx.h"
#include "Client.hpp"

#include "Service.hpp"

#include "Misc.hpp"




namespace i8desk
{


	using namespace async::iocp;
	using namespace async::network;
	using namespace std::tr1::placeholders;


	

	VDiskClient::VDiskClient(const SocketPtr &socket, VDiskService &service)
		: service_(service)
		, socket_(socket)
		, file_(service_.GetService(), service_.GetFilePath().c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED)
		, buf_(VDiskMemoryMgr::PackageSize, BufferContainerType(bufPool_))
	{}


	void VDiskClient::Start()
	{
		try
		{		
			AsyncRead(socket_, Buffer(buf_), TransferAtLeast(sizeof(DRP)),
				std::tr1::bind(&VDiskClient::_HandleNetworkRead, shared_from_this(), 
				_1, _2));

		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	void VDiskClient::Stop()
	{
		socket_.Close();
		file_.Close();
	}

	void VDiskClient::_NetworkSend()
	{
		try
		{
			// 发送数据
			AsyncWrite(socket_, Buffer(buf_), TransferAll(), 
				std::tr1::bind(&VDiskClient::_HandleNetworkWrite, shared_from_this(), _1, _2));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	void VDiskClient::_FileRead(size_t len, const u_int64 &offset)
	{
		try
		{
			AsyncRead(file_, Buffer(buf_.data(sizeof(DSTATUS)), len), offset, TransferAtLeast(len),
				std::tr1::bind(&VDiskClient::_HandleFileRead, shared_from_this(), _1, _2));
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
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

			DRP *pDrp = reinterpret_cast<DRP *>(buf_.data());
			
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
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	void VDiskClient::_HandleFileRead(u_long bytes, u_long error)
	{
		try
		{
			if( bytes == 0 )
			{
				socket_.AsyncDisconnect(std::tr1::bind(&VDiskClient::_HandleDisconnect, shared_from_this()));
				return;
			}
	
			ConstructDSTATUS(buf_.data(), STATUS_SUCCESS, bytes, bytes);
			buf_.resize(bytes + sizeof(DSTATUS));

			_NetworkSend();
		}
		catch(std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	void VDiskClient::_HandleDisconnect()
	{
		Stop();
	}


	void VDiskClient::_VDiskRefresh()
	{
		service_.RefreshBufferEx();

		ConstructDSTATUS(buf_.data(), STATUS_SUCCESS, 0, 0);
		buf_.resize(sizeof(DSTATUS));

		_NetworkSend();
	}

	void VDiskClient::_VDiskQuery()
	{
		if( service_.GetSocket() == INVALID_SOCKET )
			service_.SetSocket(*(socket_.Get()));

		LPDSTATUS lpStatus = ConstructDSTATUS(buf_.data(), STATUS_SUCCESS, sizeof(DEVINFO), sizeof(DEVINFO));

		LPDEVINFO lpDevInfo		= reinterpret_cast<LPDEVINFO>(lpStatus->buffer);
		lpDevInfo->Type			= 0;
		utility::GetDiskSize(service_.GetFilePath().c_str(), lpDevInfo->Size);

		buf_.resize(sizeof(DSTATUS) + sizeof(DEVINFO));

		// 发送数据
		_NetworkSend();
	}


	void VDiskClient::_VDiskRead(const DRP &drp)
	{
		// 组包头
		LPDSTATUS lpStatus = ConstructDSTATUS(buf_.data(), STATUS_SUCCESS, drp.Read.Length, drp.Read.Length);

		// 试图从DiskCache加载
		DWORD dwLength = drp.Read.Length;
		bool bNeedRead = true;

		if( dwLength <= DEFAULT_BLOCK )
		{
			bNeedRead = !_ReadCacheData(drp, dwLength, lpStatus->buffer);
		}

		if( bNeedRead )
		{
			// 读取文件
			const LARGE_INTEGER &offset(drp.Read.Offset);
			buf_.resize(dwLength + sizeof(DSTATUS));

			_FileRead(dwLength, offset.QuadPart);
		}
		else
		{
			// 发送
			buf_.resize(sizeof(DSTATUS) + dwLength);
			_NetworkSend();
		}
	}

	void VDiskClient::_VDiskWrite()
	{
		// do nonthing
		assert(0);
	}


	bool VDiskClient::_ReadCacheData(const DRP &drp, DWORD dwLength, char *buf)
	{
		LARGE_INTEGER begin  = {0};
		LARGE_INTEGER offset = {0};

		begin.QuadPart  = drp.Read.Offset.QuadPart;
		DWORD dwLeft	= begin.QuadPart % DEFAULT_BLOCK;
		offset.QuadPart = begin.QuadPart - dwLeft;

		// 
		if( DEFAULT_BLOCK - dwLength < begin.QuadPart - offset.QuadPart )
			return false;

	
		typedef VDiskService::DiskCachePtr	DiskCachePtr;
	
		DiskCachePtr &diskCache = service_.GetDiskCache();

		cache::DiskCache::Cache::const_iterator iter = diskCache->cache_.find(offset.QuadPart);

		// 找到缓存
		if( iter != diskCache->cache_.end() )
		{
			std::copy(iter->second + dwLeft, iter->second + dwLeft + dwLength, buf);
			return true;
		}
		else
		{
			// 读取数据放到缓存
			if( service_.GetSocket() == socket_.Get()->GetHandle() && !diskCache->cache_.overflow() )
			{
				char *cacheBuf = diskCache->diskPool_.allocate(1);
				long nHigh = offset.HighPart;
				DWORD dwReadBytes = 0;

				::SetFilePointer(service_.GetSyncHandle(), offset.LowPart, &nHigh, FILE_BEGIN);
				if( ::ReadFile(service_.GetSyncHandle(), cacheBuf, DEFAULT_BLOCK, &dwReadBytes, NULL) )
				{
					std::copy(cacheBuf + dwLeft, cacheBuf + dwLeft + dwLength, buf);
					diskCache->cache_.insert(offset.QuadPart, cacheBuf);

					return false;
				}
				else
				{
					//LogMgr::Instance() << "使用数据缓存 ReadFile出错(" << ::GetLastError() << ")" << logsystem::Endl;
					return true;
				}
			}
		}
		

		return false;
	}
}