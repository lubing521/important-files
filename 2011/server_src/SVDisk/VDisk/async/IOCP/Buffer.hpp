#ifndef __IOCP_BUFFER_HPP
#define __IOCP_BUFFER_HPP



#include "Object.hpp"


#pragma warning(disable: 4996) // 去掉

namespace async
{


	namespace IOCP
	{

		//-------------------------------------------------------------------
		// class BufferT
		// 提供缓冲区服务，支持STL标准接口

		template<typename T, size_t __DefaultSize, typename AllocT>
		class BufferT
			: public Object
		{
		public:
			typedef T			value_type;
			typedef T*			pointer;
			typedef const T*	const_pointer;
			typedef T&			reference;
			typedef const T&	const_reference;

			// 内存分配类型
			typedef typename AllocT::alloc_type alloc_type;

		private:
			// 类属内存池
			static alloc_type BufferPool;
			AllocT alloc_;		// 分配器

			size_t allocSize_;	// 已分配缓冲区大小
			size_t bufferSize_;	// 已使用缓冲区大小

			value_type *buffer_;// 缓冲区指针
			bool internal_;		// 是否外部提供缓冲区

		public:
			BufferT()
				: alloc_(BufferPool)
				, allocSize_(__DefaultSize)
				, bufferSize_(0)
				, buffer_(_Allocate(allocSize_))
				, internal_(true)
			{}
			explicit BufferT(size_t nSize)
				: alloc_(BufferPool)
				, allocSize_(nSize)
				, bufferSize_(0)
				, internal_(true)
			{
				if( nSize < 4 )
					allocSize_ = 4;

				buffer_ = _Allocate(allocSize_);
			}
			BufferT(pointer pStr, size_t nSize)
				: alloc_(BufferPool)
				, allocSize_(nSize)
				, bufferSize_(nSize)
				, buffer_(pStr)
				, internal_(false)
			{
			}
			BufferT(pointer beg, const_pointer end)
				: alloc_(BufferPool)
				, allocSize_(std::distance(beg, end))
				, bufferSize_(allocSize_)
				, buffer_(beg)
				, internal_(false)
			{
			}

			~BufferT()
			{
				// 如果为内部分配缓冲区，则需要自动释放
				if( internal_ )
					alloc_.deallocate(buffer_, allocSize_);
			}

		public:
			pointer begin()
			{
				return buffer_;
			}
			const_pointer begin() const
			{
				return buffer_;
			}

			pointer end()
			{
				return buffer_ + bufferSize_;
			}
			const_pointer end() const
			{
				return buffer_ + bufferSize_;
			}

			size_t size() const
			{
				return bufferSize_;
			}
			size_t allocSize() const
			{
				return allocSize_;
			}

			void resize(size_t nNewSize)
			{
				if( !internal_ )
					return;

				if( nNewSize <= allocSize_ )
					bufferSize_ = nNewSize;
				else
				{
					// 申请新缓冲区
					allocSize_ = nNewSize;
					pointer pNewBuf = _Allocate(allocSize_);

					// 复制缓冲区
					std::copy(buffer_, buffer_ + bufferSize_, pNewBuf);

					// 释放旧缓冲区
					alloc_.deallocate(buffer_, allocSize_);

					buffer_ = pNewBuf;
				}
			}

			pointer data(size_t offset = 0)
			{
				assert(offset < allocSize_);
				return buffer_ + offset;
			}
			const_pointer data(size_t offset = 0) const
			{
				assert(offset < allocSize_);
				return buffer_ + offset;
			}

			pointer operator[](size_t offset)
			{
				return buffer_ + offset;
			}
			const_pointer operator[](size_t offset) const
			{
				return buffer_ + offset;
			}

		private:
			pointer _Allocate(size_t nSize)
			{
				pointer pBuf = alloc_.allocate(nSize);
				std::uninitialized_fill_n(pBuf, nSize, 0);

				return pBuf;
			}
		};

		template<typename T, size_t __DefaultSize, typename AllocT>
		typename BufferT<T, __DefaultSize, AllocT>::alloc_type BufferT<T, __DefaultSize, AllocT>::BufferPool;


	} // end of IOCP


} // end of async





#endif