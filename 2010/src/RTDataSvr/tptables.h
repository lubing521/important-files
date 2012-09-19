/**
 * tptables.h 运行时临时表，不需要持久化的表
 */
#ifndef I8_TPTABLES_H
#define I8_TPTABLES_H

#include "table_T.h"

#include "../../include/irtdatasvr.h"

namespace i8desk {

//tIcon
class ZIconTable 
	: public ZTable_T<
		db::tIcon, 
		uint32,
		IIconTable, 
		IIconEventHandler, 
		IIconRecordset, 
		IIconFilter, 
		IIconSorter, 
		ZSync,
		ZIconTable
	>
{
	DECLARE_TABLE(ZIconTable, tIcon)
private:
	std::string m_strIconPath;
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TICON_GID:
				return dwParam == d->gid;
			default:
				I8_ASSERT(0);
				break;
			}
		} 
		else {
			I8_ASSERT(0);
			switch (CondMode) {
			case SQL_COND_MODE_EQUAL:
				break;
			default:
				break;
			}
		}
		return false;
	}

	bool SqlSort(const Tty *const d1, const Tty *const d2, uint64 mask, uint32 desc)
	{
		switch (mask) {
		case MASK_TICON_GID:
			return (d1->gid > d2->gid) == desc;
		default:
			I8_ASSERT(0);
			break;
		}
		return false;
	}

//override load save for template table
	virtual int Load(IDatabase *pDatabase)
	{
		I8_INFOR((LM_INFO, I8_TEXT("加载本地游戏图标数据...\n")));

		m_strIconPath = GetAppPath() + "Data\\Icon\\";

		_RecordsetPtr prcd;
		{
			TCHAR sql[BUFSIZ] = _T("SELECT GID FROM tGame WHERE Status  = 1");
			CAutoLock<IDatabase> lock(pDatabase);
			if(!pDatabase->ExecSql(sql, prcd)) {
				I8_ERROR_RETURN((LM_ERROR, I8_TEXT("读取本地游戏列表失败!\n")), -1);
			}
		}

		I8_GUARD(ZGuard, Lty, m_lock);

		I8MP.release(m_pRecords);
		I8MP.reclaim();
		m_nRecords = prcd->GetRecordCount() + 1;
		m_pRecords = I8MP.malloc<Tty>(m_nRecords);
		memset(m_pRecords, 0, sizeof(Tty) * m_nRecords);

		for (uint32 i = 1; i < m_nRecords; i++) {

			Tty& data = m_pRecords[i];
			if (FillRecord(data, prcd)) {
				//构建索引
				Kty key = GetKey(data);
				m_idxs.SetIdx(key, i);
				m_idxs.SetVer(key, CalBufCRC32(&data, sizeof(Tty)));
			}
			prcd->MoveNext();
		}
		m_ulVersion = ::_time32(0);

		return 0;
	}

	virtual int Load(const std::string& path) 
	{
		return _Base::Load(path);
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.gid == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.gid;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(ICON, SIZE,			size,			VALUE);
		UPDATE_FIELD(ICON, DATA,			data,			BUFFER);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();
		
		I8_ASSERT(0);

		END_CUMULATE();
	}

	bool FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		Tty data;
		GET_INT_FIELD(data.gid, _T("GID"), rst);

		char szIconFile[MAX_PATH];
		sprintf(szIconFile, "%s%u.ICO", m_strIconPath.c_str(), data.gid);

		FILE_Ptr f = fopen(szIconFile, "rb");
		if (!f) 
			return false;
		
		if (fseek(f, 0, SEEK_END))
			return false;

		data.size = (DWORD)ftell(f);
		if (data.size == 0)
			return false;

		if (data.size > sizeof(data.data)) 
			return false;
		
		fseek(f, 0, SEEK_SET);
		if (fread(data.data, data.size, 1, f) != 1)
			return false;

		d = data;
		return true;
	}
};

