#ifndef IRTDATASVR_H
#define IRTDATASVR_H

#include "tablestruct.h"

namespace i8desk {

///////////////////////////////////////////////////////////////////////////////////////////////////
// Table interface define macro
// Kty 主键类型, Name 表名(不包含字符't')
//
#define DECLARE_TABLE_INTERFACE(Kty, Name) \
struct I##Name##Filter \
{ \
	virtual bool bypass(const db::t##Name *const) = 0; \
}; \
struct I##Name##Sorter \
{ \
	virtual bool sort(const db::t##Name *const, const db::t##Name *const) = 0; \
}; \
struct I##Name##Recordset \
{ \
	virtual void Release(void) = 0; \
	virtual uint32 GetCount(void) const = 0; \
	virtual bool GetData(uint32, db::t##Name *, ulong * = 0) const = 0; \
	virtual db::t##Name *GetData(uint32, ulong * = 0) const = 0; \
}; \
typedef std::tr1::shared_ptr<I##Name##Recordset> Name##Recordset_Ptr; \
struct I##Name##EventHandler \
{ \
	virtual uint32 GetEvents(void) = 0; \
	virtual void HandleEvent(uint32, Kty, uint64) = 0; \
}; \
struct I##Name##Table \
{ \
	virtual ulong GetVersion(void) = 0; \
	virtual int Insert(const db::t##Name *const) = 0; \
	virtual int Ensure(const db::t##Name *const, uint64) = 0; \
	virtual int Update(Kty, const db::t##Name *const, uint64) = 0; \
	virtual int Update(const db::t##Name *const, uint64, I##Name##Filter *) = 0; \
	virtual int Cumulate(Kty, const db::t##Name *const, uint64) = 0; \
	virtual int Cumulate(const db::t##Name *const, uint64, I##Name##Filter *) = 0; \
	virtual int Delete(Kty) = 0; \
	virtual int Delete(I##Name##Filter *) = 0; \
	virtual int DeleteAll() = 0; \
	virtual int GetData(Kty, db::t##Name *, ulong * = 0) = 0; \
	virtual int Select(I##Name##Recordset **, I##Name##Filter *, I##Name##Sorter *) = 0; \
	virtual void RegisterEventHandler(I##Name##EventHandler *) = 0; \
	virtual void UnregisterEventHandler(I##Name##EventHandler *) = 0; \
};

//由两个字段组成主键
//Kty1 主键的第一个字段类型，Kty2 主键的第二个字段类型
#define DECLARE_TABLE_INTERFACE_2K(Kty1, Kty2, Name) \
struct I##Name##Filter \
{ \
	virtual bool bypass(const db::t##Name *const) = 0; \
}; \
struct I##Name##Sorter \
{ \
	virtual bool sort(const db::t##Name *const, const db::t##Name *const) = 0; \
}; \
struct I##Name##Recordset \
{ \
	virtual void Release(void) = 0; \
	virtual uint32 GetCount(void) const = 0; \
	virtual bool GetData(uint32, db::t##Name *, ulong * = 0) const = 0; \
	virtual db::t##Name *GetData(uint32, ulong * = 0) const = 0; \
}; \
typedef std::tr1::shared_ptr<I##Name##Recordset> Name##Recordset_Ptr; \
struct I##Name##EventHandler \
{ \
	virtual uint32 GetEvents(void) = 0; \
	virtual void HandleEvent(uint32, Kty1, Kty2, uint64) = 0; \
}; \
struct I##Name##Table \
{ \
	virtual ulong GetVersion(void) = 0; \
	virtual int Insert(const db::t##Name *const) = 0; \
	virtual int Ensure(const db::t##Name *const, uint64) = 0; \
	virtual int Update(Kty1, const db::t##Name *const, uint64) = 0; \
	virtual int Update(Kty1, Kty2, const db::t##Name *const, uint64) = 0; \
	virtual int Update(const db::t##Name *const, uint64, I##Name##Filter *) = 0; \
	virtual int Cumulate(Kty1, const db::t##Name *const, uint64) = 0; \
	virtual int Cumulate(Kty1, Kty2, const db::t##Name *const, uint64) = 0; \
	virtual int Cumulate(const db::t##Name *const, uint64, I##Name##Filter *) = 0; \
	virtual int Delete(Kty1) = 0; \
	virtual int Delete(Kty1, Kty2) = 0; \
	virtual int Delete(I##Name##Filter *) = 0; \
	virtual int DeleteAll() = 0; \
	virtual int GetData(Kty1, Kty2, db::t##Name *, ulong * = 0) = 0; \
	virtual int Select(I##Name##Recordset **, Kty1, I##Name##Sorter *) = 0; \
	virtual int Select(I##Name##Recordset **, I##Name##Filter *, I##Name##Sorter *) = 0; \
	virtual void RegisterEventHandler(I##Name##EventHandler *) = 0; \
	virtual void UnregisterEventHandler(I##Name##EventHandler *) = 0; \
};


DECLARE_TABLE_INTERFACE(uint32, Game)
DECLARE_TABLE_INTERFACE(const TCHAR *, Class)
DECLARE_TABLE_INTERFACE_2K(uint32,const TCHAR * , RunType)
DECLARE_TABLE_INTERFACE(const TCHAR *, SysOpt)
DECLARE_TABLE_INTERFACE(const TCHAR *, Area)
DECLARE_TABLE_INTERFACE(const TCHAR *, Client)
DECLARE_TABLE_INTERFACE(const TCHAR *, Favorite)
DECLARE_TABLE_INTERFACE(const TCHAR *, VDisk)
DECLARE_TABLE_INTERFACE(const TCHAR *, CmpBootTask)
DECLARE_TABLE_INTERFACE(uint32, PlugTool)
DECLARE_TABLE_INTERFACE_2K( uint32, const TCHAR *, SyncGame)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *, uint32,  SyncTaskStatus)
DECLARE_TABLE_INTERFACE(const TCHAR *, SyncTask)
DECLARE_TABLE_INTERFACE(const TCHAR *, Server)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *, const TCHAR *, BootTaskArea)

//DECLARE_TABLE_INTERFACE(ulong, ClientStatus)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *, uint32, VDiskStatus)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *,uint32, VDiskClient)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *, uint32, ModuleUsage)
DECLARE_TABLE_INTERFACE(const TCHAR *, Module2Usage)
DECLARE_TABLE_INTERFACE(const TCHAR *, ServerStatus)
DECLARE_TABLE_INTERFACE_2K(const TCHAR *, uint32, DiskStatus)
DECLARE_TABLE_INTERFACE(uint32, TaskStatus)
DECLARE_TABLE_INTERFACE(uint32, UpdateGameStatus)
DECLARE_TABLE_INTERFACE(uint32, PlugToolStatus)
DECLARE_TABLE_INTERFACE(uint32, GameExVersion)
DECLARE_TABLE_INTERFACE(uint32, PushGameStatic)



#ifndef RTDATASVR_PLUG_NAME
#	define RTDATASVR_PLUG_NAME			TEXT("RTDataSvr")		
#endif 

#define RTDS_CMD_GET_CLIENTLIST			0x01   //SendMessage cmd
#define RTDS_CMD_GET_UPDATESYSOPT		0x02   //SendMessage cmd
#define RTDS_CMD_GET_UPDATECLIENTVER	0x03   //SendMessage cmd


struct GamePlugNotify
{
	size_t TaskNotifyType;
	size_t TaskNotify;
	long Gid;
	size_t Error_;
};

// GamePlugNotify notify;
// SendMessage(RTDS_CMD_NOTIFYCONSOEL, &notify, 0);

#define RTDS_CMD_NOTIFYCONSOEL			0x04   //SendMessage cmd
#define RTDS_CMD_NOTIFYCHECKUSER		0x05
#define RTDS_CMD_GET_CONSOLE_IP			0x06
#define RTDS_CMD_REFRESH_PL				0x07


#define RTDS_CMD_GET_INTERFACE			0x10	//SendMessage cmd

#define RTDS_CMD_ICON_OPERATE			0x20	//SendMessage cmd
#define RTDS_CMD_ICON_OPENSURE			0x21	//SendMessage cmd
#define RTDS_CMD_ICON_OPDELETE			0x22	//SendMessage cmd

#define RTDS_INTERFACE_RTDATASVR	0		//IRTDataSvr
#define RTDS_INTERFACE_ICONTABLE	1		//IIconTable
// 插件RTDataSvr之SendMessage返回值为IRTDataSvr *
 /*IIconTable *pIconTable = reinterpret_cast<IIconTable *>(
	   pRTDataSvrPlug->SendMessage(RTDS_CMD_GET_INTERFACE,
	   RTDS_INTERFACE_RTDATASVR, 0));*/


#define IMPORT_TABLE_INTERFACE(tname) \
	virtual I##tname##Table *Get##tname##Table(void) = 0;

struct IIconTable
{
	 //virtual  bool  Ensure(IN long gid, IN size_t size, IN const void *data) = 0;
	 //virtual  bool  Delete(IN long gid) = 0;
	 //virtual  bool  GetFileData(IN long gid, OUT size_t &size, OUT void *&data) = 0;

};

////////////////////////////////////////////////////////////////////////////////////////
//// IRTDataSvr
struct IRTDataSvr
{
	virtual IGameTable *GetGameTable(void) = 0;
	virtual IClassTable *GetClassTable(void) = 0;
	virtual IRunTypeTable *GetRunTypeTable(void) = 0;
	virtual ISysOptTable *GetSysOptTable(void) = 0;
	virtual IAreaTable *GetAreaTable(void) = 0;
	virtual IClientTable *GetClientTable(void) = 0;
	virtual IFavoriteTable *GetFavoriteTable(void) = 0;
	virtual IVDiskTable *GetVDiskTable(void) = 0;
	virtual ICmpBootTaskTable *GetCmpBootTaskTable(void) = 0;
	virtual IVDiskStatusTable *GetVDiskStatusTable(void) = 0;
	virtual ISyncTaskTable *GetSyncTaskTable(void) = 0;
	virtual IServerTable *GetServerTable(void) = 0;
	virtual IBootTaskAreaTable *GetBootTaskAreaTable(void) = 0;
	virtual IModuleUsageTable *GetModuleUsageTable(void) = 0;
	virtual IModule2UsageTable *GetModule2UsageTable(void) = 0;
	virtual IServerStatusTable *GetServerStatusTable(void) = 0;
	virtual IDiskStatusTable *GetDiskStatusTable(void) = 0;
	virtual IVDiskClientTable *GetVDiskClientTable(void) = 0;
	virtual ITaskStatusTable *GetTaskStatusTable(void) = 0;
	virtual IUpdateGameStatusTable *GetUpdateGameStatusTable(void) = 0;
	virtual IPlugToolTable *GetPlugToolTable(void) = 0;
	virtual IPlugToolStatusTable *GetPlugToolStatusTable(void) = 0;
	virtual IGameExVersionTable *GetGameExVersionTable(void) = 0;
	virtual ISyncGameTable *GetSyncGameTable(void) = 0;
	virtual ISyncTaskStatusTable *GetSyncTaskStatusTable(void) = 0;
	virtual IPushGameStaticTable *GetPushGameStaticTable(void) = 0;


};

}

#endif //IRTDATASVR_H