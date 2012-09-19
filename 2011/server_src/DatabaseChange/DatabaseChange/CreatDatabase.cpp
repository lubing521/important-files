#include "stdafx.h"
#include "CreatDatabase.h"

#include "../../../include/Utility/utility.h"


using namespace ADOX;

namespace i8desk
{
	CCreatDatabase::CCreatDatabase(ILogger* pLogger, const stdex::tString datapath)
		: pLogger_(pLogger)
		, datapath_(datapath)
	{
		CoInitialize(NULL);
		pCatalog_	 =   NULL;    
		pTable_		 =   NULL;  
		pIndexNew_   =   NULL;   
		pConnn_		 =   NULL;   

	}

	CCreatDatabase::~CCreatDatabase()
	{

	}

	bool CCreatDatabase::start()
	{
		if(PathFileExists(datapath_.c_str()))
			return true;

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建数据库！"));

		pCatalog_.CreateInstance(__uuidof(Catalog)); 
		stdex::tString szConnString = TEXT("Provider=Microsoft.Jet.OLEDB.4.0.1; Data Source=") + datapath_;

		pCatalog_->Create(_bstr_t(szConnString.c_str())); 
		pConnn_.CreateInstance(_T("ADODB.Connection"));  
		pConnn_->PutCommandTimeout(30);  
		pConnn_->PutConnectionTimeout(30);  
		pConnn_->put_CursorLocation(adUseClient);  
		pConnn_->Open(_bstr_t(szConnString.c_str()),_bstr_t(),_bstr_t(),adConnectUnspecified);  
		pCatalog_->PutActiveConnection(_variant_t((IDispatch   *)   pConnn_));   

	

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建AREA表"));
		CreateAreaTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建BootTaskArea表"));
		CreateBootTaskAreaTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Class表"));
		CreateClassTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Client表"));
		CreateClientTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建CmpBootTask表"));
		CreateCmpBootTaskTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Favorite表"));
		CreateFavoriteTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Game表"));
		CreateGameTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Option表"));
		CreateOptionTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建PlugTool表"));
		CreatePlugToolTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建RunType表"));
		CreateRunTypeTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Server表"));
		CreateServerTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建SyncTask表"));
		CreateSyncTaskTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建Vdisk表"));
		CreateVdiskTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建SyncGame表"));
		CreateSyncGameTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建SyncTaskStatus表"));
		CreateSyncTaskStatusTable();

		pLogger_->WriteLog(LM_INFO, TEXT("开始创建PushGameStatic表"));
		CreatePushGameStaticTable();


		pLogger_->WriteLog(LM_INFO, TEXT("创建数据库完成！"));

		return true;
	}

	bool CCreatDatabase::stop()
	{
		CoUninitialize();
		return true;
	}

