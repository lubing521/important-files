#ifndef __UPDATE_FILE_GET_FILE_HPP
#define __UPDATE_FILE_GET_FILE_HPP



#include <tuple>


namespace i8desk
{

	struct ILogger;
	class CSocket;


	namespace update
	{

		// ------------------------------
		// class GetUpdateFile

		class GetUpdateFile
		{
			ILogger *log_;
			CSocket &sock_;
			bool needUpdate_;

			// 文件大小、文件CRC
			typedef std::tr1::tuple<DWORD, DWORD> FileInfo;

		public:
			GetUpdateFile(ILogger *log, CSocket &sock);

		private:
			GetUpdateFile(const GetUpdateFile &);
			GetUpdateFile &operator=(const GetUpdateFile &);

		public:
			void DoGetFile(std::vector<std_string> &fileList);
			bool IsOK() const
			{
				return needUpdate_;
			}

			void Reset()
			{
				needUpdate_ = false;
			}


		private:
			void _DoGetFileImpl(const i8desk::std_string &file);


			// 获取所需更新的文件列表
			void _GetAllFileList(std::vector<std_string> &fileList);

			// 获取指定文件信息：文件大小、文件CRC
			FileInfo _RequestFileInfo(const i8desk::std_string &file);
			/* 比较文件信息	
					true : 需要更新
					false: 不需要更新
			*/			
			bool _CompareFile(DWORD fileSize, DWORD crc, const i8desk::std_string &file);

			// 确保目录存在
			void _EnsureTmpDirExisit();

			// 下载指定文件
			void _DownloadFile(const i8desk::std_string &file);

			// 把文件写向临时目录
			void _WriteFileTmp(const i8desk::std_string &file, char *fileData, DWORD fileSize);

		};
	}
}



#endif