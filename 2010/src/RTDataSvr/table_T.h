#ifndef TABLE_T_H
#define TABLE_T_H

#include "../../include/dbengine.h"
#include "../../include/i8mp.h"
#include "../../include/i8logmsg.h"
#include "../../include/i8type.h"

#include "../../include/Extend STL/container.h"

#include <functional>
#include <vector>
#include <list>
#include <unordered_map>

#define DEFAULT_NULL_TABLE_RECORD_NUM 256

#include "dbversion.h"

namespace i8desk {

//
//所有表记录集的类模板
//
template<
	typename Tty, //Tty:记录结构类型
	typename Ity, //Ity:记录集接口类型
	typename Mty  //Mty:记录集内存池类型
>
class ZRecordset_T : public Ity
{
public:
//Contructor 
	ZRecordset_T(Mty& mp) : m_mp(mp) 
	{
	}

//IGameRecordset interface
	void Release(void) 
	{
		delete this; 
	}

	uint32 GetCount(void) const
	{
		return m_datas.size(); 
	}

	bool GetData(uint32 i, Tty *d, ulong *ver) const
	{
		I8_ASSERT(d);
		I8_ASSERT(i < m_datas.size());

		if (i < m_datas.size()) {
			*d = *m_datas[i].d;
			if (ver)
				*ver = m_datas[i].ver;
			return true;
		}
		return false;
	}

	Tty *GetData(uint32 i, ulong *ver = 0) const
	{
		I8_ASSERT(i < m_datas.size());

		if (i < m_datas.size()) {
			if (ver)
				*ver = m_datas[i].ver;
			return m_datas[i].d;
		}
		return 0;
	}

//internal method
	void AddData(const Tty& d, ulong ver)
	{
		Data data;
		data.d = static_cast<Tty *>(m_mp.malloc());
		*data.d = d;
		data.ver  = ver;
		m_datas.push_back(data);
	}

	template<typename S> void Sort(S *sorter)
	{
		I8_ASSERT(sorter);

		using std::tr1::placeholders::_1;
		using std::tr1::placeholders::_2;
		std::sort(m_datas.begin(), m_datas.end(), 
			std::tr1::bind(&S::sort, sorter, 
				std::tr1::bind(&Data::d, _1),
				std::tr1::bind(&Data::d, _2)));
	}

private:
// Destructor, the class instance must be created by 'new' in heap
// and must call release once to destructor(using delete this)
	virtual ~ZRecordset_T()
	{
		for (size_t i = 0; i < m_datas.size(); i++) {
			m_mp.free(m_datas[i].d);
		}
	}

	struct Data {
		Tty *d;
		ulong ver;
	};
	std::vector<Data> m_datas;
	Mty& m_mp;
};

//
// 索引表模板
//

struct Idx {
	uint32 idx;
	ulong ver;
};

template<
	typename Kty //Kty:主键的数据类型
>
class ZIndex_T
{
public:
	void SetIdx(Kty key, uint32 idx) { m_idxs[key].idx = idx; }
	void SetVer(Kty key, ulong ver) { m_idxs[key].ver = ver; }
	uint32 GetIdx(Kty key) { return m_idxs[key].idx; }
	ulong GetVer(Kty key) { return m_idxs[key].ver; }
	void ClearIdx(Kty key) { m_idxs.erase(key); }
	void ClearAll(void) { m_idxs.clear(); }
	template<typename Hty> 
	static void InvokeEventHandler(Hty *handler, uint32 optype, Kty key, uint64 mask) 
		{ handler->HandleEvent(optype, key, mask); }
	static Kty CloneKey(Kty key) { return key; }
	static void FreeKey(Kty key) { (void)key; }
private:
	std::tr1::unordered_map<Kty, Idx> m_idxs;
};

template<> //For string key, because map<const char *, Idx> will be incorrect
class ZIndex_T<const char *>
{
public:
	typedef const char * Kty;
	void SetIdx(Kty key, uint32 idx) { m_idxs[key].idx = idx; }
	void SetVer(Kty key, ulong ver) { m_idxs[key].ver = ver; }
	uint32 GetIdx(Kty key) { return m_idxs[key].idx; }
	ulong GetVer(Kty key) { return m_idxs[key].ver; }
	void ClearIdx(Kty key) { m_idxs.erase(key); }
	void ClearAll(void) { m_idxs.clear(); }
	template<typename Hty> 
	static void InvokeEventHandler(Hty *handler, uint32 optype, Kty key, uint64 mask)
		{ handler->HandleEvent(optype, key, mask); }
	static Kty CloneKey(Kty key) 
	{
		size_t n = strlen(key) + 1;
		char *p = I8MP.alloc(n); 
		memcpy(p, key, n);
		return p;
	}
	static void FreeKey(Kty key) 
	{
		I8MP.free((void *)key);
	}
private:
	std::tr1::unordered_map<std::string, Idx> m_idxs; //Kty: const char *
};

//两个字段组成主键的索引
template<typename DKty> //For double field key
class ZDoubleFieldIndex_T
{
public:
	typedef DKty Kty;
	typedef typename Kty::Key1Ty Key1Ty;
	typedef typename Kty::Key2Ty Key2Ty;
	typedef typename Kty::Key1Idx Key1Idx;
	typedef typename Kty::Key2Idx Key2Idx;
	void SetIdx(const Kty& key, uint32 idx) { m_idxs[key.key1][key.key2].idx = idx; }
	void SetVer(const Kty& key, ulong ver) { m_idxs[key.key1][key.key2].ver = ver; }
	uint32 GetIdx(const Kty& key) { return m_idxs[key.key1][key.key2].idx; }
	void GetIdx(Key1Ty key1, std::vector<uint32>& idxs)
	{
		Key2Idx& k2i = m_idxs[key1];
		Key2Idx::iterator it = k2i.begin();
		for (; it != k2i.end(); ++it) {
			idxs.push_back(it->second.idx);
		}
	}
	ulong GetVer(const Kty& key) { return m_idxs[key.key1][key.key2].ver; }
	void ClearIdx(const Kty& key) { m_idxs[key.key1].erase(key.key2); }
	void ClearIdx(Key1Ty key1) { m_idxs.erase(key1); }
	void ClearAll(void) { m_idxs.clear(); }
	template<typename Hty> 
	static void InvokeEventHandler(Hty *handler, uint32 optype, const Kty& key, uint64 mask)
		{ handler->HandleEvent(optype, key.key1, key.key2, mask); }
	static Kty CloneKey(const Kty& key) { return Kty::Clone(key); }
	static void FreeKey(Kty& key) { Kty::Free(key); }
private:
	Key1Idx m_idxs;
};

struct Uint32Uint32Key 
{
	typedef Uint32Uint32Key Kty;
	typedef uint32 Key1Ty;
	typedef uint32 Key2Ty;

