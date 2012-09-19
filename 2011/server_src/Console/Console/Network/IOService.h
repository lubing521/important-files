#ifndef __IO_SERVICE_HPP
#define __IO_SERVICE_HPP

#include "../../../../include/AsyncIO/iocp/Dispatcher.hpp"


namespace i8desk
{
	namespace io
	{
		async::iocp::IODispatcher &GetIODispatcher();
	}
}


#endif