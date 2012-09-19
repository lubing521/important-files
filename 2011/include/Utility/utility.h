#ifndef __UTILITY_HPP
#define __UTILITY_HPP

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdio>
#include <cassert>
#include <ctime>
#include <atlconv.h>
#include <atlbase.h>
#include <atlenc.h>
#include <atlconv.h>
#include <ShellAPI.h>
#include <tlhelp32.h>
#include <algorithm> 



#include "../Extend STL/UnicodeStl.h"
#include "SmartHandle.hpp"

#include <Winver.h>
#pragma comment(lib, "Version.lib")

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")

#include <IPHlpApi.h>
#pragma comment(lib, "iphlpapi")

#include <Psapi.h>
#pragma comment(lib, "Psapi")

#include <Userenv.h>
#pragma comment(lib, "Userenv")

namespace utility 
{

	// 工具函数

	inline const stdex::tString& GetAppPath()
	{
		static struct Helper 
		{
			stdex::tString strPath;
			Helper() 
			{
				TCHAR szFilePath[MAX_PATH] = {0};
				::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
				strPath = szFilePath;
				int pos = (int)strPath.find_last_of('\\');
				strPath = strPath.substr(0, pos + 1);
			}
		} helper;

		return helper.strPath;
	}


	//得到系统配置文件中的一些配置
	inline stdex::tString GetOpt(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefValue)
	{
		stdex::tString szSysConfig = GetAppPath() + _T("oem\\oem.ini");
		stdex::tString szDestFile  = GetAppPath() + _T("Data\\oem.ini");

		SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szDestFile.c_str());

		utility::CAutoFile hFile = CreateFile(szSysConfig.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hFile.IsValid())
		{
			try
			{
				DWORD dwSize = ::GetFileSize(hFile, NULL);
				ATL::CAutoVectorPtr<char> pSrc(new char[dwSize]);
				DWORD dwReadBytes = 0;
				if (!ReadFile(hFile, pSrc, dwSize, &dwReadBytes, NULL))
				{
					return stdex::tString(szDefValue);
				}

				int dwNewSize = dwSize;
				ATL::CAutoVectorPtr<BYTE> pDst(new BYTE[dwNewSize]);
				ATL::Base64Decode(pSrc, dwSize, pDst, &dwNewSize);


				utility::CAutoFile hFileDest = CreateFile(szDestFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
				if (hFileDest.IsValid())
				{
					SetFileAttributes(szDestFile.c_str(), FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_SYSTEM);
					WriteFile(hFileDest, pDst, dwNewSize, &dwReadBytes, NULL);

					TCHAR buf[4096] = {0};
					::GetPrivateProfileString(szSection, szKeyName, szDefValue, buf, _countof(buf), szDestFile.c_str());
					DeleteFile(szDestFile.c_str());

					return stdex::tString(buf);
				}

			}
			catch (...) { }
		}

		return stdex::tString(szDefValue);
	}


