/**
 * dbtables.h 数据库表，需要持久化并保持一致状态的数据
 */
#ifndef I8_DBTABLES_H
#define I8_DBTABLES_H

#include "table_T.h"
#include "../../../include/irtdatasvr.h"

namespace i8desk {

//tGame
class ZGameTable 
	: public ZTable_T<
		db::tGame, 
		uint32,
		IGameTable, 
		IGameEventHandler, 
		IGameRecordset, 
		IGameFilter, 
		IGameSorter,
		ZSync,
		ZGameTable
	>
{
	DECLARE_TABLE(ZGameTable, tblGame)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TGAME_NAME:
				return _tcscmp(szParam, d->Name) == 0;
			case MASK_TGAME_PID:
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
		case MASK_TGAME_NAME:
			return (_tcscmp(d1->Name, d2->Name) > 0) == desc;
		case MASK_TGAME_PID:
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
		return d.GID == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.GID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TGAME_PID,		realmask,	data->PID,			d->PID);
		UPDATE_FIELD(mask, MASK_TGAME_CID,		realmask, 	data->CID,			d->CID);
		UPDATE_FIELD(mask, MASK_TGAME_NAME,		realmask,	data->Name,			d->Name);
		UPDATE_FIELD(mask, MASK_TGAME_EXE,		realmask,	data->Exe,			d->Exe);
		UPDATE_FIELD(mask, MASK_TGAME_PARAM,	realmask,	data->Param,		d->Param);
		UPDATE_FIELD(mask, MASK_TGAME_SIZE,		realmask,	data->Size,			d->Size);
		UPDATE_FIELD(mask, MASK_TGAME_SVRPATH,	realmask,	data->SvrPath,		d->SvrPath);
		UPDATE_FIELD(mask, MASK_TGAME_CLIPATH,	realmask,	data->CliPath,		d->CliPath);
		UPDATE_FIELD(mask, MASK_TGAME_TRAITFILE,realmask,	data->TraitFile,	d->TraitFile);
		UPDATE_FIELD(mask, MASK_TGAME_SAVEFILTER,realmask,	data->SaveFilter,	d->SaveFilter);
		UPDATE_FIELD(mask, MASK_TGAME_DESKLINK,	realmask,	data->DeskLink,		d->DeskLink);
		UPDATE_FIELD(mask, MASK_TGAME_TOOLBAR,	realmask,	data->Toolbar,		d->Toolbar);
		UPDATE_FIELD(mask, MASK_TGAME_MEMO,		realmask,	data->Memo,			d->Memo);
		UPDATE_FIELD(mask, MASK_TGAME_GAMESOURCE,realmask,	data->GameSource,	d->GameSource);
		UPDATE_FIELD(mask, MASK_TGAME_IDCADDDATE,realmask,	data->IdcAddDate,	d->IdcAddDate);
		UPDATE_FIELD(mask, MASK_TGAME_ENDEL,	 realmask,	data->EnDel,		d->EnDel	);
		UPDATE_FIELD(mask, MASK_TGAME_IDCVER,	 realmask,	data->IdcVer,		d->IdcVer);
		UPDATE_FIELD(mask, MASK_TGAME_SVRVER,	 realmask,	data->SvrVer,		d->SvrVer);
		UPDATE_FIELD(mask, MASK_TGAME_IDCCLICK,	 realmask,	data->IdcClick,		d->IdcClick);
		UPDATE_FIELD(mask, MASK_TGAME_SVRCLICK,	 realmask,	data->SvrClick,		d->SvrClick);
		UPDATE_FIELD(mask, MASK_TGAME_SVRCLICK2, realmask,	data->SvrClick2,	d->SvrClick2);
		UPDATE_FIELD(mask, MASK_TGAME_I8PLAY,	realmask,	data->I8Play,		d->I8Play);
		UPDATE_FIELD(mask, MASK_TGAME_AUTOUPT,	realmask,	data->AutoUpt,		d->AutoUpt);
		UPDATE_FIELD(mask, MASK_TGAME_PRIORITY,	realmask,	data->Priority,		d->Priority);
		UPDATE_FIELD(mask, MASK_TGAME_FORCE,	realmask,	data->Force,		d->Force);
		UPDATE_FIELD(mask, MASK_TGAME_FORCEDIR,	realmask,	data->ForceDir,		d->ForceDir);
		UPDATE_FIELD(mask, MASK_TGAME_STATUS,	realmask,	data->Status,		d->Status);
		UPDATE_FIELD(mask, MASK_TGAME_STOPRUN,	realmask,	data->StopRun,		d->StopRun);
		UPDATE_FIELD(mask, MASK_TGAME_REPAIR,	realmask,	data->Repair,		d->Repair);
		UPDATE_FIELD(mask, MASK_TGAME_HIDE,		realmask,	data->Hide,			d->Hide);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(GAME, SVRCLICK,	SvrClick,		VALUE);
		CUMULATE_FIELD(GAME, SVRCLICK2,	SvrClick2,		VALUE);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_INT_FIELD(d.PID			, _T("PID")			, rst);			
		GET_INT_FIELD(d.GID			, _T("GID")			, rst);			
		GET_INT_FIELD(d.Size		, _T("Size")		, rst);			
		GET_INT_FIELD(d.DeskLink	, _T("DeskLink")	, rst);		
		GET_INT_FIELD(d.Toolbar		, _T("Toolbar")		, rst);		
		GET_INT_FIELD(d.EnDel		, _T("EnDel")		, rst);		
		GET_INT_FIELD(d.Priority	, _T("Priority")	, rst);	
		GET_INT_FIELD(d.IdcAddDate	, _T("IdcAddDate")	, rst);	
		GET_INT_FIELD(d.IdcVer		, _T("IdcVer")		, rst);		
		GET_INT_FIELD(d.SvrVer		, _T("SvrVer")		, rst);		
		GET_INT_FIELD(d.AutoUpt		, _T("AutoUpt")		, rst);	
		GET_INT_FIELD(d.I8Play		, _T("I8Play")		, rst);		
		GET_INT_FIELD(d.IdcClick	, _T("IdcClick")	, rst);	
		GET_INT_FIELD(d.SvrClick	, _T("SvrClick")	, rst);	
		GET_INT_FIELD(d.SvrClick2	, _T("SvrClick2")	, rst);	
		GET_INT_FIELD(d.Status		, _T("Status")		, rst);		
		GET_INT_FIELD(d.Force		, _T("Force")		, rst);		
		GET_INT_FIELD(d.StopRun		, _T("StopRun")		, rst);		
		GET_INT_FIELD(d.Repair		, _T("Repair")		, rst);		
		GET_INT_FIELD(d.Hide		, _T("Hide")		, rst);		

