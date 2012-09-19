#ifndef __USER_MANAGER_HPP
#define __USER_MANAGER_HPP

#include <unordered_map>
#include <memory>

#include "../../async/Network/Socket.hpp"
#include "Key.hpp"

namespace i8desk
{
	
	namespace logic
	{

		using async::network::SocketPtr;
		using async::network::SocketBufferPtr;

		class User;
		

		// ----------------------------------------
		// class UserMgr

		class UserMgr
		{
			typedef Key::value_type				KeyType;
			typedef std::tr1::shared_ptr<User>	UserType;
			
		private:
			std::tr1::unordered_map<KeyType, UserType> users_;

		private:
			UserMgr();
			~UserMgr();

		public:
			static UserMgr &GetInstance()
			{
				static UserMgr mgr;
				return mgr;
			}

		public:
			void AddUser(const SocketPtr &, const SocketBufferPtr &, const SOCKADDR_IN &);
			void RemoveUser(const SocketPtr &);

			void GetUserData(const SocketPtr &, const SocketBufferPtr &, size_t, size_t);
			void SaveUserData();
		
		public:

		};
	}


}
#endif