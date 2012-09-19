#include "StdAfx.h"
#include "MscRateMonitor.h"


#include "LogHelper.h"
#include "ThreadPoolHelper.h"
#include "../../../include/Utility/utility.h"
#include "../../../include/Lava/massctl.h"
#include "../../../include/Lava/P2PManager.h"
#include "../../../include/I8Type.hpp"


namespace i8desk
{

	namespace detail
	{
		struct InitMaxRate
		{
			std::map<long, long> maxRate;

			InitMaxRate()
			{
				maxRate[80]		= 64;
				maxRate[150]	= 128;
				maxRate[300]	= 256;
				maxRate[550]	= 512;
				maxRate[1100]	= 1024;
				maxRate[2000]	= 2048;
				maxRate[4000]	= 4096;
				maxRate[8200]	= 8192;
				maxRate[16500]	= 16284;
			}
		};
	}

	// --------------------------------------
	// MscRateMonitor

	MscRateMonitor::MscRateMonitor(ISysOptTable *pSysOptTable)
		: sysOpt_(pSysOptTable)
		, needUpdate_(true)
		, DNApid_(0)
	{
		assert(sysOpt_);
	}

	MscRateMonitor::~MscRateMonitor()
	{
	}


	void MscRateMonitor::Start()
	{
		sysOpt_->RegisterEventHandler(this);
		
		exit_.Create();
		thread_.RegisterFunc(std::tr1::bind(&MscRateMonitor::_Thread, this));
		thread_.Start();
		DNApid_ = _GetProcessId(_T("DNAService.exe"));

	}

	void MscRateMonitor::Stop()
	{
		exit_.SetEvent();
		thread_.Stop();

		sysOpt_->UnregisterEventHandler(this);
		
		Log(LM_DEBUG, _T("MscRateMonitor 退出成功"));
	}

	uint32 MscRateMonitor::GetEvents(void)
	{
		return SQL_OP_INSERT | SQL_OP_UPDATE;
	}
	void MscRateMonitor::HandleEvent(uint32 optype, const TCHAR *key, uint64 mask)
	{
		if( utility::Strcmp(key, OPT_D_SPEED) == 0 ||  utility::Strcmp(key, OPT_M_BROADBANDTYPE) == 0  )
			UpdateMscMaxRate();
	}


	void MscRateMonitor::SetSuspendType()
	{
		db::tSysOpt so;
		if( sysOpt_->GetData(OPT_D_ADSLSPEED, &so) != 0 ) 
			return;

		uint suspendType = 0;
		stdex::ToNumber(suspendType, so.value);

		stdex::tString strCfgFile = utility::GetAppPath() + TEXT("DNAService\\lava.ini");
		stdex::tString sz;

		if(suspendType == 0)     // 不限制上传
			sz = _T("upload");
		else
			sz = _T("all");

		BOOL ret = ::WritePrivateProfileString(_T("HTTP_SERVICE"), _T("suspend"), sz.c_str(), strCfgFile.c_str());
	}

	DWORD MscRateMonitor::_GetProcessId(LPCTSTR lpszProcName)
	{
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 ProcEntry = {sizeof(ProcEntry)};
		BOOL bRes = Process32First(hSnap, &ProcEntry);
		DWORD dwPid = 0;
		while (bRes)
		{
			if (lstrcmpi(lpszProcName, ProcEntry.szExeFile) == 0)
			{
				dwPid = ProcEntry.th32ProcessID;
				break;
			}
			bRes = Process32Next(hSnap, &ProcEntry);
		}
		CloseHandle(hSnap);
		return dwPid;//*/
	}

	bool MscRateMonitor::NeedUpdate()
	{
		static DWORD dwPID = _GetProcessId(_T("DNAService.exe"));

		if( dwPID != DNApid_ )
		{
			DNApid_ = dwPID;
			needUpdate_ = true;
		}

		return needUpdate_;
	}

	void  MscRateMonitor::UpdateMscMaxRate()
	{
		Log(LM_INFO, _T("更新三层最大速率"));

		static detail::InitMaxRate initMaxRate;

		db::tSysOpt so;
		db::tSysOpt so1;
		db::tSysOpt so2;

		if( sysOpt_->GetData(OPT_D_SPEED, &so) != 0 ) 
			return;
		if( sysOpt_->GetData(OPT_D_ADSLSPEED, &so1) != 0 )
			return;
		if( sysOpt_->GetData(OPT_M_BROADBANDTYPE, &so2) != 0 ) 
			return;

		uint maxdownrate = 0;
		stdex::ToNumber(maxdownrate, so.value);
		maxdownrate = initMaxRate.maxRate[maxdownrate] * 1024;
		int  maxuprate = 0;
		stdex::ToNumber(maxuprate, so1.value);
		uint suspendType = 0;
		stdex::ToNumber(suspendType, so2.value);

		if( /*maxuprate == -1 &&*/ suspendType == adsl )
		{
			Log(LM_DEBUG, _T("ADSL限速"), maxdownrate);
			_SetMscMaxRate(maxdownrate, 16);
		}
		else
		{
			Log(LM_DEBUG, _T("限速"));
			int upRate = ((maxdownrate / 3) / 16) * 16;
			_SetMscMaxRate(maxdownrate, upRate);
		}
	}

	void MscRateMonitor::_SetMscMaxRate(uint maxdownrate, uint maxuprate)
	{
		if( DNApid_ != 0 ) 
		{
			int nResult = msc_setmaxtransferrate(maxdownrate, maxuprate);
			if(nResult == P2P_OK)
			{
				needUpdate_ = false;
				stdex::tOstringstream os;
				os << _T("设置三层限速成功 maxdownrate = ") << maxdownrate << _T(", maxuprate = ") << maxuprate;
				Log(LM_DEBUG, os.str().c_str());
			}
		}
	}

	DWORD MscRateMonitor::_Thread()
	{
		while( !thread_.IsAborted() )
		{
			DWORD ret = ::WaitForSingleObject(exit_, DELAY_TIME);
			if( ret == WAIT_TIMEOUT )
			{
				SetSuspendType();
				if( NeedUpdate() )
					UpdateMscMaxRate();
			}
			else if( ret == WAIT_OBJECT_0 )
			{
				break;
			}
			else
			{
				assert(0);
			}
		}
		
		return 0;
	}
}