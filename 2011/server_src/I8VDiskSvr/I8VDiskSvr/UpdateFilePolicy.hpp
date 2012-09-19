#ifndef __UPDATE_FILE_UPDATE_FILE_HPP
#define __UPDATE_FILE_UPDATE_FILE_HPP


#include <vector>

namespace i8desk
{

	struct ILogger;


	namespace update
	{

		// --------------------------------------
		// class UpdateFile

		class UpdateFile
		{
		private:
			i8desk::ILogger *log_;
			bool restartService_;					// 是否需要重启服务

		public:
			explicit UpdateFile(ILogger *log);

		private:
			UpdateFile(const UpdateFile &);
			UpdateFile &operator=(const UpdateFile &);

		public:
			void DoUpdateFile(const std::vector<stdex::tString> &fileList);
			bool IsOK() const
			{
				return restartService_;
			}
			void Reset()
			{
				restartService_ = false;
			}
	
			// 删除所有的临时文件及目录
			void DeleteAllFileTmp();

		private:
			// 更新所有文件
			void _UpdateAllFileImpl(const stdex::tString &file);

			
		};
	}
}





#endif