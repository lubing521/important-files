#ifndef GAMEDISK_H
#define GAMEDISK_H

#include "thread.h"
#include "i8type.h"
#include "irtdatasvr.h"
#include "sync.h"

#include <set>

namespace i8desk {


class ZGameDisk :
	public ZTaskBase
	, public IGameEventHandler
{
public:
	ZGameDisk(IRTDataSvr *pRTDataSvr);
	~ZGameDisk(void);

//IGameEventHandler
	virtual uint32 GetEvents(void);
	virtual void HandleEvent(uint32 optype, uint32 gid, uint64 mask);

//
	void UpdateStatus(void);
	void UpdateReadDataRate() { this->SetEvent(); }

protected:
	virtual int Init(void);
	virtual void Fini(void);
	virtual unsigned int Exec(void);

private:
	void GetPartitions(std::set<char>& partitions);
	void InsertPartition(char partition);
	
	IGameTable *m_pGameTable;
	IDiskStatusTable *m_pDiskStatusTable;

	db::tDiskStatus m_DiskStatus;

	ZSync m_lock;
	std::set<char> m_partitions;
};


}


#endif //GAMEDISK_H

