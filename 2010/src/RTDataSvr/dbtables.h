/**
 * dbtables.h 数据库表，需要持久化并保持一致状态的数据
 */
#ifndef I8_DBTABLES_H
#define I8_DBTABLES_H

#include "table_T.h"
#include "../../include/irtdatasvr.h"

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
	DECLARE_TABLE(ZGameTable, tGame)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TGAME_NAME:
				return strcmp(szParam, d->Name) == 0;
			case MASK_TGAME_PID:
				return dwParam == d->PID;
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
		case MASK_TGAME_NAME:
			return (strcmp(d1->Name, d2->Name) > 0) == desc;
		case MASK_TGAME_PID:
			return (d1->PID > d2->PID) == desc;
		default:
			I8_ASSERT(0);
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

		UPDATE_FIELD(GAME, PID,			PID,			VALUE);
		UPDATE_FIELD(GAME, NAME,		Name,			STRING);
		UPDATE_FIELD(GAME, DEFCLASS,	DefClass,		STRING);
		UPDATE_FIELD(GAME, GAMEEXE,		GameExe,		STRING);
		UPDATE_FIELD(GAME, PARAM,		Param,			STRING);
		UPDATE_FIELD(GAME, SIZE,		Size,			VALUE);
		UPDATE_FIELD(GAME, URL,			URL,			STRING);
		UPDATE_FIELD(GAME, DESKLNK,		DeskLnk,		VALUE);
		UPDATE_FIELD(GAME, TOOLBAR,		Toolbar,		VALUE);
		UPDATE_FIELD(GAME, SVRPATH,		SvrPath,		STRING);
		UPDATE_FIELD(GAME, CLIPATH,		CliPath,		STRING);
		UPDATE_FIELD(GAME, PRIORITY,	Priority,		VALUE);
		UPDATE_FIELD(GAME, DOWNPRIORITY,DownPriority,	VALUE);
		UPDATE_FIELD(GAME, SHOWPRIORITY,ShowPriority,	VALUE);
		UPDATE_FIELD(GAME, MATCHFILE,	MatchFile,		STRING);
		UPDATE_FIELD(GAME, SAVEFILTER,	SaveFilter,		STRING);
		UPDATE_FIELD(GAME, IDCUPTDATE,	IdcUptDate,		VALUE);
		UPDATE_FIELD(GAME, SVRUPTDATE,	SvrUptDate,		VALUE);
		UPDATE_FIELD(GAME, IDCVER,		IdcVer,			VALUE);
		UPDATE_FIELD(GAME, SVRVER,		SvrVer,			VALUE);
		UPDATE_FIELD(GAME, AUTOUPT,		AutoUpt,		VALUE);
		UPDATE_FIELD(GAME, I8PLAY,		I8Play,			VALUE);
		UPDATE_FIELD(GAME, IDCCLICK,	IdcClick,		VALUE);
		UPDATE_FIELD(GAME, SVRCLICK,	SvrClick,		VALUE);
		UPDATE_FIELD(GAME, ADDDATE,		AddDate,		VALUE);
		UPDATE_FIELD(GAME, STATUS,		Status,			VALUE);
		UPDATE_FIELD(GAME, IFUPDATE,	IfUpdate,		VALUE);
		UPDATE_FIELD(GAME, IFDISPLAY,	IfDisplay,		VALUE);
		UPDATE_FIELD(GAME, COMMENT,		Comment,		STRING);
		UPDATE_FIELD(GAME, AUTO,		Auto,			VALUE);
		UPDATE_FIELD(GAME, DECLINECTR,	Declinectr,		VALUE);
		UPDATE_FIELD(GAME, HIDE,		hide,			VALUE);
		UPDATE_FIELD(GAME, LOCALPATH,	LocalPath,		STRING);
		UPDATE_FIELD(GAME, SAVEFILTER2, SaveFilter2,	STRING);
		UPDATE_FIELD(GAME, GAMEEXE2,	GameExe2,		STRING);
		UPDATE_FIELD(GAME, PARAM2,		Param2,			STRING);
		UPDATE_FIELD(GAME, SVRCLICK2,	SvrClick2,		VALUE);
				
		// Add url1~url4
		UPDATE_FIELD(GAME, URL1,		url1,			STRING);
		UPDATE_FIELD(GAME, URL2,		url2,			STRING);
		UPDATE_FIELD(GAME, URL3,		url3,			STRING);
		UPDATE_FIELD(GAME, URL4,		url4,			STRING);

		UPDATE_FIELD(GAME, AUTOSYNC,	AutoSync,		VALUE);

		UPDATE_FIELD(GAME, GAMESOURCE,	GameSource,		STRING);

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
		GET_INT_FIELD(d.DeskLnk		, _T("DeskLnk")		, rst);		
		GET_INT_FIELD(d.Toolbar		, _T("Toolbar")		, rst);		
		GET_INT_FIELD(d.Priority	, _T("Priority")	, rst);		
		GET_INT_FIELD(d.DownPriority, _T("DownPriority"), rst);	
		GET_INT_FIELD(d.ShowPriority, _T("ShowPriority"), rst);	
		GET_INT_FIELD(d.IdcUptDate	, _T("IdcUptDate")	, rst);	
		GET_INT_FIELD(d.SvrUptDate	, _T("SvrUptDate")	, rst);	
		GET_INT_FIELD(d.IdcVer		, _T("IdcVer")		, rst);		
		GET_INT_FIELD(d.SvrVer		, _T("SvrVer")		, rst);		
		GET_INT_FIELD(d.AutoUpt		, _T("AutoUpt")		, rst);	
		GET_INT_FIELD(d.I8Play		, _T("I8Play")		, rst);		
		GET_INT_FIELD(d.IdcClick	, _T("IdcClick")	, rst);	
		GET_INT_FIELD(d.SvrClick	, _T("SvrClick")	, rst);	
		GET_INT_FIELD(d.AddDate		, _T("AddDate")		, rst);	
		GET_INT_FIELD(d.Status		, _T("Status")		, rst);		
		GET_INT_FIELD(d.IfUpdate	, _T("IfUpdate")	, rst);	
		GET_INT_FIELD(d.IfDisplay	, _T("IfDisplay")	, rst);	
		GET_INT_FIELD(d.Auto		, _T("Auto")		, rst);		
		GET_INT_FIELD(d.Declinectr	, _T("Declinectr")	, rst);	
		GET_INT_FIELD(d.hide		, _T("hide")		, rst);		

		GET_STR_FIELD(d.Name,		_T("Name")		, rst);		
		GET_STR_FIELD(d.DefClass,	_T("DefClass")	, rst);	
		GET_STR_FIELD(d.GameExe,	_T("GameExe")	, rst);		
		GET_STR_FIELD(d.Param,		_T("Param")		, rst);		
		GET_STR_FIELD(d.URL,		_T("URL")		, rst);			
		GET_STR_FIELD(d.SvrPath,	_T("SvrPath")	, rst);		
		GET_STR_FIELD(d.CliPath,	_T("CliPath")	, rst);		
		GET_STR_FIELD(d.MatchFile,	_T("MatchFile")	, rst);	
		GET_STR_FIELD(d.SaveFilter,	_T("SaveFilter"), rst);	
		GET_STR_FIELD(d.Comment,	_T("Comment")	, rst);		
		GET_STR_FIELD(d.LocalPath,	_T("LocalPath")	, rst);	
		GET_STR_FIELD(d.SaveFilter2,_T("SaveFilter2"), rst);	
		GET_STR_FIELD(d.GameExe2,	_T("GameExe2")	, rst);	
		GET_STR_FIELD(d.Param2,		_T("Param2")	, rst);		

		// Add url1~url4
		GET_STR_FIELD(d.url1,		_T("url1")		, rst);
		GET_STR_FIELD(d.url2,		_T("url2")		, rst);
		GET_STR_FIELD(d.url3,		_T("url3")		, rst);
		GET_STR_FIELD(d.url4,		_T("url4")		, rst);

		GET_INT_FIELD(d.AutoSync,	_T("AutoSync")	, rst);
		GET_STR_FIELD(d.GameSource,	_T("GameSource"), rst);	
	}
};

