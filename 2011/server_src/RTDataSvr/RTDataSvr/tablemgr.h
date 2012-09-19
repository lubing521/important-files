#ifndef TABLEMGR_H
#define TABLEMGR_H

#include "../../../include/irtdatasvr.h"
#pragma comment(lib, "CustomFileDLL.lib")


#define FORWARD_DECLARE_TABLE(tname) \
	class Z##tname##Table;

#define DECLARE_TABLE_OBJECT(tname) \
	public: \
		I##tname##Table *Get##tname##Table(void); \
		I##tname##Filter *Create##tname##Filter(uint64, uint32, const stdex::tString&, const stdex::tString&); \
		I##tname##Filter *Create##tname##Filter(uint64, uint32, uint32, const stdex::tString&); \
		I##tname##Sorter *Create##tname##Sorter(uint64, uint32); \
		void Destroy##tname##Filter(I##tname##Filter *); \
		void Destroy##tname##Sorter(I##tname##Sorter *);


#define LOAD_TABLE_OBJECT(tname, param) \
	m_p##tname##Table->Load(param);

#define SAVE_TABLE_OBJECT(tflag, tname, param) \
	if (tables & FLAG_##tflag##_BIT) \
		m_p##tname##Table->Save(param);

#define IMPLMENT_TABLE_OBJECT(tname) \
	I##tname##Table *ZTableMgr::Get##tname##Table(void) \
	{ \
		return m_p##tname##Table.get(); \
	} \
	I##tname##Filter *ZTableMgr::Create##tname##Filter(uint64 CondMask, \
											 uint32 CondMode,  \
											 uint32 dwParam, \
											 const stdex::tString& strParam) \
	{ \
		return m_p##tname##Table->CreateFilter(CondMask, CondMode, dwParam, strParam); \
	} \
	I##tname##Sorter *ZTableMgr::Create##tname##Sorter(uint64 mask, uint32 desc) \
	{ \
		return m_p##tname##Table->CreateSorter(mask, desc); \
	} \
	void ZTableMgr::Destroy##tname##Filter(I##tname##Filter *pFilter) \
	{  \
		m_p##tname##Table->DestroyFilter( pFilter ); \
	} \
	void ZTableMgr::Destroy##tname##Sorter(I##tname##Sorter *pSorter)  \
	{ \
		m_p##tname##Table->DestroySorter( pSorter ); \
	} 

