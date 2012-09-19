#ifndef CLIENTSOFTMONITOR_H
#define CLIENTSOFTMONITOR_H

#include "../../include/thread.h"
#include "../../include/sync.h"

namespace i8desk {

class ZFileCacheMgr;

class ZClientSoftMonitor :
	public ZTaskBase
{
public:
	ZClientSoftMonitor(ZFileCacheMgr *pFileCacheMgr);
	~ZClientSoftMonitor(void);

	BOOL MakeIdx(BOOL *pbReloadCacheFile = 0);
	void UpdateVersion(DWORD dwVersion);
	ulong GetVersion(void);

protected:
	int Init(void);
	void Fini(void);
	unsigned int Exec(void);

private:
	
	BOOL GetOldIdxInfo(DWORD& dwCrc, DWORD& dwVersion);

	ZSync m_lock;
	ulong m_dwVersion;

	ZSync m_MakeLock;

	ZFileCacheMgr *m_pFileCacheMgr;
};


} //namespace i8desk

#endif // CLIENTSOFTMONITOR_H
