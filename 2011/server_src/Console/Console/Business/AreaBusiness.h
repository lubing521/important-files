#ifndef __BUSINESS_AREA_HPP
#define __BUSINESS_AREA_HPP

#include "../Data/DataHelper.h"


namespace i8desk
{
	namespace business
	{

		// ---------------------------------
		// Area

		namespace Area
		{
			// 从AID得到名称
			const stdex::tString &GetAIDByName(const stdex::tString &name);
		
			// 是否为默认分区
			bool IsDefaultArea(const data_helper::AreaTraits::ValueType &val);

			// 确保默认分区存在
			void EnsureDefaultArea();

			// 解析区域
			bool ParseArea(const stdex::tString &area, data_helper::RunTypeTraits::ValueType &runType);

			// 处理区域运行方式
			stdex::tString ParseRunType(const stdex::tString &AID, const stdex::tString &areaRunType);
			stdex::tString ParseRunType(const stdex::tString &AID, const data_helper::RunTypeTraits::VectorType &runTypes);
		
			// 检测名字是否重复
			bool HasSameName(const data_helper::AreaTraits::ValueType &val);

		}
	}

}


#endif