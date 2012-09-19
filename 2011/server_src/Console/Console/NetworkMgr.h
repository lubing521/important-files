#ifndef __CONSOLE_NETWORK_INSTANCE_HPP
#define __CONSOLE_NETWORK_INSTANCE_HPP

#include "Network/Network.h"
#include "../../../include/MultiThread/Lock.hpp"
#include "Network/NetworkImpl.h"
#include <list>


namespace i8desk
{


	// ------------------------
	// class NetworkDataMgr

	// Network Adapter

	class NetworkDataMgr
	{
	public:
		typedef std::tr1::function<void()>			Callback;
		typedef std::list<Callback>					Callbacks;
		typedef async::thread::AutoCriticalSection	Mutex;
		typedef async::thread::AutoLock<Mutex>		AutoLock;

	private:
		networklayer::NetworkInterfacePtr	network_;
		Callback							connectCallback_;
		Callbacks							callbacks_;		
		Mutex								mutexCallback_;
		Mutex								mutexData_;

		async::timer::Timer					timer_;

		std::string							ip_;
		u_short								port_;

	public:
		NetworkDataMgr();
		~NetworkDataMgr();

	private:
		NetworkDataMgr(const NetworkDataMgr &);
		NetworkDataMgr &operator=(const NetworkDataMgr &);
		
	public:
		void Start(const std::string &ip, u_short port);
		void Stop();

		void Register(const Callback &callback);
		void UnRegister(const Callback &callback);

		void RegisterOnConnect(const Callback &conCallback);


		void Send(const char *buf, size_t len) const;
		void Recv(char *buf, size_t len) const;


		bool WaitConnect(size_t timeOut);

		Mutex &MutexData()
		{ return mutexData_; }

		void Lock()
		{ mutexData_.Lock(); }

		void UnLock()
		{ mutexData_.Unlock(); }

	private:
		void _OnConnect();
		void _OnTimeout();
	};


	namespace data_helper
	{
		typedef std::tr1::shared_ptr<char> BufferPtr;
		typedef std::pair<BufferPtr, size_t> Buffer;

		typedef size_t				uint32;
		typedef unsigned long long	uint64;


		inline void BufferDeletor(void *p)
		{
			::operator delete (p);
		}

		inline BufferPtr BufferAllocator(size_t len)
		{
			typedef BufferPtr::element_type ValueType;
			BufferPtr buf(static_cast<ValueType *>(::operator new (len)), &BufferDeletor);
			return buf;
		}


		// 执行命令
		Buffer ExecuteCmd(NetworkDataMgr *network, const char *buf, size_t len);

		Buffer ExecuteMsg(NetworkDataMgr *network, const char *buf, size_t len);

		Buffer ExecuteSQL(NetworkDataMgr *network, DWORD table, uint32 CondMode = 0, uint64 CondMask = 0, 
			DWORD dwParam1 = 0, DWORD dwParam2 = 0, 
			const stdex::tString &strParam1 = _T(""), const stdex::tString &strParam2 = _T(""),
			uint64 mask = 0, uint32 desc = 0);

		template <typename T>
		Buffer ExecuteSQL(NetworkDataMgr *network, DWORD optype, DWORD table, const T *const d = 0, 
			uint32 CondMode = SQL_COND_MODE_KEY, uint64 CondMask = 0, 
			DWORD dwParam1 = 0, DWORD dwParam2 = 0, 
			const stdex::tString &strParam1 = _T(""), const stdex::tString &strParam2 = _T(""), 
			uint64 mask = 0)
		{
			char buf[8192] = {0};

			CPkgHelper out(buf, CMD_GAME_CONEXESQL, OBJECT_CONSOLE);

			out << optype << table << CondMode << CondMask 
				<< dwParam1 << dwParam2 << strParam1 <<  strParam2
				<< mask;

			if( d != 0 ) 
				out << *d;

			// 发送数据并接收
			Buffer inBuf = ExecuteCmd(network, out.GetBuffer(), out.GetLength());

			CPkgHelper in(inBuf.first.get(), inBuf.second);
			DWORD ret = 0;
			in >> ret;
			if( ret == 0 )
			{
				stdex::tString error;
				in >> error;
				throw exception::Businuss(error);
			}

			return inBuf;
		}
	}

}



#endif