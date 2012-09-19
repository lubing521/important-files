#ifndef __CLIENT_LOG_HPP
#define __CLIENT_LOG_HPP

#include "../../../include/Log/Log.hpp"
#include <functional>


namespace i8desk
{
	namespace log
	{
	
		// ---------------------------------------
		// Serialize File

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


		// ----------------------------------------------
		// class LogHelper

		class LogHelper
		{
		public:
			typedef logsystem::storage::CacheStorageT<128, FileStorage, logsystem::storage::ILogStorage<TCHAR>> Log1StorageType;
		
			typedef logsystem::MultiLogT<TCHAR, 1, logsystem::filter::DefaultFilter>		MultiLog;

		private:
			LogHelper()
			{
				//_Init();
			}
			~LogHelper()
			{
				OutputDebugStringW(L"~LogMgr\n");
			}

		private:
			LogHelper(const LogHelper &);
			LogHelper &operator=(const LogHelper &);

		public:
			static MultiLog &Instance()
			{
				static MultiLog log;
				static bool bInstance = true;
				if( bInstance )
				{
					_Init(log);
					bInstance = false;
				}

				return log;
			}

		private:
			static void _Init(MultiLog &log);
		};
	}

	inline log::LogHelper::MultiLog &Log()
	{
		return log::LogHelper::Instance();
	}
}





#endif