#ifndef __CONSOLE_TASK_MANAGER_HPP
#define __CONSOLE_TASK_MANAGER_HPP

#include "../../../../include/TaskInfo.h"
#include "DataHelper.h"
#include <memory>

namespace i8desk
{
	// forward declare
	class NetworkDataMgr;


	namespace manager
	{
		// -----------------------------------------
		// class TaskMgr

		class TaskMgr
		{
			typedef std::tr1::function<void(const stdex::tString &)> ErrorCallback;
			typedef NetworkDataMgr::AutoLock						AutoLock;

		public:
			NetworkDataMgr *network_;
			ErrorCallback	errorCallback_;

		public:
			explicit TaskMgr(NetworkDataMgr *network);
			~TaskMgr();

		private:
			TaskMgr(const TaskMgr &);
			TaskMgr &operator=(const TaskMgr &);

		public:
			void RegisterError(const ErrorCallback &errorCallback);

		public:
			bool TaskOperate(DWORD gid, TaskOperation op, DWORD DownPriority = 0, DWORD IdcVer = 0, 
				bool bIsFore = false, DWORD IdcClick = 0);
			bool DeleteFileOperate( DWORD gid ,DWORD flag);
			bool PutTopOrBottom(DWORD gid, bool isTop);

			bool RefreshPL();
			bool ServerMedical();
		};
	}
}


#endif