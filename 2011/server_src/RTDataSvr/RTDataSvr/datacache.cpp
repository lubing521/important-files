#include "StdAfx.h"
#include "datacache.h"
#include "LogHelper.h"

namespace i8desk {


	namespace DataCache 
	{
		class ZClassEventHandler : public IClassEventHandler 
		{
		public:
			ZClassEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IClassEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *CID, uint64 mask)
			{
				m_pDataCache->AddClassEvent(optype, CID, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZGameEventHandler : public IGameEventHandler 
		{
		public:
			ZGameEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IGameEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, uint32 gid, uint64 mask)
			{
				m_pDataCache->AddGameEvent(optype, gid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};


		class ZAreaEventHandler : public IAreaEventHandler 
		{
		public:
			ZAreaEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IAreaEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *aid, uint64 mask)
			{
				m_pDataCache->AddAreaEvent(optype, aid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZRunTypeEventHandler : public IRunTypeEventHandler 
		{
		public:
			ZRunTypeEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IRunTypeEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, uint32 gid, const TCHAR *aid, uint64 mask)
			{
				m_pDataCache->AddRunTypeEvent(optype, gid, aid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZClientEventHandler : public IClientEventHandler 
		{
		public:
			ZClientEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IClientEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *name, uint64 mask)
			{

			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};



		class ZFavoriteEventHandler : public IFavoriteEventHandler 
		{
		public:
			ZFavoriteEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IFavoriteEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *uid, uint64 mask)
			{
				m_pDataCache->AddFavoriteEvent(optype, uid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZSysOptEventHandler : public ISysOptEventHandler 
		{
		public:
			ZSysOptEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//ISysOptEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *key, uint64 mask)
			{
				m_pDataCache->AddSysOptEvent(optype, key, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZVDiskEventHandler : public IVDiskEventHandler 
		{
		public:
			ZVDiskEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IVDiskEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *vid, uint64 mask)
			{
				m_pDataCache->AddVDiskEvent(optype, vid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZCmpBootTaskEventHandler : public ICmpBootTaskEventHandler 
		{
		public:
			ZCmpBootTaskEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//ICmpBootTaskkEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *uid, uint64 mask)
			{
				m_pDataCache->AddCmpBootTaskEvent(optype, uid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZBootTaskAreaEventHandler : public IBootTaskAreaEventHandler 
		{
		public:
			ZBootTaskAreaEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IUserEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype, const TCHAR *uid,const TCHAR *aid, uint64 mask)
			{
				m_pDataCache->AddBootTaskAreaEvent(optype, uid, aid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};

		class ZPlugToolEventHandler : public IPlugToolEventHandler
		{
		public:
			ZPlugToolEventHandler(ZDataCache *pDataCache, uint32 optypes)
				: m_pDataCache(pDataCache), m_optypes(optypes)
			{
			}
			//IPlugToolEventHandler
			uint32 GetEvents(void) 
			{ 
				return m_optypes; 
			}
			void HandleEvent(uint32 optype,  uint32 pid, uint64 mask)
			{
				m_pDataCache->AddPlugToolEvent(optype, pid, mask);
			}
		private:
			ZDataCache *m_pDataCache;
			uint32 m_optypes;
		};


	}
	//namespace DataCache

	ZDataCache::ZDataCache(IRTDataSvr *pRTDataSvr)
		: m_pRTDataSvr(pRTDataSvr)
	{
	}

	ZDataCache::~ZDataCache(void)
	{
	}

	bool ZDataCache::LoadData(void)
	{
		return  LoadGames()
			&& LoadVDisks()
			&& LoadFavorites()
			&& LoadBootTasks()
			&& LoadSysopts()
			&& LoadPlugTools();
	}

	//thread init fini
	int ZDataCache::RegisterHandler(void)
	{
		m_pClassEventHandler = new DataCache::ZClassEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetClassTable()->RegisterEventHandler(m_pClassEventHandler);

		m_pGameEventHandler = new DataCache::ZGameEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetGameTable()->RegisterEventHandler(m_pGameEventHandler);

		m_pAreaEventHandler = new DataCache::ZAreaEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetAreaTable()->RegisterEventHandler(m_pAreaEventHandler);

		m_pRunTypeEventHandler = new DataCache::ZRunTypeEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetRunTypeTable()->RegisterEventHandler(m_pRunTypeEventHandler);

		m_pClientEventHandler = new DataCache::ZClientEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetClientTable()->RegisterEventHandler(m_pClientEventHandler);

		m_pFavoriteEventHandler = new DataCache::ZFavoriteEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetFavoriteTable()->RegisterEventHandler(m_pFavoriteEventHandler);

		m_pCmpBootTaskEventHandler = new DataCache::ZCmpBootTaskEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetCmpBootTaskTable()->RegisterEventHandler(m_pCmpBootTaskEventHandler);

		m_pBootTaskAreaEventHandler = new DataCache::ZBootTaskAreaEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetBootTaskAreaTable()->RegisterEventHandler(m_pBootTaskAreaEventHandler);

		m_pVDiskEventHandler = new DataCache::ZVDiskEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetVDiskTable()->RegisterEventHandler(m_pVDiskEventHandler);

		m_pSysOptEventHandler = new DataCache::ZSysOptEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetSysOptTable()->RegisterEventHandler(m_pSysOptEventHandler);

		m_pPlugToolEventHandler = new DataCache::ZPlugToolEventHandler(this, ~SQL_OP_SELECT);
		m_pRTDataSvr->GetPlugToolTable()->RegisterEventHandler(m_pPlugToolEventHandler);
		
		return 0;
	}

	void ZDataCache::UnregisterHandler(void)
	{
		m_pRTDataSvr->GetGameTable()->UnregisterEventHandler(m_pGameEventHandler);
		delete m_pGameEventHandler;

		m_pRTDataSvr->GetClassTable()->UnregisterEventHandler(m_pClassEventHandler);
		delete m_pClassEventHandler;

		m_pRTDataSvr->GetAreaTable()->UnregisterEventHandler(m_pAreaEventHandler);
		delete m_pAreaEventHandler;

		m_pRTDataSvr->GetRunTypeTable()->UnregisterEventHandler(m_pRunTypeEventHandler);
		delete m_pRunTypeEventHandler;

		m_pRTDataSvr->GetClientTable()->UnregisterEventHandler(m_pClientEventHandler);
		delete m_pClientEventHandler;

		m_pRTDataSvr->GetFavoriteTable()->UnregisterEventHandler(m_pFavoriteEventHandler);
		delete m_pFavoriteEventHandler;

		m_pRTDataSvr->GetCmpBootTaskTable()->UnregisterEventHandler(m_pCmpBootTaskEventHandler);
		delete m_pCmpBootTaskEventHandler;

		m_pRTDataSvr->GetBootTaskAreaTable()->UnregisterEventHandler(m_pBootTaskAreaEventHandler);
		delete m_pBootTaskAreaEventHandler;

		m_pRTDataSvr->GetVDiskTable()->UnregisterEventHandler(m_pVDiskEventHandler);
		delete m_pVDiskEventHandler;

		m_pRTDataSvr->GetSysOptTable()->UnregisterEventHandler(m_pSysOptEventHandler);
		delete m_pSysOptEventHandler;

		m_pRTDataSvr->GetPlugToolTable()->UnregisterEventHandler(m_pPlugToolEventHandler);
		delete m_pPlugToolEventHandler;
	}	


	bool ZDataCache::LoadGames(void)
	{
		Log(LM_DEBUG, _T("缓冲游戏数据...\n"));

		IGameRecordset *pRecordset = 0;
		m_pRTDataSvr->GetGameTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			ulong GameVer;
			db::tGame *d = pRecordset->GetData(i, &GameVer);
			if (d->Status != 1)
				continue;

			db::tClass c; //类别名
			if (m_pRTDataSvr->GetClassTable()->GetData(d->CID, &c) != 0)
				continue;

			//运行方式
			IRunTypeRecordset *pGARecordset = 0; 
			m_pRTDataSvr->GetRunTypeTable()->Select(&pGARecordset, d->GID, 0);

			std::map<stdex::tString, ZGameCache::RunType> rts;
			for (uint32 j = 0; j < pGARecordset->GetCount(); j++) {
				ulong GAVer;
				db::tRunType *ga = pGARecordset->GetData(j, &GAVer);
				ZGameCache::RunType rt = { ga->Type, ga->VID, GAVer };
				rts[ga->AID] = rt;
			}
			pGARecordset->Release();

			m_games.put(d->GID, GameCache_Ptr(new ZGameCache(*d, c, rts, GameVer)));
		}

		pRecordset->Release();

		return true;
	}

	bool ZDataCache::LoadVDisks(void)
	{
		Log(LM_DEBUG, _T("缓冲虚拟盘数据...\n"));

		IVDiskRecordset *pRecordset = 0;
		m_pRTDataSvr->GetVDiskTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tVDisk *d = pRecordset->GetData(i);
			m_vdisks.put(d->VID, VDiskCache_Ptr(new ZVDiskCache(*d)));
		}

		pRecordset->Release();

		return true;
	}

	bool ZDataCache::LoadFavorites(void)
	{
		Log(LM_DEBUG, _T("缓冲收藏夹数据...\n"));

		IFavoriteRecordset *pRecordset = 0;
		m_pRTDataSvr->GetFavoriteTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tFavorite *d = pRecordset->GetData(i);
			m_favorites.put(d->FID, FavoriteCache_Ptr(new ZFavoriteCache(*d)));
		}

		pRecordset->Release();

		return true;
	}

	bool ZDataCache::LoadBootTasks(void)
	{
		Log(LM_DEBUG, _T("缓冲开机任务数据...\n"));

		ICmpBootTaskRecordset *pRecordset = 0;
		m_pRTDataSvr->GetCmpBootTaskTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tCmpBootTask *d = pRecordset->GetData(i);
			m_boottasks.put(d->TID, CmpBootTaskCache_Ptr(new ZCmpBootTaskCache(*d)));
		}

		pRecordset->Release();

		return true;
	}


	bool ZDataCache::LoadSysopts(void)
	{
		Log(LM_DEBUG, _T("缓冲系统选项数据...\n"));

		ISysOptRecordset *pRecordset = 0;
		m_pRTDataSvr->GetSysOptTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			db::tSysOpt *d = pRecordset->GetData(i);
			m_sysopts.put(d->key, d->value);
		}

		pRecordset->Release();

		return true;
	}

	bool ZDataCache::LoadPlugTools(void)
	{
		Log(LM_DEBUG, _T("缓冲插件工具数据...\n"));

		IPlugToolRecordset *pRecordset = 0;
		m_pRTDataSvr->GetPlugToolTable()->Select(&pRecordset, 0, 0);

		for (uint32 i = 0; i < pRecordset->GetCount(); i++) {
			ulong ver = 0;
			db::tPlugTool *d = pRecordset->GetData(i,&ver);
			m_plugtools.put(d->PID, PlugToolCache_Ptr(new ZPlugToolCache(*d,ver)));
		}

		pRecordset->Release();

		return true;
	}


	//insert event 

	void ZDataCache::AddClassEvent(uint32 optype, const TCHAR *CID, uint64 mask)
	{
	}

	void ZDataCache::AddGameEvent(uint32 optype, uint32 gid, uint64 mask)
	{
		if (optype == SQL_OP_DELETE) 
		{
			m_games.remove(gid);
		}
		else 
		{
			GameCache_Ptr d;
			if (m_games.peek(gid, d)) 
			{
				d->LoadGame(m_pRTDataSvr); //更新
			}
			else {
				db::tGame g; //添加
				if (m_pRTDataSvr->GetGameTable()->GetData(gid, &g) == 0
					&& g.Status == 1) 
				{
					GameCache_Ptr p = GameCache_Ptr(new ZGameCache(gid));
					if (p->LoadData(m_pRTDataSvr))
						m_games.put(gid, p);
				}
			}
		}
	}

	void ZDataCache::AddAreaEvent(uint32 optype, const TCHAR *aid, uint64 mask)
	{
	}

	void ZDataCache::AddRunTypeEvent(uint32 optype, uint32 gid, const TCHAR *aid, uint64 mask)
	{
		GameCache_Ptr d;
		if (m_games.peek(gid, d)) 
			d->LoadRunType(m_pRTDataSvr); 
	}

	void ZDataCache::AddClientEvent(uint32 optype, const TCHAR *name, uint64 mask)
	{

	}

	void ZDataCache::AddFavoriteEvent(uint32 optype, const TCHAR *uid, uint64 mask)
	{
		if (optype == SQL_OP_DELETE) 
		{
			m_favorites.remove(uid);
		}
		else 
		{
			FavoriteCache_Ptr d;
			if (m_favorites.peek(uid, d))
			{
				d->LoadData(m_pRTDataSvr); //更新
			}
			else
			{
				db::tFavorite f; //添加
				if (m_pRTDataSvr->GetFavoriteTable()->GetData(uid, &f) == 0) 
				{
					m_favorites.put(uid, FavoriteCache_Ptr(new ZFavoriteCache(f)));
				}
			}
		}
	}

	void ZDataCache::AddCmpBootTaskEvent(uint32 optype, const TCHAR *uid, uint64 mask)
	{
		if (optype == SQL_OP_DELETE) 
		{
			m_boottasks.remove(uid);
		}
		else 
		{
			CmpBootTaskCache_Ptr d;
			if (m_boottasks.peek(uid, d)) 
			{
				d->LoadData(m_pRTDataSvr); //更新
			}
			else
			{
				db::tCmpBootTask t; //添加
				if (m_pRTDataSvr->GetCmpBootTaskTable()->GetData(uid, &t) == 0) 
				{
					m_boottasks.put(uid, CmpBootTaskCache_Ptr(new ZCmpBootTaskCache(t)));
				}
			}
		}
	}


	void ZDataCache::AddBootTaskAreaEvent(uint32 optype, const TCHAR *uid,const TCHAR *aid, uint64 mask)
	{

	}

	void ZDataCache::AddVDiskEvent(uint32 optype, const TCHAR *vid, uint64 mask)
	{
		if (optype == SQL_OP_DELETE) {
			m_vdisks.remove(vid);
		}
		else {
			VDiskCache_Ptr d;
			if (m_vdisks.peek(vid, d)) {
				d->LoadData(m_pRTDataSvr); //更新
			}
			else {
				db::tVDisk vd; //添加
				if (m_pRTDataSvr->GetVDiskTable()->GetData(vid, &vd) == 0) {
					m_vdisks.put(vid, VDiskCache_Ptr(new ZVDiskCache(vd)));
				}
			}
		}
	}

	void ZDataCache::AddSysOptEvent(uint32 optype, const TCHAR *key, uint64 mask)
	{
		if (optype == SQL_OP_DELETE) {
			m_sysopts.remove(key);
		}
		else {
			db::tSysOpt so;
			if (m_pRTDataSvr->GetSysOptTable()->GetData(key, &so) == 0) {
				m_sysopts.set(so.key, so.value);
			}
		}
	}

	void ZDataCache::AddPlugToolEvent(uint32 optype,  ulong pid, uint64 mask)
	{
		if (optype == SQL_OP_DELETE)
		{
			m_plugtools.remove(pid);
			return;
		}

		PlugToolCache_Ptr d;
		if (m_plugtools.peek(pid, d)) 
			d->LoadData(m_pRTDataSvr); //更新
		else 
		{
			db::tPlugTool pt; //添加
			ulong ver = 0;
			if (m_pRTDataSvr->GetPlugToolTable()->GetData(pid, &pt,&ver) == 0)
				m_plugtools.put(pid, PlugToolCache_Ptr(new ZPlugToolCache(pt,ver)));
		}
	}



	


} //namespace i8desk
