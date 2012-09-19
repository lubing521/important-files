#ifndef __REAL_DATA_MANAGER_HPP
#define __REAL_DATA_MANAGER_HPP


#include "DataHelper.h"



enum ParseMask 
{ 
	MASK_PARSE_SERVER			= 1 << 1, 
	MASK_PARSE_DISK				= 1 << 2,
	MASK_PARSE_VDISK			= 1 << 3,
	MASK_PARSE_TASKSTATUS		= 1 << 4,
	MASK_PARSE_UPDATEGAMESTATUS = 1 << 5,
	MASK_PARSE_PLUGTOOL			= 1 << 6,
	MASK_PARSE_CLIENT			= 1 << 7,
	MASK_PARSE_MONITOR			= 1 << 8,
	MASK_PARSE_STATICS			= 1 << 9,
	MASK_PARSE_PUSHGAMESTATUS	= 1 << 10,
	MASK_PARSE_SYNCTASKSTATUS	= 1 << 11,

};


namespace i8desk
{

	// forward declare
	class NetworkDataMgr;

	namespace manager
	{	

		// ------------------------------
		// class RealDataMgr
		
		class RealDataMgr
		{
			typedef std::tr1::function<void()>							UpdateCallback;
			typedef std::tr1::function<void(const stdex::tString &)>	ErrorCallback;

			typedef std::list<UpdateCallback>							UpdateCallbacks;

			typedef async::thread::AutoCriticalSection					Mutex;
			typedef async::thread::AutoLock<Mutex>						AutoLock;

		private:
			NetworkDataMgr								*network_;
			UpdateCallbacks								updateCallbacks_;
			ErrorCallback								errorCallback_;

			Mutex										mutex_;
			uint64										mask_;
			stdex::tString								syncTaskFilter_;
			
			data_helper::ServerStatusTraits::MapType	serverStatus_;
			data_helper::ClientStatusTraits::MapType	clientStatus_;
			data_helper::DiskStatusTraits::MapType		diskStatus_;
			data_helper::VDiskClientTraits::MapType		vDiskStatus_;
			data_helper::TaskStatusTraits::VectorType	taskStatus_;
			data_helper::UpdateGameStatusTraits::MapType updateGameStatus_;			
			data_helper::PushGameStatusTraits::MapType	pushGameStatus_;		
			data_helper::PlugToolStatusTraits::VectorType plugToolStatus_;
			data_helper::MonitorStatusTraits::ValueType	monitorStatus_;
			data_helper::StaticsStatusTraits::ValueType	staticsStatus_;
			data_helper::SyncTaskStatusTraits::MapType	syncTaskStatus_;		


		public:
			explicit RealDataMgr(NetworkDataMgr *network);
			~RealDataMgr();

		private:
			RealDataMgr(const RealDataMgr &);
			RealDataMgr &operator=(const RealDataMgr &);
		
		public:
			data_helper::ServerStatusTraits::MapType GetServerStatus()
			{ AutoLock lock(mutex_); return serverStatus_; }

			data_helper::ClientStatusTraits::MapType GetClientStatus()
			{ AutoLock lock(mutex_); return clientStatus_; }

			data_helper::DiskStatusTraits::MapType GetDiskStatus()
			{ AutoLock lock(mutex_); return diskStatus_; }

			data_helper::VDiskClientTraits::MapType GetVDiskClientStatus()
			{ AutoLock lock(mutex_); return vDiskStatus_; }
			
			data_helper::TaskStatusTraits::VectorType GetTaskStatus()
			{ AutoLock lock(mutex_); return taskStatus_; }

			data_helper::UpdateGameStatusTraits::MapType GetUpdateGameStatus()
			{ AutoLock lock(mutex_); return updateGameStatus_; }

			data_helper::PushGameStatusTraits::MapType GetPushGameStatus()
			{ AutoLock lock(mutex_); return pushGameStatus_; }

			data_helper::PlugToolStatusTraits::VectorType GetPlugToolStatus()
			{ AutoLock lock(mutex_); return plugToolStatus_; }

			data_helper::MonitorStatusTraits::ValueType GetMonitorStatus()
			{ AutoLock lock(mutex_); return monitorStatus_; }

			data_helper::StaticsStatusTraits::ValueType GetStaticsStatus()
			{ AutoLock lock(mutex_); return staticsStatus_; }

			data_helper::SyncTaskStatusTraits::MapType GetSyncTaskStatus(const stdex::tString &syncTaskFilter)
			{ AutoLock lock(mutex_); syncTaskFilter_ = syncTaskFilter; return syncTaskStatus_; }

		public:
			void Register(const UpdateCallback &callback, uint64 mask);
			void RegisterError(const ErrorCallback &errorCallback);
			void UnRegister(uint64 mask);

		private:
			void _Parse();
		};
		
	}

}




#endif