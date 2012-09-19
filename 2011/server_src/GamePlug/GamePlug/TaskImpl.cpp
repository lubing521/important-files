#include "stdafx.h"
#include "TaskImpl.h"

#include "TaskItem.h"
#include "TaskDownloader.h"
#include "TaskCompletor.h"
#include "TaskIdcReport.h"

#include "TaskItemHelper.h"

#include "PLInfo.h"
#include "DataSvrHelper.h"
#include "LogHelper.h"

#include "../../../include/Utility/ObjectFactory.hpp"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include "../../../include/Utility/ScopeGuard.hpp"


namespace i8desk
{

	namespace 
	{
		template<typename EventT >
		inline bool IsStop(EventT &event)
		{
			if( event.WaitForEvent(0) )
				return true;
			return false;
		}


		TaskItemPtr ProductItemToTaskItem(const task_helper::ProductTaskItem &item, IRTDataSvr *rtSvr)
		{
			TaskItemPtr taskItem = MakeTaskItem(item.gid_, item.normalGID_, rtSvr);
			taskItem->SetIdcVersion(item.idcVer_);
			taskItem->SetSvrVersion(item.svrVer_);
			taskItem->SetIdcClick(item.idcClick_);
			taskItem->SetPush(item.pushTask_ == 0 ? false : true);
			taskItem->SetForceRepair(item.forceRepair_ == 0 ? false : true);
			taskItem->SetTaskType(item.taskType_);
			taskItem->SetTaskState(tsSelfQueue);
			taskItem->SetServerPath(item.svrPath_);
			taskItem->SetMscTmpDir(item.mscPath_);

			return taskItem;
		}

		task_helper::ProductTaskItem TaskItemToProductItem(const TaskItemPtr &item)
		{
			task_helper::ProductTaskItem tmp;
			tmp.gid_		= item->GetGid();
			tmp.idcVer_		= item->GetIdcVersion();
			tmp.svrVer_		= item->GetSvrVersion();
			tmp.idcClick_	= item->GetIdcClick();
			tmp.pushTask_	= item->IsPush();
			tmp.forceRepair_= item->IsForceRepair();
			tmp.taskType_	= item->GetTaskType();
			utility::Strcpy(tmp.svrPath_, item->GetServerPath());
			utility::Strcpy(tmp.mscPath_, item->GetMscTmpDir());

			return tmp;
		}
	}

	TaskImpl::TaskImpl(ISvrPlugMgr *plugMgr, IRTDataSvr *rtData, ISysOpt *sysOpt, const task_helper::ProductTaskItem &product,
		const ComplateCallback &complateCallback, const ErrorCallback &errorCallback)
		: plugMgr_(plugMgr) 
		, rtData_(rtData)
		, sysOpt_(sysOpt)
		, isRunning_(0)
		, complateCallback_(complateCallback)
		, errorCallback_(errorCallback)
	{
		assert(complateCallback_ != 0);
		assert(errorCallback_ != 0);

		taskItem_ = ProductItemToTaskItem(product, rtData_);
	}

	TaskImpl::~TaskImpl()
	{

	}


	void TaskImpl::Start()
	{
		// 获取PLRoot
		const std::wstring &plRoot = data_helper::GetPlRoot(PLInfo::GetInstance()); ////L"http://file.i8cn.com/Product/ProductInfo";
		if( plRoot.empty() ) 
			throw std::runtime_error("PLRoot为空，暂无法下载种子文件");

		if( !exit_.Create(NULL, TRUE) )
			throw std::runtime_error("TaskImpl 创建退出事件失败!");

		if( !stopExit_.Create() )
			throw std::runtime_error("TaskImpl 创建退出事件失败!");

		taskDownloader_.reset(utility::ObjectAlloc<TaskDownloader>(taskItem_, rtData_, sysOpt_, exit_, plRoot)
			, &utility::ObjectDeallocate<TaskDownloader>);
		taskComplator_.reset(utility::ObjectAlloc<TaskCompletor>(taskItem_, plugMgr_, rtData_, sysOpt_, exit_)
			, &utility::ObjectDeallocate<TaskCompletor>);
		taskReportor_.reset(utility::ObjectAlloc<TaskIdcReportor>(taskItem_, sysOpt_, exit_)
			, &utility::ObjectDeallocate<TaskIdcReportor>);

		//async::thread::SetThreadPoolMaxThread(WT_EXECUTEDEFAULT, 20);
		//if( !pool_.Call(std::tr1::bind(&TaskImpl::_Thread, shared_from_this())) )
		//	throw std::runtime_error("TaskImpl 线程启动失败!");

		GetThreadPool().Call(std::tr1::bind(&TaskImpl::_Thread, shared_from_this()));

		::InterlockedExchange(&isRunning_, 1);

		Log(LM_DEBUG, _T("已成功启动任务 %d..."), taskItem_->GetGid());
	}

