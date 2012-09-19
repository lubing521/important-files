#include "StdAfx.h"
#include "dbtrigger.h"
#include "tablemgr.h"
#include "LogHelper.h"
#include "../../../include/tablestruct.h"
#include "../../../include/frame.h"
#include "../../../include/Extend STL/StringAlgorithm.h"

#define KEY_TYPE(X) (X)
#define STDSTRING_TO_CSTR(X) (X).c_str()

	//写数据到文件的间隔时间
#define TRAGGER_DB_INTERVAL		(60*1000*5)  //ms


namespace i8desk 
{

	namespace DBTrigger 
	{ 

		class ZAreaEventHandler : public IAreaEventHandler  
		{ 
		public:
			ZAreaEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
			: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddAreaEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 
		};

		class ZGameEventHandler : public IGameEventHandler  
		{ 
		public:
			ZGameEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype,  uint32  key, uint64 mask) 
			{ 
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddGameEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 

		class ZClassEventHandler : public IClassEventHandler  
		{ 
		public:
			ZClassEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 				
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddClassEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 

		class ZVDiskEventHandler : public IVDiskEventHandler  
		{ 
		public:
			ZVDiskEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddVDiskEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 

		class ZServerEventHandler : public IServerEventHandler  
		{ 
		public:
			ZServerEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddServerEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 

		class ZCmpBootTaskEventHandler : public ICmpBootTaskEventHandler  
		{ 
		public:
			ZCmpBootTaskEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddCmpBootTaskEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 


		class ZSyncTaskEventHandler : public ISyncTaskEventHandler  
		{ 
		public:
			ZSyncTaskEventHandler(ZDBTrigger *pDBTrigger, uint32 optypes) 
				: m_pDBTrigger(pDBTrigger), m_optypes(optypes)
			{ 
			} 
			uint32 GetEvents(void)
			{  
				return m_optypes;  
			} 
			void HandleEvent(uint32 optype, const TCHAR * key, uint64 mask) 
			{ 				
				if ( optype == SQL_OP_DELETE || optype == SQL_OP_UPDATE )
					m_pDBTrigger->AddSyncTaskEvent(optype, key, mask); 
			}
		private: 
			ZDBTrigger *m_pDBTrigger; 
			uint32 m_optypes; 

		}; 
	}


	ZDBTrigger::ZDBTrigger(ZTableMgr *pTableMgr)
		: m_pTableMgr(pTableMgr)
	{
	}

	ZDBTrigger::~ZDBTrigger(void)
	{
		Shutdown();
	}


	void ZDBTrigger::ProcessAreaEvents(uint32 threshold)
	{  
		if (m_AreaEventList.size() < threshold) 
		return; 

		AreaEventStruct s; 
		db::tRunType tblRunType;
		db::tClient tblClient;
		db::tBootTaskArea tblBootTaskArea;
		while (m_AreaEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public IRunTypeFilter {
				Filter(TCHAR* AID) : m_AID(AID) {}
				bool bypass(const db::tRunType *const d) {
					return _tcscmp(d->AID , m_AID) != 0;
				}
			private:
				TCHAR *m_AID;
			} filter(key);

			struct Filter1 : public IClientFilter {
				Filter1(TCHAR* AID) : m_AID(AID) {}
				bool bypass(const db::tClient *const d) {
					return _tcscmp(d->AID , m_AID) != 0;
				}
			private:
				TCHAR *m_AID;
			} filter1(key);

			struct Filter2 : public IBootTaskAreaFilter {
				Filter2(TCHAR* AID) : m_AID(AID) {}
				bool bypass(const db::tBootTaskArea *const d) {
					return _tcscmp(d->AID , m_AID) != 0;
				}
			private:
				TCHAR *m_AID;
			} filter2(key);
			
			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				m_pTableMgr->GetRunTypeTable()->Delete(&filter) ;
				m_pTableMgr->GetBootTaskAreaTable()->Delete(&filter2) ;
				utility::Strcpy(tblClient.AID, DEFAULT_AREA_GUID);
				//Log(LM_INFO,_T("trigger 区域删除改变客户机区域"));
				m_pTableMgr->GetClientTable()->Update(&tblClient,MASK_TCLIENT_AID,&filter1) ;
			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TAREA_AID) == MASK_TAREA_AID)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						utility::Strcpy(tblRunType.AID, key);
						m_pTableMgr->GetRunTypeTable()->Update(&tblRunType,MASK_TRUNTYPE_AID,&filter) ;
						utility::Strcpy(tblClient.AID, key);
						//Log(LM_INFO,_T("trigger 区域插入，修改改变客户机区域"));
						m_pTableMgr->GetClientTable()->Update(&tblClient,MASK_TCLIENT_AID,&filter1) ;
						utility::Strcpy(tblBootTaskArea.AID, key);
						m_pTableMgr->GetBootTaskAreaTable()->Update(&tblBootTaskArea,MASK_TBOOTTASKAREA_AID,&filter2) ;
					}
				}
			}
		}
	} 

