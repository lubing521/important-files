#ifndef __SEND_SVR_SEND_MANAGER_HPP
#define __SEND_SVR_SEND_MANAGER_HPP

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/Container/BlockingQueue.hpp"
#include "../../../include/MultiThread/Thread.hpp"
#include "../../Common/MemoryPool.hpp"



namespace sendsvr
{

	class DataMgr
	{
		typedef async::container::BlockingQueue<common::BmpInfo> QueueBuffer;
		
		typedef std::tr1::function<void(const common::BmpInfo &)> SendCallback;
		typedef std::list<SendCallback> SendCallbacks;

		class RecvImpl;
		class SendImpl;

		std::auto_ptr<RecvImpl> recvImpl_;
		std::auto_ptr<SendImpl> sendImpl_;

		SendCallbacks sendCallbacks_;
		QueueBuffer queue_;

		async::thread::AutoEvent exit_;
		async::thread::ThreadImplEx recvThread_;
		async::thread::ThreadImplEx sendThread_;

		mutable volatile long isPaused_;
		mutable volatile long isOK_;

	public:
		DataMgr(async::iocp::IODispatcher &io, u_short udpRecvPort, u_short ftpPort, const stdex::tString &ftpIP,
			const stdex::tString &ftpUserName, const stdex::tString &ftpPassword, const stdex::tString &ftpTmpPath);
		~DataMgr();

	public:
		void Start();
		void Stop();

		bool IsPaused() const
		{
			return ::InterlockedExchangeAdd(&isPaused_, 0) == 1;
		}
		
		bool IsOK() const
		{
			return ::InterlockedExchangeAdd(&isOK_, 0) == 1;	
		}

		void Pause()
		{
			::InterlockedExchange(&isPaused_, 1);
		}

		void Resume()
		{
			::InterlockedExchange(&isPaused_, 0);
		}

		void RegisterSendCallback(const SendCallback &callback);

	private:
		DWORD _RecvThread();
		DWORD _SendThread();
	};
}





#endif