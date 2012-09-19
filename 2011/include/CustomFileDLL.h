#pragma once


#include <memory>

#ifdef CUSTOMFILEDLL_EXPORTS
#define CUSTOMFILEDLL_API __declspec(dllexport)
#else
#define CUSTOMFILEDLL_API __declspec(dllimport)
#endif


#ifdef  __cplusplus
extern "C" {
#endif


struct CustomFileInfo
{
	long gid;						// gid
	DWORD crc;						// crc
	size_t size;					// 数据大小
	const void * data;				// 数据
};
typedef std::tr1::shared_ptr<CustomFileInfo> CustomFileInfoPtr;


// 创建自定义文件格式
// lpszPath 路径, 返回句柄
CUSTOMFILEDLL_API void *WINAPI CreateCustomFile(LPCTSTR lpszSavePath);

// 销毁
CUSTOMFILEDLL_API bool WINAPI DestroyCustomFile(void *handle);

// 刷新
CUSTOMFILEDLL_API bool WINAPI FlushCustomFile(void *handle);

// 通过gid与路径添加新内容
CUSTOMFILEDLL_API bool WINAPI AddCustomFile(void *handle, IN long gid, IN LPCTSTR filePath);
// 通过gid与内存数据添加
CUSTOMFILEDLL_API bool WINAPI AddCustomFile2(void *handle, IN long gid, IN size_t size, IN const void *data);

// 删除指定gid数据
CUSTOMFILEDLL_API bool WINAPI DeleteCustomFile(void *handle, IN long gid);

// 修改数据
CUSTOMFILEDLL_API bool WINAPI ModifyCustomFile(void *handle, IN long gid, IN LPCTSTR newFilePath);
CUSTOMFILEDLL_API bool WINAPI ModifyCustomFile2(void *handle, IN long gid, IN size_t size, IN const void *data);

// 获取指定gid数据
CUSTOMFILEDLL_API bool WINAPI GetFileData(void *handle, IN long gid, OUT size_t &size, OUT void *&data);

// 获取所有的数据
CUSTOMFILEDLL_API size_t WINAPI GetAllFileData(void *handle, OUT size_t &size, OUT CustomFileInfoPtr &files);


#ifdef  __cplusplus
}
#endif