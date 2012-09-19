#include "stdafx.h"
#include "Log.h"

#include "../../../include/Utility/utility.h"


namespace i8desk
{

	namespace log
	{


		struct FileStorage::StorageImpl
		{
			HANDLE file_;

			explicit StorageImpl(LPCTSTR path)
				: file_(INVALID_HANDLE_VALUE)
			{
				file_ = ::CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
				if( file_ == INVALID_HANDLE_VALUE)
					throw std::runtime_error("创建日志失败");

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


		void LogHelper::_Init(MultiLog &log)
		{
			SYSTEMTIME gmt = {0};
			::GetSystemTime(&gmt);

			stdex::tOstringstream os;
			os << utility::GetAppPath() << _T("Log/Console-") << gmt.wYear << gmt.wMonth << gmt.wDay << _T(".log");

			static Log1StorageType	file(os.str().c_str());

			log.Add(file);
		}

	}
}