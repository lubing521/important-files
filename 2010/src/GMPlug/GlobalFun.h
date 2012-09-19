#ifndef __GLOBALFUN__H__
#define __GLOBALFUN__H__

#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>

#include <shellapi.h>
#include <atlenc.h>

#include "I8Type.h"
#include "XUnzip.h"
#include "sync.h"
#include "i8mp.h"

#pragma comment(lib,"Version.lib")
#if !defined(MAKEULONGLONG)
#define MAKEULONGLONG(low,high) \
	((ULONGLONG)(((DWORD)(low)) | ((ULONGLONG)((DWORD)(high))) << 32))
#endif

#ifndef FlagOn
#define FlagOn(Flags,SingleFlag) ((Flags) & (SingleFlag))
#endif

#ifndef SetFlag
#define SetFlag(Flags,SingleFlag) ((Flags) |= (SingleFlag))
#endif

#ifndef ClearFlag
#define ClearFlag(Flags,SingleFlag) ((Flags) &= ~(SingleFlag))
#endif

#ifndef BitN
#define BitN(x) (1<<x) 
#endif

typedef std::tr1::unordered_set<std_string> StringHashSet;
typedef StringHashSet::iterator StringHashSetItr;
typedef StringHashSet::const_iterator StringHashSetCItr;