	typedef std::tr1::unordered_map<uint32, Idx> Key2Idx;
	typedef std::tr1::unordered_map<uint32, Key2Idx> Key1Idx;

	Key1Ty key1;
	Key2Ty key2;

	static Kty Clone(const Kty& key) { return key; }
	static void Free(Kty& key) { (void)key; }
};

template<> 
class ZIndex_T<Uint32Uint32Key> 
	: public ZDoubleFieldIndex_T<Uint32Uint32Key> 
{
};

struct Uint32StringKey {
	typedef Uint32StringKey Kty;
	typedef uint32 Key1Ty;
	typedef const char * Key2Ty;

	typedef std::tr1::unordered_map<std::string, Idx> Key2Idx;
	typedef std::tr1::unordered_map<uint32, Key2Idx> Key1Idx;

	Key1Ty key1;
	Key2Ty key2;

	static Kty Clone(const Kty& key) 
	{
		Kty k;

		k.key1 = key.key1;

		size_t n = strlen(key.key2) + 1;
		k.key2 = I8MP.alloc(n); 
		memcpy((void *)k.key2, key.key2, n);

		return k;
	}
	static void Free(Kty& key) 
	{
		I8MP.free((void *)key.key2);
	}
};

template<> 
class ZIndex_T<Uint32StringKey> 
	: public ZDoubleFieldIndex_T<Uint32StringKey> 
{
};


struct StringUint32Key 
{
	typedef StringUint32Key Kty;
	typedef const char * Key1Ty;
	typedef uint32 Key2Ty;

	typedef std::tr1::unordered_map<uint32, Idx> Key2Idx;
	typedef std::tr1::unordered_map<std::string, Key2Idx> Key1Idx;

	Key1Ty key1;
	Key2Ty key2;

	static Kty Clone(const Kty& key) 
	{
		Kty k;

		size_t n = strlen(key.key1) + 1;
		k.key1 = I8MP.alloc(n); 
		memcpy((void *)k.key1, key.key1, n);

		k.key2 = key.key2;

		return k;
	}
	static void Free(Kty& key) 
	{
		I8MP.free((void *)key.key1);
	}
};

template<> 
class ZIndex_T<StringUint32Key> 
	: public ZDoubleFieldIndex_T<StringUint32Key> 
{
};


struct StringStringKey 
{
	typedef StringStringKey Kty;
	typedef const char * Key1Ty;
	typedef const char * Key2Ty;

	typedef std::tr1::unordered_map<std::string, Idx> Key2Idx;
	typedef std::tr1::unordered_map<std::string, Key2Idx> Key1Idx;

	Key1Ty key1;
	Key2Ty key2;

	static Kty Clone(const Kty& key) 
	{
		Kty k;

		size_t n = strlen(key.key1) + 1;
		k.key1 = I8MP.alloc(n); 
		memcpy((void *)k.key1, key.key1, n);

		n = strlen(key.key2) + 1;
		k.key2 = I8MP.alloc(n); 
		memcpy((void *)k.key2, key.key2, n);

		return k;
	}
	static void Free(Kty& key) 
	{
		I8MP.free((void *)key.key1);
		I8MP.free((void *)key.key2);
	}
};

template<> 
class ZIndex_T<StringStringKey> 
	: public ZDoubleFieldIndex_T<StringStringKey>
{
};


//
// 表的类模板
//
template<
	typename Tty, //Tty:记录结构类型
	typename Kty, //Kty:主键的数据类型
	typename Ity, //Ity:表接口类型
	typename Hty, //Hty:表事件回调接口类型
	typename Rty, //Rty:记录集接口类型 
	typename Fty, //Fty:过滤器接口类型
	typename Sty, //Sty:排序器接口类型
	typename Lty, //Lty:表锁的类型
	typename Impty //Impty:具体实现的类型
>
class ZTable_T : public Ity
{
public:
	// typedef for subclass
	typedef Tty Tty; //Tty:记录结构类型
	typedef Kty Kty; //Kty:主键的数据类型
	typedef Ity Ity; //Ity:表接口类型
	typedef Hty Hty; //Hty:表事件回调接口类型
	typedef Rty Rty; //Rty:记录集接口类型 
	typedef Fty Fty; //Fty:过滤器接口类型
	typedef Sty Sty; //Sty:排序器接口类型
	typedef Lty Lty; //Lty:表锁的类型
	typedef Impty Impty; //Impty:具体实现的类型
	typedef ZIndex_T<Kty> Index;		//索引类

protected:
	typedef ZI8BlockMP<sizeof(Tty)> MP; //记录集的内存池类型
	typedef ZRecordset_T<Tty, Rty, MP> RecordSet; //记录集类型

