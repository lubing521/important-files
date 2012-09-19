#ifndef __LOGIC_LAYER_KEY_HPP
#define __LOGIC_LAYER_KEY_HPP



namespace i8desk
{
	namespace logic
	{

		// ------------------------------------------
		// class Key

		struct Key
		{
			typedef SOCKET value_type;

			value_type value_;

			Key(const SOCKET &sock, const SOCKADDR_IN &addr)
				: value_(sock)
			{}

			value_type operator()() const
			{
				return value_;
			}
		};
	}
}




#endif