	void CCreatDatabase::CreateAreaTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblArea ( " )
			<< _T("AID				varchar(38) WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("Name				varchar(64) WITH   COMP,")
			<< _T("SvrID			varchar(38) WITH   COMP ")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX areaindex ON tblArea (AID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);
	}

	void CCreatDatabase::CreateBootTaskAreaTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblBootTaskArea ( " )
			<< _T("TID				varchar(38) WITH   COMP NOT NULL,")
			<< _T("AID				varchar(38) WITH   COMP NOT NULL,")
			<< _T("PRIMARY KEY (TID,AID)")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX boottaskareaindex ON tblBootTaskArea (TID,AID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);
	}


	void CCreatDatabase::CreateClassTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblClass ( " )
			<< _T("CID				varchar(38) WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("Name				varchar(64) WITH   COMP,")
			<< _T("SvrPath			varchar(255) WITH   COMP")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX classindex ON tblClass (CID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);
	}

	void CCreatDatabase::CreateClientTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblClient ( " )
			<< _T("Name			varchar(255) WITH   COMP PRIMARY KEY NOT NULL ,")
			<< _T("AID			varchar(38)  WITH   COMP,") 
			<< _T("IP			int,")
			<< _T("MAC			varchar(255) WITH   COMP,")

			<< _T("Mark			int," )
			<< _T("Gate			int," )	
			<< _T("DNS			int," )
			<< _T("DNS2			int," )
			<< _T("ProtInstall	int," )

			<< _T("Partition	varchar(64) WITH   COMP," )
			<< _T("ProtVer		varchar(64) WITH   COMP," )
			<< _T("VDiskVer		varchar(64) WITH   COMP," )
			<< _T("MenuVer		varchar(64) WITH   COMP," )
			<< _T("CliSvrVer	varchar(64) WITH   COMP," )

			<< _T("System		varchar(255) WITH   COMP," )
			<< _T("CPU			varchar(255) WITH   COMP," )
			<< _T("MainBoard	varchar(255) WITH   COMP," )
			<< _T("Memory		varchar(255) WITH   COMP," )
			<< _T("Disk			varchar(255) WITH   COMP," )
			<< _T("Video		varchar(255) WITH   COMP," )
			<< _T("Audio		varchar(255) WITH   COMP," )
			<< _T("Network		varchar(255) WITH   COMP," )
			<< _T("Camera		varchar(255) WITH   COMP" )
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX clientAIDindex ON tblClient (AID)");
		
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateCmpBootTaskTable()
	{
		pTable_.CreateInstance(_T("ADOX.Table"));  
		pTable_->ParentCatalog   =pCatalog_;  
		pTable_->Name = _T("tblCmpBootTask");  
		ColumnsPtr pCols   = pTable_->Columns;  
		pCols->Append(_T("TID")     ,ADOX::adWChar,38);//自动编号字段  
		pCols->Append(_T("Type")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("Flag")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("RunDate")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("StartTime")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("DelayTime")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("EndTime")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("EndDate")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("RunType")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("Status")       ,ADOX::adInteger,0);//文本字段
		pCols->Append(_T("Parameter")       ,ADOX::adLongVarWChar,0);//文本字段
		pCols->Append(_T("Name")       ,ADOX::adWChar,255);//文本字段

		pCols->GetItem(_T("Type"))->Attributes = adColNullable;
		pCols->GetItem(_T("Flag"))->Attributes = adColNullable;
		pCols->GetItem(_T("RunDate"))->Attributes = adColNullable;
		pCols->GetItem(_T("StartTime"))->Attributes = adColNullable;
		pCols->GetItem(_T("DelayTime"))->Attributes = adColNullable;
		pCols->GetItem(_T("EndTime"))->Attributes = adColNullable;
		pCols->GetItem(_T("EndDate"))->Attributes = adColNullable;
		pCols->GetItem(_T("RunType"))->Attributes = adColNullable;
		pCols->GetItem(_T("Status"))->Attributes = adColNullable;

		pCols->GetItem(_T("Parameter"))->Attributes = adColNullable;
		pCols->GetItem(_T("Name"))->Attributes = adColNullable;


		pCatalog_->Tables->Append(_variant_t   ((IDispatch*)pTable_));//添加表  
		pCatalog_->Tables->Refresh();//刷新   

		pIndexNew_.CreateInstance(_T("ADOX.Index"));  
		pIndexNew_->Name   =   "cmpboottaskIndex";//索引名称  
		pIndexNew_->Columns->Append("TID",ADOX::adWChar,255);//索引字段  

		pIndexNew_->PutPrimaryKey(-1);//主索引  
		pIndexNew_->PutUnique(-1);//唯一索引  
		pTable_->Indexes->Append(_variant_t   ((IDispatch*)pIndexNew_));//创建索引  
	}

	void CCreatDatabase::CreateFavoriteTable()
	{

		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblFavorite ( " )
			<< _T("FID				varchar(38) WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("Type				int,")
			<< _T("Name				varchar(32) WITH   COMP,")
			<< _T("Url				varchar(255) WITH   COMP,")
			<< _T("Serial			int")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX favoriteindex ON tblFavorite (FID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateGameTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblGame ( " )
			<< _T("GID			int PRIMARY KEY NOT NULL,")
			<< _T("PID			int,") 
			<< _T("CID			varchar(38),")
			<< _T("Name			varchar(64) WITH   COMP,")
			<< _T("Exe			varchar(255) WITH   COMP," )
			<< _T("Param		varchar(255) WITH   COMP," )	
			<< _T("[Size]		int," )
			<< _T("SvrPath		varchar(255) WITH   COMP," )
			<< _T("CliPath		varchar(255) WITH   COMP," )
			<< _T("TraitFile	varchar(255) WITH   COMP," )
			<< _T("SaveFilter	varchar(255) WITH   COMP," )
			<< _T("DeskLink		int," )
			<< _T("Toolbar		int," )
			<< _T("[Memo]		varchar(255) WITH   COMP," )
			<< _T("GameSource	varchar(255) WITH   COMP," )
			<< _T("IdcAddDate	int," )
			<< _T("EnDel		int," )
			<< _T("IdcVer		int," )
			<< _T("SvrVer		int," )
			<< _T("IdcClick		int," )
			<< _T("SvrClick		int," )
			<< _T("SvrClick2	int," )
			<< _T("AutoUpt		int," )
			<< _T("I8Play		int," )
			<< _T("Priority		int," )
			<< _T("Force		int," )
			<< _T("ForceDir		varchar(255) WITH   COMP," )
			<< _T("Status		int," )
			<< _T("StopRun		int," )
			<< _T("[Repair]		int," )
			<< _T("Hide			int" )

			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX gameGidindex ON tblGame (GID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateOptionTable()
	{
		pTable_.CreateInstance(_T("ADOX.Table"));  
		pTable_->ParentCatalog   =pCatalog_;  
		pTable_->Name = _T("tblOption");  
		ColumnsPtr pCols   = pTable_->Columns;  
		pCols->Append(_T("OptName")     ,ADOX::adWChar,255);//自动编号字段  
		pCols->Append(_T("OptValue")       ,ADOX::adLongVarWChar,0);//文本字段

		pCols->GetItem(_T("OptValue"))->Attributes = adColNullable;
		pCols->GetItem(_T("OptName"))->Properties->GetItem("Jet OLEDB:Allow Zero Length")->Value = false;

		pCatalog_->Tables->Append(_variant_t   ((IDispatch*)pTable_));//添加表  
		pCatalog_->Tables->Refresh();//刷新   

		pIndexNew_.CreateInstance(_T("ADOX.Index"));  
		pIndexNew_->Name   =   "tblOptionIndex";//索引名称  
		pIndexNew_->Columns->Append("OptName",ADOX::adWChar,255);//索引字段  

		pIndexNew_->PutPrimaryKey(-1);//主索引  
		pIndexNew_->PutUnique(-1);//唯一索引  
		pTable_->Indexes->Append(_variant_t   ((IDispatch*)pIndexNew_));//创建索引  

	}

	void CCreatDatabase::CreatePlugToolTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblPlugTool ( " )
			<< _T("PID				int PRIMARY KEY NOT NULL,")

			<< _T("CID				varchar(38) WITH   COMP,")

			<< _T("IdcVer			int," )
			<< _T("SvrVer			int," )
			<< _T("Priority			int," )

			<< _T("Name				varchar(64) WITH   COMP,")
			<< _T("Comment			varchar(255) WITH   COMP," )
			<< _T("ToolSource		varchar(255) WITH   COMP," )

			<< _T("[Size]			int," )
			<< _T("IdcClick			int," )
			<< _T("Status			int," )
			<< _T("DownloadType		int," )
			<< _T("DownloadStatus	int," )

			<< _T("SvrName			varchar(64) WITH   COMP," )
			<< _T("SvrExe			varchar(255) WITH   COMP," )
			<< _T("SvrParam			varchar(255) WITH   COMP," )
			<< _T("SvrPath			varchar(255) WITH   COMP," )
			<< _T("SvrRunType		int," )

			<< _T("CliName			varchar(64) WITH   COMP," )
			<< _T("CliExe			varchar(255) WITH   COMP," )
			<< _T("CliParam			varchar(255) WITH   COMP," )
			<< _T("CliPath			varchar(255) WITH   COMP," )
			<< _T("CliRunType		int," )

			<< _T("ConfigName		varchar(64) WITH   COMP," )
			<< _T("ConfigExe		varchar(255) WITH   COMP," )
			<< _T("ConfigParam		varchar(255) WITH   COMP," )
			<< _T("ConfigPath		varchar(255) WITH   COMP," )
			<< _T("ConfigRunType	int" )

			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX plugtoolPidindex ON tblPlugTool (PID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateRunTypeTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblRunType ( " )
			<< _T("AID				varchar(38) WITH   COMP NOT NULL,")
			<< _T("GID				int NOT NULL,")

			<< _T("[Type]			int," )
			<< _T("VID				varchar(255) WITH   COMP," )
			<< _T("PRIMARY KEY (AID,GID)")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX runtypeindex ON tblRunType (AID, GID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateServerTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblServer ( " )
			<< _T("SvrID			varchar(38)  WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("SvrName			varchar(255) WITH   COMP, ")
			<< _T("SvrType			int," )
			<< _T("SvrIP			int," )
			<< _T("Ip1				int," )
			<< _T("Ip2				int," )
			<< _T("Ip3				int," )
			<< _T("Ip4				int," )
			<< _T("Speed			int," )
			<< _T("BalanceType		int" )
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX serverindex ON tblServer (SvrID)");

		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}


	void CCreatDatabase::CreateSyncTaskTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblSyncTask ( " )
			<< _T("SID			varchar(38) WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("SvrID		varchar(38) WITH   COMP, ")
			<< _T("Name			varchar(255) WITH   COMP, ")
			<< _T("SvrDir		varchar(255) WITH   COMP," )
			<< _T("NodeDir		varchar(255) WITH   COMP," )
			<< _T("SoucIP		int," )
			<< _T("DestIp		int," )
			<< _T("Speed		int," )
			<< _T("SyncType		int," )
			<< _T("NameID		varchar(38) " )
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX synctaskindex ON tblSyncTask (SID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}


	void CCreatDatabase::CreateVdiskTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblVdisk ( " )
			<< _T("VID			varchar(38) WITH   COMP PRIMARY KEY NOT NULL,")
			<< _T("SvrID		varchar(38) WITH   COMP, ")
			<< _T("SoucIP		int, ")
			<< _T("Ip			int, ")
			<< _T("Port			int, ")
			<< _T("SvrDrv		int," )
			<< _T("CliDrv		int," )
			<< _T("[Type]		int," )
			<< _T("LoadType		int," )
			<< _T("SvrMode		varchar(38) WITH   COMP," )
			<< _T("[Size]		int," )
			<< _T("SsdDrv		int" )
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX vdiskindex ON tblVdisk (VID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateSyncGameTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblSyncGame ( " )
			<< _T("SID				varchar(38) WITH   COMP NOT NULL,")
			<< _T("GID				int NOT NULL,")
			<< _T("PRIMARY KEY (SID,GID)")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX syncgameindex ON tblSyncGame (SID, GID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

	void CCreatDatabase::CreateSyncTaskStatusTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblSyncTaskStatus ( " )
			<< _T("SID				varchar(38) WITH   COMP NOT NULL,")
			<< _T("GID				int NOT NULL,")
			<< _T("NameID			varchar(38) WITH   COMP NOT NULL," )
			<< _T("GameName			varchar(255) WITH   COMP," )
			<< _T("UpdateState		int," )
			<< _T("PRIMARY KEY (SID,GID)")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX synctaskstatusindex ON tblSyncTaskStatus (SID, GID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}


	void CCreatDatabase::CreatePushGameStaticTable()
	{
		stdex::tOstringstream sql;
		sql << _T("CREATE TABLE tblPushGameStatic ( " )
			<< _T("GID			int PRIMARY KEY NOT NULL,")
			<< _T("[Size]		int,") 
			<< _T("BeginDate	int,")
			<< _T("EndDate		int")
			<< _T(")");

		_variant_t v;
		pConnn_->Execute(_bstr_t(sql.str().c_str()), &v, 0);

		stdex::tOstringstream os;
		os << _T("CREATE INDEX PushgameStaticindex ON tblPushGameStatic (GID)");
		pConnn_->Execute(_bstr_t(os.str().c_str()), &v, 0);

	}

}
