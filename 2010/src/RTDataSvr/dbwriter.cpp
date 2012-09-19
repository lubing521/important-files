#include "StdAfx.h"
#include "dbwriter.h"
#include "tablemgr.h"
#include "../../include/dbengine.h"
#include "../../include/i8logmsg.h"

#define KEY_TYPE(X) (X)
#define STDSTRING_TO_CSTR(X) (X).c_str()

#define IMPLMENT_EVENTHANDLER_OBJECT(tname, flag, Kty, Ktr) \
	namespace DBWriter { \
	class Z##tname##EventHandler : public I##tname##EventHandler  \
	{ \
	public: \
		Z##tname##EventHandler(ZDBWriter *pDBWriter, uint32 optypes) \
			: m_pDBWriter(pDBWriter), m_optypes(optypes) \
		{ \
		} \
		uint32 GetEvents(void) \
		{  \
			return m_optypes;  \
		} \
		void HandleEvent(uint32 optype, Kty key, uint64 mask) \
		{ \
			m_pDBWriter->Add##tname##Event(optype, key, mask); \
		} \
	private: \
		ZDBWriter *m_pDBWriter; \
		uint32 m_optypes; \
	}; \
	} \
	void ZDBWriter::Process##tname##Events(uint32 threshold) \
	{  \
		if (m_##tname##EventList.size() < threshold) \
			return; \
		m_pTableMgr->Save(FLAG_##flag##_BIT); \
		if (!m_pDatabase) { \
			m_##tname##EventList.clear(); \
			return; \
		} \
		tname##EventStruct s; \
		while (m_##tname##EventList.get_head(s)) { \
			bool ok = true; \
			Kty key = Ktr(std::tr1::get<1>(s)); \
			switch (std::tr1::get<0>(s)) { \
			case SQL_OP_UPDATE: { \
				db::t##tname d; \
				if (m_pTableMgr->Get##tname##Table()->GetData(key, &d) == 0) \
					ok = Update##tname(key, d, std::tr1::get<2>(s)); \
				break; } \
			case SQL_OP_INSERT: { \
				db::t##tname d; \
				if (m_pTableMgr->Get##tname##Table()->GetData(key, &d) == 0) \
					ok = Insert##tname(d); \
				break; } \
			case SQL_OP_DELETE: { \
				ok = Delete##tname(key); \
				break; } \
			default: \
				I8_ASSERT(0); \
				break; \
			} \
			if (!ok) { \
				I8_ERROR((LM_ERROR, \
					I8_TEXT("DB.SQL操作失败！\nsql=%s\nerror=%s\n"), \
					m_pSqlBuf, m_pDatabase->GetErrInfo())); \
			} \
		} \
	}

#define IMPLMENT_EVENTHANDLER_2K_OBJECT(tname, flag, Kty1, Ktr1, Kty2, Ktr2) \
	namespace DBWriter { \
	class Z##tname##EventHandler : public I##tname##EventHandler  \
	{ \
	public: \
		Z##tname##EventHandler(ZDBWriter *pDBWriter, uint32 optypes) \
			: m_pDBWriter(pDBWriter), m_optypes(optypes) \
		{ \
		} \
		uint32 GetEvents(void) \
		{  \
			return m_optypes;  \
		} \
		void HandleEvent(uint32 optype, Kty1 key1, Kty2 key2, uint64 mask) \
		{ \
			m_pDBWriter->Add##tname##Event(optype, key1, key2, mask); \
		} \
	private: \
		ZDBWriter *m_pDBWriter; \
		uint32 m_optypes; \
	}; \
	} \
	void ZDBWriter::Process##tname##Events(uint32 threshold) \
	{  \
		if (m_##tname##EventList.size() < threshold) \
			return; \
		m_pTableMgr->Save(FLAG_##flag##_BIT); \
		if (!m_pDatabase) { \
			m_##tname##EventList.clear(); \
			return; \
		} \
		tname##EventStruct s; \
		while (m_##tname##EventList.get_head(s)) { \
			bool ok = true; \
			Kty1 key1 = Ktr1(std::tr1::get<1>(s)); \
			Kty2 key2 = Ktr2(std::tr1::get<2>(s)); \
			switch (std::tr1::get<0>(s)) { \
			case SQL_OP_UPDATE: { \
				db::t##tname d; \
				if (m_pTableMgr->Get##tname##Table()->GetData(key1, key2, &d) == 0) \
					ok = Update##tname(key1, key2, d, std::tr1::get<3>(s)); \
				break; } \
			case SQL_OP_INSERT: { \
				db::t##tname d; \
				if (m_pTableMgr->Get##tname##Table()->GetData(key1, key2, &d) == 0) \
					ok = Insert##tname(d); \
				break; } \
			case SQL_OP_DELETE: { \
				ok = Delete##tname(key1, key2); \
				break; } \
			default: \
				I8_ASSERT(0); \
				break; \
			} \
			if (!ok) { \
				I8_ERROR((LM_ERROR, \
					I8_TEXT("DB.SQL操作失败！\nsql=%s\nerror=%s\n"), \
					m_pSqlBuf, m_pDatabase->GetErrInfo())); \
			} \
		} \
	}

//写数据到文件的间隔时间
#define WRITE_DB_INTERVAL		(60*1000*5)  //ms
#define WRITE_DB_INTERVAL_MIN	(60*1000)
#define WRITE_DB_INTERVAL_MAX	(WRITE_DB_INTERVAL*10)
         
//达到刷新文件的操作阀值
#define SQL_OP_FLUSH_NUM		10 //个操作/表
#define SQL_OP_FLUSH_NUM_MIN	1
#define SQL_OP_FLUSH_NUM_MAX	(SQL_OP_FLUSH_NUM*10)

namespace i8desk {

IMPLMENT_EVENTHANDLER_OBJECT(Game, GAME, uint32, KEY_TYPE)
IMPLMENT_EVENTHANDLER_OBJECT(Class, CLASS, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Area, AREA, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_2K_OBJECT(GameArea, GAMEAREA, uint32, KEY_TYPE, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Client, CLIENT, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(CmpStartTask, CMPSTARTTASK, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(SysOpt, SYSOPT, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(VDisk, VDISK, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(User, USER, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Favorite, FAVORITE, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(SyncTask, SYNCTASK, const char *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_2K_OBJECT(SyncGame, SYNCGAME, const char *, STDSTRING_TO_CSTR, uint32, KEY_TYPE)

//////////////////////////////////////////////////////
// ZDBWriter
//
ZDBWriter::	ZDBWriter(ZTableMgr *pTableMgr)
	: m_pTableMgr(pTableMgr)
	, m_pDatabase(0)
	, m_bClose(false)
{
	m_pSqlBuf = new char[1024*1024];
}

ZDBWriter::~ZDBWriter(void)
{
	delete[] m_pSqlBuf;
}

void ZDBWriter::SetDatabase(IDatabase *pDatabase)
{
	m_pDatabase = pDatabase;
}

void ZDBWriter::SaveData()
{
	m_bClose = true;
	SetEvent();
}

bool ZDBWriter::ExecSql()
{
	I8_ASSERT(m_pDatabase);
	m_pDatabase->Lock();
	bool b = m_pDatabase->ExecSql(m_pSqlBuf);
	m_pDatabase->UnLock();
	return b;
}

//thread init fini
int ZDBWriter::Init(void)
{
	CREATE_EVENTHANDLER_OBJECT(Game)
	CREATE_EVENTHANDLER_OBJECT(Class)
	CREATE_EVENTHANDLER_OBJECT(Area)
	CREATE_EVENTHANDLER_OBJECT(GameArea)
	CREATE_EVENTHANDLER_OBJECT(Client)
	CREATE_EVENTHANDLER_OBJECT(CmpStartTask)
	CREATE_EVENTHANDLER_OBJECT(SysOpt)
	CREATE_EVENTHANDLER_OBJECT(VDisk)
	CREATE_EVENTHANDLER_OBJECT(User)
	CREATE_EVENTHANDLER_OBJECT(Favorite)
	CREATE_EVENTHANDLER_OBJECT(SyncTask)
	CREATE_EVENTHANDLER_OBJECT(SyncGame)

	return 0;
}

void ZDBWriter::Fini(void)
{
	DESTROY_EVENTHANDLER_OBJECT(Game)
	DESTROY_EVENTHANDLER_OBJECT(Class)
	DESTROY_EVENTHANDLER_OBJECT(Area)
	DESTROY_EVENTHANDLER_OBJECT(GameArea)
	DESTROY_EVENTHANDLER_OBJECT(Client)
	DESTROY_EVENTHANDLER_OBJECT(CmpStartTask)
	DESTROY_EVENTHANDLER_OBJECT(SysOpt)
	DESTROY_EVENTHANDLER_OBJECT(VDisk)
	DESTROY_EVENTHANDLER_OBJECT(User)
	DESTROY_EVENTHANDLER_OBJECT(Favorite)
	DESTROY_EVENTHANDLER_OBJECT(SyncTask)
	DESTROY_EVENTHANDLER_OBJECT(SyncGame)
}

bool ZDBWriter::InsertClass(const db::tClass &d)
{	
	sprintf(m_pSqlBuf, " INSERT INTO tClass "
		" (	"
		" DefClass, "
		" Name, "
		" Path, "
		" Type "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //DefClass, 
		" '%s', " //Name, 
		" '%s', " //Path, 
		" '%s'  " //Type, 
		" ) ",
		d.DefClass,
		d.Name,
		d.Path,
		d.Type
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateClass(const std::string& DefClass,
							const db::tClass &d, 
							uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TCLASS_DEFCLASS);
	
	int len = sprintf(m_pSqlBuf, " UPDATE tClass SET ");

	if (mask & MASK_TCLASS_NAME) 
		len += sprintf(m_pSqlBuf + len, " Name = '%s', ", d.Name);
	
	if (mask & MASK_TCLASS_PATH) 
		len += sprintf(m_pSqlBuf + len, " Path = '%s', ", d.Path);
	
	if (mask & MASK_TCLASS_TYPE) 
		len += sprintf(m_pSqlBuf + len, " Type = '%s', ", d.Type);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE DefClass = '%s' ", d.DefClass);

	return this->ExecSql();
}

bool ZDBWriter::DeleteClass(const std::string& DefClass)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tClass WHERE DefClass = '%s' ", 
		DefClass.c_str());

	return this->ExecSql();
}

bool ZDBWriter::InsertGame(const db::tGame &g)
{
	sprintf(m_pSqlBuf, " INSERT INTO tGame "
		" (	"
		" GID, "
		" PID, "
		" Name, "
		" DefClass, "
		" GameExe, "
		" Param, "
		" [Size], "	
		" URL, "
		" DeskLnk, "
		" Toolbar, "
		" SvrPath, "
		" CliPath, "
		" Priority, "
		" DownPriority, "
		" ShowPriority, "
		" MatchFile, "
		" SaveFilter, "
		" IdcUptDate, "
		" SvrUptDate, "
		" IdcVer, "
		" SvrVer, "
		" AutoUpt, "
		" I8Play, "
		" IdcClick, "
		" SvrClick, "
		" AddDate, "
		" Status, "
		" IfUpdate, "
		" IfDisplay, "
		" Comment, "
		" Auto, "
		" Declinectr, "
		" hide, "
		" LocalPath, "
		" SaveFilter2, "
		" GameExe2, "
		" Param2, "
		" SvrClick2, "
		" Url1, "
		" Url2, "
		" Url3, "
		" Url4, "
		" AutoSync, "
		" GameSource "
		" ) "
		" VALUES "
		" ( "
		" %d, " //GID, 
		" %d, " //PID, 
		" '%s', " //Name, 
		" '%s', " //DefClass, 
		" '%s', " //GameExe, 
		" '%s', " //Param, 
		" %d, " //Size, 
		" '%s', " //URL, 
		" %d, " //DeskLnk, 
		" %d, " //Toolbar, 
		" '%s', " //SvrPath, 
		" '%s', " //CliPath, 
		" %d, " //Priority, 
		" %d, " //DownPriority
		" %d, " //ShowPriority,
		" '%s', " //MatchFile, 
		" '%s', " //SaveFilter, 
		" %d, " //IdcUptDate, 
		" %d, " //SvrUptDate, 
		" %d, " //IdcVer, 
		" %d, " //SvrVer, 
		" %d, " //AutoUpt, 
		" %d, " //I8Play, 
		" %d, " //IdcClick, 
		" %d, " //SvrClick, 
		" %d, " //AddDate, 
		" %d, " //Status, 
		" %d, " //IfUpdate, 
		" %d, " //IfDisplay, 
		" '%s', " //Comment, 
		" %d, " //Auto, 
		" %d, " //Declinectr, 
		" %d, " //hide, 
		" '%s', " //LocalPath, 
		" '%s', " //SaveFilter2, 
		" '%s', " //GameExe2, 
		" '%s', " //Param2, 
		" %d, " //SvrClick2
		" '%s', " //url1, 
		" '%s', " //url2, 
		" '%s', " //url3, 
		" '%s', " //url4, 
		" %d, "	// AutoSync
		" '%s' " //GameSource,
		" ) ",
		g.GID,
		g.PID,
		g.Name,
		g.DefClass,
		g.GameExe,
		g.Param,
		g.Size,	
		g.URL,
		g.DeskLnk,
		g.Toolbar,
		g.SvrPath,
		g.CliPath,
		g.Priority,
		g.DownPriority,
		g.ShowPriority,
		g.MatchFile,
		g.SaveFilter,
		g.IdcUptDate,
		g.SvrUptDate,
		g.IdcVer,
		g.SvrVer,
		g.AutoUpt,
		g.I8Play,
		g.IdcClick,
		g.SvrClick,
		g.AddDate,
		g.Status,
		g.IfUpdate,
		g.IfDisplay,
		g.Comment,
		g.Auto,
		g.Declinectr,
		g.hide,
		g.LocalPath,
		g.SaveFilter2,
		g.GameExe2,
		g.Param2,
		g.SvrClick2,
		g.url1,
		g.url2,
		g.url3,
		g.url4,
		g.AutoSync,
		g.GameSource);

	return this->ExecSql();
}

bool ZDBWriter::UpdateGame(const uint32& gid, const db::tGame &g, uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TGAME_GID);

	int len = sprintf(m_pSqlBuf, " UPDATE tGame SET ");

	if (mask & MASK_TGAME_PID) 
		len += sprintf(m_pSqlBuf + len, " PID = %d, ", g.PID);

	if (mask & MASK_TGAME_NAME) 
		len += sprintf(m_pSqlBuf + len, " Name = '%s', ", g.Name);
	
	if (mask & MASK_TGAME_DEFCLASS) 
		len += sprintf(m_pSqlBuf + len, " DefClass = '%s', ", g.DefClass);
	
	if (mask & MASK_TGAME_GAMEEXE) 
		len += sprintf(m_pSqlBuf + len, " GameExe = '%s', ", g.GameExe);
	
	if (mask & MASK_TGAME_PARAM) 
		len += sprintf(m_pSqlBuf + len, " Param = '%s', ", g.Param);
	
	if (mask & MASK_TGAME_SIZE) 
		len += sprintf(m_pSqlBuf + len, " [Size] = %d, ", g.Size);
	
	if (mask & MASK_TGAME_URL) 
		len += sprintf(m_pSqlBuf + len, " URL = '%s', ", g.URL);
	
	if (mask & MASK_TGAME_DESKLNK) 
		len += sprintf(m_pSqlBuf + len, " DeskLnk = %d, ", g.DeskLnk);
	
	if (mask & MASK_TGAME_TOOLBAR) 
		len += sprintf(m_pSqlBuf + len, " Toolbar = %d, ", g.Toolbar);
	
	if (mask & MASK_TGAME_SVRPATH) 
		len += sprintf(m_pSqlBuf + len, " SvrPath = '%s', ", g.SvrPath);
	
	if (mask & MASK_TGAME_CLIPATH) 
		len += sprintf(m_pSqlBuf + len, " CliPath = '%s', ", g.CliPath);
	
	if (mask & MASK_TGAME_PRIORITY) 
		len += sprintf(m_pSqlBuf + len, " Priority = %d, ", g.Priority);
	
	if (mask & MASK_TGAME_DOWNPRIORITY) 
		len += sprintf(m_pSqlBuf + len, " DownPriority = %d, ", g.DownPriority);
	
	if (mask & MASK_TGAME_SHOWPRIORITY) 
		len += sprintf(m_pSqlBuf + len, " ShowPriority = %d, ", g.ShowPriority);
	
	if (mask & MASK_TGAME_MATCHFILE) 
		len += sprintf(m_pSqlBuf + len, " MatchFile = '%s', ", g.MatchFile);
	
	if (mask & MASK_TGAME_SAVEFILTER) 
		len += sprintf(m_pSqlBuf + len, " SaveFilter = '%s', ", g.SaveFilter);
	
	if (mask & MASK_TGAME_IDCUPTDATE) 
		len += sprintf(m_pSqlBuf + len, " IdcUptDate = %d, ", g.IdcUptDate);
	
	if (mask & MASK_TGAME_SVRUPTDATE) 
		len += sprintf(m_pSqlBuf + len, " SvrUptDate = %d, ", g.SvrUptDate);
	
	if (mask & MASK_TGAME_IDCVER) 
		len += sprintf(m_pSqlBuf + len, " IdcVer = %d, ", g.IdcVer);
	
	if (mask & MASK_TGAME_SVRVER) 
		len += sprintf(m_pSqlBuf + len, " SvrVer = %d, ", g.SvrVer);
	
	if (mask & MASK_TGAME_AUTOUPT) 
		len += sprintf(m_pSqlBuf + len, " AutoUpt = %d, ", g.AutoUpt);
	
	if (mask & MASK_TGAME_I8PLAY) 
		len += sprintf(m_pSqlBuf + len, " I8Play = %d, ", g.I8Play);
	
	if (mask & MASK_TGAME_IDCCLICK) 
		len += sprintf(m_pSqlBuf + len, " IdcClick = %d, ", g.IdcClick);
	
	if (mask & MASK_TGAME_SVRCLICK) 
		len += sprintf(m_pSqlBuf + len, " SvrClick = %d, ", g.SvrClick);
	
	if (mask & MASK_TGAME_ADDDATE) 
		len += sprintf(m_pSqlBuf + len, " AddDate = %d, ", g.AddDate);
	
	if (mask & MASK_TGAME_STATUS) 
		len += sprintf(m_pSqlBuf + len, " Status = %d, ", g.Status);
	
	if (mask & MASK_TGAME_IFUPDATE) 
		len += sprintf(m_pSqlBuf + len, " IfUpdate = %d, ", g.IfUpdate);
	
	if (mask & MASK_TGAME_IFDISPLAY) 
		len += sprintf(m_pSqlBuf + len, " IfDisplay = %d, ", g.IfDisplay);
	
	if (mask & MASK_TGAME_COMMENT) 
		len += sprintf(m_pSqlBuf + len, " Comment = '%s', ", g.Comment);
	
	if (mask & MASK_TGAME_AUTO) 
		len += sprintf(m_pSqlBuf + len, " Auto = %d, ", g.Auto);
	
	if (mask & MASK_TGAME_DECLINECTR) 
		len += sprintf(m_pSqlBuf + len, " Declinectr = %d, ", g.Declinectr);
	
	if (mask & MASK_TGAME_HIDE) 
		len += sprintf(m_pSqlBuf + len, " hide = %d, ", g.hide);
	
	if (mask & MASK_TGAME_LOCALPATH) 
		len += sprintf(m_pSqlBuf + len, " LocalPath = '%s', ", g.LocalPath);
	
	if (mask & MASK_TGAME_SAVEFILTER2) 
		len += sprintf(m_pSqlBuf + len, " SaveFilter2 = '%s', ", g.SaveFilter2);
	
	if (mask & MASK_TGAME_GAMEEXE2) 
		len += sprintf(m_pSqlBuf + len, " GameExe2 = '%s', ", g.GameExe2);
	
	if (mask & MASK_TGAME_PARAM2) 
		len += sprintf(m_pSqlBuf + len, " Param2 = '%s', ", g.Param2);
	
	if (mask & MASK_TGAME_SVRCLICK2) 
		len += sprintf(m_pSqlBuf + len, " SvrClick2 = %d, ", g.SvrClick2);

	if (mask & MASK_TGAME_URL1) 
		len += sprintf(m_pSqlBuf + len, " Url1 = '%s', ", g.url1);

	if (mask & MASK_TGAME_URL2) 
		len += sprintf(m_pSqlBuf + len, " Url2 = '%s', ", g.url2);

	if (mask & MASK_TGAME_URL3) 
		len += sprintf(m_pSqlBuf + len, " Url3 = '%s', ", g.url3);

	if (mask & MASK_TGAME_URL4) 
		len += sprintf(m_pSqlBuf + len, " Url4 = '%s', ", g.url4);

	if( mask & MASK_TGAME_AUTOSYNC)
		len += sprintf(m_pSqlBuf + len, " AutoSync = %d, ", g.AutoSync);

	if (mask & MASK_TGAME_GAMESOURCE) 
		len += sprintf(m_pSqlBuf + len, " GameSource = '%s', ", g.GameSource);


	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE GID = %d ", gid);

	return this->ExecSql();
}

bool ZDBWriter::DeleteGame(const uint32& gid)
{	
	sprintf(m_pSqlBuf, " DELETE * FROM tGame WHERE GID = %d ", gid);

	return this->ExecSql();
}

bool ZDBWriter::InsertArea(const db::tArea &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tArea "
		" (	"
		" AID, "
		" Name, "
		" SID "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //AID, 
		" '%s', " //Name, 
		" '%s'  " //SID, 
		" ) ",
		d.AID,
		d.Name,
		d.SID
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateArea(const std::string& AID, const db::tArea &d, uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TAREA_AID);

	int len = sprintf(m_pSqlBuf, " UPDATE tArea SET ");

	if (mask & MASK_TAREA_NAME) 
		len += sprintf(m_pSqlBuf + len, " Name = '%s', ", d.Name);
	
	if (mask & MASK_TAREA_SID) 
		len += sprintf(m_pSqlBuf + len, " SID = '%s', ", d.SID);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE AID = '%s' ", AID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteArea(const std::string& AID)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tArea WHERE AID = '%s' ", AID.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertGameArea(const db::tGameArea &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO rGameArea "
		" (	"
		" GID, "
		" AID, "
		" RunType, "
		" VID "
		" ) "
		" VALUES "
		" ( "
		" %d, " //GID, 
		" '%s', " //AID, 
		" %d, " //RunType, 
		" '%s'  " //VID, 
		" ) ",
		d.GID,
		d.AID,
		d.RunType,
		d.VID
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateGameArea(const uint32& gid, 
							   const std::string& AID, 
							   const db::tGameArea &d,
							   uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TGAMEAREA_GID & ~MASK_TGAMEAREA_AID);
	
	int len = sprintf(m_pSqlBuf, " UPDATE rGameArea SET ");

	if (mask & MASK_TGAMEAREA_RUNTYPE) 
		len += sprintf(m_pSqlBuf + len, " RunType = %d, ", d.RunType);
	
	if (mask & MASK_TGAMEAREA_VID) 
		len += sprintf(m_pSqlBuf + len, " VID = '%s', ", d.VID);
	

	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE GID = %d AND AID = '%s' ", gid, AID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteGameArea(const uint32& gid, const std::string& AID)
{
	sprintf(m_pSqlBuf, 
		" DELETE * FROM rGameArea WHERE GID = %d AND AID = '%s' ",
		gid, AID.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertClient(const db::tClient &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tClient "
		" (	"
		" Name, "
		" IP, "
		" Mark, "
		" NetGate, "
		" DNS, "
		" DNS2, "
		" MAC, "
		" AID, "
		" ProtInstall, "
		" AllArea, "
		" ProtArea, "
		" ProtVer, "
		" MenuVer, "
		" CliSvrVer, "
		" GPFreeSize, "
		" SCStatus, "
		" CPU, "
		" Memory, "
		" Disk, "
		" Graphics, "
		" Mainboard, "
		" NIC, "
		" Webcam, "
		" System, "
		" IEProt, "
		" FDogDriver "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //Name, "
		" %d, " //IP, "
		" '%s', " //Mark, "
		" '%s', " //NetGate, "
		" '%s', " //DNS, "
		" '%s', " //DNS2, "
		" '%s', " //MAC, "
		" '%s', " //AID, "
		" %d,   " //ProtInstall, "
		" '%s', " //AllArea, "
		" '%s', " //ProtArea, "
		" '%s', " //ProtVer, "
		" '%s', " //MenuVer, "
		" '%s', " //CliSvrVer "
		" %d,   " 
		" %d,   "
		" '%s', "
		" '%s', "
		" '%s', "
		" '%s', "
		" '%s', "
		" '%s', "
		" '%s', "
		" '%s', "
		" %d,   "
		" %d    "
		" ) ",
		d.Name,  
		d.IP,  
		d.Mark,  
		d.NetGate,  
		d.DNS,  
		d.DNS2,  
		d.MAC,  
		d.AID,  
		d.ProtInstall,  
		d.AllArea,  
		d.ProtArea,  
		d.ProtVer,  
		d.MenuVer,  
		d.CliSvrVer,
		d.GPFreeSize,
		d.SCStatus,
		d.CPU,
		d.Memory,
		d.Disk,
		d.Graphics,
		d.Mainboard,
		d.NIC,
		d.Webcam,
		d.System,
		d.IEProt,
		d.FDogDriver
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateClient(const std::string& Name, const db::tClient &d, uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TCLIENT_NAME);

	int len = sprintf(m_pSqlBuf, " UPDATE tClient SET ");

	if (mask & MASK_TCLIENT_IP) 
		len += sprintf(m_pSqlBuf + len, " IP = %d, ", d.IP);
	
	if (mask & MASK_TCLIENT_MARK) 
		len += sprintf(m_pSqlBuf + len, " Mark = '%s', ", d.Mark);
	
	if (mask & MASK_TCLIENT_NETGATE) 
		len += sprintf(m_pSqlBuf + len, " NetGate = '%s', ", d.NetGate);
	
	if (mask & MASK_TCLIENT_DNS) 
		len += sprintf(m_pSqlBuf + len, " DNS = '%s', ", d.DNS);
	
	if (mask & MASK_TCLIENT_DNS2) 
		len += sprintf(m_pSqlBuf + len, " DNS2 = '%s', ", d.DNS2);
	
	if (mask & MASK_TCLIENT_MAC) 
		len += sprintf(m_pSqlBuf + len, " MAC = '%s', ", d.MAC);
	
	if (mask & MASK_TCLIENT_AID) 
		len += sprintf(m_pSqlBuf + len, " AID = '%s', ", d.AID);
	
	if (mask & MASK_TCLIENT_PROTINSTALL) 
		len += sprintf(m_pSqlBuf + len, " ProtInstall = %d, ", d.ProtInstall);
	
	if (mask & MASK_TCLIENT_ALLAREA) 
		len += sprintf(m_pSqlBuf + len, " AllArea = '%s', ", d.AllArea);
	
	if (mask & MASK_TCLIENT_PROTAREA) 
		len += sprintf(m_pSqlBuf + len, " ProtArea = '%s', ", d.ProtArea);
	
	if (mask & MASK_TCLIENT_PROTVER) 
		len += sprintf(m_pSqlBuf + len, " ProtVer = '%s', ", d.ProtVer);
	
	if (mask & MASK_TCLIENT_MENUVER) 
		len += sprintf(m_pSqlBuf + len, " MenuVer = '%s', ", d.MenuVer);
	
	if (mask & MASK_TCLIENT_CLISVRVER) 
		len += sprintf(m_pSqlBuf + len, " CliSvrVer = '%s', ", d.CliSvrVer);

	if (mask & MASK_TCLIENT_GPFREESIZE) 
		len += sprintf(m_pSqlBuf + len, " GPFreeSize = %d, ", d.GPFreeSize);

	if (mask & MASK_TCLIENT_SCSTATUS) 
		len += sprintf(m_pSqlBuf + len, " SCStatus = %d, ", d.SCStatus);

	if (mask & MASK_TCLIENT_CPU) 
		len += sprintf(m_pSqlBuf + len, " CPU = '%s', ", d.CPU);

	if (mask & MASK_TCLIENT_MEMORY) 
		len += sprintf(m_pSqlBuf + len, " Memory = '%s', ", d.Memory);

	if (mask & MASK_TCLIENT_DISK) 
		len += sprintf(m_pSqlBuf + len, " Disk = '%s', ", d.Disk);

	if (mask & MASK_TCLIENT_GRAPHICS) 
		len += sprintf(m_pSqlBuf + len, " Graphics = '%s', ", d.Graphics);

	if (mask & MASK_TCLIENT_MAINBOARD) 
		len += sprintf(m_pSqlBuf + len, " Mainboard = '%s', ", d.Mainboard);

	if (mask & MASK_TCLIENT_NIC) 
		len += sprintf(m_pSqlBuf + len, " NIC = '%s', ", d.NIC);

	if (mask & MASK_TCLIENT_WEBCAM) 
		len += sprintf(m_pSqlBuf + len, " Webcam = '%s', ", d.Webcam);

	if (mask & MASK_TCLIENT_SYSTEM) 
		len += sprintf(m_pSqlBuf + len, " System = '%s', ", d.System);

	if (mask & MASK_TCLIENT_IEPROT) 
		len += sprintf(m_pSqlBuf + len, " IEProt = %d, ", d.IEProt);

	if (mask & MASK_TCLIENT_FDOGDRIVER) 
		len += sprintf(m_pSqlBuf + len, " FDogDriver = %d, ", d.FDogDriver);

	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE Name = '%s' ", Name.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteClient(const std::string& Name)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tClient WHERE Name = '%s' ", Name.c_str());

	return this->ExecSql();
}

bool ZDBWriter::InsertFavorite(const db::tFavorite &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tFavorite "
		" (	"
		" Serial, "
		" UID, "
		" Name, "
		" Type, "
		" URL "
		" ) "
		" VALUES "
		" ( "
		" %d, " //Serial
		" '%s', " //UID, 
		" '%s', " //Name, 
		" '%s',  " //Type, 
		" '%s' " //URL, 
		" ) ",
		d.Serial,
		d.UID,
		d.Name,
		d.Type,
		d.URL
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateFavorite(const std::string& UID,
							   const db::tFavorite &d, 
							   uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TFAVORITE_UID);

	int len = sprintf(m_pSqlBuf, " UPDATE tFavorite SET ");

	if (mask & MASK_TFAVORITE_SERIAL) 
		len += sprintf(m_pSqlBuf + len, " Serial = %d, ", d.Serial);
	
	if (mask & MASK_TFAVORITE_NAME) 
		len += sprintf(m_pSqlBuf + len, " Name = '%s', ", d.Name);
	
	if (mask & MASK_TFAVORITE_TYPE) 
		len += sprintf(m_pSqlBuf + len, " Type = '%s', ", d.Type);
	
	if (mask & MASK_TFAVORITE_URL) 
		len += sprintf(m_pSqlBuf + len, " URL = '%s', ", d.URL);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE UID = '%s' ", UID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteFavorite(const std::string& UID)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tFavorite WHERE UID = '%s' ", UID.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertCmpStartTask(const db::tCmpStartTask &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tCmpStartTask "
		" (	"
		" UID, "
		" AreaType, "
		" AreaParam, "
		" Type, "
		" Flag, "
		" Content "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //UID, 
		" %d, " //AreaType, 
		" '%s', " //AreaParam, 
		" %d,  " //Type, 
		" %d,  " //Flag, 
		" '%s'  " //Content, 
		" ) ",
		d.UID,
		d.AreaType,
		d.AreaParam,
		d.Type,
		d.Flag,
		d.Content
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateCmpStartTask(const std::string& UID,
								   const db::tCmpStartTask &d,
								   uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TCMPSTARTTASK_UID);

	int len = sprintf(m_pSqlBuf, " UPDATE tCmpStartTask SET ");

	if (mask & MASK_TCMPSTARTTASK_AREATYPE) 
		len += sprintf(m_pSqlBuf + len, " AreaType = %d, ", d.AreaType);
	
	if (mask & MASK_TCMPSTARTTASK_AREAPARAM) 
		len += sprintf(m_pSqlBuf + len, " AreaParam = '%s', ", d.AreaParam);
	
	if (mask & MASK_TCMPSTARTTASK_TYPE) 
		len += sprintf(m_pSqlBuf + len, " Type = %d, ", d.Type);
	
	if (mask & MASK_TCMPSTARTTASK_FLAG) 
		len += sprintf(m_pSqlBuf + len, " Flag = %d, ", d.Flag);
	
	if (mask & MASK_TCMPSTARTTASK_CONTENT) 
		len += sprintf(m_pSqlBuf + len, " Content = '%s', ", d.Content);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE UID = '%s' ", UID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteCmpStartTask(const std::string& UID)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tCmpStartTask WHERE UID = '%s' ", UID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::InsertVDisk(const db::tVDisk &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tVDisk "
		" (	"
		" VID, "
		" IP, "
		" Port, "
		" SvrDrv, "
		" CliDrv, "
		" Serial, "
		" Type, "
		" LoadType, "
		" SID "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //VID, 
		" %d, " //IP, 
		" %d, " //Port, 
		" %d, " //SvrDrv, 
		" %d, " //CliDrv, 
		" %d, " //Serial, 
		" %d, " //Type, 
		" %d, " //LoadType, 
		" '%s' " //SID
		" ) ",
		d.VID,
		d.IP,
		d.Port,
		d.SvrDrv,
		d.CliDrv,
		d.Serial,
		d.Type,
		d.LoadType,
		d.SID
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateVDisk(const std::string& VID, const db::tVDisk &d, uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TVDISK_VID);

	int len = sprintf(m_pSqlBuf, " UPDATE tVDisk SET ");

	if (mask & MASK_TVDISK_IP) 
		len += sprintf(m_pSqlBuf + len, " IP = %d, ", d.IP);
	
	if (mask & MASK_TVDISK_PORT) 
		len += sprintf(m_pSqlBuf + len, " Port = %d, ", d.Port);
	
	if (mask & MASK_TVDISK_SVRDRV) 
		len += sprintf(m_pSqlBuf + len, " SvrDrv = %d, ", d.SvrDrv);
	
	if (mask & MASK_TVDISK_CLIDRV) 
		len += sprintf(m_pSqlBuf + len, " CliDrv = %d, ", d.CliDrv);
	
	if (mask & MASK_TVDISK_SERIAL) 
		len += sprintf(m_pSqlBuf + len, " Serial = %d, ", d.Serial);
	
	if (mask & MASK_TVDISK_TYPE) 
		len += sprintf(m_pSqlBuf + len, " Type = %d, ", d.Type);
	
	if (mask & MASK_TVDISK_LOADTYPE) 
		len += sprintf(m_pSqlBuf + len, " LoadType = %d, ", d.LoadType);
	
	if (mask & MASK_TVDISK_SID)
		len += sprintf(m_pSqlBuf + len, " SID = '%s', ", d.SID);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE VID = '%s' ", VID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteVDisk(const std::string& VID)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tVDisk WHERE VID = '%s' ", VID.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertUser(const db::tUser &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tUser "
		" (	"
		" Name, "
		" Pwd, "
		" RoomSize, "
		" CreateDate, "
		" LastLoginDate, "
		" LastLoginIp "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //Name, 
		" '%s', " //Pwd, 
		" %d, " //RoomSize, 
		" %d, " //CreateDate, 
		" %d, " //LastLoginDate, 
		" %d  " //LastLoginIp, 
		" ) ",
		d.Name,
		d.Pwd,
		d.RoomSize,
		d.CreateDate,
		d.LastLoginDate,
		d.LastLoginIp
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateUser(const std::string& Name, const db::tUser &d, uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TUSER_NAME);

	int len = sprintf(m_pSqlBuf, " UPDATE tUser SET ");

	if (mask & MASK_TUSER_PWD) 
		len += sprintf(m_pSqlBuf + len, " Pwd = '%s', ", d.Pwd);
	
	if (mask & MASK_TUSER_ROOMSIZE) 
		len += sprintf(m_pSqlBuf + len, " RoomSize = %d, ", d.RoomSize);
	
	if (mask & MASK_TUSER_CREATEDATE) 
		len += sprintf(m_pSqlBuf + len, " CreateDate = %d, ", d.CreateDate);
	
	if (mask & MASK_TUSER_LASTLOGINDATE) 
		len += sprintf(m_pSqlBuf + len, " LastLoginDate = %d, ", d.LastLoginDate);
	
	if (mask & MASK_TUSER_LASTLOGINIP) 
		len += sprintf(m_pSqlBuf + len, " LastLoginIp = %d, ", d.LastLoginIp);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE Name = '%s' ", Name.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteUser(const std::string& Name)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tUser WHERE Name = '%s' ", Name.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertSysOpt(const db::tSysOpt &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tSysOpt "
		" (	"
		" OptName, "
		" OptValue "
		" ) "
		" VALUES "
		" ( "
		" '%s', " //OptName, 
		" '%s'  " //OptValue, 
		" ) ",
		d.key,
		d.value
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateSysOpt(const std::string& OptName,
							 const db::tSysOpt &d, 
							 uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TSYSOPT_KEY);

	int len = sprintf(m_pSqlBuf, " UPDATE tSysOpt SET ");

	if (mask & MASK_TSYSOPT_VALUE) 
		len += sprintf(m_pSqlBuf + len, " OptValue = '%s', ", d.value);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE OptName = '%s' ", OptName.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteSysOpt(const std::string& OptName)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tSysOpt WHERE OptName = '%s' ", OptName.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertSyncTask(const db::tSyncTask &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tSyncTask "
		" (	"
		" SID, "
		" Name, "
		" SyncType, "					
		" SyncIP, "
		" VDiskIP, "
		" UpdateIP, "
		" DestDrv, "					
		" BalanceType, "					
		" MaxSyncSpeed "					
		" ) "
		" VALUES "
		" ( "
		" '%s', " //SID, "
		" '%s', " //Name, "
		" %d, " //SyncType, "					
		" '%s', " //SyncIP, "
		" '%s', " //VDiskIP, "
		" '%s', " //UpdateIP, "
		" %d, " //DestDrv, "					
		" %d, " //BalanceType "		
		" %d  " //MaxSyncSpeed "		
		" ) ",
		d.SID,
		d.Name,
		d.SyncType,
		d.SyncIP,
		d.VDiskIP,
		d.UpdateIP,
		d.DestDrv,
		d.BalanceType,
		d.MaxSyncSpeed
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateSyncTask(const std::string& SID,
							   const db::tSyncTask &d, 
							   uint64 mask)
{
	I8_ASSERT(mask & ~MASK_TSYNCTASK_SID);
	
	int len = sprintf(m_pSqlBuf, " UPDATE tSyncTask SET ");

	if (mask & MASK_TSYNCTASK_NAME) 
		len += sprintf(m_pSqlBuf + len, " Name = '%s', ", d.Name);
	
	if (mask & MASK_TSYNCTASK_SYNCTYPE) 
		len += sprintf(m_pSqlBuf + len, " SyncType = %d, ", d.SyncType);
	
	if (mask & MASK_TSYNCTASK_SYNCIP) 
		len += sprintf(m_pSqlBuf + len, " SyncIP = '%s', ", d.SyncIP);
	
	if (mask & MASK_TSYNCTASK_VDISKIP) 
		len += sprintf(m_pSqlBuf + len, " VDiskIP = '%s', ", d.VDiskIP);
	
	if (mask & MASK_TSYNCTASK_UPDATEIP) 
		len += sprintf(m_pSqlBuf + len, " UpdateIP = '%s', ", d.UpdateIP);
	
	if (mask & MASK_TSYNCTASK_DESTDRV) 
		len += sprintf(m_pSqlBuf + len, " DestDrv = %d, ", d.DestDrv);
	
	if (mask & MASK_TSYNCTASK_BALANCETYPE) 
		len += sprintf(m_pSqlBuf + len, " BalanceType = %d, ", d.BalanceType);
	
	if (mask & MASK_TSYNCTASK_MAXSYNCSPEED) 
		len += sprintf(m_pSqlBuf + len, " MaxSyncSpeed = %d, ", d.MaxSyncSpeed);
	
	//注意消除最后一个逗号
	sprintf(m_pSqlBuf + len - 2, " WHERE SID = '%s' ", SID.c_str());

	return this->ExecSql();
}

bool ZDBWriter::DeleteSyncTask(const std::string& SID)
{
	sprintf(m_pSqlBuf, " DELETE * FROM tSyncTask WHERE SID = '%s' ", SID.c_str());

	return this->ExecSql();
}


bool ZDBWriter::InsertSyncGame(const db::tSyncGame &d)
{
	sprintf(m_pSqlBuf, " INSERT INTO tSyncGame "
		" (	"
		" SID, "
		" GID "					
		" ) "
		" VALUES "
		" ( "
		" '%s', " //SID, "
		" %d " //GID, "					
		" ) ",
		d.SID,
		d.GID
		);

	return this->ExecSql();
}

bool ZDBWriter::UpdateSyncGame(const std::string& SID, 
							   const uint32& gid, 
							   const db::tSyncGame &d, 
							   uint64 mask)
{
	I8_ASSERT(0);
	return true;
}

bool ZDBWriter::DeleteSyncGame(const std::string& SID, const uint32& gid )
{
	sprintf(m_pSqlBuf, 
		" DELETE * FROM tSyncGame WHERE SID = '%s' AND GID = %d",
		SID.c_str(), gid);

	return this->ExecSql();
}

//thread func
unsigned int ZDBWriter::Exec(void)
{
	::CoInitialize(0);

	while (1) 
	{
		long fdi = m_pTableMgr->GetOption(
			OPT_S_FLUSH_DB_INTERVAL, WRITE_DB_INTERVAL);
		if (fdi < WRITE_DB_INTERVAL_MIN)
			fdi = WRITE_DB_INTERVAL_MIN;
		if (fdi > WRITE_DB_INTERVAL_MAX)
			fdi = WRITE_DB_INTERVAL_MAX;

		DWORD dwRet = WaitEvent(fdi);
		if ( dwRet == WAIT_TIMEOUT || (dwRet == WAIT_OBJECT_0 && m_bClose == true) )
		{
			ProcessGameEvents();
			ProcessClassEvents();
			ProcessAreaEvents();
			ProcessGameAreaEvents();
			ProcessClientEvents();
			ProcessFavoriteEvents();
			ProcessCmpStartTaskEvents();
			ProcessSysOptEvents();
			ProcessUserEvents();
			ProcessVDiskEvents();
			ProcessSyncTaskEvents();
			ProcessSyncGameEvents();

			I8_INFOR((LM_INFO, I8_TEXT("保存完所有数据到数据库!\n")));
			m_bClose = false;
		}
		else
		{
			long fdo = m_pTableMgr->GetOption(
				OPT_S_FLUSH_DB_OPNUMBER, SQL_OP_FLUSH_NUM);
			if (fdo < SQL_OP_FLUSH_NUM_MIN)
				fdo = SQL_OP_FLUSH_NUM_MIN;
			if (fdo > SQL_OP_FLUSH_NUM_MAX)
				fdo = SQL_OP_FLUSH_NUM_MAX;
			ProcessGameEvents( fdo );
			ProcessClassEvents( fdo );
			ProcessAreaEvents( fdo );
			ProcessGameAreaEvents( fdo );
			ProcessClientEvents( fdo );
			ProcessFavoriteEvents( fdo );
			ProcessCmpStartTaskEvents( fdo );
			ProcessSysOptEvents( fdo );
			ProcessUserEvents( fdo );
			ProcessVDiskEvents( fdo );
			ProcessSyncTaskEvents( fdo );
			ProcessSyncGameEvents( fdo );
		}

		if (IsDone()) {
			ProcessGameEvents();
			ProcessClassEvents();
			ProcessAreaEvents();
			ProcessGameAreaEvents();
			ProcessClientEvents();
			ProcessFavoriteEvents();
			ProcessCmpStartTaskEvents();
			ProcessSysOptEvents();
			ProcessUserEvents();
			ProcessVDiskEvents();
			ProcessSyncTaskEvents();
			ProcessSyncGameEvents();
			break;
		}
	}
	
	::CoUninitialize();

	return 0;
}



} //namespace i8desk
