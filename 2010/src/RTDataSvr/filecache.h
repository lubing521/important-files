/**
 * filecache.h
 * 文件缓冲 将一些只需要读的文件缓存在内存中以提高文件处理的效率
 * 文件缓冲的用户必须自己处理打开缓冲失败的情况
 * 而打开缓冲文件失败是一种正常的情况，且可能很常见
 *
**/
#ifndef I8_FILECACHE_H
#define I8_FILECACHE_H

#include "../../include/sync.h"
#include "../../include/Extend STL/container.h"
#include "../../include/Utility/utility.h"
#include "globalfun.h"

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>


#define I8_FILECACHE_MAX_TOTALSIZE (1024*1024*256)
#define I8_FILECACHE_MAX_FILESIZE  (1024*1024*32)

namespace i8desk {

class ZCacheFile
{
public:
	ZCacheFile(const std::string& strFileName, 
		BYTE *data, 
		DWORD size, 
		ULONGLONG ftLastWriteTime)
		: m_strFileName(strFileName)
		, m_data(data)
		, m_size(size)
		, m_crc(CalBufCRC32(data, size))
		, m_ftLastWriteTime(ftLastWriteTime)
	{
	}

	~ZCacheFile()
	{
		I8MP.free(m_data);
	}

	std::string GetFileName(void) const 
	{
		return m_strFileName; 
	}
	DWORD GetSize(void) const 
	{
		return m_size; 
	}
	ulong GetCrc(void) const 
	{
		return m_crc; 
	}
	ULONGLONG GetLastWriteTime(void) const 
	{
		return m_ftLastWriteTime; 
	}
	BOOL ReadData(DWORD& offset, BYTE *pbuf, DWORD& len) const
	{
		if (offset < m_size) {
			len = min(len, m_size - offset);
			memcpy(pbuf, m_data + offset, len);
			offset += len;
			return TRUE;
		}
		return FALSE;
	}

private:
	std::string m_strFileName;
	BYTE *m_data;
	DWORD m_size;
	ulong m_crc;
    ULONGLONG m_ftLastWriteTime;
};
typedef std::tr1::shared_ptr<ZCacheFile> CacheFile_Ptr;

class ZFileCacheMgr
{
public:
	ZFileCacheMgr(void) : m_dwTotalSize(0)
	{
	}

	~ZFileCacheMgr(void)
	{
	}

	CacheFile_Ptr Open(LPCSTR lpFileName)
	{
		CacheFile_Ptr f;
		if (m_ids.peek(lpFileName, f)) {
			return f;
		}
		
		{
			I8_GUARD(ZGuard, ZSync, m_lock);
			if (m_dwTotalSize > I8_FILECACHE_MAX_TOTALSIZE)
				return f;
		}
	
		ULONGLONG ftLastWriteTime;
		if (!GetFileLastWriteTime(lpFileName, ftLastWriteTime)) {
			return f;
		}

		HANDLE hFile = ::CreateFile(
			lpFileName, 
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return f;
		
		DWORD dwFileSizeHigh;
		DWORD size = ::GetFileSize(hFile, &dwFileSizeHigh);

		if (dwFileSizeHigh 
			|| size == 0 
			|| size > I8_FILECACHE_MAX_FILESIZE)
		{
			::CloseHandle(hFile);
			return f;
		}

		BYTE *data = I8MP.malloc<BYTE>(size);
		BOOL ok = ReadFileContent(hFile, data, size);
		::CloseHandle(hFile);

		if (!ok) {
			I8MP.free(data);
			return f;
		} 	

		f = CacheFile_Ptr(new ZCacheFile(lpFileName, data, size, ftLastWriteTime));
		m_ids.set(lpFileName, f);

		{
			I8_GUARD(ZGuard, ZSync, m_lock);
			m_dwTotalSize += size;
		}

		return f;
	}

	void Close(LPCSTR lpFileName)
	{
		CacheFile_Ptr f;
		if (m_ids.get(lpFileName, f)) {
			I8_GUARD(ZGuard, ZSync, m_lock);
			m_dwTotalSize -= f->GetSize();		
		}
	}

	BOOL Reload(LPCSTR lpFileName)
	{
		Close(lpFileName);
		return !!Open(lpFileName);
	}

	void ReloadAll(void)
	{
		typedef FileCacheList::element_type CachePair;
		typedef std::tr1::unordered_map<std::string, ULONGLONG> CacheMap;

		struct Functor : public std::unary_function<CachePair, void> {
			Functor(CacheMap& files) : m_files(files) {}
			result_type operator()(const argument_type& _Arg) {
				m_files[_Arg.first] = _Arg.second->GetLastWriteTime();
			}
		private:
			CacheMap& m_files;
		};
		
		CacheMap files;
		m_ids.foreach(Functor(files));

		for (CacheMap::iterator it = files.begin(); it != files.end(); ++it) 
		{
			//修改时间有变化才可能重新加载（避免无谓的打开和读取文件）
			ULONGLONG ftLastWriteTime;
			if (GetFileLastWriteTime(it->first.c_str(), ftLastWriteTime) 
				&& ftLastWriteTime != it->second)
			{
				Reload(it->first.c_str()); 
			}
		}
	}

private:
	typedef ZAssociatedContainer<std::string, CacheFile_Ptr,
		std::map<std::string, CacheFile_Ptr>, ZSync> FileCacheList;

	FileCacheList m_ids;

	ZSync m_lock;
	DWORD m_dwTotalSize;
};

} // namespace i8desk


#endif //I8_FILECACHE_H
