#ifndef DATACACHE_H
#define DATACACHE_H

#include "i8mp.h"
#include "../../../include/frame.h"
#include "../../../include/irtdatasvr.h"
#include "LogHelper.h"
#include "../../../include/Utility/utility.h"

#include "container.h"


#include <string>
#include <map>

namespace i8desk {


class ZGameCache
{
public:
	struct RunType {
		int type;
		stdex::tString vid;
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
		const std::map<stdex::tString, RunType>& runtypes,
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
	ulong GetRunTypeVersion(const stdex::tString& AID) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		std::map<stdex::tString, RunType>::const_iterator it = m_runtypes.find(AID);
		return it != m_runtypes.end() ? it->second.ver : 0;
	}
	ulong GetRunTypeType(const stdex::tString& AID) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		std::map<stdex::tString, RunType>::const_iterator it = m_runtypes.find(AID);
		return it != m_runtypes.end() ? it->second.type : GAME_RT_NONE;
	}
	bool GetGame(i8desk::db::tGame& g, db::tClass& c, std::map<stdex::tString, RunType>& runtypes) const 
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
	bool GetRunType(const stdex::tString& AID, RunType& rt) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			std::map<stdex::tString, RunType>::const_iterator it = m_runtypes.find(AID);
			if (it != m_runtypes.end()) {
				rt = it->second;
				return true;
			}
		}
		return false;
	}
	void SetGame(const db::tGame& g, const db::tClass& c, const std::map<stdex::tString, RunType>& runtypes)  
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
		Log(LM_DEBUG, _T("加载游戏数据到缓冲区,GID=%u\n"), m_gid);

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetGameTable()->GetData(m_gid, &m_game, &m_dwVersion) != 0) 
		{
			Log(LM_ERROR, 	_T("查询游戏数据失败[GID=%u]!\n"), m_gid);
			return false;
		}

		//类别名
		if (pRTDataSvr->GetClassTable()->GetData(m_game.CID, &m_class, 0) != 0) 
		{
			Log(LM_ERROR, _T("查询游戏类别定义失败[CID=%s]!\n"), m_game.CID);
			return false;
		}

		m_dwVersion = BuildVersion();

		return m_bIsValid = m_game.Status == 1; //本地游戏
	}

	bool LoadRunType(IRTDataSvr *pRTDataSvr)
	{
		Log(LM_DEBUG, _T("加载游戏运行方式到缓冲区,GID=%u\n"), m_gid);

		IRunTypeRecordset *pRecordset = 0;
		pRTDataSvr->GetRunTypeTable()->Select(&pRecordset, m_gid, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tRunType ga;
			ulong ver;
			if (pRecordset->GetData(i, &ga, &ver)) {
				RunType rt = { ga.Type, ga.VID, ver };
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
		utility::crc32 crc;
		crc << m_game.Name		<< m_game.PID 
			<< m_game.CID		<< m_class.Name
			<< m_game.Exe		<< m_game.Param
			<< m_game.DeskLink	<< m_game.Toolbar
			<< m_game.SvrPath	<< m_game.CliPath 
			<< m_game.EnDel		
			<< m_game.SaveFilter; 
		return crc.get();
	}

private:
	ulong m_dwVersion;
	uint32 m_gid;
	bool m_bIsValid;
	i8desk::db::tGame m_game;
	db::tClass m_class;
	std::map<stdex::tString, RunType> m_runtypes;

	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZGameCache> GameCache_Ptr;
typedef ZAssociatedContainer<long, GameCache_Ptr,
	std::map<long, GameCache_Ptr>, ZSync> GameCacheList;


class ZVDiskCache
{
public:
	ZVDiskCache(const stdex::tString& vid) 
		: m_vid(vid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZVDiskCache(const i8desk::db::tVDisk& vd)
	{
		SetVDisk(vd);
	}
	stdex::tString GetUid(void) const 
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
		Log(LM_DEBUG, _T("加载虚拟盘数据到缓冲区,VID=%s\n"), m_vid.c_str());

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetVDiskTable()->GetData(
			m_vid.c_str(), &m_vdisk, &m_dwVersion) != 0) 
		{
			Log(LM_ERROR, _T("查询虚拟盘数据失败[VID=%s]!\n"), m_vid.c_str());
			return false;
		}
			
		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	stdex::tString m_vid;
	bool m_bIsValid;
	i8desk::db::tVDisk m_vdisk;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZVDiskCache> VDiskCache_Ptr;
typedef ZAssociatedContainer<stdex::tString, VDiskCache_Ptr,
	std::map<stdex::tString, VDiskCache_Ptr>, ZSync> VDiskCacheList;

class ZFavoriteCache
{
public:
	ZFavoriteCache(const stdex::tString& uid) 
		: m_uid(uid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZFavoriteCache(const i8desk::db::tFavorite& f)
	{
		SetFavorite(f);
	}
	stdex::tString GetUid(void) const 
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
		m_uid = m_favorite.FID;
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		Log(LM_DEBUG, _T("加载收藏夹数据到缓冲区,FID=%s\n"), m_uid.c_str());

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetFavoriteTable()->GetData(
			m_uid.c_str(), &m_favorite, &m_dwVersion) != 0) 
		{
			Log(LM_ERROR, _T("查询收藏夹数据失败[FID=%s]!\n"), m_uid.c_str());
			return false;
		}

		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	stdex::tString m_uid;
	bool m_bIsValid;
	i8desk::db::tFavorite m_favorite;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZFavoriteCache> FavoriteCache_Ptr;
typedef ZAssociatedContainer<stdex::tString, FavoriteCache_Ptr,
	std::map<stdex::tString, FavoriteCache_Ptr>, ZSync> FavoriteCacheList;

class ZCmpBootTaskCache
{
public:
	ZCmpBootTaskCache(const stdex::tString& uid) 
		: m_uid(uid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZCmpBootTaskCache(const i8desk::db::tCmpBootTask& t)
	{
		SetCmpBootTask(t);
	}
	stdex::tString GetUid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_uid; 
	}
	int GetType(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpBootTask.Type;
	}	
	int GetFlag(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpBootTask.Flag;
	}	
	ulong GetEndDate(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_CmpBootTask.EndDate;
	}		
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	bool GetCmpBootTask(i8desk::db::tCmpBootTask& t)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			t = m_CmpBootTask;
		}
		return m_bIsValid;
	}
	void SetCmpBootTask(const i8desk::db::tCmpBootTask& t)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_CmpBootTask = t;
		m_uid = m_CmpBootTask.TID;
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		Log(LM_DEBUG, _T("加载开机任务数据到缓冲区,TID=%s\n"), m_uid.c_str());

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetCmpBootTaskTable()->GetData(
			m_uid.c_str(), &m_CmpBootTask, &m_dwVersion) != 0) 
		{
			Log(LM_ERROR,_T("查询开机任务数据失败[TID=%s]!\n"), m_uid.c_str());
	        return false;
		}

		return m_bIsValid = true;
	}


private:
	ulong m_dwVersion;
	stdex::tString m_uid;
	bool m_bIsValid;
	i8desk::db::tCmpBootTask m_CmpBootTask;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZCmpBootTaskCache> CmpBootTaskCache_Ptr;
typedef ZAssociatedContainer<stdex::tString, CmpBootTaskCache_Ptr,
	std::map<stdex::tString, CmpBootTaskCache_Ptr>, ZSync> CmpBootTaskCacheList;

typedef ZAssociatedContainer<stdex::tString, stdex::tString,
	std::map<stdex::tString, stdex::tString>, ZSync> SysoptCacheList;

class ZPlugToolCache
{
public:
	ZPlugToolCache(const ulong pid) 
		: m_pid(pid)
		, m_bIsValid(false) 
		, m_dwVersion(0)
	{
	}
	ZPlugToolCache(const i8desk::db::tPlugTool& pt,ulong ver)
		: m_pid(pt.PID)
		, m_bIsValid(true) 
		, m_plugtool(pt)
		, m_dwVersion(ver)
	{
		m_dwVersion = BuildVersion();
	}
	ulong GetPid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_pid; 
	}
	bool IsValid(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_bIsValid; 
	}
	ulong GetVersion(void) const 
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		return m_dwVersion; 
	}
	bool GetPlugTool(i8desk::db::tPlugTool& pt)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		if (m_bIsValid) {
			pt = m_plugtool;
		}
		return m_bIsValid;
	}
	void SetPlugTool(const i8desk::db::tPlugTool& pt)
	{
		I8_GUARD(ZGuard, ZSync, m_lock);
		m_plugtool = pt;
		m_pid = m_plugtool.PID;
		m_dwVersion = BuildVersion();
		m_bIsValid = true;
	}

	bool LoadData(IRTDataSvr *pRTDataSvr)
	{
		Log(LM_DEBUG, _T("加载插件工具数据到缓冲区,PID=%d\n"), m_pid);

		I8_GUARD(ZGuard, ZSync, m_lock);
		m_bIsValid = false;

		if (pRTDataSvr->GetPlugToolTable()->GetData(
			m_pid, &m_plugtool, &m_dwVersion) != 0) 
		{
			Log(LM_ERROR, _T("查询插件工具数据失败[PID=%d]!\n"), m_pid);
			return false;
		}

		m_dwVersion = BuildVersion();

		return m_bIsValid = true;
	}

