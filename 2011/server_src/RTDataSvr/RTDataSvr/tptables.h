/**
 * tptables.h 运行时临时表，不需要持久化的表
 */
#ifndef I8_TPTABLES_H
#define I8_TPTABLES_H

#include "table_T.h"

#include "../../../include/irtdatasvr.h"
#include "../../../include/Extend STL/StringAlgorithm.h"
#include "../../../include/CustomFileDLL.h"
#include "../../../include/tablestruct.h"

namespace i8desk {

	class ZTaskStatusTable 
		: public ZTable_T<
		db::tTaskStatus, 
		uint32,
		ITaskStatusTable, 
		ITaskStatusEventHandler, 
		ITaskStatusRecordset, 
		ITaskStatusFilter, 
		ITaskStatusSorter, 
		ZSync,
		ZTaskStatusTable
		>
	{
		DECLARE_TABLE(ZTaskStatusTable, tblTaskStatus)
	public:
		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode == SQL_COND_MODE_EQUAL) {
				switch (CondMask) {
	case MASK_TTASKSTATUS_GID:
		return dwParam == d->GID;
	default:
		assert(0);
		break;
				}
			} 
			else {
				assert(0);
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
	case MASK_TTASKSTATUS_GID:
		return (d1->GID > d2->GID) == desc;
	default:
		assert(0);
		break;
			}
			return false;
		}

	protected:
		bool InvalidRecord(const Tty& d)
		{
			return d.GID == 0;
		}

		Kty GetKey(const Tty& d) 
		{
			return d.GID;
		}

		uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
		{
			BEGIN_UPDATE();

			UPDATE_FIELD(mask, MASK_TTASKSTATUS_GID,				realmask,	data->GID,				d->GID);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_STATE,				realmask,	data->State,			d->State);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_TASKTYPE,			realmask,	data->TaskType,			d->TaskType);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_CONNECT,			realmask,	data->Connect,			d->Connect);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_DOWNLOADSTATE,		realmask,	data->DownloadState,	d->DownloadState);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_BYTESTRANSFERRED,	realmask,	data->BytesTransferred,	d->BytesTransferred);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_PROGRESS,			realmask,	data->Progress,			d->Progress);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_TIMEELAPSED,		realmask,	data->TimeElapsed,		d->TimeElapsed);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_TIMELEFT,			realmask,	data->TimeLeft,			d->TimeLeft);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_TRANSFERRATE,		realmask,	data->TransferRate,		d->TransferRate);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_BYTESTOTAL,			realmask,	data->BytesTotal,		d->BytesTotal);
			UPDATE_FIELD(mask, MASK_TTASKSTATUS_UPDATESIZE,			realmask,	data->UpdateSize,		d->UpdateSize);

			END_UPDATE();
		}

		uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
		{
			BEGIN_CUMULATE();

			assert(0);

			END_CUMULATE();
		}

		void FillRecord(Tty& d, const _RecordsetPtr& rst)
		{
			assert(0);	
		}
	};

	class ZUpdateGameStatusTable 
		: public ZTable_T<
		db::tUpdateGameStatus, 
		uint32,
		IUpdateGameStatusTable, 
		IUpdateGameStatusEventHandler, 
		IUpdateGameStatusRecordset, 
		IUpdateGameStatusFilter, 
		IUpdateGameStatusSorter, 
		ZSync,
		ZUpdateGameStatusTable
		>
	{
		DECLARE_TABLE(ZUpdateGameStatusTable, tblUpdateGameStatus)
	public:
		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode == SQL_COND_MODE_EQUAL) {
				switch (CondMask) {
	case MASK_TUPDATEGAMESTATUS_CLIENTIP:
		return dwParam == d->ClientIP;
	default:
		assert(0);
		break;
				}
			} 
			else {
				assert(0);
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
	case MASK_TUPDATEGAMESTATUS_CLIENTIP:
		return (d1->ClientIP > d2->ClientIP) == desc;
	default:
		assert(0);
		break;
			}
			return false;
		}

	protected:
		bool InvalidRecord(const Tty& d)
		{
			return d.ClientIP == 0;
		}

		Kty GetKey(const Tty& d) 
		{
			return d.ClientIP;
		}

		uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
		{
			BEGIN_UPDATE();

			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_CLIENTIP,		realmask,	data->ClientIP,		d->ClientIP);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_GID,			realmask,	data->GID,			d->GID);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_GAMENAME,		realmask,	data->GameName,		d->GameName);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_TID		,	realmask,	data->TID,			d->TID);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_TYPE,			realmask,	data->Type,			d->Type);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_UPDATESTATE,	realmask,	data->UpdateState,	d->UpdateState);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_SIZELEFT,		realmask,	data->SizeLeft,		d->SizeLeft);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_TRANSFERRATE,	realmask,	data->TransferRate,	d->TransferRate);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_UPDATESIZE,	realmask,	data->UpdateSize,	d->UpdateSize);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_SVRDIR,		realmask,	data->SvrDir,		d->SvrDir);
			UPDATE_FIELD(mask, MASK_TUPDATEGAMESTATUS_NODEDIR,		realmask,	data->NodeDir,		d->NodeDir);

			END_UPDATE();
		}

		uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
		{
			BEGIN_CUMULATE();

			assert(0);

			END_CUMULATE();
		}

		void FillRecord(Tty& d, const _RecordsetPtr& rst)
		{
			assert(0);	
		}
	};

	class ZPlugToolStatusTable 
		: public ZTable_T<
		db::tPlugToolStatus, 
		uint32,
		IPlugToolStatusTable, 
		IPlugToolStatusEventHandler, 
		IPlugToolStatusRecordset, 
		IPlugToolStatusFilter, 
		IPlugToolStatusSorter, 
		ZSync,
		ZPlugToolStatusTable
		>
	{
		DECLARE_TABLE(ZPlugToolStatusTable, tblPlugToolStatus)
	public:
		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode == SQL_COND_MODE_EQUAL) {
				switch (CondMask) {
	case MASK_TPLUGTOOLSTATUS_PID:
		return dwParam == d->PID;
	default:
		assert(0);
		break;
				}
			} 
			else {
				assert(0);
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
	case MASK_TPLUGTOOLSTATUS_PID:
		return (d1->PID > d2->PID) == desc;
	default:
		assert(0);
		break;
			}
			return false;
		}

	protected:
		bool InvalidRecord(const Tty& d)
		{
			return d.PID == 0;
		}

		Kty GetKey(const Tty& d) 
		{
			return d.PID;
		}

		uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
		{
			BEGIN_UPDATE();

			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_PID,				realmask,	data->PID,				d->PID);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_CID,				realmask,	data->CID,				d->CID);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_SIZE,				realmask,	data->Size,				d->Size);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_TRANSFERRATE,		realmask,	data->TransferRate,		d->TransferRate);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_AVGTRANSFERRATE,	realmask,	data->AvgTransferRate,	d->AvgTransferRate);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_TOTALTIME,			realmask,	data->TotalTime,		d->TotalTime);
			UPDATE_FIELD(mask, MASK_TPLUGTOOLSTATUS_TIMELEFT,			realmask,	data->TimeLeft,			d->TimeLeft);

			END_UPDATE();
		}

		uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
		{
			BEGIN_CUMULATE();

			assert(0);

			END_CUMULATE();
		}

		void FillRecord(Tty& d, const _RecordsetPtr& rst)
		{
			assert(0);	
		}
	};

	class ZGameExVersionTable 
		: public ZTable_T<
		db::tGameExVersion, 
		uint32,
		IGameExVersionTable, 
		IGameExVersionEventHandler, 
		IGameExVersionRecordset, 
		IGameExVersionFilter, 
		IGameExVersionSorter, 
		ZSync,
		ZGameExVersionTable
		>
	{
		DECLARE_TABLE(ZGameExVersionTable, tblGameExVersion)
	public:
		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode == SQL_COND_MODE_EQUAL) {
				switch (CondMask) {
	case MASK_TGAMEEXVERSION_GID:
		return dwParam == d->GID;
	default:
		assert(0);
		break;
				}
			} 
			else {
				assert(0);
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
	case MASK_TGAMEEXVERSION_GID:
		return (d1->GID > d2->GID) == desc;
	default:
		assert(0);
		break;
			}
			return false;
		}

	protected:
		bool InvalidRecord(const Tty& d)
		{
			return d.GID == 0;
		}

		Kty GetKey(const Tty& d) 
		{
			return d.GID;
		}

		uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
		{
			BEGIN_UPDATE();

			UPDATE_FIELD(mask, MASK_TGAMEEXVERSION_GID,				realmask,	data->GID,				d->GID);
			UPDATE_FIELD(mask, MASK_TGAMEEXVERSION_GAMEEXVER,		realmask,	data->GameExVer,		d->GameExVer);
			UPDATE_FIELD(mask, MASK_TGAMEEXVERSION_COMMENTVER,		realmask,	data->CommentVer,		d->CommentVer);
			END_UPDATE();
		}

		uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
		{
			BEGIN_CUMULATE();

			assert(0);

			END_CUMULATE();
		}

		void FillRecord(Tty& d, const _RecordsetPtr& rst)
		{
			assert(0);	
		}
	};

