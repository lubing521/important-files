#include "stdafx.h"

#include "RemoteControl.h"
#include "../../include/Win32/System/DynamicFunc.hpp"


namespace i8desk
{

	CRemoteCtrl::CRemoteCtrl(ILogger* pLogger) 
		: m_pLogger(pLogger)
		, m_hThread(NULL)
		, m_dThreadID(0)
	{
	}

	CRemoteCtrl::~CRemoteCtrl()
	{
	}

	bool CRemoteCtrl::Start()
	{

		m_hThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
		if (m_hThread == NULL)
		{
			DWORD dwError = GetLastError();
			_com_error Error(dwError);
			m_pLogger->WriteLog(LM_INFO, TEXT("Create Rmtctrl Thread Error:%d:%s"), dwError, Error.ErrorMessage());
			return false;
		}
		return true;
	}

	void CRemoteCtrl::Stop()
	{
		PostThreadMessage(m_dThreadID, WM_QUIT, 0, 0);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
		m_dThreadID = 0;
	}


	UINT __stdcall CRemoteCtrl::WorkThread(LPVOID lpVoid)
	{
		CRemoteCtrl* pThis = reinterpret_cast<CRemoteCtrl*>(lpVoid);

		pThis->m_dThreadID = GetCurrentThreadId();

		stdex::tString strFilePath = utility::GetAppPath() + TEXT("winvnc4.dll");

		typedef bool (CALLBACK *PFUNC)(const wchar_t *psw);
		static win32::system::DynamicFunc<PFUNC> func(strFilePath.c_str(), "Start");
		if( func.IsValid() )
		{
			if((*func)(_T("i8deskSvr")))
			{
				pThis->m_pLogger->WriteLog(LM_INFO, TEXT("start remotectrl sucesse"));
				return true;
			}
		}

		return false;
	}

}