		GET_STR_FIELD(d.Name,		_T("Name")		, rst);		
		GET_STR_FIELD(d.CID,		_T("CID")		, rst);	
		GET_STR_FIELD(d.Exe,		_T("Exe")		, rst);		
		GET_STR_FIELD(d.Param,		_T("Param")		, rst);		
		GET_STR_FIELD(d.SvrPath,	_T("SvrPath")	, rst);		
		GET_STR_FIELD(d.CliPath,	_T("CliPath")	, rst);		
		GET_STR_FIELD(d.TraitFile,	_T("TraitFile")	, rst);	
		GET_STR_FIELD(d.SaveFilter,	_T("SaveFilter"), rst);	
		GET_STR_FIELD(d.Memo,		_T("Memo")		, rst);		
		GET_STR_FIELD(d.ForceDir,	_T("ForceDir")	, rst);	
		GET_STR_FIELD(d.GameSource,	_T("GameSource"), rst);	
	}
};


//tPlugTool
class ZPlugToolTable 
	: public ZTable_T<
	db::tPlugTool, 
	uint32,
	IPlugToolTable, 
	IPlugToolEventHandler, 
	IPlugToolRecordset, 
	IPlugToolFilter, 
	IPlugToolSorter,
	ZSync,
	ZPlugToolTable
	>
{
	DECLARE_TABLE(ZPlugToolTable, tblPlugTool)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TPLUGTOOL_NAME:
				return _tcscmp(szParam, d->Name) == 0;
			case MASK_TPLUGTOOL_CID:
				return _tcscmp(szParam, d->CID) == 0;
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
		case MASK_TPLUGTOOL_NAME:
			return (_tcscmp(d1->Name, d2->Name) > 0) == desc;
		case MASK_TPLUGTOOL_CID:
			return (_tcscmp(d1->Name, d2->CID) > 0) == desc;
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

		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CID,			realmask,	data->CID,				d->CID);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_IDCVER,		realmask,	data->IdcVer,			d->IdcVer);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVRVER,		realmask,	data->SvrVer,			d->SvrVer);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_PRIORITY,		realmask,	data->Priority,			d->Priority);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_NAME,			realmask,	data->Name,				d->Name);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_COMMENT,		realmask,	data->Comment,			d->Comment);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_TOOLSOURCE,	realmask,	data->ToolSource,		d->ToolSource);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SIZE,			realmask,	data->Size,				d->Size);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_IDCCLICK,		realmask,	data->IdcClick,			d->IdcClick);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_DOWNLOADTYPE,	realmask,	data->DownloadType,		d->DownloadType);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_DOWNLOADSTATUS,realmask,	data->DownloadStatus,	d->DownloadStatus);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_STATUS,		realmask,	data->Status,			d->Status);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVRNAME,		realmask,	data->SvrName,			d->SvrName);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVREXE,		realmask,	data->SvrExe,			d->SvrExe);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVRPARAM,		realmask,	data->SvrParam,			d->SvrParam);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVRPATH,		realmask,	data->SvrPath,			d->SvrPath);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_SVRRUNTYPE,	realmask,	data->SvrRunType,		d->SvrRunType);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CLINAME,		realmask,	data->CliName,			d->CliName);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CLIEXE,		realmask,	data->CliExe,			d->CliExe);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CLIPARAM,		realmask,	data->CliParam,			d->CliParam);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CLIPATH,		realmask,	data->CliPath,			d->CliPath);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CLIRUNTYPE,	realmask,	data->CliRunType,		d->CliRunType);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CONFIGEXE,	realmask,	data->ConfigExe,		d->ConfigExe);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CONFIGPARAM,	realmask,	data->ConfigParam,		d->ConfigParam);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CONFIGPATH,	realmask,	data->ConfigPath,		d->ConfigPath);
		UPDATE_FIELD(mask, MASK_TPLUGTOOL_CONFIGRUNTYPE,realmask,	data->ConfigRunType,	d->ConfigRunType);


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
		GET_INT_FIELD(d.PID				, _T("PID")				, rst);			
		GET_INT_FIELD(d.IdcVer			, _T("IdcVer")			, rst);			
		GET_INT_FIELD(d.SvrVer			, _T("SvrVer")			, rst);	
		GET_INT_FIELD(d.Priority		, _T("Priority")		, rst);			
		GET_INT_FIELD(d.Size			, _T("Size")			, rst);		
		GET_INT_FIELD(d.IdcClick		, _T("IdcClick")		, rst);		
		GET_INT_FIELD(d.DownloadType	, _T("DownloadType")	, rst);	
		GET_INT_FIELD(d.DownloadStatus	, _T("DownloadStatus")	, rst);		
		GET_INT_FIELD(d.Status			, _T("Status")			, rst);	
		GET_INT_FIELD(d.SvrRunType		, _T("SvrRunType")		, rst);	
		GET_INT_FIELD(d.CliRunType		, _T("CliRunType")		, rst);		
		GET_INT_FIELD(d.ConfigRunType	, _T("ConfigRunType")	, rst);		

		GET_STR_FIELD(d.CID,			_T("CID")		, rst);	
		GET_STR_FIELD(d.Name,			_T("Name")		, rst);		
		GET_STR_FIELD(d.Comment,		_T("Comment")	, rst);		
		GET_STR_FIELD(d.ToolSource,		_T("ToolSource"), rst);		
		GET_STR_FIELD(d.SvrName,		_T("SvrName")	, rst);		
		GET_STR_FIELD(d.SvrExe,			_T("SvrExe")	, rst);		
		GET_STR_FIELD(d.SvrParam,		_T("SvrParam")	, rst);	
		GET_STR_FIELD(d.SvrPath,		_T("SvrPath")	, rst);	
		GET_STR_FIELD(d.CliName,		_T("CliName")	, rst);		
		GET_STR_FIELD(d.CliExe,			_T("CliExe")	, rst);		
		GET_STR_FIELD(d.CliParam,		_T("CliParam")	, rst);	
		GET_STR_FIELD(d.CliPath,		_T("CliPath")	, rst);	
		GET_STR_FIELD(d.ConfigName,		_T("ConfigName")	, rst);		
		GET_STR_FIELD(d.ConfigExe,		_T("ConfigExe")		, rst);		
		GET_STR_FIELD(d.ConfigParam,	_T("ConfigParam")	, rst);	
		GET_STR_FIELD(d.ConfigPath,		_T("ConfigPath")	, rst);	

	}
};



