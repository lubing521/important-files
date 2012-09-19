#ifndef __VDISK_DATA_CACHE_HPP
#define __VDISK_DATA_CACHE_HPP

#include "../../../include/MultiThread/Lock.hpp"
#include <unordered_map>


namespace i8desk
{

	//----------------------------------------------------
	// class DataCache

	template<typename KeyT, typename ValueT, typename LockT = async::thread::AutoCriticalSection>
	class DataCache
	{
	public:
		typedef KeyT			KeyType;
		typedef ValueT			ValueType;
		typedef LockT			LockType;

	private:
		typedef async::thread::AutoLock<LockType>			AutoLock;
		typedef std::tr1::unordered_map<KeyType, ValueType>	CacheType;

	public:
		typedef typename CacheType::iterator				iterator;
		typedef typename CacheType::const_iterator			const_iterator;

	

	private:
		mutable LockType	lock_;
		CacheType			cache_;
		size_t				maxSize_;

	public:
		explicit DataCache(size_t maxSize)
			: maxSize_(maxSize)
		{}

	private:
		DataCache(const DataCache &);
		DataCache &operator=(const DataCache &);

	public:
		bool insert(const KeyType &key, const ValueType &val)
		{
			AutoLock lock(lock_);

			if( cache_.size() > maxSize_ )
				return false;

			cache_.insert(std::make_pair(key, val));

			return true;
		}

		void erase(iterator iter)
		{
			AutoLock lock(lock_);

			cache_.erase(iter);
		}

		iterator find(const KeyType &key)
		{
			AutoLock lock(lock_);

			return cache_.find(key);
		}

		template<typename CallbackT>
		void clear(const CallbackT &callback)
		{
			AutoLock lock(lock_);
			
			for(iterator iter = cache_.begin(); iter != cache_.end(); ++iter)
				callback(iter->second);

			//std::for_each(cache_.begin(), cache_.end(), callback);

			cache_.clear();
		}

		bool overflow() const
		{		
			AutoLock lock(lock_);

			return cache_.size() > maxSize_;
		}

		iterator begin()
		{
			AutoLock lock(lock_);

			return cache_.begin();
		}
		const_iterator begin() const
		{
			AutoLock lock(lock_);

			return cache_.begin();
		}

		iterator end()
		{
			AutoLock lock(lock_);

			return cache_.end();
		}
		const_iterator end() const
		{
			AutoLock lock(lock_);

			return cache_.end();
		}
	};
}



#endif