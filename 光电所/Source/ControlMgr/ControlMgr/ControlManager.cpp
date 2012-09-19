#include "stdafx.h"
#include "ControlManager.hpp"

#include "../../Common/PackageHelper.hpp"
#include "../../Common/MemoryPool.hpp"
#include "../../../include/AsyncIO/Network/TCP.hpp"
#include "../../../include/AsyncIO/Timer/Timer.hpp"


namespace ctrl
{
	class ControlMgr::MgrImpl
	{
		iocp::IODispatcher &io_;
		timer::Timer timer_;
		timer::Timer getClientsTimer_;
		std::string ip_;
		u_short port_;

	public:
		bool isMonitorOK_;
		bool isMonitorPaused_;
		u_short monitorPort_;
		std::wstring monitorIP_;

		common::RemoteClients remoteClients_;
	public:
		MgrImpl(iocp::IODispatcher &io, const std::string &ip, u_short port)
			: io_(io)
			, timer_(io)
			, getClientsTimer_(io_)
			, ip_(ip)
			, port_(port)
			, isMonitorOK_(false)
			, isMonitorPaused_(false)
			, monitorPort_(0)
			, monitorIP_(L"0.0.0.0")
		{

		}
		~MgrImpl()
		{

		}

		void Start()
		{
			timer_.AsyncWait(std::tr1::bind(&MgrImpl::_OnTimer, this), 1000 * 5);
		}

		void StartGetClients()
		{
			getClientsTimer_.AsyncWait(std::tr1::bind(&MgrImpl::_OnGetClientsTimer, this), 1000 * 5);
		}

		void Stop()
		{
			timer_.Cancel();
		}

		void StopGetClients()
		{
			getClientsTimer_.Cancel();
		}


	public:
		void Query()
		{
			_Query();
		}

		void ChangeState(u_int cmd)
		{
			try
			{
				std::tr1::shared_ptr<network::Tcp::Socket> sck(new network::Tcp::Socket(io_, network::Tcp::V4()));
				sck->AsyncConnect(network::IPAddress::Parse(ip_), port_, 
					std::tr1::bind(&MgrImpl::_OnConnect, this, iocp::_Error, sck, cmd));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
				return;
			}
		}


		void PlayHistory(const std::wstring &i_path, const std::wstring &j_path, size_t i_freq, size_t j_freq)
		{
			try
			{
				network::Tcp::Socket sck(io_, network::Tcp::V4());

				sck.SetOption(network::SendTimeOut(3 * 1000));
				sck.SetOption(network::RecvTimeOut(3 * 1000));

				sck.Connect(AF_INET, network::IPAddress::Parse(ip_), port_);

				common::Buffer buf = common::MakeBuffer(1024);
				common::TCPPKHeader header = {common::StartFlag, common::CMD_HISTORY_START, _T(""), 0};

				common::TCPPKHelper out(buf.first.get(), buf.second);
				out << header << i_path << j_path << i_freq << j_freq;

				common::TCPPKHeader *tmp = reinterpret_cast<common::TCPPKHeader *>(buf.first.get());
				tmp->length_ = out.Length() - sizeof(common::TCPPKHeader);
				sck.Write(iocp::Buffer(out.Buffer(), out.Length()));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}
		}

		void StopHistory()
		{
			try
			{
				network::Tcp::Socket sck(io_, network::Tcp::V4());

				sck.SetOption(network::SendTimeOut(3 * 1000));
				sck.SetOption(network::RecvTimeOut(3 * 1000));

				sck.Connect(AF_INET, network::IPAddress::Parse(ip_), port_);

				common::Buffer buf = common::MakeBuffer(1024);
				common::TCPPKHeader header = {common::StartFlag, common::CMD_HISTORY_STOP, _T(""), 0};

				common::TCPPKHelper out(buf.first.get(), buf.second);
				out << header;

				common::TCPPKHeader *tmp = reinterpret_cast<common::TCPPKHeader *>(buf.first.get());
				tmp->length_ = out.Length() - sizeof(common::TCPPKHeader);
				sck.Write(iocp::Buffer(out.Buffer(), out.Length()));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}
		}