//tClass
class ZClassTable 
	: public ZTable_T<
		db::tClass, 
		const TCHAR *,
		IClassTable, 
		IClassEventHandler, 
		IClassRecordset, 
		IClassFilter, 
		IClassSorter,
		ZSync,
		ZClassTable
	>
{
	DECLARE_TABLE(ZClassTable, tblClass)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCLASS_NAME:
				return _tcscmp(szParam, d->Name) == 0;
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
		case MASK_TCLASS_NAME:
			return (_tcscmp(d1->Name, d2->Name) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.CID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.CID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TCLASS_NAME,	realmask,	data->Name,			d->Name);
		UPDATE_FIELD(mask, MASK_TCLASS_SVRPATH,	realmask,   data->SvrPath,		d->SvrPath);

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
		GET_STR_FIELD(d.CID,		_T("CID")		, rst);		
		GET_STR_FIELD(d.Name,		_T("Name")		, rst);	
		GET_STR_FIELD(d.SvrPath,	_T("SvrPath")	, rst);		
	}
};

class ZRunTypeTable 
	: public ZTable_T<
		db::tRunType, 
		Uint32StringKey,
		IRunTypeTable, 
		IRunTypeEventHandler, 
		IRunTypeRecordset, 
		IRunTypeFilter, 
		IRunTypeSorter,
		ZSync,
		ZRunTypeTable
	>
{
	DECLARE_TABLE(ZRunTypeTable, tblRunType)
public:
	~ZRunTypeTable()
	{

	}

	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TRUNTYPE_TYPE:
				return dwParam == d->Type;
			case MASK_TRUNTYPE_GID:
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
		case MASK_TRUNTYPE_AID:
			return (_tcscmp(d1->AID, d2->AID) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

public:
//IRunTypeTable
	int Update(uint32 gid, const Tty *const d, uint64 mask) 
	{
		struct UpdateMask {
			Kty key;
			uint64 mask;
		};
		std::vector<UpdateMask> updates;

		{
			I8_GUARD(ZGuard, Lty, m_lock);

			std::vector<uint32> idxs;

			m_idxs.GetIdx(gid, idxs);
			for (size_t n = 0; n < idxs.size(); n++) {
				Tty& data = m_pRecords[idxs[n]];
				if (uint64 realmask = UpdateRecord(&data, d, mask)) {
					UpdateMask um;
					um.key = Index::CloneKey(GetKey(data));
					um.mask = realmask;
					updates.push_back(um);
				}
			}
		}

		for (size_t n = 0; n < updates.size(); n++) {
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
			Index::FreeKey(updates[n].key);
		}
		return 0;
	}

	int Update(uint32 gid, const TCHAR *aid, const Tty *const d, uint64 mask) 
	{
		Kty key = { gid, aid };
		return _Base::Update(key, d, mask);
	}

	int Cumulate(uint32 gid, const Tty *const d, uint64 mask) 
	{
		struct UpdateMask {
			Kty key;
			uint64 mask;
		};
		std::vector<UpdateMask> updates;

		{
			I8_GUARD(ZGuard, Lty, m_lock);

			std::vector<uint32> idxs;

			m_idxs.GetIdx(gid, idxs);
			for (size_t n = 0; n < idxs.size(); n++) {
				Tty& data = m_pRecords[idxs[n]];
				if (uint64 realmask = CumulateRecord(&data, d, mask)) {
					UpdateMask um;
					um.key = Index::CloneKey(GetKey(data));
					um.mask = realmask;
					updates.push_back(um);
				}
			}
		}

		for (size_t n = 0; n < updates.size(); n++) {
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
			Index::FreeKey(updates[n].key);
		}
		return 0;
	}

	int Cumulate(uint32 gid, const TCHAR *aid, const Tty *const d, uint64 mask) 
	{
		Kty key = { gid, aid };
		return _Base::Cumulate(key, d, mask);
	}

	int Delete(uint32 gid) 
	{
		std::vector<Kty> keys;
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			std::vector<uint32> idxs;

			m_idxs.GetIdx(gid, idxs);
			for (size_t n = 0; n < idxs.size(); n++) {
				Tty& data = m_pRecords[idxs[n]];
				keys.push_back(Index::CloneKey(GetKey(data)));
				memset(&data, 0, sizeof(Tty));
			}
			m_idxs.ClearIdx(gid);
		}

		for (size_t n = 0; n < keys.size(); n++) {
			HandleEvent(SQL_OP_DELETE, keys[n], 0);
			Index::FreeKey(keys[n]);
		}
		return 0;
	}

	int Delete(uint32 gid, const TCHAR *aid) 
	{
		Kty key = { gid, aid };
		return _Base::Delete(key);
	}

	int GetData(uint32 gid, const TCHAR *aid, Tty *d, ulong *ver) 
	{
		Kty key = { gid, aid };
		return _Base::GetData(key, d, ver);
	}

	int Select(Rty **recordset, uint32 gid, Sty *sorter) 
	{
		assert(recordset);

		RecordSet *pRecordset = new RecordSet(m_mp);
		{
			I8_GUARD(ZGuard, Lty, m_lock);
			
			std::vector<uint32> idxs;
			m_idxs.GetIdx(gid, idxs);
			for (size_t n = 0; n < idxs.size(); n++) {
				Tty& data = m_pRecords[idxs[n]];
				pRecordset->AddData(data, m_idxs.GetVer(GetKey(data)));
			}
		}

		if (sorter) {
			pRecordset->Sort(sorter);
		}
		*recordset = static_cast<Rty *>(pRecordset);

		return 0;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.GID == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		Kty key = { d.GID, d.AID };
		return key;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TRUNTYPE_TYPE,	realmask,	data->Type,		d->Type);
		UPDATE_FIELD(mask, MASK_TRUNTYPE_VID,	realmask,	data->VID,		d->VID);

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
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);		
		GET_INT_FIELD(d.GID,	_T("GID")	, rst);	
		GET_INT_FIELD(d.Type,	_T("Type"), rst);		
		GET_STR_FIELD(d.VID,	_T("VID")	, rst);		
	}
};


