#ifndef __NETWORK_BUFFER_HELPER_HPP
#define __NETWORK_BUFFER_HELPER_HPP



#include "Socket.hpp"
#include <array>
#include <vector>
#include <string>

namespace async
{
	namespace network
	{
		
		// ------------------------

		inline SocketBufferPtr Buffer(char *buf, size_t sz)
		{
			return SocketBufferPtr(new SocketBuffer(buf, sz));
		}


		// --------------------------

		template<size_t _N>
		inline SocketBufferPtr Buffer(char (&arr)[_N])
		{
			return SocketBufferPtr(new SocketBuffer(arr, _N));
		}

		// --------------------------

		template<size_t _N>
		inline SocketBufferPtr Buffer(std::tr1::array<char, _N> &arr)
		{
			return SocketBufferPtr(new SocketBuffer(arr.data(), _N));
		}


		// --------------------------

		inline SocketBufferPtr Buffer(std::vector<char> &arr)
		{
			return SocketBufferPtr(new SocketBuffer(&arr[0], arr.size()));
		}


		// --------------------------

		inline SocketBufferPtr Buffer(std::string &arr)
		{
			return SocketBufferPtr(new SocketBuffer(&*arr.begin(), arr.size()));
		}

	}
}


#endif // endif