#include "stdafx.h"
#include "DBMgr.hpp"

#include <sstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


namespace db
{


	DBMgr::DBMgr()
	{
		::CoInitialize(0);
	}

	DBMgr::~DBMgr()
	{
		::CoUninitialize();
	}

	bool DBMgr::Init(const std::wstring &dbIP, const std::wstring &dbName, const std::wstring &dbUserName, const std::wstring &dbPassword)
	{
		try
		{
			connection_.CreateInstance("ADODB.Connection");

			std::wstringstream os;
			os << L"Driver={MySQL ODBC 5.1 Driver};" //L"DSN=MYSQL;"
				<< L"Server=" << dbIP << L";"
				<< L"charset=UTF8;"
				<< L"Database=" << dbName << L";"
				<< L"UID=" << dbUserName << L";"
				<< L"PASSWORD=" << dbPassword << L";"
				<< L"OPTION=3;";
			connection_->Open(os.str().c_str(), _T(""), _T(""), adModeUnknown);
		} 
		catch(_com_error &e)
		{
			::MessageBox(NULL, e.Description(), L"´íÎó", MB_OK);
			return false;
		}

		return true;
	}

	void DBMgr::UnInit()
	{
		try
		{
			if( recordset_ )
				recordset_->Close();

			if( connection_ )
				connection_->Close();
		}
		catch(_com_error &e)
		{

		}
	}

	_RecordsetPtr &DBMgr::GetRecordSet(const _bstr_t &sql)
	{
		try
		{
			recordset_.CreateInstance(__uuidof(Recordset));

			recordset_->Open(sql, connection_.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		}
		catch(_com_error &e)
		{
			::MessageBox(NULL, e.Description(), L"´íÎó", MB_OK);
		}

		return recordset_;
	}

	bool DBMgr::ExecuteSQL(const _bstr_t &sql, CommandTypeEnum cmd/* = adCmdText*/)
	{
		try
		{
			connection_->Execute(sql, NULL, cmd);
			return true;
		}
		catch(_com_error &e)
		{
			::MessageBox(NULL, e.Description(), L"´íÎó", MB_OK);
			return false;
		}
	}

	

}