	void ZDBTrigger::ProcessGameEvents(uint32 threshold)
	{  
		if (m_GameEventList.size() < threshold) 
			return; 

		GameEventStruct s; 
		db::tRunType tblRunType;
		while (m_GameEventList.get_head(s)) 
		{ 
			bool ok = true; 
			uint32 key = KEY_TYPE(std::tr1::get<1>(s));

			struct Filter : public IRunTypeFilter {
				Filter(uint32 GID) : m_GID(GID) {}
				bool bypass(const db::tRunType *const d) {
					return d->GID != m_GID;
				}
			private:
				uint32 m_GID;
			} filter(key);

			struct Filter1 : public ISyncGameFilter {
				Filter1(uint32 GID) : m_GID(GID) {}
				bool bypass(const db::tSyncGame *const d) {
					return d->GID != m_GID;
				}
			private:
				uint32 m_GID;
			} filter1(key);

			struct Filter2 : public ISyncTaskStatusFilter {
				Filter2(uint32 GID) : m_GID(GID) {}
				bool bypass(const db::tSyncTaskStatus *const d) {
					return d->GID != m_GID;
				}
			private:
				uint32 m_GID;
			} filter2(key);


			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				m_pTableMgr->GetRunTypeTable()->Delete(&filter) ;
				//m_pTableMgr->GetSyncGameTable()->Delete(&filter1) ;
				//m_pTableMgr->GetSyncTaskStatusTable()->Delete(&filter2) ;

			}
			else if(std::tr1::get<0>(s) == SQL_OP_UPDATE || std::tr1::get<0>(s) == SQL_OP_ENSURE)
			{
				if((std::tr1::get<2>(s)&MASK_TGAME_STATUS) == MASK_TGAME_STATUS)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						db::tGame game;
						m_pTableMgr->GetGameTable()->GetData(key, &game) ;

						if(game.Status == i8desk::StatusNone)
						{
							m_pTableMgr->GetRunTypeTable()->Delete(&filter) ;
							//m_pTableMgr->GetSyncGameTable()->Delete(&filter1) ;
							//m_pTableMgr->GetSyncTaskStatusTable()->Delete(&filter2) ;
						}

					}
				}

			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TGAME_GID) == MASK_TGAME_GID)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						tblRunType.GID = key;
						m_pTableMgr->GetRunTypeTable()->Update(&tblRunType,MASK_TRUNTYPE_GID,&filter) ;
					}
				}
			}
		}
	} 

	void ZDBTrigger::ProcessClassEvents(uint32 threshold)
	{  
		if (m_ClassEventList.size() < threshold) 
			return; 

		ClassEventStruct s; 
		db::tGame tblGame;
		while (m_ClassEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public IGameFilter {
				Filter(TCHAR* CID) : m_CID(CID) {}
				bool bypass(const db::tGame *const d) {
					return _tcscmp(d->CID , m_CID) != 0;
				}
			private:
				TCHAR* m_CID;
			} filter(key);

			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				//m_pTableMgr->GetGameTable()->Delete(&filter) ;
			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TCLASS_DEFCLASS) == MASK_TCLASS_DEFCLASS)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						utility::Strcpy(tblGame.CID, key);
						m_pTableMgr->GetGameTable()->Update(&tblGame,MASK_TCLASS_DEFCLASS,&filter) ;
					}
				}
			}
		}
	} 

	void ZDBTrigger::ProcessVDiskEvents(uint32 threshold)
	{  
		if (m_VDiskEventList.size() < threshold) 
			return; 

		VDiskEventStruct s; 
		db::tRunType tblRunType;
		while (m_VDiskEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public IRunTypeFilter {
				Filter(TCHAR* VID) : m_VID(VID) {}
				bool bypass(const db::tRunType *const d) {
					return _tcscmp(d->VID , m_VID) != 0;
				}
			private:
				TCHAR* m_VID;
			} filter(key);

			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				m_pTableMgr->GetRunTypeTable()->Delete(&filter) ;
			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TVDISKCLIENT_VID) == MASK_TVDISKCLIENT_VID)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						utility::Strcpy(tblRunType.VID, key);
						m_pTableMgr->GetRunTypeTable()->Update(&tblRunType,MASK_TRUNTYPE_VID,&filter) ;
					}
				}
			}
		}
	} 

	void ZDBTrigger::ProcessServerEvents(uint32 threshold)
	{  
		if (m_ServerEventList.size() < threshold) 
			return; 

		ServerEventStruct s; 
		db::tArea	  tblArea;

		while (m_ServerEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public IAreaFilter {
				Filter(TCHAR* SvrID) : m_SvrID(SvrID) {}
				bool bypass(const db::tArea *const d) {
					return _tcscmp(d->SvrID , m_SvrID) != 0;
				}
			private:
				TCHAR* m_SvrID;
			} filter(key);

			struct Filter1 : public IServerStatusFilter {
				Filter1(TCHAR* SvrID) : m_SvrID(SvrID) {}
				bool bypass(const db::tServerStatus *const d) {
					return _tcscmp(d->SvrID , m_SvrID) != 0;
				}
			private:
				TCHAR* m_SvrID;
			} filter1(key);

			
			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				m_pTableMgr->GetAreaTable()->Update(&tblArea,MASK_TAREA_SVRID,&filter) ;
				m_pTableMgr->GetServerStatusTable()->Delete(&filter1) ;
			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TSERVER_SVRID) == MASK_TSERVER_SVRID)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						utility::Strcpy(tblArea.SvrID, key);
						m_pTableMgr->GetAreaTable()->Update(&tblArea,MASK_TAREA_SVRID,&filter) ;
					}
				}
			}
		}
	} 

	void ZDBTrigger::ProcessCmpBootTaskEvents(uint32 threshold)
	{  
		if (m_CmpBootTaskEventList.size() < threshold) 
			return; 

		CmpBootTaskEventStruct s; 
		db::tBootTaskArea tblBootTaskArea;
		while (m_CmpBootTaskEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public IBootTaskAreaFilter {
				Filter(TCHAR* TID) : m_TID(TID) {}
				bool bypass(const db::tBootTaskArea *const d) {
					return _tcscmp(d->TID , m_TID) != 0;
				}
			private:
				TCHAR* m_TID;
			} filter(key);

			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
			{
				m_pTableMgr->GetBootTaskAreaTable()->Delete(&filter) ;
			}
			else
			{
				if((std::tr1::get<2>(s)&MASK_TCMPBOOTTASK_TID) == MASK_TCMPBOOTTASK_TID)
				{
					if (std::tr1::get<0>(s) != SQL_OP_SELECT)
					{
						utility::Strcpy(tblBootTaskArea.TID, key);
						m_pTableMgr->GetBootTaskAreaTable()->Update(&tblBootTaskArea,MASK_TBOOTTASKAREA_TID,&filter) ;
					}
				}
			}
		}
	} 


	void ZDBTrigger::ProcessSyncTaskEvents(uint32 threshold)
	{  
		if (m_SyncTaskEventList.size() < threshold) 
			return; 

		SyncTaskEventStruct s; 
		db::tSyncTaskStatus tblSyncTaskStatus;
		while (m_SyncTaskEventList.get_head(s)) 
		{ 
			bool ok = true; 
			TCHAR * key = (TCHAR*)STDSTRING_TO_CSTR(std::tr1::get<1>(s));

			struct Filter : public ISyncTaskStatusFilter {
				Filter(TCHAR* SID) : m_SID(SID) {}
				bool bypass(const db::tSyncTaskStatus *const d) {
					return _tcscmp(d->SID , m_SID) != 0;
				}
			private:
				TCHAR* m_SID;
			} filter(key);

			if(std::tr1::get<0>(s) == SQL_OP_DELETE)
				m_pTableMgr->GetSyncTaskStatusTable()->Delete(&filter) ;
		}
	} 

	int ZDBTrigger::Init(void)
	{
		m_pAreaEventHandler = new DBTrigger::ZAreaEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetAreaTable()->RegisterEventHandler(m_pAreaEventHandler);

		m_pGameEventHandler = new DBTrigger::ZGameEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetGameTable()->RegisterEventHandler(m_pGameEventHandler);

		m_pClassEventHandler = new DBTrigger::ZClassEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetClassTable()->RegisterEventHandler(m_pClassEventHandler);

		m_pVDiskEventHandler = new DBTrigger::ZVDiskEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetVDiskTable()->RegisterEventHandler(m_pVDiskEventHandler);

		m_pServerEventHandler = new DBTrigger::ZServerEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetServerTable()->RegisterEventHandler(m_pServerEventHandler);

		m_pCmpBootTaskEventHandler = new DBTrigger::ZCmpBootTaskEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetCmpBootTaskTable()->RegisterEventHandler(m_pCmpBootTaskEventHandler);

		m_pSyncTaskEventHandler = new DBTrigger::ZSyncTaskEventHandler(this, ~SQL_OP_SELECT); 
		m_pTableMgr->GetSyncTaskTable()->RegisterEventHandler(m_pSyncTaskEventHandler);

		return 0;
	}

	void ZDBTrigger::Fini(void)
	{
		m_pTableMgr->GetAreaTable()->UnregisterEventHandler(m_pAreaEventHandler); 
		delete m_pAreaEventHandler;

		m_pTableMgr->GetGameTable()->UnregisterEventHandler(m_pGameEventHandler); 
		delete m_pGameEventHandler;

		m_pTableMgr->GetClassTable()->UnregisterEventHandler(m_pClassEventHandler); 
		delete m_pClassEventHandler;

		m_pTableMgr->GetVDiskTable()->UnregisterEventHandler(m_pVDiskEventHandler); 
		delete m_pVDiskEventHandler;

		m_pTableMgr->GetServerTable()->UnregisterEventHandler(m_pServerEventHandler); 
		delete m_pServerEventHandler;

		m_pTableMgr->GetCmpBootTaskTable()->UnregisterEventHandler(m_pCmpBootTaskEventHandler); 
		delete m_pCmpBootTaskEventHandler;

		m_pTableMgr->GetSyncTaskTable()->UnregisterEventHandler(m_pSyncTaskEventHandler); 
		delete m_pSyncTaskEventHandler;

	}

	uint32 ZDBTrigger::Exec(void)
	{
		::CoInitialize(0);

		while(!IsDone())
		{
			WaitEvent(TRAGGER_DB_INTERVAL);
			if (IsDone()) 
				break;
			ProcessAreaEvents();
			ProcessGameEvents();
			ProcessClassEvents();
			ProcessVDiskEvents();
			ProcessServerEvents();
			ProcessCmpBootTaskEvents();
			ProcessSyncTaskEvents();
		}

		::CoUninitialize();

		return 0;
	}

}