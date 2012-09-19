#ifndef __CLIENT_DATA_BASIC_HPP
#define __CLIENT_DATA_BASIC_HPP


#include <memory>
#include "../Utility/Exception.h"


namespace i8desk
{
	namespace data
	{

		typedef std::tr1::shared_ptr<char>		BufferPtr;
		typedef std::pair<BufferPtr, DWORD>	Buffer;

		namespace detail
		{
			inline void BufferDeletor(void *p)
			{
				::operator delete (p);
			}

			inline BufferPtr BufferAllocator(DWORD len)
			{
				typedef BufferPtr::element_type ValueType;
				assert(len != 0);
				if( len == 0 )
					len += 1;

				void *tmp = ::operator new (len);
				std::uninitialized_fill_n(static_cast<char *>(tmp), len, 0);
				BufferPtr buf(static_cast<ValueType *>(tmp), &BufferDeletor);
				return buf;
			}

		}
		

		inline Buffer MakeBuffer(DWORD size)
		{
			return std::make_pair(detail::BufferAllocator(size), size);
		}

		// Ä£Äâ static_assert
		template < typename Size >
		struct static_assert
		{
			~static_assert()
			{
				char tmp[Size];
				static_cast<void *>(tmp);
			}
		};
	}
}



#endif