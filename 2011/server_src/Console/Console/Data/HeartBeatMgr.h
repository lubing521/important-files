#ifndef __HEART_BEAT_MANAGER_HPP
#define __HEART_BEAT_MANAGER_HPP


namespace i8desk
{

	// forward declare
	class NetworkDataMgr;

	namespace manager
	{	

		// ------------------------------
		// class HeartBeatMgr
		
		class HeartBeatMgr
		{

		private:
			NetworkDataMgr								*network_;

		public:
			explicit HeartBeatMgr(NetworkDataMgr *network);
			~HeartBeatMgr();

		private:
			HeartBeatMgr(const HeartBeatMgr &);
			HeartBeatMgr &operator=(const HeartBeatMgr &);
		
		private:
			void _Parse();
		};
		
	}

}




#endif