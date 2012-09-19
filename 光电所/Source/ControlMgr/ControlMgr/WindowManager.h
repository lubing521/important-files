#ifndef __CONSOLE_WINDOWS_MANAGER_HPP
#define __CONSOLE_WINDOWS_MANAGER_HPP


#include <map>


namespace ui
{

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




#endif