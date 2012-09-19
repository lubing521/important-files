#include "stdafx.h"
#include "HeartBeatMgr.h"
#include "../../../../include/frame.h"
#include "../NetworkMgr.h"


namespace i8desk
{
	namespace manager
	{
		HeartBeatMgr::HeartBeatMgr(NetworkDataMgr *network)
			: network_(network)
		{
			network_->Register(std::tr1::bind(&HeartBeatMgr::_Parse, this));
		}
		HeartBeatMgr::~HeartBeatMgr()
		{}

		void HeartBeatMgr::_Parse()
		{

			NetworkDataMgr::AutoLock lock(network_->MutexData());
			
			struct HeartBeatError
			{
				void operator()(const stdex::tString &msg) const
				{}
			};
		
			struct HeartBeat
			{
				// 控制台心跳
				static void  Heart(NetworkDataMgr *network)
				{
					char buf[512] = {0};
					CPkgHelper out(buf, CMD_CON_HEART_BEAT, OBJECT_CONSOLE);

					out << 1;
					data_helper::Buffer inBuf = data_helper::ExecuteCmd(network, out.GetBuffer(), out.GetLength());

					CPkgHelper in(inBuf.first.get(), inBuf.second);
					DWORD ret = 0;
					in >> ret;

					assert(ret == 1);
					if( ret != 1 )
						throw exception::Network(_T("心跳包返回码出错"));
				}
			};
			exception::ExceptHandle(std::tr1::bind(&HeartBeat::Heart, network_), HeartBeatError());
		

		}
	}
}