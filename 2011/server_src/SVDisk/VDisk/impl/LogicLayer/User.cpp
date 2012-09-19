#include "stdafx.h"
#include "User.h"


namespace i8desk
{

	namespace logic
	{


		User::User(const SocketPtr &socket, const SocketBufferPtr &buffer)
			: socket_(socket)
			, buffer_(buffer)
		{}

		User::~User()
		{}
	}
}