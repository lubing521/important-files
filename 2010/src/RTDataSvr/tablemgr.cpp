#include "StdAfx.h"

#include "tablemgr.h"
#include "dbtables.h"
#include "tptables.h"

namespace i8desk {

//////////////////////////////////////////////////////
//ZTableMgr


ZTableMgr::ZTableMgr(void)
{
	m_path = GetAppPath() + _T("data\\db\\");

	CREATE_TABLE_OBJECT(Game)
	CREATE_TABLE_OBJECT(Area)
	CREATE_TABLE_OBJECT(GameArea)
	CREATE_TABLE_OBJECT(Class)
	CREATE_TABLE_OBJECT(SysOpt)
	CREATE_TABLE_OBJECT(User)
	CREATE_TABLE_OBJECT(VDisk)
	CREATE_TABLE_OBJECT(Favorite)
	CREATE_TABLE_OBJECT(CmpStartTask)
	CREATE_TABLE_OBJECT(Client)
	CREATE_TABLE_OBJECT(ClientStatus)
	CREATE_TABLE_OBJECT(VDiskStatus)
	CREATE_TABLE_OBJECT(Icon)
	CREATE_TABLE_OBJECT(SyncTask)
	CREATE_TABLE_OBJECT(SyncGame)
	CREATE_TABLE_OBJECT(ModuleUsage)
	CREATE_TABLE_OBJECT(ServerStatus)
	CREATE_TABLE_OBJECT(DiskStatus)
}


ZTableMgr::~ZTableMgr(void)
{
	DESTROY_TABLE_OBJECT(Game)
	DESTROY_TABLE_OBJECT(Area)
	DESTROY_TABLE_OBJECT(GameArea)
	DESTROY_TABLE_OBJECT(Class)
	DESTROY_TABLE_OBJECT(SysOpt)
	DESTROY_TABLE_OBJECT(User)
	DESTROY_TABLE_OBJECT(VDisk)
	DESTROY_TABLE_OBJECT(Favorite)
	DESTROY_TABLE_OBJECT(CmpStartTask)
	DESTROY_TABLE_OBJECT(Client)
	DESTROY_TABLE_OBJECT(ClientStatus)
	DESTROY_TABLE_OBJECT(VDiskStatus)
	DESTROY_TABLE_OBJECT(Icon)
	DESTROY_TABLE_OBJECT(SyncTask)
	DESTROY_TABLE_OBJECT(SyncGame)
	DESTROY_TABLE_OBJECT(ModuleUsage)
	DESTROY_TABLE_OBJECT(ServerStatus)
	DESTROY_TABLE_OBJECT(DiskStatus)
}

IMPLMENT_TABLE_OBJECT(Game)
IMPLMENT_TABLE_OBJECT(Area)
IMPLMENT_TABLE_OBJECT(GameArea)
IMPLMENT_TABLE_OBJECT(Class)
IMPLMENT_TABLE_OBJECT(SysOpt)
IMPLMENT_TABLE_OBJECT(User)
IMPLMENT_TABLE_OBJECT(VDisk)
IMPLMENT_TABLE_OBJECT(Favorite)
IMPLMENT_TABLE_OBJECT(CmpStartTask)
IMPLMENT_TABLE_OBJECT(Client)
IMPLMENT_TABLE_OBJECT(ClientStatus)
IMPLMENT_TABLE_OBJECT(VDiskStatus)
IMPLMENT_TABLE_OBJECT(Icon)
IMPLMENT_TABLE_OBJECT(SyncTask)
IMPLMENT_TABLE_OBJECT(SyncGame)
IMPLMENT_TABLE_OBJECT(ModuleUsage)
IMPLMENT_TABLE_OBJECT(ServerStatus)
IMPLMENT_TABLE_OBJECT(DiskStatus)

//initialize and uninitialize
	// For database
int ZTableMgr::Load(IDatabase *pDatabase)
{
	LOAD_TABLE_OBJECT(Game,			pDatabase)
	LOAD_TABLE_OBJECT(Area,			pDatabase)
	LOAD_TABLE_OBJECT(GameArea,		pDatabase)
	LOAD_TABLE_OBJECT(Class,		pDatabase)
	LOAD_TABLE_OBJECT(SysOpt,		pDatabase)
	LOAD_TABLE_OBJECT(User,			pDatabase)
	LOAD_TABLE_OBJECT(VDisk,		pDatabase)
	LOAD_TABLE_OBJECT(Favorite,		pDatabase)
	LOAD_TABLE_OBJECT(CmpStartTask,	pDatabase)
	LOAD_TABLE_OBJECT(Client,		pDatabase)
	LOAD_TABLE_OBJECT(SyncTask,		pDatabase)
	LOAD_TABLE_OBJECT(SyncGame,		pDatabase)
	LOAD_TABLE_OBJECT(Icon,			pDatabase)

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
	LOAD_TABLE_OBJECT(GameArea,		m_path)
	LOAD_TABLE_OBJECT(Class,		m_path)
	LOAD_TABLE_OBJECT(SysOpt,		m_path)
	LOAD_TABLE_OBJECT(User,			m_path)
	LOAD_TABLE_OBJECT(VDisk,		m_path)
	LOAD_TABLE_OBJECT(Favorite,		m_path)
	LOAD_TABLE_OBJECT(CmpStartTask,	m_path)
	LOAD_TABLE_OBJECT(Client,		m_path)
	LOAD_TABLE_OBJECT(SyncTask,		m_path)
	LOAD_TABLE_OBJECT(SyncGame,		m_path)
	LOAD_TABLE_OBJECT(Icon,			m_path)
#endif 

	return 0;
}

int ZTableMgr::Save(uint32 tables)
{
#ifdef USED_FILE_DATABASE
	SAVE_TABLE_OBJECT(GAME,			Game,			m_path)
	SAVE_TABLE_OBJECT(AREA,			Area,			m_path)
	SAVE_TABLE_OBJECT(GAMEAREA,		GameArea,		m_path)
	SAVE_TABLE_OBJECT(CLASS,		Class,			m_path)
	SAVE_TABLE_OBJECT(SYSOPT,		SysOpt,			m_path)
	SAVE_TABLE_OBJECT(USER,			User,			m_path)
	SAVE_TABLE_OBJECT(VDISK,		VDisk,			m_path)
	SAVE_TABLE_OBJECT(FAVORITE,		Favorite,		m_path)
	SAVE_TABLE_OBJECT(CMPSTARTTASK, CmpStartTask,	m_path)
	SAVE_TABLE_OBJECT(CLIENT,		Client,			m_path)
	SAVE_TABLE_OBJECT(SYNCTASK,		SyncTask,		m_path)
	SAVE_TABLE_OBJECT(SYNCGAME,		SyncGame,		m_path)
	SAVE_TABLE_OBJECT(ICON,			Icon,			m_path)
#endif 

	return 0;
}


//选项方便函数
std::string ZTableMgr::GetOption(const char *key, const std::string& defval)
{
	db::tSysOpt so;
	if (m_pSysOptTable->GetData(key, &so, 0) == 0)
		return so.value;
	
	if (!defval.empty()) {
		SAFE_STRCPY(so.key, key);
		SAFE_STRCPY(so.value, defval.c_str());
		m_pSysOptTable->Ensure(&so, MASK_TSYSOPT_VALUE);
	}

	return defval;
}

long ZTableMgr::GetOption(const char *key, long defval)
{
	db::tSysOpt so;
	if (m_pSysOptTable->GetData(key, &so, 0) == 0)
		return atol(so.value);

	if (defval) {
		SAFE_STRCPY(so.key, key);
		::sprintf(so.value, "%d", defval);
		m_pSysOptTable->Ensure(&so, MASK_TSYSOPT_VALUE);
	}

	return defval;
}

} //namespace i8desk


