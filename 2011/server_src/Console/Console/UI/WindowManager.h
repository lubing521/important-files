#ifndef __CONSOLE_WINDOWS_MANAGER_HPP
#define __CONSOLE_WINDOWS_MANAGER_HPP


#include <map>
#include <set>
#include "Skin/SkinMgr.h"

namespace i8desk
{
	namespace ui
	{
		// 事件接口
		struct Event
		{
			virtual ~Event(){}
			virtual void Register()		= 0;
			virtual void UnRegister()	= 0;
		};

		// 主窗口管理子窗口接口
		struct ChildMgr
		{
			typedef std::set<CDialog *> TopDialogs;
			TopDialogs topChilds_;
			ChildMgr()
			{}

			virtual ~ChildMgr()
			{

			}
			void InsertTopChild(CDialog *child)
			{
				if( this == 0 )
					return;

				topChilds_.insert(child);
			}

			void EraseTopChild(CDialog *child)
			{
				if( this!= 0 && !topChilds_.empty() )
					topChilds_.erase(child);
			}

			void EndTopDialog()
			{
				for(TopDialogs::const_iterator iter = topChilds_.begin(); iter != topChilds_.end(); ++iter)
					(*iter)->EndDialog(IDCANCEL);
			}
		};


		enum { WM_MSG_UPDATE = WM_USER + 100, WM_MSG_REINIT, WM_ASYNC_DATA, WM_MSG_ERROR };
		enum { WM_SELECT_TAB = WM_USER + 500, WM_APPLY_SELECT, WM_UPDATE_SELECT_STATE, WM_REPORT_SELECT_STATE };
		enum { WM_CHANGE_DEBUG_MODE = WM_USER + 600, WM_SAVE_HARDWARE_MSG };
		enum { WM_ADD_VDISK_MSG = WM_USER + 700, WM_DEL_VDISK_MSG, WM_MOD_VDISK_MSG };
		enum { WM_SHOW_LOG = WM_USER + 800, WM_SHOW_LOG_MORE };

		typedef unsigned long long uint64;

		class BaseWnd
			: public CDialog
			, public Event
		{
		public:
			explicit BaseWnd(UINT ID, CWnd* pParent = 0);
			virtual ~BaseWnd();

		public:
			virtual void Register() = 0;
			virtual void UnRegister() = 0;

		public:
			virtual void OnRealDataUpdate() = 0;
			virtual void OnReConnect()		= 0;
			virtual void OnAsyncData()		= 0;
			virtual void OnDataComplate()	= 0;
			
		public:
			virtual BOOL PreTranslateMessage(MSG* pMsg);

		public:
			void UpdateCallback();
			void AsyncRun();

		private:
			DECLARE_MESSAGE_MAP()
			afx_msg LRESULT OnUpdateMsg(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnReInitMsg(WPARAM wParam, LPARAM lParam);
			afx_msg LRESULT OnAsyncMsg(WPARAM wParam, LPARAM lParam);
			afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		};

		// -------------------------------------
		// class WindowMgr

		class WindowMgr
		{
			typedef std::tr1::shared_ptr<BaseWnd> BaseWndPtr;
			typedef std::map<size_t, BaseWndPtr> Windows;

		private:
			Windows wnds_;
			BaseWndPtr lastWnd_;
			CWnd *destWnd_;
			CWnd *parentWnd_;

		public:
			WindowMgr();
			~WindowMgr();

		private:
			WindowMgr(const WindowMgr &);
			WindowMgr &operator=(const WindowMgr &);

		public:
			void SetWindow(CWnd *parentWnd, CWnd *destWnd);

			template < typename WndT >
			void Register()
			{
				BaseWndPtr wndPtr(new WndT(parentWnd_));
				wnds_[WndT::IDD] = wndPtr;
			}

			void UnRegisterAll();
			void UnRegister(size_t id);

			CWnd *Get(size_t id);
			CWnd *operator[](size_t id);
			CWnd *Select(size_t id)
			{
				return (*this)[id];
			}

			CWnd *GetCurWnd()
			{
				assert(lastWnd_ != 0);
				return lastWnd_.get(); 
			}

			void UpdateSize();

			size_t Size() const;

			template < typename HandlerT >
			void ForEach(const HandlerT &handler)
			{
				for(Windows::iterator iter = wnds_.begin(); iter != wnds_.end(); ++iter)
				{	
					if( ::IsWindow(iter->second->GetSafeHwnd()) )
						handler(iter->second.get());
				}
			}

		};


		// ---------------------------------------------
		// class ControlMgr

		class ControlMgr
		{
			typedef std::map<size_t, CWnd *> Controls;

		private:
			Controls wnds_;
			CWnd *lastWnd_;
			CWnd *destWnd_;

		public:
			ControlMgr();
			~ControlMgr();

		private:
			ControlMgr(const ControlMgr &);
			ControlMgr &operator=(const ControlMgr &);

		public:
			void SetDestWindow(CWnd *wnd);
			void Register(size_t id, CWnd *wnd);
			CWnd *operator[](size_t id);

			void UpdateSize();
			CWnd *GetCurrentControl() const
			{ return lastWnd_; };
			size_t GetCurrentID() const;
			
		};
	}
}




#endif