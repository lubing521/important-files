#ifndef CLIENTSOFTMONITOR_H
#define CLIENTSOFTMONITOR_H

#include "thread.h"
#include "sync.h"
#include <vector>

namespace i8desk {

class ZFileCacheMgr;

class ZClientSoftMonitor :
	public ZTaskBase
{
public:
	ZClientSoftMonitor(ZFileCacheMgr *pFileCacheMgr);
	~ZClientSoftMonitor(void);

	BOOL MakeIdx(BOOL *pbReloadCacheFile = FALSE);
	void UpdateVersion(DWORD dwVersion);
	ulong GetVersion(void);

protected:
	int Init(void);
	void Fini(void);
	uint32 Exec(void);
	void SetFileListbySort(LPCTSTR lpPath, std::vector<stdex::tString> &vFileList, std::vector<ULONGLONG> &vSizeList, std::vector<stdex::tString> &sortfile);
	BOOL GetFileListbySort(LPCTSTR lpPath, std::vector<stdex::tString> &vFileList, std::vector<ULONGLONG> &vSizeList, std::vector<stdex::tString> &sortfile);
	BOOL IsPriority( const stdex::tString &filename, const std::vector<stdex::tString> &sortfile);

private:
	
	BOOL GetOldIdxInfo(DWORD& dwCrc, DWORD& dwVersion);

	ZSync m_lock;
	ulong m_dwVersion;

	ZSync m_MakeLock;

	ZFileCacheMgr *m_pFileCacheMgr;
};


} //namespace i8desk

#endif // CLIENTSOFTMONITOR_H
