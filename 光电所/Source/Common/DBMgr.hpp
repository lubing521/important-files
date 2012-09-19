#ifndef __DB_MANAGER_HPP
#define __DB_MANAGER_HPP

#include <string>
#import "c:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")

namespace db
{
	class DBMgr  
	{
	private:
		_ConnectionPtr connection_;
		_RecordsetPtr recordset_;

	public:
		DBMgr();
		~DBMgr();

		bool Init(const std::wstring &dbIP, const std::wstring &dbName, const std::wstring &dbUserName, const std::wstring &dbPassword);
		void UnInit();


		_RecordsetPtr& GetRecordSet(const _bstr_t &sql);
		bool ExecuteSQL(const _bstr_t &sql, CommandTypeEnum cmd = adCmdText);
	};

	inline DBMgr &DBMgrInstance()
	{
		static DBMgr dbMgr;
		return dbMgr;
	}
}






#endif