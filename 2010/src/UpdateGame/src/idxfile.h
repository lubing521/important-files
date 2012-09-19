#ifndef __idx_file_inc__
#define __idx_file_inc__

#include <string>
#include <map>
#include <vector>

#define INDEX_FILE_NAMEW	L"I8desk.idx"
#define INDEX_FILE_NAMEA	 "I8desk.idx"
#define INDEX_BLOCK_SIZE	0xA0000

#define MAKEQWORD(low, high)	((__int64)((DWORD)(low)) | (((__int64)(DWORD)(high)) << 32))
#define LOWDWORD(num)			((DWORD)(num))
#define HIGHDWORD(num)			((DWORD)((num) >> 32))

#define ALIGN_SIZE(all_size, block_size) (((all_size) + (block_size) -1) / (block_size) * (block_size))

//支持map的less模拟函数。忽略大小写比较。
template <> struct std::less<std::wstring> : public binary_function<std::wstring, std::wstring, bool>
{
	bool operator()(const std::wstring& _Left, const std::wstring& _Right) const
	{
		//_Left < _Right;
		return _wcsicmp(_Left.c_str(), _Right.c_str()) < 0;
	}
};

//索引文件头
typedef struct tagIdxHeahder
{
	DWORD  lowsize;
	DWORD  highsize;
	DWORD  resv3;
	DWORD  resv4;
	DWORD  resv5;
	DWORD  resv6;
	DWORD  xmlsize;
	WORD   resv8;
	WORD   blknum;
}tagIdxHeahder;

//索引中文件项
typedef struct tagIdxFile
{
	std::wstring name;
	__int64	size;
	__int64	time;
	DWORD   attr;
	DWORD   blknum;
	DWORD   crc;
	DWORD   crcoff;
	DWORD	update;
}tagIdxFile;

////索引中的crc块
typedef struct tagIdxBlock
{
	__int64	offset;
	DWORD	blksize;
	DWORD	update;
	DWORD	crc;
	DWORD	resv6;
	DWORD	fileoff;
	LPSTR	pdata;
}tagIdxBlock;

//回调函数
typedef bool (__stdcall *IDXCALLBACK)(void* pcookie, LPCWSTR info);

//生成索引文件
DWORD Idxfile_Make(LPCWSTR dir, DWORD blksize = INDEX_BLOCK_SIZE);

//解析索引文件.
DWORD Idxfile_Parse(LPCWSTR filename, std::vector<tagIdxFile*>& vFiles, 
	std::vector<tagIdxBlock*>& vBlocks, std::map<std::wstring, tagIdxFile*>& mFiles);

//解析内存索引文件.
DWORD Idxfile_Parse(char* pfile, DWORD length, std::vector<tagIdxFile*>& vFiles, 
	std::vector<tagIdxBlock*>& vBlocks, std::map<std::wstring, tagIdxFile*>& mFiles);

//用索引与磁盘上的文件对比。不同的块放在vOutBlocks里。
void Idxfile_Compare(LPCWSTR dir, std::vector<tagIdxFile*>& vFiles, std::vector<tagIdxBlock*>& vBlocks, __int64& qSize,
	IDXCALLBACK pfnCallback = NULL, void* pCookie = NULL);

//用新，旧索引来对比。
void Idxfile_Compare(std::map<std::wstring, tagIdxFile*>& mNewFiles, std::vector<tagIdxBlock*>& vNewBlocks, 
	std::map<std::wstring, tagIdxFile*>& mOldFiles, std::vector<tagIdxBlock*>& vOldBlocks, __int64& qSize,
	IDXCALLBACK pfnCallback = NULL, void* pCookie = NULL);

//得到索引文件版本号.失败返回-1
DWORD Idxfile_GetVersion(LPCWSTR file);

//得到内存索引文件版本号.失败返回-1
DWORD Idxfile_GetVersion(char* pfile, DWORD length);

//根据索引删除多余文件(不会删除索引文件)
void Idxfile_DeleteMorefile(LPCWSTR dir, std::map<std::wstring, tagIdxFile*>& mFiles, IDXCALLBACK pfnCallback = NULL, void* pCookie = NULL);

//计算块的crc值。
DWORD Idxfile_BlockCrc(unsigned char* pdata, int length);

#endif