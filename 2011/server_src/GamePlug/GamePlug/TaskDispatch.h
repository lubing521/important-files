#ifndef __I8DESK_TASK_DISPATCHER_HPP
#define __I8DESK_TASK_DISPATCHER_HPP

#include "../../../include/MultiThread/Thread.hpp"


#include "SchedulerQueue.h"
#include "TaskImpl.h"
#include "TaskPolicy.h"

#include "TaskSerialize.h"
#include "TaskDispatchImpl.h"

#include "DownloadEngine.h"

namespace i8desk
{

	
	// -------------------------------------------------
	// class TaskDispatcher

	// 任务调度

	class TaskDispatcher
		: private task_policy::UpdateAddTask< TaskDispatcher >
		, private task_policy::CheckAddTask< TaskDispatcher >
		, private task_policy::CheckDownloadTask< TaskDispatcher >
		, private task_helper::TaskSerialize< TaskDispatcher, task_helper::ProductTaskItem >
		, public task_helper::TaskDispatchImpl< TaskDispatcher, TaskImplPtr >
	{
		// 最大下载数与最大等待数
		static const size_t MaxWaitting = 4096; 
		static const size_t MaxDownload = 3;

	public:
		typedef task_helper::ProductTaskItem							ProductTaskType;		// 内部任务使用类型
		typedef TaskImplPtr												TaskType;				// 任务类型

		typedef TaskDispatchImpl<TaskDispatcher, TaskType>				TaskDispatchImplType;	// 任务调度实现类型

		typedef task_policy::UpdateAddTask<TaskDispatcher>				UpdateAddTaskPolicy;
		typedef task_policy::CheckAddTask<TaskDispatcher>				CheckAddTaskPolicy;		// 任务添加策略
		typedef task_policy::CheckDownloadTask<TaskDispatcher>			CheckDownloadTaskPolicy;// 任务下载策略
		typedef task_helper::TaskSerialize<TaskDispatcher, ProductTaskType>	TaskSerializeType;	// 序列化类型

		typedef SchedulerQueue<TaskType, MaxDownload>					TaskQueue;				// 下载队列

		typedef DownloadEngine::EventCallback							EventCallback;		// 事件回调

	private:
		ISvrPlugMgr	*plugMgr_;
		IRTDataSvr	*rtData_;
		ISysOpt		*sysOpt_;
		EventCallback evenCallback_;

	private:
		async::thread::ThreadImplEx thread_;
		TaskQueue taskQueue_;

	public:
		TaskDispatcher(ISvrPlugMgr *plugMgr, IRTDataSvr *rtData, ISysOpt *sysOpt);
	
	public:
		void Start();
		void Stop();

		void Register(const EventCallback &callback);
		
	public:
		/*
			1. 检测是否为新任务
			2. 如果在下载队列中，则停止下载并去除。如果在等待队列中，则去除
			3. 增加新任务到等待队列
		*/
		void AddTask(const task_helper::ProductTaskItem &item);

		// 删除任务
		void DeleteTask(long gid);

		// 挂起下载中任务
		void SuspendTask(long gid);

		// 恢复下载中任务
		void ResumeTask(long gid);
	
		// 调整优先级
		void AdjustTask(long gid, int priority);

		// 置顶 
		void PutTop(long gid);

		// 置底
		void PutBottom(long gid);

		// 保存
		void Save();

		//
		template<typename ContainerT>
		void GetTasksInfo(ContainerT &container)
		{
			// 填充任务队列数据
			using namespace std::tr1::placeholders;
			typedef typename TaskQueue::value_type value_type;
			taskQueue_.ForEach(std::tr1::bind(&task_helper::PushTaskInfo<value_type, ContainerT>,
				_1, &container));
		}


	private:
		// 产生任务;
		void _TaskProductorImpl(const task_helper::ProductTaskItem &item);

		// 添加任务实现 
		void _AddTaskImpl(const TaskType &task);

		// 下载任务实现
		void _DownloadTaskImpl(const TaskType &task);

		// 完成任务实现
		void _ComplateTaskImpl(const TaskType &task);
		
		// 错误任务实现
		void _ErrorTaskImpl(const TaskType &task);

		// 开始任务实现
		void _StartImpl();

		// 结束任务实现
		void _QuitImpl();

	private:
		DWORD _Thread();
	};
}



namespace utility
{
	typedef i8desk::TaskDispatcher::TaskDispatchImplType::OperateItem			TaskOperateItem;

	template<>
	struct ObjectFactory<TaskOperateItem>
	{
		typedef async::memory::FixedMemoryPool<true, sizeof(TaskOperateItem)>	PoolType;
		typedef ObjectPool<PoolType>											ObjectPoolType;
	};
}



#endif