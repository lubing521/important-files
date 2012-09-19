#ifndef __VDISK_SERVICE_HPP
#define __VDISK_SERVICE_HPP



#include "../../../include/MultiThread/Tls.hpp"
#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Timer/Timer.hpp"

#include "MemoryManager.h"
#include "MemoryCache.hpp"


namespace i8desk
{

	// forward declare
	class VDiskAcceptor;

	// ----------------------------------------------------
	// class VDiskService

	class VDiskService
	{

	private:
		// 刷新虚拟盘使用的socket
		SOCKET refreshSock_;	

		// 版本号
		DWORD version_;

		// 需要读取文件路径
		stdex::tString filePath_;

		// IO Service
		async::iocp::IODispatcher io_;

		// Acceptor
		std::auto_ptr<VDiskAcceptor> acceptor_;

		// Timer
		async::timer::Timer time_;

		// DiskCache
		MemoryCacheMgr cache_;

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
		inline const stdex::tString &GetFilePath() const	{ return filePath_;		}
		
		inline void SetSocket(SOCKET sock)			{ refreshSock_ = sock;	}

		inline MemoryCacheMgr &GetDiskCache()		{ return cache_; }

	private:
		void _Total();
	};
}






#endif