	MP m_mp;
	Lty m_lock;

	Index m_idxs;

	Tty *m_pRecords;
	uint32 m_nRecords;
	ulong m_ulVersion;

public:
//Constructor lpszName必须是文本常量
	ZTable_T(LPCTSTR lpszName) 
		: m_lpszName(lpszName)
		, m_nRecords(0)
		, m_pRecords(0)
		, m_ulVersion(0)
	{
	}
	virtual ~ZTable_T() 
	{
		I8MP.release(m_pRecords);
	}

//IXXXXTable Ity:表接口 common method
	ulong GetVersion(void)
	{
		I8_GUARD(ZGuard, Lty, m_lock);
		return m_ulVersion;
	}

	int Insert(const Tty *const d)
	{
		I8_ASSERT(d);

		Kty key = _This()->GetKey(*d);
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			uint32 i = m_idxs.GetIdx(key);
			if (i != 0)
				return -1; //存在

			Tty& data = GetFreeRecord(i);
			data = *d;
			m_idxs.SetIdx(key, i);
		}

		HandleEvent(SQL_OP_INSERT, key, 0);

		return 0;
	}

	int Ensure(const Tty *const d, uint64 mask)
	{
		I8_ASSERT(d);

		Kty key = _This()->GetKey(*d);

		if (Update(key, d, mask) == 0)
			return 0;

		{
			m_lock.Lock();

			uint32 i = m_idxs.GetIdx(key);
			if (i != 0) {
				m_lock.UnLock();
				return Update(key, d, mask);
			}

			Tty& data = GetFreeRecord(i);
			data = *d;
			m_idxs.SetIdx(key, i);

			m_lock.UnLock();
		}

		HandleEvent(SQL_OP_INSERT, key, 0);

		return 0;
	}

	int Update(Kty key, const Tty *const d, uint64 mask)
	{
		I8_ASSERT(d);

		uint64 realmask = 0;
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			uint32 i = m_idxs.GetIdx(key);
			if (i == 0) 
				return -1;

			Tty& data = m_pRecords[i];
			realmask = _This()->UpdateRecord(&data, d, mask);
		}

		if (realmask) {
			HandleEvent(SQL_OP_UPDATE, key, realmask);
		}

