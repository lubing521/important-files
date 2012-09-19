#ifndef __BUSINESS_SYNC_TASK_HPP
#define __BUSINESS_SYNC_TASK_HPP

#include "../Data/DataHelper.h"
#include <set>
namespace i8desk
{
	namespace business
	{
		// -----------------------------
		// SyncTask

		namespace SyncTask
		{
			// 添加同步任务
			bool AddSyncTask(const data_helper::SyncTaskTraits::ValueType &syncTask, const data_helper::GameTraits::VectorType &games);
			
			// 修改同步任务
			bool ModifySyncTask(const data_helper::SyncTaskTraits::ValueType &syncTask);

			// 删除同步任务
			bool DelSyncTask(const stdex::tString &SID);
			
			// 添加同步游戏
			bool AddSyncGame(const stdex::tString &SID, long gid);

			// 删除同步游戏
			bool DelSyncGame(const data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus);

			// 修改同步游戏
			bool ModifySyncGame(const data_helper::SyncTaskTraits::ValueType &syncTask,	data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus);

			// 保存同步游戏到数据库
			bool SaveSyncGame(data_helper::SyncTaskStatusTraits::VectorType &syncGamestatus);

			// 获取所有类型
			void GetAllTypes(const std::tr1::function<void(int, const stdex::tString &)> &callback);
			
			// 得到所有路径
			void GetAllGamesPath( std::set<stdex::tString> &pathes);

			// 检测同步IP，不能与服务器集群有相同IP
			bool HasSameIP(const data_helper::SyncTaskTraits::ValueType &syncTask);

			// 检测同步任务名称，不能相同
			bool HasSameTaskName( const data_helper::SyncTaskTraits::ValueType &syncTask);

			// 检测是否有同步到本机相同位置的设置
			bool HasSameNodeDir( const data_helper::SyncTaskTraits::ValueType &syncTask);

			// 检测是否有同步源路径与目标路径相同
			bool HasSamePath( const data_helper::SyncTaskTraits::ValueType &syncTask, const data_helper::GameTraits::VectorType &games);

			// 获取同步任务当前速度
			uint64 GetSyncTaskSpeed();


			// 匹配同步游戏
			void MatchSyncTask(const data_helper::SyncTaskTraits::VectorType &syncTasks, data_helper::GameTraits::VectorType &syncGames);
			
			// 计算同步游戏大小
			void CumulateSize(const data_helper::GameTraits::VectorType &curGames, uint64 &gamesSize);
		
			// 获取任务名称
			void GetTaskName( std::set<std::pair<stdex::tString, stdex::tString>> &tasknames);
			
			// 查找满足条件的同步任务
            void SetSyncTaskStatus( const stdex::tString &SID, i8desk::SyncTaskState state);
		
			// 寻找不同SID的同步任务
			void FilterByNameID(const data_helper::SyncTaskTraits::MapType &syncTasks, data_helper::SyncTaskTraits::MapType &diffSyncTasks);
		
			// 获取相同NameID的同步任务源路径
			void GetSameNameIDSrcPaths(const stdex::tString &nameID, std::vector<stdex::tString> &srcPaths);

			// 获取相同NameID的同步任务目标路径
			void GetSameNameIDDstPaths(const stdex::tString &nameID, std::vector<stdex::tString> &dstPaths);

			// 删除同步任务
			bool DeleteSyncGame(data_helper::SyncTaskTraits::ValueType &taskgame);

			// 通知同步
			bool CanNotifyGame(data_helper::GameTraits::VectorType &games, const std::tr1::function<void(const stdex::tString &)> &callback);
		
		
			// 获取所有同步任务
			void GetSyncTasks(data_helper::SyncTaskTraits::VectorType &syncTasks);

			// 获取所有同步任务状态
			void GetSyncTaskStatus(data_helper::SyncTaskStatusTraits::VectorType &synctaskStatus);
		
			// 获取同步状态描述
			stdex::tString GetStateDesc(const data_helper::SyncTaskStatusTraits::ValueType &val);

			// 获取同步任务描述
			stdex::tString GetTaskNameBySID(const stdex::tString &SID);

			// 获取同步任务描述
			stdex::tString GetTaskDesc(const data_helper::SyncTaskStatusTraits::ValueType &val);

			// 获取相同任务的同步状态
			void TaskFilter(data_helper::SyncTaskStatusTraits::VectorType &taskstatus, const stdex::tString &SID);

			// 查找同步游戏
			void FindSync(LPCTSTR destText, const data_helper::SyncTaskStatusTraits::VectorType &tasks, data_helper::SyncTaskStatusTraits::VectorType &values);

			// 查找同步游戏
			void FindSync(LPCTSTR destText, const data_helper::SyncGameTraits::VectorType &tasks, data_helper::SyncGameTraits::VectorType &values);

			// 设置同步状态
			void SetSyncStatus(data_helper::SyncTaskStatusTraits::VectorType &taskstatus, const stdex::tString &SID);
			
			// 获取相同SID的同步游戏
			void GetSyncGamesBySID(const stdex::tString &SID,  data_helper::GameTraits::VectorType &games);

			// 是否已暂停
			bool IsPaused(const stdex::tString &SID);

			// 是否已暂停
			bool IsRunning(const stdex::tString &SID);
		}
	}
}

#endif
