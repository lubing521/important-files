#ifndef __LOG_BASIC_HPP
#define __LOG_BASIC_HPP

#include "Storage/Storage.hpp"
#include "Storage/CacheStorage.hpp"
#include "Storage/ThreadSafeStorage.hpp"


#include "Log/Log.hpp"
#include "Log/ScopeLog.hpp"
#include "Log/ThreadLog.hpp"

#include "Filter/Priority.hpp"
#include "Format/Format.hpp"




namespace logsystem
{
	// ----------------------------------------------------------
	// storage

	typedef storage::FileStorageT<char>			FileStorage;
	typedef storage::FileStorageT<wchar_t>		FileStorageW;

	typedef storage::DebugStorageT<char>		DebugStorage;
	typedef storage::DebugStorageT<wchar_t>		DebugStorageW;

	typedef storage::StringStorageT<std::basic_string<char>>		StringStorage;
	typedef storage::StringStorageT<std::basic_string<wchar_t>>		StringStorageW;



	// -----------------------------------------------------
	// logsystem

	typedef impl::Log<FileStorage, filter::DefaultFilter>		FileLog;
	typedef impl::Log<FileStorageW, filter::DefaultFilter>		FileLogW;

	typedef impl::Log<DebugStorage, filter::DefaultFilter>		DebugLog;
	typedef impl::Log<DebugStorageW, filter::DefaultFilter>		DebugLogW;

	typedef impl::Log<StringStorage, filter::DefaultFilter>		StringLog;
	typedef impl::Log<StringStorageW, filter::DefaultFilter>	StringLogW;



	typedef impl::FileScopeLogT<char, true, filter::DefaultFilter>		FileScopeLog;
	typedef impl::FileScopeLogT<wchar_t, true, filter::DefaultFilter>	FileScopeLogW;


	typedef impl::ThreadLog<FileLog>							ThreadFileLog;
	typedef impl::ThreadLog<FileLogW>							ThreadFileLogW;

	
	// ---------------------------------------------------------------------
	// class  MultiLog

	template<typename CharT, size_t _Storage, typename FilterT, class StorageContainer = std::tr1::array<storage::ILogStorage<CharT>*, _Storage> >
	class MultiLogT
		: public impl::Log<storage::MultiStorage<CharT, _Storage, StorageContainer>, FilterT >
	{
	public:
		template<typename T>
		void Add(T &val)
		{
			GetStorage().Add(val);
		}
	};
	
}






#endif