		return 0;
	}


	int Update(const Tty *const d, uint64 mask, Fty *filter)
	{
		I8_ASSERT(d && filter);
		
		struct UpdateMask {
			Kty key;
			uint64 mask;
		};
		std::vector<UpdateMask> updates;

		{
			I8_GUARD(ZGuard, Lty, m_lock);

			for (uint32 i = 1; i < m_nRecords; i++) {
				Tty& data = m_pRecords[i];
				if (_This()->InvalidRecord(data) || filter->bypass(&data))
					continue;

				if (uint64 realmask = _This()->UpdateRecord(&data, d, mask)) {
					UpdateMask um;
					um.key = Index::CloneKey(_This()->GetKey(data));
					um.mask = realmask;
					updates.push_back(um);
				}
			}
		}

		for (size_t n = 0; n < updates.size(); n++) {
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
			Index::FreeKey(updates[n].key);
		}

		return 0;
	}


	int Cumulate(Kty key, const Tty *const d, uint64 mask)
	{
		I8_ASSERT(d);

		uint64 realmask = 0;
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			uint32 i = m_idxs.GetIdx(key);
			if (i == 0) 
				return -1;

			Tty& data = m_pRecords[i];
			realmask = _This()->CumulateRecord(&data, d, mask);
		}

		if (realmask) {
			HandleEvent(SQL_OP_UPDATE, key, realmask);
		}

		return 0;
	}


	int Cumulate(const Tty *const d, uint64 mask, Fty *filter)
	{
		I8_ASSERT(d && filter);
		
		struct UpdateMask {
			Kty key;
			uint64 mask;
		};
		std::vector<UpdateMask> updates;

		{
			I8_GUARD(ZGuard, Lty, m_lock);

			for (uint32 i = 1; i < m_nRecords; i++) {
				Tty& data = m_pRecords[i];
				if (_This()->InvalidRecord(data) || filter->bypass(&data))
					continue;

				if (uint64 realmask = _This()->CumulateRecord(&data, d, mask)) {
					UpdateMask um;
					um.key = Index::CloneKey(_This()->GetKey(data));
					um.mask = realmask;
					updates.push_back(um);
				}
			}
		}

		for (size_t n = 0; n < updates.size(); n++) {
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask);
			Index::FreeKey(updates[n].key);
		}

		return 0;
	}

	int Delete(Kty key)
	{
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			uint32 i = m_idxs.GetIdx(key);
			if (i == 0) 
				return -1;

			m_idxs.ClearIdx(key);
			memset(m_pRecords + i, 0, sizeof(Tty));
		}
		
		HandleEvent(SQL_OP_DELETE, key, 0);

		return 0;
	}	

	int Delete(Fty *filter)
	{
		I8_ASSERT(filter);

		std::vector<Kty> keys;
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			for (uint32 i = 1; i < m_nRecords; i++) {
				Tty& data = m_pRecords[i];
				if (_This()->InvalidRecord(data) || filter->bypass(&data))
					continue;

				Kty key = _This()->GetKey(data);
				keys.push_back(Index::CloneKey(key));
				m_idxs.ClearIdx(key);
				memset(m_pRecords + i, 0, sizeof(Tty));
			}
		}

		for (size_t n = 0; n < keys.size(); n++) {
			HandleEvent(SQL_OP_DELETE, keys[n], 0);
			Index::FreeKey(keys[n]);
		}

		return 0;
	}

	int GetData(Kty key, Tty *d, ulong *ver)
	{
		I8_ASSERT(d);
		{
			I8_GUARD(ZGuard, Lty, m_lock);

			uint32 i = m_idxs.GetIdx(key);
			if (i == 0) {
				return -1;
			}
			I8_ASSERT(i < m_nRecords);
			
			*d = m_pRecords[i];
			if (ver) {
				*ver = m_idxs.GetVer(key);
			}
		}

		return 0;
	}

	int Select(Rty **recordset, Fty *filter, Sty *sorter)
	{
		I8_ASSERT(recordset);

		RecordSet *pRecordset = new RecordSet(m_mp);
		{
			I8_GUARD(ZGuard, Lty, m_lock);
			
			if (filter) {
				for (uint32 i = 1; i < m_nRecords; i++) {
					Tty& data = m_pRecords[i];
					if (_This()->InvalidRecord(data) || filter->bypass(&data))
						continue;
					pRecordset->AddData(data, m_idxs.GetVer(_This()->GetKey(data)));
				}
			}
			else {
				for (uint32 i = 1; i < m_nRecords; i++) {
					Tty& data = m_pRecords[i];
					if (_This()->InvalidRecord(data))
						continue;
					pRecordset->AddData(data, m_idxs.GetVer(_This()->GetKey(data)));
				}
			}
		}

		if (sorter) {
			pRecordset->Sort(sorter);
		}
		*recordset = static_cast<Rty *>(pRecordset);

		return 0;
	}

	void RegisterEventHandler(Hty *handler)
	{
		m_handlers.put_tail(handler);
	}

	void UnregisterEventHandler(Hty *handler)
	{
		m_handlers.remove(handler);
	}

//Load and Save ,可以在实现类中改变缺省行为，如临时表的处理等
	virtual int Load(IDatabase *pDatabase)
	{
		I8_INFOR((LM_INFO, I8_TEXT("从数据库加载表[%s]数据...\n"), m_lpszName));

		_RecordsetPtr prcd;
		{
			TCHAR sql[BUFSIZ] = _T("SELECT * From ");
			lstrcat(sql, m_lpszName);
			CAutoLock<IDatabase> lock(pDatabase);
			if(!pDatabase->ExecSql(sql, prcd)) {
				I8_ERROR_RETURN((LM_ERROR, I8_TEXT("初始化表[%s]时失败!\n"), m_lpszName), -1);
			}
		}

		I8_GUARD(ZGuard, Lty, m_lock);

		I8MP.release(m_pRecords);
		I8MP.reclaim();
		m_nRecords = prcd->GetRecordCount() + 1;
		m_pRecords = I8MP.malloc<Tty>(m_nRecords);
		memset(m_pRecords, 0, sizeof(Tty) * m_nRecords);

		for (uint32 i = 1; i < m_nRecords; i++) {

			Tty& data = m_pRecords[i];
			_This()->FillRecord(data, prcd);

			//构建索引
			Kty key = _This()->GetKey(data);
			m_idxs.SetIdx(key, i);
			m_idxs.SetVer(key, CalBufCRC32(&data, sizeof(Tty)));

			prcd->MoveNext();
		}
		m_ulVersion = ::_time32(0);

		return 0;
	}

	virtual int Save(IDatabase *pDataBase)
	{
		return 0;
	}

	virtual int Load(const std_string& strPath)
	{
		I8_GUARD(ZGuard, Lty, m_lock);

		if (!LoadFromFile(strPath)) {
			I8_INFOR((LM_INFO, I8_TEXT("从文件加载表数据失败\n")));

			I8MP.release(m_pRecords);
			I8MP.reclaim();
			m_nRecords = DEFAULT_NULL_TABLE_RECORD_NUM;
			m_pRecords = I8MP.malloc<Tty>(m_nRecords);

			m_idxs.ClearAll();
			memset(m_pRecords, 0, sizeof(Tty) * m_nRecords);
			m_ulVersion = ::_time32(0);
		}

		return 0;
	}

	virtual int Save(const std_string& strPath)
	{
		TCHAR lpszFileName[MAX_PATH];
		::_stprintf(lpszFileName, _T("%s%s"), strPath.c_str(), m_lpszName);

		I8_INFOR((LM_INFO, I8_TEXT("保存数据表到文件, %s...\n"), lpszFileName));

		I8_GUARD(ZGuard, Lty, m_lock);

		if (!m_pRecords)
			return 0;

		FILE* f =  ::fopen(lpszFileName, _T("w+b"));
		if (!f) {
			I8_ERROR_RETURN((LM_ERROR,
				I8_TEXT("打开文件失败, %s!\n"), lpszFileName), -1);
		}

		uint32 dbversion = DB_VERSION_CUR;
		if (fwrite(&dbversion, 4, 1, f) != 1 //数据库设计版本
			|| fwrite(&m_ulVersion, 4, 1, f) != 1    //表数据内容版本
			|| fwrite(m_pRecords, sizeof(Tty), m_nRecords, f) != m_nRecords) {
			fclose(f);
			I8_ERROR_RETURN((LM_ERROR,
				I8_TEXT("写入文件内容失败, %s!\n"), lpszFileName), -1);
		}
		fclose(f);

		return 0;
	}

