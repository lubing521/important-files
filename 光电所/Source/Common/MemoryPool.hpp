#ifndef __COMMON_MEMORY_POOL_HPP
#define __COMMON_MEMORY_POOL_HPP

#include <memory>
#include "PackageHelper.hpp"
#include "../../include/MemoryPool/SGIMemoryPool.hpp"
#include "../../include/MemoryPool/FixedMemoryPool.hpp"

namespace common
{
	typedef async::memory::SGIMemoryPool<true, MAX_UDP_PACKAGE> MemoryPool;
	typedef async::memory::SGIMemoryPool<true, MAX_BMP_PACKAGE> BMPMemoryPool;


	typedef std::pair<std::tr1::shared_ptr<char>, size_t>	Buffer;
	typedef std::pair<common::Buffer, std::wstring>			BmpInfo;

	namespace detail
	{
		struct NormalPool
		{
			char *Allocate(size_t len)
			{
				return (char *)::operator new(len);
			}
			void Deallocate(void *p, size_t)
			{
				::operator delete (p);
			}
		};
		

		template < typename PoolT >
		inline Buffer MakeBuffer(size_t len)
		{
			static PoolT pool;
			return std::make_pair(std::tr1::shared_ptr<char>((char *)pool.Allocate(len), 
				std::tr1::bind(&PoolT::Deallocate, std::tr1::ref(pool), std::tr1::placeholders::_1, len)), len);
		}
	}
	
	inline Buffer MakeBuffer(size_t len)	
	{
		assert(len <= MAX_UDP_PACKAGE);
		return detail::MakeBuffer<MemoryPool>(len);
	}
	

	inline Buffer MakeBMPBuffer(size_t len)	
	{
		assert(len <= MAX_BMP_PACKAGE);
		return detail::MakeBuffer<BMPMemoryPool>(len);
	}

	inline Buffer MakeNormalBuffer(size_t len)	
	{
		return detail::MakeBuffer<detail::NormalPool>(len);
	}
}





#endif