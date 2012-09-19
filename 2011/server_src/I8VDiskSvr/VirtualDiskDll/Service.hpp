#ifndef __VDISK_SERVICE_HPP
#define __VDISK_SERVICE_HPP


#include "Client.hpp"

#include "../../../include/AsyncIO/MultiThread/Thread.hpp"
#include "../../../include/AsyncIO/MultiThread/Tls.hpp"

#include "DataCache.h"


namespace i8desk
{

	namespace cache
	{
		struct DiskCache
		{
			typedef DataCache<__int64, char *>			Cache;
			typedef VDiskMemoryMgr::DiskDataAllocType	DiskPool;

			Cache cache_;
			DiskPool diskPool_;

			DiskCache()
				: cache_(10000)
			{

			}
		};
	}


	// ----------------------------------------------------
	// class VDiskService

	class VDiskService
	{
	public:
		typedef async::thread::TssPtr<cache::DiskCache>	DiskCachePtr;

	private:
		// 刷新虚拟盘使用的socket
		SOCKET refreshSock_;	

		// 阻塞同步读取文件进行缓存
		HANDLE syncFile_;

		// 版本号
		DWORD version_;

		// 需要读取文件路径
		stdex::tString filePath_;

		// 线程局部存储
		DiskCachePtr cachePtr_;

		// IO Service
		async::iocp::IODispatcher io_;
		// Socket Acceptor
		async::network::Tcp::Accpetor acceptor_;
		// Socket Acceptor Thread
		async::thread::ThreadImplEx thread_;

	public:
		VDiskService();

	private:
		VDiskService(const VDiskService &);
		VDiskService &operator=(const VDiskService &);


	public:
		// 启动
		DWORD Start(LPCTSTR ip, WORD port, LPCTSTR filePath);
		// 结束
		void Stop();

		// 获取分区信息
		void  GetVDiskInfo(PDWORD pdwConnect, PDWORD pdwVersion);
		// 刷盘
		void  RefreshBufferEx();

	public:
		inline async::iocp::IODispatcher &GetService() { return io_;		}
		inline SOCKET GetSocket() const				{ return refreshSock_;	}
		inline HANDLE GetSyncHandle() const			{ return syncFile_;		}
		inline const stdex::tString &GetFilePath() const	{ return filePath_;		}
		inline DiskCachePtr &GetDiskCache() 		{ return cachePtr_;		}

		inline void SetSocket(SOCKET sock)			{ refreshSock_ = sock;	}

	private:
		void _OnAccept(u_long error, const async::network::SocketPtr &remoteSocket);

	private:
		DWORD _Thread();
	};
}






#endif