#ifndef __WRITE_TASK_FILE_HPP
#define __WRITE_TASK_FILE_HPP

#include "../../../include/Utility/utility.h"

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/FileSystem/File.hpp"
#include "../../Common/MemoryPool.hpp"


namespace recvSvr
{
	using namespace async;

	struct WriteTaskFile
		: public std::tr1::enable_shared_from_this<WriteTaskFile>
	{
		iocp::IODispatcher &io_;
		std::wstring folerPath_;

		WriteTaskFile(iocp::IODispatcher &io, const std::wstring &folderPath)
			: io_(io)
			, folerPath_(folderPath)
		{
			std::wstring path = folerPath_.substr(0, folerPath_.length() - 2);
			utility::mkpath(path);
			utility::mkpath(folerPath_);
		}

		void Handle(const common::BmpInfo &bmpInfo)
		{
			std::wstring filePath = folerPath_ + bmpInfo.second;

			try
			{
				filesystem::FilePtr file(filesystem::MakeFile(io_, filePath.c_str(), 
					GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED));

				const common::Buffer &bmpBuffer = bmpInfo.first;
				static LARGE_INTEGER offset = {0};
				file->AsyncWrite(bmpBuffer.first.get(), bmpBuffer.second, offset, 
					std::tr1::bind(&WriteTaskFile::_OnWrite, shared_from_this(), iocp::_Size, iocp::_Error, file, bmpBuffer));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}
		}

		void _OnWrite(u_long size, u_long error, const filesystem::FilePtr &file, const common::Buffer &buffer)
		{
			if( error != 0 )
				return;

			assert(size == buffer.second);

			/*try
			{
				file->SetFileSize(buffer.second);
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}*/
		}
	};
}





#endif