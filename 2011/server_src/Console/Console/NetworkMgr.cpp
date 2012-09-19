#include "stdafx.h"
#include "NetworkMgr.h"

#include "Network/NetworkImpl.h"
#include "Network/NetworkImpl2.h"
#include "../../../include/frame.h"

#include "Network/IOService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{

	namespace data_helper
	{

		// 执行命令
		Buffer ExecuteCmd(NetworkDataMgr *network, const char *buf, size_t len)
		{
			// 发送数据
			network->Send(buf, len);

			// 先收包头
			pkgheader header = {0};
			network->Recv(reinterpret_cast<char *>(&header), sizeof(pkgheader));

			// 申请内存接收数据
			const size_t bufLen = header.Length;
			assert(bufLen != 0);
			if( bufLen == 0 )
				throw exception::Network(_T("数据包长度为0"));

			BufferPtr inBuf(BufferAllocator(bufLen));
			std::copy(reinterpret_cast<char *>(&header), reinterpret_cast<char *>(&header) + sizeof(pkgheader), inBuf.get());
			network->Recv(inBuf.get() + sizeof(pkgheader), header.Length - sizeof(pkgheader));

			return Buffer(inBuf, bufLen);
		}


		Buffer ExecuteMsg(NetworkDataMgr *network, const char *buf, size_t len)
		{
			data_helper::Buffer inBuf = data_helper::ExecuteCmd(network, buf, len);

			CPkgHelper in(inBuf.first.get(), inBuf.second);
			DWORD ret = 0;
			in >> ret;
			//assert(ret != 0);

			if( ret == 0 )
			{
				stdex::tString err;
				in >> err;
				throw exception::Businuss(err);
			}

			return inBuf;
		}

		Buffer ExecuteSQL(NetworkDataMgr *network, DWORD table, uint32 CondMode/* = 0*/, uint64 CondMask/* = 0*/, 
			DWORD dwParam1/* = 0*/, DWORD dwParam2/* = 0*/, 
			const stdex::tString &strParam1/* = _T("")*/, const stdex::tString &strParam2/* = _T("")*/,
			uint64 mask/* = 0*/, uint32 desc/* = 0*/)
		{
			char buf[1024] = {0};
			CPkgHelper out(buf, CMD_GAME_CONEXESQL, OBJECT_CONSOLE);
			const uint32 optype = SQL_OP_SELECT;

			out << optype << table << CondMode << CondMask 
				<< dwParam1 << dwParam2 << strParam1 <<  strParam2
				<< mask << desc;

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



	// 超时
	const size_t timeOut = 2000;



	NetworkDataMgr::NetworkDataMgr()
		: network_(new networklayer::NetworkImpl(std::tr1::bind(&NetworkDataMgr::_OnConnect, this)))
		, timer_(io::GetIODispatcher(), timeOut, 0, std::tr1::bind(&NetworkDataMgr::_OnTimeout, this))
		, port_(0)
	{}

	NetworkDataMgr::~NetworkDataMgr()
	{
	}

	void NetworkDataMgr::Start(const std::string &ip, u_short port)
	{
		if( !network_->IsConnect() )
		{
			network_->Connect(ip, port);
			ip_   = ip;
			port_ = port;
		}
	}

	void NetworkDataMgr::Stop()
	{
		timer_.Cancel();
		network_->Close();
	}


	void NetworkDataMgr::Register(const Callback &callback)
	{
		AutoLock lock(mutexCallback_);
		callbacks_.push_back(callback);
	}

	void NetworkDataMgr::UnRegister(const Callback &callback)
	{
		AutoLock lock(mutexCallback_);
		//callbacks_.remove(callback);
	}

	void NetworkDataMgr::RegisterOnConnect(const Callback &conCallback)
	{
		connectCallback_ = conCallback;
	}

	void NetworkDataMgr::Send(const char *buf, size_t len) const
	{
		if( network_->IsConnect() )
			network_->Send(buf, len);
		else
			throw exception::UnConnect(_T("无法连接服务器"));
	}

	void NetworkDataMgr::Recv(char *buf, size_t len) const
	{
		if( network_->IsConnect() )
			network_->Recv(buf, len);
		else
			throw exception::UnConnect(_T("无法连接服务器"));
	}

	bool NetworkDataMgr::WaitConnect(size_t timeOut)
	{
		async::thread::AutoEvent waitEvent;
		waitEvent.Create();

		size_t timeLeft = 0;
		while(true)
		{
			::WaitForSingleObject(waitEvent, timeOut / 100);
			
			if( timeLeft++ == 100 )
				break;

			if( network_->IsConnect() )
				return true;
		}

		return false;
	}

	void NetworkDataMgr::_OnConnect()
	{
		timer_.AsyncWait();

		if( connectCallback_ != 0 )
			connectCallback_();
	}

	void NetworkDataMgr::_OnTimeout()
	{

		if( !network_->IsConnect() )
		{
			network_->Connect(ip_, port_);
			return;
		}
		

		// 回调仿函数
		struct CallbackForEach
		{
			void operator()(const Callback &callback) const
			{ 
				if( callback )
					callback(); 
			}
		};

		AutoLock lock(mutexCallback_);
		std::for_each(callbacks_.begin(), callbacks_.end(), 
			CallbackForEach());

	}




}