//tVDiskClient
BEGIN_DECLARE_TABLE_CLASS(VDiskClient, StringUint32Key)

BEGIN_SQLCOND_IMPLMENT()
	SQLCOND_STRING_FIELD(VDISKCLIENT, VID, VID)
	SQLCOND_VALUE_FIELD(VDISKCLIENT, CLIENTIP, ClientIP)
	SQLCOND_VALUE_FIELD(VDISKCLIENT, VDISKIP, VDiskIP)

END_SQLCOND_IMPLMENT()

BEGIN_SQLSORT_IMPLMENT()
	SQLSORT_STRING_FIELD(VDISKCLIENT, VID, VID)
	SQLSORT_VALUE_FIELD(VDISKCLIENT, CLIENTIP, ClientIP)
END_SQLSORT_IMPLMENT()

INVALIDRECORD_IMPLMENT(VID, STRING)
GETKEY_K1_K2_IMPLMENT(VID, ClientIP)

BEGIN_UPDATE_IMPLMENT()
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_VDISKIP,		realmask,	data->VDiskIP,				d->VDiskIP);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_READCOUNT,		realmask,	data->ReadCount,			d->ReadCount);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_READSPEED,		realmask,	data->ReadSpeed,			d->ReadSpeed);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_READMAX,		realmask,	data->ReadMax,				d->ReadMax);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_CONNECTTIME,	realmask,	data->ConnectTime,			d->ConnectTime);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_CACHESHOOTING,	realmask,	data->CacheShooting,		d->CacheShooting);
	UPDATE_FIELD(mask, MASK_TVDISKCLIENT_ONLINETIME,	realmask,	data->OnlineTime,			d->OnlineTime);

