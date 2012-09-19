#ifndef __UTILITY_HPP
#define __UTILITY_HPP

#include <string>
#include <type_traits>


namespace common
{
	inline const std::wstring& GetAppPath()
	{
		static struct Helper 
		{
			std::wstring strPath;
			Helper() 
			{
				wchar_t szFilePath[MAX_PATH] = {0};
				::GetModuleFileNameW(NULL, szFilePath, _countof(szFilePath));
				strPath = szFilePath;
				size_t pos = strPath.find_last_of('\\');
				strPath = strPath.substr(0, pos + 1);
			}
		} helper;

		return helper.strPath;
	}


	namespace detail
	{
		struct IniConfigImpl
		{
			template < typename T >
			T operator()(LPCWSTR fileName, LPCWSTR section, LPCWSTR key, T)
			{
				std::wstring path = common::GetAppPath();
				path += fileName;
				T val = (T)::GetPrivateProfileIntW(section, key, 0, path.c_str());

				return val;
			}

			std::wstring operator()(LPCWSTR fileName, LPCWSTR section, LPCWSTR key, std::wstring)
			{
				wchar_t val[64] = {0};
				std::wstring path = common::GetAppPath();
				path += fileName;
				::GetPrivateProfileStringW(section, key, L"127.0.0.1", val, 64, path.c_str());

				return val;
			}
		};
	}

	template < typename T >
	inline T GetIniConfig(LPCWSTR fileName, LPCWSTR section, LPCWSTR key)
	{
		return detail::IniConfigImpl()(fileName, section, key, T());
	}
}




#endif