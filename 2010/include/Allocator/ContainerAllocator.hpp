#ifndef __ALLOCATOR_CONTAINER_ALLOCATOR_HPP
#define __ALLOCATOR_CONTAINER_ALLOCATOR_HPP


#include "../MemoryPool/SGIMemoryPool.hpp"
#include "../Utility/Singleton.hpp"



namespace DataStructure
{
	template<typename T, bool __IsMT = true, size_t __Size = 256>
	class ContainerAllocator
	{
	public:
		
		typedef SGIMemoryPool<__IsMT, __Size>		MemoryPoolType;
		typedef Singleton<MemoryPoolType, __IsMT>	SingletonType;

		typedef T							value_type;
		typedef value_type *				pointer;
		typedef const value_type *			const_pointer;
		typedef value_type &				reference;
		typedef const value_type &			const_reference;
		typedef size_t						size_type;
		typedef ptrdiff_t					difference_type;

		template<typename U>
		struct rebind
		{
			typedef ContainerAllocator<U, __IsMT, __Size> other;
		};

	public:
		ContainerAllocator()
		{
		}
		ContainerAllocator(const ContainerAllocator &rhs)
		{
		}
		template<typename U, bool IsMT, size_t size>
		ContainerAllocator(const ContainerAllocator<U, IsMT, size>& rhs)
		{
		}
		~ContainerAllocator()
		{
		}

	private:
		ContainerAllocator &operator==(const ContainerAllocator &);


	public:
		static pointer address(reference r)
		{ return &r; }
		static const_pointer address(const_reference s)
		{ return &s; }
		static size_type max_size()
		{ return (std::numeric_limits<size_type>::max)(); }
		static void construct(const pointer ptr, const value_type & t)
		{ new (ptr) T(t); }
		static void destroy(const pointer ptr)
		{
			ptr->~T();
			(void) ptr; // avoid unused variable warning
		}

		bool operator==(const ContainerAllocator &) const
		{ return true; }
		bool operator!=(const ContainerAllocator &) const
		{ return false; }

		static pointer allocate(const size_type n)
		{
			return reinterpret_cast<pointer>(SingletonType::Instance().Allocate(n * sizeof(value_type)));
		}
		static pointer allocate(const size_type n, const void * const)
		{ return allocate(n); }

		static void deallocate(const pointer ptr, const size_type n)
		{
			if (ptr == 0 || n == 0)
				return;

			SingletonType::Instance().Deallocate(ptr, n * sizeof(value_type));
		}
	};


	template<bool __IsMT, size_t __Size>
	class ContainerAllocator<void, __IsMT, __Size>
	{
	public:
		typedef void*       pointer;
		typedef const void* const_pointer;
		typedef void        value_type;

		template<typename U> 
		struct rebind 
		{
			typedef ContainerAllocator<U, __IsMT, __Size> other;
		};
	};
}


#endif