	private:
		void _Query()
		{
			try
			{
				network::Tcp::Socket sck(io_, network::Tcp::V4());
				sck.SetOption(network::SendTimeOut(3 * 1000));
				sck.SetOption(network::RecvTimeOut(3 * 1000));

				sck.Connect(AF_INET, network::IPAddress::Parse(ip_), port_);

				common::Buffer buf = common::MakeBuffer(sizeof(common::TCPPKHeader) + 4);
				common::TCPPKHeader *headerBuf = reinterpret_cast<common::TCPPKHeader *>(buf.first.get());

				headerBuf->startFlag_ = common::StartFlag;
				headerBuf->cmd_ = common::CMD_HERT_BEAT;
				_tcscpy(headerBuf->fileName_, _T(""));
				headerBuf->length_ = 4;

				sck.Write(iocp::Buffer(buf.first.get(), buf.second));

				common::TCPPKHeader header = {0};
				char *inHeaderBuffer = (char *)&header;
				size_t ret = sck.Read(iocp::Buffer(inHeaderBuffer, sizeof(header)));
				assert(ret == sizeof(header));

				common::Buffer inBuffer = common::MakeBuffer(header.length_);
				ret = sck.Read(iocp::Buffer(inBuffer.first.get(), inBuffer.second));
				assert(ret == header.length_);

				monitorIP_.clear();
				monitorPort_ = 0;
				isMonitorOK_ = false;
				isMonitorPaused_ = false;
				utility::serialize::Serialize in(inBuffer.first.get(), inBuffer.second);
				in >> monitorIP_ >> monitorPort_ >> isMonitorOK_ >> isMonitorPaused_;

			}
			catch(std::exception &e)
			{
				monitorIP_.clear();
				monitorPort_ = 0;
				isMonitorOK_ = false;
				isMonitorPaused_ = false;
				::OutputDebugStringA(e.what());
			}
		}

		void _OnConnect(u_long error, const std::tr1::shared_ptr<network::Tcp::Socket> &sck, u_int cmd)
		{
			if( error != 0 )
				return;

			common::Buffer buf = common::MakeBuffer(sizeof(common::TCPPKHeader) + 4);
			common::TCPPKHeader *headerBuf = reinterpret_cast<common::TCPPKHeader *>(buf.first.get());

			headerBuf->startFlag_ = common::StartFlag;
			headerBuf->cmd_ = cmd;
			_tcscpy(headerBuf->fileName_, _T(""));
			headerBuf->length_ = 4;


			try
			{
				sck->AsyncWrite(iocp::Buffer(buf.first.get(), buf.second), 
					std::tr1::bind(&MgrImpl::_OnWrite, this, iocp::_Size, iocp::_Error, sck, buf));
			}
			catch(std::exception &e)
			{
				::OutputDebugStringA(e.what());
			}
		}

		void _OnWrite(u_long size, u_long error, const std::tr1::shared_ptr<network::Tcp::Socket> &sck, const common::Buffer &buf)
		{
			if( size == 0 || error != 0 )
				return;
		}

		void _OnTimer()
		{
			_Query();
		}