	inline stdex::tString GetFileVersion(const stdex::tString& szFile)
	{
		TCHAR version[MAX_PATH] = _T("0.0.0.0");
		DWORD dwSize = GetFileVersionInfoSize(szFile.c_str(), NULL);
		if(dwSize)
		{
			LPTSTR pblock = new TCHAR[dwSize+1];
			GetFileVersionInfo(szFile.c_str(), 0, dwSize, pblock);
			UINT nQuerySize;
			DWORD* pTransTable = NULL;
			VerQueryValue(pblock, _T("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
			LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
			TCHAR SubBlock[MAX_PATH] = {0};
			_stprintf_s(SubBlock, _T("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
			LPTSTR lpData;
			VerQueryValue(pblock, SubBlock, (PVOID*)&lpData, &nQuerySize);
			_tcscpy_s(version, lpData);
			delete[] pblock;
		}
		stdex::tString str(version);

		// replace ',' to '.'
		std::replace_if(str.begin(), str.end(), 
			std::bind2nd(std::equal_to<stdex::tString::value_type>(), ','), '.');

		// delete [Space]
		stdex::tString::iterator iter = std::remove_if(str.begin(), str.end(),
			std::bind2nd(std::equal_to<stdex::tString::value_type>(), ' '));

		// remove redundant character
		if( iter != str.end() )
			str.erase(iter, str.end());

		return str;
	}


	// ---------------------------------------------------------------------
	// 字符串操作

	template<size_t N1, size_t N2>
	inline int Stricmp(const char (&arr1)[N1], const char (&arr2)[N2])
	{
		return ::_strnicmp(arr1, arr2, std::min(N1, N2));
	}

	template<size_t N1, size_t N2>
	inline int Stricmp(const wchar_t (&arr1)[N1], const wchar_t (&arr2)[N2])
	{
		return ::_wcsnicmp(arr1, arr2, std::min(N1, N2));
	}

	inline int Stricmp(const char *str1, const char *str2)
	{
		return ::_stricmp(str1, str2);
	}

	inline int Stricmp(const wchar_t *str1, const wchar_t *str2)
	{
		return ::_wcsicmp(str1, str2);
	}

	template < typename CharT >
	inline int Stricmp(const CharT *str1, const std::basic_string<CharT> &str2)
	{
		return Stricmp(str1, str2.c_str());
	}


	template < size_t N1, size_t N2 >
	inline int Strcmp(const char (&arr1)[N1], const char (&arr2)[N2])
	{
		return ::strncmp(arr1, arr2, std::min(N1, N2));
	}

	template < size_t N1, size_t N2 >
	inline int Strcmp(const wchar_t (&arr1)[N1], const wchar_t (&arr2)[N2])
	{
		return ::wcsncmp(arr1, arr2, std::min(N1, N2));
	}

	inline int Strcmp(const char *str1, const char *str2)
	{
		return ::strcmp(str1, str2);
	}

	inline int Strcmp(const wchar_t *str1, const wchar_t *str2)
	{
		return ::wcscmp(str1, str2);
	}



	namespace detail
	{
		inline void Strcpy(char *dest, size_t cnt, const char *src)
		{
			::strcpy_s(dest, cnt, src);
		}
		inline void Strcpy(wchar_t *dest, size_t cnt, const wchar_t *src)
		{
			::wcscpy_s(dest, cnt, src);
		}
	}

	inline void Strcpy(wchar_t *dest, size_t cnt, const wchar_t *src)
	{
		::wcscpy_s(dest, cnt, src);
	}

	template < size_t _Size >
	inline void Strcpy(char (&des)[_Size], const char *src)
	{
		strncpy(des, src, _Size - 1);
	}

	template < size_t _Size >
	inline void Strcpy(wchar_t (&des)[_Size], const wchar_t *src)
	{
		wcsncpy_s(des, src, _Size - 1);
	}

	template< typename CharT, size_t N >
	inline void Strcpy(CharT (&arr)[N], const std::basic_string<CharT> &str)
	{
		Strcpy(arr, N, str.c_str());
	}
	


	namespace detail
	{
		template < size_t N1, size_t N2 >
		inline void Strcat(char (&des)[N1], const char (&src)[N2])
		{
			assert(::strlen(des) + ::strlen(src) < N1);
			::strcat_s(des, src);
		}

		template < size_t N1, size_t N2 >
		inline void Strcat(wchar_t (&des)[N1], const wchar_t (&src)[N2])
		{
			assert(::wcslen(des) + ::wcslen(src) < N1);
			::wcscat_s(des, src);
		}
	}

	template < typename CharT, size_t N1, size_t N2 >
	inline void Strcat(CharT (&des)[N1], const CharT (&src)[N2])
	{
		detail::Strcat(des, src);
	}


	namespace detail
	{
		inline size_t StrLen(const char *des)
		{
			return ::strlen(des);
		}

		inline size_t Strcat(const wchar_t *des)
		{
			return ::wcslen(des);
		}
	}

	template < typename CharT, size_t N >
	inline size_t StrLen(const CharT (&des)[N])
	{
		return detail::StrLen(des);
	}

	template < typename CharT >
	inline size_t StrLen(const CharT *des)
	{
		return detail::StrLen(des);
	}


	// ClearString
	template<typename CharT, size_t N>
	inline void ClearString(CharT (&str)[N])
	{
		::memset(str, 0, sizeof(CharT) * N);
	}

	// IsEmpty
	inline bool IsEmpty(const char *str)
	{
		return str[0] == 0;
	}
	inline bool IsEmpty(const wchar_t *str)
	{
		return str[0] == 0;
	}


	// LastChar
	template<typename CharT, size_t N>
	inline CharT LastChar(const CharT (&arr)[N])
	{
		return arr[_tcslen(arr) - 1];
	}
	template<typename CharT>
	inline CharT LastChar(const CharT *str)
	{
		return str[_tcslen(CharT) - 1];
	}



	inline stdex::tString CreateGuidString()
	{
		GUID guid;
		UuidCreate(&guid);

#ifdef _UNICODE
		typedef RPC_WSTR RPC_TSTR;
#else
		typedef RPC_CSTR RPC_TSTR;
#endif

		RPC_TSTR rpcStr;
		UuidToString(&guid, &rpcStr);

		stdex::tString str = reinterpret_cast<TCHAR *>(rpcStr);

		RpcStringFree(&rpcStr);


		str.insert(0, _T("{")); 
		str += _T("}");

		return str;
	}

	inline bool IsValidDirName(stdex::tString strDir)
	{
		if (strDir.size() < 2)
			return false;
		if (!isalpha(strDir[0]) || strDir[1] != ':')
			return false;

		if (*strDir.rbegin() != '\\')
			strDir += _T("\\");
		size_t nStart = strDir.find('\\');
		if (nStart != stdex::tString::npos)
		{
			size_t nEnd = 0;
			while ((nEnd = strDir.find_first_of('\\', nStart+1)) != stdex::tString::npos)
			{
				stdex::tString sub = strDir.substr(nStart+1, nEnd - nStart - 1);
				if (sub.size() == 0 || sub.find_first_of(_T("<>:\"/|?*")) != stdex::tString::npos)
					return false;
				nStart = nEnd + 1;
			}
		}
		return true;
	}


	//从一个路径中得到最后一级的目录，不包含'\'
	inline stdex::tString ExtractLastDir(const stdex::tString& path)
	{
		assert(path.size() >= 2);
		size_t off = path[path.size() - 1] == _T('\\') ? 2 : 1;
		size_t pos = path.rfind(_T('\\'), path.size() - off);	
		if (pos != stdex::tString::npos) {
			return path.substr(pos + 1, path.size() - pos - off);
		}
		return path.substr(0, path.size() - off + 1);
	}



	static const unsigned long Crc32Table[256] = {
		0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 
		0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
		0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
		0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
		0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
		0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
		0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
		0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
		0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
		0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
		0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
		0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
		0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
		0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
		0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
		0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
		0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
		0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
		0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
		0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
		0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
		0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
		0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
		0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
		0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
		0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
		0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
		0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
		0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
		0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
		0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
		0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
		0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
		0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
		0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
		0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
		0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
		0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
		0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
		0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
		0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
		0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
		0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
		0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
		0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
		0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
		0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
		0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
		0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
		0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
		0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
		0x2d02ef8dL
	};


	//可以组合计算多个片段的CRC类
	class crc32
	{
	public:
		crc32() : m_crc(0xffffffff) 
		{
		}

		inline crc32& add(void const *pbuf, size_t len) 
		{
			unsigned char *buffer = (unsigned char *)pbuf;
			while(len--)
				m_crc = (m_crc >> 8) ^ Crc32Table[(m_crc & 0xFF) ^ *buffer++];
			return *this;
		}

		template<typename T> 
		inline crc32& add(const T& var)
		{
			return add(&var, sizeof(T));
		}

		template<typename T, size_t _Size> 
		inline crc32& add(const T (&_Array)[_Size])
		{
			return add(_Array, _Size * sizeof(T));
		}

		ULONG get(void) const 
		{
			return m_crc^0xffffffff;
		}

		operator ULONG() const 
		{
			return get();
		}
	private:
		ULONG m_crc;
	};

	template<typename T> 
	inline crc32& operator<<(crc32& crc, const T& var)
	{
		return crc.add(var);
	}

	template<typename T, size_t _Size>
	inline crc32& operator<<(crc32& crc, const T (&_Array)[_Size])
	{
		return crc.add(_Array);
	}

	inline ULONG CalBufCRC32(void const *pbuf, DWORD dwSize)
	{
		return crc32().add(pbuf, dwSize).get();
	}


	class CEvenyOneSD 
	{
	public:
		PVOID Build(PSECURITY_DESCRIPTOR pSD) 
		{
			PSID   psidEveryone = NULL;
			PACL   pDACL   = NULL;
			BOOL   bResult = FALSE;

			__try 
			{
				SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
				//SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

				if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
					__leave;

				if (!AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone)) 
					__leave;

				DWORD dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(psidEveryone);

				pDACL = (PACL) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
				if (!pDACL) 
					__leave;

				if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) 
					__leave;

				if (!AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone)) 
					__leave;

				if (!SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE)) 
					__leave;
				bResult = TRUE;
			} 
			__finally 
			{
				if (psidEveryone) 
					FreeSid(psidEveryone);
			}

			if (bResult == FALSE) 
			{
				if (pDACL) HeapFree(GetProcessHeap(), 0, pDACL);
				pDACL = NULL;
			}

			return (PVOID) pDACL;
		}
		CEvenyOneSD()
		{
			ptr=NULL;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = &sd;
			sa.bInheritHandle = FALSE;
			ptr = Build(&sd);
		}
		~CEvenyOneSD()
		{
			if(ptr)
			{
				HeapFree(GetProcessHeap(), 0, ptr);
			}
		}
		SECURITY_ATTRIBUTES* GetSA()
		{
			return (ptr != NULL) ? &sa : NULL;
		}
	protected:
		PVOID  ptr;
		SECURITY_ATTRIBUTES sa;
		SECURITY_DESCRIPTOR sd;
	};


	
	//检查文件是否存在
	inline BOOL IsFileExist(LPCTSTR lpszFile)
	{	    
        DWORD dwFileAtt = GetFileAttributes(lpszFile);
        if (dwFileAtt == INVALID_FILE_ATTRIBUTES)
        {
            return FALSE;
        }
        return !(dwFileAtt & FILE_ATTRIBUTE_DIRECTORY);
	}
	//检查文件夹是否存在
	inline BOOL IsDirectoryExist(LPCTSTR lpszDirectory)
	{	    
        DWORD dwFileAtt = GetFileAttributes(lpszDirectory);
        if (dwFileAtt == INVALID_FILE_ATTRIBUTES)
        {
            return FALSE;
        }
        return dwFileAtt & FILE_ATTRIBUTE_DIRECTORY;
	}


