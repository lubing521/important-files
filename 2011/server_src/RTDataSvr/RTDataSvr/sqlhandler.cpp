#include "StdAfx.h"
#include "rtdatasvr.h"
#include "tptables.h"
#include "../../../include/Win32/System/cpu.h"
#include "../../../include/PluginInfo.h"
#include "../../../include/UpdateGame.h"
#include "../../../include/Serialize/Serialize.hpp"
#include <algorithm>

#define SELECT_TABLE(tname, param) \
	if (CondMode == SQL_COND_MODE_KEY) { \
		const size_t bufsiz = sizeof(db::t##tname) + sizeof(header) + 256; \
		pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); \
		db::t##tname d; \
		if (0 == m_pTableMgr->Get##tname##Table()->GetData(param, &d, 0)) \
			out << 1L << 1L << d; \
		else \
			out << 1L << 0L; \
		header.Length = out.GetLength();\
	} \
	else { \
		I##tname##Filter *pFilter = m_pTableMgr->Create##tname##Filter(CondMask, CondMode, dwParam1, strParam1); \
		I##tname##Sorter *pSorter = m_pTableMgr->Create##tname##Sorter(mask, desc); \
		I##tname##Recordset *pRecordset = 0; \
		m_pTableMgr->Get##tname##Table()->Select(&pRecordset, pFilter, pSorter); \
		uint32 n = pRecordset->GetCount(); \
		const size_t bufsiz = sizeof(db::t##tname) * n + sizeof(header) + 256; \
		pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
		CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); \
		out << 1L << n; \
		for (uint32 i = 0; i < n; i++) { \
			out << *pRecordset->GetData(i); \
		} \
		header.Length = out.GetLength();\
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
	CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); \
	out << 1L << n; \
	for (uint32 i = 0; i < n; i++) { \
		out << *pRecordset->GetData(i); \
	} \
	header.Length = out.GetLength();\
	pRecordset->Release(); \
	m_pTableMgr->Destroy##tname##Sorter(pSorter); 

#define SELECT_TABLE_K1_K2(tname, key1, key2) \
	const size_t bufsiz = sizeof(db::t##tname) + sizeof(header) + 256; \
	pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
	CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); \
	db::t##tname d; \
	if (0 == m_pTableMgr->Get##tname##Table()->GetData(key1, key2, &d, 0)) \
		out << 1L << 1L << d; \
	else \
		out << 1L << 0L; \
	header.Length = out.GetLength();

#define SELECT_TABLE_FILTER(tname) \
	I##tname##Filter *pFilter = m_pTableMgr->Create##tname##Filter(CondMask, CondMode, dwParam1, strParam1); \
	I##tname##Sorter *pSorter = m_pTableMgr->Create##tname##Sorter(mask, desc); \
	I##tname##Recordset *pRecordset = 0; \
	m_pTableMgr->Get##tname##Table()->Select(&pRecordset, pFilter, pSorter); \
	uint32 n = pRecordset->GetCount(); \
	const size_t bufsiz = sizeof(db::t##tname) * n + sizeof(header) + 256; \
	pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); \
	CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); \
	out << 1L << n; \
	for (uint32 i = 0; i < n; i++) { \
		out << *pRecordset->GetData(i); \
	} \
	header.Length = out.GetLength();\
	pRecordset->Release(); \
	m_pTableMgr->Destroy##tname##Filter(pFilter); \
	m_pTableMgr->Destroy##tname##Sorter(pSorter); 

namespace i8desk
{
	utility::serialize::Serialize &operator>>(utility::serialize::Serialize &os, 
		db::tSyncTaskStatus &val)
	{
		os >> val.SID >> val.GID >> val.NameID >> val.GameName >> val.SvrDir >> val.NodeDir
			>> val.UpdateState >> val.TransferRate >> val.BytesTotal >> val.UpdateSize >> val.BytesTransferred;
		return os;
	}

	void CRTDataSvr::ResponseRecordset(INetLayer *pNetLayer, SOCKET sck,  LPCSTR lpPackage,CPkgHelper &inpackage,pkgheader &header)
	{
		uint32 table;
		
		//条件
		uint64 CondMask; //条件字段：1-匹配主键，CondMode无效; 其他为相应字段
		uint32 CondMode; //0:无条件, 1:<>, 2:>, 3:<, 4:>=, 5:<=, 6:NULL, 7:NOT NULL，8:=

		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;

		//排序
		uint64 mask; //排序字段
		uint32 desc; //排序方式
		ISvrPlug *gamePlug = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);

		inpackage >> table >> CondMode >> CondMask 
			>> dwParam1 >> dwParam2 >> strParam1 >> strParam2 
			>> mask >> desc;

		char *pbuf = 0;
		int ret = 0;
		switch (table) {
		case FLAG_GAME_BIT: {
			//SELECT_TABLE(Game, dwParam1)
			if (CondMode == SQL_COND_MODE_KEY) 
			{ 
				const size_t bufsiz = sizeof(db::tGame) + sizeof(header) + 256; 
				pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); 
				CPkgHelper out(pbuf, bufsiz, header.Cmd,OBJECT_UNKOWN); 
				db::tGame d; 
				if (0 == m_pTableMgr->GetGameTable()->GetData(dwParam1, &d, 0)) 
					out << 1L << 1L << d; 
				else 
				{
					out << 1L << 0L;
				}

				header.Length = out.GetLength(); 
				out.pushHeader(header); 
			} 
			else 
			{ 
			IGameFilter *pFilter = m_pTableMgr->CreateGameFilter(CondMask, CondMode, dwParam1, strParam1); 
			IGameSorter *pSorter = m_pTableMgr->CreateGameSorter(mask, desc); 
			IGameRecordset *pRecordset = 0; 
			m_pTableMgr->GetGameTable()->Select(&pRecordset, pFilter, pSorter); 
			uint32 n = pRecordset->GetCount(); 
			const size_t bufsiz = sizeof(db::tGame) * n + sizeof(header) + 256; 
			pbuf = pNetLayer->GetIMemPool()->Alloc(bufsiz); 

			CPkgHelper out(pbuf, bufsiz,header.Cmd,OBJECT_UNKOWN); 
			DWORD *pCount = (DWORD *)(pbuf + sizeof(pkgheader) + 4);
			out << 1L << 0; 
			if (pFilter != NULL)
			{
				for (uint32 i = 0; i < n; i++) { 
					db::tGame *game = pRecordset->GetData(i);
					if( game->Hide == i8desk::Hide)
						continue;

					(*pCount)++;
					out << *game; 
				}
			} 
			else
			{
				for (uint32 i = 0; i < n; i++) { 
					db::tGame *d=pRecordset->GetData(i); 
					if(d->Hide == i8desk::Hide)
						continue;
					(*pCount)++;
					out.Push(d->GID);
					out.Push(d->PID);
					out.PushString(d->CID,_tcslen(d->CID));
					out.PushString(d->Name,_tcslen(d->Name));
					out.PushString(d->Exe,_tcslen(d->Exe));
					out.PushString(d->Param,_tcslen(d->Param));
					out.Push(d->Size);
					out.PushString(d->SvrPath,_tcslen(d->SvrPath));
					out.PushString(d->CliPath,_tcslen(d->CliPath));
					out.PushString(d->TraitFile,_tcslen(d->TraitFile));
					out.PushString(d->SaveFilter,_tcslen(d->SaveFilter));

					out.Push(d->DeskLink);
					out.Push(d->Toolbar);
					out.PushString(d->Memo,_tcslen(d->Memo));
					out.PushString(d->GameSource,_tcslen(d->GameSource));
					out.Push(d->IdcAddDate);

					out.Push(d->EnDel);
					out.Push(d->IdcVer);
					out.Push(d->SvrVer);
					out.Push(d->IdcClick);
					out.Push(d->SvrClick);
					out.Push(d->SvrClick2);
					out.Push(d->I8Play);
					out.Push(d->AutoUpt);
					out.Push(d->Priority);
					out.Push(d->Force);
					out.PushString(d->ForceDir,_tcslen(d->ForceDir));
					out.Push(d->Status);
					out.Push(d->StopRun);
				} 
			}
			header.Length = out.GetLength(); 
			out.pushHeader(header); 
			pRecordset->Release(); 
			m_pTableMgr->DestroyGameFilter(pFilter); 
			m_pTableMgr->DestroyGameSorter(pSorter); 
			}
			break; }
		case FLAG_PLUGTOOL_BIT: {
			SELECT_TABLE(PlugTool, dwParam1)
				break; }
		case FLAG_CLASS_BIT: {
			SELECT_TABLE(Class, strParam1.c_str())
			break; }
		case FLAG_AREA_BIT: {
			SELECT_TABLE(Area, strParam1.c_str())
			break; }
		case FLAG_RUNTYPE_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty()) {
					SELECT_TABLE_K1(RunType, dwParam1)
				}
				else {
					SELECT_TABLE_K1_K2(RunType, dwParam1, strParam1.c_str())
				}
			}
			else {
				SELECT_TABLE_FILTER(RunType)
			}
			break; }
		case FLAG_CLIENT_BIT: {
			SELECT_TABLE(Client, strParam1.c_str())
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			SELECT_TABLE(CmpBootTask, strParam1.c_str())
			break; }
		case FLAG_FAVORITE_BIT: {
			SELECT_TABLE(Favorite, strParam1.c_str())
			break; }
		case FLAG_SYSOPT_BIT: {
			SELECT_TABLE(SysOpt, strParam1.c_str())
			break; }
		case FLAG_VDISK_BIT: {
			SELECT_TABLE(VDisk, strParam1.c_str())
			break; }
		case FLAG_SYNCTASK_BIT: {
			SELECT_TABLE(SyncTask, strParam1.c_str())
			break; }
		case FLAG_SERVER_BIT: {
			SELECT_TABLE(Server, strParam1.c_str())
				break; }
		case FLAG_BOOTTASKAREA_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.c_str() == 0) {
					SELECT_TABLE_K1(BootTaskArea, strParam1.c_str())
				}
				else {
					SELECT_TABLE_K1_K2(BootTaskArea, strParam1.c_str(), strParam2.c_str())
				}
			}
			else {
				SELECT_TABLE_FILTER(BootTaskArea)
			}
			break; }
		case FLAG_VDISKSTATUS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0) {
					SELECT_TABLE_K1(VDiskStatus, strParam1.c_str())
				}
				else {
					SELECT_TABLE_K1_K2(VDiskStatus, strParam1.c_str(), dwParam2)
				}
			}
			else {
				SELECT_TABLE_FILTER(VDiskStatus)
			}
			break; }
		case FLAG_MODULEUSAGE_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0) {
					SELECT_TABLE_K1(ModuleUsage, strParam1.c_str())
				}
				else {
					SELECT_TABLE_K1_K2(ModuleUsage, strParam1.c_str(), dwParam2)
				}
			}
			else {
				SELECT_TABLE_FILTER(ModuleUsage)
			}
			break; }
		case FLAG_SERVERSTATUS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
	/*			if (dwParam1 == 0) 
					SELECT_TABLE_K1(ServerStatus, strParam1.c_str())*/
			}
			else {
				EnsureStatus(table);
				SELECT_TABLE_FILTER(ServerStatus)
			}
			break; }
		case FLAG_DISKSTATUS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0) {
					SELECT_TABLE_K1(DiskStatus, strParam1.c_str())
				}
				else {
					EnsureStatus(table);
					SELECT_TABLE_K1_K2(DiskStatus, strParam1.c_str(), dwParam2)
				}
			}
			else {
				SELECT_TABLE_FILTER(DiskStatus)
			}
			break; }
		case FLAG_VDISKCLIENT_BIT: 
			if (CondMode != SQL_COND_MODE_KEY)
			{
				SELECT_TABLE_FILTER(VDiskClient)
			}
			break;
		case FLAG_TASKSTATUS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				/*if (dwParam1 == 0) 
					SELECT_TABLE_K1(TaskStatus, strParam1.c_str())*/
			}
			else {
				EnsureStatus(table);

				SELECT_TABLE_FILTER(TaskStatus)
			}
			break; }
		case FLAG_UPDATEGAMESTATUS_BIT: {
			if (CondMode != SQL_COND_MODE_KEY) 
			{
				EnsureStatus(table);
				SELECT_TABLE_FILTER(UpdateGameStatus)
			}
			break; }
		case FLAG_SYNCGAMESTATUS_BIT: {
			if (CondMode != SQL_COND_MODE_KEY) 
			{
				EnsureStatus(table);
				SELECT_TABLE_FILTER(UpdateGameStatus)
			}
			break; }
		case FLAG_PUSHGAMESTATUS_BIT: {
			if (CondMode != SQL_COND_MODE_KEY) 
			{
				EnsureStatus(table);
				SELECT_TABLE_FILTER(UpdateGameStatus)
			}
			break; }
		case FLAG_PLUGTOOLSTATUS_BIT: 
			{
				if (CondMode != SQL_COND_MODE_KEY) 
				{
					EnsureStatus(table);
					SELECT_TABLE_FILTER(PlugToolStatus)
				}
				break; 
			}
		case FLAG_SYNCGAME_BIT: 
			{
				if (CondMode == SQL_COND_MODE_KEY) 
				{
					if (strParam1.empty()) 
					{
						SELECT_TABLE_K1(SyncGame, dwParam1)
					}
					else 
					{
						SELECT_TABLE_K1_K2(SyncGame, dwParam1, strParam1.c_str())
					}
				}
				else 
				{
					SELECT_TABLE_FILTER(SyncGame)
				}
				break; 
			}
		case FLAG_SYNCTASKSTATUS_BIT: 
			{
				
					ISvrPlug *updateSvrPlug = m_pPlugMgr->FindPlug(PLUG_UPTSVR_NAME);
					if(updateSvrPlug == NULL) 
						return;

					SELECT_TABLE_K1(SyncTaskStatus, strParam1.c_str())
				
				break; 
			}
		case FLAG_PUSHGAMESTATIC_BIT: {
			SELECT_TABLE(PushGameStatic, dwParam1)
				break; }
		default:
			NAK_RETURN(_T("不可识别的表名称"));
			break;
		}

		if (ret) {
			assert(pbuf == 0);
			NAK_RETURN(_T("SQL操作失败"));
		}
		pNetLayer->SendPackage(sck, (char*)pbuf, header.Length );
	}

	void CRTDataSvr::EnsureStatus(uint32 table)
	{
		switch(table)
		{
		case FLAG_SERVERSTATUS_BIT:{
			//刷新本服务器状态信息
			db::tServerStatus d ;

			IServerRecordset *pServerRst = 0;
			m_pTableMgr->GetServerTable()->Select(&pServerRst,0,0);
			for( uint32 i = 0; i < pServerRst->GetCount(); ++i )
			{
				db::tServer *server = pServerRst->GetData(i);
				if( server->SvrType == MainServer )
				{
					utility::Strcpy(d.SvrID, server->SvrID);
					utility::Strcpy(d.SvrName, server->SvrName);
					d.SvrType = server->SvrType;
					d.I8DeskSvr = 1;
					break;
				}
			}
			pServerRst->Release();

			d.CPUUtilization	= i8desk_GetCpuUsage();
			d.MemoryUsage		= i8desk_GetMemoryUsage();
			d.LastBackupDBTime	= i8desk_GetDbLastBackupTime();

			if( ISvrPlug *UpdatePlug = m_pPlugMgr->FindPlug(PLUG_UPTSVR_NAME) ) 
				d.I8UpdateSvr = 1;
			else 
				d.I8UpdateSvr = -1;

			m_pTableMgr->GetServerStatusTable()->Ensure(&d, 
				MASK_TSERVERSTATUS_I8DESKSVR
				| MASK_TSERVERSTATUS_SVRTYPE
				| MASK_TSERVERSTATUS_SVRNAME
				| MASK_TSERVERSTATUS_CPUUTILIZATION 
				| MASK_TSERVERSTATUS_MEMORYUSAGE
				| MASK_TSERVERSTATUS_LASTBACKUPDBTIME
				| MASK_TSERVERSTATUS_I8UPDATESVR
				//| MASK_TSERVERSTATUS_DEADCOUNTER //本机的服务状态监控的计数只针对虚拟盘服务
				);
			
			}
			break;
		case FLAG_VDISKCLIENT_BIT:
			{
			
			}
			break;
		case FLAG_DISKSTATUS_BIT:{
				ISvrPlug *gamePlug = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);
				if(gamePlug == NULL) return;
				assert(gamePlug);
				if( gamePlug )
				{
					gamePlug->SendMessage(UPDATE_DISK, 0, 0);
				}
			}
			break;
		case FLAG_TASKSTATUS_BIT:{
			
			ISvrPlug *gamePlug = m_pPlugMgr->FindPlug(PLUG_GAMEMGR_NAME);
			if(gamePlug == NULL) return;
			TaskInfo *tmp = reinterpret_cast<TaskInfo *>(gamePlug->SendMessage(DOWNLOAD_STATUS, 0, 0));
			m_pTableMgr->GetTaskStatusTable()->DeleteAll();
			size_t index = 0;
			while(true)
			{
				const TaskInfo &taskInfo = *(tmp + index++);
				db::tTaskStatus d;
				// GID 为0则结束
				if( taskInfo.gid_ == 0 )
					break;
				d.GID = taskInfo.gid_;
				d.BytesTotal = taskInfo.bytesTotal_;
				d.BytesTransferred = taskInfo.bytesTransferred_;
				d.Connect = taskInfo.connections_;
				d.DownloadState = taskInfo.downloadState_;
				d.Progress = taskInfo.progress_;
				d.State = taskInfo.state_;
				d.TaskType = taskInfo.type_;
				d.TimeElapsed = taskInfo.timeElapsed_;
				d.TimeLeft = taskInfo.timeLeft_;
				d.TransferRate = taskInfo.transferRate_;
				d.State = taskInfo.state_;
				d.UpdateSize = taskInfo.updateSize_;
				
				m_pTableMgr->GetTaskStatusTable()->Insert( &d );
			}

			CoTaskMemFree(tmp);

		}
			break;
		case FLAG_UPDATEGAMESTATUS_BIT:
			{
				int type = Update; // 内网更新
				WriteUpdateGameStatus_(type);
			}
			break;
		case FLAG_PUSHGAMESTATUS_BIT:
			{
				int type = Push; // 游戏推送
				WriteUpdateGameStatus_(type);
			}
			break;
		case FLAG_SYNCGAMESTATUS_BIT:
			{
				int type = Sync;	// 游戏同步
				WriteUpdateGameStatus_(type);
			}
			break;
		case FLAG_PLUGTOOLSTATUS_BIT:
			{
				ISvrPlug *pPlugtool = m_pPlugMgr->FindPlug(PLUG_PLUGTOOL_NAME);
				if(pPlugtool == NULL) return;
				assert(pPlugtool);
				db::tPlugToolStatus *tmp = reinterpret_cast<db::tPlugToolStatus *>(pPlugtool->SendMessage(Status,0,0));
				if(tmp == NULL) return;
				m_pTableMgr->GetPlugToolStatusTable()->DeleteAll();
				size_t index = 0;
				while(true)
				{
					const db::tPlugToolStatus &Plugtool = *(tmp + index++);
					db::tPlugToolStatus d;
					// GID 为0则结束
					if( Plugtool.PID == 0 )
						break;
					d.PID = Plugtool.PID;
					utility::Strcpy(d.CID,Plugtool.CID);
					d.Size = Plugtool.Size;
					d.TransferRate = Plugtool.TransferRate;
					d.AvgTransferRate = Plugtool.AvgTransferRate;
					d.TotalTime = Plugtool.TotalTime;
					d.TimeLeft = Plugtool.TimeLeft;
					d.ReadSize = Plugtool.ReadSize;

					uint64 mask = 0;
					m_pTableMgr->GetPlugToolStatusTable()->Ensure( &d ,~mask);
				}
				CoTaskMemFree(tmp);
			}
			break;
		}

		return;
		
	}

	void CRTDataSvr::WriteUpdateGameStatus_(int type)
	{
		ISvrPlug *pUpdateServer = m_pPlugMgr->FindPlug(PLUG_UPTSVR_NAME);
		if(pUpdateServer == NULL) return;
		assert(pUpdateServer);

		char *tmp = reinterpret_cast<char *>(pUpdateServer->SendMessage(1, type, 0));
		if(tmp == NULL) return;
		m_pTableMgr->GetUpdateGameStatusTable()->DeleteAll();

		int count = 0;
		memcpy(&count,tmp,sizeof(int));

		for (int i = 0; i < count; i++)
		{
			db::tUpdateGameStatus UpdateGame ;
			memcpy(&UpdateGame,(tmp + sizeof(int))+( i * sizeof(db::tUpdateGameStatus)),sizeof(db::tUpdateGameStatus));

			db::tUpdateGameStatus d;
			// GID 为0则结束
			if( UpdateGame.GID == 0 )
				break;
			d.ClientIP = UpdateGame.ClientIP;

			if( type != Sync )
			{
				struct Filter : public IClientFilter 
				{
					Filter(ulong IP) : m_IP(IP) {}
					bool bypass(const db::tClient *const d) 
					{
						return d->IP != m_IP ;
					}
				private:
					ulong m_IP;
				} filter(d.ClientIP);

				IClientRecordset *pRecordset = 0;

				m_pTableMgr->GetClientTable()->Select(&pRecordset,&filter,0);
				if( pRecordset->GetCount() == 0 )
					continue;

				for (uint32 i=0;i<pRecordset->GetCount();i++)
				{
					db::tClient *client = pRecordset->GetData(i);
					utility::Strcpy(d.ClientName,client->Name);
				}

				pRecordset->Release();
			}
			

			d.GID = UpdateGame.GID;
			db::tGame game;
			m_pTableMgr->GetGameTable()->GetData(d.GID, &game);
			utility::Strcpy(d.GameName,game.Name);

			d.SizeLeft = UpdateGame.SizeLeft;
			utility::Strcpy(d.TID,UpdateGame.TID);
			d.TransferRate = UpdateGame.TransferRate * 1024;
			d.Type = UpdateGame.Type;
			d.UpdateSize = UpdateGame.UpdateSize;
			d.UpdateState = UpdateGame.UpdateState;
			utility::Strcpy(d.SvrDir,UpdateGame.SvrDir);
			utility::Strcpy(d.NodeDir,UpdateGame.NodeDir);

			uint64 mask = 0;
			m_pTableMgr->GetUpdateGameStatusTable()->Ensure( &d ,~mask);
		}
		CoTaskMemFree(tmp);
	}

	void CRTDataSvr::InsertRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;
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
		case FLAG_RUNTYPE_BIT: {
			db::tRunType d;
			inpackage >> d;
			ret = m_pTableMgr->GetRunTypeTable()->Insert( &d );
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			ret = m_pTableMgr->GetClientTable()->Insert( &d );
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			db::tCmpBootTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetCmpBootTaskTable()->Insert( &d );
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
		case FLAG_SERVER_BIT: {
			db::tServer d;
			inpackage >> d;
			ret = m_pTableMgr->GetServerTable()->Insert( &d );
			break; }
		case FLAG_BOOTTASKAREA_BIT: {
			db::tBootTaskArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetBootTaskAreaTable()->Insert( &d );
			break; }
		case FLAG_SYNCGAME_BIT: {
			db::tSyncGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncGameTable()->Insert( &d );
			break; }
		case FLAG_SYNCTASKSTATUS_BIT: {
			db::tSyncTaskStatus d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncTaskStatusTable()->Insert( &d );
			break; }
		case FLAG_PUSHGAMESTATIC_BIT: {
			db::tPushGameStatic d;
			inpackage >> d;
			ret = m_pTableMgr->GetPushGameStaticTable()->Insert( &d );
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN(_T("SQL操作失败"));
		ACK();
	}

	void CRTDataSvr::UpdateRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;
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
		case FLAG_RUNTYPE_BIT: {
			db::tRunType d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetRunTypeTable()->Update(dwParam1, &d, mask);
				else 
					ret = m_pTableMgr->GetRunTypeTable()->Update(dwParam1, strParam1.c_str(), &d, mask);
			}
			else {
				IRunTypeFilter *pFilter = m_pTableMgr->CreateRunTypeFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetRunTypeTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyRunTypeFilter(pFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				//Log(LM_INFO,_T("update Sqlhander key mask = %I64u"),mask);
				ret = m_pTableMgr->GetClientTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				//Log(LM_INFO,_T("update Sqlhander filter mask = %I64u"),mask);
				ret = m_pTableMgr->GetClientTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			db::tCmpBootTask d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpBootTaskTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				ICmpBootTaskFilter *pFilter = m_pTableMgr->CreateCmpBootTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpBootTaskTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyCmpBootTaskFilter(pFilter);
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
		case FLAG_SERVER_BIT: {
			db::tServer d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetServerTable()->Update(strParam1.c_str(), &d, mask);
			}
			else {
				IServerFilter *pFilter = m_pTableMgr->CreateServerFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetServerTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyServerFilter(pFilter);
			}
			break; }
		case FLAG_BOOTTASKAREA_BIT: {
			db::tBootTaskArea d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0)
					ret = m_pTableMgr->GetBootTaskAreaTable()->Update(strParam1.c_str(), &d, mask);
				else 
					ret = m_pTableMgr->GetBootTaskAreaTable()->Update(strParam1.c_str(), strParam2.c_str(), &d, mask);
			}
			else {
				IBootTaskAreaFilter *pFilter = m_pTableMgr->CreateBootTaskAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetBootTaskAreaTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyBootTaskAreaFilter(pFilter);
			}
			break; }
		case FLAG_SYNCTASKSTATUS_BIT: {
			db::tSyncTaskStatus d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetSyncTaskStatusTable()->Update(strParam1.c_str(), &d, mask);
				else 
					ret = m_pTableMgr->GetSyncTaskStatusTable()->Update(strParam1.c_str(), dwParam1, &d, mask);
			}
			else {
				ISyncTaskStatusFilter *pFilter = m_pTableMgr->CreateSyncTaskStatusFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetSyncTaskStatusTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroySyncTaskStatusFilter(pFilter);
			}
			break; }
		case FLAG_PUSHGAMESTATIC_BIT: {
			db::tPushGameStatic d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetPushGameStaticTable()->Update(dwParam1, &d, mask);
			}
			else {
				IPushGameStaticFilter *pFilter = m_pTableMgr->CreatePushGameStaticFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetPushGameStaticTable()->Update(&d, mask, pFilter);
				m_pTableMgr->DestroyPushGameStaticFilter(pFilter);
			}
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN(_T("SQL操作失败"));
		ACK();
	}

	void CRTDataSvr::DeleteRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;
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
		case FLAG_RUNTYPE_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetRunTypeTable()->Delete(dwParam1);
				else
					ret = m_pTableMgr->GetRunTypeTable()->Delete(dwParam1, strParam1.c_str());
			}
			else {
				struct DeleteRunType
					: public IRunTypeFilter 
				{
				private:
					LPCTSTR m_pParameter;

				public:
					DeleteRunType(LPCTSTR pParameter)
						: m_pParameter(pParameter)
					{
					}

					virtual bool bypass(const db::tRunType *const c)
					{
						if( _tcsncicmp(c->AID, m_pParameter, MAX_GUID_LEN) == 0 )
							return false;
						else
							return true;
					}
				};
				DeleteRunType deleteRunTypeFilter(strParam1.c_str());
				ret = m_pTableMgr->GetRunTypeTable()->Delete(&deleteRunTypeFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				//Log(LM_INFO,_T("delete key "));
				ret = m_pTableMgr->GetClientTable()->Delete(strParam1.c_str());
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				//Log(LM_INFO,_T("delete filter"));
				ret = m_pTableMgr->GetClientTable()->Delete(pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpBootTaskTable()->Delete(strParam1.c_str());
			}
			else {
				ICmpBootTaskFilter *pFilter = m_pTableMgr->CreateCmpBootTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpBootTaskTable()->Delete(pFilter);
				m_pTableMgr->DestroyCmpBootTaskFilter(pFilter);
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
		case FLAG_SERVER_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetServerTable()->Delete(strParam1.c_str());
			}
			else {
				IServerFilter *pFilter = m_pTableMgr->CreateServerFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetServerTable()->Delete(pFilter);
				m_pTableMgr->DestroyServerFilter(pFilter);
			}
			break; }
		case FLAG_BOOTTASKAREA_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (dwParam1 == 0)
					ret = m_pTableMgr->GetBootTaskAreaTable()->Delete(strParam1.c_str());
				else 
					ret = m_pTableMgr->GetBootTaskAreaTable()->Delete(strParam1.c_str(), strParam2.c_str());
			}
			else {
				IBootTaskAreaFilter *pFilter = m_pTableMgr->CreateBootTaskAreaFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetBootTaskAreaTable()->Delete(pFilter);
				m_pTableMgr->DestroyBootTaskAreaFilter(pFilter);
			}
			break; }
		case FLAG_SYNCGAME_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetSyncGameTable()->Delete(dwParam1);
				else
					ret = m_pTableMgr->GetSyncGameTable()->Delete(dwParam1, strParam1.c_str());
			}
			else {
				struct DeleteSyncGame
					: public ISyncGameFilter 
				{
				private:
					LPCTSTR m_pParameter;

				public:
					DeleteSyncGame(LPCTSTR pParameter)
						: m_pParameter(pParameter)
					{
					}

					virtual bool bypass(const db::tSyncGame *const c)
					{
						if( _tcsncicmp(c->SID, m_pParameter, MAX_GUID_LEN) == 0 )
							return false;
						else
							return true;
					}
				};
				DeleteSyncGame deleteSyncGameFilter(strParam1.c_str());
				ret = m_pTableMgr->GetSyncGameTable()->Delete(&deleteSyncGameFilter);
			}
			break; }
		case FLAG_SYNCTASKSTATUS_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetSyncTaskStatusTable()->Delete(strParam1.c_str());
				else
					ret = m_pTableMgr->GetSyncTaskStatusTable()->Delete(strParam1.c_str(), dwParam1);
			}
			else {
				struct DeleteSyncTaskStatus
					: public ISyncTaskStatusFilter 
				{
				private:
					LPCTSTR m_pParameter;

				public:
					DeleteSyncTaskStatus(LPCTSTR pParameter)
						: m_pParameter(pParameter)
					{
					}

					virtual bool bypass(const db::tSyncTaskStatus *const c)
					{
						if( _tcsncicmp(c->SID, m_pParameter, MAX_GUID_LEN) == 0 )
							return false;
						else
							return true;
					}
				};
				DeleteSyncTaskStatus deleteSyncTaskStatusFilter(strParam1.c_str());
				ret = m_pTableMgr->GetSyncTaskStatusTable()->Delete(&deleteSyncTaskStatusFilter);
			}
			break; }
		case FLAG_PUSHGAMESTATIC_BIT: {
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetPushGameStaticTable()->Delete(dwParam1);
			}
			else {
				IPushGameStaticFilter *pFilter = m_pTableMgr->CreatePushGameStaticFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetPushGameStaticTable()->Delete(pFilter);
				m_pTableMgr->DestroyPushGameStaticFilter(pFilter);
			}
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN(_T("SQL操作失败"));
		ACK();
	}

	void CRTDataSvr::EnsureRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage,pkgheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;
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
		case FLAG_RUNTYPE_BIT: {
			db::tRunType d;
			inpackage >> d;
			ret = m_pTableMgr->GetRunTypeTable()->Ensure( &d, mask  );
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			//Log(LM_INFO,_T("Ensuer mask = %I64u"),mask);
			ret = m_pTableMgr->GetClientTable()->Ensure( &d, mask  );
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			db::tCmpBootTask d;
			inpackage >> d;
			ret = m_pTableMgr->GetCmpBootTaskTable()->Ensure( &d, mask  );
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
		case FLAG_SERVER_BIT: {
			db::tServer d;
			inpackage >> d;
			ret = m_pTableMgr->GetServerTable()->Ensure( &d, mask  );
			break; }
		case FLAG_BOOTTASKAREA_BIT: {
			db::tBootTaskArea d;
			inpackage >> d;
			ret = m_pTableMgr->GetBootTaskAreaTable()->Ensure( &d, mask  );
			break; }
		case FLAG_SYNCGAME_BIT: {
			db::tSyncGame d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncGameTable()->Ensure( &d, mask  );
			break; }
		case FLAG_SYNCTASKSTATUS_BIT: {
			db::tSyncTaskStatus d;
			inpackage >> d;
			ret = m_pTableMgr->GetSyncTaskStatusTable()->Ensure( &d, mask  );
			break; }
		case FLAG_PUSHGAMESTATIC_BIT: {
			db::tPushGameStatic d;
			inpackage >> d;
			ret = m_pTableMgr->GetPushGameStaticTable()->Ensure( &d, mask  );
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN(_T("SQL操作失败"));
		ACK();
	}

	
	void CRTDataSvr::ComulateRecord(INetLayer *pNetLayer, SOCKET sck, CPkgHelper &inpackage, pkgheader &header)
	{
		uint32 table;
		uint32 CondMode; //条件模式
		uint64 CondMask; //条件字段
		DWORD dwParam1, dwParam2;
		stdex::tString strParam1, strParam2;
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
		case FLAG_RUNTYPE_BIT: {
			db::tRunType d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				if (strParam1.empty())
					ret = m_pTableMgr->GetRunTypeTable()->Cumulate(dwParam1, &d, mask);
				else 
					ret = m_pTableMgr->GetRunTypeTable()->Cumulate(dwParam1, strParam1.c_str(), &d, mask);
			}
			else {
				IRunTypeFilter *pFilter = m_pTableMgr->CreateRunTypeFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetRunTypeTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyRunTypeFilter(pFilter);
			}
			break; }
		case FLAG_CLIENT_BIT: {
			db::tClient d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				//Log(LM_INFO,_T("Cumulate key mask = %I64u"),mask);
				ret = m_pTableMgr->GetClientTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				IClientFilter *pFilter = m_pTableMgr->CreateClientFilter(CondMask, CondMode, dwParam1, strParam1);
				//Log(LM_INFO,_T("Cumulate filter mask = %I64u"),mask);
				ret = m_pTableMgr->GetClientTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyClientFilter(pFilter);
			}
			break; }
		case FLAG_CMPBOOTTASK_BIT: {
			db::tCmpBootTask d;
			inpackage >> d;
			if (CondMode == SQL_COND_MODE_KEY) {
				ret = m_pTableMgr->GetCmpBootTaskTable()->Cumulate(strParam1.c_str(), &d, mask);
			}
			else {
				ICmpBootTaskFilter *pFilter = m_pTableMgr->CreateCmpBootTaskFilter(CondMask, CondMode, dwParam1, strParam1);
				ret = m_pTableMgr->GetCmpBootTaskTable()->Cumulate(&d, mask, pFilter);
				m_pTableMgr->DestroyCmpBootTaskFilter(pFilter);
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
			assert(0);
			break; }
		case FLAG_SERVER_BIT: {
			assert(0);
			break; }
		case FLAG_BOOTTASKAREA_BIT: {
			assert(0);
			break; }
		case FLAG_PUSHGAMESTATIC_BIT: {
			assert(0);
			break; }
		default:
			break;
		}

		if (ret)
			NAK_RETURN(_T("SQL操作失败"));
		ACK();
	}
	
} //namespace i8desk

