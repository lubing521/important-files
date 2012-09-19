#ifndef __ZIP_HELPER_HPP
#define __ZIP_HELPER_HPP

#include <cassert>

#include "../ExtendSTL/UnicodeStl.h"
#include "../Utility/SmartHandle.hpp"
#include "XZip/XZip.h"
#include "XZip/XUnzip.h"

#ifdef min
#undef min
#endif


namespace zip
{
	
	typedef std::tr1::shared_ptr<char>	AutoBuffer;
	typedef std::pair<AutoBuffer, size_t>	Buffer;

	namespace detail
	{
		inline void *BufferAlloc(size_t sz)
		{
			void *tmp = ::operator new(sz);
			std::uninitialized_fill_n((char *)tmp, sz, 0);
			return tmp;
		}
		inline void BufferDealloc(void *p)
		{
			return ::operator delete(p);
		}
	}

	inline Buffer MakeBuffer(size_t sz)
	{
		assert(sz != 0);
		return std::make_pair(AutoBuffer(static_cast<char *>(detail::BufferAlloc(sz)), &detail::BufferDealloc), sz);
	}

	template < typename T, typename U >
	struct AutoCloseZip
	{
		void operator()(T val)
		{
			CloseZip(val);
		}
	};

	typedef utility::CSmartHandle<HZIP, AutoCloseZip> AutoZipHandle;


	// 解压
	template < typename BufferT >
	inline bool UnZip(const BufferT &inBuf, const stdex::tString &path)		
	{
		AutoZipHandle hz = ::OpenZip(inBuf.first.get(), inBuf.second, ZIP_MEMORY, path.c_str());
		if( !hz.IsValid() )
			return false;

		ZIPENTRYW ze = {0}; 
		HRESULT hRes = ::GetZipItem(hz, -1 ,&ze); 
		if( hRes != S_OK )
			return false;

		int numitems = ze.index;
		for(int i=0; i != numitems; ++i)
		{ 
			hRes = ::GetZipItem(hz,i, &ze);
			if( hRes != S_OK )
				return false;

			hRes = ::UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
			if( hRes != S_OK )
				return false;
		}

		return true;
	}

	template < typename BufferT >
	inline BufferT UnZip(const BufferT &inBuf, BufferT (*MakeBufferFn)(size_t) = MakeBuffer)
	{
		// 解压
		AutoZipHandle hz = ::OpenZip(inBuf.first.get(), inBuf.second, ZIP_MEMORY, NULL);
		if( !hz.IsValid() )
			throw std::runtime_error("打开压缩文件出错!");

		ZIPENTRYW ze = {0}; 
		const int index = 0;
		HRESULT res = ::GetZipItemW(hz, index, &ze);
		if( res != S_OK )
			throw std::runtime_error("获取压缩文件信息出错!");

		BufferT outBuf = (*MakeBufferFn)(ze.unc_size);

		HRESULT zr = ZR_MORE; 
		unsigned long readSize = 0;
		const size_t unzipLen = 4096;
		while( zr == ZR_MORE )
		{
			const size_t leftLen = ze.unc_size - readSize;
			size_t readLen = std::min(leftLen, unzipLen);

			res = ::UnzipItem(hz,index, outBuf.first.get() + readSize, readLen, ZIP_MEMORY);

			if( res == S_OK )
				break;

			readSize += readLen;
		}

		assert(readSize == ze.unc_size);

		return outBuf;
	}

	// Oh ~NO!!
	template < typename BufferT >
	void Zip(const stdex::tString &path, const stdex::tString &name, const BufferT &buffer)
	{
		AutoZipHandle hz = ::CreateZip((void *)path.c_str(), 0, ZIP_FILENAME);
		if( !hz.IsValid() )
			throw std::runtime_error("CreateZip Error");

		HRESULT res = ::ZipAdd(hz, name.c_str(), buffer.first.get(), buffer.second, ZIP_MEMORY);
		if( res != S_OK )
			throw std::runtime_error("ZipAdd Error");
	}

}




#endif