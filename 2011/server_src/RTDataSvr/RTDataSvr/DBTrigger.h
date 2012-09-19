#ifndef DBTRIGGER_H
#define DBTRIGGER_H

#include "container.h"
#include "thread.h"
#include "tablemgr.h"

#include <tuple>

namespace i8desk
{
	
	namespace DBTrigger
	{

			class ZAreaEventHandler;
			class ZGameEventHandler;
			class ZClassEventHandler;
			class ZVDiskEventHandler;
			class ZServerEventHandler;
			class ZCmpBootTaskEventHandler;
			class ZSyncTaskEventHandler;
			class ZTableMgr;

			class ZServerTable;
	}

	class ZDBTrigger 
		: public ZTaskBase
	{
	public:
		void AddAreaEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_AreaEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 

		void AddGameEvent(uint32 optype,  uint32 key, uint64 mask) 
		{ 
			m_GameEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
		void AddClassEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_ClassEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
		void AddVDiskEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_VDiskEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
		void AddServerEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_ServerEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
		void AddCmpBootTaskEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_CmpBootTaskEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
		void AddSyncTaskEvent(uint32 optype, const stdex::tString& key, uint64 mask) 
		{ 
			m_SyncTaskEventList.put_tail(std::tr1::make_tuple(optype, key, mask)); 
			SetEvent(); 
		} 
	private:
		void ProcessAreaEvents(uint32 threshold = 1); 
		DBTrigger::ZAreaEventHandler *m_pAreaEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> AreaEventStruct;
		typedef ZSequenceContainer<AreaEventStruct> AreaEventList; 
		AreaEventList m_AreaEventList;

		void ProcessGameEvents(uint32 threshold = 1); 
		DBTrigger::ZGameEventHandler *m_pGameEventHandler; 
		typedef std::tr1::tuple<uint32, uint32, uint64> GameEventStruct;
		typedef ZSequenceContainer<GameEventStruct> GameEventList; 
		GameEventList m_GameEventList;

		void ProcessClassEvents(uint32 threshold = 1); 
		DBTrigger::ZClassEventHandler *m_pClassEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> ClassEventStruct;
		typedef ZSequenceContainer<ClassEventStruct> ClassEventList; 
		ClassEventList m_ClassEventList;

		void ProcessVDiskEvents(uint32 threshold = 1); 
		DBTrigger::ZVDiskEventHandler *m_pVDiskEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> VDiskEventStruct;
		typedef ZSequenceContainer<VDiskEventStruct> VDiskEventList; 
		VDiskEventList m_VDiskEventList;

		void ProcessServerEvents(uint32 threshold = 1); 
		DBTrigger::ZServerEventHandler *m_pServerEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> ServerEventStruct;
		typedef ZSequenceContainer<AreaEventStruct> ServerEventList; 
		ServerEventList m_ServerEventList;

		void ProcessCmpBootTaskEvents(uint32 threshold = 1); 
		DBTrigger::ZCmpBootTaskEventHandler *m_pCmpBootTaskEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> CmpBootTaskEventStruct;
		typedef ZSequenceContainer<CmpBootTaskEventStruct> CmpBootTaskEventList; 
		CmpBootTaskEventList m_CmpBootTaskEventList;


		void ProcessSyncTaskEvents(uint32 threshold = 1); 
		DBTrigger::ZSyncTaskEventHandler *m_pSyncTaskEventHandler; 
		typedef std::tr1::tuple<uint32, stdex::tString, uint64> SyncTaskEventStruct;
		typedef ZSequenceContainer<SyncTaskEventStruct> SyncTaskEventList; 
		SyncTaskEventList m_SyncTaskEventList;


	public:
		ZDBTrigger(ZTableMgr *pTableMgr);
		~ZDBTrigger(void);

	protected:
		int Init(void);
		void Fini(void);
		uint32 Exec(void);

	private:
		bool m_bClose;
		ZTableMgr *m_pTableMgr;
	};



}

#endif
