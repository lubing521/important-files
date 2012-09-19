#include "StdAfx.h"
#include "dbwriter.h"
#include "tablemgr.h"
#include "LogHelper.h"
#include "../../../include/frame.h"
#include "../../../include/Extend STL/UnicodeStl.h"

#include <string>

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
				assert(0); \
				break; \
			} \
			if (!ok) { \
				Log(LM_ERROR, \
					_T("DB.SQL操作失败！\nsql=%s\nerror=%s\n"), \
					os_.str().c_str(), m_pDatabase->GetErrInfo()); \
					os_.str(_T(""));\
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
				assert(0); \
				break; \
			} \
			if (!ok) { \
				Log(LM_ERROR, \
					_T("DB.SQL操作失败！\nsql=%s\nerror=%s\n"), \
					os_.str().c_str(), m_pDatabase->GetErrInfo()); \
					os_.str(_T(""));\
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


namespace i8desk
{
	template < typename CharT, size_t N, typename T, typename U>
	inline stdex::tOstringstream &PutValue(stdex::tOstringstream &os, const CharT (&arr)[N], const T &val, U c)
	{
		os << arr << val << c;
		return os;
	}

	template < typename CharT, size_t N, typename ValueT, size_t M, typename U>
	inline stdex::tOstringstream &PutValue(stdex::tOstringstream &os, const CharT (&arr)[N], const ValueT (&val)[M], U c)
	{
		os << arr << '\'' << val << '\'' << c;
		return os;
	}

	template < typename CharT >
	inline stdex::tOstringstream &operator<<(stdex::tOstringstream &os, const std::basic_string<CharT> &arr)
	{
		os << '\'' << arr.c_str() << '\'';
		return os;
	}

	typedef stdex::tOstringstream &(Func)(stdex::tOstringstream &os);

	stdex::tOstringstream &FixUpdate(stdex::tOstringstream &os)
	{
		os.rdbuf()->pubseekoff(-1, std::ios::cur);
		return os;
	}

	inline stdex::tOstringstream &operator<<(stdex::tOstringstream &os, stdex::tOstringstream &)
	{
		return os;
	}
}

namespace i8desk {


//IMPLMENT_EVENTHANDLER_OBJECT(Game, GAME, uint32, KEY_TYPE)
	namespace DBWriter 
	{ 
		class ZGameEventHandler : public IGameEventHandler  
		{ 
		public: 
			ZGameEventHandler(ZDBWriter *pDBWriter, uint32 optypes) 
				: m_pDBWriter(pDBWriter), m_optypes(optypes) 
			{ 
			} 
			uint32 GetEvents(void) 
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, uint32 key, uint64 mask) 
			{ 
				m_pDBWriter->AddGameEvent(optype, key, mask); 
			} 
		private: 
			ZDBWriter *m_pDBWriter; 
			uint32 m_optypes; 
		}; 
	} 
	void ZDBWriter::ProcessGameEvents(uint32 threshold) 
	{  
		if (m_GameEventList.size() < threshold) 
			return; 
		m_pTableMgr->Save(FLAG_GAME_BIT); 
		if (!m_pDatabase) 
		{ 
			m_GameEventList.clear(); 
			return; 
		} 
		GameEventStruct s; 
		while (m_GameEventList.get_head(s)) 
		{ 
			bool ok = true; 
			uint32 key = KEY_TYPE(std::tr1::get<1>(s)); 
			switch (std::tr1::get<0>(s)) 
			{ 
			case SQL_OP_UPDATE: 
				{ 
				db::tGame d; 
				if (m_pTableMgr->GetGameTable()->GetData(key, &d) == 0) 
					ok = UpdateGame(key, d, std::tr1::get<2>(s)); 
				break; 
				} 
			case SQL_OP_INSERT:
				{ 
				db::tGame d; 
				if (m_pTableMgr->GetGameTable()->GetData(key, &d) == 0) 
					ok = InsertGame(d); 
				break;
				} 
			case SQL_OP_DELETE: 
				{ 
				ok = DeleteGame(key); 
				break; 
				} 
			default: 
				assert(0); 
				break; 
			} 
			if (!ok) 
			{ 
				Log(LM_ERROR, 
					_T("DB.SQL操作失败！\nsql=%s\nerror=%s\n"), 
					os_.str().c_str(), m_pDatabase->GetErrInfo()); 
				os_.str(_T(""));
			} 
		} 
	}
IMPLMENT_EVENTHANDLER_OBJECT(PlugTool, PLUGTOOL, uint32, KEY_TYPE)
IMPLMENT_EVENTHANDLER_OBJECT(Class, CLASS, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Area, AREA, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_2K_OBJECT(RunType, RUNTYPE, uint32, KEY_TYPE, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Client, CLIENT, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(CmpBootTask, CMPBOOTTASK, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(SysOpt, SYSOPT, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(VDisk, VDISK, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Favorite, FAVORITE, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(SyncTask, SYNCTASK, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(Server, SERVER, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_2K_OBJECT(BootTaskArea, BOOTTASKAREA, const TCHAR *, STDSTRING_TO_CSTR, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_2K_OBJECT(SyncGame, SYNCGAME,  uint32, KEY_TYPE, const TCHAR *, STDSTRING_TO_CSTR)
IMPLMENT_EVENTHANDLER_OBJECT(PushGameStatic, PUSHGAMESTATIC, uint32, KEY_TYPE)

 

//////////////////////////////////////////////////////
// ZDBWriter
//
ZDBWriter::	ZDBWriter(ZTableMgr *pTableMgr)
	: m_pTableMgr(pTableMgr)
	, m_pDatabase(0)
	, m_bClose(false)
{
	//m_pSqlBuf = new TCHAR[1024*1024];
}

ZDBWriter::~ZDBWriter(void)
{
	Shutdown();
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
	assert(m_pDatabase);
	m_pDatabase->Lock();
	static stdex::tString buf;
	buf = os_.str();
	bool b = m_pDatabase->ExecSql(buf.c_str());
	m_pDatabase->UnLock();
	if(b)os_.str(_T(""));
	
	return b;
}

//thread init fini
int ZDBWriter::Init(void)
{
	CREATE_EVENTHANDLER_OBJECT(Game)
	CREATE_EVENTHANDLER_OBJECT(Class)
	CREATE_EVENTHANDLER_OBJECT(Area)
	CREATE_EVENTHANDLER_OBJECT(RunType)
	CREATE_EVENTHANDLER_OBJECT(Client)
	CREATE_EVENTHANDLER_OBJECT(CmpBootTask)
	CREATE_EVENTHANDLER_OBJECT(SysOpt)
	CREATE_EVENTHANDLER_OBJECT(VDisk)
	CREATE_EVENTHANDLER_OBJECT(PlugTool)
	CREATE_EVENTHANDLER_OBJECT(Favorite)
	CREATE_EVENTHANDLER_OBJECT(SyncTask)
	CREATE_EVENTHANDLER_OBJECT(Server)
	CREATE_EVENTHANDLER_OBJECT(BootTaskArea)
	CREATE_EVENTHANDLER_OBJECT(SyncGame)
	CREATE_EVENTHANDLER_OBJECT(PushGameStatic)

	return 0;
}

void ZDBWriter::Fini(void)
{
	DESTROY_EVENTHANDLER_OBJECT(Game)
	DESTROY_EVENTHANDLER_OBJECT(Class)
	DESTROY_EVENTHANDLER_OBJECT(Area)
	DESTROY_EVENTHANDLER_OBJECT(RunType)
	DESTROY_EVENTHANDLER_OBJECT(Client)
	DESTROY_EVENTHANDLER_OBJECT(CmpBootTask)
	DESTROY_EVENTHANDLER_OBJECT(SysOpt)
	DESTROY_EVENTHANDLER_OBJECT(VDisk)
	DESTROY_EVENTHANDLER_OBJECT(PlugTool)
	DESTROY_EVENTHANDLER_OBJECT(Favorite)
	DESTROY_EVENTHANDLER_OBJECT(SyncTask)
	DESTROY_EVENTHANDLER_OBJECT(Server)
	DESTROY_EVENTHANDLER_OBJECT(BootTaskArea)
	DESTROY_EVENTHANDLER_OBJECT(SyncGame)
	DESTROY_EVENTHANDLER_OBJECT(PushGameStatic)

}

bool ZDBWriter::InsertClass(const db::tClass &d)
{	
	PutValue(os_, _T(" INSERT INTO tblClass ( CID, Name, SvrPath ) VALUES ( "), d.CID, ',');
	PutValue(os_, _T(" "), d.Name, ',');
	PutValue(os_, _T(" "), d.SvrPath, ')') << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::UpdateClass(const stdex::tString& CID,
							const db::tClass &d, 
							uint64 mask)
{
	assert(mask & ~MASK_TCLASS_DEFCLASS);

	os_ << _T(" UPDATE tblClass SET ");

	if (mask & MASK_TCLASS_NAME) 
		PutValue(os_, _T(" Name = "), d.Name, ',');

	if (mask & MASK_TCLASS_SVRPATH) 
		PutValue(os_, _T(" SvrPath = "), d.SvrPath, ',');

	PutValue(FixUpdate(os_) , _T(" WHERE CID = ") , d.CID ,_T(""));
	os_ << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteClass(const stdex::tString& CID)
{
	os_ << _T(" DELETE * FROM tblClass WHERE CID =  ")<< '\''<< CID << '\''<< std::endl;
	return this->ExecSql();
}

bool ZDBWriter::InsertGame(const db::tGame &g)
{
	os_	<< _T(" INSERT INTO tblGame ( GID, PID, CID, Name, Exe, Param, [Size], SvrPath, CliPath, ")
		<< _T(" TraitFile, SaveFilter, DeskLink, Toolbar, [Memo], GameSource, IdcAddDate, EnDel, ")
		<< _T(" IdcVer, SvrVer, IdcClick, SvrClick, SvrClick2, I8Play, AutoUpt, Priority, Force, ForceDir, ")
		<< _T(" Status, StopRun, Hide ) VALUES ( ");
		PutValue( os_, _T(""), g.GID		, _T(" , " ));//GID, 
		PutValue( os_, _T(""), g.PID		, _T(" , " ));//PID, 
		PutValue( os_, _T(""), g.CID		, _T(" , " ));//CID, 
		PutValue( os_, _T(""), g.Name		, _T(" , " )); //Name, 
		PutValue( os_, _T(""), g.Exe		, _T(" , " ));//Exe, 
		PutValue( os_, _T(""), g.Param		, _T(" , " ));//Param, 
		PutValue( os_, _T(""), g.Size		, _T(" , " ));//Size, 
		PutValue( os_, _T(""), g.SvrPath	, _T(" , " )); //SvrPath, 
		PutValue( os_, _T(""), g.CliPath	, _T(" , " ));//CliPath, 
		PutValue( os_, _T(""), g.TraitFile	, _T(" , " ));//TraitFile, 
		PutValue( os_, _T(""), g.SaveFilter	, _T(" , " ));//SaveFilter, 
		PutValue( os_, _T(""), g.DeskLink	, _T(" , " ));//DeskLink, 
		PutValue( os_, _T(""), g.Toolbar	, _T(" , " ));//Toolbar, 
		PutValue( os_, _T(""), g.Memo		, _T(" , " ));//Memo, 
		PutValue( os_, _T(""), g.GameSource	, _T(" , " )); //GameSource,
		PutValue( os_, _T(""), g.IdcAddDate	, _T(" , " ));//IdcAddDate, 
		PutValue( os_, _T(""), g.EnDel		, _T(" , " ));//EnDel, 
		PutValue( os_, _T(""), g.IdcVer		, _T(" , " ));//IdcVer, 
		PutValue( os_, _T(""), g.SvrVer		, _T(" , " ));//SvrVer, 
		PutValue( os_, _T(""), g.IdcClick	, _T(" , " ));//IdcClick, 
		PutValue( os_, _T(""), g.SvrClick	, _T(" , " ));//SvrClick, 
		PutValue( os_, _T(""), g.SvrClick2	, _T(" , " ));//SvrClick2, 
		PutValue( os_, _T(""), g.I8Play		, _T(" , " ));//I8Play, 
		PutValue( os_, _T(""), g.AutoUpt	, _T(" , " ));//AutoUpt, 
		PutValue( os_, _T(""), g.Priority	, _T(" , " ));//Priority
		PutValue( os_, _T(""), g.Force		, _T(" , " ));//Force, 
		PutValue( os_, _T(""), g.ForceDir	, _T(" , " ));//ForceDir,
		PutValue( os_, _T(""), g.Status		, _T(" , " ));//Status 
		PutValue( os_, _T(""), g.StopRun	, _T(" , " ));//StopRun 
		PutValue( os_, _T(""), g.Hide		, _T(" ) " ));//Hide 

		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateGame(const uint32& gid, const db::tGame &g, uint64 mask)
{
	assert(mask & ~MASK_TGAME_GID);

	os_ << _T(" UPDATE tblGame SET ");

	if (mask & MASK_TGAME_PID)
		PutValue(os_, _T(" PID =  "), g.PID, ',');

	if (mask & MASK_TGAME_NAME) 
		PutValue(os_ , _T(" Name =  ") , g.Name , _T(","));
	
	if (mask & MASK_TGAME_CID) 
		PutValue(os_ , _T(" CID =  ") , g.CID , _T(","));
	
	if (mask & MASK_TGAME_EXE) 
		PutValue(os_ , _T(" Exe =  ") , g.Exe , _T(","));
	
	if (mask & MASK_TGAME_PARAM) 
		PutValue(os_ , _T(" Param =  ") , g.Param , _T(","));
	
	if (mask & MASK_TGAME_SIZE) 
		PutValue(os_ , _T(" [Size] =  ") , g.Size , _T(","));
	
	if (mask & MASK_TGAME_DESKLINK) 
		PutValue(os_ , _T(" DeskLink =  ") , g.DeskLink , _T(","));
	
	if (mask & MASK_TGAME_TOOLBAR) 
		PutValue(os_ , _T(" Toolbar =  ") , g.Toolbar , _T(","));
	
	if (mask & MASK_TGAME_SVRPATH) 
		PutValue(os_ , _T(" SvrPath =  ") , g.SvrPath , _T(","));
	
	if (mask & MASK_TGAME_CLIPATH) 
		PutValue(os_ , _T(" CliPath =  ") , g.CliPath , _T(","));
	
	if (mask & MASK_TGAME_ENDEL) 
		PutValue(os_ , _T(" EnDel =  ") , g.EnDel , _T(","));
	
	if (mask & MASK_TGAME_PRIORITY) 
		PutValue(os_ , _T(" Priority =  ") , g.Priority , _T(","));
	
	if (mask & MASK_TGAME_TRAITFILE) 
		PutValue(os_ , _T(" TraitFile =  ") , g.TraitFile , _T(","));
	
	if (mask & MASK_TGAME_SAVEFILTER) 
		PutValue(os_ , _T(" SaveFilter =  ") , g.SaveFilter , _T(","));
	
	if (mask & MASK_TGAME_IDCADDDATE) 
		PutValue(os_ , _T(" IdcAddDate =  ") , g.IdcAddDate , _T(","));
	
	if (mask & MASK_TGAME_IDCVER) 
		PutValue(os_, _T(" IdcVer =  "), g.IdcVer, ',');
	
	if (mask & MASK_TGAME_SVRVER) 
		PutValue(os_ , _T(" SvrVer =  ") ,  g.SvrVer , _T(","));
	
	if (mask & MASK_TGAME_AUTOUPT) 
		PutValue(os_ , _T(" AutoUpt =  ") , g.AutoUpt , _T(","));
	
	if (mask & MASK_TGAME_I8PLAY) 
		PutValue(os_ , _T(" I8Play =  ") ,  g.I8Play , _T(","));
	
	if (mask & MASK_TGAME_IDCCLICK) 
		PutValue(os_ , _T(" IdcClick =  ") ,  g.IdcClick , _T(","));
	
	if (mask & MASK_TGAME_SVRCLICK) 
		PutValue(os_ , _T(" SvrClick =  ") ,  g.SvrClick , _T(","));

	if (mask & MASK_TGAME_SVRCLICK2) 
		PutValue(os_ , _T(" SvrClick2 =  ") ,  g.SvrClick2, _T(","));
	
	if (mask & MASK_TGAME_STATUS) 
		PutValue(os_ , _T(" Status =  ") , g.Status , _T(","));

	if (mask & MASK_TGAME_STOPRUN) 
		PutValue(os_ , _T(" StopRun =  ") , g.StopRun , _T(","));
	
	if (mask & MASK_TGAME_MEMO) 
		PutValue(os_, _T(" [Memo] =  "), g.Memo, ',');
	
	if (mask & MASK_TGAME_FORCE) 
		PutValue(os_ , _T(" Force =  ") , g.Force , _T(","));

	if (mask & MASK_TGAME_GAMESOURCE) 
		PutValue(os_ , _T(" GameSource =  ") , g.GameSource , _T(","));

	if (mask & MASK_TGAME_FORCEDIR) 
		PutValue(os_ , _T(" ForceDir =  ") , g.ForceDir , _T(","));

	if (mask & MASK_TGAME_REPAIR) 
		PutValue(os_ , _T(" [Repair] =  ") , g.Repair , _T(","));

	if (mask & MASK_TGAME_HIDE) 
		PutValue(os_ , _T(" Hide =  ") , g.Hide , _T(","));
	
	FixUpdate(os_) << _T(" WHERE GID = ") <<  gid << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteGame(const uint32& gid)
{	
	PutValue(os_ , _T(" DELETE * FROM tblGame WHERE GID = "), gid,_T("")); 
	os_ << std::endl;

	return this->ExecSql();
}

////
bool ZDBWriter::InsertPlugTool(const db::tPlugTool &g)
{
	os_	<< _T(" INSERT INTO tblPlugTool ( PID, CID, IdcVer, SvrVer, Priority, Name, Comment, "	)
		<< _T(" ToolSource, [Size], IdcClick, DownloadType, DownloadStatus, Status, SvrName, ")
		<< _T(" SvrExe, SvrParam, SvrPath, SvrRunType, CliName, CliExe, CliParam, CliPath, CliRunType, ")
		<< _T(" ConfigName, ConfigExe, ConfigParam, ConfigPath, ConfigRunType ) VALUES ( ");
		PutValue( os_, _T(""), g.PID			, _T(" , " ));
		PutValue( os_, _T(""), g.CID			, _T(" , " ));
		PutValue( os_, _T(""), g.IdcVer			, _T(" , " ));
		PutValue( os_, _T(""), g.SvrVer			, _T(" , " ));
		PutValue( os_, _T(""), g.Priority		, _T(" , " ));
		PutValue( os_, _T(""), g.Name			, _T(" , " ));
		PutValue( os_, _T(""), g.Comment		, _T(" , " ));	
		PutValue( os_, _T(""), g.ToolSource		, _T(" , " ));
		PutValue( os_, _T(""), g.Size			, _T(" , " ));
		PutValue( os_, _T(""), g.IdcClick		, _T(" , " ));
		PutValue( os_, _T(""), g.DownloadType	, _T(" , " ));
		PutValue( os_, _T(""), g.DownloadStatus	, _T(" , " ));
		PutValue( os_, _T(""), g.Status			, _T(" , " ));
		PutValue( os_, _T(""), g.SvrName		, _T(" , " ));
		PutValue( os_, _T(""), g.SvrExe			, _T(" , " ));
		PutValue( os_, _T(""), g.SvrParam		, _T(" , " ));
		PutValue( os_, _T(""), g.SvrPath		, _T(" , " ));
		PutValue( os_, _T(""), g.SvrRunType		, _T(" , " ));
		PutValue( os_, _T(""), g.CliName		, _T(" , " ));
		PutValue( os_, _T(""), g.CliExe			, _T(" , " ));
		PutValue( os_, _T(""), g.CliParam		, _T(" , " ));
		PutValue( os_, _T(""), g.CliPath		, _T(" , " ));
		PutValue( os_, _T(""), g.CliRunType		, _T(" , " ));
		PutValue( os_, _T(""), g.ConfigName		, _T(" , " ));
		PutValue( os_, _T(""), g.ConfigExe		, _T(" , " ));
		PutValue( os_, _T(""), g.ConfigParam	, _T(" , " ));
		PutValue( os_, _T(""), g.ConfigPath		, _T(" , " ));
		PutValue( os_, _T(""), g.ConfigRunType	, _T(" ) " ));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdatePlugTool(const uint32& pid, const db::tPlugTool &g, uint64 mask)
{
	assert(mask & ~MASK_TPLUGTOOL_PID);

	os_ << _T(" UPDATE tblPlugTool SET ");

	if (mask & MASK_TPLUGTOOL_CID) 
		PutValue(os_ , _T(" CID =  ") , g.CID , _T(","));

	if (mask & MASK_TPLUGTOOL_IDCVER) 
		PutValue(os_ , _T(" IdcVer =  ") , g.IdcVer , _T(","));

	if (mask & MASK_TPLUGTOOL_SVRVER) 
		PutValue(os_ , _T(" SvrVer =  ") , g.SvrVer , _T(","));

	if (mask & MASK_TPLUGTOOL_PRIORITY) 
		PutValue(os_ , _T(" Priority =  ") , g.Priority , _T(","));

	if (mask & MASK_TPLUGTOOL_NAME) 
		PutValue(os_ , _T(" Name =  ") , g.Name , _T(","));

	if (mask & MASK_TPLUGTOOL_COMMENT) 
		PutValue(os_ , _T(" Comment =  ") , g.Comment , _T(","));

	if (mask & MASK_TPLUGTOOL_TOOLSOURCE) 
		PutValue(os_ , _T(" ToolSource =  ") , g.ToolSource , _T(","));

	if (mask & MASK_TPLUGTOOL_SIZE) 
		PutValue(os_ , _T(" [Size] =  ") , g.Size , _T(","));

	if (mask & MASK_TPLUGTOOL_IDCCLICK) 
		PutValue(os_ , _T(" IdcClick =  ") , g.IdcClick , _T(","));

	if (mask & MASK_TPLUGTOOL_DOWNLOADTYPE) 
		PutValue(os_ , _T(" DownloadType =  ") , g.DownloadType , _T(","));

	if (mask & MASK_TPLUGTOOL_DOWNLOADSTATUS) 
		PutValue(os_ , _T(" DownloadStatus =  ") , g.DownloadStatus , _T(","));

	if (mask & MASK_TPLUGTOOL_STATUS) 
		PutValue(os_ , _T(" Status =  ") , g.Status , _T(","));

	if (mask & MASK_TPLUGTOOL_SVRNAME) 
		PutValue(os_ , _T(" SvrName =  ") , g.SvrName , _T(","));

	if (mask & MASK_TPLUGTOOL_SVREXE) 
		PutValue(os_ , _T(" SvrExe =  ") , g.SvrExe , _T(","));

	if (mask & MASK_TPLUGTOOL_SVRPARAM) 
		PutValue(os_ , _T(" SvrParam =  ") , g.SvrParam , _T(","));

	if (mask & MASK_TPLUGTOOL_SVRPATH) 
		PutValue(os_ , _T(" SvrPath =  ") , g.SvrPath , _T(","));

	if (mask & MASK_TPLUGTOOL_SVRRUNTYPE) 
		PutValue(os_ , _T(" SvrRunType =  ") , g.SvrRunType , _T(","));

	if (mask & MASK_TPLUGTOOL_CLINAME) 
		PutValue(os_ , _T(" CliName =  ") , g.CliName , _T(","));

	if (mask & MASK_TPLUGTOOL_CLIEXE) 
		PutValue(os_ , _T(" CliExe =  ") , g.CliExe , _T(","));

	if (mask & MASK_TPLUGTOOL_CLIPARAM) 
		PutValue(os_ , _T(" CliParam =  ") , g.CliParam , _T(","));

	if (mask & MASK_TPLUGTOOL_CLIPATH) 
		PutValue(os_ , _T(" CliPath =  ") , g.CliPath , _T(","));

	if (mask & MASK_TPLUGTOOL_CLIRUNTYPE) 
		PutValue(os_ , _T(" CliRunType =  ") , g.CliRunType , _T(","));


	if (mask & MASK_TPLUGTOOL_CONFIGNAME) 
		PutValue(os_ , _T(" ConfigName =  ") , g.ConfigName , _T(","));

	if (mask & MASK_TPLUGTOOL_CONFIGEXE) 
		PutValue(os_ , _T(" ConfigExe =  ") , g.ConfigExe , _T(","));

	if (mask & MASK_TPLUGTOOL_CONFIGPARAM) 
		PutValue(os_ , _T(" ConfigParam =  ") , g.ConfigParam , _T(","));

	if (mask & MASK_TPLUGTOOL_CONFIGPATH) 
		PutValue(os_ , _T(" ConfigPath =  ") , g.ConfigPath , _T(","));

	if (mask & MASK_TPLUGTOOL_CONFIGRUNTYPE) 
		PutValue(os_ , _T(" ConfigRunType =  ") , g.ConfigRunType , _T(","));

	FixUpdate(os_) << _T(" WHERE PID =  ") << pid << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeletePlugTool(const uint32& pid)
{	
	PutValue(os_ ,_T(" DELETE * FROM tblPlugTool WHERE PID = "), pid,_T("")); 
	os_ << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::InsertArea(const db::tArea &d)
{
	os_ << _T(" INSERT INTO tblArea ( AID, Name, SvrID ) VALUES ( ");
	    PutValue(os_ , _T(""), d.AID		, _T(" , "));//AID, 
	    PutValue(os_ , _T(""), d.Name		, _T(" , ")); //Name, 
	    PutValue(os_ , _T(""), d.SvrID		, _T(" ) "));//SvrID, 
		os_ << std::endl;
	return this->ExecSql();
}
//////////////////////////////////////////////////////////////////////////

bool ZDBWriter::UpdateArea(const stdex::tString& AID, const db::tArea &d, uint64 mask)
{
	assert(mask & ~MASK_TAREA_AID);

	os_ << _T(" UPDATE tblArea SET ");

	if (mask & MASK_TAREA_NAME) 
		PutValue(os_ , _T(" Name =  ") , d.Name , _T(","));

	if (mask & MASK_TAREA_SVRID) 
		PutValue(os_ , _T(" SvrID =  ") , d.SvrID , _T(",")) ;
	
	FixUpdate(os_) << _T(" WHERE AID =  ") << '\'' << AID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteArea(const stdex::tString& AID)
{
	os_ << _T(" DELETE * FROM tblArea WHERE AID =  ")<< '\''<< AID<< '\''<< std::endl;
	return this->ExecSql();
}


bool ZDBWriter::InsertRunType(const db::tRunType &d)
{
	os_ << _T(" INSERT INTO tblRunType (	AID, GID, [Type], VID ) VALUES ( ");
		PutValue(os_ , _T(""), d.AID	, _T(","));
		PutValue(os_ , _T(""), d.GID	, _T(","));
		PutValue(os_ , _T(""), d.Type	, _T(","));
		PutValue(os_ , _T(""), d.VID	, _T(" )"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateRunType(const uint32& gid, 
							   const stdex::tString& AID, 
							   const db::tRunType &d,
							   uint64 mask)
{
	assert(mask & ~MASK_TRUNTYPE_GID & ~MASK_TRUNTYPE_AID);
	
	os_ << _T(" UPDATE tblRunType SET ");

	if (mask & MASK_TRUNTYPE_TYPE) 
		PutValue(os_ , _T(" [Type] =  ") , d.Type , _T(","));
	
	if (mask & MASK_TRUNTYPE_VID) 
		PutValue(os_ , _T(" VID =  ") , d.VID , _T(","));
	
	PutValue(FixUpdate(os_) , _T(" WHERE GID = ") , gid,_T(""));
	os_ << _T(" AND AID =  ") << '\''<< AID<< '\'' << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteRunType(const uint32& gid, const stdex::tString& AID)
{
	PutValue(os_ , _T(" DELETE * FROM tblRunType WHERE GID = "), gid,_T("")); 
	os_ << _T(" AND AID =  ") << '\'' << AID << '\''<< std::endl;

	return this->ExecSql();
}

void ZDBWriter::SetBase64(stdex::tString &text)
{
	if(text.empty())
		return;

	std::string src = CT2A(text.c_str());
	int len = ATL::Base64EncodeGetRequiredLength(src.length());
	std::vector<char> vec;
	vec.resize(len + 1);

	ATL::Base64Encode((const byte *)src.c_str(), src.length(), &vec[0], &len);

	text = CA2T(&vec[0]);

}


bool ZDBWriter::InsertClient(const db::tClient &d)
{
	os_  << _T(" INSERT INTO tblClient ( Name, AID, IP, MAC, Mark, Gate, DNS, DNS2, ProtInstall, Partition, ")
	    << _T(" ProtVer, VDiskVer, MenuVer, CliSvrVer, System, CPU, MainBoard, Memory, Disk, ")
	    << _T(" Video, Audio, Network, Camera ) VALUES ( ");


	    PutValue(os_ , _T(""), d.Name			, _T(","));  
	    PutValue(os_ , _T(""), d.AID			, _T(","));  
		PutValue(os_ , _T(""), (int)d.IP		, _T(","));  
		PutValue(os_ , _T(""), d.MAC			, _T(","));  
		PutValue(os_ , _T(""), (int)d.Mark		, _T(","));  
		PutValue(os_ , _T(""), (int)d.Gate		, _T(",")); 
		PutValue(os_ , _T(""), (int)d.DNS		, _T(","));  
		PutValue(os_ , _T(""), (int)d.DNS2		, _T(","));  
		PutValue(os_ , _T(""), d.ProtInstall	, _T(","));  
		PutValue(os_ , _T(""), d.Partition		, _T(",")); 
		PutValue(os_ , _T(""), d.ProtVer		, _T(","));  
		PutValue(os_ , _T(""), d.VDiskVer		, _T(","));  
		PutValue(os_ , _T(""), d.MenuVer		, _T(","));  
		PutValue(os_ , _T(""), d.CliSvrVer		, _T(","));

		stdex::tString system =  d.System;
		SetBase64(system);
		TCHAR System[MAX_REMARK_LEN] = {0};
		utility::Strcpy(System, system.c_str());
		PutValue(os_ , _T(""), System			, _T(","));

		stdex::tString cpu =  d.CPU;
		SetBase64(cpu);
		TCHAR CPU[MAX_REMARK_LEN] = {0};
		utility::Strcpy(CPU, cpu.c_str());
		PutValue(os_ , _T(""), CPU				, _T(","));

		stdex::tString mainboard =  d.Mainboard;
		SetBase64(mainboard);
		TCHAR Mainboard[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Mainboard, mainboard.c_str());
		PutValue(os_ , _T(""), Mainboard		, _T(","));

		stdex::tString memory =  d.Memory;
		SetBase64(memory);
		TCHAR Memory[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Memory, memory.c_str());
		PutValue(os_ , _T(""), Memory		, _T(","));

		stdex::tString disk =  d.Disk;
		SetBase64(disk);
		TCHAR Disk[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Disk, disk.c_str());
		PutValue(os_ , _T(""), Disk		, _T(","));

		stdex::tString video =  d.Video;
		SetBase64(video);
		TCHAR Video[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Video, video.c_str());
		PutValue(os_ , _T(""), Video		, _T(","));

		stdex::tString audio =  d.Audio;
		SetBase64(audio);
		TCHAR Audio[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Audio, audio.c_str());
		PutValue(os_ , _T(""), Audio		, _T(","));

		stdex::tString network =  d.Network;
		SetBase64(network);
		TCHAR Network[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Network, network.c_str());
		PutValue(os_ , _T(""), Network		, _T(","));

		stdex::tString camera =  d.Camera;
		SetBase64(camera);
		TCHAR Camera[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Camera, camera.c_str());
		PutValue(os_ , _T(""), Camera		, _T(")"));

		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateClient(const stdex::tString& Name, const db::tClient &d, uint64 mask)
{
	// Online 和 SOCK 字段不入库
	//if( (mask &=  MASK_TCLIENT_ONLINE) == MASK_TCLIENT_ONLINE ||
	//	(mask &=  MASK_TCLIENT_SOCKET) == MASK_TCLIENT_SOCKET )
	//	return true;
	os_ << _T(" UPDATE tblClient SET ");

	if (mask & MASK_TCLIENT_IP) 
		PutValue(os_ , _T(" IP =  ") , (int)d.IP , _T(","));
	
	if (mask & MASK_TCLIENT_MARK) 
		PutValue(os_ , _T(" Mark =  ") , (int)d.Mark , _T(","));
	
	if (mask & MASK_TCLIENT_GATE) 
		PutValue(os_ , _T(" Gate =  ") , (int)d.Gate , _T(","));
	
	if (mask & MASK_TCLIENT_DNS) 
		PutValue(os_ , _T(" DNS =  ") , (int)d.DNS , _T(","));
	
	if (mask & MASK_TCLIENT_DNS2) 
		PutValue(os_ , _T(" DNS2 =  ") , (int)d.DNS2 , _T(","));
	
	if (mask & MASK_TCLIENT_MAC)
		PutValue(os_ , _T(" MAC =  ") , d.MAC , _T(","));
	
	if (mask & MASK_TCLIENT_AID) 
		PutValue(os_ , _T(" AID =  ") , d.AID , _T(","));
	
	if (mask & MASK_TCLIENT_PROTINSTALL) 
		PutValue(os_ , _T(" ProtInstall =  ") , d.ProtInstall , _T(","));
	
	if (mask & MASK_TCLIENT_PARTITION) 
		PutValue(os_ , _T(" Partition =  ") , d.Partition , _T(","));
	
	if (mask & MASK_TCLIENT_PROTVER) 
		PutValue(os_ , _T(" ProtVer =  ") , d.ProtVer , _T(","));
	
	if (mask & MASK_TCLIENT_MENUVER) 
		PutValue(os_ , _T(" MenuVer =  ") , d.MenuVer , _T(","));
	
	if (mask & MASK_TCLIENT_CLISVRVER) 
		PutValue(os_ , _T(" CliSvrVer =  ") , d.CliSvrVer , _T(","));



	if (mask & MASK_TCLIENT_CPU)
	{
		stdex::tString cpu =  d.CPU;
		SetBase64(cpu);
		TCHAR CPU[MAX_REMARK_LEN] = {0};
		utility::Strcpy(CPU, cpu.c_str());
		PutValue(os_ , _T(" CPU =  "), CPU				, _T(","));
	}

	if (mask & MASK_TCLIENT_MEMORY) 
	{
		stdex::tString memory =  d.Memory;
		SetBase64(memory);
		TCHAR Memory[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Memory, memory.c_str());
		PutValue(os_ , _T(" Memory =  "), Memory		, _T(","));
	}

	if (mask & MASK_TCLIENT_DISK) 
	{
		stdex::tString disk =  d.Disk;
		SetBase64(disk);
		TCHAR Disk[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Disk, disk.c_str());
		PutValue(os_ , _T(" Disk =  "), Disk		, _T(","));
	}

	if (mask & MASK_TCLIENT_VIDEO) 
	{
		stdex::tString video =  d.Video;
		SetBase64(video);
		TCHAR Video[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Video, video.c_str());
		PutValue(os_ , _T(" Video =  "), Video		, _T(","));

	}

	if (mask & MASK_TCLIENT_AUDIO) 
	{
		stdex::tString audio =  d.Audio;
		SetBase64(audio);
		TCHAR Audio[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Audio, audio.c_str());
		PutValue(os_ , _T(" Audio =  "), Audio		, _T(","));
	}

	if (mask & MASK_TCLIENT_MAINBOARD) 
	{
		stdex::tString mainboard =  d.Mainboard;
		SetBase64(mainboard);
		TCHAR Mainboard[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Mainboard, mainboard.c_str());
		PutValue(os_ , _T(" MainBoard =  "), Mainboard		, _T(","));
	}

	if (mask & MASK_TCLIENT_NETWORK) 
	{
		stdex::tString network =  d.Network;
		SetBase64(network);
		TCHAR Network[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Network, network.c_str());
		PutValue(os_ , _T(" Network =  "), Network		, _T(","));
	}

	if (mask & MASK_TCLIENT_CAMERA)
	{
		stdex::tString camera =  d.Camera;
		SetBase64(camera);
		TCHAR Camera[MAX_REMARK_LEN] = {0};
		utility::Strcpy(Camera, camera.c_str());
		PutValue(os_ , _T(" Camera =  "), Camera		, _T(","));
	}

	if (mask & MASK_TCLIENT_SYSTEM) 
	{		
		stdex::tString system =  d.System;
		SetBase64(system);
		TCHAR System[MAX_REMARK_LEN] = {0};
		utility::Strcpy(System, system.c_str());
		PutValue(os_ , _T(" System =  "), System			, _T(","));
	}

	if (mask & MASK_TCLIENT_ONLINE) 
		if(utility::Strcmp(os_.str().c_str(),_T(" UPDATE tblClient SET ")) == 0)
		{
			os_.str(_T(""));
			return true;
		}

	if (mask & MASK_TCLIENT_TEMPERATURE) 
		if(utility::Strcmp(os_.str().c_str(),_T(" UPDATE tblClient SET ")) == 0)
		{
			os_.str(_T(""));
			return true;
		}
			


	//注意消除最后一个逗号
	FixUpdate(os_) << _T(" WHERE Name =  ") << '\''<< Name << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteClient(const stdex::tString& Name)
{
	os_ <<_T(" DELETE * FROM tblClient WHERE Name =  ")<< '\''<< Name<< '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::InsertFavorite(const db::tFavorite &d)
{
	os_  << _T(" INSERT INTO tblFavorite ( FID, [Type], Name, Url, Serial ) VALUES ( ");
		PutValue(os_ , _T(""), d.FID	, _T(","));
		PutValue(os_ , _T(""), d.Name	, _T(","));
		PutValue(os_ , _T(""), d.Type	, _T(","));
		PutValue(os_ , _T(""), d.URL	, _T(","));
		PutValue(os_ , _T(""), d.Serial , _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateFavorite(const stdex::tString& FID,
							   const db::tFavorite &d, 
							   uint64 mask)
{
	assert(mask & ~MASK_TFAVORITE_FID);

	os_ << _T(" UPDATE tblFavorite SET ");

	if (mask & MASK_TFAVORITE_TYPE) 
		PutValue(os_ , _T(" [Type] =  ") , d.Type , _T(","));
	
	if (mask & MASK_TFAVORITE_NAME) 
		PutValue(os_ , _T(" Name =  ") , d.Name , _T(","));

	if (mask & MASK_TFAVORITE_URL) 
		PutValue(os_ , _T(" Url =  ") , d.URL , _T(","));

	if (mask & MASK_TFAVORITE_SERIAL) 
		PutValue(os_ , _T(" Serial =  ") , d.Serial , _T(","));

	//注意消除最后一个逗号
	FixUpdate(os_) << _T(" WHERE FID =  ") << '\''<< FID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteFavorite(const stdex::tString& FID)
{
	os_ << _T(" DELETE * FROM tblFavorite WHERE FID =  ")<< '\''<< FID<< '\''<< std::endl;

	return this->ExecSql();
}


bool ZDBWriter::InsertCmpBootTask(const db::tCmpBootTask &d)
{
	os_  << _T(" INSERT INTO tblCmpBootTask ( TID, Name, [Type], Flag, RunDate, StartTime, DelayTime, EndTime, EndDate,RunType,Status, Parameter ) VALUES ( ");
		PutValue(os_ , _T(""), d.TID , _T(","));
		PutValue(os_ , _T(""), d.Name , _T(","));
		PutValue(os_ , _T(""), d.Type , _T(","));
		PutValue(os_ , _T(""), d.Flag , _T(","));
		PutValue(os_ , _T(""), d.RunDate , _T(","));
		PutValue(os_ , _T(""), d.StartTime , _T(","));
		PutValue(os_ , _T(""), d.DelayTime , _T(","));
		PutValue(os_ , _T(""), d.EndTime , _T(","));
		PutValue(os_ , _T(""), d.EndDate , _T(","));
		PutValue(os_ , _T(""), d.RunType , _T(","));
		PutValue(os_ , _T(""), d.Status , _T(","));
		PutValue(os_ , _T(""), d.Parameter , _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateCmpBootTask(const stdex::tString& TID,
								   const db::tCmpBootTask &d,
								   uint64 mask)
{
	assert(mask & ~MASK_TCMPBOOTTASK_TID);

	os_ << _T(" UPDATE tblCmpBootTask SET ");

	if (mask & MASK_TCMPBOOTTASK_NAME) 
		PutValue(os_ , _T(" Name =  ") , d.Name , _T(",")) ;

	if (mask & MASK_TCMPBOOTTASK_TYPE) 
		PutValue(os_ , _T(" [Type] =  ") , d.Type , _T(","));
	
	if (mask & MASK_TCMPBOOTTASK_FLAG) 
		PutValue(os_ , _T(" Flag =  ") , d.Flag , _T(","));

	if (mask & MASK_TCMPBOOTTASK_RUNDATE) 
		PutValue(os_ , _T(" RunDate =  ") , d.RunDate , _T(","));

	if (mask & MASK_TCMPBOOTTASK_STARTTIME) 
		PutValue(os_ , _T(" StartTime =  ") , d.StartTime , _T(","));

	if (mask & MASK_TCMPBOOTTASK_DELAYTIME) 
		PutValue(os_ , _T(" DelayTime =  ") , d.DelayTime , _T(","));

	if (mask & MASK_TCMPBOOTTASK_ENDTIME) 
		PutValue(os_ , _T(" EndTime =  ") , d.EndTime , _T(","));

	if (mask & MASK_TCMPBOOTTASK_ENDDATE) 
		PutValue(os_ , _T(" EndDate =  ") , d.EndDate , _T(","));

	if (mask & MASK_TCMPBOOTTASK_RUNTYPE) 
		PutValue(os_ , _T(" RunType =  ") , d.RunType , _T(","));

	if (mask & MASK_TCMPBOOTTASK_STATUS) 
		PutValue(os_ , _T(" Status =  ") , d.Status , _T(","));

	if (mask & MASK_TCMPBOOTTASK_PARAMETER) 
		PutValue(os_ , _T(" Parameter =  ") , d.Parameter , _T(","));
	
	FixUpdate(os_)<< _T(" WHERE TID =  ") << '\''<< TID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteCmpBootTask(const stdex::tString& TID)
{
	os_ << _T(" DELETE * FROM tblCmpBootTask WHERE TID =  ")<< '\''<< TID<< '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::InsertVDisk(const db::tVDisk &d)
{
	os_  << _T(" INSERT INTO tblVDisk ( VID, SvrID, SoucIP,  Port, SvrDrv, CliDrv, [Size], SsdDrv, [Type], LoadType, SvrMode ) VALUES ( ");
		PutValue(os_ , _T(""), d.VID		, _T(","));
		PutValue(os_ , _T(""), d.SvrID		, _T(","));
		PutValue(os_ , _T(""), (int)d.SoucIP, _T(","));
		PutValue(os_ , _T(""), d.Port		, _T(","));
		PutValue(os_ , _T(""), d.SvrDrv		, _T(","));
		PutValue(os_ , _T(""), d.CliDrv		, _T(","));
		PutValue(os_ , _T(""), d.Size		, _T(","));
		PutValue(os_ , _T(""), d.SsdDrv		, _T(","));
		PutValue(os_ , _T(""), d.Type		, _T(","));
		PutValue(os_ , _T(""), d.LoadType	, _T(","));
		PutValue(os_ , _T(""), d.SvrMode	, _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateVDisk(const stdex::tString& VID, const db::tVDisk &d, uint64 mask)
{
	assert(mask & ~MASK_TVDISK_VID);

	os_ << _T(" UPDATE tblVDisk SET ");

	if (mask & MASK_TVDISK_SVRID) 
		PutValue(os_ , _T(" SvrID =  ") , (int)d.SvrID , _T(","));

	if (mask & MASK_TVDISK_SOUCIP) 
		PutValue(os_ , _T(" SoucIP =  ") , (int)d.SoucIP , _T(","));

	if (mask & MASK_TVDISK_PORT) 
		PutValue(os_ , _T(" Port =  ") , d.Port , _T(","));
	
	if (mask & MASK_TVDISK_SVRDRV) 
		PutValue(os_ , _T(" SvrDrv =  ") , d.SvrDrv , _T(","));
	
	if (mask & MASK_TVDISK_CLIDRV) 
		PutValue(os_ , _T(" CliDrv =  ") , d.CliDrv , _T(","));

	if (mask & MASK_TVDISK_SIZE) 
		PutValue(os_ , _T(" [Size] =  ") , d.Size , _T(","));

	if (mask & MASK_TVDISK_SSDDRV) 
		PutValue(os_ , _T(" SsdDrv =  ") , d.SsdDrv , _T(","));
	
	if (mask & MASK_TVDISK_TYPE) 
		PutValue(os_ , _T(" [Type] =  ") , d.Type , _T(","));
	
	if (mask & MASK_TVDISK_LOADTYPE) 
		PutValue(os_ , _T(" LoadType =  ") , d.LoadType , _T(","));

	if (mask & MASK_TVDISK_SVRMODE) 
		PutValue(os_ , _T(" SvrMode =  ") , d.SvrMode , _T(","));
	
	FixUpdate(os_) << _T(" WHERE VID =  ") << '\''<< VID<< '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteVDisk(const stdex::tString& VID)
{
	os_ << _T(" DELETE * FROM tblVDisk WHERE VID =  ")<< '\''<< VID<< '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::InsertSysOpt(const db::tSysOpt &d)
{
	os_  << _T(" INSERT INTO tblOption ( OptName, OptValue ) VALUES ( ");
		PutValue(os_ , _T(""), d.key	, _T(","));
		PutValue(os_ , _T(""), d.value	, _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateSysOpt(const stdex::tString& OptName,
							 const db::tSysOpt &d, 
							 uint64 mask)
{
	assert(mask & ~MASK_TSYSOPT_KEY);

	 os_ << _T(" UPDATE tblOption SET ");

	if (mask & MASK_TSYSOPT_VALUE) 
		PutValue(os_ , _T(" OptValue =  ") , d.value,' ' );
	
	FixUpdate(os_) << _T(" WHERE OptName =  ") << '\''<< OptName<< '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteSysOpt(const stdex::tString& OptName)
{
	os_ << _T(" DELETE * FROM tblOption WHERE OptName =  ")<< '\''<< OptName<< '\''<< std::endl;

	return this->ExecSql();
}


bool ZDBWriter::InsertSyncTask(const db::tSyncTask &d)
{
	os_  << _T(" INSERT INTO tblSyncTask ( SID, SvrID, Name, SvrDir, NodeDir, SoucIP, DestIP, Speed, SyncType, Status, ")	
		<< _T(" NameID ) VALUES ( ");
		PutValue(os_ , _T(""), d.SID		, _T(","));
		PutValue(os_ , _T(""), d.SvrID		, _T(","));
		PutValue(os_ , _T(""), d.Name		, _T(","));
		PutValue(os_ , _T(""), d.SvrDir		, _T(","));
		PutValue(os_ , _T(""), d.NodeDir	, _T(","));
		PutValue(os_ , _T(""), (int)d.SoucIp, _T(","));
		PutValue(os_ , _T(""), (int)d.DestIp, _T(","));
		PutValue(os_ , _T(""), d.Speed		, _T(","));
		PutValue(os_ , _T(""), d.SyncType	, _T(","));
		PutValue(os_ , _T(""), d.Status		, _T(","));
		PutValue(os_ , _T(""), d.NameID		, _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateSyncTask(const stdex::tString& SID,
							   const db::tSyncTask &d, 
							   uint64 mask)
{
	assert(mask & ~MASK_TSYNCTASK_SID);
	
	os_ << _T(" UPDATE tblSyncTask SET ");

	if (mask & MASK_TSYNCTASK_SVRID) 
		PutValue(os_ , _T(" SvrID =  ") , d.SvrID , _T(","));

	if (mask & MASK_TSYNCTASK_NAME) 
		PutValue(os_ , _T(" Name =  ") , d.Name , _T(","));

	if (mask & MASK_TSYNCTASK_SVRDIR) 
		PutValue(os_ , _T(" SvrDir =  ") , d.SvrDir , _T(","));

	if (mask & MASK_TSYNCTASK_NODEDIR) 
		PutValue(os_ , _T(" NodeDir =  ") , d.NodeDir , _T(","));

	if (mask & MASK_TSYNCTASK_SOUCIP) 
		PutValue(os_ , _T(" SoucIP =  ") , (int)d.SoucIp , _T(","));

	if (mask & MASK_TSYNCTASK_DESTIP) 
		PutValue(os_ , _T(" DestIP =  ") , (int)d.DestIp , _T(","));
	
	if (mask & MASK_TSYNCTASK_SPEED) 
		PutValue(os_ , _T(" Speed =  ") , d.Speed , _T(","));

	if (mask & MASK_TSYNCTASK_SYNCTYPE) 
		PutValue(os_ , _T(" SyncType =  ") , d.SyncType , _T(","));

	if (mask & MASK_TSYNCTASK_STATUS) 
		PutValue(os_ , _T(" Status =  ") , d.Status , _T(","));

	if (mask & MASK_TSYNCTASK_NAMEID) 
		PutValue(os_ , _T(" NameID =  ") , d.NameID , _T(","));

	FixUpdate(os_) << _T(" WHERE SID =  ") << '\''<< SID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteSyncTask(const stdex::tString& SID)
{
	os_ << _T(" DELETE * FROM tblSyncTask WHERE SID =  ")<< '\''<< SID<< '\''<< std::endl;

	return this->ExecSql();
}
///////////////////////////////////////////////////////////////////////

bool ZDBWriter::InsertSyncGame(const db::tSyncGame &d)
{
	os_  << _T(" INSERT INTO tblSyncGame ( SID, GID ) VALUES ( ");
	PutValue(os_ , _T(""), d.SID , _T(","));
	PutValue(os_ , _T(""), d.GID , _T(")"));
	os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateSyncGame(const uint32& gid, 
							   const stdex::tString& SID, 
							   const db::tSyncGame &d,
							   uint64 mask)
{
	assert(0);
	return true;
}

bool ZDBWriter::DeleteSyncGame( const uint32& gid,const stdex::tString& SID)
{
	PutValue(os_ , _T(" DELETE * FROM tblSyncGame WHERE GID = "), gid, _T("")); 
	os_ << _T(" AND SID =  ") << '\'' << SID << '\''<< std::endl;

	return this->ExecSql();
}


/////////////////////////////////////////////////////////////////////////////
bool ZDBWriter::InsertServer(const db::tServer &d)
{
	os_  << _T(" INSERT INTO tblServer ( SvrID, SvrName, SvrType, SvrIP, IP1, IP2, IP3, IP4, Speed, BalanceType ) VALUES ( ");
		PutValue(os_ , _T(""), d.SvrID			, _T(","));
		PutValue(os_ , _T(""), d.SvrName		, _T(","));
		PutValue(os_ , _T(""), d.SvrType		, _T(","));
		PutValue(os_ , _T(""), (int)d.SvrIP		, _T(","));
		PutValue(os_ , _T(""), (int)d.Ip1		, _T(","));
		PutValue(os_ , _T(""), (int)d.Ip2		, _T(","));
		PutValue(os_ , _T(""), (int)d.Ip3		, _T(","));
		PutValue(os_ , _T(""), (int)d.Ip4		, _T(","));
		PutValue(os_ , _T(""), d.Speed			, _T(","));
		PutValue(os_ , _T(""), d.BalanceType	, _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateServer(const stdex::tString& SvrID,
							   const db::tServer &d, 
							   uint64 mask)
{
	assert(mask & ~MASK_TSERVER_SVRID);

	os_ << _T(" UPDATE tblServer SET ");

	if (mask & MASK_TSERVER_SVRNAME) 
		PutValue(os_ , _T(" SvrName =  ") , d.SvrName , _T(","));

	if (mask & MASK_TSERVER_SVRTYPE) 
		PutValue(os_ , _T(" SvrType =  ") , d.SvrType , _T(","));

	if (mask & MASK_TSERVER_SVRIP) 
		PutValue(os_ , _T(" SvrIP =  ") , (int)d.SvrIP , _T(","));

	if (mask & MASK_TSERVER_IP1) 
		PutValue(os_ , _T(" IP1 =  ") , (int)d.Ip1 , _T(","));

	if (mask & MASK_TSERVER_IP2) 
		PutValue(os_ , _T(" IP2 =  ") , (int)d.Ip2 , _T(","));

	if (mask & MASK_TSERVER_IP3) 
		PutValue(os_ , _T(" IP3 =  ") , (int)d.Ip3 , _T(","));

	if (mask & MASK_TSERVER_IP4) 
		PutValue(os_ , _T(" IP4 =  ") , (int)d.Ip4 , _T(","));

	if (mask & MASK_TSERVER_SPEED) 
		PutValue(os_ , _T(" Speed =  ") , d.Speed , _T(","));

	if (mask & MASK_TSERVER_BALANCETYPE) 
		PutValue(os_ , _T(" BalanceType =  ") , d.BalanceType , _T(","));

	//注意消除最后一个逗号
	FixUpdate(os_) << _T(" WHERE SvrID =  ") << '\''<<SvrID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeleteServer(const stdex::tString& SvrID)
{
	os_ <<_T(" DELETE * FROM tblServer WHERE SvrID =  ")<< '\''<< SvrID<< '\''<< std::endl;

	return this->ExecSql();
}

///////

bool ZDBWriter::InsertBootTaskArea(const db::tBootTaskArea &d)
{
	os_  << _T(" INSERT INTO tblBootTaskArea ( TID, AID ) VALUES ( ");
		PutValue(os_ , _T(""), d.TID , _T(","));
		PutValue(os_ , _T(""), d.AID , _T(")"));
		os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdateBootTaskArea(const stdex::tString& TID, 
							   const stdex::tString& AID,
							   const db::tBootTaskArea &d, 
							   uint64 mask)
{
	assert(0);
	return true;
}

bool ZDBWriter::DeleteBootTaskArea(const stdex::tString& TID, const stdex::tString& AID)
{
	os_ << _T(" DELETE * FROM tblBootTaskArea WHERE TID = ") << '\''<< TID<< '\''; 
	os_ << _T(" AND AID = ") << '\''<< AID << '\''<< std::endl;

	return this->ExecSql();
}

bool ZDBWriter::InsertPushGameStatic(const db::tPushGameStatic &g)
{
	os_	<< _T(" INSERT INTO tblPushGameStatic ( GID, [Size], BeginDate, EndDate) VALUES ( ");
	PutValue( os_, _T(""), g.GID		, _T(" , " ));//GID, 
	PutValue( os_, _T(""), g.Size		, _T(" , " ));//PID, 
	PutValue( os_, _T(""), g.BeginDate	, _T(" , " ));//BeginDate, 
	PutValue( os_, _T(""), g.EndDate	, _T(" ) " )); //EndDate, 
	
	os_ << std::endl;
	return this->ExecSql();
}

bool ZDBWriter::UpdatePushGameStatic(const uint32& gid, const db::tPushGameStatic &g, uint64 mask)
{
	assert(mask & ~MASK_TPUSHGAMESTATIC_GID);

	os_ << _T(" UPDATE tblPushGameStatic SET ");

	if (mask & MASK_TPUSHGAMESTATIC_SIZE)
		PutValue(os_, _T(" [Size] =  "), g.Size, ',');

	if (mask & MASK_TPUSHGAMESTATIC_BEGINDATE) 
		PutValue(os_ , _T(" BeginDate =  ") , g.BeginDate , _T(","));

	if (mask & MASK_TPUSHGAMESTATIC_ENDDATE) 
		PutValue(os_ , _T(" EndDate =  ") , g.EndDate , _T(","));

	FixUpdate(os_) << _T(" WHERE GID = ") <<  gid << std::endl;

	return this->ExecSql();
}

bool ZDBWriter::DeletePushGameStatic(const uint32& gid)
{	
	PutValue(os_ , _T(" DELETE * FROM tblPushGameStatic WHERE GID = "), gid,_T("")); 
	os_ << std::endl;

	return this->ExecSql();
}

//thread func
uint32 ZDBWriter::Exec(void)
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
			ProcessRunTypeEvents();
			ProcessClientEvents();
			ProcessFavoriteEvents();
			ProcessCmpBootTaskEvents();
			ProcessSysOptEvents();
			ProcessVDiskEvents();
			ProcessSyncTaskEvents();
			ProcessServerEvents();
			ProcessBootTaskAreaEvents();
			ProcessPlugToolEvents();
			ProcessSyncGameEvents();
			ProcessPushGameStaticEvents( );

			Log(LM_DEBUG, _T("保存完所有数据到数据库!\n"));
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
			ProcessRunTypeEvents( fdo );
			ProcessClientEvents( fdo );
			ProcessFavoriteEvents( fdo );
			ProcessCmpBootTaskEvents( fdo );
			ProcessSysOptEvents( fdo );
			ProcessVDiskEvents( fdo );
			ProcessSyncTaskEvents( fdo );
			ProcessServerEvents( fdo );
			ProcessBootTaskAreaEvents( fdo );
			ProcessPlugToolEvents( fdo );
			ProcessSyncGameEvents( fdo );
			ProcessPushGameStaticEvents( fdo );
			//Log(LM_DEBUG, _T("保存完所有数据到数据库!\n"));

		}

		if (IsDone()) {
			ProcessGameEvents();
			ProcessClassEvents();
			ProcessAreaEvents();
			ProcessRunTypeEvents();
			ProcessClientEvents();
			ProcessFavoriteEvents();
			ProcessCmpBootTaskEvents();
			ProcessSysOptEvents();
			ProcessVDiskEvents();
			ProcessSyncTaskEvents();
			ProcessServerEvents();
			ProcessBootTaskAreaEvents();
			ProcessPlugToolEvents();
			ProcessSyncGameEvents( );
			ProcessPushGameStaticEvents();
			Log(LM_DEBUG, _T("保存完所有数据到数据库!\n"));

			break;
		}
	}
	
	::CoUninitialize();

	return 0;
}


} //namespace i8desk
