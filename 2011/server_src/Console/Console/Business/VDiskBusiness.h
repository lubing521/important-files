#ifndef __BUSINESS_VDISK_HPP
#define __BUSINESS_VDISK_HPP

#include "../Data/DataHelper.h"

namespace i8desk
{
	namespace business
	{
		// -----------------------------
		// VDISK

		namespace VDisk
		{
			extern u_short VDiskPort;

			// 处理运行方式
			void RunTypeDesc(const data_helper::VDiskTraits::ValueType &val, stdex::tString &msg);

			template < typename CallBackT >
			void RunTypeDesc(const CallBackT &callback)
			{
				i8desk::GetDataMgr().GetAllData(i8desk::GetDataMgr().GetVDisks());

				using data_helper::VDiskTraits;
				const VDiskTraits::MapType &vDisks = GetDataMgr().GetVDisks();

				stdex::tString msg;
				for(VDiskTraits::MapConstIterator iter = vDisks.begin();
					iter != vDisks.end(); ++iter)
				{
					RunTypeDesc(iter->second, msg);

					callback(msg.c_str(), iter->second);
				}
			}


			// 检测虚拟盘链接
			void Check(const std::string &ip, u_short port, CString &result);

			// 刷新虚拟盘
			void Refresh(const std::string &ip, u_short port, CString &result);

			// 检测虚拟盘服务器盘符与客户机盘符唯一性
			bool IsValid(const stdex::tString &curVID, ulong ip, TCHAR svrDrv, TCHAR cliDrv, const std::tr1::function<void(LPCTSTR, LPCTSTR)> &errCallback);
		
			// 获取虚拟盘名称
			stdex::tString GetVDiskName(const data_helper::VDiskTraits::ValueType &val);
		
			// 删除虚拟盘
			bool Delete(const stdex::tString &VID);

			// 获取虚拟盘刷盘方式
			void GetLoadTypes(const std::tr1::function<void(int, LPCTSTR)> &callback);
		
			// 通过刷盘方式获取虚拟盘刷盘方式描述
			LPCTSTR GetLoadTypeDescByType(int type);

			void GetSsdDrv(const std::tr1::function<void(int, LPCTSTR)> &callback);

			LPCTSTR GetSsdDescByType(int type);

		}

	}

}


#endif