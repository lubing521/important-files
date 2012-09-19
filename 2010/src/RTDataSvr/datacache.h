#ifndef DATACACHE_H
#define DATACACHE_H

#include "../../include/i8mp.h"
#include "../../include/i8logmsg.h"
#include "../../include/irtdatasvr.h"

#include "../../include/Utility/utility.h"

#include "../../include/Extend STL/container.h"


#include <string>
#include <map>

namespace i8desk {


class ZGameCache
{
public:
	struct RunType {
		int type;
		std::string vid;
		ulong ver;
	};
	ZGameCache(long gid) 
		: m_gid(gid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZGameCache(const db::tGame& g,
		const db::tClass& c,
		const std::map<std::string, RunType>& runtypes,
		ulong ver) 
		: m_gid(g.GID)
		, m_game(g)
		, m_class(c)
		, m_runtypes(runtypes)
		, m_bIsValid(true) 
		, m_dwVersion(ver)
	{
		m_dwVersion = BuildVersion();
	}
	bool IsValid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_bIsValid; 
	}
	uint32 GetGid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_gid; 
	}
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	ulong GetGameAreaVersion(const std::string& AID) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		std::map<std::string, RunType>::const_iterator it = m_runtypes.find(AID);
		return it != m_runtypes.end() ? it->second.ver : 0;
	}
	ulong GetGameAreaRunType(const std::string& AID) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		std::map<std::string, RunType>::const_iterator it = m_runtypes.find(AID);
		return it != m_runtypes.end() ? it->second.type : ERT_NONE;
	}
	bool GetGame(i8desk::db::tGame& g, db::tClass& c, std::map<std::string, RunType>& runtypes) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			g = m_game;
			c = m_class;
			runtypes = m_runtypes;
		}
		return m_bIsValid;
	}
	bool GetGame(i8desk::db::tGame& g, db::tClass& c) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			g = m_game;
			c = m_class;
		}
		return m_bIsValid;
	}	
	bool GetGameArea(const std::string& AID, RunType& rt) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			std::map<std::string, RunType>::const_iterator it = m_runtypes.find(AID);
			if (it != m_runtypes.end()) {
				rt = it->second;
				return true;
			}
		}
		return false;
	}
	void SetGame(const db::tGame& g, const db::tClass& c, const std::map<std::string, RunType>& runtypes)  
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_game = g;
		m_class = c;
		m_runtypes = runtypes;
		m_dwVersion = BuildVersion();
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		return LoadGame(pRTDataSvr)
			&& LoadRunType(pRTDataSvr);
	}

	bool LoadGame(IRTDataSvr *pRTDataSvr)
	{
		I8_DEBUG3((LM_DEBUG, I8_TEXT("加载游戏数据到缓冲区,GID=%u\n"), m_gid));

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetGameTable()->GetData(m_gid, &m_game, &m_dwVersion) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询游戏数据失败[GID=%u]!\n"), m_gid), false);

		//类别名
		if (pRTDataSvr->GetClassTable()->GetData(m_game.DefClass, &m_class, 0) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询游戏类别定义失败[DefClass=%s]!\n"), m_game.DefClass), false);

		m_dwVersion = BuildVersion();

		return m_bIsValid = m_game.Status == 1; //本地游戏
	}

	bool LoadRunType(IRTDataSvr *pRTDataSvr)
	{
		I8_DEBUG3((LM_DEBUG, I8_TEXT("加载游戏运行方式到缓冲区,GID=%u\n"), m_gid));

		IGameAreaRecordset *pRecordset = 0;
		pRTDataSvr->GetGameAreaTable()->Select(&pRecordset, m_gid, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::rGameArea ga;
			ulong ver;
			if (pRecordset->GetData(i, &ga, &ver)) {
				RunType rt = { ga.RunType, ga.VID, ver };
				m_runtypes[ga.AID] = rt;
			}
		}
		pRecordset->Release();

		return m_bIsValid; //取决于游戏数据是否有效
	}

	DWORD AddSvrClick(DWORD inc)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_game.SvrClick += inc;
		return m_game.SvrClick;
	}

