#ifndef __I8DESK_DOWNLOAD_SCHEDULER_HPP
#define __I8DESK_DOWNLOAD_SCHEDULER_HPP


#include <list>
#include <limits>

#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MemoryPool/SGIMemoryPool.hpp"
#include "../../../include/Allocator/ContainerAllocator.hpp"


namespace i8desk
{

	namespace detail
	{
		template<typename T>
		struct AllocatorTraits
		{
			typedef async::memory::SGIMTMemoryPool						MemoryPool;
			typedef async::allocator::ContainerAllocator<T, MemoryPool>	Allocator;

			static Allocator &GetAllocator()
			{
				static MemoryPool pool;
				static Allocator allocator(pool);
				return allocator;
			}
		};
	}
	

	// ---------------------------------------
	// class SchedulerQueue

	template<typename T, size_t _Sep>
	class SchedulerQueue
	{
		typedef async::thread::AutoCriticalSection		SCLock;
		typedef async::thread::AutoLock<SCLock>			AutoLock;

	public:
		typedef T										value_type;
		typedef detail::AllocatorTraits<value_type>		AllocatorT;
		typedef typename AllocatorT::Allocator			Allocator;
		typedef std::list<value_type, Allocator>		ContainerType;		
		typedef typename ContainerType::iterator		iterator;
		typedef typename ContainerType::const_iterator	const_iterator;

	private:
		ContainerType queue_;
		mutable SCLock mutex_;

		static const size_t SEPARATE = _Sep;

	public:
		SchedulerQueue()
			: queue_(AllocatorT::GetAllocator())
		{}


		// 基本方法
	public:
		iterator Begin()
		{
			AutoLock lock(mutex_);
			return queue_.begin();
		}
		
		const_iterator Begin() const
		{
			AutoLock lock(mutex_);
			return queue_.begin();
		}

		iterator End()
		{
			AutoLock lock(mutex_);
			return queue_.end();
		}

		const_iterator End() const
		{
			AutoLock lock(mutex_);
			return queue_.end();
		}

		T Top()
		{	
			AutoLock lock(mutex_);

			if( queue_.size() <= SEPARATE )
				throw std::logic_error("List not full!");

			iterator iter = queue_.begin();
			std::advance(iter, SEPARATE);
			T tmp = *iter;
			queue_.erase(iter);

			return tmp;
		}

		void PushFront(const T &item)
		{
			AutoLock lock(mutex_);

			if( queue_.size() > SEPARATE )
			{
				iterator iter = queue_.begin();
				std::advance(iter, SEPARATE);
				queue_.insert(iter, item);
			}
			else
				queue_.push_back(item);
		}

		void PushBack(const T &item)
		{
			AutoLock lock(mutex_);
			queue_.push_back(item);
		}

		bool MoveTo(const T &item, size_t index)
		{
			AutoLock lock(mutex_);

			if( index >= queue_.size() )
				return false;

			T tmp = item;
			queue_.remove(item);
			iterator iter = queue_.begin();
			std::advance(iter, index);
			queue_.insert(iter, tmp);

			return true;
		}

		bool MoveToBack(const T &item)
		{
			AutoLock lock(mutex_);
			T tmp = item;
			queue_.remove(item);
			queue_.push_back(tmp);

			return true;
		}

		size_t Size() const
		{
			AutoLock lock(mutex_);
			return queue_.size();
		}

		void Clear()
		{
			AutoLock lock(mutex_);
			queue_.clear();
		}

		bool Empty() const
		{
			AutoLock lock(mutex_);

			return queue_.empty();
		}

		bool Full() const
		{
			AutoLock lock(mutex_);

			return queue_.size() >= SEPARATE;
		}

		bool Has(const T &val)
		{
			const_iterator iter = Find(val);
			if( iter != End() )
				return true;
			
			return false;
		}

		// 算法方法
	public:
		void Remove(const T &item)
		{
			if( Empty() )
				return;

			AutoLock lock(mutex_);
			queue_.remove(item);
		}
		template<typename Pred>
		void RemoveIf(const Pred &pred)
		{
			if( Empty() )
				return;

			AutoLock lock(mutex_);
			queue_.remove_if(pred);
		}

		template<typename Pred>
		void ForEach(const Pred &pred)
		{
			if( Empty() )
				return;

			AutoLock lock(mutex_);
			std::for_each(queue_.begin(), queue_.end(), pred);
		}


		const_iterator Find(const T &item) const
		{
			AutoLock lock(mutex_);
			// lower_bound
			return std::find(queue_.begin(), queue_.end(), item);
		}

		template<typename Pred>
		const_iterator FindIf(const Pred &pred) const
		{
			AutoLock lock(mutex_);
			// 
			return std::find_if(queue_.begin(), queue_.end(), pred);
		}

		template<typename Pred>
		void Sort(Pred &pred)
		{
			AutoLock lock(mutex_);
			
			if( queue_.size() > SEPARATE )
			{
				pred(queue_, SEPARATE);
			}
		}
	};
}



#endif