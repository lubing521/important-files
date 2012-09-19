#include "stdafx.h"
#include "UpdateFilePolicy.hpp"

#include "../../../include/Utility/SmartPtr.hpp"
#include "tool.h"
#include "../VirtualDisk/VirtualDisk.h"
#include "../../../include/Utility/FileOperator.hpp"

#include <fstream>


namespace i8desk
{
	namespace update
	{

		namespace file_delete
		{
			struct CheckTmpFile
			{
				template<typename FileFindT>
				static bool Check(const FileFindT &filefind)
				{
					const std_string fileName = filefind.GetFileName();
					size_t index = fileName.rfind(_T("."));
					
					if( index != std_string::npos )
					{
						std_string tmpFile = fileName.substr(index + 1, fileName.length());
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
				static bool Check(const FileFindT &)
				{
					return true;
				}
			};

			typedef i8desk::utility::FilesOperator<CheckTmpFile, utility::DeleteFileT> FileTmpDeleter;
			typedef i8desk::utility::FilesOperator<CheckFile, utility::DeleteFileT> FileDeleter;
		}

		UpdateFile::UpdateFile(ILogger *log)
			: log_(log)
			, restartService_(false)
		{}


		void UpdateFile::DoUpdateFile(const std::vector<i8desk::std_string> &fileList)
		{
			log_->WriteLog(LM_INFO, "开始替换原有程序");
			std::for_each(fileList.begin(), fileList.end(), 
				std::tr1::bind(&UpdateFile::_UpdateAllFileImpl, this, std::tr1::placeholders::_1));
		}


		void UpdateFile::DeleteAllFileTmp(/*const std_string &file*/)
		{
			// 删除临时文件及目录
			std_string path = GetAppPath();
			std_string dir = GetAppPath() + internal::tmpFile + _T("/*.");

			// 删除临时文件
			file_delete::FileTmpDeleter tmpDeletor;
			tmpDeletor(path);

			// 删除所有文件
			file_delete::FileDeleter fileDeletor;
			fileDeletor(dir);

			// 删除目录
			::RemoveDirectory(path.c_str());
		}


		void UpdateFile::_UpdateAllFileImpl(const i8desk::std_string &file)
		{
			const i8desk::std_string tmpFile	= GetAppPath() + internal::tmpFile + _T("\\") + file;
			const i8desk::std_string destFile	= GetAppPath() + file;

			::Sleep(10);


			if( !::PathFileExists(tmpFile.c_str()) )
			{
				std::stringstream os;
				os << "更新临时文件( " << file << " )不存在! code = " << ::GetLastError();
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
				const i8desk::std_string tmp = destFile + ".~tmp";
				::SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(tmp.c_str());

				if( !::MoveFileEx(destFile.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING) )
				{
					std::stringstream os;
					os << "Rename File: " << destFile << "error! code = " << ::GetLastError();
					throw std::runtime_error(os.str());
				}

				if( !CopyFileEx(tmpFile.c_str(), destFile.c_str(), 0, 0, 0, COPY_FILE_FAIL_IF_EXISTS) )
				{
					std::stringstream os;
					os << "ReCopy File: " << destFile << "error! code = " << ::GetLastError();
					throw std::runtime_error(os.str());
				}
				

				restartService_ = true;
			}
		}


	}
}