//Table Name
	LPCTSTR GetName(void) const { return m_lpszName; }

//Create Filter Sorter Helper
	Fty *CreateFilter(uint64 CondMask, uint32 CondMode, 
		uint32 dwParam, const std::string& strParam)
	{
		if (CondMode == 0)
			return 0;
		
		void *p = m_FilterMP.alloc();
		return new (p)Filter(_This(), CondMask, CondMode, dwParam, strParam);
	}

	void DestroyFilter(Fty *pFilter)
	{
		if (!pFilter) 
			return;
			
		((Filter*)pFilter)->~Filter();
		m_FilterMP.free(pFilter);
	}

	Sty *CreateSorter(uint64 mask, uint32 desc)
	{
		if (mask == 0)
			return 0;

		void *p = m_SorterMP.alloc();
		return new (p)Sorter(_This(), mask, desc);
	}

	void DestroySorter(Sty *pSorter)
	{
		if (!pSorter) 
			return;
			
		((Sorter*)pSorter)->~Sorter();
		m_SorterMP.free(pSorter);
	}


private:
	struct Filter : public Fty {
		Filter(Impty *pTable, uint64 CondMask, uint32 CondMode, 
			uint32 dwParam, const std::string& strParam)
			: m_pTable(pTable)
			, m_CondMask(CondMask) 
			, m_CondMode(CondMode)
			, m_dwParam(dwParam) 
		{
			strncpy(m_szParam, strParam.c_str(), sizeof(m_szParam) - 1);
		}
		bool bypass(const Tty *const d) 
		{
			return !m_pTable->SqlCond(d, m_CondMask, m_CondMode, m_dwParam, m_szParam);
		}
	private:
		uint64 m_CondMask;
		uint32 m_CondMode;
		uint32 m_dwParam;
		char m_szParam[sizeof(Tty)];
		Impty *m_pTable;
	};
	
	typedef ZI8BlockMP<sizeof(Filter)> FilterMP;
	FilterMP m_FilterMP;

	struct Sorter : public Sty {
		Sorter(Impty *pTable, uint64 mask, uint32 desc)
			: m_pTable(pTable), m_mask(mask), m_desc(desc)
		{
		}
		bool sort(const Tty *const d1, const Tty *const d2)
		{
			return m_pTable->SqlSort(d1, d2, m_mask, m_desc);
		}
	private:
		Impty *m_pTable;
		uint64 m_mask;
		uint32 m_desc;
	};

	typedef ZI8BlockMP<sizeof(Sorter)> SorterMP;
	SorterMP m_SorterMP;   

protected:
	/* 下面的方法与具体的记录字段相关，必须在派生类中得到实现
	bool InvalidRecord(const Tty& d);
	Kty GetKey(const Tty& d);
	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask);
	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask);
	void FillRecord(Tty& d, const _RecordsetPtr& rst);
	bool SqlCond(const Tty *const d, uint64 CondMask, uint32 CondMode,
		uint32 dwParam, const char *szParam);
	bool SqlSort(const Tty *const d1, const Tty *const d2, uint64 mask, uint32 desc);
	//*/

	void HandleEvent(uint32 optype, Kty key, uint64 mask)
	{
		//刷新版本
		I8_ASSERT(optype != SQL_OP_SELECT);
		{
			I8_GUARD(ZGuard, Lty, m_lock);
			//表的版本
			m_ulVersion = ::_time32(0); 
			if (optype == SQL_OP_INSERT || optype == SQL_OP_UPDATE) {
				//记录的版本
				if (uint32 i = m_idxs.GetIdx(key)) {
					m_idxs.SetVer(key, CalBufCRC32(m_pRecords + i, sizeof(Tty)));
				}
			}
		}

		//触发事件处理器
		struct FValidOptype : std::unary_function<Hty *, bool> {
			FValidOptype(uint32 ot) : m_optype(ot) {}
			result_type operator()(const argument_type& _Arg) {
				return (_Arg->GetEvents() & m_optype) != 0;
			}
		private:
			uint32 m_optype;
		};

		using std::tr1::placeholders::_1;
		m_handlers.op_if(std::tr1::bind(&Index::InvokeEventHandler<Hty>, 
			_1, optype, key, mask), FValidOptype(optype));
	}

