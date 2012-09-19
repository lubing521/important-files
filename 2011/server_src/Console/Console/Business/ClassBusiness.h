#ifndef __CLASS_BUSINESS_HPP
#define __CLASS_BUSINESS_HPP

#include "../Data/DataHelper.h"


namespace i8desk
{
	namespace business
	{

		namespace Class
		{
			// 根据Class Name获取CID
			stdex::tString GetCID(const stdex::tString &className);

			// 根据Class CID得到Name
			stdex::tString GetName(const stdex::tString &CID);

			// 是否为单机游戏
			bool IsSingleGame(const stdex::tString &CID);

			// 得到默认排序后的所有Class表
			void GetAllSortedClasses( data_helper::ClassTraits::VectorType &classes );

			// 得到默认排序后的官方Class表
			void GetSortDefaultClass( data_helper::ClassTraits::VectorType &classes );

			// 是否为默认类别
			bool IsDefault(const stdex::tString &CID);

			// 获取图标
			const utility::BITMAPPtr &GetIcon(const data_helper::ClassTraits::ValueType &val);

			// 设置图标
			void SetIcon(const data_helper::ClassTraits::ValueType &val, const utility::BITMAPPtr &icon);
		
			// 检测名字是否重复
			bool HasSameName(const data_helper::ClassTraits::ValueType &val);
			
			// 获取路径
			stdex::tString GetPath(const data_helper::ClassTraits::ValueType &val);

			// 检测类别下是否存在游戏
			bool HasGame(const stdex::tString &CID);

			// 设置桌面快捷方式
			void SetDeskTopClass( const data_helper::ClassTraits::ValueType &val);

			// 取消桌面快捷方式
			void UnSetDeskTopClass(const data_helper::ClassTraits::ValueType &val);

			// 是否设置桌面快捷方式
			bool IsSetDeskTopClass(const data_helper::ClassTraits::ValueType &val);

		}
	}
}



#endif