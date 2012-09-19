#ifndef __LOCAL_GAME_CHECKER_HPP
#define __LOCAL_GAME_CHECKER_HPP

#include "../../../include/MultiThread/Thread.hpp"
#include "../../../include/MultiThread/Lock.hpp"
#include "I8ExInfoMgrS.h"


namespace i8desk
{
	// ---------------------------
	// class LocalGameChecker

	class TaskProductor
	{
		IRTDataSvr *rt_;
        CI8ExInfoMgrS* clExInfoMgr_;
		async::thread::ThreadImplEx thread_;
		async::thread::AutoEvent exit_;

		typedef std::tr1::function<void(long, long)> GameCallback;
		GameCallback gameCallback_;

		typedef std::tr1::function<void()> ClientMenuCallback;
		ClientMenuCallback clientCallback_;

        static const long StartWaitTimeout = 10 * 1000;
        static const long WaitTimeout = 60 * 1000;

	public:
		TaskProductor(IRTDataSvr *rt, CI8ExInfoMgrS* gameExInfoMgr, const GameCallback &callback, const ClientMenuCallback &clientCallback);

	public:
		void Start();
		void Stop();

	private:
		DWORD _Thread();
	};
}


#endif