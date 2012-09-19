#include "stdafx.h"
#include "DownloadEngine.hpp"

#include "detail/DownloadEngineImpl.hpp"

namespace engine
{

	DownloadEngine::DownloadEngine(CI8ExInfoMgrS* gameExInfoMgr)
		: impl_(new detail::DownloadEngineImpl)
        , clExInfoMgr_(gameExInfoMgr)
	{}

	DownloadEngine::~DownloadEngine()
	{}

	void DownloadEngine::Start()
	{
		impl_->Start();
	}

	void DownloadEngine::Stop()
	{
		impl_->Stop();
	}

    void DownloadEngine::Add(long type, long gid, long priority, i8desk::ISvrPlugMgr *plugMgr)
	{
		impl_->Add(type, gid, priority, plugMgr, clExInfoMgr_);
	}

	void DownloadEngine::Delete(long gid)
	{
		assert(0);
	}
}