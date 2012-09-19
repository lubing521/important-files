#include "StdAfx.h"

#include "tablemgr.h"
#include "dbtables.h"
#include "tptables.h"

namespace i8desk {

//////////////////////////////////////////////////////
//ZTableMgr


ZTableMgr::ZTableMgr(void)
{
	m_path = utility::GetAppPath() + _T("data\\db\\");

	m_pGameTable.reset(new ZGameTable);
	m_pAreaTable.reset(new ZAreaTable);
	m_pRunTypeTable.reset(new ZRunTypeTable);
	m_pClassTable.reset(new ZClassTable);
	m_pSysOptTable.reset(new ZSysOptTable);
	m_pVDiskTable.reset(new ZVDiskTable);
	m_pFavoriteTable.reset(new ZFavoriteTable);
	m_pCmpBootTaskTable.reset(new ZCmpBootTaskTable);
	m_pClientTable.reset(new ZClientTable);
	m_pVDiskStatusTable.reset(new ZVDiskStatusTable);
	m_pSyncTaskTable.reset(new ZSyncTaskTable);
	m_pServerTable.reset(new ZServerTable);
	m_pBootTaskAreaTable.reset(new ZBootTaskAreaTable);
	m_pModuleUsageTable.reset(new ZModuleUsageTable);
	m_pModule2UsageTable.reset(new ZModule2UsageTable);
	m_pServerStatusTable.reset(new ZServerStatusTable);
	m_pDiskStatusTable.reset(new ZDiskStatusTable);
	m_pVDiskClientTable.reset(new ZVDiskClientTable);
	m_pTaskStatusTable.reset(new ZTaskStatusTable);
	m_pUpdateGameStatusTable.reset(new ZUpdateGameStatusTable);
	m_pPlugToolTable.reset(new ZPlugToolTable);
	m_pPlugToolStatusTable.reset(new ZPlugToolStatusTable);
	m_pGameExVersionTable.reset(new ZGameExVersionTable);
	m_pSyncGameTable.reset(new ZSyncGameTable);
	m_pSyncTaskStatusTable.reset(new ZSyncTaskStatusTable);
	m_pPushGameStaticTable.reset(new ZPushGameStaticTable);

}


ZTableMgr::~ZTableMgr(void)
{
	//m_pGameTable.release();
	//m_pAreaTable.release();
	//m_pRunTypeTable.release();
	//m_pClassTable.release();
	//m_pSysOptTable.release();
	//m_pVDiskTable.release();
	//m_pFavoriteTable.release();
	//m_pCmpBootTaskTable.release();
	//m_pClientTable.release();
	//m_pVDiskStatusTable.release();
	//m_pSyncTaskTable.release();
	//m_pServerTable.release();
	//m_pBootTaskAreaTable.release();
	//m_pModuleUsageTable.release();
	//m_pModule2UsageTable.release();
	//m_pServerStatusTable.release();
	//m_pDiskStatusTable.release();
	//m_pVDiskClientTable.release();
	//m_pTaskStatusTable.release();
	//m_pUpdateGameStatusTable.release();
	//m_pPlugToolTable.release();
	//m_pPlugToolStatusTable.release();
	//m_pGameExVersionTable.release();
	//m_pSyncGameTable.release();
	//m_pSyncTaskStatusTable.release();
	//m_pPushGameStaticTable.release();

	m_pGameTable.reset(0);
	m_pAreaTable.reset(0);
	m_pRunTypeTable.reset(0);
	m_pClassTable.reset(0);
	m_pSysOptTable.reset(0);
	m_pVDiskTable.reset(0);
	m_pFavoriteTable.reset(0);
	m_pCmpBootTaskTable.reset(0);
	m_pClientTable.reset(0);
	m_pVDiskStatusTable.reset(0);
	m_pSyncTaskTable.reset(00);
	m_pServerTable.reset(0);
	m_pBootTaskAreaTable.reset(0);
	m_pModuleUsageTable.reset(0);
	m_pModule2UsageTable.reset(0);
	m_pServerStatusTable.reset(0);
	m_pDiskStatusTable.reset(0);
	m_pVDiskClientTable.reset(0);
	m_pTaskStatusTable.reset(0);
	m_pUpdateGameStatusTable.reset(0);
	m_pPlugToolTable.reset(0);
	m_pPlugToolStatusTable.reset(0);
	m_pGameExVersionTable.reset(0);
	m_pSyncGameTable.reset(0);
	m_pSyncTaskStatusTable.reset(0);
	m_pPushGameStaticTable.reset(0);


	//m_pGameTable = 0;
	//m_pAreaTable = 0;
	//m_pRunTypeTable = 0;
	//m_pClassTable = 0;
	//m_pSysOptTable = 0;
	//m_pVDiskTable = 0;
	//m_pFavoriteTable = 0;
	//m_pCmpBootTaskTable = 0;
	//m_pClientTable = 0;
	//m_pVDiskStatusTable = 0;
	//m_pSyncTaskTable = 0;
	//m_pServerTable = 0;
	//m_pBootTaskAreaTable = 0;
	//m_pModuleUsageTable = 0;
	//m_pModule2UsageTable = 0;
	//m_pServerStatusTable = 0;
	//m_pDiskStatusTable = 0;
	//m_pVDiskClientTable = 0;
	//m_pTaskStatusTable = 0;
	//m_pUpdateGameStatusTable = 0;
	//m_pPlugToolTable = 0;
	//m_pPlugToolStatusTable = 0;
	//m_pGameExVersionTable = 0;
	//m_pSyncGameTable = 0;
	//m_pSyncTaskStatusTable = 0;
	//m_pPushGameStaticTable = 0;

}

IMPLMENT_TABLE_OBJECT(Game)
IMPLMENT_TABLE_OBJECT(Area)
IMPLMENT_TABLE_OBJECT(RunType)
IMPLMENT_TABLE_OBJECT(Class)
IMPLMENT_TABLE_OBJECT(SysOpt)
IMPLMENT_TABLE_OBJECT(VDisk)
IMPLMENT_TABLE_OBJECT(Favorite)
IMPLMENT_TABLE_OBJECT(CmpBootTask)
IMPLMENT_TABLE_OBJECT(Client)
IMPLMENT_TABLE_OBJECT(VDiskStatus)
IMPLMENT_TABLE_OBJECT(SyncTask)
IMPLMENT_TABLE_OBJECT(Server)
IMPLMENT_TABLE_OBJECT(BootTaskArea)
IMPLMENT_TABLE_OBJECT(ModuleUsage)
IMPLMENT_TABLE_OBJECT(Module2Usage)
IMPLMENT_TABLE_OBJECT(ServerStatus)
IMPLMENT_TABLE_OBJECT(DiskStatus)
IMPLMENT_TABLE_OBJECT(VDiskClient)
IMPLMENT_TABLE_OBJECT(TaskStatus)
IMPLMENT_TABLE_OBJECT(UpdateGameStatus)
IMPLMENT_TABLE_OBJECT(PlugTool)
IMPLMENT_TABLE_OBJECT(PlugToolStatus)
IMPLMENT_TABLE_OBJECT(GameExVersion)
IMPLMENT_TABLE_OBJECT(SyncGame)
IMPLMENT_TABLE_OBJECT(SyncTaskStatus)
IMPLMENT_TABLE_OBJECT(PushGameStatic)



//initialize and uninitialize
	// For database
int ZTableMgr::Load(IDatabase *pDatabase)
{
	LOAD_TABLE_OBJECT(Game,			pDatabase)
	LOAD_TABLE_OBJECT(Area,			pDatabase)
	LOAD_TABLE_OBJECT(RunType,		pDatabase)
	LOAD_TABLE_OBJECT(Class,		pDatabase)
	LOAD_TABLE_OBJECT(SysOpt,		pDatabase)
	LOAD_TABLE_OBJECT(VDisk,		pDatabase)
	LOAD_TABLE_OBJECT(Favorite,		pDatabase)
	LOAD_TABLE_OBJECT(CmpBootTask,	pDatabase)
	LOAD_TABLE_OBJECT(Client,		pDatabase)
	LOAD_TABLE_OBJECT(SyncTask,		pDatabase)
	LOAD_TABLE_OBJECT(Server,		pDatabase)
	LOAD_TABLE_OBJECT(BootTaskArea,	pDatabase)
	LOAD_TABLE_OBJECT(PlugTool,		pDatabase)
	LOAD_TABLE_OBJECT(SyncGame,		pDatabase)
	LOAD_TABLE_OBJECT(PushGameStatic,pDatabase)

	return 0;
}


int ZTableMgr::Save(IDatabase *pDatabase, uint32 tables)
{
	return 0;
}

