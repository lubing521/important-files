#ifndef __I8DESK_GAME_DISK_MONITOR_HPP
#define __I8DESK_GAME_DISK_MONITOR_HPP


#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/MultiThread/Thread.hpp"

namespace i8desk
{
	// ----------------------------------------------
	// class GameDiskMonitor

	// 游戏磁盘监控
	class GameDiskMonitor
		: public IGameEventHandler
	{
		typedef async::thread::AutoCriticalSection	SCLock;
		typedef async::thread::AutoLock<SCLock>		AutoLock;

	public:
		typedef std::set<TCHAR> DiskPartionType;

	private:
		IRTDataSvr *rtDataSvr_						;
		ISysOpt *sysOpt_							;	// 选项表

		ulonglong mscSize_							;	// 三层下载临时目录

		SCLock mutex_								;	
		async::thread::AutoEvent update_			;		
		async::thread::AutoEvent exit_				;
		async::thread::ThreadImplEx thread_			;

	

	public:
		explicit GameDiskMonitor(IRTDataSvr *pRTDataSvr, ISysOpt *sysOpt);
		~GameDiskMonitor();

	private:
		GameDiskMonitor(const GameDiskMonitor &);
		GameDiskMonitor &operator=(const GameDiskMonitor &);

	public:
		// IGameEventHandler
		virtual uint32 GetEvents();
		virtual void HandleEvent(uint32 optype, uint32 gid, uint64 mask);

		// 对外接口
	public:
		void Start();		// 失败抛出异常
		void Stop();

		// 更新读取速度
		void Update();

		// 更新状态
		void UpdateStatus();

		// 获取三层下载临时目录大小
		ulonglong MscTmpSize() const;

	private:
		bool _Init();
		void _Uninit();

		// 获取硬盘分区
		void _GetPartitions(DiskPartionType &partion);
		// 插入分区符号
		void _InsertPartition(TCHAR partition);

		DWORD _Thread();
	};
}




#endif