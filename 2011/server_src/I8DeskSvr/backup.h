#ifndef __i8desk_backup_inc__
#define __i8desk_backup_inc__

#pragma once

#include "plugmgr.h"
#include "../../include/Zip/XZip.h"

namespace i8desk
{
	class CBackup
	{
	public:
		CBackup(ILogger* pLogger) : m_pLogger(pLogger), m_hThread(NULL), m_hExited(NULL) 
		{
		}
		~CBackup() { StopBackup(); }
	public:
		bool StartBackup();
		void StopBackup();
	private:
		bool ExecBackup();
		bool ZipData(HZIP hZip, LPCTSTR szDir, int nBase);
		bool Judg(int hour, int minute);
		void DeleteLog(int Day);
		void DeleteBack();
	private:
		static UINT __stdcall WorkThread(LPVOID lpVoid);
	private:
		ILogger*	m_pLogger;
		HANDLE		m_hThread;
		HANDLE		m_hExited;
	};
}

#endif