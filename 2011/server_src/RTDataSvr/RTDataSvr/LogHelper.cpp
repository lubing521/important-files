#include "stdafx.h"
#include "LogHelper.h"
namespace i8desk
{
	namespace log
	{
		LogHelper::LogHelper(i8desk::ISvrPlugMgr	*plugMgr)
			 : log_(0)
		{
#ifndef DEBUG_TEST
			log_ = plugMgr->CreateLogger();

			// 设置日志属性
			log_->SetLogFileName(_T("RTDataSvr"));
			log_->SetMaxLogSize(1024*1024*10);
			log_->SetAddDateBefore(true);
			log_->SetAddCrLfAfter(true);
#endif
		}

		LogHelper::~LogHelper()
		{
#ifndef DEBUG_TEST
			log_->Release();
#endif
		}
	}

	void Log(size_t priority, LPCTSTR lpszText, ...)
	{
		TCHAR buf[1024*4] = {0};

#ifndef DEBUG_TEST
		int nLen = 0, nOffset = 0;
		if (log::LogHelper::GetInstance().log_->GetAddDateBefore())
		{
			SYSTEMTIME st = {0};
			GetLocalTime(&st);
			nOffset = _stprintf(buf, TEXT("%02d:%02d:%02d "), st.wHour, st.wMinute, st.wSecond);
			nLen += nOffset;
		}

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
#endif
	}
}