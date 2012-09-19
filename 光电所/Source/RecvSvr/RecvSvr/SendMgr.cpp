#include "stdafx.h"
#include "SendMgr.hpp"


namespace recvSvr
{


	size_t SendMgr::Register(const Callback &callback)
	{
		static size_t ID = 0;
		callbacks_.insert(++ID, callback);
		
		return ID;
	}

	void SendMgr::UnRegister(size_t ID)
	{
		callbacks_.erase(ID);
	}

	void SendMgr::Start()
	{
		exit_.Create(0, TRUE);
		io_.Post(std::tr1::bind(&SendMgr::_Handle, this));
	}

	void SendMgr::Stop()
	{
		exit_.SetEvent();
	}


	void SendMgr::_Handle()
	{
		struct OP
		{
			const common::BmpInfo &buffer_;
			OP(const common::BmpInfo &buf)
				: buffer_(buf)
			{}
			void operator()(const Callbacks::value_type &val) const
			{
				val.second(buffer_);
			}
		};

		while(1)
		{
			common::BmpInfo buffer = bufferCallback_();
			if( buffer.first.second == 0 || 
				exit_.WaitForEvent(0) )
				break;

			callbacks_.for_each(OP(buffer));
		}
		
	}
}