//tSysOpt
class ZSysOptTable 
	: public ZTable_T<
		db::tSysOpt, 
		const TCHAR *,
		ISysOptTable, 
		ISysOptEventHandler, 
		ISysOptRecordset, 
		ISysOptFilter, 
		ISysOptSorter,
		ZSync,
		ZSysOptTable
	>
{
	DECLARE_TABLE(ZSysOptTable, tblOption)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TSYSOPT_KEY:
				if (_tcscmp(szParam, d->key) == 0)
					return true;
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
		case MASK_TSYSOPT_KEY:
			return (_tcscmp(d1->key, d2->key) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.key[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.key;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TSYSOPT_VALUE, realmask, data->value, d->value);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(SYSOPT, VALUE, value, STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.key,	_T("OptName")	, rst);		
		GET_STR_FIELD(d.value,	_T("OptValue")	, rst);	
	}
};

//tArea
class ZAreaTable 
	: public ZTable_T<
		db::tArea, 
		const TCHAR *,
		IAreaTable, 
		IAreaEventHandler, 
		IAreaRecordset, 
		IAreaFilter, 
		IAreaSorter,
		ZSync,
		ZAreaTable
	>
{
	DECLARE_TABLE(ZAreaTable, tblArea)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TAREA_NAME:
				return _tcscmp(szParam, d->Name) == 0;
			case MASK_TAREA_SVRID:
				return _tcscmp(szParam, d->SvrID) == 0;
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
		case MASK_TAREA_NAME:
			return (_tcscmp(d1->Name, d2->Name) > 0) == desc;
		case MASK_TAREA_SVRID:
			return (_tcscmp(d1->SvrID, d2->SvrID) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.AID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.AID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TAREA_NAME,	 realmask,	data->Name,			d->Name);
		UPDATE_FIELD(mask, MASK_TAREA_SVRID, realmask,	data->SvrID,		d->SvrID);
	
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
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);		
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_STR_FIELD(d.SvrID,	_T("SvrID")	,rst);	

	}
};

