#include "stdafx.h"
#include "UpdateFilePolicy.hpp"

#include "../../../include/Utility/SmartPtr.hpp"
#include "tool.h"
#include "../VirtualDisk/VirtualDisk.h"
#include "../../../include/Win32/FileSystem/FileHelper.hpp"
#include "../../../include/Utility/utility.h"

#include <fstream>


#ifdef _DEBUG 
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif


namespace i8desk
{
	namespace update
	{

		namespace file_delete
		{
			struct CheckTmpFile
			{
				template<typename FileFindT>
				bool operator()(const FileFindT &filefind)
				{
					const stdex::tString fileName = filefind.GetFileName();
					size_t index = fileName.rfind(_T("."));
					
					if( index != stdex::tString::npos )
					{
						stdex::tString tmpFile = fileName.substr(index + 1, fileName.length());
						if( tmpFile == _T("~tmp") )
							return true;
						else
							return false;
					}
					
					return false;
				}
			};

			struct CheckFile
			{
				template<typename FileFindT>
				bool operator()(const FileFindT &)
				{
					return true;
				}
			};

			typedef win32::file::FileDepthSearch<CheckTmpFile,  win32::file::detail::DeleteFileT> FileTmpDeleter;
		}

		UpdateFile::UpdateFile(ILogger *log)
			: log_(log)
			, restartService_(false)
		{}


		void UpdateFile::DoUpdateFile(const std::vector<stdex::tString> &fileList)
		{
			log_->WriteLog(LM_INFO, _T("开始替换原有程序"));
			std::for_each(fileList.begin(), fileList.end(), 
				std::tr1::bind(&UpdateFile::_UpdateAllFileImpl, this, std::tr1::placeholders::_1));
		}


		void UpdateFile::DeleteAllFileTmp(/*const stdex::tString &file*/)
		{
			log_->WriteLog(LM_INFO, _T("删除临时文件"));
			// 删除临时文件及目录
			stdex::tString path = utility::GetAppPath();
			stdex::tString dir = utility::GetAppPath() + internal::tmpFile + _T("/*.");

			// 删除临时文件
			file_delete::CheckTmpFile checkTmpFile;
			win32::file::detail::DeleteFileT deleteFile;
			file_delete::FileTmpDeleter tmpDeletor(checkTmpFile, deleteFile);
			tmpDeletor.Run(path);

			// 删除所有文件
			win32::file::DeleteDirectory(dir.c_str());
		}


		void UpdateFile::_UpdateAllFileImpl(const stdex::tString &file)
		{
			const stdex::tString tmpFile	= utility::GetAppPath() + internal::tmpFile + _T("\\") + file;
			const stdex::tString destFile	= utility::GetAppPath() + file;

			::Sleep(10);


			if( !::PathFileExists(tmpFile.c_str()) )
			{
				std::stringstream os;
				os << "更新临时文件( " << CT2A(file.c_str()) << " )不存在! code = " << ::GetLastError();
				throw std::runtime_error(os.str());
			}

			if( ::CopyFileEx(tmpFile.c_str(), destFile.c_str(), 0, 0, 0, COPY_FILE_FAIL_IF_EXISTS) )
			{
				/*i8desk::CAutoFile hFile = CreateFile(destFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);

				if( hFile.IsValid() )
				{}*/

				restartService_ = true;
			}
			else
			{
				const stdex::tString tmp = destFile + _T(".~tmp");
				::SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(tmp.c_str());

				if( !::MoveFileEx(destFile.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING) )
				{
					std::stringstream os;
					os << "Rename File: " << CT2A(destFile.c_str()) << "error! code = " << ::GetLastError();
					throw std::runtime_error(os.str());
				}

				if( !CopyFileEx(tmpFile.c_str(), destFile.c_str(), 0, 0, 0, COPY_FILE_FAIL_IF_EXISTS) )
				{
					std::stringstream os;
					os << "ReCopy File: " << CT2A(destFile.c_str()) << "error! code = " << ::GetLastError();
					throw std::runtime_error(os.str());
				}
				

				restartService_ = true;
			}
		}


	}
}