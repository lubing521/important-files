#include "stdafx.h"
#include "RealDataMgr.h"

#include "../NetworkMgr.h"


namespace i8desk
{
	namespace manager
	{


		RealDataMgr::RealDataMgr(NetworkDataMgr *network)
			: network_(network)
			, mask_(0)
			, monitorStatus_(new data_helper::MonitorStatusTraits::ElementType)
			, staticsStatus_(new data_helper::StaticsStatusTraits::ElementType)
		{
			network_->Register(std::tr1::bind(&RealDataMgr::_Parse, this));
		}
		RealDataMgr::~RealDataMgr()
		{}

		void RealDataMgr::_Parse()
		{
			try
  			{
				{
					AutoLock dataLock(mutex_);

					NetworkDataMgr::AutoLock lock(network_->MutexData());

					if( mask_ & MASK_PARSE_SERVER )
						data_helper::DataHandleTraits<data_helper::ServerStatusTraits>::GetAllData(network_, serverStatus_);
					if( mask_ & MASK_PARSE_DISK )
						data_helper::DataHandleTraits<data_helper::DiskStatusTraits>::GetAllData(network_, diskStatus_);
					if( mask_ & MASK_PARSE_VDISK )
						data_helper::DataHandleTraits<data_helper::VDiskClientTraits>::GetAllData(network_, vDiskStatus_);
					if( mask_ & MASK_PARSE_TASKSTATUS )
						data_helper::DataHandleTraits<data_helper::TaskStatusTraits>::GetAllData(network_, taskStatus_);
					if( mask_ & MASK_PARSE_UPDATEGAMESTATUS )
						data_helper::DataHandleTraits<data_helper::UpdateGameStatusTraits>::GetAllData(network_, updateGameStatus_);
					if( mask_ & MASK_PARSE_CLIENT )
						data_helper::DataHandleTraits<data_helper::ClientStatusTraits>::GetAllData(network_, clientStatus_);
					if( mask_ & MASK_PARSE_PLUGTOOL )
						data_helper::DataHandleTraits<data_helper::PlugToolStatusTraits>::GetAllData(network_, plugToolStatus_);
					if( mask_ & MASK_PARSE_MONITOR )
						data_helper::DataHandleTraits<data_helper::MonitorStatusTraits>::GetData(network_, monitorStatus_);
					if( mask_ & MASK_PARSE_STATICS )
						data_helper::DataHandleTraits<data_helper::StaticsStatusTraits>::GetData(network_, staticsStatus_);
					if( mask_ & MASK_PARSE_PUSHGAMESTATUS )
						data_helper::DataHandleTraits<data_helper::PushGameStatusTraits>::GetAllData(network_, pushGameStatus_);
					if( mask_ & MASK_PARSE_SYNCTASKSTATUS )
						data_helper::DataHandleTraits<data_helper::SyncTaskStatusTraits>::GetAllData(network_, syncTaskStatus_, syncTaskFilter_);

				}
				
		
				struct ForEach
				{
					void operator()(const UpdateCallback &callback) const
					{
						callback();
					}
				};

				if( mask_ != 0 )
					std::for_each(updateCallbacks_.begin(), updateCallbacks_.end(), ForEach());
			}
			catch(exception::Base &e)
			{
				if( errorCallback_ != 0 )
				{
					stdex::tString msg(e.What());
					errorCallback_(std::tr1::cref(msg));
				}
			}
		}

		void RealDataMgr::Register(const UpdateCallback &callback, uint64 mask)
		{
			mask_ |= mask;
			updateCallbacks_.push_back(callback);
		}
		void RealDataMgr::RegisterError(const ErrorCallback &errorCallback)
		{
			errorCallback_	= errorCallback;
		}

		void RealDataMgr::UnRegister(uint64 mask)
		{
			mask_ &= ~mask;
		}
	}
}