protected:

	ulong BuildVersion(void)
	{
		//下列信息纳入客户端游戏信息版本管理中
		//游戏名称, 游戏关联id, 游戏类别，运行EXE，运行参数，是否显示到桌面和工具栏，
		//服务端路径，客户端路径，游戏等级，游戏存档，游戏官网，游戏运行方式，
		utility::crc32 crc;
		crc << m_plugtool.CID		<< m_plugtool.IdcVer 
			<< m_plugtool.SvrVer	<< m_plugtool.Name
			<< m_plugtool.Size		<< m_plugtool.IdcClick
			<< m_plugtool.DownloadType	<< m_plugtool.DownloadStatus
			<< m_plugtool.Status	<< m_plugtool.CliName 
			<< m_plugtool.CliExe	<< m_plugtool.CliParam
			<< m_plugtool.CliPath	<< m_plugtool.CliRunType; 
		return crc.get();
	}

private:
	ulong m_dwVersion;
	ulong m_pid;
	bool m_bIsValid;
	i8desk::db::tPlugTool m_plugtool;
	mutable ZSync m_lock;
};

typedef std::tr1::shared_ptr<ZPlugToolCache> PlugToolCache_Ptr;
typedef ZAssociatedContainer<ulong, PlugToolCache_Ptr,
std::map<ulong, PlugToolCache_Ptr>, ZSync> PlugToolCacheList;

