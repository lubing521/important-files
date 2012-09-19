#include "stdafx.h"
#include "DownloadEngine.h"

#include "LogHelper.h"
#include "TaskDispatch.h"
#include "TaskItemHelper.h"
#include "Msc.h"


namespace i8desk
{

	
	DownloadEngine::DownloadEngine(ISvrPlugMgr *plugMgr, IRTDataSvr *rtDataSvr, ISysOpt *sysOpt)
		: plugMgr_(plugMgr)
		, rtDataSvr_(rtDataSvr)
		, sysOpt_(sysOpt)
	{
		assert(rtDataSvr_);
		assert(sysOpt_);
	}

	DownloadEngine::~DownloadEngine()
	{

	}

	void DownloadEngine::Start()
	{
		taskDispatcher_.reset(new TaskDispatcher(plugMgr_, rtDataSvr_, sysOpt_));
		taskDispatcher_->Start();
	}

	void DownloadEngine::Stop()
	{
		taskDispatcher_->Stop();
		taskDispatcher_.reset();
	}

	void DownloadEngine::Save()
	{
		taskDispatcher_->Save();
	}

	void DownloadEngine::Register(const EventCallback &callback)
	{
		taskDispatcher_->Register(callback);
	}


	void DownloadEngine::AddTask(
						long	gid					,	// gid
						long	normalGID			, 
						int		downloadPriority	,	// 下载优先级
						DWORD	idcClick			,	// 中心点击率
						DWORD	idcVer				,	// 中心版本号
						DWORD	forceRepair			,	// 强制修复
						DWORD	pushTask			,	// 强推
						DWORD	sender				,	// 强修发送者
						TaskType taskType)				// 任务类型
	{
		db::tGame game;
		if( rtDataSvr_->GetGameTable()->GetData(gid, &game) != 0 )
		{
			Log(LM_ERROR, _T("添加任务时数据库不存在此游戏 gid = %d"), gid);
			return;
		}

		task_helper::ProductTaskItem item;
		item.gid_				= gid;
		item.forceRepair_		= forceRepair;
		item.idcClick_			= idcClick;
		item.idcVer_			= idcVer;
		item.svrVer_			= game.SvrVer;
		item.pushTask_			= pushTask;
		item.sender_			= sender;
		item.taskType_			= taskType;

		// 是否为强制更新
		taskDispatcher_->AddTask(item);
	}

	void DownloadEngine::DeleteTask(long gid)
	{
		taskDispatcher_->DeleteTask(gid);
	}
	void DownloadEngine::SuspendTask(long gid)
	{
		taskDispatcher_->SuspendTask(gid);
	}
	void DownloadEngine::ResumeTask(long gid)
	{
		taskDispatcher_->ResumeTask(gid);
	}

	void DownloadEngine::AdjustPriorityTask(long gid, int priority)
	{
		taskDispatcher_->AdjustTask(gid, priority);
	}

	void DownloadEngine::PutTop(long gid)
	{
		taskDispatcher_->PutTop(gid);
	}

	void DownloadEngine::PutBottom(long gid)
	{
		taskDispatcher_->PutBottom(gid);
	}

	void DownloadEngine::GetTasksInfo(TasksInfo &container) const
	{
		container.clear();
		taskDispatcher_->GetTasksInfo(container);
	}

	std::pair<long, long> DownloadEngine::GetMscSpeed() const
	{
		long upRate		= msc::MscSpeed::GetInstance().upRate_;
		long downRate	= msc::MscSpeed::GetInstance().downRate_;

		return std::make_pair(upRate, downRate);
	}

}