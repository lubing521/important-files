#include "stdafx.h"
#include "LocalGameChecker.hpp"


namespace i8desk
{
	TaskProductor::TaskProductor(IRTDataSvr *rt, CI8ExInfoMgrS* gameExInfoMgr, const GameCallback &callback, const ClientMenuCallback &clientCallback)
		: rt_(rt)
        , clExInfoMgr_(gameExInfoMgr)
		, gameCallback_(callback)
		, clientCallback_(clientCallback)
	{}


	void TaskProductor::Start()
	{
		exit_.Create(0, TRUE, FALSE);
		thread_.RegisterFunc(std::tr1::bind(&TaskProductor::_Thread, this));
		thread_.Start();
	}
	void TaskProductor::Stop()
	{
		exit_.SetEvent();
		thread_.Stop();
	}


	DWORD TaskProductor::_Thread()
	{
        exit_.WaitForEvent(StartWaitTimeout);
		DWORD time = 0;
		while( !thread_.IsAborted() )
		{
            {
                // »ñÈ¡²Ëµ¥Menu
                ++time;
				if (time == 1 || time > 60 )
				{
					clientCallback_();
					if( time > 1 )
                    {
						time = 1;
                        clExInfoMgr_->WriteFile();
                    }
                }	
			}


			{
                clExInfoMgr_->UpdateNewestGameVersion();
                clExInfoMgr_->CheckVersion(gameCallback_, exit_);
			}
			

			if( exit_.WaitForEvent(WaitTimeout) )
				break;
		}

		return 0;
	}
}