//tClient
class ZClientTable 
	: public ZTable_T<
		db::tClient, 
		const TCHAR *,
		IClientTable, 
		IClientEventHandler, 
		IClientRecordset, 
		IClientFilter, 
		IClientSorter,
		ZSync,
		ZClientTable
	>
{
	DECLARE_TABLE(ZClientTable, tblClient)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCLIENT_AID:
				return _tcsicmp(szParam, d->AID) == 0;
			case MASK_TCLIENT_IP:
				return dwParam == d->IP;
			case MASK_TCLIENT_ONLINE:
				return dwParam == d->Online;
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
		case MASK_TCLIENT_NAME:
			return (_tcsicmp(d1->Name, d2->Name) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.Name[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.Name;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();
	
		UPDATE_FIELD(mask, MASK_TCLIENT_AID,			realmask, 	data->AID,				d->AID);
		UPDATE_FIELD(mask, MASK_TCLIENT_IP,				realmask, 	data->IP,				d->IP);
		UPDATE_FIELD(mask, MASK_TCLIENT_MAC,			realmask,	data->MAC,				d->MAC);
		UPDATE_FIELD(mask, MASK_TCLIENT_MARK,			realmask,	data->Mark,				d->Mark);
		UPDATE_FIELD(mask, MASK_TCLIENT_GATE,			realmask,	data->Gate,				d->Gate);
		UPDATE_FIELD(mask, MASK_TCLIENT_DNS,			realmask,	data->DNS,				d->DNS);
		UPDATE_FIELD(mask, MASK_TCLIENT_DNS2,			realmask,	data->DNS2,				d->DNS2);
		UPDATE_FIELD(mask, MASK_TCLIENT_PROTINSTALL,	realmask,	data->ProtInstall,		d->ProtInstall);
		UPDATE_FIELD(mask, MASK_TCLIENT_PARTITION,		realmask,	data->Partition,		d->Partition);
		UPDATE_FIELD(mask, MASK_TCLIENT_PROTVER,		realmask,	data->ProtVer,			d->ProtVer);
		UPDATE_FIELD(mask, MASK_TCLIENT_VDISKVER,		realmask,	data->VDiskVer,			d->VDiskVer);
		UPDATE_FIELD(mask, MASK_TCLIENT_MENUVER,		realmask,	data->MenuVer,			d->MenuVer);
		UPDATE_FIELD(mask, MASK_TCLIENT_CLISVRVER,		realmask,	data->CliSvrVer,		d->CliSvrVer);
		UPDATE_FIELD(mask, MASK_TCLIENT_SYSTEM,			realmask,	data->System,			d->System);
		UPDATE_FIELD(mask, MASK_TCLIENT_CPU,			realmask,	data->CPU,				d->CPU);
		UPDATE_FIELD(mask, MASK_TCLIENT_MAINBOARD,		realmask,	data->Mainboard,		d->Mainboard);
		UPDATE_FIELD(mask, MASK_TCLIENT_MEMORY,			realmask,	data->Memory,			d->Memory);
		UPDATE_FIELD(mask, MASK_TCLIENT_DISK,			realmask,	data->Disk,				d->Disk);
		UPDATE_FIELD(mask, MASK_TCLIENT_VIDEO,			realmask,	data->Video,			d->Video);
		UPDATE_FIELD(mask, MASK_TCLIENT_AUDIO,			realmask,	data->Audio,			d->Audio);
		UPDATE_FIELD(mask, MASK_TCLIENT_NETWORK,		realmask,	data->Network,			d->Network);
		UPDATE_FIELD(mask, MASK_TCLIENT_CAMERA,			realmask, 	data->Camera,			d->Camera);
		UPDATE_FIELD(mask, MASK_TCLIENT_ONLINE,			realmask, 	data->Online,			d->Online);
		UPDATE_FIELD(mask, MASK_TCLIENT_TEMPERATURE,	realmask,   data->Temperature,		d->Temperature);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(CLIENT, PARTITION,		Partition,		STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);	
		GET_INT_FIELD(d.IP,		_T("IP")	, rst);		
		GET_STR_FIELD(d.MAC,	_T("MAC")	, rst);	
		GET_INT_FIELD(d.Mark,	_T("Mark")	, rst);	
		GET_INT_FIELD(d.Gate,	_T("Gate")	, rst);	
		GET_INT_FIELD(d.DNS,	_T("DNS")	, rst);	
		GET_INT_FIELD(d.DNS2,	_T("DNS2")	, rst);	
		GET_INT_FIELD(d.ProtInstall, _T("ProtInstall"), rst);		
		GET_STR_FIELD(d.Partition,	_T("Partition")	, rst);	
		GET_STR_FIELD(d.ProtVer,	_T("ProtVer")	, rst);	
		GET_STR_FIELD(d.VDiskVer,	_T("VDiskVer")	, rst);	
		GET_STR_FIELD(d.MenuVer,	_T("MenuVer")	, rst);	
		GET_STR_FIELD(d.CliSvrVer,	_T("CliSvrVer")	, rst);	


		GET_STR_FIELD(d.System,		_T("System")	, rst);	
		stdex::tString val = d.System;
		utility::Strcpy(d.System, GetBase64(val).c_str());

		GET_STR_FIELD(d.CPU,		_T("CPU")		, rst);	
		stdex::tString CPU = d.CPU;
		utility::Strcpy(d.CPU, GetBase64(CPU).c_str());

		GET_STR_FIELD(d.Mainboard,	_T("Mainboard")	, rst);	
		stdex::tString Mainboard = d.Mainboard;
		utility::Strcpy(d.Mainboard, GetBase64(Mainboard).c_str());

		GET_STR_FIELD(d.Memory,		_T("Memory")	, rst);	
		stdex::tString Memory = d.Memory;
		utility::Strcpy(d.Memory, GetBase64(Memory).c_str());

		GET_STR_FIELD(d.Disk,		_T("Disk")		, rst);	
		stdex::tString Disk = d.Disk;
		utility::Strcpy(d.Disk, GetBase64(Disk).c_str());

		GET_STR_FIELD(d.Video,		_T("Video")		, rst);
		stdex::tString Video = d.Video;
		utility::Strcpy(d.Video, GetBase64(Video).c_str());

		GET_STR_FIELD(d.Audio,		_T("Audio")		, rst);	
		stdex::tString Audio = d.Audio;
		utility::Strcpy(d.Audio, GetBase64(Audio).c_str());

		GET_STR_FIELD(d.Network,	_T("Network")	, rst);	
		stdex::tString Network = d.Network;
		utility::Strcpy(d.Network, GetBase64(Network).c_str());

		GET_STR_FIELD(d.Camera,		_T("Camera")	, rst);	
		stdex::tString Camera = d.Camera;
		utility::Strcpy(d.Camera, GetBase64(Camera).c_str());
	}


	stdex::tString GetBase64(stdex::tString &val)
	{
		if(val.empty())
			return val;

		std::string src =  CT2A(val.c_str());
		int len = ATL::Base64DecodeGetRequiredLength(src.length());
		std::vector<byte> vec(len + 1);

		ATL::Base64Decode(src.data(), len, &vec[0], &len);

		stdex::tString text(CA2T((char *)&(vec[0])));

		return text;
	}
};




//tFavorite
class ZFavoriteTable 
	: public ZTable_T<
		db::tFavorite, 
		const TCHAR *,
		IFavoriteTable, 
		IFavoriteEventHandler, 
		IFavoriteRecordset, 
		IFavoriteFilter, 
		IFavoriteSorter,
		ZSync,
		ZFavoriteTable
	>
{
	DECLARE_TABLE(ZFavoriteTable, tblFavorite)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TFAVORITE_NAME:
				return _tcscmp(szParam, d->Name) == 0;
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
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.FID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.FID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TFAVORITE_TYPE,	realmask,	data->Type,		d->Type);
		UPDATE_FIELD(mask, MASK_TFAVORITE_NAME,	realmask,	data->Name,		d->Name);
		UPDATE_FIELD(mask, MASK_TFAVORITE_URL,	realmask,	data->URL,		d->URL);
		UPDATE_FIELD(mask, MASK_TFAVORITE_SERIAL,realmask,	data->Serial,	d->Serial);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(FAVORITE, URL,		URL,		STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.FID,	_T("FID")	, rst);	
		GET_INT_FIELD(d.Type,	_T("Type")	, rst);		
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_STR_FIELD(d.URL,	_T("URL")	, rst);	
		GET_INT_FIELD(d.Serial,	_T("Serial"), rst);	
	}
};


