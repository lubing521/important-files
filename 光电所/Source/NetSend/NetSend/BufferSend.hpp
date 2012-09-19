#ifndef __BUFFER_SEND_HPP
#define __BUFFER_SEND_HPP

#include <vector>
#include <ctime>
#include <array>
#include <atlbase.h>
#include <atlconv.h>

#include "../../../include/AsyncIO/IOCP/Dispatcher.hpp"
#include "../../../include/AsyncIO/Network/UDP.hpp"

#include "../../Common/PackageHelper.hpp"
#include "../../Common/Utility.hpp"

using namespace async;
using namespace async::network;
using namespace async::iocp;

namespace oc
{
	// -------------------------------
	// udp数据包发送

	class BufferSend
	{
		const char *buf_;
		size_t buflen_;
		
		std::string fileName_;

		IODispatcher io_;
		Udp::Socket sck_;

		SOCKADDR_IN addr_;

	public:
		BufferSend(const std::wstring &portName)
			: buf_(0)
			, buflen_(0)
			, io_(0)
			, sck_(io_, Udp::V4())
		{
			short port = common::GetIniConfig<short>(L"NetSend.ini", L"CONFIG", portName.c_str());
			std::wstring ip = common::GetIniConfig<std::wstring>(L"NetSend.ini", L"CONFIG", L"IP");
				
			addr_.sin_family = AF_INET;
			addr_.sin_port = ::htons(port);
			addr_.sin_addr.s_addr = ::htonl(IPAddress::Parse((LPCSTR)CW2A(ip.c_str())));

			sck_.SetOption(network::RecvTimeOut(3000));	
		}
		~BufferSend()
		{
			io_.Stop();
		}
	public:
		void SetBuffer(const char *buf, size_t len, const char *fileName);

		void Run();

	private:
		void _Subcontract();
		void _Send(const char *buf, size_t len);
		void _SendToImpl(size_t,  size_t);
		void _RecvFromImpl(size_t, size_t);
	};
}



#endif