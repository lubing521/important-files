#ifndef __MONITOR_BUSINESS_HPP
#define __MONITOR_BUSINESS_HPP

#include "../LogRecord.h"
#include "../Data/DataHelper.h"

#include <list>
#include <tuple>

namespace i8desk
{
	namespace business
	{

		
		namespace monitor
		{

			// 插入日志消息
			void PushLog(const LogRecordPtr &log);


			// 改变Debug 模式
			void ChangeDebugMode();

			// 获取出错消息
			void GetLogRecord(size_t index, i8desk::LogRecord &log);

			// 获取消息总个数
			size_t GetLogCount();


			// 插入客户机异常消息
			void PushExcept(const std::tr1::tuple<data_helper::ClientTraits::ValueType, DWORD> &exceptInfo);

			// 获取异常信息
			std::tr1::tuple<stdex::tString, stdex::tString, stdex::tString, stdex::tString > GetExceptInfo(size_t index);

			// 获取异常消息总个数
			size_t GetExceptCount();

			// 是否处于异常
			std::pair<bool, UINT> IsException(const stdex::tString &name, const stdex::tString &ip);

			// 清除异常
			void ClearException();

		}
	}
}




#endif