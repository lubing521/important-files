// UDPProxyTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../../../include/AsyncIO/Network/TCP.hpp"

#include "../../Common/PackageHelper.hpp"

#include <iostream>


using namespace async;
using namespace network;
using namespace iocp;

int _tmain(int argc, _TCHAR* argv[])
{
	IODispatcher io(0);
	Tcp::Socket sck(io, Tcp::V4());

	/*try
	{
		sck.Connect(Tcp::V4().Family(), IPAddress::Parse(common::PROXY_SEND_UDP_IP), common::PROXY_TCP_PORT);

		while( 1 )
		{
			char buf[1024] = {0};
			common::TCPPKHelper out(buf, 1024, common::CMD_HERT_BEAT);
			out << 1L;
			size_t ret = sck.Write(Buffer(out.Buffer(), out.Length()));
			assert(ret == out.Length());

			ret = sck.Read(Buffer(buf));
			common::TCPPKHelper in(buf, ret);
			
			DWORD flag = 0;
			bool isSendingData = false;

			in >> flag >> isSendingData;
			assert(flag == 1L);

			std::cout << (isSendingData ? "Sending" : "Pending") << std::endl; 

			::Sleep(2000);
		}
	}

	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}*/


	system("pause");
	return 0;
}

