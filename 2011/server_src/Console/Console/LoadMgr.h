#ifndef __LOAD_MANAGER_HPP
#define __LOAD_MANAGER_HPP



namespace i8desk
{
	struct LoginMgr
	{
		LoginMgr()
			: mgrInstance_(new i8desk::ManagerInstance)
		{

		}
		~LoginMgr()
		{
			mgrInstance_->Stop();
		}

		bool Connect(const stdex::tString &ip)
		{
			ip_ = CT2A(ip.c_str());

			//mgrInstance_->Stop();
			return mgrInstance_->Start(ip_);
		}

		bool Password(const stdex::tString &password)
		{
			stdex::tString pwd = i8desk::GetDataMgr().GetOptVal(OPT_U_CTLPWD, _T("1234567"));

			if( i8desk::IsLocalMachine(ip_) )
				return true;

			if( password != pwd )
				return false;

			return true;
		}

		std::auto_ptr<i8desk::ManagerInstance> mgrInstance_;
		std::string ip_;
	};
}






#endif