#ifndef __NETWORK_FLOW_HPP
#define __NETWORK_FLOW_HPP


namespace i8desk
{

	struct Flow
	{
		volatile unsigned __int64 systemRead_;
		volatile unsigned __int64 diskRead_;
		volatile unsigned __int64 cacheRead_;

		volatile unsigned __int64 systemSize_;
		volatile unsigned __int64 diskSize_;
		volatile unsigned __int64 cacheSize_;
	};

	extern Flow g_flow;

	template < typename T, typename U >
	inline void Add(T &val, const U &u)
	{
		//::InterlockedExchangeAdd(&val, u);
		val += u;
	}

}




#endif