//tClass
class ZClassTable 
	: public ZTable_T<
		db::tClass, 
		const char *,
		IClassTable, 
		IClassEventHandler, 
		IClassRecordset, 
		IClassFilter, 
		IClassSorter,
		ZSync,
		ZClassTable
	>
{
	DECLARE_TABLE(ZClassTable, tClass)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCLASS_NAME:
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
		case MASK_TCLASS_NAME:
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
		return d.DefClass[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.DefClass;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(CLASS, NAME,		Name,			STRING);
		UPDATE_FIELD(CLASS, TYPE,		Type,			STRING);
		UPDATE_FIELD(CLASS, PATH,		Path,			STRING);

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
		GET_STR_FIELD(d.DefClass,	_T("DefClass")	, rst);		
		GET_STR_FIELD(d.Name,		_T("Name")		, rst);	
		GET_STR_FIELD(d.Path,		_T("Path")		, rst);		
		GET_STR_FIELD(d.Type,		_T("Type")		, rst);		
	}
};

class ZGameAreaTable 
	: public ZTable_T<
		db::tGameArea, 
		Uint32StringKey,
		IGameAreaTable, 
		IGameAreaEventHandler, 
		IGameAreaRecordset, 
		IGameAreaFilter, 
		IGameAreaSorter,
		ZSync,
		ZGameAreaTable
	>
{
	DECLARE_TABLE(ZGameAreaTable, rGameArea)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TGAMEAREA_RUNTYPE:
				return dwParam == d->RunType;
			case MASK_TGAMEAREA_GID:
				return dwParam == d->GID;
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
		case MASK_TGAMEAREA_AID:
			return (strcmp(d1->AID, d2->AID) > 0) == desc;
		default:
			I8_ASSERT(0);
			break;
		}
		return false;
	}