//tClientStatus
class ZClientStatusTable 
	: public ZTable_T<
		db::tClientStatus, 
		SOCKET,
		IClientStatusTable, 
		IClientStatusEventHandler, 
		IClientStatusRecordset, 
		IClientStatusFilter, 
		IClientStatusSorter, 
		ZSync,
		ZClientStatusTable
	>
{
	DECLARE_TABLE(ZClientStatusTable, tClientStatus)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCLIENTSTATUS_IP:
				return dwParam == d->IP;
			case MASK_TCLIENTSTATUS_ONLINE:
				return dwParam == d->Online;
			case MASK_TCLIENTSTATUS_NAME:
				return strcmp(szParam, d->Name) == 0;
			default:
				I8_ASSERT(0);
				break;
			}
		} 
		else {
			I8_ASSERT(0);
			switch (CondMode) {
			case SQL_COND_MODE_EQUAL:
				break;
			default:
				break;
			}
		}
		return false;
	}

	bool SqlSort(const Tty *const d1, const Tty *const d2, uint64 mask, uint32 desc)
	{
		switch (mask) {
		case MASK_TCLIENTSTATUS_IP:
			return (d1->IP > d2->IP) == desc;
		case MASK_TCLIENTSTATUS_ONLINE:
			return (d1->Online > d2->Online) == desc;
		case MASK_TCLIENTSTATUS_NAME:
			return (strcmp(d1->Name, d2->Name) > 0) == desc;
		default:
			I8_ASSERT(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.sck == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.sck;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(CLIENTSTATUS, IP,				IP,				VALUE);
		UPDATE_FIELD(CLIENTSTATUS, ONLINE,			Online,			VALUE);
		UPDATE_FIELD(CLIENTSTATUS, NAME,			Name,			STRING);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		I8_ASSERT(0);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		I8_ASSERT(0);	
	}
};

//tVDiskStatus
BEGIN_DECLARE_TABLE_CLASS(VDiskStatus, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(VDISKSTATUS, VID, VID)
		SQLCOND_VALUE_FIELD(VDISKSTATUS, IP, IP)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(VDISKSTATUS, VID, VID)
		SQLSORT_VALUE_FIELD(VDISKSTATUS, IP, IP)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(VID, STRING)
	GETKEY_K1_K2_IMPLMENT(VID, IP)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(VDISKSTATUS, IP,			IP,				VALUE);
		UPDATE_FIELD(VDISKSTATUS, VERSION,		Version,		VALUE);
		UPDATE_FIELD(VDISKSTATUS, CLIENTCOUNT,	ClientCount,	VALUE);
		UPDATE_FIELD(VDISKSTATUS, HOSTNAME,		HostName,		STRING);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		I8_ASSERT(0);	
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		I8_ASSERT(0);	
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()

BEGIN_DECLARE_TABLE_CLASS(ModuleUsage, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(MODULEUSAGE, MAC, MAC)
		SQLCOND_VALUE_FIELD(MODULEUSAGE, MID, MID)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(MODULEUSAGE, MAC, MAC)
		SQLSORT_VALUE_FIELD(MODULEUSAGE, MID, MID)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(MAC, STRING)
	GETKEY_K1_K2_IMPLMENT(MAC, MID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(MODULEUSAGE, CLICKCOUNT,	ClickCount, VALUE);
		UPDATE_FIELD(MODULEUSAGE, USEDTIME,	UsedTime,	VALUE);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		CUMULATE_FIELD(MODULEUSAGE, CLICKCOUNT,	ClickCount,	VALUE);
		CUMULATE_FIELD(MODULEUSAGE, USEDTIME,	UsedTime,	VALUE);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.Name,		_T("Name")			, rst);		
		GET_STR_FIELD(d.MAC,		_T("MAC")			, rst);		
		GET_INT_FIELD(d.MID,		_T("MID")			, rst);		
		GET_INT_FIELD(d.ClickCount,	_T("ClickCount")	, rst);		
		GET_INT_FIELD(d.UsedTime,	_T("UsedTime")		, rst);		
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()


//tServerStatus
BEGIN_DECLARE_TABLE_CLASS(ServerStatus, const char *)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(SERVERSTATUS, HOSTNAME, HostName)
		SQLCOND_VALUE_FIELD(SERVERSTATUS, I8DESKSVR, I8DeskSvr)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(SERVERSTATUS, HOSTNAME, HostName)
		SQLSORT_VALUE_FIELD(SERVERSTATUS, I8DESKSVR, I8DeskSvr)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(HostName, STRING)
	GETKEY_IMPLMENT(HostName)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(SERVERSTATUS, I8DESKSVR,		I8DeskSvr,			VALUE);
		UPDATE_FIELD(SERVERSTATUS, I8VDISKSVR,		I8VDiskSvr,			VALUE);
		UPDATE_FIELD(SERVERSTATUS, I8UPDATESVR,		I8UpdateSvr,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, DNASERVICE,		DNAService,			VALUE);
		UPDATE_FIELD(SERVERSTATUS, I8MALLCASHIER,	I8MallCashier,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, LASTBACKUPDBTIME,LastBackupDBTime,	VALUE);
		UPDATE_FIELD(SERVERSTATUS, CPUUTILIZATION,	CPUUtilization,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, MEMORYUSAGE,		MemoryUsage,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, DEADCOUNTER,		DeadCounter,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, REMAINMEMORY,	RemainMemory,		VALUE);
		UPDATE_FIELD(SERVERSTATUS, NETWORKSENDRATE,	NetworkSendRate,	VALUE);
		UPDATE_FIELD(SERVERSTATUS, NETWORKRECVRATE,	NetworkRecvRate,	VALUE);
		UPDATE_FIELD(SERVERSTATUS, DEVICEINFO,		DeviceInfo,			STRING);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		UPDATE_FIELD(SERVERSTATUS, I8VDISKSVR,		I8VDiskSvr,		VALUE);
		CUMULATE_FIELD(SERVERSTATUS, DEADCOUNTER,	DeadCounter,	VALUE);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		I8_ASSERT(0);
	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()

//tDiskStatus
BEGIN_DECLARE_TABLE_CLASS(DiskStatus, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(DISKSTATUS, HOSTNAME, HostName)
		SQLCOND_VALUE_FIELD(DISKSTATUS, PARTITION, Partition)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(DISKSTATUS, HOSTNAME, HostName)
		SQLSORT_VALUE_FIELD(DISKSTATUS, PARTITION, Partition)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(HostName, STRING)
	GETKEY_K1_K2_IMPLMENT(HostName, Partition)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(DISKSTATUS, TYPE,			Type,			VALUE);
		UPDATE_FIELD(DISKSTATUS, CAPACITY,		Capacity,		VALUE);
		UPDATE_FIELD(DISKSTATUS, USEDSIZE,		UsedSize,		VALUE);
		UPDATE_FIELD(DISKSTATUS, FREESIZE,		FreeSize,		VALUE);
		UPDATE_FIELD(DISKSTATUS, DEADCOUNTER,	DeadCounter,	VALUE);
		UPDATE_FIELD(DISKSTATUS, READDATARATE,	ReadDataRate,	VALUE);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		CUMULATE_FIELD(DISKSTATUS, DEADCOUNTER,	DeadCounter,	VALUE);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		I8_ASSERT(0);	
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()

} //namespace i8desk


#endif //I8_TPTABLES_H