private:
	Impty* _This(void)
	{
		return static_cast<Impty *>(this);
	}

	Tty& GetFreeRecord(uint32& idx)
	{
		for (uint32 i = 1; i < m_nRecords; i++) {
			if (_This()->InvalidRecord(m_pRecords[i])) {
				idx = i;
				return m_pRecords[idx];
			}
		}

		if (m_nRecords) {
			static uint32 nIncSeed = 32;	//增长因子
			const uint32 nMaxInc = 1024;	//最大增长因子

			uint32 newsize = m_nRecords + nIncSeed;

			Tty *p = I8MP.calloc<Tty>(newsize);
			memcpy(p, m_pRecords, m_nRecords * sizeof(Tty));

			I8MP.free(m_pRecords);
			I8MP.reclaim();

			m_pRecords = p;
			idx = m_nRecords;
			m_nRecords = newsize;

			if (nIncSeed < nMaxInc) {
				nIncSeed <<= 1; 
			}
		} 
		else {
			m_nRecords = DEFAULT_NULL_TABLE_RECORD_NUM;
			m_pRecords = I8MP.malloc<Tty>(m_nRecords);
			memset(m_pRecords, 0, sizeof(Tty) * m_nRecords);
			idx = 1;
		}

		return m_pRecords[idx];
	}

	bool LoadFromFile(const std::string strPath)
	{
		TCHAR szFileName[ MAX_PATH ];
		::_stprintf(szFileName, _T("%s%s"), strPath.c_str(), m_lpszName);

		I8_INFOR((LM_INFO, I8_TEXT("从文件加载表数据, %s...\n"), m_lpszName));

		I8MP.release(m_pRecords);
		I8MP.reclaim();
		m_nRecords = 0;

		bool is_current_version = false;
		FILE_Ptr f =  fopen(szFileName, _T("rb"));

		if (!f) {
			I8_ERROR((LM_WARNING, I8_TEXT("初始化表[%s]时打开文件[%s]失败!\n"),
				m_lpszName, szFileName));

			//打开基本文件失败则打开当前版本的文件
			sprintf(szFileName, "%s%x\\%s", strPath.c_str(), DB_VERSION_CUR, m_lpszName);

			I8_INFOR((LM_INFO, I8_TEXT("从文件加载表数据, %s...\n"), szFileName));
			f =  fopen(szFileName, _T("rb"));
			if (!f) {
				I8_ERROR_RETURN((LM_ERROR, I8_TEXT("从文件初始化表[%s]失败!\n"),
					m_lpszName), false);
			}
			is_current_version = true;
		}

		bool load_ok = false;
		uint32 dbversion = 0;
		if (!is_current_version) {
			if (fread(&dbversion, 4, 1, f) != 1 || fread(&m_ulVersion, 4, 1, f) != 1) {
				I8_ERROR_RETURN((LM_ERROR, 
					I8_TEXT("初始化表[%s]时从文件读取版本失败!\n"),
					m_lpszName), false);
			}
			if (dbversion != DB_VERSION_CUR) {
				ZDBVersionTranlator<Tty> dbvt;
				load_ok = dbvt.TranlateDBVersion(f, dbversion, m_nRecords, m_pRecords);
				if (load_ok) {
					//转换成功后要备份老版本的文件
					BackupDatabaseFile(strPath.c_str(), dbversion, m_lpszName, 
						m_ulVersion, m_pRecords, m_nRecords);
				}
				else {
					I8_ERROR((LM_WARNING, 
						I8_TEXT("初始化表[%s]时转换数据版本失败[%x=>%x]!\n"),
						m_lpszName, dbversion, DB_VERSION_CUR));

					//转换失败则打开当前版本的文件
					sprintf(szFileName, "%s%x\\%s", strPath.c_str(), DB_VERSION_CUR, m_lpszName);

					I8_INFOR((LM_INFO, I8_TEXT("从文件加载表数据, %s...\n"), szFileName));
					f =  fopen(szFileName, _T("rb"));
					if (!f) {
						I8_ERROR_RETURN((LM_ERROR, I8_TEXT("从文件初始化表[%s]失败!\n"),
							m_lpszName), false);
					}
				}
			}
		}

		if (!load_ok) {
			if (fseek(f, 0, SEEK_END)) {
				I8_ERROR_RETURN((LM_ERROR, 
					I8_TEXT("初始化表[%s]时取文件大小失败!\n"),
					m_lpszName), false);
			}
			uint32 sz = (uint32)ftell(f);

			if (sz < 8) {
				I8_ERROR_RETURN((LM_ERROR, 
					I8_TEXT("初始化表[%s]时取文件大小异常!\n"),
					m_lpszName), false);
			}

			sz -= 8;
			if (sz < sizeof(Tty))  {
				I8_ERROR_RETURN((LM_ERROR,
					I8_TEXT("初始化表[%s]时文件数据大小异常!\n"),
					m_lpszName), false);
			}

			m_nRecords = (sz )/ sizeof(Tty);
			m_pRecords = I8MP.malloc<Tty>(m_nRecords);

			fseek(f, 4, SEEK_SET);
			fread(&m_ulVersion, 4, 1, f);

			if (fread(m_pRecords, sizeof(Tty), m_nRecords, f) != m_nRecords) {
				I8_ERROR_RETURN((LM_ERROR, 
					I8_TEXT("初始化表[%s]时从文件读取数据失败!\n"), 
					m_lpszName), false);
			}
		}

		//构建索引
		m_idxs.ClearAll();
		for (uint32 i = 0; i < m_nRecords; i++) {
			Tty& data = m_pRecords[i];
			if (!_This()->InvalidRecord(data)) {
				Kty key = _This()->GetKey(data);
				m_idxs.SetIdx(key, i);
				m_idxs.SetVer(key, CalBufCRC32(&data, sizeof(Tty)));
			}
		}

		return true;
	}

