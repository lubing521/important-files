#ifndef __GLOBALFUN__H__
#define __GLOBALFUN__H__



#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>



#include "../../../include/Zip/XUnzip.h"
#include "../../../include/Utility/Smartptr.hpp"



#pragma comment(lib,"Version.lib")
#if !defined(MAKEULONGLONG)
#define MAKEULONGLONG(low,high) \
	((ULONGLONG)(((DWORD)(low)) | ((ULONGLONG)((DWORD)(high))) << 32))
#endif



namespace utility
{



	typedef struct tagFileHeader
	{
		DWORD dwGameSizeLow;
		DWORD dwGameSizeHigh;
		DWORD resv1;  //保留表示我们不使用,陈直他们会使用.
		DWORD resv2;
		DWORD resv3;
		DWORD resv4;
		DWORD dwXmlSize; //XML文件的大小.
		WORD  resv5;
		WORD  CrcBlockNum; //CRC块数.
	}tagFileHeader;



	typedef std::tr1::unordered_set<stdex::tString> StringHashSet;
	typedef StringHashSet::iterator StringHashSetItr;
	typedef StringHashSet::const_iterator StringHashSetCItr;



	//目录名最后不带'\'
	BOOL GetFileTreeList(LPCTSTR lpPath,std::vector<stdex::tString> &vFileList,std::vector<ULONGLONG> &vSizeList);


	BOOL GetFileTreeList2(LPCTSTR lpPath,  
		std::vector<stdex::tString> &vFileList,
		std::vector<stdex::tString> &vDirList,
		std::vector<ULONGLONG> &vSizeList);


	BOOL GetFileTreeList3(LPCTSTR lpPath,
		std::vector<stdex::tString> &vFileList,
		std::vector<stdex::tString> &vDirList,
		std::vector<ULONGLONG> &vSizeList,
		std::vector<stdex::tString> &vCreateTimeList);


	// 把ULONGLONG的时间转换成string类型的时间
	stdex::tString DateTimeToString(ULONGLONG ulDT);

	template < typename CallbackT >
	bool UnZipItem(const stdex::tString &srcPath, const stdex::tString &dstPath, const CallbackT &callback)
	{
		HZIP hz = ::OpenZip(srcPath.c_str(), 0, ZIP_FILENAME, dstPath.c_str());
		if( hz == 0 )
			return false;

		ZIPENTRYW ze = {0}; 
		HRESULT res = ::GetZipItem(hz,-1,&ze); 

		if( res == S_OK )
		{
			int numitems = ze.index;
			for(int i = 0; i != numitems; ++i)
			{ 
				res = ::GetZipItem(hz,i,&ze);
				if( res == S_OK )
					::UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);

				callback();
			}
		}

		::CloseZip(hz);
		return true;
	}

}

#endif