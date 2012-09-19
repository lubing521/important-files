#ifndef __DOWNLOAD_ENGINE_HPP
#define __DOWNLOAD_ENGINE_HPP

#include <memory>
#include "frame.h"
#include "../I8ExInfoMgrS.h"

namespace i8desk
{
	struct IRTDataSvr;
}

namespace engine
{

	namespace detail
	{
		class DownloadEngineImpl;
	}
	
	// ---------------------------
	// class DownloadEngine

	class DownloadEngine
	{
		typedef std::auto_ptr<detail::DownloadEngineImpl> DownloadEngineImplPtr;
		DownloadEngineImplPtr impl_;
        CI8ExInfoMgrS* clExInfoMgr_;


	public:
		explicit DownloadEngine(CI8ExInfoMgrS* gameExInfoMgr);
		~DownloadEngine();
	
	private:
		DownloadEngine(const DownloadEngine &);
		DownloadEngine &operator=(const DownloadEngine &);

		// Interface
	public:
		void Start();
		void Stop();

        void Add(long type, long gid, long priority, i8desk::ISvrPlugMgr *plugMgr);
		void Delete(long gid);
	};
}





#endif