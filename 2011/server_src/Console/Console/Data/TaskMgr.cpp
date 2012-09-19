#include "stdafx.h"
#include "TaskMgr.h"


#include "../NetworkMgr.h"
#include "DataHelper.h"

namespace i8desk
{

	namespace manager
	{

		TaskMgr::TaskMgr(NetworkDataMgr *network)
			: network_(network)
			, errorCallback_(0)
		{}

		TaskMgr::~TaskMgr()
		{
			
		}

		void TaskMgr::RegisterError(const ErrorCallback &errorCallback)
		{
			errorCallback_ = errorCallback;
		}


		bool TaskMgr::RefreshPL()
		{
			char outBuf[1024] = {0};
			CPkgHelper out(outBuf, CMD_CON_NOTIFY_REFRESH_PL, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool TaskMgr::ServerMedical()
		{
			char outBuf[1024] = {0};
			CPkgHelper out(outBuf, CMD_NOTIFY_SERVER_MEDICAL, OBJECT_CONSOLE);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool TaskMgr::TaskOperate(DWORD gid, TaskOperation op, DWORD priority /* = 0 */, DWORD IdcVer /* = 0 */, bool bIsFore /* = false */, DWORD IdcClick /* = 0 */)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_TASK_OPERATION, OBJECT_CONSOLE);
			out << static_cast<DWORD>(op) << gid << priority << IdcVer << (bIsFore ? 1L : 0L) << IdcClick;
			
			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool TaskMgr::DeleteFileOperate( DWORD gid ,DWORD flag )
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_CON_NOTIFY_DELETEFILE, OBJECT_CONSOLE);
			out << gid << flag;

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}

		bool TaskMgr::PutTopOrBottom(DWORD gid, bool isTop)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_TASK_OPERATION, OBJECT_CONSOLE);
			out << static_cast<DWORD>(i8desk::toAdjust) << gid << (isTop ? 1L : 0L);

			AutoLock lock(network_->MutexData());
			return exception::ExceptHandle(std::tr1::bind(&data_helper::ExecuteMsg, 
				network_, out.GetBuffer(), out.GetLength()), errorCallback_);
		}
	}
	
}