public:
//IGameAreaTable
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

	int Update(uint32 gid, const char *aid, const Tty *const d, uint64 mask) 
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

	int Cumulate(uint32 gid, const char *aid, const Tty *const d, uint64 mask) 
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

	int Delete(uint32 gid, const char *aid) 
	{
		Kty key = { gid, aid };
		return _Base::Delete(key);
	}

	int GetData(uint32 gid, const char *aid, Tty *d, ulong *ver) 
	{
		Kty key = { gid, aid };
		return _Base::GetData(key, d, ver);
	}

	int Select(Rty **recordset, uint32 gid, Sty *sorter) 
	{
		I8_ASSERT(recordset);

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

		UPDATE_FIELD(GAMEAREA, RUNTYPE,	RunType,		VALUE);
		UPDATE_FIELD(GAMEAREA, VID,		VID,			STRING);

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
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);		
		GET_INT_FIELD(d.GID,	_T("GID")	, rst);	
		GET_INT_FIELD(d.RunType,_T("RunType"), rst);		
		GET_STR_FIELD(d.VID,	_T("VID")	, rst);		
	}
};


//tSysOpt
class ZSysOptTable 
	: public ZTable_T<
		db::tSysOpt, 
		const char *,
		ISysOptTable, 
		ISysOptEventHandler, 
		ISysOptRecordset, 
		ISysOptFilter, 
		ISysOptSorter,
		ZSync,
		ZSysOptTable
	>
{
	DECLARE_TABLE(ZSysOptTable, tSysOpt)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TSYSOPT_KEY:
				if (strcmp(szParam, d->key) == 0)
					return true;
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
		case MASK_TSYSOPT_KEY:
			return (strcmp(d1->key, d2->key) > 0) == desc;
		default:
			I8_ASSERT(0);
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

		UPDATE_FIELD(SYSOPT, VALUE, value, STRING);

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
		const char *,
		IAreaTable, 
		IAreaEventHandler, 
		IAreaRecordset, 
		IAreaFilter, 
		IAreaSorter,
		ZSync,
		ZAreaTable
	>
{
	DECLARE_TABLE(ZAreaTable, tArea)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TAREA_NAME:
				return strcmp(szParam, d->Name) == 0;
			case MASK_TAREA_SID:
				return strcmp(szParam, d->SID) == 0;
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
		case MASK_TAREA_NAME:
			return (strcmp(d1->Name, d2->Name) > 0) == desc;
		case MASK_TAREA_SID:
			return (strcmp(d1->SID, d2->SID) > 0) == desc;
		default:
			I8_ASSERT(0);
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

		UPDATE_FIELD(AREA, NAME,	Name,			STRING);
		UPDATE_FIELD(AREA, SID,		SID,			STRING);
	
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
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);		
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_STR_FIELD(d.SID,	_T("SID")	, rst);	
	}
};

