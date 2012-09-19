#include "StdAfx.h"
#include "gamedisk.h"
#include "i8logmsg.h"
#include "cpu.h"

namespace i8desk {

ZGameDisk::ZGameDisk(IRTDataSvr *pRTDataSvr)
{
	m_pGameTable = pRTDataSvr->GetGameTable();
	m_pDiskStatusTable = pRTDataSvr->GetDiskStatusTable();
}

ZGameDisk::~ZGameDisk(void)
{
}

uint32 ZGameDisk::GetEvents(void)
{
	return SQL_OP_INSERT | SQL_OP_UPDATE;
}

void ZGameDisk::HandleEvent(uint32 optype, uint32 gid, uint64 mask)
{
	if ((mask & MASK_TGAME_SVRPATH) == 0)
		return;

	db::tGame d;
	if (m_pGameTable->GetData(gid, &d) != 0)
		return;

	if (d.Status == 1 && !STRING_ISEMPTY(d.SvrPath))
		InsertPartition(d.SvrPath[0]);
}

int ZGameDisk::Init(void)
{
	//获取计算名
	DWORD nSize = sizeof(m_DiskStatus.HostName);
	if (!i8desk::GetServerName(m_DiskStatus.HostName, &nSize))
		I8_ERROR_RETURN((LM_ERROR, I8_TEXT("获取计算名失败")), -1);
	m_DiskStatus.DeadCounter = 0;

	//监视游戏服务端路径变化
	m_pGameTable->RegisterEventHandler(this);

	//初始化游戏盘列表
	IGameRecordset *pRecordset = 0;
	m_pGameTable->Select(&pRecordset, 0, 0);
	for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
		db::tGame *d = pRecordset->GetData(i);
		if (d->Status == 1 && !STRING_ISEMPTY(d->SvrPath))
			InsertPartition(d->SvrPath[0]);
	}
	pRecordset->Release();

	UpdateStatus();

	return 0;
}


void ZGameDisk::Fini(void)
{
	m_pGameTable->UnregisterEventHandler(this);
}


void ZGameDisk::GetPartitions(std::set<char>& partitions)
{
	I8_GUARD(ZGuard, ZSync, m_lock);
	partitions = m_partitions;
}


void ZGameDisk::InsertPartition(char partition) 
{
	I8_GUARD(ZGuard, ZSync, m_lock);
	m_partitions.insert((char)::toupper(partition));
}


void ZGameDisk::UpdateStatus()
{
	std::set<char> partitions;
	GetPartitions(partitions);

	std::set<char>::iterator it = partitions.begin();
	for (; it != partitions.end(); ++it) {
		db::tDiskStatus d = {0};
		SAFE_STRCPY(d.HostName, m_DiskStatus.HostName);
		if (!i8desk_GetDriverInfo(*it, d.Type, 
			d.Capacity, d.UsedSize, d.FreeSize)) 
		{
			continue;
		}
		d.Partition = *it;	
		m_pDiskStatusTable->Ensure(&d, 			
			MASK_TDISKSTATUS_TYPE
			| MASK_TDISKSTATUS_CAPACITY
			| MASK_TDISKSTATUS_USEDSIZE
			| MASK_TDISKSTATUS_FREESIZE
			| MASK_TDISKSTATUS_DEADCOUNTER);
	}
}


unsigned int ZGameDisk::Exec(void)
{
	while (!IsDone()) {

		IDiskStatusRecordset *pRecordset = 0;
		IDiskStatusFilter *pFilter = 0;
		m_pDiskStatusTable->Select(&pRecordset, pFilter, 0);
		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tDiskStatus d = *pRecordset->GetData(i);
			d.ReadDataRate = GetPartitionSpeed(d.Partition);
			m_pDiskStatusTable->Update(d.HostName, d.Partition, 
				&d, MASK_TDISKSTATUS_READDATARATE);
		}
		pRecordset->Release();

		this->WaitEvent(INFINITE);
	}

	return 0;
}



}

