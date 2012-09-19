#ifndef I8_SYNC_H
#define I8_SYNC_H

class ZNullLock 
{
public:
	void Create() {}
	void Lock() {}
	void UnLock() {}
};

class ZSync
{
public:
	ZSync(void) 
	{
		m_bCreated = InitializeCriticalSectionAndSpinCount(&m_lock, 0x80000400);
	}
	~ZSync(void)
	{
		if (m_bCreated) {
			DeleteCriticalSection(&m_lock);
			m_bCreated = FALSE;
		}
	}
protected:
	CRITICAL_SECTION m_lock;
	BOOL m_bCreated;
public:
	BOOL Create(void)
	{
		return m_bCreated;
	}

	void Lock(void)
	{
		EnterCriticalSection(&m_lock);
	}

	void UnLock(void)
	{
		LeaveCriticalSection(&m_lock);
	}
};

class ZThreadRecursiveMutex
{
public:
	ZThreadRecursiveMutex() { m_Mutex = ::CreateMutex(0, 0, 0); }
	~ZThreadRecursiveMutex() { ::CloseHandle(m_Mutex); m_Mutex = 0; }
	BOOL Create(void) { return TRUE; }

	void Lock() { ::WaitForSingleObject(m_Mutex, INFINITE); }
	void UnLock() { ::ReleaseMutex(m_Mutex); }

protected:
	HANDLE m_Mutex;
};

template<typename L>
class ZGuard 
{
public:
	ZGuard (L& l) : m_l(l) { m_l.Lock(); }
	~ZGuard () { m_l.UnLock(); }
private:
	L &m_l;
};

#define I8_GUARD(Gty, Lty, lock) Gty<Lty> gd(lock)

#endif 