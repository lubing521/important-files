#ifndef __THREAD_POOL_HELPER_HPP
#define __THREAD_POOL_HELPER_HPP

#include "../../../include/MultiThread/ThreadPool.hpp"
#include "../../../include/MemoryPool/SGIMemoryPool.hpp"
#include "../../../include/MultiThread/Thread.hpp"


namespace async
{
	namespace thread
	{
		
		namespace detail
		{
			typedef async::memory::SGIMTMemoryPool Pool;

			inline Pool &MemoryPool()
			{
				static Pool pool;
				return pool;
			}
		}
		
		// ¶¨ÖÆHookº¯Êý
		inline void *HandlerAllocateHook(size_t sz, void *p)
		{
			return detail::MemoryPool().Allocate(sz);
		}

		inline void HandlerDeallocateHook(size_t sz, void *p, void *addr)
		{
			detail::MemoryPool().Deallocate(p, sz);
		}
	}
}



#endif