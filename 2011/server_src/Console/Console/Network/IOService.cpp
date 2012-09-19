#include "stdafx.h"
#include "IOService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace i8desk
{
	namespace io
	{
		async::iocp::IODispatcher &GetIODispatcher()
		{
			static async::iocp::IODispatcher io(3);
			return io;
		}
	}
}