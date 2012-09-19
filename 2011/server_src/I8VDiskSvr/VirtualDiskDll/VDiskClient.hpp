#ifndef __VDISK_CLIENT_HPP
#define __VDISK_CLIENT_HPP

#include "../../../include/Extend STL/UnicodeStl.h"

#include "../../../include/AsyncIO/Network/TCP.hpp"
#include "../../../include/AsyncIO/FileSystem/BasicFile.hpp"

#include "MemoryManager.h"


namespace i8desk
{

	class VDiskService;

	class VDiskClient
		: public std::tr1::enable_shared_from_this<VDiskClient>
	{
		typedef VDiskMemoryMgr::PackageAllocType					BufferContainerType;
		typedef VDiskMemoryMgr::PackagePoolType						BufferPoolType;
		typedef async::iocp::AutoBufferT<char, BufferContainerType>	AutoBufferType;

	private:
		VDiskService &					service_;
		async::network::Tcp::Socket		socket_;
		async::filesystem::BasicFile	file_;

		std::vector<char>				buf_;


	public:
		VDiskClient(const async::network::SocketPtr &socket, VDiskService &service);

	public:
		void Start();
		void Stop();

	private:
		VDiskClient(const VDiskClient &);
		VDiskClient &operator=(const VDiskClient &);

	private:
		void _HandleNetworkRead(u_long bytes, u_long error);
		void _HandleNetworkWrite(u_long bytes, u_long error);
		void _HandleDisconnect();

		void _HandleFileRead(u_long bytes, u_long error, const LARGE_INTEGER &offset);

	private:
		void _NetworkSend(size_t len);
		void _FileRead(size_t len, const LARGE_INTEGER &offset);

		void _VDiskRefresh();
		void _VDiskQuery();
		void _VDiskRead(const DRP &);
		void _VDiskWrite();
	};


	typedef std::tr1::shared_ptr<VDiskClient> VDiskClientPtr;

	template <typename ServiceT>
	inline VDiskClientPtr CreateVDiskClient(const async::network::SocketPtr &socket, ServiceT &service)
	{
		return VDiskClientPtr(async::iocp::ObjectAllocate<VDiskClient>(socket, service), 
			&async::iocp::ObjectDeallocate<VDiskClient>);
	}
}

#include <memory>
namespace async
{
	namespace iocp
	{
		inline AsyncCallbackBase *MakeAsyncCallback(const CallbackType &handler, CallbackType *addr)
		{
			return ObjectAllocate<AsyncCallback>(handler);
		}


		inline void ReleaseAsyncCallback(AsyncCallbackBase *p, CallbackType *addr)
		{
			return ObjectDeallocate<AsyncCallback>(static_cast<AsyncCallback *>(p));
		}

	}

}

// 定制自己的工厂
namespace async
{
	namespace iocp
	{
		template<>
		struct ObjectFactory< i8desk::VDiskClient >
		{
			typedef async::memory::FixedMemoryPool<true, sizeof(i8desk::VDiskClient)>	PoolType;
			typedef ObjectPool< PoolType >												ObjectPoolType;
		};
	}
}


#endif