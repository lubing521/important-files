#ifndef __TASK_POLICY_HPP
#define __TASK_POLICY_HPP



namespace i8desk
{
	namespace task_policy
	{

		// ------------------------------------------
		// class UpdateAddTask

		template < typename ImplT >
		class UpdateAddTask
		{
			typedef UpdateAddTask<ImplT> SelfType;

		public:
			template<typename ProductTaskT, typename RTDataSvrT>
			void Run(ProductTaskT &item, RTDataSvrT *rtDataSvr)
			{
				// 判断是否更新时为强制更新
				i8desk::db::tGame game;
				if( rtDataSvr->GetGameTable()->GetData(item.gid_, &game) == 0 )
				{
					if( game.Repair == 1 )
						item.forceRepair_ = 1;
				}
			}
		};

		// 添加新任务状态
		enum CheckTaskType
		{
			NEW_TASK, UPDATE_TASK, INVALID_TASK, LATEST_VERSION, EXSIT_TASK
		};


		// ------------------------------------------
		// class CheckAddTask

		// 添加新任务，进行检测
		template<typename ImplT>
		class CheckAddTask
		{
			typedef CheckAddTask<ImplT> SelfType;

		public:
			template<typename QueueT, typename ProductTaskT>
			CheckTaskType Run(const QueueT &queue, const ProductTaskT &item)
			{
				ImplT *pThis = static_cast<ImplT *>(this);
				if( item.gid_ < MIN_IDC_GID )
					return INVALID_TASK;

				// 如果版本号相同且不是强修
				if( item.svrVer_ == item.idcVer_ && item.forceRepair_ != 1 )
					return LATEST_VERSION;

				// 比较队列是否为空
				if( queue.Empty() )
					return NEW_TASK;
				
				// 查找队列
				typename QueueT::const_iterator iter = queue.FindIf(task_helper::TaskFind(item.gid_));
				if( iter != queue.End() )
				{
					// 如果是强修或者版本号不匹配
					if( item.forceRepair_ || (item.idcVer_ != (*iter)->GetIdcVersion()) )
						return UPDATE_TASK;
					else
						return EXSIT_TASK;
				}
				else
					return NEW_TASK;
			}
		};



		// ------------------------------------------
		// class CheckDownloadTask

		// 检测任务是否需要被下载

		template<typename ImplT>
		class CheckDownloadTask
		{
			typedef CheckAddTask<ImplT> SelfType;

		public:
			template<typename QueueT, typename TaskTypeT>
			bool Run(const QueueT &queue, const size_t nMax, TaskTypeT &task)
			{	
				ImplT *pThis = static_cast<ImplT *>(this);

				size_t pos = 0;
				for(typename QueueT::const_iterator iter = queue.Begin(); 
					iter != queue.End(); ++iter, ++pos)
				{
					// 如果超过
					if( pos < nMax )
					{
						if( !(*iter)->IsStarted() )
						{
							task = *iter;
							return true;
						}
					}
					else
						return false;
				}

				return false;
			}
		};
	}
}

#endif