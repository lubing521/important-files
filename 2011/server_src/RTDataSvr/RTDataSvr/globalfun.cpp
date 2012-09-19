#include "stdafx.h"
#include "GlobalFun.h"
#include "../../../include/Utility/utility.h"
#include "i8mp.h"

namespace utility
{


	BOOL DeleteFile(LPCTSTR lpszFileName)
	{
		::SetFileAttributes(lpszFileName,FILE_ATTRIBUTE_NORMAL);
		BOOL bRet=::DeleteFile(lpszFileName);

		return bRet;
	}

	BOOL DeleteDirectory(const stdex::tString& path)
	{
		BOOL bRet;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwErrorCode;
		stdex::tString strPath,strFileName,strSubPath;
		strPath = path;
		strPath+=_T("\\*.*");
		hFind = FindFirstFile(strPath.c_str(), &FindFileData);
		if(hFind==INVALID_HANDLE_VALUE)
			return FALSE;


		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!((FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='\0')||(FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='.' && FindFileData.cFileName[2]=='\0')))
			{
				strPath = path;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				if (!DeleteDirectory(strPath.c_str()))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
		}
		else
		{
			strFileName = path;
			strFileName+=_T("\\");
			strFileName+=FindFileData.cFileName;
			if (!DeleteFile(strFileName.c_str()))
			{
				bRet=FindClose(hFind);
				assert(bRet);
				return FALSE;
			}
		}

		while (FindNextFile(hFind,&FindFileData))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//.or ..
				if (FindFileData.cFileName[0] == '.')
				{
					if (FindFileData.cFileName[1] == '\0' ||
						(FindFileData.cFileName[1] == '.' &&
						FindFileData.cFileName[2] == '\0'))
					{
						continue;
					}
				}

