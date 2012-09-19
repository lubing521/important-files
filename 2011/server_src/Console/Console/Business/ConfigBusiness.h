#ifndef __CONFIG_BUSINESS_HPP
#define __CONFIG_BUSINESS_HPP

#include <functional>


namespace i8desk
{
	namespace business
	{
		namespace config
		{
			// 获取所有省名称
			void GetProvinceName(const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			// 获取省下所有市名称
			void GetCityName(size_t parentID, const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			// 获取磁盘类型描述
			void GetDiskType(const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			// 获取网络类型描述
			void GetNetworkType(const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			// 获取磁盘类型名称
			stdex::tString GetDiskNameByType(size_t type);

			// 获取网络名称
			stdex::tString GetNetworkByType(size_t type);

		
			// 获取无盘产品名称
			void GetDiskProduct(const std::tr1::function<void(LPCTSTR, size_t)> &callback);
		
			// 获取收费软件名称
			void GetFeeProductName(const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			// 获取文化软件名称
			void GetCulturalProductName(const std::tr1::function<void(LPCTSTR, size_t)> &callback);

			//判断盘符是否存在，盘符不存在则提示，存在则看路径是否存在，路径不存在则创建
			bool IsValidPath(LPCTSTR path);
		}
	}
}


#endif