//tVDisk
class ZVDiskTable 
	: public ZTable_T<
		db::tVDisk, 
		const TCHAR *,
		IVDiskTable, 
		IVDiskEventHandler, 
		IVDiskRecordset, 
		IVDiskFilter, 
		IVDiskSorter,
		ZSync,
		ZVDiskTable
	>
{
	DECLARE_TABLE(ZVDiskTable, tblVDisk)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TVDISK_SVRID:
				return _tcscmp(szParam, d->SvrID) == 0;
			case MASK_TVDISK_TYPE:
				return dwParam == d->Type;
			case MASK_TVDISK_CLIDRV:
				return dwParam == d->CliDrv;
			case MASK_TVDISK_SVRMODE:
				return _tcscmp(szParam, d->SvrMode) == 0;
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
		case MASK_TVDISK_CLIDRV:
			return (d1->CliDrv > d2->CliDrv) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.VID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.VID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();
		UPDATE_FIELD(mask, MASK_TVDISK_SVRID,		realmask,	data->SvrID,	d->SvrID);
		UPDATE_FIELD(mask, MASK_TVDISK_SOUCIP,		realmask,	data->SoucIP,	d->SoucIP);
		UPDATE_FIELD(mask, MASK_TVDISK_PORT,		realmask,	data->Port,		d->Port);
		UPDATE_FIELD(mask, MASK_TVDISK_SVRDRV,		realmask,	data->SvrDrv,	d->SvrDrv);
		UPDATE_FIELD(mask, MASK_TVDISK_CLIDRV,		realmask,	data->CliDrv,	d->CliDrv);
		UPDATE_FIELD(mask, MASK_TVDISK_TYPE,		realmask,	data->Type,		d->Type);
		UPDATE_FIELD(mask, MASK_TVDISK_LOADTYPE,	realmask,	data->LoadType,	d->LoadType);
		UPDATE_FIELD(mask, MASK_TVDISK_SVRMODE,		realmask,	data->SvrMode,	d->SvrMode);
		UPDATE_FIELD(mask, MASK_TVDISK_SIZE,		realmask,	data->Size,		d->Size);
		UPDATE_FIELD(mask, MASK_TVDISK_SSDDRV,		realmask,	data->SsdDrv,	d->SsdDrv);


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
		GET_STR_FIELD(d.VID,		_T("VID")		, rst);	
		GET_STR_FIELD(d.SvrID,		_T("SvrID")		, rst);	
		GET_INT_FIELD(d.SoucIP,		_T("SoucIp")	, rst);		
		GET_INT_FIELD(d.Port,		_T("Port")		, rst);		
		GET_INT_FIELD(d.SvrDrv,		_T("SvrDrv")	, rst);		
		GET_INT_FIELD(d.CliDrv,		_T("CliDrv")	, rst);		
		GET_INT_FIELD(d.Type,		_T("Type")		, rst);		
		GET_INT_FIELD(d.LoadType,	_T("LoadType")	, rst);	
		GET_STR_FIELD(d.SvrMode,	_T("SvrMode")	, rst);	
		GET_INT_FIELD(d.Size,		_T("Size")		, rst);	
		GET_INT_FIELD(d.SsdDrv,		_T("SsdDrv")	, rst);	

	}
};


//tCmpBootTask
class ZCmpBootTaskTable 
	: public ZTable_T<
		db::tCmpBootTask, 
		const TCHAR *,
		ICmpBootTaskTable, 
		ICmpBootTaskEventHandler, 
		ICmpBootTaskRecordset, 
		ICmpBootTaskFilter, 
		ICmpBootTaskSorter,
		ZSync,
		ZCmpBootTaskTable
	>
{
	DECLARE_TABLE(ZCmpBootTaskTable, tblCmpBootTask)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const TCHAR *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCMPBOOTTASK_TID:
				return _tcscmp(szParam, d->TID) == 0;
			case MASK_TCMPBOOTTASK_NAME:
				return _tcscmp(szParam, d->Name) == 0;
			case MASK_TCMPBOOTTASK_TYPE:
				return dwParam == d->Type;
			case MASK_TCMPBOOTTASK_FLAG:
				return dwParam == d->Flag;
			case MASK_TCMPBOOTTASK_RUNDATE:
				return dwParam == d->RunDate;
			case MASK_TCMPBOOTTASK_STARTTIME:
				return dwParam == d->StartTime;
			case MASK_TCMPBOOTTASK_DELAYTIME:
				return dwParam == d->DelayTime;
			case MASK_TCMPBOOTTASK_ENDTIME:
				return dwParam == d->EndTime;
			case MASK_TCMPBOOTTASK_ENDDATE:
				return dwParam == d->EndDate;
			case MASK_TCMPBOOTTASK_RUNTYPE:
				return dwParam == d->RunType;
			case MASK_TCMPBOOTTASK_STATUS:
				return dwParam == d->Status;
			default:
				assert(0);
				break;
			}
		}
		else if (CondMode == SQL_COND_MODE_NEQUAL)
		{
			switch (CondMask) {
			case MASK_TCMPBOOTTASK_TYPE:
				return dwParam != d->Type;
				break;
			default:
				assert(0);
				break;
			}
		}
		else 
		{
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
		case MASK_TCMPBOOTTASK_TID:
			return (_tcscmp(d1->TID, d2->TID) > 0) == desc;
		default:
			assert(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.TID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.TID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_NAME,		realmask,	data->Name,			d->Name);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_TYPE,		realmask,	data->Type,			d->Type);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_FLAG,		realmask,	data->Flag,			d->Flag);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_RUNDATE,	realmask,	data->RunDate,		d->RunDate);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_STARTTIME, realmask,	data->StartTime,	d->StartTime);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_DELAYTIME, realmask,	data->DelayTime,	d->DelayTime);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_ENDTIME,	realmask,	data->EndTime,		d->EndTime);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_ENDDATE,	realmask,	data->EndDate,		d->EndDate);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_RUNTYPE,	realmask,	data->RunType,		d->RunType);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_STATUS,	realmask,	data->Status,		d->Status);
		UPDATE_FIELD(mask, MASK_TCMPBOOTTASK_PARAMETER,	realmask,	data->Parameter,	d->Parameter);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(CMPBOOTTASK, PARAMETER,		Parameter,	STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.TID,		_T("TID")		, rst);	
		GET_STR_FIELD(d.Name,		_T("Name")		, rst);	
		GET_INT_FIELD(d.Type,		_T("Type")		, rst);		
		GET_INT_FIELD(d.Flag,		_T("Flag")		, rst);	
		GET_INT_FIELD(d.RunDate,	_T("RunDate")		, rst);	
		GET_INT_FIELD(d.StartTime,	_T("StartTime")		, rst);	
		GET_INT_FIELD(d.DelayTime,	_T("DelayTime")		, rst);	
		GET_INT_FIELD(d.EndTime,	_T("EndTime")		, rst);	
		GET_INT_FIELD(d.EndDate,	_T("EndDate")		, rst);		
		GET_INT_FIELD(d.RunType,	_T("RunType")		, rst);	
		GET_INT_FIELD(d.Status,		_T("Status")		, rst);	
		GET_STR_FIELD(d.Parameter,	_T("Parameter")	, rst);	
	}
};


