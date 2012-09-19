#include "stdafx.h"
#include "MemoryCache.hpp"




namespace i8desk
{
	BufferPool::Pool BufferPool::pool_;

	BufferPtr BufferPool::Allocator(size_t len)
	{
		BufferPtr buf(reinterpret_cast<char *>(pool_.Allocate(len)), 
			std::tr1::bind(&BufferPool::Deallocate, std::tr1::placeholders::_1, len));

		return buf;
	}

	void BufferPool::Deallocate(char *p, size_t len)
	{
		return pool_.Deallocate(p, len);
	}


	namespace detail
	{

		inline size_t ROUNDUP(size_t bytes)
		{
			return ((bytes) + Size::Size4K - 1) & ~(Size::Size4K - 1);
		}



		template < typename D, typename I, typename T >
		void Insert(D &d, I &i, T &val, size_t N)
		{
			d.insert(std::make_pair(N, 
				std::tr1::bind(&T::get, std::tr1::ref(val), std::tr1::placeholders::_1)));

			i.insert(std::make_pair(N, 
				std::tr1::bind(&T::insert, std::tr1::ref(val), std::tr1::placeholders::_1, std::tr1::placeholders::_2)));
		}

	}



	MemoryCacheMgr::MemoryCacheMgr()
		: /*cache4K_(1024 * 100)
		, cache8K_(1024 * 100)
		, cache12K_(1024 * 100)
		, cache16K_(1024 * 100)
		, cache20K_(1024 * 100)
		, cache24K_(1024 * 100)
		, cache28K_(1024 * 100)
		, cache32K_(1024 * 100)
		, cache36K_(1024 * 100)
		, cache40K_(1024 * 100)
		, cache44K_(1024 * 100)
		, cache48K_(1024 * 100)
		, cache52K_(1024 * 100)
		, cache56K_(1024 * 100)
		, cache60K_(1024 * 100)
		,*/ cache64K_(1024 * 1024)
	{
		/*detail::Insert(getDispatch_, insertDispatch_, cache4K_, detail::Size::Size4K);
		detail::Insert(getDispatch_, insertDispatch_, cache8K_, detail::Size::Size8K);
		detail::Insert(getDispatch_, insertDispatch_, cache12K_, detail::Size::Size12K);
		detail::Insert(getDispatch_, insertDispatch_, cache16K_, detail::Size::Size16K);
		detail::Insert(getDispatch_, insertDispatch_, cache20K_, detail::Size::Size20K);
		detail::Insert(getDispatch_, insertDispatch_, cache24K_, detail::Size::Size24K);
		detail::Insert(getDispatch_, insertDispatch_, cache28K_, detail::Size::Size28K);
		detail::Insert(getDispatch_, insertDispatch_, cache32K_, detail::Size::Size32K);
		detail::Insert(getDispatch_, insertDispatch_, cache36K_, detail::Size::Size36K);
		detail::Insert(getDispatch_, insertDispatch_, cache40K_, detail::Size::Size40K);
		detail::Insert(getDispatch_, insertDispatch_, cache44K_, detail::Size::Size44K);
		detail::Insert(getDispatch_, insertDispatch_, cache48K_, detail::Size::Size48K);
		detail::Insert(getDispatch_, insertDispatch_, cache52K_, detail::Size::Size52K);
		detail::Insert(getDispatch_, insertDispatch_, cache56K_, detail::Size::Size56K);
		detail::Insert(getDispatch_, insertDispatch_, cache60K_, detail::Size::Size60K);*/
		detail::Insert(getDispatch_, insertDispatch_, cache64K_, detail::Size::Size64K);
	}

	std::pair<BufferPtr, bool> MemoryCacheMgr::Get(unsigned __int64 offset, size_t len)
	{
		/*auto iter = getDispatch_.find(detail::ROUNDUP(len));
		if( iter != getDispatch_.end() )
			return std::move(iter->second(offset));
		else
			throw std::runtime_error("没有找到对应缓存");*/

		return cache64K_.get(offset);

	}

	void MemoryCacheMgr::Insert(unsigned __int64 key, const BufferPtr &val, size_t len)
	{
		/*auto iter = insertDispatch_.find(detail::ROUNDUP(len));
		if( iter != insertDispatch_.end() )
			return iter->second(key, val);
		else
			throw std::runtime_error("没有找到对应缓存");*/
		return cache64K_.insert(key, val);
	}
}