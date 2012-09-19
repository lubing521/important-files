#ifndef __USER_HPP
#define __USER_HPP


#include "../../async/Network/Socket.hpp"

namespace i8desk
{

	namespace logic
	{
		using async::network::SocketPtr;
		using async::network::SocketBufferPtr;


		// ----------------------------------------
		// class User

		class User
		{
		private:
			SocketPtr socket_;
			SocketBufferPtr buffer_;

		public:
			User(const SocketPtr &, const SocketBufferPtr &);
			~User();

		private:
			User(const User &);
			User &operator=(const User &);
		};

	}


}


#endif