private:
	
	LPCTSTR m_lpszName;

	typedef ZSequenceContainer<Hty *, 
		std::list<Hty *>, ZSync> HandlerList;
	HandlerList m_handlers;
};

//方便表类的实现的宏定义

#define DECLARE_TABLE(classname, tablename) \
	public: \
	typedef ZTable_T<Tty, Kty, Ity, Hty, Rty, Fty, Sty, Lty, Impty> _Base; \
	friend class _Base; \
	classname() : _Base(_T(#tablename)) { }

#define GET_INT_FIELD(Var, Name, Rst) \
		I8_CHK_EXP(Var = Rst->GetCollect(Name);)

#define GET_STR_FIELD(Var, Name, Rst) \
		I8_CHK_EXP(strncpy(Var, (_bstr_t)Rst->GetCollect(Name), sizeof(Var));)


//update record code helper
#define BEGIN_UPDATE() uint64 realmask = 0

#define UPDATE_FIELD(urtab, urmn, urfn, urty) \
	do { if ((mask & MASK_T##urtab##_##urmn) \
		&& urty##_FIELD_COMP(data->##urfn, d->##urfn)) { \
		urty##_FIELD_COPY(data->##urfn, d->##urfn); \
		realmask |= MASK_T##urtab##_##urmn; \
	} } while (0)

#define END_UPDATE() return realmask

//cumulate record code helper
#define BEGIN_CUMULATE() uint64 realmask = 0

#define CUMULATE_FIELD(urtab, urmn, urfn, urty) \
	do { if ((mask & MASK_T##urtab##_##urmn) \
		&& urty##_FIELD_VALID(d->##urfn)) { \
		urty##_FIELD_CUMULATE(data->##urfn, d->##urfn); \
		realmask |= MASK_T##urtab##_##urmn; \
	} } while (0)

#define END_CUMULATE() return realmask



#define BEGIN_DECLARE_TABLE_CLASS(tname, key) \
	class Z##tname##Table  \
		: public ZTable_T< \
			db::t##tname, \
			key, \
			I##tname##Table, \
			I##tname##EventHandler, \
			I##tname##Recordset, \
			I##tname##Filter, \
			I##tname##Sorter, \
			ZSync, \
			Z##tname##Table \
		> \
	{ \
		DECLARE_TABLE(Z##tname##Table, t##tname) 

#define END_DECLARE_TABLE_CLASS() \
	};

#define BEGIN_SQLCOND_IMPLMENT() \
	public: \
	bool SqlCond(const Tty *const d, \
		uint64 CondMask, uint32 CondMode,  \
		uint32 dwParam, const char *szParam) \
	{ \
		if (CondMode == SQL_COND_MODE_EQUAL) { \
			switch (CondMask) {

#define SQLCOND_STRING_FIELD(urtab, urmn, urfn) \
			case MASK_T##urtab##_##urmn: \
				return !STRING_FIELD_COMP(szParam, d->urfn);

#define SQLCOND_VALUE_FIELD(urtab, urmn, urfn) \
			case MASK_T##urtab##_##urmn: \
				return !VALUE_FIELD_COMP(dwParam, d->urfn);

#define END_SQLCOND_IMPLMENT() \
			case 0: \
				I8_ASSERT(0); \
				break; \
			default: \
				I8_ASSERT(0); \
				break; \
			} \
		}  \
		else { \
			I8_ASSERT(0); \
		} \
		return false; \
	}

#define BEGIN_SQLSORT_IMPLMENT() \
	public: \
	bool SqlSort(const Tty *const d1, \
		const Tty *const d2, uint64 mask, uint32 desc) \
	{ \
		switch (mask) { 

#define SQLSORT_STRING_FIELD(urtab, urmn, urfn) \
		case MASK_T##urtab##_##urmn: \
			return (strcmp(d1->urfn, d2->urfn) > 0) == desc;

#define SQLSORT_VALUE_FIELD(urtab, urmn, urfn) \
		case MASK_T##urtab##_##urmn: \
			return (d1->urfn > d2->urfn) == desc;

#define END_SQLSORT_IMPLMENT() \
		case 0: \
				I8_ASSERT(0); \
				break; \
		default: \
			I8_ASSERT(0); \
			break; \
		} \
		return false; \
	}
	
#define INVALIDRECORD_IMPLMENT( fn, ty) \
	protected: \
	bool InvalidRecord(const Tty& d) \
	{ \
		return !ty##_FIELD_VALID(d.fn); \
	} 

#define GETKEY_IMPLMENT(fn) \
	protected: \
	Kty GetKey(const Tty& d) \
	{ \
		return d.fn; \
	}

#define BEGIN_UPDATE_IMPLMENT() \
	protected: \
	uint64 UpdateRecord(Tty *data, const Tty *const d, uint64 mask) \
	{ \
		BEGIN_UPDATE();

#define END_UPDATE_IMPLMENT() \
		END_UPDATE(); \
	}

#define BEGIN_CUMULATE_IMPLMENT() \
	protected: \
	uint64 CumulateRecord(Tty *data, const Tty *const d, uint64 mask) \
	{ \
		BEGIN_CUMULATE();

#define END_CUMULATE_IMPLMENT() \
		END_CUMULATE(); \
	}

#define BEGIN_FILLRECORD_IMPLMENT() \
	protected: \
	void FillRecord(Tty& d, const _RecordsetPtr& rst) \
	{ 

#define END_FILLRECORD_IMPLMENT() \
	}


#define GETKEY_K1_K2_IMPLMENT(fn1, fn2) \
	protected: \
	Kty GetKey(const Tty& d) \
	{ \
		Kty key; \
		key.key1 = d.fn1; \
		key.key2 = d.fn2; \
		return key; \
	}

#define INTERFACE_2K_IMPLMENT() \
	public: \
	typedef Index::Key1Ty Key1Ty; \
	typedef Index::Key2Ty Key2Ty; \
	int Update(Key1Ty key1, const Tty *const d, uint64 mask)  \
	{ \
		struct UpdateMask { \
			Kty key; \
			uint64 mask; \
		}; \
		std::vector<UpdateMask> updates; \
		{ \
			I8_GUARD(ZGuard, Lty, m_lock); \
			std::vector<uint32> idxs; \
 			m_idxs.GetIdx(key1, idxs); \
			for (size_t n = 0; n < idxs.size(); n++) { \
				Tty& data = m_pRecords[idxs[n]]; \
				if (uint64 realmask = UpdateRecord(&data, d, mask)) { \
					UpdateMask um; \
					um.key = Index::CloneKey(GetKey(data)); \
					um.mask = realmask; \
					updates.push_back(um); \
				} \
			} \
		} \
		for (size_t n = 0; n < updates.size(); n++) { \
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask); \
			Index::FreeKey(updates[n].key); \
		} \
		return 0; \
	} \
	int Update(Key1Ty key1, Key2Ty key2, const Tty *const d, uint64 mask)  \
	{ \
		Kty key = { key1, key2 }; \
		return _Base::Update(key, d, mask); \
	} \
	int Cumulate(Key1Ty key1, const Tty *const d, uint64 mask) \
	{ \
		struct UpdateMask { \
			Kty key; \
			uint64 mask; \
		}; \
		std::vector<UpdateMask> updates; \
		{ \
			I8_GUARD(ZGuard, Lty, m_lock); \
			std::vector<uint32> idxs; \
			m_idxs.GetIdx(key1, idxs); \
			for (size_t n = 0; n < idxs.size(); n++) { \
				Tty& data = m_pRecords[idxs[n]]; \
				if (uint64 realmask = CumulateRecord(&data, d, mask)) { \
					UpdateMask um; \
					um.key = Index::CloneKey(GetKey(data)); \
					um.mask = realmask; \
					updates.push_back(um); \
				} \
			} \
		} \
		for (size_t n = 0; n < updates.size(); n++) { \
			HandleEvent(SQL_OP_UPDATE, updates[n].key, updates[n].mask); \
			Index::FreeKey(updates[n].key); \
		} \
		return 0; \
	} \
	int Cumulate(Key1Ty key1, Key2Ty key2, const Tty *const d, uint64 mask)  \
	{ \
		Kty key = { key1, key2 }; \
		return _Base::Cumulate(key, d, mask); \
	} \
	int Delete(Key1Ty key1) \
	{ \
		std::vector<Kty> keys; \
		{ \
			I8_GUARD(ZGuard, Lty, m_lock); \
			std::vector<uint32> idxs; \
			m_idxs.GetIdx(key1, idxs); \
			for (size_t n = 0; n < idxs.size(); n++) { \
				Tty& data = m_pRecords[idxs[n]]; \
				keys.push_back(Index::CloneKey(GetKey(data))); \
				memset(&data, 0, sizeof(Tty)); \
			} \
			m_idxs.ClearIdx(key1);\
		} \
		for (size_t n = 0; n < keys.size(); n++) { \
			HandleEvent(SQL_OP_DELETE, keys[n], 0); \
			Index::FreeKey(keys[n]); \
		} \
		return 0; \
	} \
	int Delete(Key1Ty key1, Key2Ty key2) \
	{ \
		Kty key = { key1, key2 }; \
		return _Base::Delete(key); \
	} \
	int GetData(Key1Ty key1, Key2Ty key2, Tty *d, ulong *ver) \
	{ \
		Kty key = { key1, key2 }; \
		return _Base::GetData(key, d, ver); \
	} \
	int Select(Rty **recordset, Key1Ty key1, Sty *sorter) \
	{ \
		I8_ASSERT(recordset); \
		RecordSet *pRecordset = new RecordSet(m_mp); \
		{ \
			I8_GUARD(ZGuard, Lty, m_lock); \
			std::vector<uint32> idxs; \
			m_idxs.GetIdx(key1, idxs); \
			for (size_t n = 0; n < idxs.size(); n++) { \
				Tty& data = m_pRecords[idxs[n]]; \
				pRecordset->AddData(data, m_idxs.GetVer(GetKey(data))); \
			} \
		} \
		if (sorter) { \
			pRecordset->Sort(sorter); \
		} \
		*recordset = static_cast<Rty *>(pRecordset); \
		return 0; \
	}

//*/
} //namespace i8desk

#endif TABLE_T_H
