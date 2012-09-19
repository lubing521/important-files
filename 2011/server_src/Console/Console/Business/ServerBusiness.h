#ifndef __BUSINESS_SERVER_HPP
#define __BUSINESS_SERVER_HPP

#include "../Data/DataHelper.h"


namespace i8desk
{
	namespace business
	{
		namespace Server
		{
			// 是否运行在主服务器上
			bool IsRunOnMainServer();

			// 是否是主服务器名称
			bool IsMainServer(const data_helper::ServerTraits::ValueType &svr);

			// 获取所有服务器
			void GetAllService(const std::tr1::function<void(LPCTSTR, LPCTSTR)> &callback);

			// 通过SID获得Service
			const data_helper::ServerTraits::ValueType &GetServiceBySID(const stdex::tString &sid);

			// 过滤服务器虚拟盘
			void VDiskFilter(data_helper::VDiskClientTraits::VectorType &disks, const stdex::tString &svrName, const stdex::tString &vID);
		
			// 过滤服务器状态
			void ServerFilter(data_helper::ServerStatusTraits::VectorType &servers, const stdex::tString &serverName);
			

			// 检测名字是否重复
			bool HasSameName(const stdex::tString &svrID, const stdex::tString &svrName);

			// 修改区域优先
			bool ModifyAreaPriority(bool isNew, const data_helper::AreaTraits::ElementType *area, const stdex::tString &svrName);
		
			// 是否是主服务器IP
			bool IsMainServerIP(const ulong svrip);

			// 检测该服务器是否已存在
			bool IsServerExsit(const stdex::tString &SID);

			// 获取主服务器
			const data_helper::ServerTraits::ValueType &GetMainServer();

			// 通过IP找服务器ID
			stdex::tString GetServerIDBySvrIP(ulong ip);

			// 通过SID删除服务器
			bool Delete(const stdex::tString &SID, const std::tr1::function<void(LPCTSTR, LPCTSTR)> &error);

			// 格式化服务器IP
			void FormatIP(const stdex::tString &svrID, stdex::tString &text);

			// 格式化服务器磁盘信息
			void FormatDisk(const stdex::tString &svrID, stdex::tString &text);

			// 服务器运行方式
			void GetBalanceTypes(const std::tr1::function<void(int, LPCTSTR)> &callback);
			LPCTSTR GetBalanceTypeDescByType(int type);

			// 服务器类型
			void GetSvrTypes(const std::tr1::function<void(int, LPCTSTR)> &callback);
			LPCTSTR GetSvrTypeDescByType(int type);

			// 通过SID找服务器区域优先
			void GetSvrAreaPriotyDescBySID(const stdex::tString &svrID, stdex::tString &text);

			// 通过SID获得Service名称
			stdex::tString GetNameBySID(const stdex::tString &sid);

		}
	}
}

#endif