END_UPDATE_IMPLMENT()

BEGIN_CUMULATE_IMPLMENT()
	assert(0);	
END_CUMULATE_IMPLMENT()

BEGIN_FILLRECORD_IMPLMENT()
	assert(0);	
END_FILLRECORD_IMPLMENT()

INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()


//tVDiskStatus
BEGIN_DECLARE_TABLE_CLASS(VDiskStatus, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(VDISKSTATUS, VID, VID)
		SQLCOND_VALUE_FIELD(VDISKSTATUS, VDISKIP, VDiskIP)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(VDISKSTATUS, VID, VID)
		SQLSORT_VALUE_FIELD(VDISKSTATUS, VDISKIP, VDiskIP)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(VID, STRING)
	GETKEY_K1_K2_IMPLMENT(VID, VDiskIP)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TVDISKSTATUS_VDISKIP,		realmask,	data->VDiskIP,		d->VDiskIP);
		UPDATE_FIELD(mask, MASK_TVDISKSTATUS_VERSION,		realmask,	data->Version,		d->Version);
		UPDATE_FIELD(mask, MASK_TVDISKSTATUS_CLIENTCOUNT,	realmask,	data->ClientCount,	d->ClientCount);
		UPDATE_FIELD(mask, MASK_TVDISKSTATUS_HOSTNAME,		realmask,	data->HostName,		d->HostName);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		assert(0);	
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		assert(0);	
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
		UPDATE_FIELD(mask, MASK_TMODULEUSAGE_CLICKCOUNT,	realmask,	data->ClickCount,	d->ClickCount);
		UPDATE_FIELD(mask, MASK_TMODULEUSAGE_USEDTIME,		realmask,	data->UsedTime,		d->UsedTime);
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


