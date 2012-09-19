#ifndef __i8desk_RemoteCtrl_inc__
#define __i8desk_RemoteCtrl_inc__

#pragma once

#include "plugmgr.h"

namespace i8desk
{
	class CRemoteCtrl
	{
	public:
		CRemoteCtrl( ILogger* pLogger);
		~CRemoteCtrl();

	public:
		bool Start();
		void Stop();

	private:
		static UINT __stdcall WorkThread(LPVOID lpVoid);
		bool StartRemoteControl();

	private:
		ILogger*	m_pLogger;
		HANDLE		m_hThread;
		volatile DWORD   m_dThreadID; 

	};
}

#endif