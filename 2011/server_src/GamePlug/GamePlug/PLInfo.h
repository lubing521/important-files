#ifndef __I8DESK_PL_ROOT_HPP
#define __I8DESK_PL_ROOT_HPP


#include "../../../include/MultiThread/Lock.hpp"
#include "../../../include/Allocator/ContainerAllocator.hpp"
#include "../../../include/MemoryPool/FixedMemoryPool.hpp"


#include <tuple>


namespace i8desk
{


	// ---------------------------------------------------
	// class PLRoot

	class PLInfo
	{
		typedef async::thread::AutoCriticalSection	SCLock;
		typedef async::thread::AutoLock<SCLock>		AutoLock;

	private:
		std::wstring plRoot_;
		std::wstring plVersion_;
		mutable SCLock rootMutex_;
		mutable SCLock versionMutex_;


	public:
		void PLRoot(const std::wstring &root)
		{
			AutoLock lock(rootMutex_);
			plRoot_ = root;
		}

		const std::wstring &PLRoot() const
		{
			AutoLock lock(rootMutex_);
			return plRoot_;
		}

		void PLVersion(const std::wstring &version)
		{
			AutoLock lock(versionMutex_);
			plVersion_ = version;
		}

		const std::wstring &PLVersion() const
		{
			AutoLock lock(versionMutex_);
			return plVersion_;
		}


	public:
		static PLInfo &GetInstance()
		{
			static PLInfo plInfo;
			return plInfo;
		}
	};




	// ------------------------------------------------
	// class DeleteInfo

	class DeleteInfo
	{
		typedef async::thread::AutoCriticalSection	SCLock;
		typedef async::thread::AutoLock<SCLock>		AutoLock;

		typedef std::tr1::tuple<long, stdex::tString, DWORD>				GameInfo;
		typedef async::memory::SGIMTMemoryPool								Pool;
		typedef async::allocator::ContainerAllocator<GameInfo, Pool>		SetAllocator;
		typedef std::set< GameInfo, std::less<GameInfo>, SetAllocator >		GameInfoContainer;

	public:
		GameInfoContainer	gamesInfo_;
		SCLock				lock_;
		IRTDataSvr			*rtDataSvr_;

	private:
		DeleteInfo(IRTDataSvr *rtDataSvr)
			: gamesInfo_(std::less<GameInfo>(), _Allocator())
			, rtDataSvr_(rtDataSvr)
		{}

	public:
		void PushGame(long gid, DWORD delFlag)
		{
			AutoLock lock(lock_);

			db::tGame game;	
			if( rtDataSvr_->GetGameTable()->GetData(gid, &game) != 0 )
				return;

			gamesInfo_.insert(std::tr1::make_tuple(gid, game.SvrPath, delFlag));
		}

		void Clear()
		{
			AutoLock lock(lock_);
			gamesInfo_.clear();
		}

		template<typename Pred>
		void ForEach(const Pred &pred)
		{
			AutoLock lock(lock_);

			std::for_each(gamesInfo_.begin(), gamesInfo_.end(), pred);

			Clear();
		}

	private:
		SetAllocator &_Allocator()
		{
			static Pool pool;
			static SetAllocator alloc(pool);
			return alloc;
		}

	public:
		static DeleteInfo &GetInstance(IRTDataSvr *rtDataSvr = 0)
		{
			static DeleteInfo deleteInfo(rtDataSvr);
			return deleteInfo;
		}
	};

}




#endif