BEGIN_DECLARE_TABLE_CLASS(Module2Usage, const TCHAR *)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(MODULE2USAGE, MAC, MAC)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(MODULE2USAGE, MAC, MAC)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(MAC, STRING)
	GETKEY_IMPLMENT(MAC)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TMODULE2USAGE_MID,			realmask,	data->MID,			d->MID);
		UPDATE_FIELD(mask, MASK_TMODULE2USAGE_CLICKCOUNT,	realmask,	data->ClickCount,	d->ClickCount);
		UPDATE_FIELD(mask, MASK_TMODULE2USAGE_USEDTIME,		realmask,	data->UsedTime,		d->UsedTime);
		UPDATE_FIELD(mask, MASK_TMODULE2USAGE_NAME,			realmask,	data->Name,			d->Name);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		CUMULATE_FIELD(MODULE2USAGE, MID,			MID,		VALUE);
		CUMULATE_FIELD(MODULE2USAGE, CLICKCOUNT,	ClickCount,	VALUE);
		CUMULATE_FIELD(MODULE2USAGE, USEDTIME,	UsedTime,	VALUE);
		CUMULATE_FIELD(MODULE2USAGE, NAME,		Name,		STRING);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.MAC,		_T("MAC")			, rst);		
		GET_INT_FIELD(d.MID,		_T("MID")			, rst);		
		GET_INT_FIELD(d.ClickCount,	_T("ClickCount")	, rst);		
		GET_INT_FIELD(d.UsedTime,	_T("UsedTime")		, rst);
		GET_STR_FIELD(d.Name,		_T("Name")			, rst);
	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()



//tServerStatus
BEGIN_DECLARE_TABLE_CLASS(ServerStatus, const TCHAR *)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(SERVERSTATUS, SVRID,		SvrID)
		SQLCOND_STRING_FIELD(SERVERSTATUS, SVRNAME, SvrName)
		SQLCOND_VALUE_FIELD(SERVERSTATUS, I8DESKSVR, I8DeskSvr)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(SERVERSTATUS,	SVRID,		SvrID)
		SQLSORT_STRING_FIELD(SERVERSTATUS, SVRNAME,		SvrName)
		SQLSORT_VALUE_FIELD(SERVERSTATUS, I8DESKSVR, I8DeskSvr)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SvrID, STRING)
	GETKEY_IMPLMENT(SvrID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_SVRNAME,			realmask,	data->SvrName,			d->SvrName);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_I8DESKSVR,		realmask,	data->I8DeskSvr,		d->I8DeskSvr);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_I8VDISKSVR,		realmask,	data->I8VDiskSvr,		d->I8VDiskSvr);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_I8UPDATESVR,		realmask,	data->I8UpdateSvr,		d->I8UpdateSvr);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_DNASERVICE,		realmask,	data->DNAService,		d->DNAService);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_I8MALLCASHIER,	realmask,	data->I8MallCashier,	d->I8MallCashier);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_LASTBACKUPDBTIME,	realmask,	data->LastBackupDBTime,	d->LastBackupDBTime);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_CPUUTILIZATION,	realmask,	data->CPUUtilization,	d->CPUUtilization);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_MEMORYUSAGE,		realmask,	data->MemoryUsage,		d->MemoryUsage);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_DEADCOUNTER,		realmask,	data->DeadCounter,		d->DeadCounter);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_REMAINMEMORY,		realmask,	data->RemainMemory,		d->RemainMemory);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_NETWORKSENDRATE,	realmask,	data->NetworkSendRate,	d->NetworkSendRate);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_NETWORKRECVRATE,	realmask,	data->NetworkRecvRate,	d->NetworkRecvRate);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_DEVICEINFO,		realmask,	data->DeviceInfo,		d->DeviceInfo);
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_SVRTYPE,			realmask,	data->SvrType,			d->SvrType);

	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TSERVERSTATUS_I8VDISKSVR,	realmask,	data->I8VDiskSvr,		d->I8VDiskSvr);
		CUMULATE_FIELD(SERVERSTATUS, DEADCOUNTER,	DeadCounter,	VALUE);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		assert(0);
	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()

