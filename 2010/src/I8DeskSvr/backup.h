#ifndef __i8desk_backup_inc__
#define __i8desk_backup_inc__

#include "../../include/plug.h"
#include "../../include/MultiThread/ThreadPool.hpp"

#include "XZip.h"


namespace i8desk
{
	class CBackup
	{
		// 备份数据文件间隔时间
		static const size_t backupPeriodTime	= 1 * 60 * 1000;
		static const size_t backupDelayTime		= 1 * 60 * 1000;

		// 清除数据文件间隔时间
		static const size_t cleanPeriodTime		= 1 * 60 * 60 * 1000;
		static const size_t cleanDelayTime		= 25 * 60 * 1000;

	private:
		ILogger*	m_pLogger;
		async::thread::QueueTimerPool backupTimer_;
		async::thread::QueueTimerPool cleanTimer_;
		
	public:
		CBackup(ILogger* pLogger) 
			: m_pLogger(pLogger)
			, backupTimer_(std::tr1::bind(&CBackup::_Backup, this, std::tr1::placeholders::_1))
			, cleanTimer_(std::tr1::bind(&CBackup::_Clean, this, std::tr1::placeholders::_1))
		{
		}
		~CBackup() {}

	public:
		bool StartBackup();
		void StopBackup();

	private:
		bool ExecBackup();
		bool ZipData(HZIP hZip, LPCSTR szDir, int nBase);

	private:
		// 备份数据文件
		void _Backup(BOOLEAN);
		// 清除数据文件
		void _Clean(BOOLEAN);

	
	};
}

#endif