	// URL Encode
	inline stdex::tString URLEncode(const stdex::tString &url)
	{
		std::string source = CT2A(url.c_str());
		char element[4] = {0};
		unsigned char chr = 0;
		std::string tmp;

		for(size_t i=0; i<source.length(); ++i)
		{
			chr = source[i];
			if (chr > 127)		//汉字
			{
				sprintf_s(element, "%%%02X", chr);				
				tmp += element;
				chr = source[++i];
				sprintf_s(element, "%%%02X", chr);
				tmp += element;
			}
			else if ( (chr >= '0' && chr <= '9') || 
				(chr >= 'a' && chr <= 'z') ||
				(chr >= 'A' && chr <= 'Z') )
			{
				tmp += chr;
			}
			else
			{
				sprintf_s( element, "%%%02X", chr); 
				tmp += element;
			}
		}	

		stdex::tString res(CA2T(tmp.c_str()));
		return res;
	}
	
    inline std::wstring Base64Decode(LPCTSTR lpszBase64String)
    {
        std::string sSrcString = CW2A(lpszBase64String);
        int iDesLen = ATL::Base64DecodeGetRequiredLength(sSrcString.length()) + 1;
        std::vector<char> vec(iDesLen);

        ATL::Base64Decode(sSrcString.c_str(), sSrcString.length() + 1, reinterpret_cast<BYTE*>(&vec[0]), &iDesLen);

        return std::wstring(CA2W(&vec[0]));
    }

}

#endif 