				strPath= path;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				if (!DeleteDirectory(strPath.c_str()))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
			else
			{
				strFileName=path;
				strFileName+=_T("\\");
				strFileName+=FindFileData.cFileName;
				if (!DeleteFile(strFileName.c_str()))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}

			}
		}

		dwErrorCode=GetLastError();
		bRet=FindClose(hFind);
		assert(bRet);
		if (dwErrorCode!=ERROR_NO_MORE_FILES)
		{
			SetLastError(dwErrorCode);
			return FALSE;
		}

		if(!::RemoveDirectory(path.c_str()))
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL GetFileLastWriteTime(LPCTSTR lpszFileName,ULONGLONG &ftLastWriteTime)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (::GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad)) {
			ftLastWriteTime = *(ULONGLONG *)&fad.ftLastWriteTime;
			return TRUE;
		}
		return FALSE;
	}

	 BOOL ExtractZipContent(LPCTSTR lpZipFileName, 
		LPCTSTR lpTargetPath, 
		LPCTSTR lpContent, 
		stdex::tString& strErrorInfo)
	{
		BOOL bRet=TRUE;
		stdex::tString strUnixStyleTargetPath=lpTargetPath;
		std::replace(strUnixStyleTargetPath.begin(),strUnixStyleTargetPath.end(),'\\','/');
		HZIP hz=OpenZip((void*)lpZipFileName,0, ZIP_FILENAME,strUnixStyleTargetPath.c_str());
		if(hz==NULL)
		{
			strErrorInfo=_T("解压文件时打开文件失败:");
			strErrorInfo+=lpZipFileName;
			return FALSE;
		}

		int index;

		#if defined UNICODE || defined _UNICODE
			ZIPENTRYW ze;
		#else
			ZIPENTRY ze;
		#endif
		
		ZRESULT res = FindZipItem(hz, lpContent, TRUE, &index, &ze);
		if (res != ZR_OK) {
			TCHAR ErrorBuf[1024] = {0};
			FormatZipMessage(res,ErrorBuf,sizeof(ErrorBuf)-1);
			strErrorInfo = _T("压缩文件没有指定的内容,");
			strErrorInfo += ErrorBuf;
			strErrorInfo += lpZipFileName;
			strErrorInfo += lpContent;
			CloseZip(hz);
			return FALSE;
		}

		res = UnzipItem(hz, index, (void *)lpContent, 0, ZIP_FILENAME);
		if (res != ZR_OK)
		{
			TCHAR ErrorBuf[1024] = {0};
			FormatZipMessage(res,ErrorBuf,sizeof(ErrorBuf)-1);
			strErrorInfo = _T("解压指定文件失败,");
			strErrorInfo += (TCHAR)ErrorBuf;
			strErrorInfo += lpZipFileName;
			strErrorInfo += lpContent;
			CloseZip(hz);
			return FALSE;
		}

		CloseZip(hz);

		return TRUE;
	}

	// 目录名最后不带'\'
	BOOL GetFileTreeList(LPCTSTR lpPath,std::vector<stdex::tString> &vFileList,std::vector<ULONGLONG> &vSizeList)
	{
		BOOL bRet;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwErrorCode;
		stdex::tString strPath,strFileName,strSubPath;
		strPath=lpPath;
		strPath+=_T("\\*.*");
		hFind = FindFirstFile(strPath.c_str(), &FindFileData);
		if(hFind==INVALID_HANDLE_VALUE)
			return FALSE;


		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!((FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='\0')||(FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='.' && FindFileData.cFileName[2]=='\0')))
			{
				strPath=lpPath;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				if (!GetFileTreeList(strPath.c_str(),vFileList,vSizeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
		}
		else
		{
			strFileName=lpPath;
			strFileName+=_T("\\");
			strFileName+=FindFileData.cFileName;
			vFileList.push_back(strFileName);
			vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
		}

		while (FindNextFile(hFind,&FindFileData))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//.or ..
				if (FindFileData.cFileName[0] == '.')
				{
					if (FindFileData.cFileName[1] == '\0' ||
						(FindFileData.cFileName[1] == '.' &&
						FindFileData.cFileName[2] == '\0'))
					{
						continue;
					}
				}

				strPath=lpPath;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				if (!GetFileTreeList(strPath.c_str(),vFileList,vSizeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
			else
			{
				strFileName=lpPath;
				strFileName+=_T("\\");
				strFileName+=FindFileData.cFileName;
				vFileList.push_back(strFileName);
				vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
			}
		}

		dwErrorCode=GetLastError();
		bRet=FindClose(hFind);
		assert(bRet);
		if (dwErrorCode!=ERROR_NO_MORE_FILES)
		{
			SetLastError(dwErrorCode);
			return FALSE;
		}

		return TRUE;
	}


	BOOL GetFileTreeList2(LPCTSTR lpPath,
		std::vector<stdex::tString> &vFileList,
		std::vector<stdex::tString> &vDirList,
		std::vector<ULONGLONG> &vSizeList)
	{
		BOOL bRet;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwErrorCode;
		stdex::tString strPath,strFileName,strSubPath;
		strPath=lpPath;
		strPath+=_T("\\*.*");
		hFind = FindFirstFile(strPath.c_str(), &FindFileData);
		if(hFind==INVALID_HANDLE_VALUE)
			return FALSE;

		stdex::ToLower(FindFileData.cFileName);
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!(
				(FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='\0')
				|| (FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='.' && FindFileData.cFileName[2]=='\0')
				|| memcmp(FindFileData.cFileName, _T("i8desk"), ::lstrlen( _T("i8desk"))) 
				)
				)
			{
				strPath = lpPath;
				strPath +=  _T("\\");
				strPath += FindFileData.cFileName;
				vDirList.push_back(strPath);
				if (!GetFileTreeList2(strPath.c_str(),vFileList,vDirList,vSizeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
		}
		else
		{
			strFileName = lpPath;
			strFileName += _T("\\");
			strFileName += FindFileData.cFileName;
			vFileList.push_back(strFileName);
			vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
		}

		while (FindNextFile(hFind,&FindFileData))
		{
			stdex::ToLower(FindFileData.cFileName);
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//.or ..
				if (FindFileData.cFileName[0] == '.')
				{
					if (FindFileData.cFileName[1] == '\0' ||
						(FindFileData.cFileName[1] == '.' &&
						FindFileData.cFileName[2] == '\0'))
					{
						continue;
					}
				}

				if (memcmp(FindFileData.cFileName, 
					_T("i8desk"), ::lstrlen( _T("i8desk"))) == 0) 
				{
					continue;
				}

				strPath = lpPath;
				strPath +=  _T("\\");
				strPath += FindFileData.cFileName;
				vDirList.push_back(strPath);
				if (!GetFileTreeList2(strPath.c_str(),vFileList,vDirList,vSizeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
			else
			{
				strFileName = lpPath;
				strFileName += _T("\\");
				strFileName += FindFileData.cFileName;		
				vFileList.push_back(strFileName);
				vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
			}
		}

		dwErrorCode=GetLastError();
		bRet=FindClose(hFind);
		assert(bRet);
		if (dwErrorCode!=ERROR_NO_MORE_FILES)
		{
			SetLastError(dwErrorCode);
			return FALSE;
		}

		return TRUE;
	}

	BOOL utility::GetFileTreeList3(LPCTSTR lpPath,std::vector<stdex::tString> &vFileList,std::vector<stdex::tString> &vDirList,std::vector<ULONGLONG> &vSizeList,std::vector<stdex::tString> &vCreateTimeList)
	{
		BOOL bRet;
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwErrorCode;
		stdex::tString strPath,strFileName,strSubPath;
		strPath=lpPath;
		strPath+=_T("\\*.*");
		hFind = FindFirstFile(strPath.c_str(), &FindFileData);
		if(hFind==INVALID_HANDLE_VALUE)
			return FALSE;


		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(!((FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='\0')||(FindFileData.cFileName[0]=='.' && FindFileData.cFileName[1]=='.' && FindFileData.cFileName[2]=='\0')))
			{
				strPath=lpPath;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				vDirList.push_back(strPath);
				if (!utility::GetFileTreeList3(strPath.c_str(),vFileList,vDirList,vSizeList,vCreateTimeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
		}
		else
		{
			strFileName=lpPath;
			strFileName+=_T("\\");
			strFileName+=FindFileData.cFileName;
			vFileList.push_back(strFileName);
			vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
			vCreateTimeList.push_back(DateTimeToString(MAKEULONGLONG(FindFileData.ftCreationTime.dwLowDateTime,FindFileData.ftCreationTime.dwHighDateTime)).c_str());
		}

		while (FindNextFile(hFind,&FindFileData))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//.or ..
				if (FindFileData.cFileName[0] == '.')
				{
					if (FindFileData.cFileName[1] == '\0' ||
						(FindFileData.cFileName[1] == '.' &&
						FindFileData.cFileName[2] == '\0'))
					{
						continue;
					}
				}

				strPath=lpPath;
				strPath+=_T("\\");
				strPath+=FindFileData.cFileName;
				vDirList.push_back(strPath);
				if (!utility::GetFileTreeList3(strPath.c_str(),vFileList,vDirList,vSizeList,vCreateTimeList))
				{
					bRet=FindClose(hFind);
					assert(bRet);
					return FALSE;
				}
			}
			else
			{
				strFileName=lpPath;
				strFileName+=_T("\\");
				strFileName+=FindFileData.cFileName;
				vFileList.push_back(strFileName);
				vSizeList.push_back(MAKEULONGLONG(FindFileData.nFileSizeLow,FindFileData.nFileSizeHigh));
				vCreateTimeList.push_back(DateTimeToString(MAKEULONGLONG(FindFileData.ftCreationTime.dwLowDateTime,FindFileData.ftCreationTime.dwHighDateTime)).c_str());
			}
		}

		dwErrorCode=GetLastError();
		bRet=FindClose(hFind);
		assert(bRet);
		if (dwErrorCode!=ERROR_NO_MORE_FILES)
		{
			SetLastError(dwErrorCode);
			return FALSE;
		}

		return TRUE;
	}

	BOOL GetFileSize(LPCTSTR lpszFileName,ULONGLONG &ulFileSize)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (::GetFileAttributesEx(lpszFileName, GetFileExInfoStandard, &fad)) {
			ulFileSize = MAKEULONGLONG(fad.nFileSizeLow, fad.nFileSizeHigh);
			return TRUE;
		}
		return FALSE;
	}


	// 把ULONGLONG的时间转换成string类型的时间
	stdex::tString DateTimeToString(ULONGLONG ulDT)
	{
		TCHAR buf[50];
		LARGE_INTEGER li;
		li.QuadPart=ulDT;

		FILETIME FileTime={li.LowPart,li.HighPart};
		SYSTEMTIME SystemTime,LocalTime;
		BOOL bRet=FileTimeToSystemTime(&FileTime,&SystemTime);
		assert(bRet);

		SystemTimeToTzSpecificLocalTime(NULL,&SystemTime,&LocalTime);

		_stprintf(buf,_T("%04u-%02u-%02u %02u:%02u:%02u"),
			LocalTime.wYear,LocalTime.wMonth,LocalTime.wDay,LocalTime.wHour,LocalTime.wMinute,LocalTime.wSecond);
		return buf;
	}

	BOOL GetFileCrc32(LPCTSTR lpFileName,DWORD& dwCrc32)
	{
		HANDLE hFile=CreateFile(lpFileName,           // open MYFILE.TXT 
			GENERIC_READ,              // open for reading 
			FILE_SHARE_READ | FILE_SHARE_WRITE,           // share for reading 
			NULL,                      // no security 
			OPEN_EXISTING,             // existing file only 
			FILE_ATTRIBUTE_NORMAL,     // normal file 
			NULL);                     // no attr. template 

		if (hFile==INVALID_HANDLE_VALUE)
			return FALSE;

		DWORD dwSize=::GetFileSize(hFile,NULL);
		if(dwSize==INVALID_FILE_SIZE)
		{
			CloseHandle(hFile);
			return FALSE;
		}

		BYTE *lpFileBuf=NULL;
		try
		{
			//lpFileBuf=new BYTE[dwSize];
			lpFileBuf = (BYTE *)I8MP.alloc(dwSize);
			if (lpFileBuf==NULL)
			{
				CloseHandle(hFile);
				return FALSE;
			}
		}
		catch (...)
		{
			CloseHandle(hFile);
			return FALSE;
		}
		

		if (!ReadFileContent(hFile,(BYTE*)lpFileBuf,dwSize))
		{
			CloseHandle(hFile);
			//delete []lpFileBuf;
			I8MP.free(lpFileBuf);
			return FALSE;
		}

		CloseHandle(hFile);

		dwCrc32=utility::CalBufCRC32(lpFileBuf,dwSize);
		//delete []lpFileBuf;
		I8MP.free(lpFileBuf);
		return TRUE;
	}

	BOOL GetFileCrc32(HANDLE hFile,DWORD& dwCrc32)
	{
		DWORD dwSize=::GetFileSize(hFile,NULL);
		if(dwSize==INVALID_FILE_SIZE)
			return FALSE;

		dwSize-=sizeof(DWORD);
		if (SetFilePointer(hFile,4,NULL,FILE_BEGIN)==INVALID_SET_FILE_POINTER)
			return FALSE;



		BYTE *lpFileBuf=NULL;
		try
		{
			//lpFileBuf=new BYTE[dwSize];
			lpFileBuf = (BYTE *)I8MP.alloc(dwSize);
			if (lpFileBuf==NULL)
				return FALSE;
		}
		catch (...)
		{
			return FALSE;
		}
		
		if (!ReadFileContent(hFile,(BYTE*)lpFileBuf,dwSize))
		{
			DWORD dwErr=GetLastError();
			//delete []lpFileBuf;
			I8MP.free(lpFileBuf);
			return FALSE;
		}

		dwCrc32=utility::CalBufCRC32(lpFileBuf,dwSize);
		//delete []lpFileBuf;
		I8MP.free(lpFileBuf);
		return TRUE;
	}


}