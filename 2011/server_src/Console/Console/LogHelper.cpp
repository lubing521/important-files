#include "stdafx.h"
#include "LogHelper.h"

#include "../../../include/frame.h"
#include "../../../include/Utility/utility.h"

namespace i8desk
{
	
	struct FileStorage::StorageImpl
	{
		HANDLE file_;

		explicit StorageImpl(LPCTSTR path)
			: file_(INVALID_HANDLE_VALUE)
		{
			file_ = ::CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
			if( file_ == INVALID_HANDLE_VALUE)
				return;

#ifdef _UNICODE
			short start = 0;
			DWORD dwBytes = 0;
			if( !::ReadFile(file_, &start, 2, &dwBytes, NULL) || start != 0xfeff )
			{
				::SetFilePointer(file_, 0, NULL, FILE_BEGIN);
				start = (short)0xfeff;
				::WriteFile(file_, &start, sizeof(start), &dwBytes, NULL);
			}
#endif
			::SetFilePointer(file_, 0, 0, FILE_END);
		}

		~StorageImpl()
		{
			assert(file_ != INVALID_HANDLE_VALUE);
			::CloseHandle(file_);
			file_ = INVALID_HANDLE_VALUE;
		}

		void Push(LPCTSTR msg, size_t len)
		{
			assert(file_ != INVALID_HANDLE_VALUE);
			if( file_ == INVALID_HANDLE_VALUE )
				return;

			if( len <= sizeof(TCHAR) )
			{
				assert(len <= sizeof(TCHAR));
				len = _tcslen(msg);
			}

			TCHAR xbuf[8192] = {0};
		
			SYSTEMTIME st = {0};
			GetLocalTime(&st);
			int nOffset = _stprintf(xbuf, _T("%02d:%02d:%02d "), st.wHour, st.wMinute, st.wSecond);
			
			DWORD ret = 0;

			::WriteFile(file_, xbuf, nOffset * sizeof(TCHAR), &ret, NULL);
			assert(ret == nOffset * sizeof(TCHAR));

			
			::WriteFile(file_, msg, len * sizeof(TCHAR), &ret, NULL);
			assert(ret == len * sizeof(TCHAR));

			::WriteFile(file_, _T("\r\n"), 2 * sizeof(TCHAR), &ret, NULL);
		}

		void Flush()
		{
			assert(file_ != INVALID_HANDLE_VALUE);
			if( file_ == INVALID_HANDLE_VALUE )
				return;

			::FlushFileBuffers(file_);
		}
	};


	FileStorage::FileStorage(LPCTSTR path)
		: impl_(new StorageImpl(path))
	{}

	FileStorage::~FileStorage() 
	{}

	void FileStorage::Put(char_type ch)
	{
		assert(0);
	}

	void FileStorage::Put(size_t szCount, char_type ch)
	{
		assert(0);
	}

	void FileStorage::Put(const char_type *pStr, size_t szCount)
	{
		impl_->Push(pStr, szCount);
	}

	void FileStorage::Put(const char_type *fmt, va_list args)
	{
		assert(0);
	}


	void FileStorage::Flush()
	{
		impl_->Flush();
	}


	// ==============================================
	struct ExceptionStorage::StorageImpl
	{
		typedef ExceptionStorage::Callback Callback;
		Callback callback_;

		explicit StorageImpl(const Callback &callback)
			: callback_(callback)
		{}

		void Push(LPCTSTR msg, size_t len)
		{
			LogRecordPtr log(new LogRecord);
			utility::Strcpy(log->plugin, _T("Console"));

			if( len <= sizeof(TCHAR) )
			{
				assert(len <= sizeof(TCHAR));
				len = _tcslen(msg);
			}

			stdex::tString text(msg);
			utility::Strcpy(log->data, text);
			log->time = (int)::time(0);
			log->type = LM_ERROR;

			if( callback_ != 0 )
				callback_(std::tr1::cref(log));
		}
	};


	ExceptionStorage::ExceptionStorage(const Callback &callback)
		: impl_(new StorageImpl(callback))
	{}

	ExceptionStorage::~ExceptionStorage() 
	{}

	void ExceptionStorage::Put(char_type ch)
	{
		assert(0);
	}
	
	void ExceptionStorage::Put(size_t szCount, char_type ch)
	{
		assert(0);
	}
	
	void ExceptionStorage::Put(const char_type *pStr, size_t szCount)
	{
		impl_->Push(pStr, szCount);
	}
	
	void ExceptionStorage::Put(const char_type *fmt, va_list args)
	{
		assert(0);
	}

	
	void ExceptionStorage::Flush()
	{
		
	}


	std::auto_ptr<LogHelper> LOG;

	void LogHelper::_Init()
	{
		if(!i8desk::IsRunOnServer())
			return;

		SYSTEMTIME gmt = {0};
		::GetSystemTime(&gmt);

		stdex::tOstringstream os;
		os << utility::GetAppPath() << _T("Log/Console-") << gmt.wYear << gmt.wMonth << gmt.wDay << _T(".log");

		log1_.reset(new Log1StorageType(os.str().c_str()));

		logs_.Add(*log1_);
	}
}