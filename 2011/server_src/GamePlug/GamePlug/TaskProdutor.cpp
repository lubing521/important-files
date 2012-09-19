#include "stdafx.h"
#include "TaskProductor.h"

#include "LogHelper.h"
#include "DataSvrHelper.h"

#include "ForceGameZip.h"

namespace i8desk
{

	namespace productor_condition
	{
		// 强推条件的过滤器
		struct ForceFilter : public IGameFilter 
		{
			bool bypass(const db::tGame *const d) 
			{
				// 中心版本与本地版本相同 
				return d->IdcVer == d->SvrVer || d->Force != 1;
			}
		};


		// 按下载优先级和中心点击率排序
		struct Sorter : public IGameSorter 
		{
			bool sort(const db::tGame *const d1, const db::tGame *const d2) 
			{
				// 如果下载优先级相同则比较中心点击率
				if( d1->Priority == d2->Priority ) 
				{
					return d1->IdcClick > d2->IdcClick;
				}

				return d1->Priority > d2->Priority;
			}
		};



		// 自动更新游戏的过滤器
		struct AutoUpdateFilter : public IGameFilter 
		{
			bool bypass(const db::tGame *const d) 
			{
				// 如果中心版本与本地版本相同 或
				// 不是自动更新 或
				// 不是本地游戏
				return d->IdcVer == d->SvrVer
					|| d->AutoUpt != 1 
					|| d->Status != 1;   
			}
		};
	}



	TaskProductor::TaskProductor(IGameTable *gameTable, ISysOpt *sysOpt, UnZipGame *unZipGame, const Callback &callback)
		: gameTable_(gameTable)
		, sysOpt_(sysOpt)
		, unZipGame_(unZipGame)
		, callback_(callback)
	{
		assert(gameTable);
	}

	TaskProductor::~TaskProductor()
	{}


	void TaskProductor::Start()
	{
		wait_.Create();
		exit_.Create();
		
		thread_.RegisterFunc(std::tr1::bind(&TaskProductor::_Thread, this));
		thread_.Start();
	}

	void TaskProductor::Stop()
	{
		exit_.SetEvent();
		thread_.Stop();
		
		Log(LM_DEBUG, _T("TaskProductor 退出成功"));
	}

	void TaskProductor::ForceCheck()
	{
		wait_.SetEvent();
	}

	DWORD TaskProductor::ForceUpdateGame(long gid, DWORD ip, const db::tGame &game)
	{
		callback_(gid, 0, game.Priority, game.IdcClick, game.IdcVer, 1, 0, ip, tForceTask);

		Log(LM_DEBUG, _T("同步强制更新游戏.GID = %d   IdcVer = %d  SvrVer = %d\n"),
			game.GID, game.IdcVer, game.SvrVer);

		return 0;
	}

	void TaskProductor::_HasForceZipTask()
	{
		// 得到强推压缩包的任务
		IGameRecordset *pRecordset = 0;
		gameTable_->Select(&pRecordset, 0, 0);

		for(uint32 i = 0; i != pRecordset->GetCount(); ++i) 
		{
			db::tGame *d = pRecordset->GetData(i);
			// PL配置为强推且游戏不在本地
			if( unZipGame_->IsForceZip(d->GID) )
			{
				long forceGID = unZipGame_->GetForceGID(d->GID);
				if( forceGID != 0 )
				{
					Log(LM_DEBUG, _T("强推压缩包.GID = %d	IdcVer = %d  SvrVer = %d\n"), d->GID, d->IdcVer, d->SvrVer);
					callback_(forceGID, d->GID, d->Priority, d->IdcClick, d->IdcVer, 0, 1, 0, tForceTask);
				}
			}
		}

		pRecordset->Release();
	}


	void TaskProductor::_HasForceTask()
	{
		//一.得到强推的任务
		productor_condition::ForceFilter filter;
		productor_condition::Sorter sorter;

		IGameRecordset *pRecordset = 0;
		gameTable_->Select(&pRecordset, &filter, &sorter);


		for(uint32 i = 0; i < pRecordset->GetCount(); ++i) 
		{
			db::tGame *d = pRecordset->GetData(i);

			//if( /* && !unZipGame_->IsForceZip(d->GID) */) 
			{
				Log(LM_DEBUG, _T("强推.GID = %d   IdcVer = %d  SvrVer = %d\n"), d->GID, d->IdcVer, d->SvrVer);

				callback_(d->GID, 0, d->Priority, d->IdcClick, d->IdcVer, 0, 1, 0, tForceTask);
			}
		}

		pRecordset->Release();
	}

	void TaskProductor::_HasTimeUpdateTask()
	{
		// 得到时段更新任务
		bool bTimeCondition = data_helper::CheckTimeCondition(sysOpt_);
		if( !bTimeCondition )
		{
			Log(LM_INFO, _T("当前时段不在自动更新时段内"));
			return;
		}

		productor_condition::AutoUpdateFilter filter;
		productor_condition::Sorter sorter;

		IGameRecordset *pRecordset = 0;
		gameTable_->Select(&pRecordset, &filter, &sorter);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) 
		{
			db::tGame *d = pRecordset->GetData(i);

			Log(LM_DEBUG, _T("自动更新.GID = %d   IdcVer = %d  SvrVer = %d\n"), d->GID, d->IdcVer, d->SvrVer);

			/*if( !unZipGame_->IsForceZip(d->GID) )
			{*/
				callback_(d->GID, 0, d->Priority, d->IdcClick, d->IdcVer, 0, 1, 0, d->Repair == 1 ? tForceTask : tTimeTask);
			//}
		}

		pRecordset->Release();

	}

	DWORD TaskProductor::_Thread()
	{
		HANDLE handle[] = { wait_, exit_ };

		while( !thread_.IsAborted() )
		{
			DWORD ret = ::WaitForMultipleObjects(_countof(handle), handle, FALSE, WAIT_TIME);
		
			if( ret == WAIT_OBJECT_0  || ret == WAIT_TIMEOUT )
			{
				// 检测强推压缩包任务
				//_HasForceZipTask();

				if( exit_.IsSignalled() )
					break;

				// 监测强推任务
				_HasForceTask();

				if( exit_.IsSignalled() )
					break;

				// 监测时段更新任务
				_HasTimeUpdateTask();
			}
			else if( ret == WAIT_OBJECT_0 + 1 )
			{
				break;
			}
			else
			{
				assert(0);
			}
		}

		return 0;
	}

}