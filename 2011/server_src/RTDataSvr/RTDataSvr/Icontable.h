#ifndef ICONTABLE_H
#define ICONTABLE_H


#include "../../../include/irtdatasvr.h"
#include "../../../include/frame.h"
#include "../../../include/CustomFileDLL.h"
#include "container.h"

namespace i8desk
{

	struct IDatabase;

	class ZIconTable :public IIconTable
	{
	public:
		ZIconTable(void):version_(0)
		{
		};
		virtual ~ZIconTable(void);

		int RegisterHandler(void);
		void UnregisterHandler(void);


	private:
		stdex::tString strIconPath_;
		HANDLE IconHandle_;
		volatile long version_;

	public:
		bool Ensure(IN long gid, IN size_t size, IN const void *data);
		long GetVersion();
		int Load(IDatabase *pDatabase);

		// 创建自定义文件格式
		// lpszPath 路径, 返回句柄
		void * CreateCustomFile(LPCTSTR lpszSavePath);
		// 销毁
		bool  DestroyCustomFile();
		// 通过gid与路径添加新内容
		bool  AddCustomFile(IN long gid, IN LPCTSTR filePath);

		// 通过gid与内存数据添加
		bool  AddCustomFile2(IN long gid, IN size_t size, IN const void *data);
		// 删除指定gid数据
		bool  DeleteCustomFile( IN long gid);
		// 修改数据
		bool  ModifyCustomFile(IN long gid, IN LPCTSTR newFilePath);
		bool  ModifyCustomFile2(IN long gid, IN size_t size, IN const void *data);
		// 获取指定gid数据
		bool  GetFileData(IN long gid, OUT size_t &size, OUT void *&data);
		// 获取所有的数据
		size_t  GetAllFileData(OUT size_t &size, OUT CustomFileInfoPtr &files);



	};


}



#endif 