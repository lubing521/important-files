#include "stdafx.h"
#include "GlobalFun.h"


namespace utility
{
	static const TCHAR *I8DESK_SPECIAL_DIR = TEXT("i8desk");
	static const size_t I8DESK_SPECIAL_DIR_LEN = ::lstrlen(I8DESK_SPECIAL_DIR);


	BOOL DeleteFile(LPCTSTR lpszFileName)
	{
		::SetFileAttributes(lpszFileName,FILE_ATTRIBUTE_NORMAL);
		BOOL bRet=::DeleteFile(lpszFileName);

		return bRet;
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
				|| memcmp(FindFileData.cFileName, I8DESK_SPECIAL_DIR, I8DESK_SPECIAL_DIR_LEN) 
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
					I8DESK_SPECIAL_DIR, I8DESK_SPECIAL_DIR_LEN) == 0) 
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

	BOOL GetFileTreeList3(LPCTSTR lpPath,std::vector<stdex::tString> &vFileList,std::vector<stdex::tString> &vDirList,std::vector<ULONGLONG> &vSizeList,std::vector<stdex::tString> &vCreateTimeList)
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
				if (!GetFileTreeList3(strPath.c_str(),vFileList,vDirList,vSizeList,vCreateTimeList))
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
				if (!GetFileTreeList3(strPath.c_str(),vFileList,vDirList,vSizeList,vCreateTimeList))
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

		_stprintf_s(buf,_T("%04u-%02u-%02u %02u:%02u:%02u"),
			LocalTime.wYear,LocalTime.wMonth,LocalTime.wDay,LocalTime.wHour,LocalTime.wMinute,LocalTime.wSecond);
		return buf;
	}
}