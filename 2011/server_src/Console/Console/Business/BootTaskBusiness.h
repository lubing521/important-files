#ifndef __BOOTTASK_BUSINESS_HPP
#define __BOOTTASK_BUSINESS_HPP

#include "../ManagerInstance.h"
#include <atlpath.h>

namespace i8desk
{
	namespace business
	{

		namespace BootTask
		{
			// 获取开机任务区域描述
			stdex::tString GetAreaDesc(const data_helper::BootTaskTraits::ValueType &val);

			// 获取执行日期段
			stdex::tString GetRangeDate(const data_helper::BootTaskTraits::ValueType &val);

			// 获取执行时间段
			stdex::tString GetRangeTime(const data_helper::BootTaskTraits::ValueType &val);

			// 获取开机执行类型
			stdex::tString GetRunType(const data_helper::BootTaskTraits::ValueType &val);
			
			// 获取开机任务类型描述
			stdex::tString GetTypeDesc(const data_helper::BootTaskTraits::ValueType &val);

			// 描述信息回调
			typedef std::tr1::function<int (LPCTSTR)> TypeDescCallback;
			typedef std::tr1::function<int (int, DWORD_PTR)> TypeDataCallback;
			void TypeDesc(const TypeDescCallback &descCallback, const TypeDataCallback &dataCallback);

			// 检测名字是否重复
			bool HasSameName(const data_helper::BootTaskTraits::ValueType &val);

			// 添加
			template < typename AIDT >
			bool Add(const data_helper::BootTaskTraits::ValueType &bootTask, const AIDT &aids)
			{
				using data_helper::BootTaskTraits;
				using data_helper::BootTaskAreaTraits;

				BootTaskTraits::MapType &bootTasks = GetDataMgr().GetBootTasks();
				BootTaskAreaTraits::MapType &bootAreas = GetDataMgr().GetBootTaskAreas();

				if( !GetDataMgr().AddData(bootTasks, bootTask->TID, bootTask) )
					return false;

				for(typename AIDT::const_iterator iter = aids.begin(); iter != aids.end(); ++iter)
				{
					BootTaskAreaTraits::ValueType val(new BootTaskAreaTraits::ElementType);

					utility::Strcpy(val->TID, bootTask->TID);
					utility::Strcpy(val->AID, *iter);

					if( !GetDataMgr().AddData(bootAreas, std::make_pair(val->TID, val->AID), val) )
						return false;
				}

				return true;
			}

			// 修改
			template < typename AIDT >
			bool Modify(const data_helper::BootTaskTraits::ValueType &bootTask, const AIDT &srcAID, AIDT &destAID)
			{
				using data_helper::BootTaskTraits;
				using data_helper::BootTaskAreaTraits;

				BootTaskTraits::MapType &bootTasks = GetDataMgr().GetBootTasks();

				if( !GetDataMgr().ModifyData(bootTasks, bootTask->TID, bootTask) )
					return false;

				BootTaskAreaTraits::MapType &bootAreas = GetDataMgr().GetBootTaskAreas();
				// 删除旧元素
				for(typename AIDT::const_iterator iter = srcAID.begin(); iter != srcAID.end(); ++iter)
				{
					if( destAID.find(*iter) != destAID.end() )
					{
						destAID.erase(*iter);
					}
					else
					{
						BootTaskAreaTraits::ValueType val(new BootTaskAreaTraits::ElementType);

						utility::Strcpy(val->TID, bootTask->TID);
						utility::Strcpy(val->AID, *iter);

						if( !GetDataMgr().DelData(bootAreas, std::make_pair(val->TID, val->AID)) )
							return false;
					}
				}

				// 添加新元素
				for(typename AIDT::const_iterator iter = destAID.begin(); iter != destAID.end(); ++iter)
				{
					BootTaskAreaTraits::ValueType val(new BootTaskAreaTraits::ElementType);

					utility::Strcpy(val->TID, bootTask->TID);
					utility::Strcpy(val->AID, *iter);

					if( !GetDataMgr().AddData(bootAreas, std::make_pair(val->TID, val->AID), val) )
						return false;
				}

				return true;
			}
			
			// 获取图片名称
			void GetImageFileName( std::vector<stdex::tString> &files );

			// 是否为图片格式
			bool IsImage(const CString &ext);
		
		
			// 获取执行日期
			void GetRunDate(const std::tr1::function<void(int, LPCTSTR)> &callback);

			// 获取选中日期
			void GetCheckDate(const std::tr1::function<void(int)> &callback);

			// 运行方式
			bool IsRunAtBoot(const data_helper::BootTaskTraits::ValueType &val);

			// 设置运行方式
			int SetRunType(bool isRunBoot);
			
		}
	}
}


#endif