//tClient
class ZClientTable 
	: public ZTable_T<
		db::tClient, 
		const char *,
		IClientTable, 
		IClientEventHandler, 
		IClientRecordset, 
		IClientFilter, 
		IClientSorter,
		ZSync,
		ZClientTable
	>
{
	DECLARE_TABLE(ZClientTable, tClient)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCLIENT_AID:
				return _stricmp(szParam, d->AID) == 0;
			case MASK_TCLIENT_IP:
				return dwParam == d->IP;
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
		case MASK_TCLIENT_NAME:
			return (_stricmp(d1->Name, d2->Name) > 0) == desc;
		default:
			I8_ASSERT(0);
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

		UPDATE_FIELD(CLIENT, IP,			IP,				VALUE);
		UPDATE_FIELD(CLIENT, MARK,			Mark,			STRING);
		UPDATE_FIELD(CLIENT, NETGATE,		NetGate,		STRING);
		UPDATE_FIELD(CLIENT, DNS,			DNS,			STRING);
		UPDATE_FIELD(CLIENT, DNS2,			DNS2,			STRING);
		UPDATE_FIELD(CLIENT, MAC,			MAC,			STRING);
		UPDATE_FIELD(CLIENT, AID,			AID,			STRING);
		UPDATE_FIELD(CLIENT, PROTINSTALL,	ProtInstall,	VALUE);
		UPDATE_FIELD(CLIENT, ALLAREA,		AllArea,		STRING);
		UPDATE_FIELD(CLIENT, PROTAREA,		ProtArea,		STRING);
		UPDATE_FIELD(CLIENT, PROTVER,		ProtVer,		STRING);
		UPDATE_FIELD(CLIENT, MENUVER,		MenuVer,		STRING);
		UPDATE_FIELD(CLIENT, CLISVRVER,		CliSvrVer,		STRING);
		UPDATE_FIELD(CLIENT, GPFREESIZE,	GPFreeSize,		VALUE);
		UPDATE_FIELD(CLIENT, SCSTATUS,		SCStatus,		VALUE);
		UPDATE_FIELD(CLIENT, CPU,			CPU,			STRING);
		UPDATE_FIELD(CLIENT, MEMORY,		Memory,			STRING);
		UPDATE_FIELD(CLIENT, DISK,			Disk,			STRING);
		UPDATE_FIELD(CLIENT, GRAPHICS,		Graphics,		STRING);
		UPDATE_FIELD(CLIENT, MAINBOARD,		Mainboard,		STRING);
		UPDATE_FIELD(CLIENT, NIC,			NIC,			STRING);
		UPDATE_FIELD(CLIENT, WEBCAM,		Webcam,			STRING);
		UPDATE_FIELD(CLIENT, SYSTEM,		System,			STRING);
		UPDATE_FIELD(CLIENT, IEPROT,		IEProt,			VALUE);
		UPDATE_FIELD(CLIENT, FDOGDRIVER,	FDogDriver,		VALUE);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(CLIENT, ALLAREA,		AllArea,		STRING);
		CUMULATE_FIELD(CLIENT, PROTAREA,	ProtArea,		STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_INT_FIELD(d.IP,		_T("IP")	, rst);		
		GET_STR_FIELD(d.Mark,	_T("Mark")	, rst);	
		GET_STR_FIELD(d.NetGate,_T("NetGate"), rst);	
		GET_STR_FIELD(d.DNS,	_T("DNS")	, rst);	
		GET_STR_FIELD(d.DNS2,	_T("DNS2")	, rst);	
		GET_STR_FIELD(d.MAC,	_T("MAC")	, rst);	
		GET_STR_FIELD(d.AID,	_T("AID")	, rst);	
		GET_INT_FIELD(d.ProtInstall, _T("ProtInstall"), rst);		
		GET_STR_FIELD(d.AllArea,	_T("AllArea")	, rst);	
		GET_STR_FIELD(d.ProtArea,	_T("ProtArea")	, rst);	
		GET_STR_FIELD(d.ProtVer,	_T("ProtVer")	, rst);	
		GET_STR_FIELD(d.MenuVer,	_T("MenuVer")	, rst);	
		GET_STR_FIELD(d.CliSvrVer,	_T("CliSvrVer")	, rst);	
		GET_INT_FIELD(d.GPFreeSize,	_T("GPFreeSize"), rst);		
		GET_INT_FIELD(d.SCStatus,	_T("SCStatus")	, rst);		
		GET_STR_FIELD(d.CPU,		_T("CPU")		, rst);	
		GET_STR_FIELD(d.Memory,		_T("Memory")	, rst);	
		GET_STR_FIELD(d.Disk,		_T("Disk")		, rst);	
		GET_STR_FIELD(d.Graphics,	_T("Graphics")	, rst);	
		GET_STR_FIELD(d.Mainboard,	_T("Mainboard")	, rst);	
		GET_STR_FIELD(d.NIC,		_T("NIC")		, rst);	
		GET_STR_FIELD(d.Webcam,		_T("Webcam")	, rst);	
		GET_STR_FIELD(d.System,		_T("System")	, rst);	
		GET_INT_FIELD(d.IEProt,		_T("IEProt")	, rst);		
		GET_INT_FIELD(d.FDogDriver,	_T("FDogDriver"), rst);		
	}
};

