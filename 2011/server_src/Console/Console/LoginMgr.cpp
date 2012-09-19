#include "stdafx.h"
#include "LoginMgr.h"

#include "ManagerInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{

	i8desk::LoginMgr loginHelper;



	LoginMgr::LoginMgr()
		: mgrInstance_(new i8desk::ManagerInstance)
	{

	}
	LoginMgr::~LoginMgr()
	{
		//mgrInstance_->Stop();
	}

	bool LoginMgr::Connect(const stdex::tString &ip)
	{
		ip_ = CT2A(ip.c_str());

		
		return mgrInstance_->Start(ip_);
	}

	void LoginMgr::LogOff()
	{
		GetControlMgr().NotifySaveData();
		mgrInstance_->Stop();
		mgrInstance_.reset();
	}

	bool LoginMgr::Password(const stdex::tString &password)
	{
		stdex::tString pwd = i8desk::GetDataMgr().GetOptVal(OPT_U_CTLPWD, _T("1234567"));

		if( i8desk::IsLocalMachine(ip_) )
			return true;

		if( password != pwd )
			return false;

		return true;
	}

}