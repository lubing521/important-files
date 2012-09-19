#ifndef IRTDATASVR_H
#define IRTDATASVR_H

#include "i8type.h"

#include <memory>

namespace i8desk {

/*

///////////////////////////////////////////////////////////////////////////////////////////////////
// game

// 要过滤掉的返回true
// 不能在其中直接或间接调用IGameTable的方法
struct IGameFilter
{
	virtual bool bypass(const db::tGame *const g) = 0;
};

// g1排在g2前面时返回true
// 不能在其中直接或间接调用IGameTable的方法
struct IGameSorter
{
	virtual bool sort(const db::tGame *const g1, const db::tGame *const g2) = 0;
};

// 游戏数据记录集，此接口由组件返回
// 必须也只能调用一次Release来释放
// 如不需要crc则传空指针即可，下同 
//
struct IGameRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tGame *g, ulong *ver = 0) const = 0;
	virtual db::tGame *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IGameRecordset> GameRecordset_Ptr;
//
//构造GameRecordset_Ptr时必须使用IGameRecordset::Release方法作为释放方法
//示例如下: 
//GameRecordset_Ptr rst = GameRecordset_Ptr(pRecordset, 
//	  std::tr1::bind(&IGameRecordset::Release, _1));
//以后rst可以任意复制保存，但是不得再用pRecordset
//

//游戏表事件处理接口
// optype: 操作类型，在define中定义
// #define SQL_OP_NONE						 0x00
// #define SQL_OP_SELECT					 0x01
// #define SQL_OP_INSERT					 0x02
// #define SQL_OP_UPDATE					 0x04
// #define SQL_OP_DELETE					 0x08
// mask: 字段俺码，在define中定义
// 可以在其中调用IGameTable的方法
// GetEvents 返回需要的事件类型
// HandleEvent必须是多线程安全且可重入的
//
struct IGameEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, uint32 gid, uint64 mask) = 0;
};

//游戏表接口
// recordset为组件内部分配，调用者必使用其Release惟一地释放一次
// Select方法返回成功recordset必有效，失败必无效
// Insert方法在记录存在时返回失败
// Update方法在记录不存在时返回失败
// Ensure方法会在记录不存在时尝试插入一条记录，否则更新之
// Cumulate为累积方法，对整数字段为累加，对字符串字段为连接
// 返回0表示操作成功，其他为失败
// 除crc、filter、sorter外，其他指针必须有效
//
struct IGameTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tGame *const g) = 0;
	virtual int Ensure(const db::tGame *const g, uint64 mask) = 0;
	virtual int Update(uint32 gid, const db::tGame *const g, uint64 mask) = 0;
	virtual int Update(const db::tGame *const g, uint64 mask, IGameFilter *filter) = 0;
	virtual int Cumulate(uint32 gid, const db::tGame *const g, uint64 mask) = 0;
	virtual int Cumulate(const db::tGame *const g, uint64 mask, IGameFilter *filter) = 0;
	virtual int Delete(uint32 gid) = 0;
	virtual int Delete(IGameFilter *filter) = 0;
	virtual int GetData(uint32 gid, db::tGame *g, ulong *ver = 0) = 0;
	virtual int Select(IGameRecordset **recordset, IGameFilter *filter, IGameSorter *sorter) = 0;
	virtual void RegisterEventHandler(IGameEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IGameEventHandler *handler) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// class
struct IClassFilter
{
	virtual bool bypass(const db::tClass *const c) = 0;
};

struct IClassSorter
{
	virtual bool sort(const db::tClass *const c1, const db::tClass *const c2) = 0;
};

struct IClassRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tClass *c, ulong *ver = 0) const = 0;
	virtual db::tClass *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IClassRecordset> ClassRecordset_Ptr;

struct IClassEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *def, uint64 mask) = 0;
};

struct IClassTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tClass *const c) = 0;
	virtual int Ensure(const db::tClass *const c, uint64 mask) = 0;
	virtual int Update(const char *def, const db::tClass *const c, uint64 mask) = 0;
	virtual int Update(const db::tClass *const g, uint64 mask, IClassFilter *filter) = 0;
	virtual int Cumulate(const char *def, const db::tClass *const c, uint64 mask) = 0;
	virtual int Cumulate(const db::tClass *const g, uint64 mask, IClassFilter *filter) = 0;
	virtual int Delete(const char *def) = 0;
	virtual int Delete(IClassFilter *filter) = 0;
	virtual int GetData(const char *def, db::tClass *c, ulong *ver = 0) = 0;
	virtual int Select(IClassRecordset **recordset, IClassFilter *filter, IClassSorter *sorter) = 0;
	virtual void RegisterEventHandler(IClassEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IClassEventHandler *handler) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// GameArea
struct IGameAreaFilter
{
	virtual bool bypass(const db::tGameArea *const c) = 0;
};

struct IGameAreaSorter
{
	virtual bool sort(const db::tGameArea *const ga1, const db::tGameArea *const ga2) = 0;
};

struct IGameAreaRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tGameArea *ga, ulong *ver = 0) const = 0;
	virtual db::tGameArea *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IGameAreaRecordset> GameAreaRecordset_Ptr;

struct IGameAreaEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, uint32 gid, const char *aid, uint64 mask) = 0;
};

struct IGameAreaTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tGameArea *const ga) = 0;
	virtual int Ensure(const db::tGameArea *const ga, uint64 mask) = 0;
	virtual int Update(uint32 gid, const db::tGameArea *const ga, uint64 mask) = 0;
	virtual int Update(uint32 gid, const char *aid, const db::tGameArea *const ga, uint64 mask) = 0;
	virtual int Update(const db::tGameArea *const ga, uint64 mask, IGameAreaFilter *filter) = 0;
	virtual int Cumulate(uint32 gid, const db::tGameArea *const ga, uint64 mask) = 0;
	virtual int Cumulate(uint32 gid, const char *aid, const db::tGameArea *const ga, uint64 mask) = 0;
	virtual int Cumulate(const db::tGameArea *const ga, uint64 mask, IGameAreaFilter *filter) = 0;
	virtual int Delete(uint32 gid) = 0;
	virtual int Delete(uint32 gid, const char *aid) = 0;
	virtual int Delete(IGameAreaFilter *filter) = 0;
	virtual int GetData(uint32 gid, const char *aid, db::tGameArea *ga, ulong *ver = 0) = 0;
	virtual int Select(IGameAreaRecordset **recordset, uint32 gid, IGameAreaSorter *sorter) = 0;
	virtual int Select(IGameAreaRecordset **recordset, IGameAreaFilter *filter, IGameAreaSorter *sorter) = 0;
	virtual void RegisterEventHandler(IGameAreaEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IGameAreaEventHandler *handler) = 0;
};



///////////////////////////////////////////////////////////////////////////////////////////////////
// SysOpt
struct ISysOptFilter
{
	virtual bool bypass(const db::tSysOpt *const so) = 0;
};

struct ISysOptSorter
{
	virtual bool sort(const db::tSysOpt *const so1, const db::tSysOpt *const so2) = 0;
};

struct ISysOptRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tSysOpt *so, ulong *ver = 0) const = 0;
	virtual db::tSysOpt *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<ISysOptRecordset> SysOptRecordset_Ptr;

struct ISysOptEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *key, uint64 mask) = 0;
};

struct ISysOptTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tSysOpt *const so) = 0;
	virtual int Ensure(const db::tSysOpt *const so, uint64 mask) = 0;
	virtual int Update(const char *key, const db::tSysOpt *const so, uint64 mask) = 0;
	virtual int Update(const db::tSysOpt *const so, uint64 mask, ISysOptFilter *filter) = 0;
	virtual int Cumulate(const char *key, const db::tSysOpt *const so, uint64 mask) = 0;
	virtual int Cumulate(const db::tSysOpt *const so, uint64 mask, ISysOptFilter *filter) = 0;
	virtual int Delete(const char *key) = 0;
	virtual int Delete(ISysOptFilter *filter) = 0;
	virtual int GetData(const char *key, db::tSysOpt *so, ulong *ver = 0) = 0;
	virtual int Select(ISysOptRecordset **recordset, ISysOptFilter *filter, ISysOptSorter *sorter) = 0;
	virtual void RegisterEventHandler(ISysOptEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(ISysOptEventHandler *handler) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Area
struct IAreaFilter
{
	virtual bool bypass(const db::tArea *const a) = 0;
};

struct IAreaSorter
{
	virtual bool sort(const db::tArea *const a1, const db::tArea *const a2) = 0;
};

struct IAreaRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tArea *a, ulong *ver = 0) const = 0;
	virtual db::tArea *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IAreaRecordset> AreaRecordset_Ptr;

struct IAreaEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *aid, uint64 mask) = 0;
};

struct IAreaTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tArea *const a) = 0;
	virtual int Ensure(const db::tArea *const a, uint64 mask) = 0;
	virtual int Update(const char *aid, const db::tArea *const a, uint64 mask) = 0;
	virtual int Update(const db::tArea *const a, uint64 mask, IAreaFilter *filter) = 0;
	virtual int Cumulate(const char *aid, const db::tArea *const a, uint64 mask) = 0;
	virtual int Cumulate(const db::tArea *const a, uint64 mask, IAreaFilter *filter) = 0;
	virtual int Delete(const char *aid) = 0;
	virtual int Delete(IAreaFilter *filter) = 0;
	virtual int GetData(const char *aid, db::tArea *a, ulong *ver = 0) = 0;
	virtual int Select(IAreaRecordset **recordset, IAreaFilter *filter, IAreaSorter *sorter) = 0;
	virtual void RegisterEventHandler(IAreaEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IAreaEventHandler *handler) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Client
struct IClientFilter
{
	virtual bool bypass(const db::tClient *const c) = 0;
};

struct IClientSorter
{
	virtual bool sort(const db::tClient *const c1, const db::tClient *const c2) = 0;
};

struct IClientRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tClient *c, ulong *ver = 0) const = 0;
	virtual db::tClient *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IClientRecordset> ClientRecordset_Ptr;

struct IClientEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *name, uint64 mask) = 0;
};

struct IClientTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tClient *const c) = 0;
	virtual int Ensure(const db::tClient *const c, uint64 mask) = 0;
	virtual int Update(const char *name, const db::tClient *const c, uint64 mask) = 0;
	virtual int Update(const db::tClient *const c, uint64 mask, IClientFilter *filter) = 0;
	virtual int Cumulate(const char *name, const db::tClient *const c, uint64 mask) = 0;
	virtual int Cumulate(const db::tClient *const c, uint64 mask, IClientFilter *filter) = 0;
	virtual int Delete(const char *name) = 0;
	virtual int Delete(IClientFilter *filter) = 0;
	virtual int GetData(const char *name, db::tClient *c, ulong *ver = 0) = 0;
	virtual int Select(IClientRecordset **recordset, IClientFilter *filter, IClientSorter *sorter) = 0;
	virtual void RegisterEventHandler(IClientEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IClientEventHandler *handler) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Favorite
struct IFavoriteFilter
{
	virtual bool bypass(const db::tFavorite *const a) = 0;
};

struct IFavoriteSorter
{
	virtual bool sort(const db::tFavorite *const f1, const db::tFavorite *const f2) = 0;
};

struct IFavoriteRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tFavorite *f, ulong *ver = 0) const = 0;
	virtual db::tFavorite *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IFavoriteRecordset> FavoriteRecordset_Ptr;

struct IFavoriteEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *uid, uint64 mask) = 0;
};

struct IFavoriteTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tFavorite *const f) = 0;
	virtual int Ensure(const db::tFavorite *const f, uint64 mask) = 0;
	virtual int Update(const char *uid, const db::tFavorite *const f, uint64 mask) = 0;
	virtual int Update(const db::tFavorite *const f, uint64 mask, IFavoriteFilter *filter) = 0;
	virtual int Cumulate(const char *uid, const db::tFavorite *const f, uint64 mask) = 0;
	virtual int Cumulate(const db::tFavorite *const f, uint64 mask, IFavoriteFilter *filter) = 0;
	virtual int Delete(const char *uid) = 0;
	virtual int Delete(IFavoriteFilter *filter) = 0;
	virtual int GetData(const char *uid, db::tFavorite *f, ulong *ver = 0) = 0;
	virtual int Select(IFavoriteRecordset **recordset, IFavoriteFilter *filter, IFavoriteSorter *sorter) = 0;
	virtual void RegisterEventHandler(IFavoriteEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IFavoriteEventHandler *handler) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// VDisk
struct IVDiskFilter
{
	virtual bool bypass(const db::tVDisk *const vd) = 0;
};

struct IVDiskSorter
{
	virtual bool sort(const db::tVDisk *const vd1, const db::tVDisk *const vd2) = 0;
};

struct IVDiskRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tVDisk *vd, ulong *ver = 0) const = 0;
	virtual db::tVDisk *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IVDiskRecordset> VDiskRecordset_Ptr;

struct IVDiskEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *vid, uint64 mask) = 0;
};

struct IVDiskTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tVDisk *const vd) = 0;
	virtual int Ensure(const db::tVDisk *const vd, uint64 mask) = 0;
	virtual int Update(const char *vid, const db::tVDisk *const vd, uint64 mask) = 0;
	virtual int Update(const db::tVDisk *const vd, uint64 mask, IVDiskFilter *filter) = 0;
	virtual int Cumulate(const char *vid, const db::tVDisk *const vd, uint64 mask) = 0;
	virtual int Cumulate(const db::tVDisk *const vd, uint64 mask, IVDiskFilter *filter) = 0;
	virtual int Delete(const char *vid) = 0;
	virtual int Delete(IVDiskFilter *filter) = 0;
	virtual int GetData(const char *vid, db::tVDisk *vd, ulong *ver = 0) = 0;
	virtual int Select(IVDiskRecordset **recordset, IVDiskFilter *filter, IVDiskSorter *sorter) = 0;
	virtual void RegisterEventHandler(IVDiskEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IVDiskEventHandler *handler) = 0;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// CmpStartTask
struct ICmpStartTaskFilter
{
	virtual bool bypass(const db::tCmpStartTask *const cst) = 0;
};

struct ICmpStartTaskSorter
{
	virtual bool sort(const db::tCmpStartTask *const cst1, const db::tCmpStartTask *const cst2) = 0;
};

struct ICmpStartTaskRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tCmpStartTask *cst, ulong *ver = 0) const = 0;
	virtual db::tCmpStartTask *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<ICmpStartTaskRecordset> CmpStartTaskRecordset_Ptr;

struct ICmpStartTaskEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *uid, uint64 mask) = 0;
};

struct ICmpStartTaskTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tCmpStartTask *const cst) = 0;
	virtual int Ensure(const db::tCmpStartTask *const cst, uint64 mask) = 0;
	virtual int Update(const char *uid, const db::tCmpStartTask *const cst, uint64 mask) = 0;
	virtual int Update(const db::tCmpStartTask *const cst, uint64 mask, ICmpStartTaskFilter *filter) = 0;
	virtual int Cumulate(const char *uid, const db::tCmpStartTask *const cst, uint64 mask) = 0;
	virtual int Cumulate(const db::tCmpStartTask *const cst, uint64 mask, ICmpStartTaskFilter *filter) = 0;
	virtual int Delete(const char *uid) = 0;
	virtual int Delete(ICmpStartTaskFilter *filter) = 0;
	virtual int GetData(const char *uid, db::tCmpStartTask *cst, ulong *ver = 0) = 0;
	virtual int Select(ICmpStartTaskRecordset **recordset, ICmpStartTaskFilter *filter, ICmpStartTaskSorter *sorter) = 0;
	virtual void RegisterEventHandler(ICmpStartTaskEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(ICmpStartTaskEventHandler *handler) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// User
struct IUserFilter
{
	virtual bool bypass(const db::tUser *const u) = 0;
};

struct IUserSorter
{
	virtual bool sort(const db::tUser *const u1, const db::tUser *const u2) = 0;
};

struct IUserRecordset
{
	virtual void Release(void) = 0;
	virtual uint32 GetCount(void) const = 0;
	virtual bool GetData(uint32 i, db::tUser *u, ulong *ver = 0) const = 0;
	virtual db::tUser *GetData(uint32 i, ulong *ver = 0) const = 0;
};
typedef std::tr1::shared_ptr<IUserRecordset> UserRecordset_Ptr;

struct IUserEventHandler
{
	virtual uint32 GetEvents(void) = 0;
	virtual void HandleEvent(uint32 optype, const char *name, uint64 mask) = 0;
};

struct IUserTable
{
	virtual ulong GetVersion(void) = 0;
	virtual int Insert(const db::tUser *const u) = 0;
	virtual int Ensure(const db::tUser *const u, uint64 mask) = 0;
	virtual int Update(const char *name, const db::tUser *const u, uint64 mask) = 0;
	virtual int Update(const db::tUser *const cst, uint64 mask, IUserFilter *filter) = 0;
	virtual int Cumulate(const char *name, const db::tUser *const u, uint64 mask) = 0;
	virtual int Cumulate(const db::tUser *const cst, uint64 mask, IUserFilter *filter) = 0;
	virtual int Delete(const char *name) = 0;
	virtual int Delete(IUserFilter *filter) = 0;
	virtual int GetData(const char *name, db::tUser *u, ulong *ver = 0) = 0;
	virtual int Select(IUserRecordset **recordset, IUserFilter *filter, IUserSorter *sorter) = 0;
	virtual void RegisterEventHandler(IUserEventHandler *handler) = 0;
	virtual void UnregisterEventHandler(IUserEventHandler *handler) = 0;
};

*/
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
	virtual int GetData(Kty1, Kty2, db::t##Name *, ulong * = 0) = 0; \
	virtual int Select(I##Name##Recordset **, Kty1, I##Name##Sorter *) = 0; \
	virtual int Select(I##Name##Recordset **, I##Name##Filter *, I##Name##Sorter *) = 0; \
	virtual void RegisterEventHandler(I##Name##EventHandler *) = 0; \
	virtual void UnregisterEventHandler(I##Name##EventHandler *) = 0; \
};


DECLARE_TABLE_INTERFACE(uint32, Game)
DECLARE_TABLE_INTERFACE(const char *, Class)
DECLARE_TABLE_INTERFACE_2K(uint32, const char *, GameArea)
DECLARE_TABLE_INTERFACE(const char *, SysOpt)
DECLARE_TABLE_INTERFACE(const char *, Area)
DECLARE_TABLE_INTERFACE(const char *, Client)
DECLARE_TABLE_INTERFACE(const char *, Favorite)
DECLARE_TABLE_INTERFACE(const char *, VDisk)
DECLARE_TABLE_INTERFACE(const char *, CmpStartTask)
DECLARE_TABLE_INTERFACE(const char *, User)


DECLARE_TABLE_INTERFACE(uint32, Icon)
DECLARE_TABLE_INTERFACE(SOCKET, ClientStatus)
DECLARE_TABLE_INTERFACE_2K(const char *, uint32, VDiskStatus)
DECLARE_TABLE_INTERFACE(const char *, SyncTask)
DECLARE_TABLE_INTERFACE_2K(const char *, uint32, SyncGame)
DECLARE_TABLE_INTERFACE_2K(const char *, uint32, ModuleUsage)
DECLARE_TABLE_INTERFACE(const char *, ServerStatus)
DECLARE_TABLE_INTERFACE_2K(const char *, uint32, DiskStatus)

#ifndef RTDATASVR_PLUG_NAME
#	define RTDATASVR_PLUG_NAME			TEXT("RTDataSvr")		
#endif 

#define RTDS_CMD_GET_CLIENTLIST			0x01   //SendMessage cmd
#define RTDS_CMD_GET_UPDATESYSOPT		0x02   //SendMessage cmd
#define RTDS_CMD_GET_UPDATECLIENTVER	0x03   //SendMessage cmd

#define RTDS_CMD_GET_INTERFACE			0x10	//SendMessage cmd

#define RTDS_INTERFACE_RTDATASVR	0		//IRTDataSvr
// 插件RTDataSvr之SendMessage返回值为IRTDataSvr *
// IRTDataSvr *pRTDataSvr = reinterpret_cast<IRTDataSvr *>(
//	   pRTDataSvrPlug->SendMessage(RTDS_CMD_GET_INTERFACE,
//	   RTDS_INTERFACE_RTDATASVR, 0));


#define IMPORT_TABLE_INTERFACE(tname) \
	virtual I##tname##Table *Get##tname##Table(void) = 0;

////////////////////////////////////////////////////////////////////////////////////////
// IRTDataSvr
struct IRTDataSvr
{
	virtual IGameTable *GetGameTable(void) = 0;
	virtual IClassTable *GetClassTable(void) = 0;
	virtual IGameAreaTable *GetGameAreaTable(void) = 0;
	virtual ISysOptTable *GetSysOptTable(void) = 0;
	virtual IAreaTable *GetAreaTable(void) = 0;
	virtual IClientTable *GetClientTable(void) = 0;
	virtual IFavoriteTable *GetFavoriteTable(void) = 0;
	virtual IVDiskTable *GetVDiskTable(void) = 0;
	virtual ICmpStartTaskTable *GetCmpStartTaskTable(void) = 0;
	virtual IUserTable *GetUserTable(void) = 0;
	virtual IClientStatusTable *GetClientStatusTable(void) = 0;
	virtual IIconTable *GetIconTable(void) = 0;
	virtual IVDiskStatusTable *GetVDiskStatusTable(void) = 0;
	virtual ISyncTaskTable *GetSyncTaskTable(void) = 0;
	virtual ISyncGameTable *GetSyncGameTable(void) = 0;
	virtual IModuleUsageTable *GetModuleUsageTable(void) = 0;
	virtual IServerStatusTable *GetServerStatusTable(void) = 0;
	virtual IDiskStatusTable *GetDiskStatusTable(void) = 0;
};

}

#endif //IRTDATASVR_H