//tFavorite
class ZFavoriteTable 
	: public ZTable_T<
		db::tFavorite, 
		const char *,
		IFavoriteTable, 
		IFavoriteEventHandler, 
		IFavoriteRecordset, 
		IFavoriteFilter, 
		IFavoriteSorter,
		ZSync,
		ZFavoriteTable
	>
{
	DECLARE_TABLE(ZFavoriteTable, tFavorite)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TFAVORITE_NAME:
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
		case MASK_TFAVORITE_SERIAL:
			return (d1->Serial > d2->Serial) == desc;
		default:
			I8_ASSERT(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.UID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.UID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(FAVORITE, SERIAL,	Serial,		VALUE);
		UPDATE_FIELD(FAVORITE, NAME,	Name,		STRING);
		UPDATE_FIELD(FAVORITE, TYPE,	Type,		STRING);
		UPDATE_FIELD(FAVORITE, URL,		URL,		STRING);

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
		GET_INT_FIELD(d.Serial,	_T("Serial"), rst);		
		GET_STR_FIELD(d.UID,	_T("UID")	, rst);	
		GET_STR_FIELD(d.Name,	_T("Name")	, rst);	
		GET_STR_FIELD(d.Type,	_T("Type")	, rst);		
		GET_STR_FIELD(d.URL,	_T("URL")	, rst);	
	}
};


//tVDisk
class ZVDiskTable 
	: public ZTable_T<
		db::tVDisk, 
		const char *,
		IVDiskTable, 
		IVDiskEventHandler, 
		IVDiskRecordset, 
		IVDiskFilter, 
		IVDiskSorter,
		ZSync,
		ZVDiskTable
	>
{
	DECLARE_TABLE(ZVDiskTable, tVDisk)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TVDISK_TYPE:
				return dwParam == d->Type;
			case MASK_TVDISK_CLIDRV:
				return dwParam == d->CliDrv;
			case MASK_TVDISK_SID:
				return strcmp(szParam, d->SID) == 0;
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
		case MASK_TVDISK_CLIDRV:
			return (d1->CliDrv > d2->CliDrv) == desc;
		default:
			I8_ASSERT(0);
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

		UPDATE_FIELD(VDISK, IP,			IP,		VALUE);
		UPDATE_FIELD(VDISK, PORT,		Port,		VALUE);
		UPDATE_FIELD(VDISK, SVRDRV,		SvrDrv,		VALUE);
		UPDATE_FIELD(VDISK, CLIDRV,		CliDrv,		VALUE);
		UPDATE_FIELD(VDISK, SERIAL,		Serial,		VALUE);
		UPDATE_FIELD(VDISK, TYPE,		Type,		VALUE);
		UPDATE_FIELD(VDISK, LOADTYPE,	LoadType,	VALUE);
		UPDATE_FIELD(VDISK, SID,		SID,		STRING);

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
		GET_STR_FIELD(d.VID,		_T("VID")		, rst);	
		GET_INT_FIELD(d.IP,			_T("IP")		, rst);		
		GET_INT_FIELD(d.Port,		_T("Port")		, rst);		
		GET_INT_FIELD(d.SvrDrv,		_T("SvrDrv")	, rst);		
		GET_INT_FIELD(d.CliDrv,		_T("CliDrv")	, rst);		
		GET_INT_FIELD(d.Serial,		_T("Serial")	, rst);		
		GET_INT_FIELD(d.Type,		_T("Type")		, rst);		
		GET_INT_FIELD(d.LoadType,	_T("LoadType")	, rst);		
		GET_STR_FIELD(d.SID,		_T("SID")		, rst);		
	}
};


