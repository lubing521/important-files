#include "stdafx.h"
#include "WindowManager.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	// ---------------------------------------
	ControlMgr::ControlMgr()
		: destWnd_(0)
		, lastWnd_(0)
	{}
	ControlMgr::~ControlMgr()
	{

	}


	void ControlMgr::SetDestWindow(CWnd *wnd)
	{
		destWnd_ = wnd;
	}


	void ControlMgr::Register(size_t id, CWnd *wnd)
	{
		assert(wnd != 0);

		wnds_[id] = wnd;
		lastWnd_ = wnd;
	}

	CWnd *ControlMgr::operator[](size_t id)
	{
		Controls::const_iterator iter = wnds_.find(id);
		assert(iter != wnds_.end());
		assert(destWnd_ != 0);

		lastWnd_->ShowWindow(SW_HIDE);
		if( destWnd_ != 0 )
		{
			CRect rc;
			destWnd_->GetWindowRect(rc);
			destWnd_->GetParent()->ScreenToClient(rc);
			iter->second->MoveWindow(rc);
			iter->second->ShowWindow(SW_SHOW);
		}

		lastWnd_ = iter->second;
		return iter->second;
	}

	void ControlMgr::UpdateSize()
	{
		if( lastWnd_ != NULL && destWnd_ != NULL )
		{
			CRect rcDest;
			destWnd_->GetWindowRect(rcDest);
			destWnd_->GetParent()->ScreenToClient(rcDest);
			lastWnd_->MoveWindow(rcDest);
		}
	}

	size_t ControlMgr::GetCurrentID() const
	{
		for(Controls::const_iterator iter = wnds_.begin(); iter != wnds_.end(); ++iter)
		{
			if( iter->second == lastWnd_ )
				return iter->first;
		}

		return 0;
	}
}

