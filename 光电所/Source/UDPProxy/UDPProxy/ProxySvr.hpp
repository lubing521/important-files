#ifndef __PROXY_SERVER_HPP
#define __PROXY_SERVER_HPP

#include "../../../include/Win32/WinService/win32svr.h"
#include "../../Common/Utility.hpp"

#include "UDPProxy.hpp"
#include "StateMgr.hpp"


namespace proxy
{

	class ProxySvr
		: public win32::CWin32Svr
	{
		iocp::IODispatcher io_;

		std::auto_ptr<UDPProxy> udpProxyI_;
		std::auto_ptr<UDPProxy> udpProxyJ_;
		std::auto_ptr<StateMgr> stateMgr_;

	public:
		ProxySvr()
			: win32::CWin32Svr(L"UDPProxy", L"UDP Proxy", (common::GetAppPath() + L"UDPProxy.exe").c_str())
			, io_()
		{
			short udpRecvPortI	= common::GetIniConfig<short>(L"UDPProxy.ini", L"CONFIG", L"UDP_RECV_PORT_I");
			short udpSendPortI	= common::GetIniConfig<short>(L"UDPProxy.ini", L"CONFIG", L"UDP_SEND_PORT_I");
			short udpRecvPortJ	= common::GetIniConfig<short>(L"UDPProxy.ini", L"CONFIG", L"UDP_RECV_PORT_J");
			short udpSendPortJ	= common::GetIniConfig<short>(L"UDPProxy.ini", L"CONFIG", L"UDP_SEND_PORT_J");
			
			short tcpPort		= common::GetIniConfig<short>(L"UDPProxy.ini", L"CONFIG", L"TCP_PORT");

			std::wstring sendUDPIP = common::GetIniConfig<std::wstring>(L"UDPProxy.ini", L"CONFIG", L"UDP_SEND_IP");
			std::string udpIP = CW2A(sendUDPIP.c_str());

			udpProxyI_.reset(new UDPProxy(io_, udpRecvPortI, udpIP, udpSendPortI));
			udpProxyJ_.reset(new UDPProxy(io_, udpRecvPortJ, udpIP, udpSendPortJ));

			stateMgr_.reset(new StateMgr(io_, tcpPort));
		}

	public:
		virtual DWORD OnStart()
		{
			udpProxyI_->Start();
			udpProxyJ_->Start();

			using namespace std::tr1::placeholders;
			stateMgr_->Start(std::tr1::bind(&ProxySvr::_OnRead, this, _1, _2));

			return ERROR_SUCCESS;
		}

		virtual DWORD OnStop()
		{
			udpProxyI_->Stop();
			udpProxyJ_->Stop();
			stateMgr_->Stop();

			return ERROR_SUCCESS;
		}

	private:
		void _OnRead(const char *buf, size_t len)
		{
			const common::TCPPKHeader *inHeader = reinterpret_cast<const common::TCPPKHeader *>(buf);
			common::TCPPKHelper out(stateMgr_->Buffer().data(), stateMgr_->Buffer().size());
			common::TCPPKHeader *outHeader = reinterpret_cast<common::TCPPKHeader *>(stateMgr_->Buffer().data());

			common::TCPPKHeader header = {common::StartFlag, inHeader->cmd_, _T(""), 0};
			out << header;
			switch(inHeader->cmd_)
			{
			case common::CMD_HERT_BEAT:
				out << udpProxyI_->localIP_ << udpProxyI_->recvPort_ << udpProxyI_->IsOK() << udpProxyI_->IsPaused();
				break;
			case common::CMD_PAUSED_SEND_UDP:
				udpProxyI_->Pause();
				udpProxyJ_->Pause();
				out << 1L;
				break;
			case common::CMD_RESUME_SEND_UDP:
				udpProxyI_->Resume();
				udpProxyJ_->Resume();
				out << 1L;
				break;
			default:
				break;
			}

			outHeader->length_ = out.Length() - sizeof(common::TCPPKHeader);
			stateMgr_->Send(out.Length());
		}
	};
}



#endif