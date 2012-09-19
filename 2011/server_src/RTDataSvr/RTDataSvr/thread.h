#ifndef I8_THREAD_H
#define I8_THREAD_H

#include <windows.h>
#include <process.h>
#include <assert.h>

#include "sync.h"

namespace i8desk {

	class ZThread
	{
	public:
		ZThread() : m_hThread(0), m_uiId(0)
		{
		}

		~ZThread() 
		{
			assert(m_hThread == 0);
		}

		int Open(void)
		{
			if (int err = Init())
				return err;
			m_hThread = (HANDLE)::_beginthreadex(0, 0, run, this, 0, &m_uiId);
			return m_hThread ? 0 : GetLastError();
		}

		void Close(void)
		{
			Fini();
			::WaitForSingleObject(m_hThread, INFINITE);
			CloseHandle(m_hThread);
			m_hThread = 0;
		}

		unsigned int GetThreadId(void) const { return m_uiId; }

	protected:
		virtual int Init(void) = 0;
		virtual void Fini(void) = 0;
		virtual unsigned int Exec(void) = 0;

	private:

		HANDLE m_hThread;
		unsigned int m_uiId;

		static unsigned int __stdcall run(void *p)
		{
			ZThread *pThis = reinterpret_cast<ZThread *>(p);
			return pThis->Exec();
		}
	};

	class ZTaskBase : protected ZThread
	{
	public:
		ZTaskBase() : m_bDone(false)
		{
			m_hEvent = ::CreateEvent(0, FALSE, TRUE, 0);
		}
		virtual ~ZTaskBase()
		{
			/*if (!IsDone())
				Shutdown();*/

			if (m_hEvent) {
				::CloseHandle(m_hEvent);
				m_hEvent = 0;
			}
		}
		int Startup(void) 
		{
			int err = Open(); 
			if (err != 0)
				SetDone();
			return err;
		}
		void Shutdown(void)
		{
			if (!IsDone())
			{
				SetDone();
				SetEvent();
				Close();
			}
		}

	protected:
		void SetEvent(void)			{ ::SetEvent(m_hEvent); }
		DWORD WaitEvent(DWORD to)	{ return ::WaitForSingleObject(m_hEvent, to); }
		void SetDone(void)			{ m_bDone = true; }
		bool IsDone(void) const		{ return m_bDone; }

	private:
		bool m_bDone;
		HANDLE m_hEvent;
	};

	template<typename T, typename L = ZNullLock>
	class ZMessageQueue
	{
	public:
		enum
		{
			ACTIVATED	= 0,
			DEACTIVATED = 1,
			PULSED		= 2,
			TIMEOUT		= 3,
			FAILED		= 4,
		};

		ZMessageQueue(int max_q = 4096)
			: cur_count_ (0)
			, cur_thrs_(0)
			, cur_q_(0)
			, max_q_(max_q)
			, completion_port_ (INVALID_HANDLE_VALUE)
		{
			q_ = new Q_TYPE[max_q_];
			completion_port_ = ::CreateIoCompletionPort (
				INVALID_HANDLE_VALUE,
				0,
				ACTIVATED,
				2);
		}

		~ZMessageQueue (void)
		{
			::CloseHandle (completion_port_);
			delete[] q_;
		}

		bool empty (void)
		{
			I8_GUARD (ZGuard, L, lock_);
			return cur_count_ > 0;
		}

		size_t size (void)
		{
			I8_GUARD (ZGuard, L, lock_);
			return cur_count_;
		}

		int enqueue (T new_item)
		{
			I8_GUARD (ZGuard, L, lock_);

			if (state_ != DEACTIVATED)
			{
				Q_TYPE *q = q_ + cur_q_;
				q->m = new_item;
				DWORD msize = 0;
				ULONG_PTR state_to_post = ACTIVATED;
				if (::PostQueuedCompletionStatus (
					completion_port_,
					msize,
					state_to_post,
					reinterpret_cast<LPOVERLAPPED> (q)))
				{
					cur_q_ = ++cur_q_ % max_q_;
					++cur_count_;
				}
			}
			return state_;
		}

		int dequeue (T &first_item, int timeout = -1)
		{
			{
				I8_GUARD (ZGuard, L, lock_);
				if (state_ == DEACTIVATED)
					return state_;
				++cur_thrs_;       
			}

			DWORD msize;
			ULONG_PTR queue_state;
			Q_TYPE *q;
			int retv = ::GetQueuedCompletionStatus (
				completion_port_,
				&msize,
				&queue_state,
				reinterpret_cast<LPOVERLAPPED *> (&q),
				(timeout == -1 ? INFINITE : timeout));
			
			I8_GUARD (ZGuard, L, lock_);
			--cur_thrs_;       
			if (retv) {
				switch (queue_state) {
				case ACTIVATED:	{                     
					--cur_count_;
					first_item = q->m;
					return ACTIVATED; }
				case PULSED: {
					return PULSED; }
				case DEACTIVATED: {
					return DEACTIVATED; }
				default: 
					assert(0);
					break;		
				}
			} else if (::GetLastError() == WAIT_TIMEOUT) {
				return TIMEOUT;
			}
			return FAILED;
		}

		int deactivate (void)
		{
			I8_GUARD (ZGuard, L, lock_);

			int const previous_state = state_;
			if (previous_state != DEACTIVATED)
			{
				state_ = DEACTIVATED;

				int cntr = cur_thrs_ - cur_count_;
				while (cntr-- > 0)
					::PostQueuedCompletionStatus (completion_port_,
					0,
					state_,
					0);
			}
			return previous_state;
		}

		int activate (void)
		{
			I8_GUARD (ZGuard, L, lock_);
			int const previous_status = state_;
			state_ = ACTIVATED;
			return previous_status;
		}

		int pulse (void)
		{
			I8_GUARD (ZGuard, L, lock_);

			int const previous_state = state_;
			if (previous_state != DEACTIVATED)
			{
				state_ = PULSED;

				int cntr = cur_thrs_ - cur_count_;
				while (cntr-- > 0)
					::PostQueuedCompletionStatus (completion_port_,
					0,
					state_,
					0);
			}
			return previous_state;
		}

	private:
		int state_;
		int cur_count_;
		int cur_thrs_;
		L lock_;
		HANDLE completion_port_;

		template<typename M>
		struct Elem {
			M m;
		};

		typedef Elem<T> Q_TYPE;
		Q_TYPE *q_;
		int cur_q_;
		int max_q_;
	};
	
} //namespace i8desk

#endif 