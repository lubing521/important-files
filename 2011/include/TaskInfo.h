#ifndef __TASK_INFO_HPP
#define __TASK_INFO_HPP

#include <vector>

namespace i8desk
{
	
	//任务类型
	enum TaskType
	{
		tUserTask,
		tForceTask,
		tSmartForceTask	,	/*人性化强推*/
		tTimeTask,			/*时段任务*/
		tPlugToolTask,		//插件任务
	};

	enum TaskState 
	{
		tsChecking,			//正在对比
		tsQueue,			//等待下载
		tsDownloading,		//正在下载
		tsSuspend,			//暂停下载
		tsComplete,			//下载完成
		tsSelfQueue,		//正在排队
		tsWaitSeed,			//等待索引
		tsHashmap,			//校验索引
		tsCopyfile,			//复制文件
		tsDeleteOldFile,	//清理文件
		tsCommit,			//合并文件
	};


	// 任务操作
	enum TaskOperation
	{
		toAdd,		// 增加任务
		toDel,		// 删除任务
		toGetState,	// 获取任务状态
		toSync,		// 通知同步工具同步游戏
		toSuspend,	// 挂起任务
		toRestart,	// 恢复任务
		toAdjust,	// 调整等待任务优先级
		toDelGame,	// 通知同步工具删除游戏目录
	};


	// 任务操作
	enum PlugCenterOperation
	{
		todownload  =1,	// 下载
		tostart,	// 启用
		tostop,		// 禁用	
		tooption,	// 选项
	};


	// 对外数据接口

	struct TaskInfo
	{
		long					gid_;			// gid
		int						state_;			// task state
		int						type_;			// task type
		unsigned short			connections_;	// 连接数
		unsigned long			downloadState_;	// 下载中状态
		unsigned long			transferRate_;	// 传输速度
		unsigned long			timeLeft_;		// 剩余时间
		unsigned long			timeElapsed_;	// 消耗时间
		unsigned long			progress_;		// 进度
		ULONGLONG				bytesTotal_;	// 总下载量
		ULONGLONG				updateSize_;	// 更新量
		ULONGLONG				bytesTransferred_;// 传输量

		TaskInfo()
		{
			::memset(this, 0, sizeof(*this));
		}
	};



	typedef std::vector<TaskInfo> TasksInfo;
}


#endif