		void _OnGetClientsTimer()
		{
			try
			{
				network::Tcp::Socket sck(io_, network::Tcp::V4());
				
				sck.SetOption(network::SendTimeOut(3 * 1000));
				sck.SetOption(network::RecvTimeOut(3 * 1000));

				sck.Connect(AF_INET, network::IPAddress::Parse(ip_), port_);

				common::Buffer buf = common::MakeBuffer(sizeof(common::TCPPKHeader) + 4);
				common::TCPPKHeader *headerBuf = reinterpret_cast<common::TCPPKHeader *>(buf.first.get());

				headerBuf->startFlag_ = common::StartFlag;
				headerBuf->cmd_ = common::CMD_GET_CLIENTS;
				_tcscpy(headerBuf->fileName_, _T(""));
				headerBuf->length_ = 4;

				sck.Write(iocp::Buffer(buf.first.get(), buf.second));

				common::TCPPKHeader header = {0};
				char *inHeaderBuffer = (char *)&header;
				size_t ret = sck.Read(iocp::Buffer(inHeaderBuffer, sizeof(header)));
				assert(ret == sizeof(header));

				common::Buffer inBuffer = common::MakeBuffer(header.length_);
				ret = sck.Read(iocp::Buffer(inBuffer.first.get(), inBuffer.second));
				assert(ret == inBuffer.second);

				utility::serialize::Serialize in(inBuffer.first.get(), inBuffer.second);
				remoteClients_.clear();
				in >> remoteClients_;
			}
			catch(std::exception &e)
			{
				remoteClients_.clear();
				::OutputDebugStringA(e.what());
			}
		}


	};

	
	ControlMgr::ControlMgr()
		: io_(2)
	{

	}
	ControlMgr::~ControlMgr()
	{

	}

	void ControlMgr::Start(const RemoteInfo &proxy, const RemoteInfo &sendSvr, const RemoteInfo &recvSvr)
	{
		proxyMgr_.reset(new MgrImpl(io_, proxy.first, proxy.second));
		proxyMgr_->Start();

		sendMgr_.reset(new MgrImpl(io_, sendSvr.first, sendSvr.second));
		sendMgr_->Start();

		recvMgr_.reset(new MgrImpl(io_, recvSvr.first, recvSvr.second));
		recvMgr_->Start();

		recvMgr_->StartGetClients();
	}

	void ControlMgr::Stop()
	{
		recvMgr_->StopGetClients();
		recvMgr_->Stop();
		sendMgr_->Stop();
		proxyMgr_->Stop();

		io_.Stop();
	}	

	void ControlMgr::Query()
	{
		proxyMgr_->Query();
		sendMgr_->Query();
		recvMgr_->Query();
	}

	void ControlMgr::ChangeSendSvrState(bool isOK)
	{
		sendMgr_->ChangeState(isOK ? common::CMD_PAUSED_VIEW : common::CMD_RESUME_VIEW);
		sendMgr_->Query();
	}

	void ControlMgr::ChangeUDPProxyState(bool isOK)
	{
		proxyMgr_->ChangeState(isOK ? common::CMD_PAUSED_SEND_UDP : common::CMD_RESUME_SEND_UDP);
		proxyMgr_->Query();
	}

	void ControlMgr::ChangeRecvSvrState(bool isOK)
	{
		recvMgr_->ChangeState(isOK ? common::CMD_PAUSED_VIEW : common::CMD_RESUME_VIEW);
		recvMgr_->Query();
	}

	StatusInfo ControlMgr::GetUDPProxyStatus() const
	{
		return std::tr1::make_tuple(proxyMgr_->isMonitorOK_, proxyMgr_->isMonitorPaused_, proxyMgr_->monitorIP_, proxyMgr_->monitorPort_);
	}


	StatusInfo ControlMgr::GetSendSvrStatus() const
	{
		return std::tr1::make_tuple(sendMgr_->isMonitorOK_, sendMgr_->isMonitorPaused_, sendMgr_->monitorIP_, sendMgr_->monitorPort_);
	}
	StatusInfo ControlMgr::GetRecvSvrStatus() const
	{
		return std::tr1::make_tuple(recvMgr_->isMonitorOK_, recvMgr_->isMonitorPaused_, recvMgr_->monitorIP_, recvMgr_->monitorPort_);
	}

	common::RemoteClients ControlMgr::GetRecvSvrClients() const
	{
		return recvMgr_->remoteClients_;
	}

	void ControlMgr::PlayHistory(const std::wstring &i_path, const std::wstring &j_path, size_t i_freq, size_t j_freq)
	{
		recvMgr_->PlayHistory(i_path, j_path, i_freq, j_freq);
	}

	void ControlMgr::StopHistory()
	{
		recvMgr_->StopHistory();
	}
}