BEGIN_DECLARE_TABLE_CLASS(SyncTask, const TCHAR *)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_VALUE_FIELD(SYNCTASK, DESTIP, DestIp)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_VALUE_FIELD(SYNCTASK, DESTIP, DestIp)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SID, STRING)
	GETKEY_IMPLMENT(SID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TSYNCTASK_SVRID,		realmask,	data->SvrID,			d->SvrID);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_NAME,			realmask,	data->Name,				d->Name);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_SVRDIR,		realmask,	data->SvrDir,			d->SvrDir);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_NODEDIR,		realmask,	data->NodeDir,			d->NodeDir);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_SOUCIP,		realmask,	data->SoucIp,			d->SoucIp);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_DESTIP,		realmask,	data->DestIp,			d->DestIp);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_SPEED,		realmask,	data->Speed,			d->Speed);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_SYNCTYPE,		realmask,	data->SyncType,			d->SyncType);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_STATUS,		realmask,	data->Status,			d->Status);
		UPDATE_FIELD(mask, MASK_TSYNCTASK_NAMEID,		realmask,	data->NameID,			d->NameID);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		assert(0);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.SID,		 _T("SID")			, rst);		
		GET_STR_FIELD(d.SvrID,		 _T("SvrID")		, rst);		
		GET_STR_FIELD(d.Name,		 _T("Name")			, rst);	
		GET_STR_FIELD(d.SvrDir,		 _T("SvrDir")		, rst);	
		GET_STR_FIELD(d.NodeDir,	 _T("NodeDir")		, rst);	
		GET_INT_FIELD(d.SoucIp,		 _T("SoucIp")		, rst);		
		GET_INT_FIELD(d.DestIp,		 _T("DestIp")		, rst);		
		GET_INT_FIELD(d.Speed,		 _T("Speed")		, rst);		
		GET_INT_FIELD(d.SyncType,	 _T("SyncType")		, rst);		
		GET_INT_FIELD(d.Status,		 _T("Status")		, rst);		
		GET_STR_FIELD(d.NameID,		 _T("NameID")		, rst);		

	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()



BEGIN_DECLARE_TABLE_CLASS(Server, const TCHAR *)

	BEGIN_SQLCOND_IMPLMENT()
	SQLCOND_STRING_FIELD(SERVER, SVRID, SvrID)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
	SQLSORT_STRING_FIELD(SERVER, SVRID, SvrID)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SvrID, STRING)
	GETKEY_IMPLMENT(SvrID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(mask, MASK_TSERVER_SVRID,			realmask,	data->SvrID,		d->SvrID);
		UPDATE_FIELD(mask, MASK_TSERVER_SVRNAME,		realmask,	data->SvrName,		d->SvrName);
		UPDATE_FIELD(mask, MASK_TSERVER_SVRTYPE,		realmask,	data->SvrType,		d->SvrType);
		UPDATE_FIELD(mask, MASK_TSERVER_SVRIP,			realmask,	data->SvrIP,		d->SvrIP);
		UPDATE_FIELD(mask, MASK_TSERVER_IP1,			realmask,	data->Ip1,			d->Ip1);
		UPDATE_FIELD(mask, MASK_TSERVER_IP2,			realmask,	data->Ip2,			d->Ip2);
		UPDATE_FIELD(mask, MASK_TSERVER_IP3,			realmask,	data->Ip3,			d->Ip3);
		UPDATE_FIELD(mask, MASK_TSERVER_IP4,			realmask,	data->Ip4,			d->Ip4);
		UPDATE_FIELD(mask, MASK_TSERVER_SPEED,			realmask,	data->Speed,		d->Speed);
		UPDATE_FIELD(mask, MASK_TSERVER_BALANCETYPE,	realmask,	data->BalanceType,	d->BalanceType);

	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		assert(0);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.SvrID,		 _T("SvrID")		, rst);	
		GET_STR_FIELD(d.SvrName,	 _T("SvrName")		, rst);	
		GET_INT_FIELD(d.SvrType,	 _T("SvrType")		, rst);	
		GET_INT_FIELD(d.SvrIP,		 _T("SvrIp")		, rst);	
		GET_INT_FIELD(d.Ip1,		 _T("Ip1")			, rst);	
		GET_INT_FIELD(d.Ip2,		 _T("Ip2")			, rst);	
		GET_INT_FIELD(d.Ip3,		 _T("Ip3")			, rst);	
		GET_INT_FIELD(d.Ip4,		 _T("Ip4")			, rst);	
		GET_INT_FIELD(d.Speed,		 _T("Speed")		, rst);		
		GET_INT_FIELD(d.BalanceType, _T("BalanceType")	, rst);		

	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()


