#ifndef __I8DESK_LOG_HELPER_HPP
#define __I8DESK_LOG_HELPER_HPP

namespace i8desk
{
	
	namespace log
	{
		// ----------------------------------------------------
		// class LogHelper

		struct LogHelper
		{
			i8desk::ILogger *log_;

			explicit LogHelper(i8desk::ISvrPlugMgr	*plugMgr)
				: log_(0)
			{
				log_ = plugMgr->CreateLogger();

				// 设置日志属性
				log_->SetLogFileName(_T("GamePlug"));
				log_->SetMaxLogSize(1024*1024*10);
				log_->SetAddDateBefore(true);
				log_->SetAddCrLfAfter(true);
			}
			~LogHelper()
			{
				log_->Release();
			}
			
			static LogHelper &GetInstance(i8desk::ISvrPlugMgr *plugMgr = 0)
			{
				static LogHelper log(plugMgr);
				return log;
			}

		private:
			LogHelper();
		};

		

	}
	

	
	inline void Log(size_t priority, LPCTSTR lpszText, ...)
	{
		TCHAR buf[8192] = {0};

		va_list marker;
		va_start(marker, lpszText);
		int nOk = ::_vsntprintf_s(buf, _countof(buf), lpszText, marker);
		va_end(marker);

		if (nOk <= 0)
		{
			assert(0);
			return;
		}

		log::LogHelper::GetInstance().log_->WriteLog(priority, buf);
	}

	inline void Log(size_t priority, const stdex::tString &text, ...)
	{
		TCHAR buf[8192] = {0};

		va_list marker;
		LPCTSTR lpszText = text.c_str();
		va_start(marker, lpszText);
		int nOk = ::_vsntprintf_s(buf, _countof(buf), lpszText, marker);
		va_end(marker);

		if (nOk <= 0)
		{
			assert(0);
			return;
		}

		log::LogHelper::GetInstance().log_->WriteLog(priority, buf);
	}

}



#endif