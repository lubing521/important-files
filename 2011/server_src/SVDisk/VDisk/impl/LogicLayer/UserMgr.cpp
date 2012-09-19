#include "stdafx.h"
#include "UserMgr.h"


#include "User.h"
#include "../LayerHelper.h"


namespace i8desk
{

	namespace logic
	{
		UserMgr::UserMgr()
		{

		}

		UserMgr::~UserMgr()
		{

		}


		void UserMgr::AddUser(const SocketPtr &socket, const SocketBufferPtr &buffer, const SOCKADDR_IN &addr)
		{
			if( users_.find(*socket) == users_.end() )
				return;

			Key key(*socket, addr);
			UserType newUser(new User(socket, buffer));
			
			users_.insert(std::make_pair(key(), newUser));
		}

		void UserMgr::RemoveUser(const SocketPtr &socket)
		{
			users_.erase(*socket);
		}
	}
}