//tCmpStartTask
class ZCmpStartTaskTable 
	: public ZTable_T<
		db::tCmpStartTask, 
		const char *,
		ICmpStartTaskTable, 
		ICmpStartTaskEventHandler, 
		ICmpStartTaskRecordset, 
		ICmpStartTaskFilter, 
		ICmpStartTaskSorter,
		ZSync,
		ZCmpStartTaskTable
	>
{
	DECLARE_TABLE(ZCmpStartTaskTable, tCmpStartTask)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TCMPSTARTTASK_UID:
				return strcmp(szParam, d->UID) == 0;
			case MASK_TCMPSTARTTASK_AREATYPE:
				return dwParam == d->AreaType;
			case MASK_TCMPSTARTTASK_TYPE:
				return dwParam == d->Type;
			case MASK_TCMPSTARTTASK_FLAG:
				return dwParam == d->Flag;
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
		case MASK_TCMPSTARTTASK_UID:
			return (strcmp(d1->UID, d2->UID) > 0) == desc;
		default:
			I8_ASSERT(0);
			break;
		}
		return false;
	}

protected:
	bool InvalidRecord(const Tty& d)
	{
		return d.UID[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.UID;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(CMPSTARTTASK, AREATYPE,	AreaType,	VALUE);
		UPDATE_FIELD(CMPSTARTTASK, AREAPARAM,	AreaParam,	STRING);
		UPDATE_FIELD(CMPSTARTTASK, TYPE,		Type,		VALUE);
		UPDATE_FIELD(CMPSTARTTASK, FLAG,		Flag,		VALUE);
		UPDATE_FIELD(CMPSTARTTASK, CONTENT,		Content,	STRING);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(CMPSTARTTASK, AREAPARAM,		AreaParam,	STRING);
		CUMULATE_FIELD(CMPSTARTTASK, CONTENT,		Content,	STRING);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.UID,		_T("UID")		, rst);	
		GET_INT_FIELD(d.AreaType,	_T("AreaType")	, rst);		
		GET_STR_FIELD(d.AreaParam,	_T("AreaParam")	, rst);	
		GET_INT_FIELD(d.Type,		_T("Type")		, rst);		
		GET_INT_FIELD(d.Flag,		_T("Flag")		, rst);		
		GET_STR_FIELD(d.Content,	_T("Content")	, rst);	
	}
};

//tUser
class ZUserTable 
	: public ZTable_T<
		db::tUser, 
		const char *,
		IUserTable, 
		IUserEventHandler, 
		IUserRecordset, 
		IUserFilter, 
		IUserSorter, 
		ZSync,
		ZUserTable
	>
{
	DECLARE_TABLE(ZUserTable, tUser)
public:
	bool SqlCond(const Tty *const d, 
		uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const char *szParam)
	{
		if (CondMode == SQL_COND_MODE_EQUAL) {
			switch (CondMask) {
			case MASK_TUSER_NAME:
				if (strcmp(szParam, d->Name) == 0)
					return true;
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
		case MASK_TUSER_NAME:
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
		return d.Name[0] == 0;
	}

	Kty GetKey(const Tty& d) 
	{
		return d.Name;
	}

	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) 
	{
		BEGIN_UPDATE();

		UPDATE_FIELD(USER, PWD,				Pwd,			STRING);
		UPDATE_FIELD(USER, ROOMSIZE,		RoomSize,		VALUE);
		UPDATE_FIELD(USER, CREATEDATE,		CreateDate,		VALUE);
		UPDATE_FIELD(USER, LASTLOGINDATE,	LastLoginDate,	VALUE);
		UPDATE_FIELD(USER, LASTLOGINIP,		LastLoginIp,	VALUE);

		END_UPDATE();
	}

	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask)
	{
		BEGIN_CUMULATE();

		CUMULATE_FIELD(USER, ROOMSIZE,		RoomSize,		VALUE);

		END_CUMULATE();
	}

	void FillRecord(Tty& d, const _RecordsetPtr& rst)
	{
		GET_STR_FIELD(d.Name,		_T("Name")		, rst);	
		GET_STR_FIELD(d.Pwd,		_T("Pwd")		, rst);		
		GET_INT_FIELD(d.RoomSize,	_T("RoomSize")	, rst);		
		GET_INT_FIELD(d.CreateDate,	_T("CreateDate"), rst);		
		GET_INT_FIELD(d.LastLoginDate,		_T("LastLoginDate"), rst);		
		GET_INT_FIELD(d.LastLoginIp,		_T("LastLoginIp")  , rst);		
	}
};


