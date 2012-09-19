#include "stdafx.h"
#include "ManagerInstance.h"

#include "Console.h"
#include "LogHelper.h"
#include "../../../include/I8Type.hpp"
#include "AsyncDataHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace i8desk
{

	ManagerInstance::ManagerInstance()
		: rtServer_()
		, dataMgr_(&rtServer_)
		, realDataMgr_(&rtServer_)
		, controlMgr_(&rtServer_)
		, heartBeatMgr_(&rtServer_)
		, taskMgr_(&rtServer_)
		, connect_(0)
	{}
	ManagerInstance::~ManagerInstance()
	{
		Stop();
	}

	void ManagerInstance::_OnConnect()
	{
		if( ::InterlockedExchangeAdd(&connect_, 0) == 0 )
		{
			if( ::IsWindow(AfxGetApp()->m_pMainWnd->GetSafeHwnd()) )
				AfxGetApp()->m_pMainWnd->PostMessage(WM_USER + 101/*i8desk::ui::WM_MSG_REINIT*/);
		}

		::InterlockedExchange(&connect_, 1);
	}

	bool ManagerInstance::Start(const std::string &ip)
	{
		using namespace std::tr1::placeholders;

		rtServer_.RegisterOnConnect(std::tr1::bind(&ManagerInstance::_OnConnect, this));

		dataMgr_.RegisterError(std::tr1::bind(&ManagerInstance::HandleError, this, _1));
		realDataMgr_.RegisterError(std::tr1::bind(&ManagerInstance::HandleError, this, _1));
		controlMgr_.RegisterError(std::tr1::bind(&ManagerInstance::HandleError, this, _1));
		taskMgr_.RegisterError(std::tr1::bind(&ManagerInstance::HandleError, this, _1));
		recvDataMgr_.RegisterError(std::tr1::bind(&ManagerInstance::HandleError, this, _1));


		// Start
		try
		{
			rtServer_.Start(ip, 17918);
			recvDataMgr_.Start(17900);

			if( !rtServer_.WaitConnect(5000) )
				return false;
		}
		catch(exception::Base &e)
		{
			::InterlockedExchange(&connect_, 0);
			Log() << e.What() << logsystem::Endl;
			return false;
		}
		catch(std::exception &e)
		{
			::InterlockedExchange(&connect_, 0);
			Log() << CA2T(e.what()) << logsystem::Endl;
			return false;
		}
		catch(...)
		{
			::InterlockedExchange(&connect_, 0);
			Log() << _T("链接服务器发生未知错误!") << logsystem::Endl;
			return false;
		}

		::InterlockedExchange(&connect_, 1);
		return true;
	}

	void ManagerInstance::Stop()
	{
		rtServer_.Stop();
		recvDataMgr_.Stop();
	}

	void ManagerInstance::HandleError(const stdex::tString &msg)
	{
		if( msg.find(_T("UnConnect")) != stdex::tString::npos )
		{
			::InterlockedExchange(&connect_, 0);
			if( ::IsWindow(AfxGetApp()->m_pMainWnd->GetSafeHwnd()) )
				AfxGetApp()->m_pMainWnd->PostMessage(WM_USER + 0x150);
		}

		::OutputDebugString(msg.c_str());
		::OutputDebugString(_T("\n"));
		
		if( ::InterlockedExchangeAdd(&connect_, 0) == 1 )
			Log() << msg << logsystem::Endl;
	}

	bool ManagerInstance::IsConnect() const
	{
		return ::InterlockedExchangeAdd(&connect_, 0) == 1;
	}


	ManagerInstance &GetManagerInstance()
	{
		return *AfxManagerInstance();
	}
}