	bool TaskImpl::IsStarted() const
	{
		return ::InterlockedExchangeAdd(&isRunning_, 0) == 1;
	}

	void TaskImpl::Stop()
	{
		if( exit_.IsOpen() )
			exit_.SetEvent();

		if( stopExit_.IsOpen() )
			stopExit_.SetEvent();
	}

	//void TaskImpl::Reset() 
	//{ 
	//	::InterlockedExchange(&isRunning_, 0);

	//	taskItem_->Reset();
	//	//if( stopExit_.IsOpen() )
	//	//	stopExit_.Close();

	//	//if( exit_.IsOpen() )
	//	//	exit_.Close();

	//	taskDownloader_.reset();
	//	taskComplator_.reset();
	//	taskReportor_.reset();
	//}



	void TaskImpl::SetDelete()
	{
		if( taskDownloader_ )
			taskDownloader_->SetDelete();

		Stop();
	}
	void TaskImpl::SetSusupend()
	{
		if( taskDownloader_ )
			taskDownloader_->SetSusupend();
	}
	void TaskImpl::SetResume()
	{
		if( taskDownloader_ )
			taskDownloader_->SetResume();
	}

	long TaskImpl::GetGid() const
	{ return taskItem_->GetGid(); }

	long TaskImpl::GetNormalGid() const
	{ return taskItem_->GetNormalGID(); }

	DWORD TaskImpl::GetIdcClick() const
	{ return taskItem_->GetIdcClick(); }

	DWORD TaskImpl::GetIdcVersion() const
	{ return taskItem_->GetIdcVersion(); }

	DWORD TaskImpl::GetSvrVersion() const
	{ return taskItem_->GetSvrVersion(); }

	TaskState TaskImpl::GetTaskState() const
	{ return taskItem_->GetTaskState(); }

	TaskType TaskImpl::GetTaskType() const
	{ return taskItem_->GetTaskType(); }

	const MSC_DOWNLOAD_PROGRESS &TaskImpl::GetPdp() const
	{ return taskItem_->GetPdp(); }

	ULONGLONG TaskImpl::GetUpdateSize() const
	{ return taskItem_->GetUpdateSize(); }

	bool TaskImpl::IsForceRepair() const
	{ return taskItem_->IsForceRepair(); }

	bool TaskImpl::IsPush() const
	{ return taskItem_->IsPush(); }

	DWORD TaskImpl::GetSenderIP() const
	{ return taskItem_->GetSenderIP(); }

	TaskNotify TaskImpl::GetTaskNotify() const
	{ return taskItem_->GetTaskNotify(); }

	size_t TaskImpl::GetErrorCode() const
	{ return taskItem_->GetErrorrCode(); }

	const stdex::tString &TaskImpl::GetSvrPath() const
	{ return taskItem_->GetServerPath(); }

	
	void TaskImpl::SetManualAdjust()
	{ taskItem_->SetManualAdjust(); }

	bool TaskImpl::GetManualAdjust() const
	{ return taskItem_->GetManualAdjust(); }

	task_helper::ProductTaskItem TaskImpl::GetProductTaskItem()
	{ return TaskItemToProductItem(taskItem_); }

	void TaskImpl::_Thread()
	{
		//async::ScopeGuard<void> scope(
		//	std::tr1::bind(&TaskImpl::Reset, this));
		//async::ScopeGuard<BOOL> scope1(
		//	std::tr1::bind(&async::thread::AutoEvent::SetEvent, std::tr1::ref(stopExit_)));

		
		try
		{
			if( IsStop(exit_) )
				return;

			// 下载
			Log(LM_DEBUG, _T("开始新任务下载 %d..."), taskItem_->GetGid());
			taskDownloader_->Run();

			if( IsStop(exit_) )
				return;

			// 善后
			if( taskDownloader_->IsDelete() )
			{
				taskComplator_->Clear();
				return;
			}
			else if( taskDownloader_->IsOK() )
			{
				taskComplator_->Run();
			}
			else
			{
				std::string info = "下载新任务出现异常 GID: " + stdex::ToString<std::string>(taskItem_->GetGid());
				throw std::runtime_error(info);
			}


			if( IsStop(exit_) )
				return;

			// 上报
			taskReportor_->Run();

			// 回调通知
			complateCallback_(std::tr1::cref(shared_from_this()));
			Log(LM_DEBUG, _T("结束任务下载 %d..."), taskItem_->GetGid());
		}
		catch(std::exception &e)
		{
			Log(LM_ERROR, CA2T(e.what()));

			errorCallback_(std::tr1::cref(shared_from_this()));
		}
	}
}