inline std_string CreateGUIDString()
{
	TCHAR buf[64] = {0};
	GUID guid;
	if( S_OK == ::CoCreateGuid(&guid) )
	{
		_sntprintf( buf, sizeof(buf)
			, _T("%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X")
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	return buf;
}

inline const std_string& GetAppPath()
{
	static struct Helper {
		std_string strPath;
		Helper() 
		{
			TCHAR szFilePath[MAX_PATH];
			::GetModuleFileName(NULL, szFilePath, MAX_PATH);
			strPath = szFilePath;
			int pos = (int)strPath.find_last_of('\\');
			strPath = strPath.substr(0, pos + 1);
		}
	} helper;

	return helper.strPath;
}

inline BOOL GetFileSize(LPCTSTR lpszFileName,ULONGLONG &ulFileSize)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(lpszFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bRet=FindClose(hFind);
	assert(bRet);

	ulFileSize=MAKEULONGLONG(findFileData.nFileSizeLow,findFileData.nFileSizeHigh);

	return bRet;
}

inline bool ReadFileContent(HANDLE hFile,BYTE *buf,DWORD len)
{
	if (len == 0)
		return true;

	DWORD dwLeft=len;
	DWORD dwIndex=0;
	DWORD dwRead;
	while(dwLeft!=0)
	{
		if(!ReadFile(hFile,&buf[dwIndex],dwLeft,&dwRead,NULL) || dwRead == 0)
			return false;
		dwLeft-=dwRead;
		dwIndex+=dwRead;
	}
	return true;
}

inline bool WriteFileContent(HANDLE hFile,BYTE *buf,DWORD len)
{
	if (len == 0)
		return true;

	DWORD dwLeft=len;
	DWORD dwIndex=0;
	DWORD dwWrite;
	while(dwLeft!=0)
	{
		if(!WriteFile(hFile,&buf[dwIndex],dwLeft,&dwWrite,NULL) || dwWrite == 0)
			return false;
		dwLeft-=dwWrite;
		dwIndex+=dwWrite;
	}
	return true;
}

//-------------------------------------------------------------------
//Parameters返回由符号分隔的各个参数,返回值为解分析出的参数个数

inline size_t GetParameters(std::string strProtocol,char cSign,std::vector<std::string> &Parameters)
{
	if(strProtocol.find(cSign)==std::string::npos)
		Parameters.push_back(strProtocol);
	else
	{
		size_t i=0,j=0;
		while((j=strProtocol.find(cSign,j))!=std::string::npos)
		{
			Parameters.push_back(strProtocol.substr(i,j-i));
			i=j+1;
			j++;
		}
		Parameters.push_back(strProtocol.substr(i,strProtocol.size()-j));
	}
	return Parameters.size();
} 

inline BOOL UnZipGame(LPCTSTR lpZipFileName, LPCTSTR lpTargetPath, std_string& strErrorInfo)
{
	BOOL bRet=TRUE;
    std_string strUnixStyleTargetPath=lpTargetPath;
	std::replace(strUnixStyleTargetPath.begin(),strUnixStyleTargetPath.end(),'\\','/');
	HZIP hz=OpenZip((void*)lpZipFileName,0, ZIP_FILENAME,strUnixStyleTargetPath.c_str());
	if(hz==NULL)
	{
		strErrorInfo=_T("解压文件时打开文件失败:");
		strErrorInfo+=lpZipFileName;
		return FALSE;
	}

	for (int i=0;;i++)
	{ 
		ZIPENTRY ze; 
		ZRESULT res = GetZipItem(hz,i,&ze);
		if (res!=ZR_OK) 
			break; // no more

		res=UnzipItem(hz,i, (void*)(ze.name),0,ZIP_FILENAME);
		if(res!=ZR_OK)
		{
			char ErrorBuf[1024] = {0};
			FormatZipMessage(res,ErrorBuf,sizeof(ErrorBuf)-1);
			strErrorInfo=_T("解压文件失败,");
			strErrorInfo+=ErrorBuf;
			strErrorInfo+=lpZipFileName;
			bRet=FALSE;
			break;
		}
	}
	CloseZip(hz);
	return bRet;
}


inline BOOL ExtractZipContent(LPCTSTR lpZipFileName, 
							  LPCTSTR lpTargetPath, 
							  LPCTSTR lpContent, 
							  std_string& strErrorInfo)
{
	BOOL bRet=TRUE;
    std_string strUnixStyleTargetPath=lpTargetPath;
	std::replace(strUnixStyleTargetPath.begin(),strUnixStyleTargetPath.end(),'\\','/');
	HZIP hz=OpenZip((void*)lpZipFileName,0, ZIP_FILENAME,strUnixStyleTargetPath.c_str());
	if(hz==NULL)
	{
		strErrorInfo=_T("解压文件时打开文件失败:");
		strErrorInfo+=lpZipFileName;
		return FALSE;
	}

	int index;
	ZIPENTRY ze;
	ZRESULT res = FindZipItem(hz, lpContent, TRUE, &index, &ze);
	if (res != ZR_OK) {
		char ErrorBuf[1024] = {0};
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
		char ErrorBuf[1024] = {0};
		FormatZipMessage(res,ErrorBuf,sizeof(ErrorBuf)-1);
		strErrorInfo = _T("解压指定文件失败,");
		strErrorInfo += ErrorBuf;
		strErrorInfo += lpZipFileName;
		strErrorInfo += lpContent;
		CloseZip(hz);
		return FALSE;
	}

	CloseZip(hz);

	return TRUE;
}

inline std::string uri_encode(LPCTSTR lpszSouce)
{
	std::string source((LPCSTR)_bstr_t(lpszSouce));
	char element[4];
	unsigned char chr;
	std::string res;
	for ( unsigned int i=0; i<source.length(); i++ )
	{
		chr = source[i];
		if (chr > 127)		//汉字
		{
			sprintf(element, "%%%02X", chr);				
			res += element;
			chr = source[++i];
			sprintf(element, "%%%02X", chr);
			res += element;
		}
		else if ( (chr >= '0' && chr <= '9') || 
			(chr >= 'a' && chr <= 'z') ||
			(chr >= 'A' && chr <= 'Z') )
		{
			res += chr;
		}
		else
		{
			sprintf( element, "%%%02X", chr); 
			res += element;
		}
	}	

	return res;
}

int CalBufCRC32(const unsigned char* buffer, DWORD dwSize);

bool IsValidChar(const TCHAR *buf);


//目录名最后不带'\'
inline BOOL GetFileTreeList(LPCTSTR lpPath,std::vector<std_string> &vFileList,std::vector<ULONGLONG> &vSizeList)
{
	BOOL bRet;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwErrorCode;
	std_string strPath,strFileName,strSubPath;
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


inline BOOL GetFileTreeList2(LPCTSTR lpPath,
							 std::vector<std_string> &vFileList,
							 std::vector<std_string> &vDirList,
							 std::vector<ULONGLONG> &vSizeList)
{
	BOOL bRet;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwErrorCode;
	std_string strPath,strFileName,strSubPath;
	strPath=lpPath;
	strPath+=_T("\\*.*");
	hFind = FindFirstFile(strPath.c_str(), &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE)
		return FALSE;

	i8desk::strlwr(FindFileData.cFileName);
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
		i8desk::strlwr(FindFileData.cFileName);
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



//把ULONGLONG的时间转换成string类型的时间
inline std_string DateTimeToString(ULONGLONG ulDT)
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

inline BOOL GetFileTreeList3(LPCTSTR lpPath,std::vector<std_string> &vFileList,std::vector<std_string> &vDirList,std::vector<ULONGLONG> &vSizeList,std::vector<std_string> &vCreateTimeList)
{
	BOOL bRet;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwErrorCode;
	std_string strPath,strFileName,strSubPath;
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

//目录名最后不带'\'
inline BOOL GetIconList(LPCTSTR lpPath,std::vector<std_string> &vFileList)
{
	BOOL bRet;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwErrorCode;
	std_string strPath,strFileName,strSubPath;
	strPath=lpPath;
	strPath+=_T("\\*.ICO");
	hFind = FindFirstFile(strPath.c_str(), &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE)
		return FALSE;


	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		strFileName=lpPath;
		strFileName+=_T("\\");
		strFileName+=FindFileData.cFileName;
		vFileList.push_back(strFileName);
	}

	while (FindNextFile(hFind,&FindFileData))
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			strFileName=lpPath;
			strFileName+=_T("\\");
			strFileName+=FindFileData.cFileName;
			vFileList.push_back(strFileName);
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

//目录名最后不带'\'
inline BOOL GetTaskList(LPCTSTR lpPath,std::vector<std_string> &vFileList)
{
	BOOL bRet;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwErrorCode;
	std_string strPath,strFileName,strSubPath;
	strPath=lpPath;
	strPath+=_T("\\0*.TA");
	hFind = FindFirstFile(strPath.c_str(), &FindFileData);
	if(hFind==INVALID_HANDLE_VALUE)
		return FALSE;


	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		strFileName=lpPath;
		strFileName+=_T("\\");
		strFileName+=FindFileData.cFileName;
		vFileList.push_back(strFileName);
	}

	while (FindNextFile(hFind,&FindFileData))
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			strFileName=lpPath;
			strFileName+=_T("\\");
			strFileName+=FindFileData.cFileName;
			vFileList.push_back(strFileName);
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

//创建游戏目录
inline BOOL CreateGameDirectory(LPCTSTR lpszPathName)
{
	std::vector<std_string> vSubDir;
	size_t sSubDirCount=GetParameters(lpszPathName,'\\',vSubDir);
	std_string strPath;
	for (size_t i=0;i<vSubDir.size();i++)
	{
		if (vSubDir[i].empty())
			break;

		strPath+=vSubDir[i];
		if (!PathFileExists(strPath.c_str()))
		{
			if (!CreateDirectory(strPath.c_str(),NULL))
				return FALSE;
		}

		strPath+=_T("\\");
	}

	return TRUE;
}

BOOL DeleteGameDirectory(LPCTSTR lpPath);

inline BOOL DeleteGameDirectory(const std::string& path)
{
	return DeleteGameDirectory(path.c_str());
}

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

inline LPCTSTR GetFileVer(const std_string& strFileName)
{
	static struct Helper {
		Helper() { m_lock.Create(); }
		LPCTSTR GetVer(const std_string& strFileName) 
		{
			I8_GUARD(ZGuard, ZSync, m_lock);
			std_string& str = m_vers[strFileName];
			if (!str.empty()) {
				return str.c_str();
			}
			str = GetVerHelper(strFileName);
			if (!str.empty())
				return str.c_str();
			return _T("0.0.0.0");
		}
	private:
		std_string GetVerHelper(const std_string& strFileName) 
		{
			std_string strVer;
			DWORD dwHandle = 0;
			DWORD dwSize = 0;
			dwSize = GetFileVersionInfoSize(strFileName.c_str(), &dwHandle);
			if (dwSize) {
				//TCHAR *lpData = new TCHAR[dwSize];
				char *lpData = I8MP.alloc(dwSize);
				if (GetFileVersionInfo(strFileName.c_str(), 0, dwSize, lpData)) {
					LPVOID lpBuffer = 0;
					UINT uLen = 0;
					BOOL bRet = VerQueryValue(lpData,
						_T("\\StringFileInfo\\080404b0\\FileVersion"), &lpBuffer, &uLen);
					strVer = (TCHAR *)lpBuffer;
				}
				//delete[] lpData;
				I8MP.free(lpData);
			}
			std::replace(strVer.begin(), strVer.end(), _T(','), _T('.'));

			return strVer;
		}
		ZSync m_lock;
		std::map<std_string, std_string> m_vers;
	} helper;

	return helper.GetVer(strFileName);
}

//得到系统配置文件中的一些配置
inline LPCTSTR GetI8deskSysConfig(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue)
{
	static struct Helper {
		Helper() { m_lock.Create(); }
		LPCTSTR GetConfig(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue)
		{
			I8_GUARD(ZGuard, ZSync, m_lock);
			std_string& str = m_configs[szSection][szKeyName];
			if (!str.empty()) {
				return str.c_str();
			}
			str = GetConfigHelper(szSection, szKeyName);
			if (!str.empty())
				return str.c_str();
			return szDefValue;
		}
	private:
		std_string GetConfigHelper(LPCTSTR szSection, LPCTSTR szKeyName)
		{
			std_string szSysConfig = GetAppPath() + TEXT("i8desksys.config");
			std_string szDestFile  = GetAppPath() + TEXT("Data\\i8desksys.config.ini");

			SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szDestFile.c_str());

			HANDLE hFile = CreateFile(szSysConfig.c_str(), 
				GENERIC_READ, 
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 
				0, 
				NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				try
				{
					DWORD dwSize = GetFileSize(hFile, NULL);
					//char *pSrc = new char[dwSize];
					char *pSrc = I8MP.alloc(dwSize);
					DWORD dwReadBytes = 0;
					ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL);
					CloseHandle(hFile);

					int dwNewSize = dwSize;
					//BYTE* pDst = new BYTE[dwNewSize];
					BYTE *pDst = (BYTE *)I8MP.alloc(dwNewSize);
					Base64Decode(pSrc, dwSize, pDst, &dwNewSize);
					hFile = CreateFile(szDestFile.c_str(),
						GENERIC_WRITE, 
						FILE_SHARE_READ,
						NULL,
						CREATE_NEW, 
						0,
						NULL);
					if (hFile != INVALID_HANDLE_VALUE)
					{
						SetFileAttributes(szDestFile.c_str(),
							FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SYSTEM);
						WriteFile(hFile, pDst, dwNewSize, &dwReadBytes, NULL);
						CloseHandle(hFile);

						TCHAR buf[4096] = {0};
						::GetPrivateProfileString(szSection, szKeyName,
							_T(""), buf, sizeof(buf), szDestFile.c_str());
						DeleteFile(szDestFile.c_str());
						//delete []pSrc;
						//delete []pDst;
						I8MP.free(pSrc);
						I8MP.free(pDst);
						return std_string(buf);
					}
					//delete []pSrc;
					//delete []pDst;
					I8MP.free(pSrc);
					I8MP.free(pDst);
				}
				catch (...) { }
			}
			return _T("");
		}
		ZSync m_lock;
		std::map<std_string, std::map<std_string, std_string> > m_configs;
	} helper;
	
	return helper.GetConfig(szSection, szKeyName, szDefValue);
}

typedef struct tagFileItem
{
	DWORD dwFileCrc32;
	char  FileName[MAX_PATH];
}tagFileItem;

typedef struct tagIdxFile
{
	DWORD dwCrc32;				//后面内容的ＣＲＣ.
	DWORD dwVersion;			//索引的版本，用_time32(NULL)来表示.只需要唯一即可
	DWORD dwFileCount;			//表示后面文件列表的个数.
	tagFileItem FileList[1];	//具体的文件项列表
}tagIdxFile;

BOOL GetFileCrc32(LPCTSTR lpFileName,DWORD& dwCrc32);
BOOL GetFileCrc32(HANDLE hFile,DWORD& dwCrc32);

#endif