#include "StdAfx.h"
#include "rtdatasvr.h"

#include <algorithm>

#define SELECT_TABLE(tname, param) \
	if (CondMode == SQL_COND_MODE_KEY) { \
		const size_t bufsiz = sizeof(db::t##tname) + sizeof(header) + 256; \
		pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
		CPackageHelper out(pbuf, bufsiz); \
		db::t##tname d; \
		if (0 == m_pTableMgr->Get##tname##Table()->GetData(param, &d, 0)) \
			out << 1L << 1L << d; \
		else \
			out << 1L << 0L; \
		header.Length = out.GetOffset(); \
		out.pushPackageHeader(header); \
	} \
	else { \
		I##tname##Filter *pFilter = m_pTableMgr->Create##tname##Filter(CondMask, CondMode, dwParam1, strParam1); \
		I##tname##Sorter *pSorter = m_pTableMgr->Create##tname##Sorter(mask, desc); \
		I##tname##Recordset *pRecordset = 0; \
		m_pTableMgr->Get##tname##Table()->Select(&pRecordset, pFilter, pSorter); \
		uint32 n = pRecordset->GetCount(); \
		const size_t bufsiz = sizeof(db::t##tname) * n + sizeof(header) + 256; \
		pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
		CPackageHelper out(pbuf, bufsiz); \
		out << 1L << n; \
		for (uint32 i = 0; i < n; i++) { \
			out << *pRecordset->GetData(i); \
		} \
		header.Length = out.GetOffset(); \
		out.pushPackageHeader(header); \
		pRecordset->Release(); \
		m_pTableMgr->Destroy##tname##Filter(pFilter); \
		m_pTableMgr->Destroy##tname##Sorter(pSorter); \
	}

#define SELECT_TABLE_K1(tname, key1) \
	I##tname##Sorter *pSorter = m_pTableMgr->Create##tname##Sorter(mask, desc); \
	I##tname##Recordset *pRecordset = 0; \
	m_pTableMgr->Get##tname##Table()->Select(&pRecordset, key1, pSorter); \
	uint32 n = pRecordset->GetCount(); \
	const size_t bufsiz = sizeof(db::t##tname) * n + sizeof(header) + 256; \
	pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
	CPackageHelper out(pbuf, bufsiz); \
	out << 1L << n; \
	for (uint32 i = 0; i < n; i++) { \
		out << *pRecordset->GetData(i); \
	} \
	header.Length = out.GetOffset(); \
	out.pushPackageHeader(header); \
	pRecordset->Release(); \
	m_pTableMgr->Destroy##tname##Sorter(pSorter); 

#define SELECT_TABLE_K1_K2(tname, key1, key2) \
	const size_t bufsiz = sizeof(db::t##tname) + sizeof(header) + 256; \
	pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
	CPackageHelper out(pbuf, bufsiz); \
	db::t##tname d; \
	if (0 == m_pTableMgr->Get##tname##Table()->GetData(key1, key2, &d, 0)) \
		out << 1L << 1L << d; \
	else \
		out << 1L << 0L; \
	header.Length = out.GetOffset(); \
	out.pushPackageHeader(header);

#define SELECT_TABLE_FILTER(tname) \
	I##tname##Filter *pFilter = m_pTableMgr->Create##tname##Filter(CondMask, CondMode, dwParam1, strParam1); \
	I##tname##Sorter *pSorter = m_pTableMgr->Create##tname##Sorter(mask, desc); \
	I##tname##Recordset *pRecordset = 0; \
	m_pTableMgr->Get##tname##Table()->Select(&pRecordset, pFilter, pSorter); \
	uint32 n = pRecordset->GetCount(); \
	const size_t bufsiz = sizeof(db::t##tname) * n + sizeof(header) + 256; \
	pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
	CPackageHelper out(pbuf, bufsiz); \
	out << 1L << n; \
	for (uint32 i = 0; i < n; i++) { \
		out << *pRecordset->GetData(i); \
	} \
	header.Length = out.GetOffset(); \
	out.pushPackageHeader(header); \
	pRecordset->Release(); \
	m_pTableMgr->Destroy##tname##Filter(pFilter); \
	m_pTableMgr->Destroy##tname##Sorter(pSorter); 

namespace i8desk
{

	void CRTDataSvr::ResponseRecordset(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header)
	{
		uint32 table;
		
		//条件
		uint64 CondMask; //条件字段：1-匹配主键，CondMode无效; 其他为相应字段
		uint32 CondMode; //0:无条件, 1:<>, 2:>, 3:<, 4:>=, 5:<=, 6:NULL, 7:NOT NULL，8:=

		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;

		//排序
		uint64 mask; //排序字段
		uint32 desc; //排序方式

		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask >> desc;

		char *pbuf = 0;
		int ret = 0;
		switch (table) {
		case FLAG_GAME_BIT: {
			//SELECT_TABLE(Game, dwParam1)
			if (CondMode == SQL_COND_MODE_KEY) { 
				const size_t bufsiz = sizeof(db::tGame) + sizeof(header) + 256; 
				pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); 
				CPackageHelper out(pbuf, bufsiz); 
				db::tGame d; 
				if (0 == m_pTableMgr->GetGameTable()->GetData(dwParam1, &d, 0)) 
				out << 1L << 1L << d; 
			else 
			out << 1L << 0L; 
			header.Length = out.GetOffset(); 
			out.pushPackageHeader(header); 
			} 
			else { 
			IGameFilter *pFilter = m_pTableMgr->CreateGameFilter(CondMask, CondMode, dwParam1, strParam1); 
			IGameSorter *pSorter = m_pTableMgr->CreateGameSorter(mask, desc); 
			IGameRecordset *pRecordset = 0; 
			m_pTableMgr->GetGameTable()->Select(&pRecordset, pFilter, pSorter); 
			uint32 n = pRecordset->GetCount(); 
			const size_t bufsiz = sizeof(db::tGame) * n + sizeof(header) + 256; 
			pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); 

			CPackageHelper out(pbuf, bufsiz); 
			out << 1L << n; 
			if (pFilter != NULL)
			{
				for (uint32 i = 0; i < n; i++) { 
					out << *pRecordset->GetData(i); 
				} 
			} 
			else
			{
				for (uint32 i = 0; i < n; i++) { 
					db::tGame *d=pRecordset->GetData(i); 
					out.pushDWORD(d->GID);
					out.pushDWORD(d->PID);
					out.pushString(d->Name,strlen(d->Name));
					out.pushString(d->DefClass,strlen(d->DefClass));
					out.pushString(d->GameExe,strlen(d->GameExe));
					out.pushString(d->Param,strlen(d->Param));
					out.pushDWORD(d->Size);
					out.pushString(d->URL,strlen(d->URL));
					out.pushDWORD(d->DeskLnk);
					out.pushDWORD(d->Toolbar);
					out.pushString(d->SvrPath,strlen(d->SvrPath));
					out.pushString(d->CliPath,strlen(d->CliPath));
					out.pushDWORD(d->Priority);
					out.pushDWORD(d->DownPriority);
					out.pushDWORD(d->ShowPriority);
					out.pushString(d->MatchFile,strlen(d->MatchFile));
					out.pushString(d->SaveFilter,strlen(d->SaveFilter));
					out.pushDWORD(d->IdcUptDate);
					out.pushDWORD(d->SvrUptDate);
					out.pushDWORD(d->IdcVer);
					out.pushDWORD(d->SvrVer);
					out.pushDWORD(d->AutoUpt);
					out.pushDWORD(d->I8Play);
					out.pushDWORD(d->IdcClick);
					out.pushDWORD(d->SvrClick);
					out.pushDWORD(d->AddDate);
					out.pushDWORD(d->Status);
					out.pushDWORD(d->IfUpdate);
					out.pushDWORD(d->IfDisplay);
					out.pushString(d->Comment,strlen(d->Comment));
					out.pushDWORD(d->Auto);
					out.pushDWORD(d->Declinectr);
					out.pushDWORD(d->hide);
					out.pushString(d->LocalPath,strlen(d->LocalPath));
					out.pushString(d->SaveFilter2,strlen(d->SaveFilter2));
					out.pushString(d->GameExe2,strlen(d->GameExe2));
					out.pushString(d->Param2,strlen(d->Param2));
					out.pushDWORD(d->SvrClick2);
					out.pushString(d->url1,strlen(d->url1));
					out.pushString(d->url2,strlen(d->url2));
					out.pushString(d->url3,strlen(d->url3));
					out.pushString(d->url4,strlen(d->url4));
					out.pushDWORD(d->AutoSync);
					out.pushString(d->GameSource,strlen(d->GameSource));
				} 
			}
			header.Length = out.GetOffset(); 
			out.pushPackageHeader(header); 
			pRecordset->Release(); 
			m_pTableMgr->DestroyGameFilter(pFilter); 
			m_pTableMgr->DestroyGameSorter(pSorter); 
			}
			break; }
		case FLAG_CLASS_BIT: {
			SELECT_TABLE(Class, strParam1.c_str())
			break; }
		case FLAG_AREA_BIT: {
			SELECT_TABLE(Area, strParam1.c_str())
			break; }
		case FLAG_GAMEAREA_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty()) {
					SELECT_TABLE_K1(GameArea, dwParam1)
				}
				else {
					SELECT_TABLE_K1_K2(GameArea, dwParam1, strParam1.c_str())
				}
			}
			else {
				SELECT_TABLE_FILTER(GameArea)
			}
			break; }
		case FLAG_CLIENT_BIT: {
			SELECT_TABLE(Client, strParam1.c_str())
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			SELECT_TABLE(CmpStartTask, strParam1.c_str())
			break; }
		case FLAG_FAVORITE_BIT: {
			SELECT_TABLE(Favorite, strParam1.c_str())
			break; }
		case FLAG_SYSOPT_BIT: {
			SELECT_TABLE(SysOpt, strParam1.c_str())
			break; }
		case FLAG_USER_BIT: {
			SELECT_TABLE(User, strParam1.c_str())
			break; }
		case FLAG_VDISK_BIT: {
			SELECT_TABLE(VDisk, strParam1.c_str())
			break; }
		case FLAG_SYNCTASK_BIT: {
			SELECT_TABLE(SyncTask, strParam1.c_str())
			break; }
		case FLAG_SYNCGAME_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0) {
					SELECT_TABLE_K1(SyncGame, strParam1.c_str())
				}
				else {
					SELECT_TABLE_K1_K2(SyncGame, strParam1.c_str(), dwParam1)
				}
			}
			else {
				SELECT_TABLE_FILTER(SyncGame)
			}
			break; }
		default:
			break;
		}

		if (ret) {
			I8_ASSERT(pbuf == 0);
			NAK_RETURN("SQL操作失败");
		}
		pNetLayer->SendPackage(sck, pbuf, header.Length);
	}

	void CRTDataSvr::InsertRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;
		uint64 mask; //需要处理字段集
		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask;

		int ret = -1;
		switch (table) {
		case FLAG_GAME_BIT: {
			db::tGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetGameTable()->Insert( &d );
			break; }
		case FLAG_CLASS_BIT: {
			db::tClass d;
			inpackage >> d;
			ret = m_pTableMgr->GetClassTable()->Insert( &d );
			break; }
		case FLAG_AREA_BIT: {
			db::tArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetAreaTable()->Insert( &d );
			break; }
		case FLAG_GAMEAREA_BIT: {
			db::tGameArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetGameAreaTable()->Insert( &d );
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			ret = m_pTableMgr->GetClientTable()->Insert( &d );
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			db::tCmpStartTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetCmpStartTaskTable()->Insert( &d );
			break; }
		case FLAG_FAVORITE_BIT: {
			db::tFavorite d;
			inpackage >> d;
			ret = m_pTableMgr->GetFavoriteTable()->Insert( &d );
			break; }
		case FLAG_SYSOPT_BIT: {
			db::tSysOpt d;
			inpackage >> d;
			ret = m_pTableMgr->GetSysOptTable()->Insert( &d );
			break; }
		case FLAG_USER_BIT: {
			db::tUser d;
			inpackage >> d;
			ret = m_pTableMgr->GetUserTable()->Insert( &d );
			break; }
		case FLAG_VDISK_BIT: {
			db::tVDisk d;
			inpackage >> d;
			ret = m_pTableMgr->GetVDiskTable()->Insert( &d );
			break; }
		case FLAG_SYNCTASK_BIT: {
			db::tSyncTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncTaskTable()->Insert( &d );
			break; }
		case FLAG_SYNCGAME_BIT: {
			db::tSyncGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncGameTable()->Insert( &d );
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN("SQL操作失败");
		ACK();
	}

	void CRTDataSvr::UpdateRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;
		uint64 mask; //需要处理字段集
		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask;

		int ret = -1;
		switch (table) {
		case FLAG_GAME_BIT: {
			db::tGame d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetGameTable()->Update(dwParam1, &d, mask);
			}
			else {
				IGameFilter *pFilter = m_pTableMgr->CreateGameFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyGameFilter(pFilter);
			}
			break; }
		case FLAG_CLASS_BIT: {
			db::tClass d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClassTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IClassFilter *pFilter = m_pTableMgr->CreateClassFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClassTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyClassFilter(pFilter);
			}
			break; }
		case FLAG_AREA_BIT: {
			db::tArea d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetAreaTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IAreaFilter *pFilter = m_pTableMgr->CreateAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetAreaTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyAreaFilter(pFilter);
			}
			break; }
		case FLAG_GAMEAREA_BIT: {
			db::tGameArea d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetGameAreaTable()->Update(dwParam1, &d, mask);
				else 
					ret = m_pTableMgr->GetGameAreaTable()->Update(dwParam1, strParam1.c_str(), &d, mask);
			}
			else {
				IGameAreaFilter *pFilter = m_pTableMgr->CreateGameAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameAreaTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyGameAreaFilter(pFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClientTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClientTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			db::tCmpStartTask d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpStartTaskTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				ICmpStartTaskFilter *pFilter = m_pTableMgr->CreateCmpStartTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpStartTaskTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyCmpStartTaskFilter(pFilter);
			}
			break; }
		case FLAG_FAVORITE_BIT: {
			db::tFavorite d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetFavoriteTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IFavoriteFilter *pFilter = m_pTableMgr->CreateFavoriteFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetFavoriteTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyFavoriteFilter(pFilter);
			}
			break; }
		case FLAG_SYSOPT_BIT: {
			db::tSysOpt d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetSysOptTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				ISysOptFilter *pFilter = m_pTableMgr->CreateSysOptFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSysOptTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroySysOptFilter(pFilter);
			}
			break; }
		case FLAG_USER_BIT: {
			db::tUser d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetUserTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IUserFilter *pFilter = m_pTableMgr->CreateUserFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetUserTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyUserFilter(pFilter);
			}
			break; }
		case FLAG_VDISK_BIT: {
			db::tVDisk d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetVDiskTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IVDiskFilter *pFilter = m_pTableMgr->CreateVDiskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetVDiskTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyVDiskFilter(pFilter);
			}
			break; }
		case FLAG_SYNCTASK_BIT: {
			db::tSyncTask d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetSyncTaskTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				ISyncTaskFilter *pFilter = m_pTableMgr->CreateSyncTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSyncTaskTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroySyncTaskFilter(pFilter);
			}
			break; }
		case FLAG_SYNCGAME_BIT: {
			db::tSyncGame d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0)
					ret = m_pTableMgr->GetSyncGameTable()->Update(strParam1.c_str(), &d, mask);
				else 
					ret = m_pTableMgr->GetSyncGameTable()->Update(strParam1.c_str(), dwParam1, &d, mask);
			}
			else {
				ISyncGameFilter *pFilter = m_pTableMgr->CreateSyncGameFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSyncGameTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroySyncGameFilter(pFilter);
			}
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN("SQL操作失败");
		ACK();
	}

	void CRTDataSvr::DeleteRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;
		uint64 mask; //需要处理字段集
		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask;

		int ret = -1;
		switch (table) {
		case FLAG_GAME_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetGameTable()->Delete(dwParam1);
			}
			else {
				IGameFilter *pFilter = m_pTableMgr->CreateGameFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameTable()->Delete(pFilter);
				m_pTableMgr->DestroyGameFilter(pFilter);
			}
			break; }
		case FLAG_CLASS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClassTable()->Delete(strParam1.c_str());
			}
			else {
				IClassFilter *pFilter = m_pTableMgr->CreateClassFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClassTable()->Delete(pFilter);
				m_pTableMgr->DestroyClassFilter(pFilter);
			}
			break; }
		case FLAG_AREA_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetAreaTable()->Delete(strParam1.c_str());
			}
			else {
				IAreaFilter *pFilter = m_pTableMgr->CreateAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetAreaTable()->Delete(pFilter);
				m_pTableMgr->DestroyAreaFilter(pFilter);
			}
			break; }
		case FLAG_GAMEAREA_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetGameAreaTable()->Delete(dwParam1);
				else
					ret = m_pTableMgr->GetGameAreaTable()->Delete(dwParam1, strParam1.c_str());
			}
			else {
				struct DeleteGameArea
					: public IGameAreaFilter 
				{
				private:
					LPCTSTR m_pParameter;

				public:
					DeleteGameArea(LPCTSTR pParameter)
						: m_pParameter(pParameter)
					{
					}

					virtual bool bypass(const db::tGameArea *const c)
					{
						if( strncmp(c->AID, m_pParameter, MAX_GUID_LEN) == 0 )
							return false;
						else
							return true;
					}
				};
				DeleteGameArea deleteGameAreaFilter(strParam1.c_str());
				//IGameAreaFilter *pFilter = m_pTableMgr->CreateGameAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameAreaTable()->Delete(&deleteGameAreaFilter);
				//m_pTableMgr->DestroyGameAreaFilter(pFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClientTable()->Delete(strParam1.c_str());
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClientTable()->Delete(pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpStartTaskTable()->Delete(strParam1.c_str());
			}
			else {
				ICmpStartTaskFilter *pFilter = m_pTableMgr->CreateCmpStartTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpStartTaskTable()->Delete(pFilter);
				m_pTableMgr->DestroyCmpStartTaskFilter(pFilter);
			}
			break; }
		case FLAG_FAVORITE_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetFavoriteTable()->Delete(strParam1.c_str());
			}
			else {
				IFavoriteFilter *pFilter = m_pTableMgr->CreateFavoriteFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetFavoriteTable()->Delete(pFilter);
				m_pTableMgr->DestroyFavoriteFilter(pFilter);
			}
			break; }
		case FLAG_SYSOPT_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetSysOptTable()->Delete(strParam1.c_str());
			}
			else {
				ISysOptFilter *pFilter = m_pTableMgr->CreateSysOptFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSysOptTable()->Delete(pFilter);
				m_pTableMgr->DestroySysOptFilter(pFilter);
			}
			break; }
		case FLAG_USER_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetUserTable()->Delete(strParam1.c_str());
			}
			else {
				IUserFilter *pFilter = m_pTableMgr->CreateUserFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetUserTable()->Delete(pFilter);
				m_pTableMgr->DestroyUserFilter(pFilter);
			}
			break; }
		case FLAG_VDISK_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetVDiskTable()->Delete(strParam1.c_str());
			}
			else {
				IVDiskFilter *pFilter = m_pTableMgr->CreateVDiskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetVDiskTable()->Delete(pFilter);
				m_pTableMgr->DestroyVDiskFilter(pFilter);
			}
			break; }
		case FLAG_SYNCTASK_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetSyncTaskTable()->Delete(strParam1.c_str());
			}
			else {
				ISyncTaskFilter *pFilter = m_pTableMgr->CreateSyncTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSyncTaskTable()->Delete(pFilter);
				m_pTableMgr->DestroySyncTaskFilter(pFilter);
			}
			break; }
		case FLAG_SYNCGAME_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0)
					ret = m_pTableMgr->GetSyncGameTable()->Delete(strParam1.c_str());
				else 
					ret = m_pTableMgr->GetSyncGameTable()->Delete(strParam1.c_str(), dwParam1);
			}
			else {
				ISyncGameFilter *pFilter = m_pTableMgr->CreateSyncGameFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSyncGameTable()->Delete(pFilter);
				m_pTableMgr->DestroySyncGameFilter(pFilter);
			}
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN("SQL操作失败");
		ACK();
	}

	void CRTDataSvr::EnsureRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage,_packageheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;
		uint64 mask; //需要处理字段集
		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask;

		int ret = -1;
		switch (table) {
		case FLAG_GAME_BIT: {
			db::tGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetGameTable()->Ensure( &d, mask  );
			break; }
		case FLAG_CLASS_BIT: {
			db::tClass d;
			inpackage >> d;
			ret = m_pTableMgr->GetClassTable()->Ensure( &d, mask  );
			break; }
		case FLAG_AREA_BIT: {
			db::tArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetAreaTable()->Ensure( &d, mask  );
			break; }
		case FLAG_GAMEAREA_BIT: {
			db::tGameArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetGameAreaTable()->Ensure( &d, mask  );
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			ret = m_pTableMgr->GetClientTable()->Ensure( &d, mask  );
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			db::tCmpStartTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetCmpStartTaskTable()->Ensure( &d, mask  );
			break; }
		case FLAG_FAVORITE_BIT: {
			db::tFavorite d;
			inpackage >> d;
			ret = m_pTableMgr->GetFavoriteTable()->Ensure( &d, mask  );
			break; }
		case FLAG_SYSOPT_BIT: {
			db::tSysOpt d;
			inpackage >> d;
			ret = m_pTableMgr->GetSysOptTable()->Ensure( &d, mask  );
			break; }
		case FLAG_USER_BIT: {
			db::tUser d;
			inpackage >> d;
			ret = m_pTableMgr->GetUserTable()->Ensure( &d, mask  );
			break; }
		case FLAG_VDISK_BIT: {
			db::tVDisk d;
			inpackage >> d;
			ret = m_pTableMgr->GetVDiskTable()->Ensure( &d, mask  );
			break; }
		case FLAG_SYNCTASK_BIT: {
			db::tSyncTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncTaskTable()->Ensure( &d, mask  );
			break; }
		case FLAG_SYNCGAME_BIT: {
			db::tSyncGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncGameTable()->Ensure( &d, mask  );
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN("SQL操作失败");
		ACK();
	}

	
	void CRTDataSvr::ComulateRecord(INetLayer *pNetLayer, SOCKET sck, CPackageHelper &inpackage, _packageheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		std::string strParam1, strParam2;
		uint64 mask; //需要处理字段集
		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask;

		int ret = -1;
		switch (table) {
		case FLAG_GAME_BIT: {
			db::tGame d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetGameTable()->Cumulate(dwParam1, &d, mask);
			}
			else {
				IGameFilter *pFilter = m_pTableMgr->CreateGameFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyGameFilter(pFilter);
			}
			break; }
		case FLAG_CLASS_BIT: {
			db::tClass d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClassTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IClassFilter *pFilter = m_pTableMgr->CreateClassFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClassTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyClassFilter(pFilter);
			}
			break; }
		case FLAG_AREA_BIT: {
			db::tArea d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetAreaTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IAreaFilter *pFilter = m_pTableMgr->CreateAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetAreaTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyAreaFilter(pFilter);
			}
			break; }
		case FLAG_GAMEAREA_BIT: {
			db::tGameArea d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetGameAreaTable()->Cumulate(dwParam1, &d, mask);
				else 
					ret = m_pTableMgr->GetGameAreaTable()->Cumulate(dwParam1, strParam1.c_str(), &d, mask);
			}
			else {
				IGameAreaFilter *pFilter = m_pTableMgr->CreateGameAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetGameAreaTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyGameAreaFilter(pFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetClientTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetClientTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPSTARTTASK_BIT: {
			db::tCmpStartTask d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpStartTaskTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				ICmpStartTaskFilter *pFilter = m_pTableMgr->CreateCmpStartTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpStartTaskTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyCmpStartTaskFilter(pFilter);
			}
			break; }
		case FLAG_FAVORITE_BIT: {
			db::tFavorite d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetFavoriteTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IFavoriteFilter *pFilter = m_pTableMgr->CreateFavoriteFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetFavoriteTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyFavoriteFilter(pFilter);
			}
			break; }
		case FLAG_SYSOPT_BIT: {
			db::tSysOpt d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetSysOptTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				ISysOptFilter *pFilter = m_pTableMgr->CreateSysOptFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSysOptTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroySysOptFilter(pFilter);
			}
			break; }
		case FLAG_USER_BIT: {
			db::tUser d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetUserTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IUserFilter *pFilter = m_pTableMgr->CreateUserFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetUserTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyUserFilter(pFilter);
			}
			break; }
		case FLAG_VDISK_BIT: {
			db::tVDisk d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetVDiskTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IVDiskFilter *pFilter = m_pTableMgr->CreateVDiskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetVDiskTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyVDiskFilter(pFilter);
			}
			break; }
		case FLAG_SYNCTASK_BIT: {
			I8_ASSERT(0);
			break; }
		case FLAG_SYNCGAME_BIT: {
			I8_ASSERT(0);
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN("SQL操作失败");
		ACK();
	}

	void CRTDataSvr::Cmd_Game_CliExeSql(INetLayer*pNetLayer, SOCKET sck, LPCSTR lpPackage,CPackageHelper &inpackage,_packageheader &header)
	{
		I8_ASSERT(0);
	}

	
} //namespace i8desk

