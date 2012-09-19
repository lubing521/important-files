#include "stdafx.h"
#include "WindowManager.h"

#include "../../../../include/ui/wtl/atlwinmisc.h"
#include "../Network/IOService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{
	namespace ui
	{
		namespace detail
		{
			struct AsyncOp
				: public std::tr1::enable_shared_from_this<AsyncOp>
			{
				BaseWnd *base_;

				AsyncOp(BaseWnd *base)
					: base_(base)
				{}

				void Run()
				{
					/*async::iocp::AsyncCallbackBasePtr callback(async::iocp::MakeAsyncCallback(
						));*/
					i8desk::io::GetIODispatcher().Post(
						std::tr1::bind(&AsyncOp::_RunImpl, shared_from_this()));
				}
				void _RunImpl()
				{
					base_->OnAsyncData();
					
					assert(::IsWindow(base_->GetSafeHwnd()));
					base_->PostMessage(WM_ASYNC_DATA);
				}
			};

			typedef std::tr1::shared_ptr<AsyncOp> AsyncOpPtr;
		}


		BaseWnd::BaseWnd(UINT ID, CWnd* pParent /* = NULL */)
			: CDialog(ID, pParent)
		{}
		BaseWnd::~BaseWnd()
		{}


		BEGIN_MESSAGE_MAP(BaseWnd, CDialog)
			ON_MESSAGE(WM_MSG_UPDATE, &BaseWnd::OnUpdateMsg)
			ON_MESSAGE(WM_MSG_REINIT, &BaseWnd::OnReInitMsg)
			ON_MESSAGE(WM_ASYNC_DATA, &BaseWnd::OnAsyncMsg)
			ON_WM_ERASEBKGND()
		END_MESSAGE_MAP()

		
		void BaseWnd::UpdateCallback()
		{
			if( ::IsWindow(m_hWnd) )
				PostMessage(WM_MSG_UPDATE);
		}


		LRESULT BaseWnd::OnUpdateMsg(WPARAM wParam, LPARAM lParam)
		{
			OnRealDataUpdate();
			return TRUE;
		}


		LRESULT BaseWnd::OnReInitMsg(WPARAM wParam, LPARAM lParam)
		{
			OnReConnect();
			AsyncRun();

			return TRUE;
		}

		LRESULT BaseWnd::OnAsyncMsg(WPARAM wParam, LPARAM lParam)
		{
			OnDataComplate();
			return TRUE;
		}

		void BaseWnd::AsyncRun()
		{
			detail::AsyncOpPtr op(new detail::AsyncOp(this));
			op->Run();
		}

		BOOL BaseWnd::PreTranslateMessage(MSG* pMsg)
		{
			if( pMsg->message == WM_KEYDOWN )     
			{     
				if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )     
					return TRUE;       
			}

			return __super::PreTranslateMessage(pMsg);
		}

		BOOL BaseWnd::OnEraseBkgnd(CDC* pDC)
		{
			/*WTL::CClientRect rcClient(GetSafeHwnd());
			CMemDC memDC(*pDC, rcClient);
			memDC.GetDC().FillSolidRect(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
				RGB(255, 255, 255));*/

			return TRUE;//__super::OnEraseBkgnd(pDC);
		}

		



		// -------------------------------------

		WindowMgr::WindowMgr()
			: destWnd_(0)
			, parentWnd_(0)
		{}
		WindowMgr::~WindowMgr()
		{
		}
	

		void WindowMgr::SetWindow(CWnd *parentWnd, CWnd *destWnd)
		{
			//assert(destWnd_ != 0);
			assert(parentWnd != 0);

			parentWnd_ = parentWnd;
			destWnd_ = destWnd;
		}


		void WindowMgr::UnRegisterAll()
		{
			for(Windows::const_iterator iter = wnds_.begin(); 
				iter != wnds_.end(); ++iter)
			{
				if( iter->second->GetSafeHwnd() != NULL )
				{
					iter->second->DestroyWindow();
				}
			}
		}
		void WindowMgr::UnRegister(size_t id)
		{
			Windows::const_iterator iter = wnds_.find(id);
			assert(iter != wnds_.end());

			iter->second->DestroyWindow();
		}

		CWnd *WindowMgr::Get(size_t id)
		{
			return wnds_[id].get();
		}

		CWnd *WindowMgr::operator[](size_t id)
		{
			Windows::const_iterator iter = wnds_.find(id);
			assert(iter != wnds_.end());
			assert(destWnd_ != 0);

			if( destWnd_ != 0 && lastWnd_ != iter->second )
			{
				CRect rc;
				destWnd_->GetWindowRect(rc);
				destWnd_->GetParent()->ScreenToClient(rc);

				if( !::IsWindow(iter->second->GetSafeHwnd()) )
				{
					iter->second->Create(iter->first, parentWnd_);
					iter->second->AsyncRun();
				}

				iter->second->MoveWindow(rc);
				iter->second->ShowWindow(SW_SHOW);
				iter->second->Register();

				if( lastWnd_->GetSafeHwnd() != NULL )
				{
					lastWnd_->ShowWindow(SW_HIDE);
					lastWnd_->UnRegister();
				}
			}

            parentWnd_->PostMessage(i8desk::ui::WM_UPDATE_SELECT_STATE);
            iter->second.get()->PostMessage(i8desk::ui::WM_REPORT_SELECT_STATE);

			lastWnd_ = iter->second;
			return iter->second.get();
		}

		void WindowMgr::UpdateSize()
		{
			if( lastWnd_->GetSafeHwnd() != NULL && destWnd_->GetSafeHwnd() != NULL )
			{
				CRect rcDest;
				destWnd_->GetWindowRect(rcDest);
				destWnd_->GetParent()->ScreenToClient(rcDest);
				lastWnd_->MoveWindow(rcDest);
			}
		}

		size_t WindowMgr::Size() const
		{
			return wnds_.size();
		}


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
}

