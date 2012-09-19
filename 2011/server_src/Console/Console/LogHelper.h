#ifndef __LOG_HELPER_HPP
#define __LOG_HELPER_HPP


#include "../../../include/Log/Log.hpp"
#include "LogRecord.h"
#include "Misc.h"

#include <functional>

namespace i8desk
{

	class FileStorage
		: public logsystem::storage::ILogStorage<TCHAR>
	{
		struct StorageImpl;
		std::auto_ptr<StorageImpl> impl_;

	public:
		explicit FileStorage(LPCTSTR path);
		virtual ~FileStorage();

		virtual void Put(char_type ch);
		virtual void Put(size_t szCount, char_type ch);
		virtual void Put(const char_type *pStr, size_t szCount);
		virtual void Put(const char_type *fmt, va_list args);

		virtual void Flush();
	};



	class ExceptionStorage
		: public logsystem::storage::ILogStorage<TCHAR>
	{
		struct StorageImpl;
		std::auto_ptr<StorageImpl> impl_;

		typedef std::tr1::function<void (const LogRecordPtr &)> Callback;

	public:
		ExceptionStorage(const Callback &callback);
		virtual ~ExceptionStorage();

		virtual void Put(char_type ch);
		virtual void Put(size_t szCount, char_type ch);
		virtual void Put(const char_type *pStr, size_t szCount);
		virtual void Put(const char_type *fmt, va_list args);

		virtual void Flush();
	};


	// ----------------------------------------------
	// class LogHelper

	class LogHelper
	{
	public:
		typedef logsystem::storage::CacheStorageT<512, FileStorage, logsystem::storage::ILogStorage<TCHAR>> Log1StorageType;
		typedef logsystem::storage::CacheStorageT<512, ExceptionStorage, logsystem::storage::ILogStorage<TCHAR>> Log2StorageType;

		typedef logsystem::MultiLogT<TCHAR, logsystem::filter::DefaultFilter>		MultiLog;

	private:
		std::auto_ptr<Log1StorageType> log1_;
		std::auto_ptr<Log2StorageType> log2_;

		MultiLog logs_;

	public:
		LogHelper()
		{
			_Init();
		}
		~LogHelper()
		{
			OutputDebugStringW(L"~LogMgr\n");
		}

	private:
		LogHelper(const LogHelper &);
		LogHelper &operator=(const LogHelper &);

	public:
		operator MultiLog&()
		{
			return logs_;
		}

		operator const MultiLog &()
		{
			return logs_;
		}

	private:
		void _Init();
	};

	extern std::auto_ptr<LogHelper> LOG;

	inline LogHelper::MultiLog &Log()
	{
		if( LOG.get() == 0 )
			LOG.reset(new LogHelper);

		return *LOG;
	}

	inline void DestroyLog()
	{
		LOG.reset();
	}
}


#endif