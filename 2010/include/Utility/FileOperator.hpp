#ifndef __FILE_OPERATOR_HPP
#define __FILE_OPERATOR_HPP

#include "../FileSystem/FindFile.hpp"

namespace i8desk
{
	namespace utility
	{
		// -----------------------
		struct CheckFileTimeT
		{
			enum { DAYS = 7 };
			static const __int64 PerSecDay = (__int64)10000000 * 24 * 60 * 60;

			template<typename FileFindT>
			static bool Check(const FileFindT &filefind)
			{
				// 文件最后写时间
				ULARGE_INTEGER time = {0};
				::memcpy(&time, &filefind.m_fd.ftLastWriteTime, sizeof(ULARGE_INTEGER));
				
				// 系统当前时间
				ULARGE_INTEGER now = {0};
				SYSTEMTIME st = {0};
				::GetSystemTime(&st);
				FILETIME f = {0};
				::SystemTimeToFileTime(&st, &f);
				::memcpy(&now, &f, sizeof(now));

				// 时间差
				__int64 span = now.QuadPart - time.QuadPart;

				if( span >= PerSecDay * DAYS )
					return true;

				return false;
			}
		};

		struct CheckFileNull
		{
			template<typename FileFindT>
			static bool Check(const FileFindT &filefind)
			{
				return true;
			}
		};

		// -----------------------
		struct DeleteFileT
		{
			template<typename StringT>
			static void Operate(const StringT &path)
			{
				::DeleteFile(path.c_str());
			}
		};

		struct DeleteFileNull
		{
			template<typename StringT>
			static void Operate(const StringT &path)
			{
				// do nothing
			}
		};


		// -----------------------
		template<typename CondTraits, typename OPTraits>
		class FilesOperator
		{

		public:
			template<typename StringT>
			void operator()(StringT &dir)
			{
				filesystem::CFindFile find;

				dir = dir + _T("*");

				BOOL bSuc = find.FindFile(dir.c_str());
				while( bSuc )
				{
					bSuc = find.FindNextFile();

					if( find.IsDots() )
						continue;

					if( CondTraits::Check(find) )
					{
						OPTraits::Operate(find.GetFilePath());
					}

				}
			}

		};
	}
}


#endif