protected:
	
	ulong BuildVersion(void)
	{
		//下列信息纳入客户端游戏信息版本管理中
		//游戏名称, 游戏关联id, 游戏类别，运行EXE，运行参数，是否显示到桌面和工具栏，
		//服务端路径，客户端路径，游戏等级，游戏存档，游戏官网，游戏运行方式，
		crc32 crc;
		crc << m_game.Name		<< m_game.PID 
			<< m_game.DefClass	<< m_class.Name
			<< m_game.GameExe	<< m_game.Param
			<< m_game.DeskLnk	<< m_game.Toolbar
			<< m_game.SvrPath	<< m_game.CliPath 
			<< m_game.Priority	<< m_game.ShowPriority
			<< m_game.SaveFilter 
			<< m_game.URL;
		return crc.get();
	}

private:
	ulong m_dwVersion;
	uint32 m_gid;
	bool m_bIsValid;
	i8desk::db::tGame m_game;
	db::tClass m_class;
	std::map<std::string, RunType> m_runtypes;

	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZGameCache> GameCache_Ptr;
typedef ZAssociatedContainer<long, GameCache_Ptr,
	std::map<long, GameCache_Ptr>, ZSync> GameCacheList;


class ZVDiskCache
{
public:
	ZVDiskCache(const std::string& vid) 
		: m_vid(vid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZVDiskCache(const i8desk::db::tVDisk& vd)
	{
		SetVDisk(vd);
	}
	std::string GetUid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_vid; 
	}
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	bool GetVDisk(i8desk::db::tVDisk& vd)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			vd = m_vdisk;
		}
		return m_bIsValid;
	}
	void SetVDisk(const i8desk::db::tVDisk& vd)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_vdisk = vd;
		m_vid = m_vdisk.VID;
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		I8_DEBUG3((LM_DEBUG, I8_TEXT("加载虚拟盘数据到缓冲区,VID=%s\n"), m_vid.c_str()));

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetVDiskTable()->GetData(
			m_vid.c_str(), &m_vdisk, &m_dwVersion) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询虚拟盘数据失败[VID=%s]!\n"), m_vid.c_str()), false);

		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	std::string m_vid;
	bool m_bIsValid;
	i8desk::db::tVDisk m_vdisk;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZVDiskCache> VDiskCache_Ptr;
typedef ZAssociatedContainer<std::string, VDiskCache_Ptr,
	std::map<std::string, VDiskCache_Ptr>, ZSync> VDiskCacheList;

class ZFavoriteCache
{
public:
	ZFavoriteCache(const std::string& uid) 
		: m_uid(uid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZFavoriteCache(const i8desk::db::tFavorite& f)
	{
		SetFavorite(f);
	}
	std::string GetUid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_uid; 
	}
	int GetSerial(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_favorite.Serial; 
	}
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	bool GetFavorite(i8desk::db::tFavorite& f)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			f = m_favorite;
		}
		return m_bIsValid;
	}
	void SetFavorite(const i8desk::db::tFavorite& f)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_favorite = f;
		m_uid = m_favorite.UID;
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		I8_DEBUG3((LM_DEBUG, I8_TEXT("加载收藏夹数据到缓冲区,UID=%s\n"), m_uid.c_str()));

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetFavoriteTable()->GetData(
			m_uid.c_str(), &m_favorite, &m_dwVersion) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询收藏夹数据失败[UID=%s]!\n"), m_uid.c_str()), false);

		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	std::string m_uid;
	bool m_bIsValid;
	i8desk::db::tFavorite m_favorite;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZFavoriteCache> FavoriteCache_Ptr;
typedef ZAssociatedContainer<std::string, FavoriteCache_Ptr,
	std::map<std::string, FavoriteCache_Ptr>, ZSync> FavoriteCacheList;