BEGIN_DECLARE_TABLE_CLASS(SyncTask, const char *)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(SYNCTASK, SYNCIP, SyncIP)
		SQLCOND_VALUE_FIELD(SYNCTASK, SYNCTYPE, SyncType)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(SYNCTASK, SYNCIP, SyncIP)
		SQLSORT_VALUE_FIELD(SYNCTASK, SYNCTYPE, SyncType)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(SID, STRING)
	GETKEY_IMPLMENT(SID)

	BEGIN_UPDATE_IMPLMENT()
		UPDATE_FIELD(SYNCTASK, NAME,		Name,			STRING);
		UPDATE_FIELD(SYNCTASK, SYNCIP,		SyncIP,			STRING);
		UPDATE_FIELD(SYNCTASK, VDISKIP,		VDiskIP,		STRING);
		UPDATE_FIELD(SYNCTASK, UPDATEIP,	UpdateIP,		STRING);
		UPDATE_FIELD(SYNCTASK, SYNCTYPE,	SyncType,		VALUE);
		UPDATE_FIELD(SYNCTASK, DESTDRV,		DestDrv,		VALUE);
		UPDATE_FIELD(SYNCTASK, BALANCETYPE, BalanceType,	VALUE);
		UPDATE_FIELD(SYNCTASK, MAXSYNCSPEED,MaxSyncSpeed,	VALUE);
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		I8_ASSERT(0);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.SID,		 _T("SID")			, rst);		
		GET_STR_FIELD(d.Name,		 _T("Name")			, rst);		
		GET_STR_FIELD(d.SyncIP,		 _T("SyncIP")		, rst);		
		GET_STR_FIELD(d.VDiskIP,	 _T("VDiskIP")		, rst);		
		GET_STR_FIELD(d.UpdateIP,	 _T("UpdateIP")		, rst);		
		GET_INT_FIELD(d.SyncType,	 _T("SyncType")		, rst);		
		GET_INT_FIELD(d.DestDrv,	 _T("DestDrv")		, rst);		
		GET_INT_FIELD(d.BalanceType, _T("BalanceType")	, rst);		
		GET_INT_FIELD(d.MaxSyncSpeed,_T("MaxSyncSpeed")	, rst);		
	END_FILLRECORD_IMPLMENT()

END_DECLARE_TABLE_CLASS()


BEGIN_DECLARE_TABLE_CLASS(SyncGame, StringUint32Key)

	BEGIN_SQLCOND_IMPLMENT()
		SQLCOND_STRING_FIELD(SYNCGAME, SID, SID)
		SQLCOND_VALUE_FIELD(SYNCGAME, GID, GID)
	END_SQLCOND_IMPLMENT()

	BEGIN_SQLSORT_IMPLMENT()
		SQLSORT_STRING_FIELD(SYNCGAME, SID, SID)
		SQLSORT_VALUE_FIELD(SYNCGAME, GID, GID)
	END_SQLSORT_IMPLMENT()

	INVALIDRECORD_IMPLMENT(GID, VALUE)
	GETKEY_K1_K2_IMPLMENT(SID, GID)

	BEGIN_UPDATE_IMPLMENT()
	END_UPDATE_IMPLMENT()

	BEGIN_CUMULATE_IMPLMENT()
		I8_ASSERT(0);
	END_CUMULATE_IMPLMENT()

	BEGIN_FILLRECORD_IMPLMENT()
		GET_STR_FIELD(d.SID,		_T("SID")			, rst);		
		GET_INT_FIELD(d.GID,		_T("GID")			, rst);		
	END_FILLRECORD_IMPLMENT()

	INTERFACE_2K_IMPLMENT()

END_DECLARE_TABLE_CLASS()

} //namespace i8desk


#endif //I8_DBTABLES_H