BEGIN_DECLARE_TABLE_CLASS(BootTaskArea, StringStringKey)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(BOOTTASKAREA, TID, TID)
		SQLCOND_STRING_FIELD(BOOTTASKAREA, AID, AID)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(BOOTTASKAREA, TID, TID)
		SQLSORT_STRING_FIELD(BOOTTASKAREA, AID, AID)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_K1_K2_IMPLMENT(TID,AID, STRING,STRING)

	GETKEY_K1_K2_IMPLMENT(TID, AID)

	BEGIN_UPDATE_IMPLMENT()
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		assert(0);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.TID,		_T("TID")			, rst);		
		GET_STR_FIELD(d.AID,		_T("AID")			, rst);		
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()


	class ZSyncGameTable 
		: public ZTable_T<
		db::tSyncGame, 
		Uint32StringKey,
		ISyncGameTable, 
		ISyncGameEventHandler, 
		ISyncGameRecordset, 
		ISyncGameFilter, 
		ISyncGameSorter,
		ZSync,
		ZSyncGameTable
		>
	{
		DECLARE_TABLE(ZSyncGameTable, tblSyncGame)
	public:
		~ZSyncGameTable()
		{

		}

		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode != SQL_COND_MODE_EQUAL) 
				return false;

			switch (CondMask) 
			{
			case MASK_TSYNCGAME_SID:
				 return _tcscmp(szParam, d->SID) == 0;
			case MASK_TSYNCGAME_GID:
				return dwParam == d->GID;
			default:
				assert(0);
				break;
			}

			return false;

		}

		bool SqlSort(const Tty *const d1, const Tty *const d2, uint64 mask, uint32 desc)
		{
			switch (mask) 
			{
			case MASK_TSYNCGAME_SID:
				return (_tcscmp(d1->SID, d2->SID) > 0) == desc;
			default:
				assert(0);
				break;
			}
			return false;
		}

	public:
		//ISyncGameTable
		int Update(uint32 gid, const Tty *const d, uint64 mask) 
		{
			struct UpdateMask {
				Kty key;
				uint64 mask;
			};
			std::vector<UpdateMask> updates;

			{
				I8_GUARD(ZGuard, Lty, m_lock);

				std::vector<uint32> idxs;

				m_idxs.GetIdx(gid, idxs);
				for (size_t n = 0; n < idxs.size(); n++) {
					Tty& data = m_pRecords[idxs[n]];
					if (uint64 realmask = UpdateRecord(&data, d, mask)) {
						UpdateMask um;
						um.key = Index::CloneKey(GetKey(data));
						um.mask = realmask;
						updates.push_back(um);
					}
				}
			}

			for (size_t n = 0; n < updates.size(); n++) {
				HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
				Index::FreeKey(updates[n].key);
			}
			return 0;
		}

		int Update(uint32 gid, const TCHAR *aid, const Tty *const d, uint64 mask) 
		{
			Kty key = { gid, aid };
			return _Base::Update(key, d, mask);
		}

		int Cumulate(uint32 gid, const Tty *const d, uint64 mask) 
		{
			struct UpdateMask {
				Kty key;
				uint64 mask;
			};
			std::vector<UpdateMask> updates;

			{
				I8_GUARD(ZGuard, Lty, m_lock);

				std::vector<uint32> idxs;

				m_idxs.GetIdx(gid, idxs);
				for (size_t n = 0; n < idxs.size(); n++) {
					Tty& data = m_pRecords[idxs[n]];
					if (uint64 realmask = CumulateRecord(&data, d, mask)) {
						UpdateMask um;
						um.key = Index::CloneKey(GetKey(data));
						um.mask = realmask;
						updates.push_back(um);
					}
				}
			}

			for (size_t n = 0; n < updates.size(); n++) {
				HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
				Index::FreeKey(updates[n].key);
			}
			return 0;
		}

		int Cumulate(uint32 gid, const TCHAR *sid, const Tty *const d, uint64 mask) 
		{
			Kty key = { gid, sid };
			return _Base::Cumulate(key, d, mask);
		}

		int Delete(uint32 gid) 
		{
			std::vector<Kty> keys;
			{
				I8_GUARD(ZGuard, Lty, m_lock);

				std::vector<uint32> idxs;

				m_idxs.GetIdx(gid, idxs);
				for (size_t n = 0; n < idxs.size(); n++) {
					Tty& data = m_pRecords[idxs[n]];
					keys.push_back(Index::CloneKey(GetKey(data)));
					memset(&data, 0, sizeof(Tty));
				}
				m_idxs.ClearIdx(gid);
			}

			for (size_t n = 0; n < keys.size(); n++) {
				HandleEvent(SQL_OP_DELETE, keys[n], 0);
				Index::FreeKey(keys[n]);
			}
			return 0;
		}

		int Delete(uint32 gid, const TCHAR *sid) 
		{
			Kty key = { gid, sid };
			return _Base::Delete(key);
		}

		int GetData(uint32 gid, const TCHAR *sid, Tty *d, ulong *ver) 
		{
			Kty key = { gid, sid };
			return _Base::GetData(key, d, ver);
		}

		int Select(Rty **recordset, uint32 gid, Sty *sorter) 
		{
			assert(recordset);

			RecordSet *pRecordset = new RecordSet(m_mp);
			{
				I8_GUARD(ZGuard, Lty, m_lock);

				std::vector<uint32> idxs;
				m_idxs.GetIdx(gid, idxs);
				for (size_t n = 0; n < idxs.size(); n++) {
					Tty& data = m_pRecords[idxs[n]];
					pRecordset->AddData(data, m_idxs.GetVer(GetKey(data)));
				}
			}

			if (sorter) {
				pRecordset->Sort(sorter);
			}
			*recordset = static_cast<Rty *>(pRecordset);

			return 0;
		}

	protected:
		bool InvalidRecord(const Tty& d)
		{
			return d.GID == 0;
		}

		Kty GetKey(const Tty& d) 
		{
			Kty key = { d.GID, d.SID };
			return key;
		}

		uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
		{
			BEGIN_UPDATE();
			assert(0);
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
			GET_STR_FIELD(d.SID,	_T("SID")	, rst);		
			GET_INT_FIELD(d.GID,	_T("GID")	, rst);	
		}
	};


//tPushGameStatic
	class ZPushGameStaticTable 
		: public ZTable_T<
		db::tPushGameStatic, 
		uint32,
		IPushGameStaticTable, 
		IPushGameStaticEventHandler, 
		IPushGameStaticRecordset, 
		IPushGameStaticFilter, 
		IPushGameStaticSorter,
		ZSync,
		ZPushGameStaticTable
		>
	{
		DECLARE_TABLE(ZPushGameStaticTable, tblPushGameStatic)
	public:
		bool SqlCond(const Tty *const d, 
			uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const TCHAR *szParam)
		{
			if (CondMode == SQL_COND_MODE_EQUAL) {
				switch (CondMask) {
	case MASK_TPUSHGAMESTATIC_BEGINDATE:
		return dwParam == d->BeginDate;
	case MASK_TPUSHGAMESTATIC_ENDDATE:
		return dwParam == d->EndDate;
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
	case MASK_TPUSHGAMESTATIC_BEGINDATE:
		return  (d1->BeginDate > d2->BeginDate) == desc;
	case MASK_TPUSHGAMESTATIC_ENDDATE:
		return  (d1->EndDate > d2->EndDate) == desc;
	case MASK_TPUSHGAMESTATIC_SIZE:
		return (d1->Size > d2->Size) == desc;
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

			UPDATE_FIELD(mask, MASK_TPUSHGAMESTATIC_SIZE,		realmask,	data->Size,			d->Size);
			UPDATE_FIELD(mask, MASK_TPUSHGAMESTATIC_BEGINDATE,	realmask, 	data->BeginDate,	d->BeginDate);
			UPDATE_FIELD(mask, MASK_TPUSHGAMESTATIC_ENDDATE,	realmask,	data->EndDate,		d->EndDate);
			
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
			GET_INT_FIELD(d.Size		, _T("Size")		, rst);			
			GET_INT_FIELD(d.BeginDate	, _T("BeginDate")	, rst);			
			GET_INT_FIELD(d.EndDate		, _T("EndDate")		, rst);			
		}
	};



} //namespace i8desk


#endif //I8_DBTABLES_H