class ZCmpStartTaskCache
{
public:
	ZCmpStartTaskCache(const std::string& uid) 
		: m_uid(uid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZCmpStartTaskCache(const i8desk::db::tCmpStartTask& t)
	{
		SetCmpStartTask(t);
	}
	std::string GetUid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_uid; 
	}
	int GetAreaType(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpStartTask.AreaType;
	}	
	std::string GetAreaParam(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpStartTask.AreaParam;
	}	
	int GetType(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpStartTask.Type;
	}	
	int GetFlag(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpStartTask.Flag;
	}		
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	bool GetCmpStartTask(i8desk::db::tCmpStartTask& t)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			t = m_CmpStartTask;
		}
		return m_bIsValid;
	}
	void SetCmpStartTask(const i8desk::db::tCmpStartTask& t)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_CmpStartTask = t;
		m_uid = m_CmpStartTask.UID;
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		I8_DEBUG3((LM_DEBUG, I8_TEXT("加载开机任务数据到缓冲区,UID=%s\n"), m_uid.c_str()));

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetCmpStartTaskTable()->GetData(
			m_uid.c_str(), &m_CmpStartTask, &m_dwVersion) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询开机任务数据失败[UID=%s]!\n"), m_uid.c_str()), false);

		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	std::string m_uid;
	bool m_bIsValid;
	i8desk::db::tCmpStartTask m_CmpStartTask;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZCmpStartTaskCache> CmpStartTaskCache_Ptr;
typedef ZAssociatedContainer<std::string, CmpStartTaskCache_Ptr,
	std::map<std::string, CmpStartTaskCache_Ptr>, ZSync> CmpStartTaskCacheList;


class ZIconCache
{
public:
	enum { ICONDATA_MAX_SIZE = 5*1024 };
	ZIconCache(long gid) 
		: m_gid(gid)
		, m_dwVersion(0)
		, m_dwDataVersion(0)
		, m_bIsValid(false)
	{
	}
	ZIconCache(const db::tIcon& icon, ulong ver) 
		: m_gid(icon.gid)
		, m_icon(icon)
		, m_dwVersion(ver)
		, m_dwDataVersion(0)
		, m_bIsValid(true)
	{
		m_dwDataVersion = CalBufCRC32(m_icon.data, m_icon.size);
	}
	~ZIconCache()
	{
	}
	ulong GetVersion(void) const
	{ 
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	ulong GetDataVersion(void) const
	{ 
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwDataVersion; 
	}	
	long GetGid(void) const
	{ 
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_gid; 
	}
	DWORD GetSize(void) const 
	{ 
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_bIsValid ? m_icon.size : 0; 
	}
	DWORD GetData(char *pbuf, DWORD _Size) const
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			if (_Size < m_icon.size) {
				I8_ASSERT(0);
				return 0;
			}
			memcpy(pbuf, m_icon.data, m_icon.size);

			return m_icon.size;
		}
		return 0;
	}
	template<size_t _Size>
	DWORD GetData(char (&pbuf)[_Size]) const
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			if (_Size < m_icon.size) {
				I8_ASSERT(0);
				return 0;
			}
			memcpy(pbuf, m_icon.data, m_icon.size);

			return m_icon.size;
		}
		return 0;
	}
	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);

		m_bIsValid = false;
		if (pRTDataSvr->GetIconTable()->GetData(m_gid, &m_icon, &m_dwVersion) != 0) 
			I8_ERROR_RETURN((LM_ERROR, 
				I8_TEXT("查询图标数据失败[GID=%d]!\n"), m_gid), false);

		m_dwDataVersion = CalBufCRC32(m_icon.data, m_icon.size);

		return m_bIsValid = true;
	}
private:
	ulong m_dwVersion;
	long m_gid; 
	ulong m_dwDataVersion;
	db::tIcon m_icon;
	bool m_bIsValid;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZIconCache> IconCache_Ptr;
