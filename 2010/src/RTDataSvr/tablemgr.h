#ifndef TABLEMGR_H
#define TABLEMGR_H

#include "../../include/irtdatasvr.h"

#define FORWARD_DECLARE_TABLE(tname) \
	class Z##tname##Table;

#define DECLARE_TABLE_OBJECT(tname) \
	public: \
		I##tname##Table *Get##tname##Table(void); \
		I##tname##Filter *Create##tname##Filter(uint64, uint32, uint32, const std::string&); \
		I##tname##Sorter *Create##tname##Sorter(uint64, uint32); \
		void Destroy##tname##Filter(I##tname##Filter *); \
		void Destroy##tname##Sorter(I##tname##Sorter *); \
	private: \
		Z##tname##Table *m_p##tname##Table; 

#define CREATE_TABLE_OBJECT(tname) \
	 m_p##tname##Table = new Z##tname##Table;

#define DESTROY_TABLE_OBJECT(tname) \
	 delete m_p##tname##Table;

#define LOAD_TABLE_OBJECT(tname, param) \
	m_p##tname##Table->Load(param);

#define SAVE_TABLE_OBJECT(tflag, tname, param) \
	if (tables & FLAG_##tflag##_BIT) \
		m_p##tname##Table->Save(param);

#define IMPLMENT_TABLE_OBJECT(tname) \
	I##tname##Table *ZTableMgr::Get##tname##Table(void) \
	{ \
		return m_p##tname##Table; \
	} \
	I##tname##Filter *ZTableMgr::Create##tname##Filter(uint64 CondMask, \
											 uint32 CondMode,  \
											 uint32 dwParam, \
											 const std::string& strParam) \
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
FORWARD_DECLARE_TABLE(GameArea)
FORWARD_DECLARE_TABLE(Class)
FORWARD_DECLARE_TABLE(SysOpt)
FORWARD_DECLARE_TABLE(User)
FORWARD_DECLARE_TABLE(VDisk)
FORWARD_DECLARE_TABLE(Favorite)
FORWARD_DECLARE_TABLE(CmpStartTask)
FORWARD_DECLARE_TABLE(Client)
FORWARD_DECLARE_TABLE(ClientStatus)
FORWARD_DECLARE_TABLE(VDiskStatus)
FORWARD_DECLARE_TABLE(Icon)
FORWARD_DECLARE_TABLE(SyncTask)
FORWARD_DECLARE_TABLE(SyncGame)
FORWARD_DECLARE_TABLE(ModuleUsage)
FORWARD_DECLARE_TABLE(ServerStatus)
FORWARD_DECLARE_TABLE(DiskStatus)

class ZTableMgr 
	: public IRTDataSvr
{
public:
	ZTableMgr(void);
	virtual ~ZTableMgr(void);

	DECLARE_TABLE_OBJECT(Game)
	DECLARE_TABLE_OBJECT(Area)
	DECLARE_TABLE_OBJECT(GameArea)
	DECLARE_TABLE_OBJECT(Class)
	DECLARE_TABLE_OBJECT(SysOpt)
	DECLARE_TABLE_OBJECT(User)
	DECLARE_TABLE_OBJECT(VDisk)
	DECLARE_TABLE_OBJECT(Favorite)
	DECLARE_TABLE_OBJECT(CmpStartTask)
	DECLARE_TABLE_OBJECT(Client)
	DECLARE_TABLE_OBJECT(ClientStatus)
	DECLARE_TABLE_OBJECT(VDiskStatus)
	DECLARE_TABLE_OBJECT(Icon)
	DECLARE_TABLE_OBJECT(SyncTask)
	DECLARE_TABLE_OBJECT(SyncGame)
	DECLARE_TABLE_OBJECT(ModuleUsage)
	DECLARE_TABLE_OBJECT(ServerStatus)
	DECLARE_TABLE_OBJECT(DiskStatus)

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
	std::string GetOption(const char *key, const std::string& defval = "");
	long GetOption(const char *key, long defval);

private:
	std_string m_path;
};




} //namespace i8desk

#endif TABLEMGR_H