	// For File 
int ZTableMgr::Load(void)
{
#ifdef USED_FILE_DATABASE
	LOAD_TABLE_OBJECT(Game,			m_path)
	LOAD_TABLE_OBJECT(Area,			m_path)
	LOAD_TABLE_OBJECT(RunType,		m_path)
	LOAD_TABLE_OBJECT(Class,		m_path)
	LOAD_TABLE_OBJECT(SysOpt,		m_path)
	LOAD_TABLE_OBJECT(VDisk,		m_path)
	LOAD_TABLE_OBJECT(Favorite,		m_path)
	LOAD_TABLE_OBJECT(CmpBootTask,	m_path)
	LOAD_TABLE_OBJECT(Client,		m_path)
	LOAD_TABLE_OBJECT(SyncTask,		m_path)
	LOAD_TABLE_OBJECT(Server,		m_path)
	LOAD_TABLE_OBJECT(BootTaskArea,	m_path)
	LOAD_TABLE_OBJECT(PlugTool,		m_path)
	LOAD_TABLE_OBJECT(SyncGame,		m_path)
	LOAD_TABLE_OBJECT(PushGameStatic,m_path)

#endif 

	return 0;
}

int ZTableMgr::Save(uint32 tables)
{
#ifdef USED_FILE_DATABASE
	SAVE_TABLE_OBJECT(GAME,			Game,			m_path)
	SAVE_TABLE_OBJECT(AREA,			Area,			m_path)
	SAVE_TABLE_OBJECT(RUNTYPE,		RunType,		m_path)
	SAVE_TABLE_OBJECT(CLASS,		Class,			m_path)
	SAVE_TABLE_OBJECT(SYSOPT,		SysOpt,			m_path)
	SAVE_TABLE_OBJECT(VDISK,		VDisk,			m_path)
	SAVE_TABLE_OBJECT(FAVORITE,		Favorite,		m_path)
	SAVE_TABLE_OBJECT(CMPBOOTTASK,  CmpBootTask,	m_path)
	SAVE_TABLE_OBJECT(CLIENT,		Client,			m_path)
	SAVE_TABLE_OBJECT(SYNCTASK,		SyncTask,		m_path)
	SAVE_TABLE_OBJECT(SERVER,		Server,			m_path)
	SAVE_TABLE_OBJECT(BOOTTASKAREA,	BootTaskArea,	m_path)
	SAVE_TABLE_OBJECT(PLUGTOOL,		PlugTool,		m_path)
	SAVE_TABLE_OBJECT(SYNCGame,		SyncGame,		m_path)
	SAVE_TABLE_OBJECT(PUSHGAMESTATIC,PushGameStatic,m_path)

#endif 

	return 0;
}


//选项方便函数
stdex::tString ZTableMgr::GetOption(const TCHAR *key, const stdex::tString& defval)
{
	db::tSysOpt so;
	if (m_pSysOptTable->GetData(key, &so, 0) == 0)
		return so.value;
	
	if (!defval.empty()) {
		utility::Strcpy(so.key, key);
		utility::Strcpy(so.value, defval.c_str());
		m_pSysOptTable->Ensure(&so, MASK_TSYSOPT_VALUE);
	}

	return defval;
}

long ZTableMgr::GetOption(const TCHAR *key, long defval)
{
	db::tSysOpt so;
	if (m_pSysOptTable->GetData(key, &so, 0) == 0)
		return _tstol(so.value);

	if (defval) {
		utility::Strcpy(so.key, key);
		::_stprintf(so.value, _T("%d"), defval);
		m_pSysOptTable->Ensure(&so, MASK_TSYSOPT_VALUE);
	}

	return defval;
}

} //namespace i8desk