typedef ZAssociatedContainer<long, IconCache_Ptr,
	std::map<long, IconCache_Ptr>, ZSync> IconCacheList;

typedef ZAssociatedContainer<std::string, std::string,
	std::map<std::string, std::string>, ZSync> SysoptCacheList;

namespace DataCache {

class ZGameEventHandler;
class ZClassEventHandler;
class ZAreaEventHandler;
class ZGameAreaEventHandler;
class ZClientEventHandler;
class ZFavoriteEventHandler;
class ZCmpStartTaskEventHandler;
class ZVDiskEventHandler;
class ZUserEventHandler;
class ZSysOptEventHandler;
class ZIconEventHandler;

} //namespace DataCache

class ZDataCache
{
public:
	ZDataCache(IRTDataSvr *pRTDataSvr);
	~ZDataCache(void);

//Load data
	bool LoadData(void);

//init fini
	int RegisterHandler(void);
	void UnregisterHandler(void);

	IRTDataSvr *GetRTData()
	{
		return m_pRTDataSvr;
	}

//Get cache data
	IconCacheList& GetIcons(void) { return m_icons; }
	GameCacheList& GetGames(void) { return m_games; }
	VDiskCacheList& GetVDisks(void) { return m_vdisks; }
	FavoriteCacheList& GetFavorites(void) { return m_favorites; }
	CmpStartTaskCacheList& GetBootTasks(void) { return m_boottasks; }
	SysoptCacheList& GetSysopts(void) { return m_sysopts; }

//EventHandler
	void AddGameEvent(uint32 optype, uint32 gid, uint64 mask);
	void AddClassEvent(uint32 optype, const char *DefClass, uint64 mask);
	void AddAreaEvent(uint32 optype, const char *aid, uint64 mask);
	void AddGameAreaEvent(uint32 optype, uint32 gid, const char *aid, uint64 mask);
	void AddClientEvent(uint32 optype, const char *name, uint64 mask);
	void AddFavoriteEvent(uint32 optype, const char *uid, uint64 mask);
	void AddCmpStartTaskEvent(uint32 optype, const char *uid, uint64 mask);
	void AddVDiskEvent(uint32 optype, const char *vid, uint64 mask);
	void AddUserEvent(uint32 optype, const char *name, uint64 mask);
	void AddSysOptEvent(uint32 optype, const char *key, uint64 mask);
	void AddIconEvent(uint32 optype, uint32 gid, uint64 mask);

protected:

	bool LoadIcons(void);
	bool LoadGames(void);
	bool LoadVDisks(void);
	bool LoadFavorites(void);
	bool LoadBootTasks(void);
	bool LoadSysopts(void);

private:
	
	IRTDataSvr *m_pRTDataSvr;

	IconCacheList m_icons;
	GameCacheList m_games;
	VDiskCacheList m_vdisks;
	FavoriteCacheList m_favorites;
	CmpStartTaskCacheList m_boottasks;
	SysoptCacheList m_sysopts;

	DataCache::ZGameEventHandler *m_pGameEventHandler;
	DataCache::ZClassEventHandler *m_pClassEventHandler;
	DataCache::ZAreaEventHandler *m_pAreaEventHandler;
    DataCache::ZGameAreaEventHandler *m_pGameAreaEventHandler;
    DataCache::ZClientEventHandler *m_pClientEventHandler;
    DataCache::ZFavoriteEventHandler *m_pFavoriteEventHandler;
    DataCache::ZCmpStartTaskEventHandler *m_pCmpStartTaskEventHandler;
    DataCache::ZVDiskEventHandler *m_pVDiskEventHandler;
    DataCache::ZUserEventHandler *m_pUserEventHandler;
    DataCache::ZSysOptEventHandler *m_pSysOptEventHandler;
    DataCache::ZIconEventHandler *m_pIconEventHandler;
};


} //namespace i8desk

#endif //DATACACHE_H
