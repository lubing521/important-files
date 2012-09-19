#ifndef DBWRITER_H
#define DBWRITER_H

#include "../../include/Extend STL/container.h"
#include "../../include/thread.h"

#include <tuple>

#define FORWARD_DECLARE_EVENTHANDLER(tname) \
	namespace DBWriter { \
		class Z##tname##EventHandler; \
	}

#define CREATE_EVENTHANDLER_OBJECT(tname) \
	m_p##tname##EventHandler = new DBWriter::Z##tname##EventHandler(this, ~SQL_OP_SELECT); \
	m_pTableMgr->Get##tname##Table()->RegisterEventHandler(m_p##tname##EventHandler);

#define DESTROY_EVENTHANDLER_OBJECT(tname) \
	m_pTableMgr->Get##tname##Table()->UnregisterEventHandler(m_p##tname##EventHandler); \
	delete m_p##tname##EventHandler;

#define DECLARE_EVENTHANDLER_OBJECT(tname, Kty) \
	public: \
		void Add##tname##Event(uint32 optype, const Kty& key, uint64 mask) \
		{ \
			m_##tname##EventList.put_tail(std::tr1::make_tuple(optype, key, mask)); \
			SetEvent(); \
		} \
	private: \
		void Process##tname##Events(uint32 threshold = 1); \
		bool Insert##tname(const db::t##tname &d); \
		bool Update##tname(const Kty& key, const db::t##tname &d, uint64 mask); \
		bool Delete##tname(const Kty& key); \
		DBWriter::Z##tname##EventHandler *m_p##tname##EventHandler; \
		typedef std::tr1::tuple<uint32, Kty, uint64> tname##EventStruct; \
		typedef ZSequenceContainer<tname##EventStruct> tname##EventList; \
		tname##EventList m_##tname##EventList;

#define DECLARE_EVENTHANDLER_2K_OBJECT(tname, Kty1, Kty2) \
	public: \
		void Add##tname##Event(uint32 optype, const Kty1& key1, const Kty2& key2, uint64 mask) \
		{ \
			m_##tname##EventList.put_tail(std::tr1::make_tuple(optype, key1, key2, mask)); \
			SetEvent(); \
		} \
	private: \
		void Process##tname##Events(uint32 threshold = 1); \
		bool Insert##tname(const db::t##tname &d); \
		bool Update##tname(const Kty1& key1, const Kty2& key2, const db::t##tname &d, uint64 mask); \
		bool Delete##tname(const Kty1& key1, const Kty2& key2); \
		DBWriter::Z##tname##EventHandler *m_p##tname##EventHandler; \
		typedef std::tr1::tuple<uint32, Kty1, Kty2, uint64> tname##EventStruct; \
		typedef ZSequenceContainer<tname##EventStruct> tname##EventList; \
		tname##EventList m_##tname##EventList;



namespace i8desk {

FORWARD_DECLARE_EVENTHANDLER(Game)
FORWARD_DECLARE_EVENTHANDLER(Class)
FORWARD_DECLARE_EVENTHANDLER(Area)
FORWARD_DECLARE_EVENTHANDLER(GameArea)
FORWARD_DECLARE_EVENTHANDLER(Client)
FORWARD_DECLARE_EVENTHANDLER(CmpStartTask)
FORWARD_DECLARE_EVENTHANDLER(SysOpt)
FORWARD_DECLARE_EVENTHANDLER(VDisk)
FORWARD_DECLARE_EVENTHANDLER(User)
FORWARD_DECLARE_EVENTHANDLER(Favorite)
FORWARD_DECLARE_EVENTHANDLER(SyncTask)
FORWARD_DECLARE_EVENTHANDLER(SyncGame)

class ZTableMgr;
struct IDatabase;

class ZDBWriter 
	: public ZTaskBase
{
	DECLARE_EVENTHANDLER_OBJECT(Game, uint32)
	DECLARE_EVENTHANDLER_OBJECT(Class, std::string)
	DECLARE_EVENTHANDLER_OBJECT(Area, std::string)
	DECLARE_EVENTHANDLER_2K_OBJECT(GameArea, uint32, std::string)
	DECLARE_EVENTHANDLER_OBJECT(Client, std::string)
	DECLARE_EVENTHANDLER_OBJECT(CmpStartTask, std::string)
	DECLARE_EVENTHANDLER_OBJECT(SysOpt, std::string)
	DECLARE_EVENTHANDLER_OBJECT(VDisk, std::string)
	DECLARE_EVENTHANDLER_OBJECT(User, std::string)
	DECLARE_EVENTHANDLER_OBJECT(Favorite, std::string)
	DECLARE_EVENTHANDLER_OBJECT(SyncTask, std::string)
	DECLARE_EVENTHANDLER_2K_OBJECT(SyncGame, std::string, uint32)
public:
	ZDBWriter(ZTableMgr *pTableMgr);
	~ZDBWriter(void);
	void SetDatabase(IDatabase *pDatabae);
	void SaveData();

protected:
	int Init(void);
	void Fini(void);
	unsigned int Exec(void);
	bool ExecSql(void);

private:
	bool m_bClose;
	ZTableMgr *m_pTableMgr;
	IDatabase *m_pDatabase;
	char *m_pSqlBuf;
};


} //namespace i8desk


#endif //DBWRITER_H