namespace i8desk {

FORWARD_DECLARE_TABLE(Game)
FORWARD_DECLARE_TABLE(Area)
FORWARD_DECLARE_TABLE(RunType)
FORWARD_DECLARE_TABLE(Class)
FORWARD_DECLARE_TABLE(SysOpt)
FORWARD_DECLARE_TABLE(VDisk)
FORWARD_DECLARE_TABLE(Favorite)
FORWARD_DECLARE_TABLE(CmpBootTask)
FORWARD_DECLARE_TABLE(Client)
//FORWARD_DECLARE_TABLE(ClientStatus)
FORWARD_DECLARE_TABLE(VDiskStatus)
//FORWARD_DECLARE_TABLE(Icon)
FORWARD_DECLARE_TABLE(SyncTask)
FORWARD_DECLARE_TABLE(Server)
FORWARD_DECLARE_TABLE(BootTaskArea)
FORWARD_DECLARE_TABLE(ModuleUsage)
FORWARD_DECLARE_TABLE(Module2Usage)
FORWARD_DECLARE_TABLE(ServerStatus)
FORWARD_DECLARE_TABLE(DiskStatus)
FORWARD_DECLARE_TABLE(VDiskClient)
FORWARD_DECLARE_TABLE(TaskStatus)
FORWARD_DECLARE_TABLE(UpdateGameStatus)
FORWARD_DECLARE_TABLE(PlugTool)
FORWARD_DECLARE_TABLE(PlugToolStatus)
FORWARD_DECLARE_TABLE(GameExVersion)
FORWARD_DECLARE_TABLE(SyncGame)
FORWARD_DECLARE_TABLE(SyncTaskStatus)
FORWARD_DECLARE_TABLE(PushGameStatic)



class ZTableMgr 
	: public IRTDataSvr
{
public:
	ZTableMgr(void);
	virtual ~ZTableMgr(void);

	DECLARE_TABLE_OBJECT(Game)
	DECLARE_TABLE_OBJECT(Area)
	DECLARE_TABLE_OBJECT(RunType)
	DECLARE_TABLE_OBJECT(Class)
	DECLARE_TABLE_OBJECT(SysOpt)
	DECLARE_TABLE_OBJECT(VDisk)
	DECLARE_TABLE_OBJECT(Favorite)
	DECLARE_TABLE_OBJECT(CmpBootTask)
	DECLARE_TABLE_OBJECT(Client)
	DECLARE_TABLE_OBJECT(VDiskStatus)
	DECLARE_TABLE_OBJECT(SyncTask)
	DECLARE_TABLE_OBJECT(Server)
	DECLARE_TABLE_OBJECT(BootTaskArea)
	DECLARE_TABLE_OBJECT(ModuleUsage)
	DECLARE_TABLE_OBJECT(Module2Usage)
	DECLARE_TABLE_OBJECT(ServerStatus)
	DECLARE_TABLE_OBJECT(DiskStatus)
	DECLARE_TABLE_OBJECT(VDiskClient)
	DECLARE_TABLE_OBJECT(TaskStatus)
	DECLARE_TABLE_OBJECT(UpdateGameStatus)
	DECLARE_TABLE_OBJECT(PlugTool)
	DECLARE_TABLE_OBJECT(PlugToolStatus)
	DECLARE_TABLE_OBJECT(GameExVersion)
	DECLARE_TABLE_OBJECT(SyncGame)
	DECLARE_TABLE_OBJECT(SyncTaskStatus)
	DECLARE_TABLE_OBJECT(PushGameStatic)

//initialize and uninitialize
public:
	// For database
	int Load(IDatabase *pDataBase);
	int Save(IDatabase *pDataBase, uint32 tables = ~0L);
	// For File 
	int Load(void);
	int Save(uint32 tables = ~0L);

//选项方便函数
public:
	stdex::tString GetOption(const TCHAR *key, const stdex::tString& defval = _T(""));
	long GetOption(const TCHAR *key, long defval);
//	ZTaskStatusTable *GetTaskStatus(void){ return m_pTaskStatusTable; };

private:
	stdex::tString m_path;

	std::auto_ptr<ZGameTable> m_pGameTable; 
	std::auto_ptr<ZAreaTable> m_pAreaTable; 
	std::auto_ptr<ZRunTypeTable> m_pRunTypeTable; 
	std::auto_ptr<ZClassTable> m_pClassTable; 
	std::auto_ptr<ZSysOptTable> m_pSysOptTable; 
	std::auto_ptr<ZVDiskTable> m_pVDiskTable; 
	std::auto_ptr<ZFavoriteTable> m_pFavoriteTable; 
	std::auto_ptr<ZCmpBootTaskTable> m_pCmpBootTaskTable; 
	std::auto_ptr<ZClientTable> m_pClientTable; 
	std::auto_ptr<ZVDiskStatusTable> m_pVDiskStatusTable; 
	std::auto_ptr<ZSyncTaskTable> m_pSyncTaskTable; 
	std::auto_ptr<ZServerTable> m_pServerTable; 
	std::auto_ptr<ZBootTaskAreaTable> m_pBootTaskAreaTable; 
	std::auto_ptr<ZModuleUsageTable> m_pModuleUsageTable;
	std::auto_ptr<ZModule2UsageTable> m_pModule2UsageTable; 
	std::auto_ptr<ZServerStatusTable> m_pServerStatusTable; 
	std::auto_ptr<ZDiskStatusTable> m_pDiskStatusTable; 
	std::auto_ptr<ZVDiskClientTable> m_pVDiskClientTable; 
	std::auto_ptr<ZTaskStatusTable> m_pTaskStatusTable; 
	std::auto_ptr<ZUpdateGameStatusTable> m_pUpdateGameStatusTable; 
	std::auto_ptr<ZPlugToolTable> m_pPlugToolTable; 
	std::auto_ptr<ZPlugToolStatusTable> m_pPlugToolStatusTable; 
	std::auto_ptr<ZGameExVersionTable> m_pGameExVersionTable; 
	std::auto_ptr<ZSyncGameTable> m_pSyncGameTable; 
	std::auto_ptr<ZSyncTaskStatusTable> m_pSyncTaskStatusTable; 
	std::auto_ptr<ZPushGameStaticTable> m_pPushGameStaticTable; 

	

};




} //namespace i8desk

#endif TABLEMGR_H