//tDiskStatus
BEGIN_DECLARE_TABLE_CLASS(DiskStatus, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(DISKSTATUS, SVRID, SvrID)
		SQLCOND_VALUE_FIELD(DISKSTATUS, PARTITION, Partition)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(DISKSTATUS, SVRID, SvrID)
		SQLSORT_VALUE_FIELD(DISKSTATUS, PARTITION, Partition)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SvrID, STRING)
	GETKEY_K1_K2_IMPLMENT(SvrID, Partition)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_SVRNAME,		realmask,	data->SvrName,		d->SvrName);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_TYPE,			realmask,	data->Type,			d->Type);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_CAPACITY,		realmask,	data->Capacity,		d->Capacity);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_USEDSIZE,		realmask,	data->UsedSize,		d->UsedSize);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_FREESIZE,		realmask,	data->FreeSize,		d->FreeSize);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_DEADCOUNTER,	realmask,	data->DeadCounter,	d->DeadCounter);
		UPDATE_FIELD(mask, MASK_TDISKSTATUS_READDATARATE,	realmask,	data->ReadDataRate,	d->ReadDataRate);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		CUMULATE_FIELD(DISKSTATUS, DEADCOUNTER,	DeadCounter,	VALUE);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		assert(0);	
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()



//tSyncTaskStatus
BEGIN_DECLARE_TABLE_CLASS(SyncTaskStatus, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(SYNCTASKSTATUS, SID, SID)
		SQLCOND_VALUE_FIELD(SYNCTASKSTATUS, GID, GID)
		SQLCOND_STRING_FIELD(SYNCTASKSTATUS, NAMEID, NameID)
		SQLCOND_VALUE_FIELD(SYNCTASKSTATUS, UPDATESTATE, UpdateState)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(SYNCTASKSTATUS, SID, SID)
		SQLSORT_VALUE_FIELD(SYNCTASKSTATUS, GID, GID)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SID, STRING)
	GETKEY_K1_K2_IMPLMENT(SID, GID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_NAMEID,			realmask,	data->NameID,				d->NameID);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_GAMENAME,		realmask,	data->GameName,				d->GameName);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_SVRDIR,			realmask,	data->SvrDir,				d->SvrDir);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_NODEDIR,		realmask,	data->NodeDir,				d->NodeDir);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_UPDATESTATE,	realmask,	data->UpdateState,			d->UpdateState);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_TRANSFERRATE,	realmask,	data->TransferRate,			d->TransferRate);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_BYTESTOTAL,		realmask,	data->BytesTotal,			d->BytesTotal);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_UPDATESIZE,		realmask,	data->UpdateSize,			d->UpdateSize);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_BYTESTRANSFERRED,realmask,	data->BytesTransferred,		d->BytesTransferred);
		UPDATE_FIELD(mask, MASK_TSYNCTASKSTATUS_ERROR,			realmask,	data->Error,				d->Error);

	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		assert(0);	
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		assert(0);	
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()



} //namespace i8desk


#endif //I8_TPTABLES_H