namespace DataCache {

class ZGameEventHandler;
class ZClassEventHandler;
class ZAreaEventHandler;
class ZRunTypeEventHandler;
class ZClientEventHandler;
class ZFavoriteEventHandler;
class ZCmpBootTaskEventHandler;
class ZBootTaskAreaEventHandler;
class ZVDiskEventHandler;
class ZSysOptEventHandler;
class ZPlugToolEventHandler;


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
	GameCacheList& GetGames(void) { return m_games; }
	VDiskCacheList& GetVDisks(void) { return m_vdisks; }
	FavoriteCacheList& GetFavorites(void) { return m_favorites; }
	CmpBootTaskCacheList& GetBootTasks(void) { return m_boottasks; }
	SysoptCacheList& GetSysopts(void) { return m_sysopts; }
	PlugToolCacheList& GetPlugTools(void) { return m_plugtools; }

//EventHandler
	void AddGameEvent(uint32 optype, uint32 gid, uint64 mask);
	void AddClassEvent(uint32 optype, const TCHAR *DefClass, uint64 mask);
	void AddAreaEvent(uint32 optype, const TCHAR *aid, uint64 mask);
	void AddRunTypeEvent(uint32 optype, uint32 gid, const TCHAR *aid, uint64 mask);
	void AddClientEvent(uint32 optype, const TCHAR *name, uint64 mask);
	void AddFavoriteEvent(uint32 optype, const TCHAR *uid, uint64 mask);
	void AddCmpBootTaskEvent(uint32 optype, const TCHAR *uid, uint64 mask);
	void AddBootTaskAreaEvent(uint32 optype, const TCHAR *uid, const TCHAR *aid,uint64 mask);
	void AddVDiskEvent(uint32 optype, const TCHAR *vid, uint64 mask);
	void AddSysOptEvent(uint32 optype, const TCHAR *key, uint64 mask);
	void AddPlugToolEvent(uint32 optype,  ulong pid, uint64 mask);

protected:

	bool LoadGames(void);
	bool LoadVDisks(void);
	bool LoadFavorites(void);
	bool LoadBootTasks(void);
	bool LoadSysopts(void);
	bool LoadPlugTools(void);

private:
	
	IRTDataSvr *m_pRTDataSvr;

	GameCacheList m_games;
	VDiskCacheList m_vdisks;
	FavoriteCacheList m_favorites;
	CmpBootTaskCacheList m_boottasks;
	SysoptCacheList m_sysopts;
	PlugToolCacheList m_plugtools;

	DataCache::ZGameEventHandler *m_pGameEventHandler;
	DataCache::ZClassEventHandler *m_pClassEventHandler;
	DataCache::ZAreaEventHandler *m_pAreaEventHandler;
    DataCache::ZRunTypeEventHandler *m_pRunTypeEventHandler;
    DataCache::ZClientEventHandler *m_pClientEventHandler;
    DataCache::ZFavoriteEventHandler *m_pFavoriteEventHandler;
    DataCache::ZCmpBootTaskEventHandler *m_pCmpBootTaskEventHandler;
	DataCache::ZBootTaskAreaEventHandler *m_pBootTaskAreaEventHandler;
    DataCache::ZVDiskEventHandler *m_pVDiskEventHandler;
    DataCache::ZSysOptEventHandler *m_pSysOptEventHandler;
	DataCache::ZPlugToolEventHandler *m_pPlugToolEventHandler;

};


} //namespace i8desk

#endif //DATACACHE_H
